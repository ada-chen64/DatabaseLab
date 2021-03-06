#include "backend/backend.h"
#include "gtest/gtest.h"
#include "sql_generator.h"
#include "system/instance.h"
#include "utils/display.h"

namespace thdb {

TEST(Lab1, RandomSqlTest) {
  Instance *pDB = new Instance();
  size_t data_num = 1000;
  size_t sample_num = 10;
  std::vector<String> iSQLVec = SqlGenerator(data_num, sample_num);
  ASSERT_EQ(iSQLVec.size(), 1 + data_num + 1 + sample_num + 1);
  std::vector<size_t> results(1 + data_num + 1 + sample_num + 1, 1);
  results[data_num + 1] = data_num;
  for (uint32_t i = 0; i < iSQLVec.size(); i++) {
    std::vector<Result *> iResVec = Execute(pDB, iSQLVec[i]);
    EXPECT_EQ(iResVec.size(), 1);
    EXPECT_EQ(iResVec[0]->GetSize(), results[i]);
  }
  delete pDB;
}

}  // namespace thdb
