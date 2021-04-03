#include "backend/backend.h"
#include "gtest/gtest.h"
#include "sql_generator.h"
#include "system/instance.h"
#include "utils/display.h"

namespace thdb {

TEST(Lab2, RandomSqlTest) {
  Instance *pDB = new Instance();
  Size data_num = 1000;  // 插入记录的行数
  Size sample_num = 10;  // 选择 10 records after inserting

  // 生成 1 条 create table 语句, 1000 条 insert 语句, 1 条 select * 语句, 10 条条件 select 语句, 1 条 drop table 语句
  std::vector<String> iSQLVec = SqlGenerator(data_num, sample_num);
  EXPECT_EQ(iSQLVec.size(), 1 + data_num + 1 + 1 + sample_num + 1);

  // results 表示 sql 执行结果行数
  std::vector<Size> results(1 + data_num + 1 + 1 + sample_num + 1, 1);
  results[data_num + 2] = data_num;

  // 执行每条 sql 语句
  for (uint32_t i = 0; i < iSQLVec.size(); i++) {
    std::vector<Result *> iResVec = Execute(pDB, iSQLVec[i]);
    // 由于 SqlGenerator 函数生成的 sql 中，每行只有一条 sql 语句，因此 iResVec 的大小始终为 1.
    // 只有当一行包含多条 sql 时，iResVec 大小为一行内 sql 语句的个数。
    EXPECT_EQ(iResVec.size(), 1);
    // 比较 sql 执行结果数量是否正确
    EXPECT_EQ(iResVec[0]->GetSize(), results[i]);
  }
  delete pDB;
}

}  // namespace thdb
