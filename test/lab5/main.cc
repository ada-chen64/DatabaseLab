#include "backend/backend.h"
#include "gtest/gtest.h"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  thdb::Clear();
  auto res = RUN_ALL_TESTS();
  thdb::Close();
  return res;
}
