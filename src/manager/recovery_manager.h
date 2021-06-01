#ifndef RECOVERY_MANAGER_H_
#define RECOVERY_MANAGER_H_
#include "manager/table_manager.h"
#include "manager/transaction_manager.h"
#include "manager/wal_manager.h"
namespace thdb {

class RecoveryManager {
 public:
  RecoveryManager(TransactionManager* _transactionManager,
    TableManager* _tableManager, WALManager* _walManager);

  ~RecoveryManager() = default;

  void Redo();
  void Undo();

 private:
  TransactionManager* transactionManager;
  TableManager* tableManager;
  WALManager* walManager;
  int actionnum = 0;
  int tablename = 1;
  int pagenum = 2;
  int slotnum = 3;
  int oldrec = 4;
  int newrec = 5;
};

}  // namespace thdb

#endif  // RECOVERY_MANAGER_H_
