#include <stdio.h>

#include "backend/backend.h"
#include "gtest/gtest.h"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  // clear database files
  thdb::Clear();
  // Initialize database files
  thdb::Init();
  auto res = RUN_ALL_TESTS();
  // Close database
  thdb::Close();
  return res;
}
