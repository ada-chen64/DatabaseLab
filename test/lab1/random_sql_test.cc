#include "backend/backend.h"
#include "gtest/gtest.h"
#include "sql_generator.h"
#include "system/instance.h"
#include "utils/display.h"

namespace thdb {

TEST(Lab1, RandomSqlTest) {
  Instance *pDB = new Instance();
  std::vector<String> iSQLVec = SqlGenerator();
  for (const auto &sSQL : iSQLVec) {
    auto results = Execute(pDB, sSQL);
    PrintTable(results);
  }
  delete pDB;
}

}  // namespace thdb
