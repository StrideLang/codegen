/*
    Stride is licensed under the terms of the GNU Affero General Public License version 3.

    Copyright (C) 2022-2026 Andres Cabrera
    All rights reserved.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    Based on BSD/MIT code by Andres Cabrera and Joseph Tilbian.

    Author: Andres Cabrera
*/

#include "gtest/gtest.h"

#include "stride/codegen/coderesolver.hpp"
#include "stride/parser/platformnode.h"
#include "stride/utils/astfunctions.h"
#include "stride/utils/astquery.h"
#include "stride/utils/astvalidation.h"
// #include "stride/codegen/stridelibrary.hpp"

#include <fstream>

using namespace strd;

std::string writeCodeToFile(std::string code) {
  std::string fileName = "temp.stride";
  std::ofstream output_file(fileName);

  EXPECT_TRUE(output_file.is_open());

  output_file << code;
  output_file.close();
  return fileName;
}

TEST(Resolver, ResolveBlockPersistence) {
  auto strideroot = ASTFunctions::getDefaultStrideRoot();
  ASTNode tree =
      AST::parseFile(TESTS_SOURCE_DIR "resolver/block_persistence.stride");
  ASSERT_TRUE(tree != nullptr);
  ASTFunctions::preprocess(tree);

  CodeResolver resolver(tree, strideroot);
  resolver.process();
  {
    auto modDecl = ASTQuery::findDeclarationByName("Test", ScopeStack(), tree);
    EXPECT_TRUE(modDecl);
    auto blocks = modDecl->getPropertyValue("blocks");

    auto decl =
        ASTQuery::findDeclarationByName("WrittenFirst", ScopeStack(), blocks);
    ASSERT_TRUE(decl);
    auto persistentProp = decl->getCompilerProperty("persistent");
    EXPECT_FALSE(persistentProp);
    decl = ASTQuery::findDeclarationByName("ReadFirst", ScopeStack(), blocks);
    ASSERT_TRUE(decl);
    persistentProp = decl->getCompilerProperty("persistent");
    ASSERT_TRUE(persistentProp);
    EXPECT_EQ(persistentProp->getNodeType(), AST::Switch);
    EXPECT_TRUE(
        std::static_pointer_cast<ValueNode>(persistentProp)->getSwitchValue());
  }
  // TODO resolve persistence recursively in function instances
  // https://github.com/StrideLang/codegen/issues/4
}

