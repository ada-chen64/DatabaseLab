#include "system/instance.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <iostream>

#include "condition/conditions.h"
#include "exception/exceptions.h"
#include "manager/table_manager.h"
#include "manager/wal_manager.h"
#include "record/fixed_record.h"
#include "record/record.h"
#include "field/fields.h"

namespace thdb {

Instance::Instance() {
  _pTableManager = new TableManager();
  _pIndexManager = new IndexManager();
  _pTransactionManager = new TransactionManager();
  _pWALManager = new WALManager();
  _pRecoveryManager = new RecoveryManager(_pTransactionManager, _pTableManager,
    _pWALManager);
  _pTransactionManager->SetWALManager(_pWALManager);
}

Instance::~Instance() {
  delete _pTableManager;
  delete _pIndexManager;
  delete _pTransactionManager;
  delete _pRecoveryManager;
}

Table *Instance::GetTable(const String &sTableName) const {
  return _pTableManager->GetTable(sTableName);
}

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

bool Instance::DropTable(const String &sTableName) {
  for (const auto &sColName : _pIndexManager->GetTableIndexes(sTableName))
    _pIndexManager->DropIndex(sTableName, sColName);
  _pTableManager->DropTable(sTableName);
  return true;
}

FieldID Instance::GetColID(const String &sTableName,
                           const String &sColName) const {
  Table *pTable = GetTable(sTableName);
  if (pTable == nullptr) throw TableException();
  return pTable->GetPos(sColName);
}

FieldType Instance::GetColType(const String &sTableName,
                               const String &sColName) const {
  Table *pTable = GetTable(sTableName);
  if (pTable == nullptr) throw TableException();
  return pTable->GetType(sColName);
}

Size Instance::GetColSize(const String &sTableName,
                          const String &sColName) const {
  Table *pTable = GetTable(sTableName);
  if (pTable == nullptr) throw TableException();
  return pTable->GetSize(sColName);
}

bool CmpPageSlotID(const PageSlotID &iA, const PageSlotID &iB) {
  if (iA.first == iB.first) return iA.second < iB.second;
  return iA.first < iB.first;
}

std::vector<PageSlotID> Intersection(std::vector<PageSlotID> iA,
                                     std::vector<PageSlotID> iB) {
  std::sort(iA.begin(), iA.end(), CmpPageSlotID);
  std::sort(iB.begin(), iB.end(), CmpPageSlotID);
  std::vector<PageSlotID> iRes{};
  std::set_intersection(iA.begin(), iA.end(), iB.begin(), iB.end(),
                        std::back_inserter(iRes));
  return iRes;
}

std::vector<PageSlotID> Instance::Search(
    const String &sTableName, Condition *pCond,
    const std::vector<Condition *> &iIndexCond, 
    const Transaction *txn) {
  Table *pTable = GetTable(sTableName);
  if (pTable == nullptr) throw TableException();
  std::vector<PageSlotID> iRes;
  if (iIndexCond.size() > 0) {
    IndexCondition *pIndexCond = dynamic_cast<IndexCondition *>(iIndexCond[0]);
    assert(pIndexCond != nullptr);
    auto iName = pIndexCond->GetIndexName();
    auto iRange = pIndexCond->GetIndexRange();
    iRes =
        GetIndex(iName.first, iName.second)->Range(iRange.first, iRange.second);
    for (Size i = 1; i < iIndexCond.size(); ++i) {
      IndexCondition *pIndexCond =
          dynamic_cast<IndexCondition *>(iIndexCond[i]);
      auto iName = pIndexCond->GetIndexName();
      auto iRange = pIndexCond->GetIndexRange();
      iRes = Intersection(iRes, GetIndex(iName.first, iName.second)
                                    ->Range(iRange.first, iRange.second));
    }
    
  } 
  else
    iRes = pTable->SearchRecord(pCond);
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
  }

  return iRes;
}

