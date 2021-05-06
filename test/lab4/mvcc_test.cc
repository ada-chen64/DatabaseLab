#include <chrono>
#include <random>

#include "backend/backend.h"
#include "gtest/gtest.h"
#include "record/fixed_record.h"
#include "table/schema.h"

namespace thdb {

class MVCCTests : public ::testing::Test {
 public:
  void SetUp() override {
    db = new Instance();
    std::vector<Column> columns({Column(column_name, FieldType::INT_TYPE)});
    Schema schema(columns);
    db->CreateTable(table_name, schema);
    table = db->GetTable(table_name);
    txn_manager = db->GetTransactionManager();
    insert_record = table->EmptyRecord();
    insert_record->Build({record_init});
  }

  void TearDown() override {
    delete insert_record;
    db->DropTable(table_name);
    delete db;
  }

  void CheckExist(Transaction *txn, String record_string) {
    std::vector<PageSlotID> rids = table->SearchRecord(nullptr, txn);
    EXPECT_EQ(rids.size(), 1);
    Record *select_record = table->GetRecord(rids[0].first, rids[0].second);
    EXPECT_EQ(select_record->ToString(), record_string);
    delete select_record;
  }

  void CheckNotExist(Transaction *txn) {
    std::vector<PageSlotID> rids = table->SearchRecord(nullptr, txn);
    EXPECT_EQ(rids.size(), 0);
  }

  Instance *db;
  Table *table;
  TransactionManager *txn_manager;
  Record *insert_record;
  // Record 的 ToString 函数在字符串末添加了一个空格
  const String record_init = "0 ";
  const String record_updated = "1 ";

 private:
  const String table_name = "test";
  const String column_name = "mvcc";
};

//    Txn #0 | Txn #1 | Txn #2 |
//    --------------------------
//    BEGIN  |        |        |
//    W(X)   |        |        |
//    R(X)   |        |        |
//           | BEGIN  |        |
//           | R(X)   |        |
//    COMMIT |        |        |
//           | R(X)   |        |
//           | COMMIT |        |
//           |        | BEGIN  |
//           |        | R(X)   |
//           |        | COMMIT |
//
// Txn #0 should only read Txn #0's version of X
// Txn #1 should NOT read Txn #0's version of X because its start time is before #0's commit
// Txn #2 should only read Txn #0's version of X
TEST_F(MVCCTests, CommitInsert1) {
  auto *txn0 = txn_manager->Begin();

  table->InsertRecord(insert_record, txn0);

  CheckExist(txn0, record_init);

  auto *txn1 = txn_manager->Begin();

  CheckNotExist(txn1);

  txn_manager->Commit(txn0);
  delete txn0;

  CheckNotExist(txn1);

  txn_manager->Commit(txn1);
  delete txn1;

  auto *txn2 = txn_manager->Begin();

  CheckExist(txn2, record_init);

  txn_manager->Commit(txn2);
  delete txn2;
}

//    Txn #0 | Txn #1 | Txn #2 |
//    --------------------------
//    BEGIN  |        |        |
//           | BEGIN  |        |
//           | W(X)   |        |
//    R(X)   |        |        |
//           | R(X)   |        |
//           | COMMIT |        |
//    R(X)   |        |        |
//    COMMIT |        |        |
//           |        | BEGIN  |
//           |        | R(X)   |
//           |        | COMMIT |
//
// Txn #0 should NOT read Txn #1's version of X because its start time is before #1's commit
// Txn #1 should only read Txn #1's version of X
// Txn #2 should only read Txn #1's version of X
TEST_F(MVCCTests, CommitInsert2) {
  auto *txn0 = txn_manager->Begin();

  auto *txn1 = txn_manager->Begin();

  table->InsertRecord(insert_record, txn1);

  CheckNotExist(txn0);

  CheckExist(txn1, record_init);

  txn_manager->Commit(txn1);
  delete txn1;

  CheckNotExist(txn0);

  txn_manager->Commit(txn0);
  delete txn0;

  auto *txn2 = txn_manager->Begin();

  CheckExist(txn2, record_init);

  txn_manager->Commit(txn2);
  delete txn2;
}

//    Txn #0 | Txn #1 | Txn #2 |
//    --------------------------
//    BEGIN  |        |        |
//    W(X)   |        |        |
//    R(X)   |        |        |
//           | BEGIN  |        |
//           | R(X)   |        |
//    ABORT  |        |        |
//           | R(X)   |        |
//           | COMMIT |        |
//           |        | BEGIN  |
//           |        | R(X)   |
//           |        | COMMIT |
//
// Txn #0 should only read Txn #0's version of X
// Txn #1 should NOT read Txn #0's version of X because its start time is before #0's commit
// Txn #2 should NOT read Txn #0's version of X because Txn #0 aborted
TEST_F(MVCCTests, AbortInsert1) {
  auto *txn0 = txn_manager->Begin();

  table->InsertRecord(insert_record, txn0);

  CheckExist(txn0, record_init);

  auto *txn1 = txn_manager->Begin();

  CheckNotExist(txn1);

  txn_manager->Abort(txn0);
  delete txn0;

  CheckNotExist(txn1);

  txn_manager->Commit(txn1);
  delete txn1;

  auto *txn2 = txn_manager->Begin();

  CheckNotExist(txn2);

  txn_manager->Commit(txn2);
  delete txn2;
}

//    Txn #0 | Txn #1 | Txn #2 |
//    --------------------------
//    BEGIN  |        |        |
//           | BEGIN  |        |
//           | W(X)   |        |
//    R(X)   |        |        |
//           | R(X)   |        |
//           | ABORT  |        |
//    R(X)   |        |        |
//    COMMIT |        |        |
//           |        | BEGIN  |
//           |        | R(X)   |
//           |        | COMMIT |
//
// Txn #0 should NOT read Txn #1's version of X because its start time is before #1's commit
// Txn #1 should only read Txn #1's version of X
// Txn #2 should NOT read Txn #1's version of X because Txn #1 aborted
TEST_F(MVCCTests, AbortInsert2) {
  auto *txn0 = txn_manager->Begin();

  auto *txn1 = txn_manager->Begin();

  table->InsertRecord(insert_record, txn1);

  CheckNotExist(txn0);

  CheckExist(txn1, record_init);

  txn_manager->Abort(txn1);
  delete txn1;

  CheckNotExist(txn0);

  txn_manager->Commit(txn0);
  delete txn0;

  auto *txn2 = txn_manager->Begin();

  CheckNotExist(txn2);

  txn_manager->Commit(txn2);
  delete txn2;
}

}  // namespace thdb
