#include "backend/backend.h"
#include "gtest/gtest.h"
#include "system/instance.h"
#include "utils/display.h"

namespace thdb {

TEST(Lab1, InsertSelectTest) {
  Instance *pDB = new Instance();
  std::vector<String> iSQLVec = {
      "CREATE TABLE Persons(ID INT, FirstName VARCHAR(20), LastName VARCHAR(20), Temperature FLOAT);",
      "INSERT INTO Persons VALUES(1, 'James', 'Smith', 36.4);",  // NOLINT
      "SELECT * FROM Persons;",                                  // NOLINT
      "DROP TABLE Persons;"                                      // NOLINT
  };
  for (const auto &sSQL : iSQLVec) {
    std::vector<Result *> iResVec = Execute(pDB, sSQL);
    PrintTable(iResVec);
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
  for (const auto &sSQL : iSQLVec) {
    std::vector<Result *> iResVec = Execute(pDB, sSQL);
    PrintTable(iResVec);
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
  for (const auto &sSQL : iSQLVec) {
    std::vector<Result *> iResVec = Execute(pDB, sSQL);
    PrintTable(iResVec);
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
  for (const auto &sSQL : iSQLVec) {
    std::vector<Result *> iResVec = Execute(pDB, sSQL);
    PrintTable(iResVec);
  }
  delete pDB;
}

}  // namespace thdb