PageSlotID Instance::Insert(const String &sTableName,
                            const std::vector<String> &iRawVec,
                            const Transaction *txn) {
  Table *pTable = GetTable(sTableName);
  if (pTable == nullptr) throw TableException();
  Record *pRecord = pTable->EmptyRecord();
  
  
  //printf("before joining Record to string, %s\n", pRecord->ToString().c_str());
  if(txn != nullptr)
  {
    std::vector<String> nRawVec;
    for(int i = 0; i < iRawVec.size(); i++)
    {
      nRawVec.push_back(iRawVec[i]);
    }
    //printf("txn ID: %s\n",std::to_string(txn->GetTxnID()).c_str());
    nRawVec.push_back(std::to_string(txn->GetTxnID()));
    pRecord->Build(nRawVec);


  }
  else
  {
    pRecord->Build(iRawVec);
  }
  

  PageSlotID iPair = pTable->InsertRecord(pRecord);
  _pWALManager->writeLog(sTableName, iPair.first, iPair.second, 
      ActionType::INSERT_TYPE, "", pRecord->ToString());
  if(txn != nullptr)
  {
    std::vector<WriteRecord*>* iWR = txn->GetWriteRecords();
    iWR->push_back(new WriteRecord(pTable, pRecord, ActionType::DELETE_TYPE, iPair));
  }
  // Handle Insert on Index
  if (_pIndexManager->HasIndex(sTableName)) {
    auto iColNames = _pIndexManager->GetTableIndexes(sTableName);
    for (const auto &sCol : iColNames) {
      FieldID nPos = pTable->GetPos(sCol);
      Field *pKey = pRecord->GetField(nPos);
      _pIndexManager->GetIndex(sTableName, sCol)->Insert(pKey, iPair);
    }
  }
  
  delete pRecord;
  return iPair;
}

uint32_t Instance::Delete(const String &sTableName, Condition *pCond,
                          const std::vector<Condition *> &iIndexCond,
                          const Transaction *txn) {
  auto iResVec = Search(sTableName, pCond, iIndexCond, txn);

  Table *pTable = GetTable(sTableName);
  bool bHasIndex = _pIndexManager->HasIndex(sTableName);
  for (const auto &iPair : iResVec) {
    // Handle Delete on Index
    if (bHasIndex) {
      Record *pRecord = pTable->GetRecord(iPair.first, iPair.second);
      auto iColNames = _pIndexManager->GetTableIndexes(sTableName);
      for (const auto &sCol : iColNames) {
        FieldID nPos = pTable->GetPos(sCol);
        Field *pKey = pRecord->GetField(nPos);
        _pIndexManager->GetIndex(sTableName, sCol)->Delete(pKey, iPair);
      }
      delete pRecord;
    }
    Record *pRecord = pTable->GetRecord(iPair.first, iPair.second);
    _pWALManager->writeLog(sTableName,iPair.first, iPair.second,
        ActionType::DELETE_TYPE, pRecord->ToString(), "");
    pTable->DeleteRecord(iPair.first, iPair.second);
    
  }
  return iResVec.size();
}

uint32_t Instance::Update(const String &sTableName, Condition *pCond,
                          const std::vector<Condition *> &iIndexCond,
                          const std::vector<Transform> &iTrans,
                          const Transaction *txn) {
  auto iResVec = Search(sTableName, pCond, iIndexCond, txn);

  Table *pTable = GetTable(sTableName);
  bool bHasIndex = _pIndexManager->HasIndex(sTableName);
  for (const auto &iPair : iResVec) {
    // Handle Delete on Index
    if (bHasIndex) {
      Record *pRecord = pTable->GetRecord(iPair.first, iPair.second);
      auto iColNames = _pIndexManager->GetTableIndexes(sTableName);
      for (const auto &sCol : iColNames) {
        FieldID nPos = pTable->GetPos(sCol);
        Field *pKey = pRecord->GetField(nPos);
        _pIndexManager->GetIndex(sTableName, sCol)->Delete(pKey, iPair);
      }
      delete pRecord;
    }
    std::vector<Transform> nTrans = iTrans;
    if(txn != nullptr)
    {
      Record *pRecord = pTable->GetRecord(iPair.first, iPair.second);
      //txn add pRecord to WriteRecord
      std::vector<WriteRecord*>* iWR = txn->GetWriteRecords();
      iWR->push_back(new WriteRecord(pTable, pRecord, ActionType::UPDATE_TYPE, iPair));
      Transform _iTrans(pTable->GetPos(affairsCol), FieldType::INT_TYPE, 
            std::to_string(txn->GetTxnID()));
      nTrans.push_back(_iTrans);
    }
    pTable->UpdateRecord(iPair.first, iPair.second, nTrans);
    
    // Handle Delete on Index
    if (bHasIndex) {
      Record *pRecord = pTable->GetRecord(iPair.first, iPair.second);
      auto iColNames = _pIndexManager->GetTableIndexes(sTableName);
      for (const auto &sCol : iColNames) {
        FieldID nPos = pTable->GetPos(sCol);
        Field *pKey = pRecord->GetField(nPos);
        _pIndexManager->GetIndex(sTableName, sCol)->Insert(pKey, iPair);
      }
      delete pRecord;
    }
  }
  return iResVec.size();
}

