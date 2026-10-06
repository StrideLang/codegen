#include "gtest/gtest.h"

#include "stride/codegen/stridesystem.hpp"
#include "stride/utils/astfunctions.h"

using namespace strd;

TEST(StrideSystem, Initialize) {
    std::string strideRoot = ASTFunctions::getDefaultStrideRoot();
    
    // Test listAvailableSystems
    auto systems = StrideSystem::listAvailableSystems(strideRoot);
    EXPECT_GE(systems.size(), 0);

    // Non existent system should record error
    StrideSystem system(strideRoot, "NonExistentSystem", 1, 0, {});
    
    auto errors = system.getErrors();
    EXPECT_GT(errors.size(), 0);
}

TEST(StrideSystem, EmptySystemName) {
    std::string strideRoot = ASTFunctions::getDefaultStrideRoot();
    StrideSystem system(strideRoot, "", -1, -1, {});
    EXPECT_EQ(system.systemName(), "");
    EXPECT_EQ(system.getErrors().size(), 0);
}

TEST(StrideSystem, SafeOperationsOnEmpty) {
    std::string strideRoot = ASTFunctions::getDefaultStrideRoot();
    StrideSystem system(strideRoot, "", -1, -1, {});
    
    EXPECT_NO_THROW({
        auto imports = system.listAvailableImports();
        auto errors = system.getErrors();
        auto warnings = system.getWarnings();
        auto names = system.getFrameworkNames();
    });
}
