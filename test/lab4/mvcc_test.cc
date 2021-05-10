#include <chrono>
#include <random>

#include "backend/backend.h"
#include "gtest/gtest.h"
#include "record/fixed_record.h"
#include "record/transform.h"
#include "table/schema.h"

namespace thdb {

class MVCCTests : public ::testing::Test {
 public:
  // This function is called before every test.
  void SetUp() override {
    db = new Instance();
    std::vector<Column> columns({Column(column_name, FieldType::INT_TYPE)});
    Schema schema(columns);
    db->CreateTable(table_name, schema, true);
    txn_manager = db->GetTransactionManager();
  }

  // This function is called after every test
  void TearDown() override {
    db->DropTable(table_name);
    delete db;
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
  TransactionManager *txn_manager;
  std::vector<Transform> transform = {Transform(0, FieldType::INT_TYPE, "1")};

  const std::vector<String> record_build = {"0"};

  const String record_init = "0 ";
  const String record_updated = "1 ";

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

  db->Insert(table_name, record_build, txn0);

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

  db->Insert(table_name, record_build, txn1);

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

  db->Insert(table_name, record_build, txn0);

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

  db->Insert(table_name, record_build, txn1);

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

/**
 * The following tests are optional
 */

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
// Txn #1 should only read the previous version of X because its start time is before #0's commit
// Txn #2 should only read Txn #0's version of X
TEST_F(MVCCTests, CommitUpdate1) {
  // Insert the tuple to be Updated later
  auto *txn = txn_manager->Begin();
  db->Insert(table_name, record_build, txn);
  txn_manager->Commit(txn);

  auto *txn0 = txn_manager->Begin();

  db->Update(table_name, nullptr, {}, transform, txn0);

  CheckExist(txn0, record_updated);

  auto *txn1 = txn_manager->Begin();

  CheckExist(txn1, record_init);

  txn_manager->Commit(txn0);
  delete txn0;

  CheckExist(txn1, record_init);

  txn_manager->Commit(txn1);
  delete txn1;

  auto *txn2 = txn_manager->Begin();

  CheckExist(txn2, record_updated);

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
// Txn #0 should only read the previous version of X because its start time is before #1's commit
// Txn #1 should only read Txn #1's version of X
// Txn #2 should only read Txn #1's version of X
TEST_F(MVCCTests, CommitUpdate2) {
  auto *txn = txn_manager->Begin();
  db->Insert(table_name, record_build, txn);
  txn_manager->Commit(txn);

  auto *txn0 = txn_manager->Begin();

  auto *txn1 = txn_manager->Begin();

  db->Update(table_name, nullptr, {}, transform, txn1);

  CheckExist(txn0, record_init);

  CheckExist(txn1, record_updated);

  txn_manager->Commit(txn1);
  delete txn1;

  CheckExist(txn0, record_init);

  txn_manager->Commit(txn0);
  delete txn0;

  auto *txn2 = txn_manager->Begin();

  CheckExist(txn2, record_updated);

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
//    COMMIT |        |        |
//           | R(X)   |        |
//           | COMMIT |        |
//           |        | BEGIN  |
//           |        | R(X)   |
//           |        | COMMIT |
//
// Txn #0 should only read Txn #0's version of X
// Txn #1 should only read the previous version of X because its start time is before #0's commit
// Txn #2 should only read Txn #0's version of X
TEST_F(MVCCTests, CommitDelete1) {
  // Insert the tuple to be Deleted later
  auto *txn = txn_manager->Begin();
  db->Insert(table_name, record_build, txn);
  txn_manager->Commit(txn);

  auto *txn0 = txn_manager->Begin();

  db->Delete(table_name, nullptr, {}, txn0);

  CheckNotExist(txn0);

  auto *txn1 = txn_manager->Begin();

  CheckExist(txn1, record_init);

  txn_manager->Commit(txn0);
  delete txn0;

  CheckExist(txn1, record_init);

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
//           | COMMIT |        |
//    R(X)   |        |        |
//    COMMIT |        |        |
//           |        | BEGIN  |
//           |        | R(X)   |
//           |        | COMMIT |
//
// Txn #0 should only read the previous version of X because its start time is before #1's commit
// Txn #1 should only read Txn #1's version of X
// Txn #2 should only read Txn #1's version of X
TEST_F(MVCCTests, CommitDelete2) {
  auto *txn = txn_manager->Begin();
  db->Insert(table_name, record_build, txn);
  txn_manager->Commit(txn);

  auto *txn0 = txn_manager->Begin();

  auto *txn1 = txn_manager->Begin();

  db->Delete(table_name, nullptr, {}, txn1);

  CheckExist(txn0, record_init);

  CheckNotExist(txn1);

  txn_manager->Commit(txn1);
  delete txn1;

  CheckExist(txn0, record_init);

  txn_manager->Commit(txn0);
  delete txn0;

  auto *txn2 = txn_manager->Begin();

  CheckNotExist(txn2);

  txn_manager->Commit(txn2);
  delete txn2;
}

}  // namespace thdb
