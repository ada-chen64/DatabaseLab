#include "table/table.h"

#include "backend/backend.h"
#include "field/field.h"
#include "gtest/gtest.h"
#include "record/fixed_record.h"
#include "system/instance.h"
#include "utils/display.h"

namespace thdb {

<<<<<<< HEAD
TEST(Lab1, DISABLED_TableTest) {
=======
TEST(Lab1, TableTest) {
  // 新建数据库对象
>>>>>>> f317760095ecb5b05bc6ce2ba2228a4c22f20782
  Instance *pDB = new Instance();

  // 通过 create table 语句新建测试表
  Execute(pDB, "CREATE TABLE Persons(ID INT, FirstName VARCHAR(20), LastName VARCHAR(20), Temperature FLOAT);");

  // 用 GetTable 函数，传入表名，获取 Table 对象
  Table *table = pDB->GetTable("Persons");

  // 构造一条记录
  std::vector<String> values = {"1", "'James'", "'Smith'", "36.4"};
  std::vector<FieldType> types = {FieldType::INT_TYPE, FieldType::STRING_TYPE, FieldType::STRING_TYPE,
                                  FieldType::FLOAT_TYPE};
  std::vector<Size> sizes = {4, 20, 20, 8};
  Record *record = new FixedRecord(4, types, sizes);
  record->Build(values);

  // 利用 InsertRecord 函数插入记录，并记录 page_id 和 slot_id
  PageSlotID page_slot_id = table->InsertRecord(record);
  PageID page_id = page_slot_id.first;
  SlotID slot_id = page_slot_id.second;

  Record *new_record = table->GetRecord(page_id, slot_id);
  EXPECT_EQ(record->ToString(), new_record->ToString());

  delete record;
  delete new_record;

  // 利用 SeachRecord 函数查找记录，并传入 nullptr, 表示查找该表的所有记录
  std::vector<PageSlotID> page_slot_ids = table->SearchRecord(nullptr);
  // 比较 page_id 和 slot_id 是否正确
  EXPECT_EQ(page_slot_ids.size(), 1);
  EXPECT_EQ(page_slot_ids[0].first, page_id);
  EXPECT_EQ(page_slot_ids[0].second, slot_id);

  // 构造用于更新的 record
  std::vector<String> values_new = {"1", "'James'", "'Smith'", "36.8"};
  Record *update_record = new FixedRecord(4, types, sizes);
  update_record->Build(values_new);
  std::vector<Transform> trans_vec;
  trans_vec.push_back(Transform(3, types[3], values_new[3]));

  // 执行更新操作
  table->UpdateRecord(page_id, slot_id, trans_vec);
  // 读出记录，判断记录是否已更新
  Record *update_record_new = table->GetRecord(page_id, slot_id);
  EXPECT_EQ(update_record->ToString(), update_record_new->ToString());

  delete update_record;
  delete update_record_new;

  // 删除记录
  table->DeleteRecord(page_id, slot_id);

  // 判断记录是否已删除
  page_slot_ids = table->SearchRecord(nullptr);
  EXPECT_EQ(page_slot_ids.size(), 0);

  Execute(pDB, "DROP TABLE Persons;");
  delete pDB;
}

}  // namespace thdb
