#include "backend/backend.h"
#include "gtest/gtest.h"
#include "system/instance.h"
#include "utils/display.h"

namespace thdb {

TEST(Lab1, TableTest) {
  Instance *pDB = new Instance();
  std::vector<String> iSQLVec = {
      "CREATE TABLE Persons(ID INT, FirstName VARCHAR(20), LastName VARCHAR(20), Temperature FLOAT);",
      "SHOW TABLES;",   // NOLINT
      "DESC Persons;",  // NOLINT
      "DROP TABLE A;"   // NOLINT
  };
  for (const auto &sSQL : iSQLVec) {
    std::vector<Result *> results = Execute(pDB, sSQL);
    PrintTable(results);
  }
  delete pDB;
}

}  // namespace thdb
