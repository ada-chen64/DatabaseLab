#include "backend/backend.h"
#include "gtest/gtest.h"
#include "system/instance.h"
#include "utils/display.h"

namespace thdb {

TEST(Lab2, InsertSelectTest) {
  Instance *pDB = new Instance();
  std::vector<String> iSQLVec = {
      "CREATE TABLE Persons(ID INT, FirstName VARCHAR(20), LastName VARCHAR(20), Temperature FLOAT);",
      "ALTER TABLE Persons ADD INDEX(ID);",                      // NOLINT
      "ALTER TABLE Persons ADD INDEX(Temperature);",             // NOLINT
      "INSERT INTO Persons VALUES(1, 'James', 'Smith', 36.4);",  // NOLINT
      "SELECT * FROM Persons;",                                  // NOLINT
      "DROP TABLE Persons;"                                      // NOLINT
  };
  // 存放 sql 执行结果
  std::vector<String> results = {"1\n", "1\n", "1\n", "1\n", "1,James,Smith,36.4\n", "1\n"};
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
      "CREATE TABLE Persons(ID INT, FirstName VARCHAR(20), LastName VARCHAR(20), Temperature FLOAT);",
      "ALTER TABLE Persons ADD INDEX(ID);",                           // NOLINT
      "ALTER TABLE Persons ADD INDEX(Temperature);",                  // NOLINT
      "INSERT INTO Persons VALUES(1, 'James', 'Smith', 36.4);",       // NOLINT
      "INSERT INTO Persons VALUES(2, 'Maria', 'Garcia', 36.7);",      // NOLINT
      "UPDATE Persons SET Temperature = 35.4 WHERE Persons.ID = 2;",  // NOLINT
      "SELECT * FROM Persons;",                                       // NOLINT
      "DROP TABLE Persons;"                                           // NOLINT
  };
  std::vector<String> results = {"1\n", "1\n", "1\n", "1\n", "1\n", "1\n", "1,James,Smith,36.4\n2,Maria,Garcia,35.4\n",
                                 "1\n"};
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
      "CREATE TABLE Persons(ID INT, FirstName VARCHAR(20), LastName VARCHAR(20), Temperature FLOAT);",
      "ALTER TABLE Persons ADD INDEX(ID);",                       // NOLINT
      "ALTER TABLE Persons ADD INDEX(Temperature);",              // NOLINT
      "INSERT INTO Persons VALUES(1, 'James', 'Smith', 36.4);",   // NOLINT
      "INSERT INTO Persons VALUES(2, 'Maria', 'Garcia', 36.7);",  // NOLINT
      "DELETE FROM Persons WHERE Persons.ID = 1;",                // NOLINT
      "SELECT * FROM Persons;",                                   // NOLINT
      "DROP TABLE Persons;"                                       // NOLINT
  };
  std::vector<String> results = {"1\n", "1\n", "1\n", "1\n", "1\n", "1\n", "2,Maria,Garcia,36.7\n", "1\n"};
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
      "SELECT * FROM Persons;",                                       // NOLINT
      "DROP TABLE Persons;"                                           // NOLINT
  };
  std::vector<String> results = {"1\n", "1\n", "1\n", "1\n", "1\n", "1\n", "1\n", "2,Maria,Garcia,35.7\n", "1\n"};
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
      "INSERT INTO Persons VALUES(3, 'Robert', 'Smith', 36.8);",  // NOLINT
      "SELECT * FROM Persons;",                                   // NOLINT
      "DROP TABLE Persons;"                                       // NOLINT
  };
  // clang-format off
  std::vector<String> results = {"1\n", "1\n", "1\n", "1\n", "1\n", "1\n", "1\n", "3,Robert,Smith,36.8\n2,Maria,Garcia,36.7\n", "1\n"};
  std::vector<String> results_2 = {"1\n", "1\n", "1\n", "1\n", "1\n", "1\n", "1\n", "2,Maria,Garcia,36.7\n3,Robert,Smith,36.8\n", "1\n"};
  // clang-format on
  for (uint32_t i = 0; i < iSQLVec.size(); i++) {
    std::vector<Result *> iResVec = Execute(pDB, iSQLVec[i]);
    EXPECT_EQ(iResVec.size(), 1);
    if (i == 5) {
      if (iResVec[0]->ToString()[0] == '3') {
        EXPECT_EQ(iResVec[0]->ToString(), results[i]);
      } else {
        EXPECT_EQ(iResVec[0]->ToString(), results_2[i]);
      }
    }
  }
  delete pDB;
}

}  // namespace thdb
