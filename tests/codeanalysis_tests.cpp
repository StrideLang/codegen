#include "gtest/gtest.h"

#include "stride/codegen/codeanalysis.hpp"
#include "stride/codegen/coderesolver.hpp"
#include "stride/utils/astfunctions.h"
#include "stride/utils/astquery.h"

using namespace strd;

TEST(CodeAnalysis, TypeTree) {
  auto strideroot = ASTFunctions::getDefaultStrideRoot();
  ASTNode tree;
  tree = AST::parseFile(TESTS_SOURCE_DIR "codeanalysis/typetree.stride");
  EXPECT_TRUE(tree != nullptr);
  ASTFunctions::preprocess(tree);

  CodeResolver resolver(tree, strideroot);
  resolver.process();

  auto typeTree = CodeAnalysis::getStateStructInformation(ScopeStack(), tree);

  ASSERT_EQ(typeTree.internal.size(), 4);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.internal[0].first), "Input");
  EXPECT_EQ(typeTree.internal[0].second, "_IntType");
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.internal[1].first), "Output");
  EXPECT_EQ(typeTree.internal[1].second, "_IntType");
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.internal[2].first), "ListInput");
  EXPECT_EQ(typeTree.internal[2].second, "_IntType");
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.internal[3].first), "ListOutput");
  EXPECT_EQ(typeTree.internal[3].second, "_IntType");

  ASSERT_EQ(typeTree.nodes.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[0].instance), "Mod");
  ASSERT_EQ(typeTree.nodes[0].external.size(), 2);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[0].external[0].first),
            "ModInput");
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[0].external[1].first),
            "ModOutput");
  ASSERT_EQ(typeTree.nodes[0].internal.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[0].internal[0].first),
            "ModInternal");
}

TEST(CodeAnalysis, TypeTreeModuleNested) {
  auto strideroot = ASTFunctions::getDefaultStrideRoot();
  ASTNode tree;
  tree = AST::parseFile(TESTS_SOURCE_DIR
                        "codeanalysis/typetree_module_nested.stride");
  EXPECT_TRUE(tree != nullptr);
  ASTFunctions::preprocess(tree);

  CodeResolver resolver(tree, strideroot);
  resolver.process();

  auto typeTree = CodeAnalysis::getStateStructInformation(ScopeStack(), tree);

  ASSERT_EQ(typeTree.internal.size(), 4);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.internal[0].first), "NestedInput");
  EXPECT_EQ(typeTree.internal[0].second, "_IntType");
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.internal[1].first), "NestedOutput");
  EXPECT_EQ(typeTree.internal[1].second, "_IntType");
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.internal[2].first), "Input");
  EXPECT_EQ(typeTree.internal[2].second, "_IntType");
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.internal[3].first), "Output");
  EXPECT_EQ(typeTree.internal[3].second, "_IntType");

  ASSERT_EQ(typeTree.nodes.size(), 4);

  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[0].instance), "NestingMod");
  ASSERT_EQ(typeTree.nodes[0].external.size(), 2);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[0].external[0].first),
            "NestingModInput");
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[0].external[1].first),
            "NestingModOutput");
  ASSERT_EQ(typeTree.nodes[0].internal.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[0].internal[0].first),
            "NestingModInternal");
  ASSERT_EQ(typeTree.nodes[0].nodes.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[0].nodes[0].instance),
            "NestedMod");
  ASSERT_EQ(typeTree.nodes[0].nodes[0].external.size(), 2);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[0].nodes[0].external[0].first),
            "NestedModInput");
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[0].nodes[0].external[1].first),
            "NestedModOutput");
  ASSERT_EQ(typeTree.nodes[0].nodes[0].internal.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[0].nodes[0].internal[0].first),
            "NestedModInternal");

  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[1].instance), "Mod");
  ASSERT_EQ(typeTree.nodes[1].external.size(), 2);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[1].external[0].first),
            "ModInput");
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[1].external[1].first),
            "ModOutput");
  ASSERT_EQ(typeTree.nodes[1].internal.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[1].internal[0].first),
            "ModInternal");

  ASSERT_EQ(typeTree.nodes[1].nodes.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[1].nodes[0].instance),
            "NestedReaction");
  ASSERT_EQ(typeTree.nodes[1].nodes[0].external.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[1].nodes[0].external[0].first),
            "NestedReactionOutput");
  ASSERT_EQ(typeTree.nodes[1].nodes[0].internal.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[1].nodes[0].internal[0].first),
            "NestedReactionInternal");

  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[2].instance), "ModWithLoop");
  ASSERT_EQ(typeTree.nodes[2].external.size(), 2);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[2].external[0].first),
            "ModInput");
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[2].external[1].first),
            "ModOutput");
  ASSERT_EQ(typeTree.nodes[2].internal.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[2].internal[0].first),
            "ModInternal");

  ASSERT_EQ(typeTree.nodes[2].nodes.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[2].nodes[0].instance),
            "NestedLoop");
  ASSERT_EQ(typeTree.nodes[2].nodes[0].external.size(), 2);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[2].nodes[0].external[0].first),
            "Input");
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[2].nodes[0].external[1].first),
            "Output");
  ASSERT_EQ(typeTree.nodes[2].nodes[0].internal.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[2].nodes[0].internal[0].first),
            "Done");
  ASSERT_EQ(typeTree.nodes[2].nodes[0].persistent.size(), 1);
  EXPECT_EQ(
      ASTQuery::getNodeName(typeTree.nodes[2].nodes[0].persistent[0].first),
      "Index");

  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[3].instance),
            "ModulePersistentVar");
  ASSERT_EQ(typeTree.nodes[3].external.size(), 2);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[3].external[0].first),
            "Output");
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[3].external[1].first),
            "Input");
  ASSERT_EQ(typeTree.nodes[3].persistent.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[3].persistent[0].first),
            "InternalPersistent");
}

TEST(CodeAnalysis, TypeTreePlatform) {
  auto strideroot = ASTFunctions::getDefaultStrideRoot();
  ASTNode tree;
  tree =
      AST::parseFile(TESTS_SOURCE_DIR "codeanalysis/typetree_platform.stride");
  EXPECT_TRUE(tree != nullptr);
  ASTFunctions::preprocess(tree);

  CodeResolver resolver(tree, strideroot);
  resolver.process();

  auto typeTree = CodeAnalysis::getStateStructInformation(ScopeStack(), tree);

  ASSERT_EQ(typeTree.internal.size(), 2);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.internal[0].first), "Input");
  EXPECT_EQ(typeTree.internal[0].second, "_RealType");
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.internal[1].first), "Output");
  EXPECT_EQ(typeTree.internal[1].second, "_SwitchType");

  ASSERT_EQ(typeTree.nodes.size(), 1);

  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[0].instance),
            "PlatformModule");
}

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
