#include "gtest/gtest.h"

#include "backend/backend.h"
#include "system/instance.h"

using namespace thdb;

TEST(Lab1, InstanceTest) {
    Instance * pDB = new Instance();
    
    delete pDB;
}