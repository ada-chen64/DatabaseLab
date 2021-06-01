#ifndef WAL_MANAGER_H_
#define WAL_MANAGER_H_
#include <stdio.h>
#include <stdlib.h>
#include "transaction/writerecord.h"
namespace thdb {

class WALManager{
 public:
  WALManager();
  ~WALManager() = default;
  void writeLog(String tableName, PageID nPageID, SlotID nSlotID,
            ActionType action, String oldRecord, String newRecord);
  void deleteLog();
  String LOGPATH = "LOG.txt";
};

}
#endif