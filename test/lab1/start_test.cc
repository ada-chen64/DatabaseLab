#include "gtest/gtest.h"

namespace thdb {

TEST(Lab1, StartTest) {
  // 测试 googletest 是否正常
  int a = 1;
  EXPECT_EQ(a, 1);
}

}  // namespace thdb
