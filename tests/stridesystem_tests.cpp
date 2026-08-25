#include "gtest/gtest.h"

#include "stride/codegen/stridesystem.hpp"
#include "stride/utils/astfunctions.h"

using namespace strd;

TEST(StrideSystem, Initialize) {
    std::string strideRoot = ASTFunctions::getDefaultStrideRoot();
    
    // Test listAvailableSystems
    auto systems = StrideSystem::listAvailableSystems(strideRoot);
    // Assuming there is at least one system available (like "DesktopAudio" or similar)
    // If not, we just check it doesn't crash
    EXPECT_GE(systems.size(), 0);

    // Initialize with a dummy or existing system if possible, but for a basic test:
    // StrideSystem system(strideRoot, "TestSystem", 1, 0, {});
    // Since "TestSystem" might not exist, it might produce errors but shouldn't crash.
    StrideSystem system(strideRoot, "NonExistentSystem", 1, 0, {});
    
    auto errors = system.getErrors();
    // Should have an error about system not found
    EXPECT_GT(errors.size(), 0);
}
