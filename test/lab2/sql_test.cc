#include "backend/backend.h"
#include "gtest/gtest.h"
#include "system/instance.h"
#include "utils/display.h"

namespace thdb {

TEST(Lab2, InsertSelectTest) {
  Instance *pDB = new Instance();
  std::vector<String> iSQLVec = {
      "CREATE TABLE Persons(ID INT, FirstName VARCHAR(20), LastName VARCHAR(20), Temperature FLOAT);",  // NOLINT
      "ALTER TABLE Persons ADD INDEX(ID);",                                                             // NOLINT
      "ALTER TABLE Persons ADD INDEX(Temperature);",                                                    // NOLINT
      "INSERT INTO Persons VALUES(1, 'James', 'Smith', 36.4);",                                         // NOLINT
      "SELECT * FROM Persons WHERE Persons.ID = 1;",                                                    // NOLINT
      "SELECT * FROM Persons WHERE Persons.Temperature < 37;",                                          // NOLINT
      "DROP TABLE Persons;"                                                                             // NOLINT
  };
  // 存放 sql 执行结果
  std::vector<String> results = {
      "1\n",                   // NOLINT
      "1\n",                   // NOLINT
      "1\n",                   // NOLINT
      "1\n",                   // NOLINT
      "1,James,Smith,36.4\n",  // NOLINT
      "1,James,Smith,36.4\n",  // NOLINT
      "1\n"                    // NOLINT
  };
  // 执行每条 sql 语句
  for (uint32_t i = 0; i < iSQLVec.size(); i++) {
    std::vector<Result *> iResVec = Execute(pDB, iSQLVec[i]);
    // 由于测试 sql 每行只有一条 sql 语句，因此 iResVec 的大小始终为 1.
    EXPECT_EQ(iResVec.size(), 1);
    // 比较 sql 执行结果是否一致
    EXPECT_EQ(iResVec[0]->ToString(), results[i]);
  }
  delete pDB;
}

TEST(Lab2, UpdateTest) {
  Instance *pDB = new Instance();
  std::vector<String> iSQLVec = {
      "CREATE TABLE Persons(ID INT, FirstName VARCHAR(20), LastName VARCHAR(20), Temperature FLOAT);",  // NOLINT
      "ALTER TABLE Persons ADD INDEX(ID);",                                                             // NOLINT
      "ALTER TABLE Persons ADD INDEX(Temperature);",                                                    // NOLINT
      "INSERT INTO Persons VALUES(1, 'James', 'Smith', 36.4);",                                         // NOLINT
      "INSERT INTO Persons VALUES(2, 'Maria', 'Garcia', 36.7);",                                        // NOLINT
      "SELECT * FROM Persons WHERE Persons.ID = 1;",                                                    // NOLINT
      "UPDATE Persons SET Temperature = 35.4 WHERE Persons.ID = 2;",                                    // NOLINT
      "SELECT * FROM Persons WHERE Persons.ID = 2;",                                                    // NOLINT
      "SELECT * FROM Persons WHERE Persons.Temperature <= 36;",                                         // NOLINT
      "DROP TABLE Persons;"                                                                             // NOLINT
  };
  std::vector<String> results = {
      "1\n",                    // NOLINT
      "1\n",                    // NOLINT
      "1\n",                    // NOLINT
      "1\n",                    // NOLINT
      "1\n",                    // NOLINT
      "1,James,Smith,36.4\n",   // NOLINT
      "1\n",                    // NOLINT
      "2,Maria,Garcia,35.4\n",  // NOLINT
      "2,Maria,Garcia,35.4\n",  // NOLINT
      "1\n"                     // NOLINT
  };
  for (uint32_t i = 0; i < iSQLVec.size(); i++) {
    std::vector<Result *> iResVec = Execute(pDB, iSQLVec[i]);
    EXPECT_EQ(iResVec.size(), 1);
    EXPECT_EQ(iResVec[0]->ToString(), results[i]);
  }
  delete pDB;
}

TEST(Lab2, DeleteTest) {
  Instance *pDB = new Instance();
  std::vector<String> iSQLVec = {
      "CREATE TABLE Persons(ID INT, FirstName VARCHAR(20), LastName VARCHAR(20), Temperature FLOAT);",  // NOLINT
      "ALTER TABLE Persons ADD INDEX(ID);",                                                             // NOLINT
      "ALTER TABLE Persons ADD INDEX(Temperature);",                                                    // NOLINT
      "INSERT INTO Persons VALUES(1, 'James', 'Smith', 36.4);",                                         // NOLINT
      "INSERT INTO Persons VALUES(2, 'Maria', 'Garcia', 36.7);",                                        // NOLINT
      "DELETE FROM Persons WHERE Persons.ID = 1;",                                                      // NOLINT
      "SELECT * FROM Persons WHERE Persons.ID = 1;",                                                    // NOLINT
      "SELECT * FROM Persons WHERE Persons.ID = 2;",                                                    // NOLINT
      "SELECT * FROM Persons WHERE Persons.Temperature >= 36 AND Persons.Temperature < 37;",            // NOLINT
      "DROP TABLE Persons;"                                                                             // NOLINT
  };
  std::vector<String> results = {
      "1\n",                    // NOLINT
      "1\n",                    // NOLINT
      "1\n",                    // NOLINT
      "1\n",                    // NOLINT
      "1\n",                    // NOLINT
      "1\n",                    // NOLINT
      "",                       // NOLINT
      "2,Maria,Garcia,36.7\n",  // NOLINT
      "2,Maria,Garcia,36.7\n",  // NOLINT
      "1\n"                     // NOLINT
  };
  for (uint32_t i = 0; i < iSQLVec.size(); i++) {
    std::vector<Result *> iResVec = Execute(pDB, iSQLVec[i]);
    EXPECT_EQ(iResVec.size(), 1);
    EXPECT_EQ(iResVec[0]->ToString(), results[i]);
  }
  delete pDB;
}

TEST(Lab2, UpdateDeleteTest) {
  Instance *pDB = new Instance();
  std::vector<String> iSQLVec = {
      "CREATE TABLE Persons(ID INT, FirstName VARCHAR(20), LastName VARCHAR(20), Temperature FLOAT);",
      "ALTER TABLE Persons ADD INDEX(ID);",                           // NOLINT
      "ALTER TABLE Persons ADD INDEX(Temperature);",                  // NOLINT
      "INSERT INTO Persons VALUES(1, 'James', 'Smith', 36.4);",       // NOLINT
      "INSERT INTO Persons VALUES(2, 'Maria', 'Garcia', 36.7);",      // NOLINT
      "UPDATE Persons SET Temperature = 35.7 WHERE Persons.ID = 2;",  // NOLINT
      "DELETE FROM Persons WHERE Persons.ID = 1;",                    // NOLINT
      "SELECT * FROM Persons WHERE Persons.ID = 1;",                  // NOLINT
      "SELECT * FROM Persons WHERE Persons.ID = 2;",                  // NOLINT
      "SELECT * FROM Persons WHERE Persons.Temperature <= 37;",       // NOLINT
      "DROP TABLE Persons;"                                           // NOLINT
  };
  std::vector<String> results = {
      "1\n",                    // NOLINT
      "1\n",                    // NOLINT
      "1\n",                    // NOLINT
      "1\n",                    // NOLINT
      "1\n",                    // NOLINT
      "1\n",                    // NOLINT
      "1\n",                    // NOLINT
      "",                       // NOLINT
      "2,Maria,Garcia,35.7\n",  // NOLINT
      "2,Maria,Garcia,35.7\n",  // NOLINT
      "1\n"                     // NOLINT
  };
  for (uint32_t i = 0; i < iSQLVec.size(); i++) {
    std::vector<Result *> iResVec = Execute(pDB, iSQLVec[i]);
    EXPECT_EQ(iResVec.size(), 1);
    EXPECT_EQ(iResVec[0]->ToString(), results[i]);
  }
  delete pDB;
}

TEST(Lab2, DeleteInsertTest) {
  Instance *pDB = new Instance();
  std::vector<String> iSQLVec = {
      "CREATE TABLE Persons(ID INT, FirstName VARCHAR(20), LastName VARCHAR(20), Temperature FLOAT);",
      "ALTER TABLE Persons ADD INDEX(ID);",                       // NOLINT
      "ALTER TABLE Persons ADD INDEX(Temperature);",              // NOLINT
      "INSERT INTO Persons VALUES(1, 'James', 'Smith', 36.4);",   // NOLINT
      "INSERT INTO Persons VALUES(2, 'Maria', 'Garcia', 36.7);",  // NOLINT
      "DELETE FROM Persons WHERE Persons.ID = 1;",                // NOLINT
      "INSERT INTO Persons VALUES(3, 'Robert', 'Smith', 37.4);",  // NOLINT
      "SELECT * FROM Persons WHERE Persons.ID = 1;",              // NOLINT
      "SELECT * FROM Persons WHERE Persons.ID = 2;",              // NOLINT
      "SELECT * FROM Persons WHERE Persons.ID = 3;",              // NOLINT
      "SELECT * FROM Persons WHERE Persons.Temperature > 37;",    // NOLINT
      "SELECT * FROM Persons WHERE Persons.Temperature < 37;",    // NOLINT
      "DROP TABLE Persons;"                                       // NOLINT
  };
  std::vector<String> results = {
      "1\n",                    // NOLINT
      "1\n",                    // NOLINT
      "1\n",                    // NOLINT
      "1\n",                    // NOLINT
      "1\n",                    // NOLINT
      "1\n",                    // NOLINT
      "1\n",                    // NOLINT
      "",                       // NOLINT
      "2,Maria,Garcia,36.7\n",  // NOLINT
      "3,Robert,Smith,37.4\n",  // NOLINT
      "3,Robert,Smith,37.4\n",  // NOLINT
      "2,Maria,Garcia,36.7\n",  // NOLINT
      "1\n"                     // NOLINT
  };
  for (uint32_t i = 0; i < iSQLVec.size(); i++) {
    std::vector<Result *> iResVec = Execute(pDB, iSQLVec[i]);
    EXPECT_EQ(iResVec.size(), 1);
    EXPECT_EQ(iResVec[0]->ToString(), results[i]);
  }
  delete pDB;
}

}  // namespace thdb
