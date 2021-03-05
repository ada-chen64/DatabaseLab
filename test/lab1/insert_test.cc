#include <iostream>

#include "backend/backend.h"
#include "gtest/gtest.h"
#include "result/results.h"
#include "system/instance.h"

namespace thdb {

TEST(Lab1, InsertTest) {
  Instance *pDB = new Instance();
  std::vector<String> iSQLVec = {
      "SHOW TABLES;",
      "CREATE TABLE A(A INT, Bsdfdsfdsfs FLOAT, C VARCHAR(30), D FLOAT);",
      "SHOW TABLES;",
      "INSERT INTO A VALUES(4, 4.5, 'test_string', 5.0)",
      "INSERT INTO A VALUES(5, 4.5, 'test', 5.0)",
      "INSERT INTO A VALUES(6, 4.5, 'string', 5.0)",
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
    std::vector<Result *> results = Execute(pDB, sSQL);
    for (auto result : results) {
      result->Display();
      delete result;
    }
    // printf("%s Finish\n", sSQL.c_str());
  }
  delete pDB;
}

}  // namespace thdb
