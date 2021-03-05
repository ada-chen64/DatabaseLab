#include "backend/backend.h"
#include "gtest/gtest.h"
#include "sql_generator.h"
#include "system/instance.h"

namespace thdb {

TEST(Lab1, RandomSqlTest) {
  Instance *pDB = new Instance();
  std::vector<String> iSQLVec = SqlGenerator();
  for (const auto &sSQL : iSQLVec) {
    printf("%s\n", sSQL.c_str());
    auto results = Execute(pDB, sSQL);
    EXPECT_EQ(results.size(), 1);
    results[0]->Display();
    printf("%u\n", results[0]->GetSize());
    delete results[0];
  }
  delete pDB;
}

}  // namespace thdb
