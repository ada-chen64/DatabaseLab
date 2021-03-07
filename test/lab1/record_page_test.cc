#include "page/record_page.h"

#include "backend/backend.h"
#include "gtest/gtest.h"
#include "system/instance.h"

namespace thdb {

TEST(Lab1, RecordPageTest) {
  // 新建数据库对象，并通过 create table 语句新建测试表
  Instance *pDB = new Instance();
  Execute(pDB, "CREATE TABLE Persons(ID INT, FirstName VARCHAR(20), LastName VARCHAR(20), Temperature FLOAT);");
  const Size size = 4 + 20 + 20 + 8;
  RecordPage *record_page = new RecordPage(size, true);
  uint8_t src[size] = "test";
  // 将 src 存入 record，并记录 slot_id
  SlotID slot_id = record_page->InsertRecord(src);
  // 根据 slot_id 读出 record
  uint8_t *dst = record_page->GetRecord(slot_id);
  // 比较存入的 src 和读出的 dst 是否一致
  EXPECT_EQ(0, memcmp(dst, src, size));
  // 删除 slot_id 中的记录
  record_page->DeleteRecord(slot_id);
  Execute(pDB, "DROP TABLE Persons;");
  delete pDB;
}

}  // namespace thdb
