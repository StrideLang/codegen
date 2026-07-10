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
  ASSERT_EQ(typeTree.nodes[0].input.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[0].input[0].first),
            "ModInput");
  ASSERT_EQ(typeTree.nodes[0].output.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[0].output[0].first),
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
  ASSERT_EQ(typeTree.nodes[0].input.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[0].input[0].first),
            "NestingModInput");
  ASSERT_EQ(typeTree.nodes[0].output.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[0].output[0].first),
            "NestingModOutput");
  ASSERT_EQ(typeTree.nodes[0].internal.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[0].internal[0].first),
            "NestingModInternal");
  ASSERT_EQ(typeTree.nodes[0].nodes.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[0].nodes[0].instance),
            "NestedMod");
  ASSERT_EQ(typeTree.nodes[0].nodes[0].input.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[0].nodes[0].input[0].first),
            "NestedModInput");
  ASSERT_EQ(typeTree.nodes[0].nodes[0].output.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[0].nodes[0].output[0].first),
            "NestedModOutput");
  ASSERT_EQ(typeTree.nodes[0].nodes[0].internal.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[0].nodes[0].internal[0].first),
            "NestedModInternal");

  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[1].instance), "Mod");
  ASSERT_EQ(typeTree.nodes[1].input.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[1].input[0].first),
            "ModInput");
  ASSERT_EQ(typeTree.nodes[1].output.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[1].output[0].first),
            "ModOutput");
  ASSERT_EQ(typeTree.nodes[1].internal.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[1].internal[0].first),
            "ModInternal");

  ASSERT_EQ(typeTree.nodes[1].nodes.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[1].nodes[0].instance),
            "NestedReaction");
  ASSERT_EQ(typeTree.nodes[1].nodes[0].output.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[1].nodes[0].output[0].first),
            "NestedReactionOutput");
  ASSERT_EQ(typeTree.nodes[1].nodes[0].internal.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[1].nodes[0].internal[0].first),
            "NestedReactionInternal");

  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[2].instance), "ModWithLoop");
  ASSERT_EQ(typeTree.nodes[2].input.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[2].input[0].first),
            "ModInput");
  ASSERT_EQ(typeTree.nodes[2].output.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[2].output[0].first),
            "ModOutput");
  ASSERT_EQ(typeTree.nodes[2].internal.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[2].internal[0].first),
            "ModInternal");

  ASSERT_EQ(typeTree.nodes[2].nodes.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[2].nodes[0].instance),
            "NestedLoop");
  ASSERT_EQ(typeTree.nodes[2].nodes[0].input.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[2].nodes[0].input[0].first),
            "Input");
  ASSERT_EQ(typeTree.nodes[2].nodes[0].output.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[2].nodes[0].output[0].first),
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
  ASSERT_EQ(typeTree.nodes[3].output.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[3].output[0].first), "Output");
  ASSERT_EQ(typeTree.nodes[3].input.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(typeTree.nodes[3].input[0].first), "Input");
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

TEST(CodeAnalysis, TypeTreeDomain) {
  auto strideroot = ASTFunctions::getDefaultStrideRoot();
  ASTNode tree;
  tree = AST::parseFile(TESTS_SOURCE_DIR "codeanalysis/typetree_domain.stride");
  EXPECT_TRUE(tree != nullptr);
  ASTFunctions::preprocess(tree);

  CodeResolver resolver(tree, strideroot);
  resolver.process();

  auto typeTree = CodeAnalysis::getStateStructInformation(ScopeStack(), tree);

  EXPECT_EQ(typeTree.nodes.size(), 1);
  auto *domainTreeNode = typeTree.getDomainRootTree("TestDomain");
  EXPECT_TRUE(domainTreeNode);
  EXPECT_EQ(ASTQuery::getNodeName(domainTreeNode->instance), "TestDomain");

  ASSERT_EQ(domainTreeNode->external.size(), 2);
  EXPECT_EQ(ASTQuery::getNodeName(domainTreeNode->external[0].first), "Out");
  EXPECT_EQ(domainTreeNode->external[0].second, "_RealType");
  EXPECT_EQ(ASTQuery::getNodeName(domainTreeNode->external[1].first), "In");
  EXPECT_EQ(domainTreeNode->external[1].second, "_RealType");

  ASSERT_EQ(domainTreeNode->internal.size(), 1);
  EXPECT_EQ(ASTQuery::getNodeName(domainTreeNode->internal[0].first), "Value");
  EXPECT_EQ(domainTreeNode->internal[0].second, "_RealType");
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

TEST(CodeAnalysis, GetParentTreeForNode) {
  auto strideroot = ASTFunctions::getDefaultStrideRoot();
  ASTNode tree;
  tree = AST::parseFile(TESTS_SOURCE_DIR
                        "codeanalysis/typetree_module_nested.stride");
  ASSERT_TRUE(tree != nullptr);
  ASTFunctions::preprocess(tree);

  CodeResolver resolver(tree, strideroot);
  resolver.process();

  auto typeTree = CodeAnalysis::getStateStructInformation(ScopeStack(), tree);

  // We should have at least 1 nested node in typeTree.nodes
  ASSERT_GE(typeTree.nodes.size(), 1);
  auto nestingModTree = &typeTree.nodes[0];
  EXPECT_EQ(ASTQuery::getNodeName(nestingModTree->instance), "NestingMod");

  ASSERT_GE(nestingModTree->nodes.size(), 1);
  auto nestedModTree = &nestingModTree->nodes[0];
  EXPECT_EQ(ASTQuery::getNodeName(nestedModTree->instance), "NestedMod");

  // Test 1: Get parent for a grandchild node (NestedMod)
  auto *parentOfNested = typeTree.getParentTreeForNode(nestedModTree->instance);
  ASSERT_NE(parentOfNested, nullptr);
  EXPECT_EQ(parentOfNested, nestingModTree);

  // Test 2: Get parent for a child node (NestingMod)
  auto *parentOfNesting = typeTree.getParentTreeForNode(nestingModTree->instance);
  ASSERT_NE(parentOfNesting, nullptr);
  EXPECT_EQ(parentOfNesting, &typeTree);

  // Test 3: Get parent for a node that is not in the tree
  auto dummyNode = std::make_shared<BlockNode>("DummyBlock", __FILE__, __LINE__);
  auto *parentOfDummy = typeTree.getParentTreeForNode(dummyNode);
  EXPECT_EQ(parentOfDummy, nullptr);

  // Test 4: Passing nullptr node
  auto *parentOfNull = typeTree.getParentTreeForNode(nullptr);
  EXPECT_EQ(parentOfNull, nullptr);
}
