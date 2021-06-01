#ifndef WRITERECORD_H_
#define WRITERECORD_H_

#include "defines.h"
#include "record/record.h"
#include "table/table.h"


namespace thdb {
    enum class ActionType{
        DELETE_TYPE = 0,
        UPDATE_TYPE = 1,
        INSERT_TYPE = 2,
        COMMIT_TYPE = 3
    };
    class WriteRecord{
     public:
        WriteRecord(Table *iTable, Record *iRecord, 
                ActionType action, PageSlotID iPair):
            _iTable(iTable), _iRecord(iRecord), 
            _action(action), _iPair(iPair){}
        Table* _iTable;
        Record *_iRecord;
        ActionType _action;
        PageSlotID _iPair;
    };
}

#endif