Record *Instance::GetRecord(const String &sTableName, const PageSlotID &iPair,
                            const Transaction *txn) const {
  Table *pTable = GetTable(sTableName);
  Record *pRecord = pTable->GetRecord(iPair.first, iPair.second);
  
  if(txn != nullptr)
  {
    FieldID iPos = pTable->GetPos(affairsCol);
    
    pRecord->Remove(iPos);
    
  }
  return pRecord;
}

std::vector<Record *> Instance::GetTableInfos(const String &sTableName) const {
  std::vector<Record *> iVec{};
  for (const auto &sName : GetColumnNames(sTableName)) {
    FixedRecord *pDesc = new FixedRecord(
        3,
        {FieldType::STRING_TYPE, FieldType::STRING_TYPE, FieldType::INT_TYPE},
        {COLUMN_NAME_SIZE, 10, 4});
    pDesc->SetField(0, new StringField(sName));
    pDesc->SetField(1,
                    new StringField(toString(GetColType(sTableName, sName))));
    pDesc->SetField(2, new IntField(GetColSize(sTableName, sName)));
    iVec.push_back(pDesc);
  }
  return iVec;
}
std::vector<String> Instance::GetTableNames() const {
  return _pTableManager->GetTableNames();
}
std::vector<String> Instance::GetColumnNames(const String &sTableName) const {
  return _pTableManager->GetColumnNames(sTableName);
}

bool Instance::IsIndex(const String &sTableName, const String &sColName) const {
  return _pIndexManager->IsIndex(sTableName, sColName);
}

Index *Instance::GetIndex(const String &sTableName,
                          const String &sColName) const {
  return _pIndexManager->GetIndex(sTableName, sColName);
}

std::vector<Record *> Instance::GetIndexInfos() const {
  std::vector<Record *> iVec{};
  for (const auto &iPair : _pIndexManager->GetIndexInfos()) {
    FixedRecord *pInfo =
        new FixedRecord(4,
                        {FieldType::STRING_TYPE, FieldType::STRING_TYPE,
                         FieldType::STRING_TYPE, FieldType::INT_TYPE},
                        {TABLE_NAME_SIZE, COLUMN_NAME_SIZE, 10, 4});
    pInfo->SetField(0, new StringField(iPair.first));
    pInfo->SetField(1, new StringField(iPair.second));
    pInfo->SetField(
        2, new StringField(toString(GetColType(iPair.first, iPair.second))));
    pInfo->SetField(3, new IntField(GetColSize(iPair.first, iPair.second)));
    iVec.push_back(pInfo);
  }
  return iVec;
}

bool Instance::CreateIndex(const String &sTableName, const String &sColName,
                           FieldType iType) {
  auto iAll = Search(sTableName, nullptr, {});
  _pIndexManager->AddIndex(sTableName, sColName, iType);
  Table *pTable = GetTable(sTableName);
  // Handle Exists Data
  for (const auto &iPair : iAll) {
    FieldID nPos = pTable->GetPos(sColName);
    Record *pRecord = pTable->GetRecord(iPair.first, iPair.second);
    Field *pKey = pRecord->GetField(nPos);
    _pIndexManager->GetIndex(sTableName, sColName)->Insert(pKey, iPair);
    delete pRecord;
  }
  return true;
}

bool Instance::DropIndex(const String &sTableName, const String &sColName) {
  auto iAll = Search(sTableName, nullptr, {});
  Table *pTable = GetTable(sTableName);
  for (const auto &iPair : iAll) {
    FieldID nPos = pTable->GetPos(sColName);
    Record *pRecord = pTable->GetRecord(iPair.first, iPair.second);
    Field *pKey = pRecord->GetField(nPos);
    _pIndexManager->GetIndex(sTableName, sColName)->Delete(pKey, iPair);
    delete pRecord;
  }
  _pIndexManager->DropIndex(sTableName, sColName);
  return true;
}

