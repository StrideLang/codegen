#include "gtest/gtest.h"

#include "stride/codegen/coderesolver.hpp"
#include "stride/codegen/codevalidator.hpp"
#include "stride/utils/astfunctions.h"

using namespace strd;

TEST(CodeValidator, ValidTree) {
  auto strideroot = ASTFunctions::getDefaultStrideRoot();
  ASTNode tree =
      AST::parseFile(TESTS_SOURCE_DIR "resolver/block_persistence.stride");
  ASSERT_TRUE(tree != nullptr);
  ASTFunctions::preprocess(tree);

  CodeResolver resolver(tree, strideroot);
  resolver.process();

  CodeValidator validator(tree);
  EXPECT_TRUE(validator.isValid());
  EXPECT_EQ(validator.getErrors().size(), 0);
}

TEST(CodeValidator, InvalidStreamSizes) {
  auto strideroot = ASTFunctions::getDefaultStrideRoot();
  ASTNode tree =
      AST::parseFile(TESTS_SOURCE_DIR "validator/invalid_sizes.stride");
  ASSERT_TRUE(tree != nullptr);
  ASTFunctions::preprocess(tree);

  CodeResolver resolver(tree, strideroot);
  resolver.process();

  CodeValidator validator(tree);
  EXPECT_FALSE(validator.isValid());

  auto errors = validator.getErrors();
  EXPECT_GT(errors.size(), 0);
  // Look for stream size mismatch error
  bool foundSizeError = false;
  for (const auto &err : errors) {
    if (err.type == LangError::StreamMemberSizeMismatch) {
      foundSizeError = true;
      break;
    }
  }
  EXPECT_TRUE(foundSizeError);
}

TEST(CodeValidator, InvalidBundleAccess) {
  auto strideroot = ASTFunctions::getDefaultStrideRoot();
  ASTNode tree =
      AST::parseFile(TESTS_SOURCE_DIR "validator/invalid_bundle.stride");
  ASSERT_TRUE(tree != nullptr);
  ASTFunctions::preprocess(tree);

  CodeResolver resolver(tree, strideroot);
  resolver.process();

  CodeValidator validator(tree);
  // TODO implement validation of statically sized bundles
  // EXPECT_FALSE(validator.isValid());

  // auto errors = validator.getErrors();
  // EXPECT_GT(errors.size(), 0);

  // bool foundIndexError = false;
  // for (const auto &err : errors) {
  //   if (err.type == LangError::ArrayIndexOutOfBounds) {
  //     foundIndexError = true;
  //     break;
  //   }
  // }
  // EXPECT_TRUE(foundIndexError);
}
