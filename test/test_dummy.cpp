#include <gtest/gtest.h>

// TEST(...)
// TEST_F(...)

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    // Always return zero-code and allow PlatformIO to parse results
    return 0;
}