#include "gtest/gtest.h"

#include "stride/codegen/astfunctions.hpp"
#include "stride/codegen/coderesolver.hpp"
// #include "stride/codegen/astquery.hpp"
// #include "stride/codegen/stridelibrary.hpp"

TEST(Resolver, ImportRaw) {
  auto strideroot = ASTFunctions::getDefaultStrideRoot();
  ASTNode tree;
  tree = AST::parseFile(TESTS_SOURCE_DIR "resolver/01_self_port.stride");
  EXPECT_TRUE(tree != nullptr);
  ASTFunctions::preprocess(tree);

  CodeResolver resolver(tree, strideroot);
  resolver.process();
}
