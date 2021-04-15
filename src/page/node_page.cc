#include "page/node_page.h"

#include <assert.h>
#include <float.h>

#include "exception/exceptions.h"
#include "field/fields.h"
#include "macros.h"
#include "minios/os.h"

namespace thdb {

// const PageOffset NEXT_PAGE_OFFSET = 4;
// const PageOffset PREV_PAGE_OFFSET = 8;
const PageOffset LEAF_OFFSET = 8;
const PageOffset USED_SLOT_OFFSET = 12;
const PageOffset KEY_LEN_OFFSET = 16;
const PageOffset KEY_TYPE_OFFSET = 20;
const PageOffset NULL_KEY_OFFSET = 24;

NodePage::NodePage(Size nKeyLen, FieldType iKeyType, bool bLeaf)
    : _nKeyLen(nKeyLen), _iKeyType(iKeyType), _bLeaf(bLeaf){
  SetHeader((uint8_t*)&_bLeaf, 4, LEAF_OFFSET);
  SetHeader((uint8_t*)&_nKeyLen, 4, KEY_LEN_OFFSET);
  SetHeader((uint8_t*)&_iKeyType, 4, KEY_TYPE_OFFSET);
  //printf("Initialize NodePage\n");
  // TODO: 基于自己实现的Store算法确定最大容量
  if(_bLeaf)
  {
      _nCap = (DATA_SIZE) / (_nKeyLen + 6)+1;
      _nNullKey = false;
  }
  else
  {
    _nCap = (DATA_SIZE - 4) / (_nKeyLen + 4)+1;
    InitFirst();
  }  
  // TODO: 如果为中间结点，注意可能需要初始化第一个子结点
}

NodePage::NodePage(Size nKeyLen, FieldType iKeyType, bool bLeaf,
                   const std::vector<Field *> &iKeyVec,
                   const std::vector<PageSlotID> &iChildVec)
    : _nKeyLen(nKeyLen),
      _iKeyType(iKeyType),
      _bLeaf(bLeaf),
      _iKeyVec(iKeyVec),
      _iChildVec(iChildVec){
  // TODO: 基于自己实现的Store算法确定最大容量
  // SetHeader((uint8_t*)&_nNextID, 4, NEXT_PAGE_OFFSET);
  // SetHeader((uint8_t*)&_nPrevID, 4,PREV_PAGE_OFFSET);
  SetHeader((uint8_t*)&_bLeaf, 4, LEAF_OFFSET);
  SetHeader((uint8_t*)&_nKeyLen, 4, KEY_LEN_OFFSET);
  SetHeader((uint8_t*)&_iKeyType, 4, KEY_TYPE_OFFSET);
  if(_bLeaf)
  {
    _nCap = (DATA_SIZE) / (_nKeyLen + 6)+1;
    _nNullKey = false;
  }
  else
  {
    _nCap = (DATA_SIZE - 4) / (_nKeyLen + 4)+1;
    _nNullKey = false;

  }
}

NodePage::NodePage(PageID nPageID) : Page(nPageID) {
  // TODO: 从格式化页面中导入结点信息
  // GetHeader((uint8_t*)&_nNextID, 4, NEXT_PAGE_OFFSET);
  // GetHeader((uint8_t*)&_nPrevID, 4,PREV_PAGE_OFFSET);
  GetHeader((uint8_t*)&_bLeaf, 4, LEAF_OFFSET);
  GetHeader((uint8_t*)&_nKeyLen, 4, KEY_LEN_OFFSET);
  GetHeader((uint8_t*)&_iKeyType, 4, KEY_TYPE_OFFSET);
  GetHeader((uint8_t*)&_nNullKey,4, NULL_KEY_OFFSET);
  // TODO: 确定最大容量
  //printf("PAGE CONSTRUCT BASED ON ID. _bLeaf: %d\n",_bLeaf);
  if(_bLeaf)
  {
    _nCap = (DATA_SIZE) / (_nKeyLen + 6)+1;
  }
  else
  {
    _nCap = (DATA_SIZE - 4) / (_nKeyLen + 4)+1;
  }
  Load();
}

NodePage::~NodePage() {
  // TODO: 将结点信息格式化并写回到页面中
  Size used_slots = _iKeyVec.size();
  SetHeader((uint8_t*)&used_slots, 4, USED_SLOT_OFFSET);
  SetHeader((uint8_t*)&_nNullKey, 4, NULL_KEY_OFFSET);
  Store();
  // TODO: 注意析构KeyVec中的指针
  //printf("store complete\n");
}

bool NodePage::Insert(Field *pKey, const PageSlotID &iPair) {
  // TODO: 需要基于结点类型判断执行过程
  // 叶结点：
  // 1.确定插入位置后插入数据即可
  //printf("In NodePage Insert pKey: %s\n", pKey->ToString().c_str());
  //printf("_bLeaf = %d _nNullKey=%d\n", _bLeaf, _nNullKey);
  if(_bLeaf)
  {
    if(_iKeyVec.size() >= _nCap)
      return false;
    Size pos = LowerBound(pKey);
    //printf("Leaf Keyvec size = %d\tpos=%u\n", _iKeyVec.size(), pos);
    //printf("_iChildVec.size() %d\n",_iChildVec.size());
    while(pos != _iChildVec.size())
    {
      //printf("in while\n");
      if(Equal(_iKeyVec[pos], pKey, _iKeyType)&&
        iPair >= _iChildVec[pos])
      {
        
        _iKeyVec.insert(_iKeyVec.begin() + pos, pKey);
        _iChildVec.insert(_iChildVec.begin() + pos, iPair);
        //printf("Equal. _iKeysize=%d\n", _iKeyVec.size());
        return true;
      }
      else if(Greater(_iKeyVec[pos], pKey, _iKeyType))
      {
        _iKeyVec.insert(_iKeyVec.begin() + pos, pKey);
        _iChildVec.insert(_iChildVec.begin() + pos, iPair);
        //printf("Greater. _iKeysize=%d\n", _iKeyVec.size());
        //printf("iPair.first: %u, iPair.second:%u\n", iPair.first, iPair.second);
        return true;
      }
      pos++;
    }
    //printf("After while\n");
    _iKeyVec.push_back(pKey);
    _iChildVec.push_back(iPair);
    //printf("returning from leafnode insert\n");
    return true;
  }
  // 中间结点:
  // 1.确定执行插入函数的子结点
  // 2.对应的子结点执行插入函数
  // 3.判断子结点是否为满结点，满结点时执行分裂
  // 4.子结点分裂情况下需要更新KeyVec和ChildVec
  
  if(_nNullKey)
  {
    //if this is an empty node
    _iKeyVec.pop_back();
    _iKeyVec.push_back(pKey);
    _nNullKey = false;
    NodePage *childpage = new NodePage(_iChildVec[1].first);
    childpage->Insert(pKey, iPair);
    delete(childpage);
    return true;
  }
  // printf("KeyVec size %d\n",_iKeyVec.size());
  // printf("ChildVec size %d\n", _iChildVec.size());
  // printf("Key %s\n", _iKeyVec[0]->ToString().c_str());
  Size pg = LowerBound(pKey);
  // printf("HELLO pg: %d\n", pg);
  PageID cpID;
  if(pg == _iKeyVec.size())
  {
    cpID = _iChildVec[pg].first;
  }
  else if(Less(pKey, _iKeyVec[pg], _iKeyType))
  {
    cpID = _iChildVec[pg].first;
    //printf("Less, cpID: %u\n", cpID);
  }
  else
  {
    cpID = _iChildVec[pg+1].first;
    //printf("Greater Equal, cpID: %u\n", cpID);
  }
  NodePage *childpage = new NodePage(cpID);
  childpage->Insert(pKey, iPair);
  if(childpage->Full())
  {
    //printf("childpage full\n");
    std::pair<std::vector<Field *>, std::vector<PageSlotID>> newchildinfo = 
      childpage->PopHalf();
    NodePage *newchild = new NodePage(_nKeyLen, _iKeyType, 
                              childpage->GetBLeaf(),
                              newchildinfo.first, 
                              newchildinfo.second);
    
    Size pos = LowerBound(newchildinfo.first[0]);
    if(pos == _iKeyVec.size())
    {
      _iKeyVec.push_back(newchildinfo.first[0]);
      _iChildVec.push_back({newchild->GetPageID(),0});
    }
    else if(!Less(_iKeyVec[pos], newchildinfo.first[0], _iKeyType))
    {
      _iKeyVec.insert(_iKeyVec.begin() + pos, newchildinfo.first[0]);
      _iChildVec.insert(_iChildVec.begin() + pos+1, {newchild->GetPageID(),0});    
    }
    
    delete newchild;
  }
  delete childpage;
  return true;
  // ALERT:
  // 中间结点执行插入过程中，需要考虑到实际中间结点为空结点的特殊情况进行特判
  // ALERT: 对于头结点的插入可能更新头结点的Key值
  // ALERT: KeyVec中的Key的赋值需要使用深拷贝，否则会出现析构导致的问题
  // ALERT: 上层保证每次插入的iPair不同
}

Size NodePage::Delete(Field *pKey) {
  // TODO: 需要基于结点类型判断执行过程
  // 叶结点：
  // 1.确定删除位置后删除数据即可
  //printf("Delete pKey: %s", pKey->ToString().c_str());
  if(_bLeaf)
  {
    Size amt = 0;
    auto k_it = _iKeyVec.begin();
    auto c_it = _iChildVec.begin();
    while(k_it != _iKeyVec.end())
    {
      if(Equal(*k_it, pKey, _iKeyType))
      {
        _iKeyVec.erase(k_it);
        _iChildVec.erase(c_it);
        amt++;
      }
      else
      {
        k_it++;
        c_it++;
      }
    }
    return amt;
  }
  // 中间结点:
  // 1.确定执行删除函数的子结点
  // 2.对应的子结点执行删除函数
  // 3.判断子结点是否为满结点，空结点时清除空结点
  // 4.删除空结点情况下需要更新KeyVec和ChildVec
  Size pos = LowerBound(pKey);
  PageID cpID;
  bool less = false;
  if(Less(pKey,_iKeyVec[pos], _iKeyType))
  {
    cpID = _iChildVec[pos].first;
    less = true;
  }
  else
  {
    cpID = _iChildVec[pos+1].first;
    less = false;
  }
  NodePage *childpage = new NodePage(cpID);
  Size amt = childpage->Delete(pKey);
  if(childpage->Empty())
  {
    if(_iKeyVec.size() > 1)
    {
      if(less)
      {
        _iChildVec.erase(_iChildVec.begin() + pos);
        _iKeyVec.erase(_iKeyVec.begin() + pos - 1);          
      }
      else
      {
        _iChildVec.erase(_iChildVec.begin() + pos +1);
        _iKeyVec.erase(_iKeyVec.begin() + pos);
      }
    }
    else
    {
      if(!less)
      {
        _iKeyVec.clear();
        Field *iField;
        if(_iKeyType ==FieldType::INT_TYPE)
        {
          iField = new IntField(0);
        }
        else if(_iKeyType == FieldType::FLOAT_TYPE)
        {
          iField = new FloatField(0);
        }
        _iKeyVec.push_back(iField);
        _nNullKey = true;
      }
        
    }
    
  }
  else
  {
    if(!less)
    {
      _iKeyVec[pos] = childpage->FirstKey();
    }
    else if(pos > 0)
    {
      _iKeyVec[pos - 1] = childpage->FirstKey();
    }
  }
  delete childpage;
  return amt;
  // ALERT: 注意删除结点过程中如果清除了Key则需要析构
  // ALERT:
  // 注意存在键值相同的情况发生，所以需要保证所有需要执行删除函数的子结点都执行了删除函数
  // ALERT: 可以适当简化合并函数，例如不删除空的中间结点
}

bool NodePage::Delete(Field *pKey, const PageSlotID &iPair) {
  // TODO: 需要基于结点类型判断执行过程
  // 叶结点：
  // 1.确定删除位置后删除数据即可
  // printf("In Delete pKey %s Ipair _bLEaf: %d\n", pKey->ToString().c_str(), _bLeaf);
  // printf("KeyVec size %d\n", _iKeyVec.size());
  if(_bLeaf)
  {
    Size pos = LowerBound(pKey);
    for(; pos < _iKeyVec.size(); pos++)
    {
      if(Equal(_iKeyVec[pos], pKey, _iKeyType) &&
        _iChildVec[pos] == iPair)
      {
        _iKeyVec.erase(_iKeyVec.begin()+pos);
        _iChildVec.erase(_iChildVec.begin() + pos);
        //printf("After deleting in leaf KeyVec size %d\n", _iKeyVec.size());
        return true;
      }
    }
    return false;
  }
  // 中间结点:
  // 1.确定执行删除函数的子结点
  // 2.对应的子结点执行删除函数
  // 3.判断子结点是否为满结点，空结点时清除空结点
  // 4.删除空结点情况下需要更新KeyVec和ChildVec
  
  Size pos = LowerBound(pKey);
  PageID cpID;
  bool less = false;
  if(pos == _iKeyVec.size())
  {
    cpID = _iChildVec[pos].first;
  }
  else if(Less(pKey,_iKeyVec[pos], _iKeyType))
  {
    cpID = _iChildVec[pos].first;
    less = true;
  }
  else
  {
    cpID = _iChildVec[pos+1].first;
    less = false;
  }
  NodePage *childpage = new NodePage(cpID);
  bool success = childpage->Delete(pKey, iPair);
  if(childpage->Empty())
  {
    //printf("Childpage empty\n");
    if(_iKeyVec.size() > 1)
    {
      if(less)
      {
        _iChildVec.erase(_iChildVec.begin() + pos);
        _iKeyVec.erase(_iKeyVec.begin() + pos - 1);          
      }
      else
      {
        _iChildVec.erase(_iChildVec.begin() + pos +1);
        _iKeyVec.erase(_iKeyVec.begin() + pos);
      }
    }
    else
    {
      //printf("KEy only has one elem\n");
      if(!less)
      {
        PageID lsID = _iChildVec[pos].first;
        NodePage *leftsib = new NodePage(lsID);
        if(leftsib->Empty())
        {
          _iKeyVec.clear();
          Field *iField;
          if(_iKeyType ==FieldType::INT_TYPE)
          {
            iField = new IntField(0);
          }
          else if(_iKeyType == FieldType::FLOAT_TYPE)
          {
            iField = new FloatField(0);
          }
          _iKeyVec.push_back(iField);
          _nNullKey = true;
        }
        else 
        {
          //printf("left sib not empty\n");
          _iChildVec.pop_back();
          _iKeyVec.clear();
          std::pair<std::vector<Field *>, std::vector<PageSlotID>> halfelem = leftsib->PopHalf();
          //printf("pophalf success\n fields %d\t pageslot %d\n", halfelem.first.size(), halfelem.second.size());
          NodePage *newchildpage = new NodePage(_nKeyLen, _iKeyType, childpage->GetBLeaf(),
                                                halfelem.first, halfelem.second);
          _iKeyVec.push_back(newchildpage->FirstKey());
          // printf("leftsib empty? %d", leftsib->Empty());
          // printf("_iKeyVec new head %s", _iKeyVec[0]->ToString().c_str());
          _iChildVec.push_back({newchildpage->GetPageID(), 0});
          delete newchildpage;
        }
        delete leftsib;
        
      }
    }
    
  }
  else
  {
    if(!less)
    {
      _iKeyVec[pos] = childpage->FirstKey();
      //printf("AFTER DELETE NEW FIRST KEY: %s\n", _iKeyVec[pos]->ToString().c_str());
    }
    else if(pos > 0)
    {
      _iKeyVec[pos - 1] = childpage->FirstKey();
    }
  }
  delete childpage;
  return success;
  // ALERT:
  // 由于Insert过程中保证了没用相同的Value值，所以只要成功删除一个结点即可保证删除成功
}

bool NodePage::Update(Field *pKey, const PageSlotID &iOld,
                      const PageSlotID &iNew) {
  // TODO: 需要基于结点类型判断执行过程
  // 叶结点：
  // 1.确定更新位置后更新数据即可
  //printf("UPDATE _BLEAF: %d\n", _bLeaf);
  if(_bLeaf)
  {
    Size pos = LowerBound(pKey);
    for(; pos < _iKeyVec.size(); pos++)
    {
      if(Equal(pKey, _iKeyVec[pos], _iKeyType) 
        && _iChildVec[pos] == iOld)
      {
        _iChildVec[pos] = iNew;
        return true;
      }
    }
    return false;
    
  }
  // 中间结点:
  // 1.确定执行更新函数的子结点
  // 2.对应的子结点执行更新函数
  Size pos = LowerBound(pKey);
  PageID cpID;
  bool less = false;
  if(pos == _iKeyVec.size())
  {
    cpID = _iChildVec[pos].first;
  }
  else if(Less(pKey,_iKeyVec[pos], _iKeyType))
  {
    cpID = _iChildVec[pos].first;
    less = true;
  }
  else
  {
    cpID = _iChildVec[pos+1].first;
    less = false;
  }
  NodePage *childpage = new NodePage(cpID);
  bool success = childpage->Update(pKey, iOld, iNew);
  delete childpage;
  return success;
  // ALERT: 由于更新函数不改变结点内存储的容量，所以不需要结构变化
  // ALERT:
  // 由于Insert过程中保证了没用相同的Value值，所以只要成功更新一个结点即可保证更新成功
}

std::vector<PageSlotID> NodePage::Range(Field *pLow, Field *pHigh) {
  // TODO: 需要基于结点类型判断执行过程 [pLow, pHigh)
  // 叶结点：
  // 1.确定上下界范围，返回这一区间内的所有Value值
  // printf("IN Range _bLeaf=%d\n", _bLeaf);
  // printf("low field: %s\t", pLow->ToString().c_str());
  // printf("high field: %s\n", pHigh->ToString().c_str());
  // printf("_iKeyVec.size()=%d\n", _iKeyVec.size());
  std::vector<PageSlotID> ans;
  if(_bLeaf)
  {
    Size lpos = LowerBound(pLow);
    //printf("lpos=%d\n", lpos);
    for(; lpos < _iKeyVec.size(); lpos++)
    {
      if(!Less(_iKeyVec[lpos], pLow, _iKeyType) && 
        Greater(pHigh, _iKeyVec[lpos],_iKeyType))
      {
        ans.push_back(_iChildVec[lpos]);
      }
    }
    return ans;
  }
  // 中间结点:
  // 1.确定所有可能包含上下界范围的子结点
  // 2.依次对添加各个子结点执行查询函数所得的结果
  Size kpos = LowerBound(pLow);
  //printf("kpos=%d\n", kpos);
  Size lpos, hpos= _iKeyVec.size()+1;
  
  
  PageID cpID;
  bool less = false;
  if(kpos == _iKeyVec.size())
  {
    lpos = kpos;
  }
  else if(Less(pLow,_iKeyVec[kpos], _iKeyType))
  {
    lpos = kpos;
  }
  else
  {
    lpos = kpos+1;
    //printf("Greater Equal. lpos %d\n", lpos);
  }
  for(; kpos < _iKeyVec.size();kpos++)
  {
    //printf("kpos: %d, Key: %s\n", kpos, _iKeyVec[kpos]->ToString().c_str());
    if(!Less(_iKeyVec[kpos], pHigh, _iKeyType))
    {
      //printf("ALERT\n");
      hpos = kpos+1;
      break;
    }
  }
  for(Size i = lpos; i < hpos; i++)
  {
    cpID = _iChildVec[i].first;
    //printf("UPDATE cpID %u\n", cpID);
    NodePage *childpage = new NodePage(cpID);
    std::vector<PageSlotID> childans = childpage->Range(pLow, pHigh);
    ans.insert(ans.end(), childans.begin(), childans.end());
    delete childpage;
  }
  //printf("Range done\n");
  return ans;
  // ALERT: 注意叶结点可能为空结点，需要针对这种情况进行特判
}

void NodePage::Clear() {
  // TODO: 需要基于结点类型判断执行过程
  // 叶结点：直接释放占用空间
  // 中间结点：先释放子结点空间，之后释放自身占用空间
  if(_bLeaf)
  {
    _iChildVec.clear();
    _iKeyVec.clear();
  }
  else
  {
    while(!_iChildVec.empty())
    {
      NodePage *childpage = new NodePage(_iChildVec.back().first);
      childpage->Clear();
      delete childpage;
      _iChildVec.pop_back();
    }
    _iKeyVec.clear();
    ResetFirst();
  }
  
}

bool NodePage::Full() const { return _iKeyVec.size() == _nCap; }
bool NodePage::Empty() const { return _iKeyVec.size() == 0; }
bool NodePage::LessHalf() const {return _iKeyVec.size() < _nCap / 2;}
FieldType NodePage::GetType() const { return _iKeyType; }
bool NodePage::GetBLeaf() const {return _bLeaf;}
PageID NodePage::GetNextID(){
  return _nNextID;
}
PageID NodePage::GetPrevID(){
  return _nPrevID;
}
Field *NodePage::FirstKey() const {
  if (Empty()) return nullptr;
  if(_bLeaf)
    return _iKeyVec[0];
  else
  {
    NodePage *childpage = new NodePage(_iChildVec[0].first);
    return childpage->FirstKey();
  }
}

std::pair<std::vector<Field *>, std::vector<PageSlotID>> NodePage::PopHalf() {
  std::vector<Field *> iKeyVec{};
  std::vector<PageSlotID> iChildVec{};
  Size mid = _iKeyVec.size() / 2;
  for (auto it = _iKeyVec.begin() + mid; it != _iKeyVec.end();) {
    iKeyVec.push_back(*it);
    it = _iKeyVec.erase(it);
  }
  for (auto it = _iChildVec.begin() + mid; it != _iChildVec.end();) {
    iChildVec.push_back(*it);
    it = _iChildVec.erase(it);
  }
  return {iKeyVec, iChildVec};
}

void NodePage::InitFirst() {
  // TODO:
  // 当初始化一个空的中间结点时，默认为其分配一个空的叶子结点有利于简化实现
  _nNullKey = true;
  Field *iField;
  if(_iKeyType ==FieldType::INT_TYPE)
  {
    iField = new IntField(0);
  }
  else if(_iKeyType == FieldType::FLOAT_TYPE)
  {
    iField = new FloatField(0);
  }
  _iKeyVec.push_back(iField);
  //printf("InitFirst pushback iField complete\n");
  NodePage *childone = new NodePage(_nKeyLen, _iKeyType, true);
  NodePage *childtwo = new NodePage(_nKeyLen, _iKeyType, true);
  _iChildVec.push_back({childone->GetPageID(), 0});
  _iChildVec.push_back({childtwo->GetPageID(), 0});
  delete childone;
  delete childtwo;
  // TODO:
  // 此处需要基于结点Key的类型，初始化一个叶子结点并为KeyVec和ChildVec添加第一个值
}

void NodePage::ResetFirst() {
  // TODO: 当中间结点清空时，为其保留一个空的叶子结点可以简化删除的合并过程
  // TODO: 此处需要基于结点Key的类型，重新设置KeyVec的第一个Key值
  _nNullKey = true;
  Field *iField;
  if(_iKeyType ==FieldType::INT_TYPE)
  {
    iField = new IntField(0);
  }
  else if(_iKeyType == FieldType::FLOAT_TYPE)
  {
    iField = new FloatField(0);
  }
  _iKeyVec.push_back(iField);
  NodePage *childone = new NodePage(_nKeyLen, _iKeyType, true);
  NodePage *childtwo = new NodePage(_nKeyLen, _iKeyType, true);
  _iChildVec.push_back({childone->GetPageID(), 0});
  _iChildVec.push_back({childtwo->GetPageID(), 0});
  delete childone;
  delete childtwo;
}

void NodePage::Load() {
  // TODO: 从格式化页面数据中导入结点信息
  Size used_slots;
  //printf("In Load. Keylen %d\n", _nKeyLen);
  GetHeader((uint8_t*)&used_slots, 4, USED_SLOT_OFFSET);
  if(_bLeaf) //leaf node
  {
    Size recordsize = 6 + _nKeyLen;
    for(Size i = 0; i < used_slots; i++)
    {
      PageSlotID pageslot;
      Field *iField;
      uint8_t *data = new uint8_t[_nKeyLen];
      GetData((uint8_t*)&pageslot.first, 4, recordsize * i);
      GetData((uint8_t*)&pageslot.second, 2, recordsize * i + 4);
      GetData(data, _nKeyLen, recordsize*i + 6);
      if(_iKeyType == FieldType::NONE_TYPE)
      {
        iField = new NoneField();
      }
      else if(_iKeyType ==FieldType::INT_TYPE)
      {
        iField = new IntField();
      }
      else if(_iKeyType == FieldType::FLOAT_TYPE)
      {
        iField = new FloatField();
      }
      else if(_iKeyType == FieldType::STRING_TYPE)
      {
        iField = new StringField(_nKeyLen);
      }
      iField->SetData(data,_nKeyLen);
      _iKeyVec.push_back(iField);
      _iChildVec.push_back(pageslot);
    }
  }
  else
  {
    Size recordsize = 4 + _nKeyLen;
    Size i = 0;
    PageSlotID pageslot;
    Field *iField;
    uint8_t *data = new uint8_t[_nKeyLen];
    for(; i < used_slots; i++)
    {
      
      GetData((uint8_t*)&pageslot.first, 4, recordsize * i);
      pageslot.second = 0;
      GetData(data, _nKeyLen, recordsize*i + 4);
      //printf("Load nonLeaf data: %u\n", data);
      if(_iKeyType == FieldType::NONE_TYPE)
      {
        iField = new NoneField();
      }
      else if(_iKeyType ==FieldType::INT_TYPE)
      {
        iField = new IntField();
      }
      else if(_iKeyType == FieldType::FLOAT_TYPE)
      {
        iField = new FloatField();
      }
      else if(_iKeyType == FieldType::STRING_TYPE)
      {
        iField = new StringField(_nKeyLen);
      }
      iField->SetData(data, _nKeyLen);
      _iKeyVec.push_back(iField);
      _iChildVec.push_back(pageslot);
    }
    GetData((uint8_t*)&pageslot.first, 4, recordsize * i);
    _iChildVec.push_back(pageslot);
  }
  
  // TODO: 自行设计，注意和Store匹配
}

void NodePage::Store() {
  // TODO: 格式化结点信息并保存到页面中
  // TODO: 自行设计，注意和Load匹配
  
  if(_bLeaf)
  {
    Size recordsize = 6 + _nKeyLen;
    for(Size i = 0; i < _iKeyVec.size(); i++)
    {
      uint8_t *data = new uint8_t[_nKeyLen];
      _iKeyVec[i]->GetData(data, _nKeyLen);
      
      SetData((uint8_t*)&_iChildVec[i].first, 4, recordsize * i);
      SetData((uint8_t*)&_iChildVec[i].second, 2, recordsize * i + 4);
      SetData(data, _nKeyLen, recordsize*i + 6);
    }
  }
  else
  {
    Size recordsize = 4 + _nKeyLen;
    Size i = 0;
    for(; i < _iKeyVec.size(); i++)
    {
      uint8_t *data = new uint8_t[_nKeyLen];
      _iKeyVec[i]->GetData(data, _nKeyLen);
      SetData((uint8_t*)&_iChildVec[i].first, 4, recordsize * i);
      //SetData((uint8_t*)&_iChildVec[i].second, 2, recordsize * i + 4);
      SetData(data, _nKeyLen, recordsize*i + 4);
    }
    SetData((uint8_t*)&_iChildVec[i].first, 4, recordsize * i);
  }
  
}

Size NodePage::LowerBound(Field *pField) {
  // TODO: 二分查找找下界，此处给出实现
  // TODO: 边界的理解非常重要，可以自行重新测试一下
  Size nBegin = 0, nEnd = _iKeyVec.size();
  while (nBegin < nEnd) {
    Size nMid = (nBegin + nEnd) / 2;
    if (!Less(_iKeyVec[nMid], pField, _iKeyType)) {
      //printf("Lowerbound !Less\n");
      nEnd = nMid;
    } else {
      //printf("Lowerbound less\n");
      nBegin = nMid+1;
    }
  }
  //printf("return from LOwerBound\n");
  return nBegin;
}

Size NodePage::UpperBound(Field *pField) {
  // TODO: 二分查找找上界，此处给出实现
  // TODO: 边界的理解非常重要，可以自行重新测试一下
  Size nBegin = 0, nEnd = _iKeyVec.size();
  while (nBegin < nEnd) {
    Size nMid = (nBegin + nEnd) / 2;
    if (Greater(_iKeyVec[nMid], pField, _iKeyType)) {
      nEnd = nMid;
    } else {
      nBegin = nMid+1;
    }
  }
  return nBegin;
}

Size NodePage::LessBound(Field *pField) {
  if (Empty()) return 0;
  if (Less(pField, _iKeyVec[0], _iKeyType)) return 0;
  return UpperBound(pField) - 1;
}

}  // namespace thdb
