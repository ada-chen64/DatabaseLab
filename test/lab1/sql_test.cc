#include "backend/backend.h"
#include "gtest/gtest.h"
#include "system/instance.h"
#include "utils/display.h"

namespace thdb {

TEST(Lab1, SimpleSqlTest) {
  Instance *pDB = new Instance();
  std::vector<String> iSQLVec = {
      "CREATE TABLE TEST(A INT, B FLOAT, C VARCHAR(30), D FLOAT);",
      "INSERT INTO TEST VALUES(1, 2.5, 'zxcasdweq', 3.4);",
      "SELECT * FROM TEST;", "DROP TABLE TEST;"};
  for (const auto &sSQL : iSQLVec) {
    std::vector<Result *> iResVec = Execute(pDB, sSQL);
    PrintTable(iResVec);
  }
  delete pDB;
}

}  // namespace thdb
