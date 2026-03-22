#include "gtest/gtest.h"

#include "stride/codegen/coderesolver.hpp"
#include "stride/utils/astfunctions.h"
#include "stride/utils/astquery.h"
// #include "stride/codegen/stridelibrary.hpp"

using namespace strd;

TEST(CompilerProperties, ModuleInputPortBLock) {
  auto strideroot = ASTFunctions::getDefaultStrideRoot();
  ASTNode tree;
  tree = AST::parseFile(TESTS_SOURCE_DIR "resolver/compiler_properties.stride");
  EXPECT_TRUE(tree != nullptr);
  ASTFunctions::preprocess(tree);

  CodeResolver resolver(tree, strideroot);
  resolver.process();

  for (const auto &node : tree->getChildren()) {
    if (node->getLine() == 23) {
      EXPECT_EQ(node->getNodeType(), AST::Stream);
      auto stream = std::static_pointer_cast<StreamNode>(node);
      auto funcNode = stream->getRight()->getChildren()[0];
      EXPECT_TRUE(funcNode);
      EXPECT_EQ(funcNode->getNodeType(), AST::Function);
      auto inputBlock = funcNode->getCompilerProperty("inputBlock");
      EXPECT_TRUE(inputBlock);
      EXPECT_EQ(inputBlock->getNodeType(), AST::Block);
      EXPECT_EQ(std::static_pointer_cast<BlockNode>(inputBlock)->getName(),
                "OuterInput");
      auto outputBlock = funcNode->getCompilerProperty("outputBlock");
      EXPECT_TRUE(outputBlock);
      EXPECT_EQ(outputBlock->getNodeType(), AST::Block);
      EXPECT_EQ(std::static_pointer_cast<BlockNode>(outputBlock)->getName(),
                "OuterOutput");
    }
  }
}
