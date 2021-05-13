#include "manager/transaction_manager.h"
#include "system/instance.h"
namespace thdb {
TransactionManager::TransactionManager(){
    num_transact = 0;
}

Transaction *TransactionManager::Begin() { 
    Transaction *newtransact = new Transaction(num_transact);
    active.push_back(num_transact);
    newtransact->SetActive(active);
    newtransact->SetCommit(committed);
    num_transact++;
    return newtransact; 
}

void TransactionManager::Commit(Transaction *txn) {
    committed.push_back(txn->GetTxnID());
    for(int i = 0; i < active.size(); i++)
    {
        if(active[i] == txn->GetTxnID())
        {
            active.erase(active.begin() + i);
            break;
        }
    } 

    return; 
}

void TransactionManager::Abort(Transaction *txn) { 
    std::vector<WriteRecord*> *_iWR = txn->GetWriteRecords();
    auto it = _iWR->begin();
    for(;it != _iWR->end(); it++)
    {
        WriteRecord *wr = *it;
        Table *pTable = wr->_iTable;
        if(wr->_action == ActionType::DELETE_TYPE)
        {
            pTable->DeleteRecord(wr->_iPair.first, wr->_iPair.second);
        }
        else if(wr->_action == ActionType::UPDATE_TYPE)
        {
            std::vector<Transform> iTrans;
            Record *iRec = wr->_iRecord;
            for(FieldID i = 0; i < iRec->GetSize(); i++)
            {
                Transform trans(i, iRec->GetField(i)->GetType(), 
                    iRec->GetField(i)->ToString());
                iTrans.push_back(trans);
            }
            pTable->UpdateRecord(wr->_iPair.first, wr->_iPair.second,iTrans);
        }
    }
    committed.push_back(txn->GetTxnID());
    for(int i = 0; i < active.size(); i++)
    {
        if(active[i] == txn->GetTxnID())
        {
            active.erase(active.begin() + i);
            break;
        }
    } 
    return; 
}

std::vector<TxnID> TransactionManager::GetActive(){
    return active;
}



}  // namespace thdb
