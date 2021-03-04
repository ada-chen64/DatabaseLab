#include <iostream>

#include "backend/backend.h"
#include "gtest/gtest.h"
#include "sql_generator.h"
#include "system/instance.h"

namespace thdb {

TEST(Lab1, RandomSqlTest) {
  Instance *pDB = new Instance();
  std::vector<String> iSQLVec = SqlGenerator();
  for (const String &sql : iSQLVec) {
    std::cout << sql << std::endl;
  }
  for (const auto &sSQL : iSQLVec) {
    Execute(pDB, sSQL);
    printf("%s Finish\n", sSQL.c_str());
  }
  delete pDB;
}

}  // namespace thdb
