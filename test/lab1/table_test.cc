#include "table/table.h"

#include "backend/backend.h"
#include "field/field.h"
#include "gtest/gtest.h"
#include "record/fixed_record.h"
#include "system/instance.h"
#include "utils/display.h"

namespace thdb {

TEST(Lab1, TableTest) {
  Instance *pDB = new Instance();
  Execute(pDB, "CREATE TABLE Persons(ID INT, FirstName VARCHAR(20), LastName VARCHAR(20), Temperature FLOAT);");
  Table *table = pDB->GetTable("Persons");
  std::vector<FieldType> types = {FieldType::INT_TYPE, FieldType::STRING_TYPE, FieldType::STRING_TYPE,
                                  FieldType::FLOAT_TYPE};
  std::vector<Size> sizes = {4, 20, 20, 8};
  std::vector<String> values = {"1", "'James'", "'Smith'", "36.4"};
  Record *record = new FixedRecord(4, types, sizes);
  record->Build(values);
  PageSlotID page_slot_id = table->InsertRecord(record);
  PageID page_id = page_slot_id.first;
  SlotID slot_id = page_slot_id.second;

  const Size record_size = 4 + 20 + 20 + 8;

  uint8_t dst[record_size];
  Size store_size = record->Store(dst);
  EXPECT_EQ(store_size, record_size);

  Size load_size = record->Load(dst);
  EXPECT_EQ(load_size, record_size);

  Record *new_record = table->GetRecord(page_id, slot_id);
  EXPECT_EQ(record->ToString(), new_record->ToString());

  std::vector<PageSlotID> page_slot_ids = table->SearchRecord(nullptr);
  EXPECT_EQ(page_slot_ids.size(), 1);
  EXPECT_EQ(page_slot_ids[0].first, page_id);
  EXPECT_EQ(page_slot_ids[0].second, slot_id);

  std::vector<String> values_new = {"1", "'James'", "'Smith'", "36.4"};
  std::vector<Transform> trans_vec;
  for (size_t i = 0; i < values_new.size(); i++) {
    trans_vec.push_back(Transform(i, types[i], values_new[i]));
  }
  table->UpdateRecord(page_id, slot_id, trans_vec);

  table->DeleteRecord(page_id, slot_id);

  page_slot_ids = table->SearchRecord(nullptr);
  EXPECT_EQ(page_slot_ids.size(), 0);

  Execute(pDB, "DROP TABLE Persons;");
  delete pDB;
}

}  // namespace thdb
