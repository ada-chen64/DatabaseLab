#ifndef TRANSACTION_MANAGER_H_
#define TRANSACTION_MANAGER_H_

#include <unordered_map>

#include "defines.h"
#include "transaction/transaction.h"
#include "manager/wal_manager.h"
namespace thdb {

class TransactionManager {
 public:
  TransactionManager();
  ~TransactionManager() = default;

  Transaction *Begin();
  void Commit(Transaction *txn);
  void Abort(Transaction *txn);
  std::vector<TxnID> GetActive();
  void SetWALManager(WALManager* _walManager);
 private:
  TxnID num_transact;
  std::vector<TxnID> active;
  std::vector<TxnID> committed;
  WALManager* walManager;
  
};

}  // namespace thdb

#endif  // TRANSACTION_MANAGER_H_
