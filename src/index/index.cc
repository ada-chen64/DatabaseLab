#include "index/index.h"
#include "field/field.h"
#include "page/node_page.h"

namespace thdb {
Index::Index(FieldType iType) {
  // TODO: 建立一个新的根结点，注意需要基于类型判断根结点的属性
  // TODO: 根结点需要设为中间结点
  // TODO: 注意记录RootID
  Size nKeyLen = 0;
  if(iType == FieldType::INT_TYPE)
    nKeyLen = 4;
  else if(iType == FieldType::FLOAT_TYPE)
    nKeyLen = 8;
  NodePage *rootpage = new NodePage(nKeyLen, iType, false);
  _nRootID = rootpage->GetPageID();
  delete rootpage;
}

Index::Index(PageID nPageID) {
  // TODO: 记录RootID即可
  _nRootID = nPageID;
}

Index::~Index() {
  // TODO: 如果不添加额外的指针，理论上不用额外写回内容
}

void Index::Clear() {
  // TODO: 利用RootID获得根结点
  // TODO: 利用根结点的Clear函数清除全部索引占用页面
  NodePage *rootpage = new NodePage(_nRootID);
  rootpage->Clear();
  delete rootpage;
}

PageID Index::GetRootID() const { return _nRootID; }

bool Index::Insert(Field *pKey, const PageSlotID &iPair) {
  printf("In Index Insert\n");
  // TODO: 利用RootID获得根结点
  // TODO: 利用根结点的Insert执行插入
  // TODO: 根结点满时，需要进行分裂操作，同时更新RootID
  NodePage *rootpage = new NodePage(_nRootID);
  printf("rootpage construct complete\n");
  bool success = rootpage->Insert(pKey, iPair);
  if(rootpage->Full())
  {
    std::pair<std::vector<Field *>, std::vector<PageSlotID>> newsibinfo = 
      rootpage->PopHalf();
    FieldType iType = rootpage->GetType();
    Size nKeyLen = 0;
    if(iType == FieldType::INT_TYPE)
      nKeyLen = 4;
    else if(iType == FieldType::FLOAT_TYPE)
      nKeyLen = 8;
    NodePage *newsibpage = new NodePage(nKeyLen, iType, false,
                                      newsibinfo.first, 
                                      newsibinfo.second);
    std::vector<Field*> keyvec;
    keyvec.push_back(newsibpage->FirstKey());
    std::vector<PageSlotID> childvec;
    childvec.push_back({rootpage->GetPageID(),0});
    childvec.push_back({newsibpage->GetPageID(), 0});
    NodePage *newroot = new NodePage(nKeyLen, iType, false,
                                      keyvec, childvec);
    _nRootID = newroot->GetPageID();
    delete newsibpage;
    delete newroot;
  }
  delete rootpage;
  return success;
}

Size Index::Delete(Field *pKey) {
  // ALERT:
  // 结点合并实现难度较高，没有测例，不要求实现，感兴趣的同学可自行实现并设计测例
  // TODO: 利用RootID获得根结点
  // TODO: 利用根结点的Delete执行删除
  NodePage *rootpage = new NodePage(_nRootID);
  Size amt = rootpage->Delete(pKey);
  delete rootpage;
  return amt;
}

bool Index::Delete(Field *pKey, const PageSlotID &iPair) {
  // ALERT:
  // 结点合并实现难度较高，没有测例，不要求实现，感兴趣的同学可自行实现并设计测例
  // TODO: 利用RootID获得根结点
  // TODO: 利用根结点的Delete执行删除
  NodePage *rootpage = new NodePage(_nRootID);
  bool success = rootpage->Delete(pKey, iPair);
  delete rootpage;
  return success;
}

bool Index::Update(Field *pKey, const PageSlotID &iOld,
                   const PageSlotID &iNew) {
  // TODO: 利用RootID获得根结点
  // TODO: 利用根结点的Update执行删除
  NodePage *rootpage = new NodePage(_nRootID);
  bool success = rootpage->Update(pKey, iOld, iNew);
  delete rootpage;
  return success;
}

std::vector<PageSlotID> Index::Range(Field *pLow, Field *pHigh) {
  // TODO: 利用RootID获得根结点
  // TODO: 利用根结点的Range执行范围查找
  NodePage *rootpage = new NodePage(_nRootID);
  std::vector<PageSlotID> range = rootpage->Range(pLow, pHigh);
  delete rootpage;
  return range;
}

}  // namespace thdb
