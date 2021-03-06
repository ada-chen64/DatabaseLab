#include "backend/backend.h"
#include "gtest/gtest.h"
#include "system/instance.h"
#include "utils/display.h"

namespace thdb {

TEST(Lab1, TableTest) {
  Instance *pDB = new Instance();
  std::vector<String> iSQLVec = {
      "SHOW TABLES;",
      "CREATE TABLE TEST(A INT, B FLOAT, C VARCHAR(30), D FLOAT);",
      "SHOW TABLES;", "DROP TABLE A;"};
  for (const auto &sSQL : iSQLVec) {
    std::vector<Result *> results = Execute(pDB, sSQL);
    PrintTable(results);
  }
  delete pDB;
}

}  // namespace thdb