TEST(Resolver, ResolveTypes) {
  auto strideroot = ASTFunctions::getDefaultStrideRoot();
  ASTNode tree;
  tree = AST::parseFile(TESTS_SOURCE_DIR "resolver/resolve_types.stride");
  EXPECT_TRUE(tree != nullptr);
  ASTFunctions::preprocess(tree);

  CodeResolver resolver(tree, strideroot);
  resolver.process();

  // ForwardModuleIn >> PassThroughForward() >> ForwardModuleOut; # nothing
  // should be resolved here
  //     ForwardModuleInInt >> PassTPassThroughForwardhrough() >>
  //     ForwardModuleOutResolved; # forward resolve
  // ForwardModuleInNotResolved >>
  //     PassThroughForward() >> ForwardModuleOutInt; # reverse resolve, not
  //     resolved
  {
    auto decl =
        ASTQuery::findDeclarationByName("ForwardModuleOut", ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    EXPECT_TRUE(dataType);
    // Not resolved from connections: default from the type class
    ASSERT_EQ(dataType->getNodeType(), AST::Entity);
    EXPECT_EQ(std::static_pointer_cast<EntityNode>(dataType)->getName(),
              "_RealType");
  }
  {
    auto decl = ASTQuery::findDeclarationByName("ForwardModuleOutResolved",
                                                ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    EXPECT_TRUE(dataType);
    EXPECT_EQ(dataType->getNodeType(), AST::Entity);
    // FIXME
    // EXPECT_EQ(std::static_pointer_cast<EntityNode>(dataType)->getName(),
    //           "_IntType");
  }
  {
    auto decl = ASTQuery::findDeclarationByName("ForwardModuleInNotResolved",
                                                ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    EXPECT_TRUE(dataType);
    // Not resolved from connections: default from the type class
    ASSERT_EQ(dataType->getNodeType(), AST::Entity);
    EXPECT_EQ(std::static_pointer_cast<EntityNode>(dataType)->getName(),
              "_RealType");
  }
  // ModuleIn >> PassThrough() >> ModuleOut; # nothing should be resolved here
  // ModuleInInt >> PassThrough() >> ModuleOutNotResolved; # forward resolve,
  // not resolved ModuleInResolved >> PassThrough() >> ModuleOutInt; # reverse
  // resolve
  {
    auto decl =
        ASTQuery::findDeclarationByName("ModuleOut", ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    EXPECT_TRUE(dataType);
    // Not resolved from connections: default from the type class
    ASSERT_EQ(dataType->getNodeType(), AST::Entity);
    EXPECT_EQ(std::static_pointer_cast<EntityNode>(dataType)->getName(),
              "_RealType");
  }
  {
    auto decl = ASTQuery::findDeclarationByName("ModuleOutNotResolved",
                                                ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    EXPECT_TRUE(dataType);
    // Not resolved from connections: default from the type class
    ASSERT_EQ(dataType->getNodeType(), AST::Entity);
    EXPECT_EQ(std::static_pointer_cast<EntityNode>(dataType)->getName(),
              "_RealType");
  }
  {
    auto decl =
        ASTQuery::findDeclarationByName("ModuleInResolved", ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    EXPECT_TRUE(dataType);
    EXPECT_EQ(dataType->getNodeType(), AST::Entity);
    EXPECT_EQ(std::static_pointer_cast<EntityNode>(dataType)->getName(),
              "_IntType");
  }
  // ModuleInInt >> PassThroughWithType() >> ModuleWithTypeOutResolved;
  // ModuleWithTypeInResolved >> PassThroughWithType() >> ModuleOutInt;
  {
    auto decl = ASTQuery::findDeclarationByName("ModuleWithTypeOutResolved",
                                                ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    EXPECT_TRUE(dataType);
    EXPECT_EQ(dataType->getNodeType(), AST::Entity);
    EXPECT_EQ(std::static_pointer_cast<EntityNode>(dataType)->getName(),
              "_RealType");
  }
  {
    auto decl = ASTQuery::findDeclarationByName("ModuleWithTypeInResolved",
                                                ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    EXPECT_TRUE(dataType);
    EXPECT_EQ(dataType->getNodeType(), AST::Entity);
    EXPECT_EQ(std::static_pointer_cast<EntityNode>(dataType)->getName(),
              "_RealType");
  }
  // Forward resolution
  {
    auto decl = ASTQuery::findDeclarationByName("Output", ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    EXPECT_TRUE(dataType);
    EXPECT_EQ(dataType->getNodeType(), AST::Entity);
    EXPECT_EQ(std::static_pointer_cast<EntityNode>(dataType)->getName(),
              "_IntType");
  }
  {
    auto decl = ASTQuery::findDeclarationByName("Input", ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    EXPECT_TRUE(dataType);
    EXPECT_EQ(dataType->getNodeType(), AST::Entity);
    EXPECT_EQ(std::static_pointer_cast<EntityNode>(dataType)->getName(),
              "_IntType");
  }
  {
    auto decl = ASTQuery::findDeclarationByName("OutputB2", ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    EXPECT_TRUE(dataType);
    EXPECT_EQ(dataType->getNodeType(), AST::Entity);
    EXPECT_EQ(std::static_pointer_cast<EntityNode>(dataType)->getName(),
              "_IntType");
  }
  {
    auto decl = ASTQuery::findDeclarationByName("InputA2", ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    EXPECT_TRUE(dataType);
    EXPECT_EQ(dataType->getNodeType(), AST::Entity);
    EXPECT_EQ(std::static_pointer_cast<EntityNode>(dataType)->getName(),
              "_IntType");
  }
  // Bundles
  {
    auto decl =
        ASTQuery::findDeclarationByName("OutputBundle", ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    EXPECT_TRUE(dataType);
    EXPECT_EQ(dataType->getNodeType(), AST::Entity);
    EXPECT_EQ(std::static_pointer_cast<EntityNode>(dataType)->getName(),
              "_IntType");
  }
  {
    auto decl =
        ASTQuery::findDeclarationByName("InputBundle", ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    EXPECT_TRUE(dataType);
    EXPECT_EQ(dataType->getNodeType(), AST::Entity);
    EXPECT_EQ(std::static_pointer_cast<EntityNode>(dataType)->getName(),
              "_IntType");
  }
  {
    auto decl = ASTQuery::findDeclarationByName("OutputBundleMember",
                                                ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    EXPECT_TRUE(dataType);
    ASSERT_EQ(dataType->getNodeType(), AST::Entity);
    EXPECT_EQ(std::static_pointer_cast<EntityNode>(dataType)->getName(),
              "_IntType");
  }
  {
    auto decl = ASTQuery::findDeclarationByName("InputBundleMember",
                                                ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    EXPECT_TRUE(dataType);
    EXPECT_EQ(dataType->getNodeType(), AST::Entity);
    EXPECT_EQ(std::static_pointer_cast<EntityNode>(dataType)->getName(),
              "_IntType");
  }

  // ModuleIn >> PassThrough() >> ModuleOut; # nothing should be resolved here
  // ModuleInInt >> PassThrough() >> ModuleOutResolved; # forward resolve
  // ModuleInResolved >> PassThrough() >> ModuleOutInt; # reverse resolve
  {
    auto decl =
        ASTQuery::findDeclarationByName("ModuleOut", ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    EXPECT_TRUE(dataType);
    // Not resolved from connections: default from the type class
    ASSERT_EQ(dataType->getNodeType(), AST::Entity);
    EXPECT_EQ(std::static_pointer_cast<EntityNode>(dataType)->getName(),
              "_RealType");
  }
  {
    auto decl = ASTQuery::findDeclarationByName("ModuleIn", ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    EXPECT_TRUE(dataType);
    // Not resolved from connections: default from the type class
    ASSERT_EQ(dataType->getNodeType(), AST::Entity);
    EXPECT_EQ(std::static_pointer_cast<EntityNode>(dataType)->getName(),
              "_RealType");
  }
}

TEST(Resolver, TypeClassDefaults) {
  auto strideroot = ASTFunctions::getDefaultStrideRoot();
  ASTNode tree = AST::parseFile(TESTS_SOURCE_DIR "resolver/type_class.stride");
  ASSERT_TRUE(tree != nullptr);
  tree->addChild(
      std::make_shared<SystemNode>("JIT", 1, 0, __FILE__, __LINE__));
  ASTFunctions::preprocess(tree);

  CodeResolver resolver(tree, strideroot);
  resolver.process();

  // _Typed lists _NumericTypes in allowedTypes for its type property
  EXPECT_TRUE(ASTQuery::findTypeClass("_NumericTypes", tree));
  {
    // Default comes from the first member of the typeClass
    auto decl = ASTQuery::findDeclarationByName("Untyped", ScopeStack(), tree);
    ASSERT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    ASSERT_TRUE(dataType);
    ASSERT_EQ(dataType->getNodeType(), AST::Entity);
    EXPECT_EQ(ASTQuery::getNodeName(dataType), "_RealType");
  }
  {
    auto decl =
        ASTQuery::findDeclarationByName("IntSignal", ScopeStack(), tree);
    ASSERT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    ASSERT_TRUE(dataType);
    EXPECT_EQ(ASTQuery::getNodeName(dataType), "_IntType");
  }
  {
    // switch has a builtin data type and no type property
    auto decl = ASTQuery::findDeclarationByName("Flag", ScopeStack(), tree);
    ASSERT_TRUE(decl);
    EXPECT_FALSE(decl->getPropertyValue("type"));
  }
  // Undeclared blocks receiving _SwitchType are declared as switches
  for (const auto &name :
       {"AutoSwitch", "AutoSwitchFromFlag", "AutoSwitchFromPlatform"}) {
    auto decl = ASTQuery::findDeclarationByName(name, ScopeStack(), tree);
    ASSERT_TRUE(decl) << name;
    EXPECT_EQ(decl->getEntityType(), "switch") << name;
    EXPECT_FALSE(decl->getPropertyValue("type")) << name;
    auto defaultNode = decl->getPropertyValue("default");
    ASSERT_TRUE(defaultNode) << name;
    EXPECT_EQ(defaultNode->getNodeType(), AST::Switch) << name;
  }
}

TEST(Resolver, TypeClassRejectsNonMember) {
  auto strideroot = ASTFunctions::getDefaultStrideRoot();
  ASTNode tree = AST::parseFile(TESTS_SOURCE_DIR "resolver/type_class.stride");
  ASSERT_TRUE(tree != nullptr);
  tree->addChild(
      std::make_shared<SystemNode>("JIT", 1, 0, __FILE__, __LINE__));
  ASTFunctions::preprocess(tree);

  CodeResolver resolver(tree, strideroot);
  resolver.process();

  // Only "signal SwitchSignal { type: _SwitchType }" is invalid, as
  // _SwitchType is not a member of _NumericTypes
  auto errors = ASTValidation::validate(tree);
  int typeErrors = 0;
  int unresolvedErrors = 0;
  for (const auto &error : errors) {
    if (error.type == LangError::InvalidPortType &&
        error.errorTokens.size() == 4 && error.errorTokens[1] == "type") {
      EXPECT_EQ(error.errorTokens[0], "signal");
      EXPECT_NE(error.errorTokens[3].find("_RealType"), std::string::npos);
      typeErrors++;
    } else if (error.type == LangError::UnresolvedType) {
      // FromOutput takes its output type from its output port
      ASSERT_EQ(error.errorTokens.size(), 2);
      EXPECT_EQ(error.errorTokens[0], "Unresolvable");
      EXPECT_EQ(error.errorTokens[1], "FromOutput");
      unresolvedErrors++;
    }
  }
  EXPECT_EQ(typeErrors, 1);
  EXPECT_EQ(unresolvedErrors, 1);
}

TEST(Resolver, ReadOnlyPortAttributes) {
  auto strideroot = ASTFunctions::getDefaultStrideRoot();
  ASTNode tree =
      AST::parseFile(TESTS_SOURCE_DIR "resolver/read_only_port.stride");
  ASSERT_TRUE(tree != nullptr);
  tree->addChild(
      std::make_shared<SystemNode>("JIT", 1, 0, __FILE__, __LINE__));
  ASTFunctions::preprocess(tree);

  CodeResolver resolver(tree, strideroot);
  resolver.process();

  // Only the type written on InputPort is an error. The port attributes
  // filled in by the compiler on both ports are not.
  auto errors = ASTValidation::validate(tree);
  int readOnlyErrors = 0;
  for (const auto &error : errors) {
    if (error.type == LangError::ReadOnlyPort) {
      ASSERT_EQ(error.errorTokens.size(), 2);
      EXPECT_EQ(error.errorTokens[0], "mainInputPort");
      EXPECT_EQ(error.errorTokens[1], "type");
      readOnlyErrors++;
    }
  }
  EXPECT_EQ(readOnlyErrors, 1);
}

TEST(Resolver, SelfPort) {
  auto strideroot = ASTFunctions::getDefaultStrideRoot();
  ASTNode tree;
  tree = AST::parseFile(TESTS_SOURCE_DIR "resolver/01_self_port.stride");
  EXPECT_TRUE(tree != nullptr);
  ASTFunctions::preprocess(tree);

  CodeResolver resolver(tree, strideroot);
  resolver.process();

  auto decl = ASTQuery::findDeclarationByName("TypeTest", ScopeStack(), tree);
  EXPECT_TRUE(decl);
  auto filledValue = decl->getPropertyValue("values");
  EXPECT_TRUE(filledValue);
  EXPECT_EQ(filledValue->getNodeType(), AST::List);
  EXPECT_EQ(filledValue->getChildren().size(), 9);
}

TEST(Resolver, SynthesizeBundleDeclarationMember) {
  auto strideroot = ASTFunctions::getDefaultStrideRoot();
  ASTNode tree;
  tree = AST::parseFile(TESTS_SOURCE_DIR "resolver/01_self_port.stride");
  EXPECT_TRUE(tree != nullptr);
  ASTFunctions::preprocess(tree);

  CodeResolver resolver(tree, strideroot);
  resolver.process();

  auto decl = ASTQuery::findDeclarationByName("TypeTest", ScopeStack(), tree);
  EXPECT_TRUE(decl);
  auto elementDecl =
      ASTQuery::synthesizeArrayDeclarationElement(decl, 3, ScopeStack(), tree);
  EXPECT_TRUE(elementDecl);
  EXPECT_EQ(elementDecl->getNodeType(), AST::Declaration);
  auto elementValue = elementDecl->getPropertyValue("values");
  EXPECT_TRUE(elementValue);
  EXPECT_EQ(elementValue->getNodeType(), AST::Int);
  EXPECT_EQ(std::static_pointer_cast<ValueNode>(elementValue)->getIntValue(),
            3);
}

TEST(Resolver, SynthesizeBundleDeclarationMemberNested) {
  auto strideroot = ASTFunctions::getDefaultStrideRoot();
  ASTNode tree;
  tree = AST::parseFile(TESTS_SOURCE_DIR "resolver/01_self_port.stride");
  EXPECT_TRUE(tree != nullptr);
  ASTFunctions::preprocess(tree);

  CodeResolver resolver(tree, strideroot);
  resolver.process();

  auto decl =
      ASTQuery::findDeclarationByName("NestedTypeTest", ScopeStack(), tree);
  EXPECT_TRUE(decl);
  auto elementDecl =
      ASTQuery::synthesizeArrayDeclarationElement(decl, 3, ScopeStack(), tree);
  ASSERT_TRUE(elementDecl);
  EXPECT_EQ(elementDecl->getNodeType(), AST::Declaration);
  auto elementValue = elementDecl->getPropertyValue("values");
  // EXPECT_TRUE(elementValue);
  // EXPECT_EQ(elementValue->getNodeType(), AST::Int);
  // EXPECT_EQ(std::static_pointer_cast<ValueNode>(elementValue)->getIntValue(),
  //           3);
}
