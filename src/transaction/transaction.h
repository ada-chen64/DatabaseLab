#ifndef TRANSACTION_H_
#define TRANSACTION_H_

#include "defines.h"
#include "record/record.h"
#include "transaction/writerecord.h"

namespace thdb {

class Transaction {
 public:
  explicit Transaction(TxnID txn_id);
  ~Transaction() = default;
  TxnID GetTxnID() const;
  void AddWriteRecord(Record* iRecord);
  void ClearWriteRecords();
  std::vector<WriteRecord*>* GetWriteRecords() const;
  void SetActive(std::vector<TxnID>);
  void SetCommit(std::vector<TxnID>);
  std::vector<TxnID> GetActive() const;
  std::vector<TxnID> GetCommitted() const;

 private:
  TxnID _txn_id;
  std::vector<TxnID> active;
  std::vector<TxnID> committed;
  std::vector<WriteRecord*> *_iWR;
};

}  // namespace thdb

#endif  // TRANSACTION_H_
