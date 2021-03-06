#include "backend/backend.h"
#include "gtest/gtest.h"
#include "system/instance.h"
#include "utils/display.h"

namespace thdb {

TEST(Lab1, TableTest) {
  Instance *pDB = new Instance();
  std::vector<String> iSQLVec = {
      "CREATE TABLE Persons(ID INT, FirstName VARCHAR(20), LastName VARCHAR(20), Temperature FLOAT);",
      "SHOW TABLES;",        // NOLINT
      "DESC Persons;",       // NOLINT
      "DROP TABLE Persons;"  // NOLINT
  };
  std::vector<String> results = {"1\n", "Persons\n",
                                 "ID,Integer,4\nFirstName,String,20\nLastName,String,20\nTemperature,Float,8\n", "1\n"};
  for (uint32_t i = 0; i < iSQLVec.size(); i++) {
    std::vector<Result *> iResVec = Execute(pDB, iSQLVec[i]);
    EXPECT_EQ(iResVec.size(), 1);
    EXPECT_EQ(iResVec[0]->ToString(), results[i]);
  }
  delete pDB;
}

}  // namespace thdb
