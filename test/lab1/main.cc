#include <stdio.h>

#include "backend/backend.h"
#include "gtest/gtest.h"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  thdb::Clear();
  thdb::Init();
  auto res = RUN_ALL_TESTS();
  printf("Write Back Begin;\n");
  thdb::Close();
  printf("Write Back End;\n");
  return res;
}
