#include "gtest/gtest.h"

#include "stride/codegen/codeanalysis.hpp"
#include "stride/codegen/coderesolver.hpp"
#include "stride/utils/astfunctions.h"
#include "stride/utils/astquery.h"

using namespace strd;

TEST(CodeAnalysis, EvaluateBundleNumOutputs) {
  auto strideroot = ASTFunctions::getDefaultStrideRoot();
  ASTNode tree =
      AST::parseFile(TESTS_SOURCE_DIR "resolver/resolve_types.stride");
  ASSERT_TRUE(tree != nullptr);
  ASTFunctions::preprocess(tree);

  CodeResolver resolver(tree, strideroot);
  resolver.process();

  // Pick a bundle declaration: signal IntInputBundle[2]
  auto decl =
      ASTQuery::findDeclarationByName("IntInputBundle", ScopeStack(), tree);
  ASSERT_TRUE(decl);

  std::vector<LangError> errors;
  int bundleSize =
      CodeAnalysis::getNodeNumOutputs(decl, ScopeStack(), tree, &errors);
  EXPECT_EQ(bundleSize, 2);
}

TEST(CodeAnalysis, EvaluateSizePortProperty) {
  auto strideroot = ASTFunctions::getDefaultStrideRoot();
  ASTNode tree =
      AST::parseFile(TESTS_SOURCE_DIR "resolver/compiler_properties.stride");
  ASSERT_TRUE(tree != nullptr);
  ASTFunctions::preprocess(tree);

  CodeResolver resolver(tree, strideroot);
  resolver.process();

  // Find a module definition with ports
  bool checked = false;
  for (const auto &node : tree->getChildren()) {
    if (node->getLine() == 23) {
      EXPECT_EQ(node->getNodeType(), AST::Stream);
      auto stream = std::static_pointer_cast<StreamNode>(node);
      auto funcNode = stream->getRight()->getChildren()[0];
      EXPECT_TRUE(funcNode);
      EXPECT_EQ(funcNode->getNodeType(), AST::Function);
      auto functionNode = std::static_pointer_cast<FunctionNode>(funcNode);

      std::string framework;
      auto funcDecl = ASTQuery::findDeclarationByName(
          functionNode->getName(), ScopeStack(), tree,
          functionNode->getNamespaceList(), framework);

      if (funcDecl) {
        int size = CodeAnalysis::evaluateSizePortProperty(
            "InputPort", ScopeStack(), funcDecl, functionNode, tree);
        EXPECT_GE(size, -2);
        checked = true;
      }
    }
  }
  EXPECT_TRUE(checked);
}
