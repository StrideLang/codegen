#include "gtest/gtest.h"

#include "stride/codegen/coderesolver.hpp"
#include "stride/utils/astfunctions.h"
#include "stride/utils/astquery.h"
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
  ASTNode tree;

  auto fileName = writeCodeToFile(R"(
Noop >> WrittenFirst;

WrittenFirst >> Noop;

ReadFirst >> Noop;
Noop >> ReadFirst;
)");

  tree = AST::parseFile(fileName.c_str());
  ASSERT_TRUE(tree != nullptr);
  ASTFunctions::preprocess(tree);

  CodeResolver resolver(tree, strideroot);
  resolver.process();
  {
    auto decl =
        ASTQuery::findDeclarationByName("WrittenFirst", ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto persistentProp = decl->getCompilerProperty("persistent");
    EXPECT_FALSE(persistentProp);
  }
  {
    auto decl =
        ASTQuery::findDeclarationByName("ReadFirst", ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto persistentProp = decl->getCompilerProperty("persistent");
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
    EXPECT_EQ(dataType->getNodeType(), AST::None);
  }
  {
    auto decl = ASTQuery::findDeclarationByName("ForwardModuleOutResolved",
                                                ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    EXPECT_TRUE(dataType);
    EXPECT_EQ(dataType->getNodeType(), AST::Block);
    // FIXME
    // EXPECT_EQ(std::static_pointer_cast<BlockNode>(dataType)->getName(),
    //           "_IntType");
  }
  {
    auto decl = ASTQuery::findDeclarationByName("ForwardModuleInNotResolved",
                                                ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    EXPECT_TRUE(dataType);
    EXPECT_EQ(dataType->getNodeType(), AST::None);
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
    EXPECT_EQ(dataType->getNodeType(), AST::None);
  }
  {
    auto decl = ASTQuery::findDeclarationByName("ModuleOutNotResolved",
                                                ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    EXPECT_TRUE(dataType);
    EXPECT_EQ(dataType->getNodeType(), AST::None);
  }
  {
    auto decl =
        ASTQuery::findDeclarationByName("ModuleInResolved", ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    EXPECT_TRUE(dataType);
    EXPECT_EQ(dataType->getNodeType(), AST::Block);
    EXPECT_EQ(std::static_pointer_cast<BlockNode>(dataType)->getName(),
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
    EXPECT_EQ(dataType->getNodeType(), AST::Block);
    EXPECT_EQ(std::static_pointer_cast<BlockNode>(dataType)->getName(),
              "_RealType");
  }
  {
    auto decl = ASTQuery::findDeclarationByName("ModuleWithTypeInResolved",
                                                ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    EXPECT_TRUE(dataType);
    EXPECT_EQ(dataType->getNodeType(), AST::Block);
    EXPECT_EQ(std::static_pointer_cast<BlockNode>(dataType)->getName(),
              "_RealType");
  }
  // Forward resolution
  {
    auto decl = ASTQuery::findDeclarationByName("Output", ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    EXPECT_TRUE(dataType);
    EXPECT_EQ(dataType->getNodeType(), AST::Block);
    EXPECT_EQ(std::static_pointer_cast<BlockNode>(dataType)->getName(),
              "_IntType");
  }
  {
    auto decl = ASTQuery::findDeclarationByName("Input", ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    EXPECT_TRUE(dataType);
    EXPECT_EQ(dataType->getNodeType(), AST::Block);
    EXPECT_EQ(std::static_pointer_cast<BlockNode>(dataType)->getName(),
              "_IntType");
  }
  {
    auto decl = ASTQuery::findDeclarationByName("OutputB2", ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    EXPECT_TRUE(dataType);
    EXPECT_EQ(dataType->getNodeType(), AST::Block);
    EXPECT_EQ(std::static_pointer_cast<BlockNode>(dataType)->getName(),
              "_IntType");
  }
  {
    auto decl = ASTQuery::findDeclarationByName("InputA2", ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    EXPECT_TRUE(dataType);
    EXPECT_EQ(dataType->getNodeType(), AST::Block);
    EXPECT_EQ(std::static_pointer_cast<BlockNode>(dataType)->getName(),
              "_IntType");
  }
  // Bundles
  {
    auto decl =
        ASTQuery::findDeclarationByName("OutputBundle", ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    EXPECT_TRUE(dataType);
    EXPECT_EQ(dataType->getNodeType(), AST::Block);
    EXPECT_EQ(std::static_pointer_cast<BlockNode>(dataType)->getName(),
              "_IntType");
  }
  {
    auto decl =
        ASTQuery::findDeclarationByName("InputBundle", ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    EXPECT_TRUE(dataType);
    EXPECT_EQ(dataType->getNodeType(), AST::Block);
    EXPECT_EQ(std::static_pointer_cast<BlockNode>(dataType)->getName(),
              "_IntType");
  }
  {
    auto decl = ASTQuery::findDeclarationByName("OutputBundleMember",
                                                ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    EXPECT_TRUE(dataType);
    ASSERT_EQ(dataType->getNodeType(), AST::Block);
    EXPECT_EQ(std::static_pointer_cast<BlockNode>(dataType)->getName(),
              "_IntType");
  }
  {
    auto decl = ASTQuery::findDeclarationByName("InputBundleMember",
                                                ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    EXPECT_TRUE(dataType);
    EXPECT_EQ(dataType->getNodeType(), AST::Block);
    EXPECT_EQ(std::static_pointer_cast<BlockNode>(dataType)->getName(),
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
    EXPECT_EQ(dataType->getNodeType(), AST::None);
  }
  {
    auto decl = ASTQuery::findDeclarationByName("ModuleIn", ScopeStack(), tree);
    EXPECT_TRUE(decl);
    auto dataType = decl->getPropertyValue("type");
    EXPECT_TRUE(dataType);
    EXPECT_EQ(dataType->getNodeType(), AST::None);
  }
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
      ASTQuery::synthesizeBundleDeclarationElement(decl, 3, ScopeStack(), tree);
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
      ASTQuery::synthesizeBundleDeclarationElement(decl, 3, ScopeStack(), tree);
  ASSERT_TRUE(elementDecl);
  EXPECT_EQ(elementDecl->getNodeType(), AST::Declaration);
  auto elementValue = elementDecl->getPropertyValue("values");
  // EXPECT_TRUE(elementValue);
  // EXPECT_EQ(elementValue->getNodeType(), AST::Int);
  // EXPECT_EQ(std::static_pointer_cast<ValueNode>(elementValue)->getIntValue(),
  //           3);
}
