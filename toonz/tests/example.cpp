
#include "gtest/gtest.h"

TEST (StupidExampleTest, Addition) {
    const int value = 2 + 2;
    ASSERT_EQ(4, value) << "Should be 4!";
}
