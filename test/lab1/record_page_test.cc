#include "page/record_page.h"

#include "backend/backend.h"
#include "gtest/gtest.h"
#include "system/instance.h"

namespace thdb {

TEST(Lab1, RecordPageTest) {
  Instance *pDB = new Instance();
  Execute(pDB, "CREATE TABLE Persons(ID INT, FirstName VARCHAR(20), LastName VARCHAR(20), Temperature FLOAT);");
  const Size size = 4 + 20 + 20 + 8;
  RecordPage *record_page = new RecordPage(size, true);
  uint8_t src[size] = "test";
  SlotID slot_id = record_page->InsertRecord(src);
  uint8_t *dst = record_page->GetRecord(slot_id);
  EXPECT_EQ(0, memcmp(dst, src, size));
  record_page->DeleteRecord(slot_id);
  Execute(pDB, "DROP TABLE Persons;");
  delete pDB;
}

}  // namespace thdb
