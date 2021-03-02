#include <vector>

#include "gtest/gtest.h"
#include "antlr4-runtime.h"

#include "defines.h"

namespace thdb {

TEST(Lab1, AntlrTest) {
    String CreateSQL = "CREATE TABLE A( A INT, B FLOAT);\n";
    String InsertSQL = "INSERT INTO A VALUES (0, 1.0);\n";
    String SelectAllSQL = "SELECT * FROM A;\n";
    String SelectConditionSQL = "SELECT * FROM A;\n";
    std::vector<String> iSQLs = {CreateSQL, InsertSQL, SelectAllSQL, SelectConditionSQL};
    for (const auto &it : iSQLs) {
        antlr4::ANTLRInputStream sInputStream(it);
    }
}

} // namespace thdb
