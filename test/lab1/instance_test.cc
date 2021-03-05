#include "system/instance.h"

#include "backend/backend.h"
#include "gtest/gtest.h"

using namespace thdb;

TEST(Lab1, InstanceTest) {
  Instance *pDB = new Instance();
  printf("Instance Test\n");
  for (const auto &it : pDB->GetTableList()) printf("%s\n", it.c_str());
  delete pDB;
}