std::pair<std::vector<String>, std::vector<Record *>> Instance::Join(
    std::map<String, std::vector<PageSlotID>> &iResultMap,
    std::vector<Condition *> &iJoinConds) {
  // LAB3 BEGIN
  // TODO:实现正确且高效的表之间JOIN过程

  // ALERT:由于实现临时表存储具有一定难度，所以允许JOIN过程中将中间结果保留在内存中，不需要存入临时表
  // ALERT:一定要注意，存在JOIN字段值相同的情况，需要特别重视
  // ALERT:针对于不同的JOIN情况（此处只需要考虑数据量和是否为索引列），可以选择使用不同的JOIN算法
  // ALERT:JOIN前已经经过了Filter过程
  // ALERT:建议不要使用不经过优化的NestedLoopJoin算法
  // TIPS:JoinCondition中保存了JOIN两方的表名和列名
  // TIPS:利用GetTable(TableName)的方式可以获得Table*指针，之后利用lab1中的Table::GetRecord获得初始Record*数据
  // TIPs:利用Table::GetColumnNames可以获得Table初始的列名，与初始Record*顺序一致
  // TIPS:Record对象添加了Copy,Sub,Add,Remove函数，方便同学们对于Record进行处理
  // TIPS:利用GetColID/Type/Size三个函数可以基于表名和列名获得列的信息
  // TIPS:利用IsIndex可以判断列是否存在索引
  // TIPS:利用GetIndex可以获得索引Index*指针
  //determining the order which tables should get iterated
  /*std::map<int,int> tableorder;
  int i = 0;
  for(auto it = iResultMap.begin(); it != iResultMap.end(); it++)
  {
    Size tablesize = it->second.size();
    tableorder[tablesize] = i;
    i++;
  }
  std::vector<int> order;
  for(auto it = tableorder.begin(); it != tableorder.end(); it++)
  {
    order.push_back(it->second);
  }*/
  //std::pair<std::vector<String>, std::vector<Record *>> result;
  std::vector<String> totcolNames;
  std::vector<Record *> newTable;
  //printf("JOIN CONDITIONS: %d\n", iJoinConds.size());
  JoinCondition *joinCond = dynamic_cast<JoinCondition*> (iJoinConds[0]);
  String sTableA = joinCond->sTableA;//outer
  String sTableB = joinCond->sTableB; //inner
  String sColA = joinCond->sColA;
  String sColB = joinCond->sColB;
  FieldID iColA = GetColID(sTableA, sColA);
  FieldID iColB = GetColID(sTableB, sColB);
  Table *tableA = GetTable(sTableA);
  Table *tableB = GetTable(sTableB);
  std::vector<String> aColNames = tableA->GetColumnNames();
  // for(int i = 0; i < aColNames.size(); i++)
  // {
  //   if(sColA.compare(aColNames[i]) == 0)
  //   {
  //     printf("%s in table %s\n", sColA.c_str(), sTableA.c_str());
  //   }
  // }
  std::vector<String> bColNames = tableB->GetColumnNames();
  totcolNames.insert(totcolNames.begin(), bColNames.begin(), bColNames.end());
  totcolNames.insert(totcolNames.begin(), aColNames.begin(), aColNames.end());
  /*nested loop begin
  for(auto iPairA: iResultMap[sTableA])
  {
    Record *aRecord = GetRecord(sTableA, iPairA);
    Field *aField = aRecord->GetField(iColA);
    FieldType aType = aField->GetType();
    for(auto iPairB: iResultMap[sTableB])
    {
      Record *bRecord = GetRecord(sTableB, iPairB);
      Field *bField = bRecord->GetField(iColB);
      FieldType bType = bField->GetType();
      assert(bType == aType);
      //printf("btype and aType same\n");
      if(Equal(aField, bField, aType))
      {
        //printf("bField == aField\n");
        //satisfies join cond
        Record *newRec = aRecord->Copy();
        newRec->Add(bRecord);
        newTable.push_back(newRec);
      }
    }
  }
  nested loop end*/
  // heap join
  std::map<String, std::vector<Record*>> hashmap;
  for(auto iPair: iResultMap[sTableA])
  {
    Record *aRecord = GetRecord(sTableA, iPair);
    Field *aField = aRecord->GetField(iColA);
    if(hashmap.find(aField->ToString()) == hashmap.end())
    {
      hashmap[aField->ToString()] = {aRecord};
    }
    else
      hashmap[aField->ToString()].push_back(aRecord);
    
  }
  for(auto iPair: iResultMap[sTableB])
  {
    Record *bRecord = GetRecord(sTableB, iPair);
    Field *bField = bRecord->GetField(iColA);
    std::vector<Record*> aRecords = hashmap[bField->ToString()];
    for(auto record: aRecords)
    {
      Record *newRec = record->Copy();
      newRec->Add(bRecord);
      newTable.push_back(newRec);
    }
  }
  return {totcolNames, newTable};

  // EXTRA:JOIN的表的数量超过2时，所以需要先计算一个JOIN执行计划（不要求复杂算法）,有兴趣的同学可以自行实现
  // EXTRA:在多表JOIN时，可以采用并查集或执行树来确定执行JOIN的数据内容

  // LAB3 END
}

}  // namespace thdb
