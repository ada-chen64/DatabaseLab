#include "backend/backend.h"
#include "gtest/gtest.h"

namespace thdb {

class RecoveryTests : public ::testing::Test {
 public:
  // This function is called before every test.
  void SetUp() override {
    Init();
    db = new Instance();
    std::vector<Column> columns({Column(column_name, FieldType::INT_TYPE)});
    Schema schema(columns);
    db->CreateTable(table_name, schema, true);
  }

  // This function is called after every test
  void TearDown() override {
    db->DropTable(table_name);
    delete db;
    Clear();
  }

  void CheckExist(Transaction *txn, String record_string) {
    std::vector<PageSlotID> rids = db->Search(table_name, nullptr, {}, txn);
    EXPECT_EQ(rids.size(), 1);
    Record *select_record = db->GetRecord(table_name, rids[0], txn);
    EXPECT_EQ(select_record->ToString(), record_string);
    delete select_record;
  }

  void CheckNotExist(Transaction *txn) {
    std::vector<PageSlotID> rids = db->Search(table_name, nullptr, {}, txn);
    std::vector<Record *> records;
    for (const auto &rid : rids) {
      records.push_back(db->GetRecord(table_name, rid, txn));
    }
    EXPECT_EQ(records.size(), 0);
  }

  Instance *db;

  const std::vector<String> record_build = {"0"};

  const String record_init = "0 ";

  const String table_name = "test";
  const String column_name = "recovery";
};

TEST_F(RecoveryTests, RedoTest) {
  Transaction *txn = db->GetTransactionManager()->Begin();
  db->Insert(table_name, record_build, txn);
  db->GetTransactionManager()->Commit(txn);
  delete txn;

  delete db;

  db = new Instance();

  db->GetRecoveryManager()->Redo();
  db->GetRecoveryManager()->Undo();

  txn = db->GetTransactionManager()->Begin();
  CheckExist(txn, record_init);
  db->GetTransactionManager()->Commit(txn);
  delete txn;
}

TEST_F(RecoveryTests, UndoTest) {
  Transaction *txn = db->GetTransactionManager()->Begin();
  db->Insert(table_name, record_build, txn);
  delete txn;

  delete db;

  db = new Instance();

  db->GetRecoveryManager()->Redo();
  db->GetRecoveryManager()->Undo();

  txn = db->GetTransactionManager()->Begin();
  CheckNotExist(txn);
  db->GetTransactionManager()->Commit(txn);
  delete txn;
}

}  // namespace thdb
