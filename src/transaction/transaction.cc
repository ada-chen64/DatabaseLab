#include "transaction/transaction.h"

namespace thdb {

Transaction::Transaction(TxnID txn_id) : _txn_id(txn_id) {
    _iWR = new std::vector<WriteRecord*>;
}

TxnID Transaction::GetTxnID() const{
    return _txn_id;
}

void Transaction::SetActive(std::vector<TxnID> _active){
    
    for(TxnID txnID: _active)
    {
        active.push_back(txnID);
    }
}
void Transaction::SetCommit(std::vector<TxnID> _committed){
    for(TxnID txnID: _committed)
    {
        committed.push_back(txnID);
    }
}
std::vector<TxnID> Transaction::GetActive() const{
    return active;
}
std::vector<TxnID> Transaction::GetCommitted() const{
    return committed;
}
std::vector<WriteRecord*>* Transaction::GetWriteRecords() const{
    return _iWR;
}
}  // namespace thdb
