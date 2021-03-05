#include "system/instance.h"

#include "backend/backend.h"
#include "gtest/gtest.h"

using namespace thdb;

TEST(Lab1, InstanceTest) {
  Instance *pDB = new Instance();
  delete pDB;
}
