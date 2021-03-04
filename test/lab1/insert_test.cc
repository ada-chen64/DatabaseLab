#include "backend/backend.h"
#include "gtest/gtest.h"
#include "system/instance.h"

namespace thdb {

TEST(Lab1, DISABLED_InsertTest) {
  Instance *pDB = new Instance();
  std::vector<String> iSQLVec = {
      "SHOW TABLES;",
      "CREATE TABLE A(A INT, B FLOAT, C VARCHAR(30), D FLOAT);",
      "SHOW TABLES;",
      "INSERT INTO A VALUES(4, 4.5, 'test_string', 5.0)",
      "SELECT * FROM A;",
      "SELECT * FROM A WHERE A.A = 4;",
      "SELECT * FROM A WHERE A.A < 4;",
      "SELECT * FROM A WHERE A.A > 4;",
      "SELECT * FROM A WHERE A.A <= 4;",
      "SELECT * FROM A WHERE A.A >= 4;",
      "SELECT * FROM A WHERE A.A <> 4;",
      "SELECT * FROM A WHERE A.A <> 3;",
      "SELECT * FROM A WHERE A.A < 5;",
      "SELECT * FROM A WHERE A.A > 3;",
      "INSERT INTO A VALUES(5, 4.5, 'test_string', 5.0)",
      "SELECT * FROM A;",
      "DELETE FROM A WHERE A.A = 4;",
      "SELECT * FROM A;",
      "DROP TABLE A;"};
  for (const auto &sSQL : iSQLVec) {
    Execute(pDB, sSQL);
    printf("%s Finish\n", sSQL.c_str());
  }
  delete pDB;
}

}  // namespace thdb
