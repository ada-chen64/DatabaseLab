#include "backend/backend.h"
#include "gtest/gtest.h"
#include "system/instance.h"
#include "utils/display.h"

namespace thdb {

TEST(Lab1, ShowTableTest) {
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

TEST(Lab1, InsertSelectTest) {
  Instance *pDB = new Instance();
  std::vector<String> iSQLVec = {
      "CREATE TABLE Persons(ID INT, FirstName VARCHAR(20), LastName VARCHAR(20), Temperature FLOAT);",
      "INSERT INTO Persons VALUES(1, 'James', 'Smith', 36.4);",  // NOLINT
      "SELECT * FROM Persons;",                                  // NOLINT
      "DROP TABLE Persons;"                                      // NOLINT
  };
  std::vector<String> results = {"1\n", "1\n", "1,James,Smith,36.4\n", "1\n"};
  for (uint32_t i = 0; i < iSQLVec.size(); i++) {
    std::vector<Result *> iResVec = Execute(pDB, iSQLVec[i]);
    EXPECT_EQ(iResVec.size(), 1);
    EXPECT_EQ(iResVec[0]->ToString(), results[i]);
  }
  delete pDB;
}

TEST(Lab1, UpdateTest) {
  Instance *pDB = new Instance();
  std::vector<String> iSQLVec = {
      "CREATE TABLE Persons(ID INT, FirstName VARCHAR(20), LastName VARCHAR(20), Temperature FLOAT);",
      "INSERT INTO Persons VALUES(1, 'James', 'Smith', 36.4);",       // NOLINT
      "INSERT INTO Persons VALUES(2, 'Maria', 'Garcia', 36.7);",      // NOLINT
      "UPDATE Persons SET Temperature = 35.4 WHERE Persons.ID = 2;",  // NOLINT
      "SELECT * FROM Persons;",                                       // NOLINT
      "DROP TABLE Persons;"                                           // NOLINT
  };
  std::vector<String> results = {"1\n", "1\n", "1\n", "1\n", "1,James,Smith,36.4\n2,Maria,Garcia,35.4\n", "1\n"};
  for (uint32_t i = 0; i < iSQLVec.size(); i++) {
    std::vector<Result *> iResVec = Execute(pDB, iSQLVec[i]);
    EXPECT_EQ(iResVec.size(), 1);
    EXPECT_EQ(iResVec[0]->ToString(), results[i]);
  }
  delete pDB;
}

TEST(Lab1, DeleteTest) {
  Instance *pDB = new Instance();
  std::vector<String> iSQLVec = {
      "CREATE TABLE Persons(ID INT, FirstName VARCHAR(20), LastName VARCHAR(20), Temperature FLOAT);",
      "INSERT INTO Persons VALUES(1, 'James', 'Smith', 36.4);",   // NOLINT
      "INSERT INTO Persons VALUES(2, 'Maria', 'Garcia', 36.7);",  // NOLINT
      "DELETE FROM Persons WHERE Persons.ID = 1;",                // NOLINT
      "SELECT * FROM Persons;",                                   // NOLINT
      "DROP TABLE Persons;"                                       // NOLINT
  };
  std::vector<String> results = {"1\n", "1\n", "1\n", "1\n", "2,Maria,Garcia,36.7\n", "1\n"};
  for (uint32_t i = 0; i < iSQLVec.size(); i++) {
    std::vector<Result *> iResVec = Execute(pDB, iSQLVec[i]);
    EXPECT_EQ(iResVec.size(), 1);
    EXPECT_EQ(iResVec[0]->ToString(), results[i]);
  }
  delete pDB;
}

TEST(Lab1, UpdateDeleteTest) {
  Instance *pDB = new Instance();
  std::vector<String> iSQLVec = {
      "CREATE TABLE Persons(ID INT, FirstName VARCHAR(20), LastName VARCHAR(20), Temperature FLOAT);",
      "INSERT INTO Persons VALUES(1, 'James', 'Smith', 36.4);",       // NOLINT
      "INSERT INTO Persons VALUES(2, 'Maria', 'Garcia', 36.7);",      // NOLINT
      "UPDATE Persons SET Temperature = 35.7 WHERE Persons.ID = 2;",  // NOLINT
      "DELETE FROM Persons WHERE Persons.ID = 1;",                    // NOLINT
      "SELECT * FROM Persons;",                                       // NOLINT
      "DROP TABLE Persons;"                                           // NOLINT
  };
  std::vector<String> results = {"1\n", "1\n", "1\n", "1\n", "1\n", "2,Maria,Garcia,35.7\n", "1\n"};
  for (uint32_t i = 0; i < iSQLVec.size(); i++) {
    std::vector<Result *> iResVec = Execute(pDB, iSQLVec[i]);
    EXPECT_EQ(iResVec.size(), 1);
    EXPECT_EQ(iResVec[0]->ToString(), results[i]);
  }
  delete pDB;
}

TEST(Lab1, DeleteInsertTest) {
  Instance *pDB = new Instance();
  std::vector<String> iSQLVec = {
      "CREATE TABLE Persons(ID INT, FirstName VARCHAR(20), LastName VARCHAR(20), Temperature FLOAT);",
      "INSERT INTO Persons VALUES(1, 'James', 'Smith', 36.4);",   // NOLINT
      "INSERT INTO Persons VALUES(2, 'Maria', 'Garcia', 36.7);",  // NOLINT
      "DELETE FROM Persons WHERE Persons.ID = 1;",                // NOLINT
      "INSERT INTO Persons VALUES(3, 'Robert', 'Smith', 36.8);",  // NOLINT
      "SELECT * FROM Persons;",                                   // NOLINT
      "DROP TABLE Persons;"                                       // NOLINT
  };
  // clang-format off
  std::vector<String> results = {"1\n", "1\n", "1\n", "1\n", "1\n", "3,Robert,Smith,36.8\n2,Maria,Garcia,36.7\n", "1\n"};
  // clang-format on
  for (uint32_t i = 0; i < iSQLVec.size(); i++) {
    std::vector<Result *> iResVec = Execute(pDB, iSQLVec[i]);
    EXPECT_EQ(iResVec.size(), 1);
    EXPECT_EQ(iResVec[0]->ToString(), results[i]);
  }
  delete pDB;
}

}  // namespace thdb
