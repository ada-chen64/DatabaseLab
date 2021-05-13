# Lab 4报告

陈昱霏 2017080067

## Lab4 实验说明

本次实验对实验框架加入事务支持，并要求同学们实现简单的 MVCC 多版本并发控制机制，有效避免脏读和不可重复读现象，实验前可先复习一下数据库不同隔离级别相关概念，以及脏读、幻读、不可重复读等现象的定义。

实验前先合并 lab4 新增的实验代码，若合并出现冲突需手动解决，合并后确保不影响前 3 次实验测试的正确性。

## 框架主要变更

- 新增类：TransactionManager, Transaction
- 接口修改：
  - Instance 类的 InsertRecord, DeleteRecord, UpdateRecord, SearchRecord, GetRecord 接口增加一个传入参数 Transaction *txn，表示进行当前操作的事务，同时为了不影响前 3 次实验，为该参数添加默认值 nullptr。
  - Instance 类的 CreateTable 函数增加传入参数 useTxn，默认值为 false，表示创建 table 时是否需要考虑事务。

## Lab4 需要实现的接口

- manager
  - TransactionManager：事务管理器
    - Begin：事务开始
    - Commit：事务提交
    - Abort：事务终止
- transaction
  - Transaction：事务，自行添加需要的函数和变量

同时，还需要对 Instance 类的 InsertRecord, DeleteRecord, UpdateRecord, SearchRecord, GetRecord 接口进行修改，加入对事务的支持。

## Lab4实验思路

该实验的主要思路还是使用了参考思路所提供的方法。首先给每个事务分配一个事务号txnID，在调用Begin的时候通过TransactionManager分配。

创建Table的时候，如果参数useTxn为true，则通过修改iSchema来增加一个为INT_TYPE的列来记录事务号。

```c++
bool Instance::CreateTable(const String &sTableName, const Schema &iSchema,
                           bool useTxn) {
  Schema nSchema = iSchema;
  if(useTxn)
  {
    Column nCol(affairsCol, FieldType::INT_TYPE);
    nSchema.AddCol(nCol);
  }
  
  _pTableManager->AddTable(sTableName, nSchema);
  
  return true;
}
```

每次写的时候，比如   Insert和Update，都要修改事务号，在Insert的时候时候加事务号，Update时候对事务号进行修改。在GetRecord中要对事务号进行删除。

```c++
//Insert
if(txn != nullptr)
{
    std::vector<String> nRawVec;
    for(int i = 0; i < iRawVec.size(); i++)
    {
        nRawVec.push_back(iRawVec[i]);
    }
    nRawVec.push_back(std::to_string(txn->GetTxnID()));
    pRecord->Build(nRawVec);


}
//Update
if(txn != nullptr)
{
    Transform _iTrans(pTable->GetPos(affairsCol), FieldType::INT_TYPE, 
                      std::to_string(txn->GetTxnID()));
    nTrans.push_back(_iTrans);
}
```

访问的时候要考虑当前的事务(为了方便我称为current.txn)与record中记录的事务（record.txn）是不是同一个事务（对比事务号）。若是同一个，则可以访问，若不是还需要考虑record.txn是不是在current.txn Begin之前commit的。在TransactionManager中有一个active队列， 该队列记录了当前正在运行的事务，每次一个新的事务开始的时候TransactionManager就会将active队列存入新的事务中，也就是说这些事务是在current.txn开始时还未结束的。如果current.txn != record.txn，则先查看current.txn.active中有没有record.txn，如果有，则不能读取。但是，假设current.txn是在record.txn之前开始的，那么current.txn.active中就不会记载record.txn，所以除了active队列，TransactionManager和Transaction类还需要维护一个committed队列，记录当前已经结束的事务。如果current.txn.active中没有record.txn，以及current.txn.commited也没有record.txn，说明record.txn是在current.txn之后才开始的，不能读取。

```c++
if(txn != nullptr)
{
    std::vector<PageSlotID> tRes;
    for(auto iPair: iRes)
    {
        Record *pRecord = GetRecord(sTableName, iPair, nullptr);

        FieldID nPos = pTable->GetPos(affairsCol);
        Field *tField = pRecord->GetField(nPos);
        IntField *iField = dynamic_cast<IntField*>(tField);
        if(txn->GetTxnID() == iField->GetIntData())
        {

            tRes.push_back(iPair);
        }
        else
        {

            std::vector<TxnID> active = txn->GetActive();
            for(auto ID: active)
            {
                //current txn started before record.txn committed
                if(ID == iField->GetIntData())
                {
                    return {};
                }
            }
            std::vector<TxnID> committed = txn->GetCommitted();
            bool toAdd = false;
            for(auto ID: committed)
            {
                //the record.txn hasn't committed yet
                if(ID == iField->GetIntData())
                {
                    toAdd = true;
                    break;
                }
            }
            if(toAdd)
                tRes.push_back(iPair);
            else
                return {};
        }
    }
    return tRes;
```

如果有事务中途abort了怎么办？对我来说这是本次实验的难点。参考思路中说在Transaction维护了一个WriteRecord类的队列，但是由于Instance中的函数里传的Transaction都是const，不能对txn做更改，所以问了助教。助教建议将Transaction中的WriteRecord队列以指针的形式传给Instance，就可以直接对队列进行修改。所以我就是采用这种方法。但是后来助教想了想，觉得Instance中的Transaction用const的确不合适，所以改了，但是因为我已经写完了也就没有再该。

WriteRecord的实现方法其实就是创建一个WriteRecord类，类中记录了Table指针，Record指针，PageSlotID，以及操作类型ActionType。ActionType我记录的是该事务abort之后应该执行什么操作，而不是该事务执行了哪种写操作。所以ActionType分DELETE_TYPE和UPDATE_TYPE。Insert的时候要对事务的WriteRecord队列存入一个DELETE_TYPE指令，说明如果abort的话要将该Record删除。如果时候Update操作，则要将更新之前原Record以及UPDATE_TYPE存入WriteRecord队列。

```c++
//Insert
if(txn != nullptr)
{
    std::vector<WriteRecord*>* iWR = txn->GetWriteRecords();
    iWR->push_back(new WriteRecord(pTable, pRecord, ActionType::DELETE_TYPE, iPair));
}
//Update
if(txn != nullptr)
{
    Record *pRecord = pTable->GetRecord(iPair.first, iPair.second);
    //txn add pRecord to WriteRecord
    std::vector<WriteRecord*>* iWR = txn->GetWriteRecords();
    iWR->push_back(new WriteRecord(pTable, pRecord, ActionType::UPDATE_TYPE, iPair));
}
```

在TransactionManager调用Abort的时候要获得事务的WriteRecord队列并且执行里面的所有操作。

```c++
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
```

