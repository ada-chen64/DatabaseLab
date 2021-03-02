#include "gtest/gtest.h"

#include "backend/backend.h"
#include "system/instance.h"

using namespace thdb;

TEST(Lab1, SQLTest) {
    Instance * pDB = new Instance();
    std::vector<String> iSQLVec = {
        "SHOW TABLES;",
        "CREATE TABLE A(A INT, B FLOAT);",
        "SHOW TABLES;"
    };
    for (const auto &sSQL : iSQLVec) {
        Execute(pDB, sSQL);
        printf("%s Finish\n", sSQL.c_str());
    }
    delete pDB;
}