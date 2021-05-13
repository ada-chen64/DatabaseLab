#include "table/table.h"

#include <assert.h>

#include <algorithm>
#include "exception/exceptions.h"
#include "macros.h"
#include "minios/os.h"
#include "page/record_page.h"
#include "record/fixed_record.h"
#include <fstream>
#include <iostream>

namespace thdb {

PageID NextPageID(PageID nCur) {
  LinkedPage *pPage = new LinkedPage(nCur);
  PageID nNext = pPage->GetNextID();
  delete pPage;
  return nNext;
}

Table::Table(PageID nTableID) {
  recordcount = 0;
  insertvisted=0;
  pTable = new TablePage(nTableID);

  _nHeadID = pTable->GetHeadID();
  _nTailID = pTable->GetTailID();
  _nNotFull = _nHeadID;
  NextNotFull();
}

Table::~Table() { delete pTable; }

Record *Table::GetRecord(PageID nPageID, SlotID nSlotID) {
  //return nullptr;  // 开始实验时删除此行
  // LAB1 BEGIN
  // TODO: 获得一条记录
  //printf("Table Get RECORD %d\n", nPageID);
  RecordPage *rp = new RecordPage(nPageID);
  uint8_t *rec = rp->GetRecord(nSlotID);
  FixedRecord *fr = new FixedRecord(pTable->GetFieldSize(), pTable->GetTypeVec(),
    pTable->GetSizeVec());
  fr->Load(rec);
  
  delete rp;
  delete[] rec;
  return fr;
  // TIPS: 利用RecordPage::GetRecord获取无格式记录数据
  // TIPS: 利用TablePage::GetFieldSize, GetTyepVec,
  // GetSizeVec三个函数可以构建空的FixedRecord对象 TIPS:
  // 利用Record::Load导入数据 ALERT: 需要注意析构所有不会返回的内容
  // LAB1 END
}

PageSlotID Table::InsertRecord(Record *pRecord) {
  // LAB1 BEGIN
  // TODO: 插入一条记录
  
  insertvisted++;
  //printf("insert visited %d times\n", insertvisted);
  PageID pid = _nNotFull;
  RecordPage *page = new RecordPage(pid);
  uint8_t * rec = new uint8_t[PAGE_SIZE];
  pRecord->Store(rec);
  
  SlotID slot = -1;
  slot = page->InsertRecord(rec);
  if(slot != -1)
    recordcount++;
  Record *record = GetRecord(pid, slot);
  
  if(page->Full())
  { 
    delete page;
    //printf("page %u full\n", _nNotFull); 
    NextNotFull();
  }
  else
    delete page;
  delete[] rec;
  delete record;


  //printf("insert rec done \n");
  return std::pair<PageID, SlotID>(pid, slot);
  // TIPS: 利用_nNotFull来获取有空间的页面
  // TIPS: 利用Record::Store获得序列化数据
  // TIPS: 利用RecordPage::InsertRecord插入数据
  // TIPS: 注意页满时更新_nNotFull
  // LAB1 END
}

void Table::DeleteRecord(PageID nPageID, SlotID nSlotID) {
  // LAB1 BEGIN
  // TIPS: 利用RecordPage::DeleteRecord插入数据
  //printf("delete record\n");
  RecordPage *rp = new RecordPage(nPageID);
  rp->DeleteRecord(nSlotID);
  _nNotFull = nPageID;
  delete rp;
  // TIPS: 注意更新_nNotFull来保证较高的页面空间利用效率 ???
  // LAB1 END
}

void Table::UpdateRecord(PageID nPageID, SlotID nSlotID,
                         const std::vector<Transform> &iTrans) {
  // LAB1 BEGIN
  // TIPS: 仿照InsertRecord从无格式数据导入原始记录
  //printf("in update rec\n");

  RecordPage *page = new RecordPage(nPageID);
  // TIPS: 构建Record对象，利用Record::SetField更新Record对象
  Record *record = GetRecord(nPageID, nSlotID);
  uint8_t * rec = new uint8_t[PAGE_SIZE];
  // printf("record: ");
  // printf("%s\n",record->ToString().c_str());
  // printf("itranssize: %d\n", iTrans.size());
  for(int i = 0; i < iTrans.size(); i++)
  {
    FieldID pos = iTrans[i].GetPos();
    Field *_ifield = iTrans[i].GetField();
    //printf("pos %d field %s\n", pos, _ifield->ToString().c_str());
    record->SetField(pos, _ifield);
  }
  
  record->Store(rec);
  page->UpdateRecord(nSlotID, rec);
  delete page;
  delete record;
  // TIPS: Trasform::GetPos表示更新位置，GetField表示更新后的字段
  // TIPS: 将新的记录序列化
  // TIPS: 利用RecordPage::UpdateRecord更新一条数据
  // LAB1 END
}

std::vector<PageSlotID> Table::SearchRecord(Condition *pCond) {
  
  // LAB1 BEGIN
  // TODO: 对记录的条件检索
  // TIPS: 仿照InsertRecord从无格式数据导入原始记录
  //printf("Condition type = %d\n", pCond->GetType());
  //printf("in search. table inserted %d times\n table successfully inserted %d times\n", insertvisted, recordcount);
  std::vector<PageSlotID> matches;
  PageID nBegin = _nHeadID;
  int totalrecords = 0;
  while(nBegin != NULL_PAGE)
  {
    RecordPage *page = new RecordPage(nBegin);
    //printf("visit page: %u insert %d times\n", page->GetPageID(), page->visitInsert);
    for(SlotID i = 0; i < page->GetCap(); i++)
    {
      
      if(page->HasRecord(i))
      {
        totalrecords++;
        uint8_t * rec = page->GetRecord(i);
        Record *record = EmptyRecord();
        record->Load(rec);
        if(!pCond || pCond->Match(*record))
        {
          matches.push_back(std::pair<PageID,SlotID>(nBegin, i));
        }
        delete[] rec;
        delete record;
    
      }
      // else
      // {
      //   printf("miss page %u slot %u\n", page->GetPageID(), i);
      // }
    }
    
    nBegin = page->GetNextID();
    //printf("next page: %u\n", nBegin);
    delete page;
    
  }
  //printf("total has records %d \n", totalrecords);
  //printf("matches: %d \n", matches.size());
  return matches;
  // TIPS: 依次导入各条记录进行条件判断
  // TIPS: Condition的抽象方法Match可以判断Record是否满足检索条件
  // TIPS: 返回所有符合条件的结果的pair<PageID,SlotID>
  // LAB1 END
}

void Table::SearchRecord(std::vector<PageSlotID> &iPairs, Condition *pCond) {
  if (!pCond) return;
  auto it = iPairs.begin();
  while (it != iPairs.end()) {
    Record *pRecord = GetRecord(it->first, it->second);
    if (!pCond->Match(*pRecord)) {
      it = iPairs.erase(it);
    } else
      ++it;
    delete pRecord;
  }
}

void Table::Clear() {
  PageID nBegin = _nHeadID;
  while (nBegin != NULL_PAGE) {
    PageID nTemp = nBegin;
    nBegin = NextPageID(nBegin);
    MiniOS::GetOS()->DeletePage(nTemp);
  }
}

void Table::NextNotFull() {
  // LAB1 BEGIN
  // TODO: 实现一个快速查找非满记录页面的算法
  //printf("in NextNotFUll\n");
  PageID pid = _nHeadID;
  //printf("headID: %u\n", pid);
  while (pid != NULL_PAGE) {
    RecordPage *rp = new RecordPage(pid);
    if(!rp->Full())
    {
      _nNotFull = pid;
      //printf("not full %u\n",_nNotFull);
      delete rp;
      return;
    }
    pid = rp->GetNextID();
    //printf("nextID: %u\n", pid);
    delete rp;
  }
  //pages full, create new page
  //printf("tailID: %u\n", _nTailID);
  RecordPage *tailpage = new RecordPage(_nTailID);
  RecordPage *newpage = new RecordPage(pTable->GetTotalSize(), true);
  tailpage->PushBack(newpage);
  _nTailID = newpage->GetPageID();
  _nNotFull = newpage->GetPageID();
  //printf("completely full. new page: %u\n", _nTailID);
  delete tailpage;
  delete newpage;
  

  // ALERT: ！！！一定要注意！！！
  // 不要同时建立两个指向相同磁盘位置的且可变对象，否则会出现一致性问题
  // ALERT: 可以适当增加传入参数，本接口不会被外部函数调用，例如额外传入Page
  // *指针
  // TIPS:
  // 充分利用链表性质，注意全满时需要在结尾_pTable->GetTailID对应结点后插入新的结点，并更新_pTable的TailID
  // TIPS: 只需要保证均摊复杂度较低即可
  // LAB1 END
}

FieldID Table::GetPos(const String &sCol) const { return pTable->GetPos(sCol); }

FieldType Table::GetType(const String &sCol) const {
  return pTable->GetType(sCol);
}

Size Table::GetSize(const String &sCol) const { return pTable->GetSize(sCol); }

Record *Table::EmptyRecord() const {
  FixedRecord *pRecord = new FixedRecord(
      pTable->GetFieldSize(), pTable->GetTypeVec(), pTable->GetSizeVec());
  return pRecord;
}

bool CmpByFieldID(const std::pair<String, FieldID> &a,
                  const std::pair<String, FieldID> &b) {
  return a.second < b.second;
}

std::vector<String> Table::GetColumnNames() const {
  std::vector<String> iVec{};
  std::vector<std::pair<String, FieldID>> iPairVec(pTable->_iColMap.begin(),
                                                   pTable->_iColMap.end());
  std::sort(iPairVec.begin(), iPairVec.end(), CmpByFieldID);
  for (const auto &it : iPairVec) iVec.push_back(it.first);
  return iVec;
}


}  // namespace thdb
