#include "gtest/gtest.h"

#include "stride/codegen/coderesolver.hpp"
#include "stride/utils/astfunctions.h"
#include "stride/utils/astquery.h"
// #include "stride/codegen/stridelibrary.hpp"

using namespace strd;

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
  EXPECT_TRUE(elementDecl);
  EXPECT_EQ(elementDecl->getNodeType(), AST::Declaration);
  auto elementValue = elementDecl->getPropertyValue("values");
  // EXPECT_TRUE(elementValue);
  // EXPECT_EQ(elementValue->getNodeType(), AST::Int);
  // EXPECT_EQ(std::static_pointer_cast<ValueNode>(elementValue)->getIntValue(),
  //           3);
}
