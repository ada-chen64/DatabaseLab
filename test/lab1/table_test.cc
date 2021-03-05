#include "backend/backend.h"
#include "gtest/gtest.h"
#include "system/instance.h"

namespace thdb {

TEST(Lab1, TableTest) {
  Instance *pDB = new Instance();
  std::vector<String> iSQLVec = {
      "SHOW TABLES;", "CREATE TABLE A(A INT, B FLOAT, C VARCHAR(30), D FLOAT);",
      "SHOW TABLES;", "DROP TABLE A;"};
  for (const auto &sSQL : iSQLVec) {
    Execute(pDB, sSQL);
    printf("%s Finish\n", sSQL.c_str());
  }
  delete pDB;
}

}  // namespace thdb
