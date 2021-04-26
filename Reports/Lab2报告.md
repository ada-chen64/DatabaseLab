# Lab2

陈昱霏 2017080067

## 实验任务：

1. 合并Lab2新增的实验框架代码，保证合并后不影响Lab1原始测试。
2. 设计索引的底层存储结构，实现基于索引进行高速查找的算法。索引要求至少支持整形(Integer)和浮点型数据(Double)两种基本的字段类型。只需要支持单字段落索引。

## 新模块

### NodePage

节点页面，利用内部变量区分不同类型的树节点

这个模块有很多需要实现的函数，所以我先从存储和读取方式解释。对于叶节点和中间节点，存储和读取方式是不一样的。因为叶节点存储的是一个个Key和Value对，所以`_iKeyVec`和`_iChildVec`的大小是相同的。但是对于中间节点`_iChildVec`永远比`_iKeyVec`多一项。所以在存储和读取的时候要先判断是否为叶子节点。

**Store**

对于叶子节点，我采用的时候一个Value一个Key的存储方法，将`PageSlotID`分开，现存`PageID`再存`SlotID`，总共6个字节，然后Key按照Key的大小存进去，反复指导都存好为止。而因为中间节点的`_iChildVec`中的SlotID全是0，所以并不需要存下来，因此我们按照一个Value一个Key的存储方法，Value只占用4个字节，但是要注意因为Value永远比Key多一项所以最后要补上。

```c++
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
      SetData(data, _nKeyLen, recordsize*i + 4);
    }
    SetData((uint8_t*)&_iChildVec[i].first, 4, recordsize * i);
  }
  
}
```

**Load**

读取的时候先从Header中读取usedSlot，也就是`_iKeyVec`的大小，然后根据大小和Store的存储方法读取信息。这里就不给代码了。

由以上的存储方法得到以下的`_nCaps`计算方法：

```c++
if(_bLeaf)
{
    _nCap = (DATA_SIZE) / (_nKeyLen + 6)+1;
}
else
{
    _nCap = (DATA_SIZE - 4) / (_nKeyLen + 4)+1;
}  
```

因为`Full`函数代表在`_iKeyVec.size()==_nCap`的时候为溢出，所以`_nCap`要在最高存储量之上加一。

**简易B+树结构**

由于该框架实现的补上传统的B+树或B树，而是一个简易，不需要删除中间节点并且叶子节点没有连接的B+树结构，它的初始化是有讲究的。一般情况下我在插入和删除的时候，都是顺着`_iKeyVec`找该插入或删除的位置，但是加入这个节点时候空的怎么办？

对于叶子节点，如果时候空的就直接插入Key和Value，和非空的时候时候一样的。如果时候中间节点的话，那就要维护`_iChildVec`在初始化，或者清空的时候永远保持两个叶子节点。对于空的中间节点要保证只想两个叶子节点。

```c++
void NodePage::InitFirst() {
  NodePage *childone = new NodePage(_nKeyLen, _iKeyType, true);
  NodePage *childtwo = new NodePage(_nKeyLen, _iKeyType, true);
  _iChildVec.push_back({childone->GetPageID(), 0});
  _iChildVec.push_back({childtwo->GetPageID(), 0});
  delete childone;
  delete childtwo;
}

void NodePage::ResetFirst() {
  NodePage *childone = new NodePage(_nKeyLen, _iKeyType, true);
  NodePage *childtwo = new NodePage(_nKeyLen, _iKeyType, true);
  _iChildVec.push_back({childone->GetPageID(), 0});
  _iChildVec.push_back({childtwo->GetPageID(), 0});
  delete childone;
  delete childtwo;
}
```

**插入删除**

插入叶子节点的时候直接判断插入位置进行插入即可，不需要考虑满的时候的问题。

当插入到一个空的中间节点的时候直接将Key加入`_iKeyVec`中，但注意大于等于该Key的项要插入右孩子，所以第一个Key Value对要插入右孩子。

如果中间节点非空，那么要判断执行插入的子节点进行插入，在进行插入之后判断子节点是否满了，如果满了就要做分裂。由于分裂时候父节点管子节点，从父节点执行分裂函数，所以之前定义为private的PopHalf函数改成了public，这样可以从上层节点操控下层节点的分裂，得到一半的Key Value对，并且创建新的子节点，插入到当前的`_iKeyVec`中。如果子节点没有满的话，只需要判断如果时候插入到右孩子的话，该分支里最小的Key是什么，把当前的Key进行修改。为此，我写了一个FirstKey函数专门沿着分支查找最小的Key。

```c++
bool NodePage::Insert(Field *pKey, const PageSlotID &iPair) {
  // TODO: 需要基于结点类型判断执行过程
  // 叶结点：
  // 1.确定插入位置后插入数据即可
  if(_bLeaf)
  {
    if(_iKeyVec.size() >= _nCap)
      return false;
    Size pos = LowerBound(pKey);
    while(pos != _iChildVec.size())
    {
      if(Equal(_iKeyVec[pos], pKey, _iKeyType)&&
        iPair >= _iChildVec[pos])
      {
        
        _iKeyVec.insert(_iKeyVec.begin() + pos, pKey);
        _iChildVec.insert(_iChildVec.begin() + pos, iPair);
        return true;
      }
      else if(Greater(_iKeyVec[pos], pKey, _iKeyType))
      {
        _iKeyVec.insert(_iKeyVec.begin() + pos, pKey);
        _iChildVec.insert(_iChildVec.begin() + pos, iPair);
        return true;
      }
      pos++;
    }
    _iKeyVec.push_back(pKey);
    _iChildVec.push_back(iPair);
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
  
  Size pg = LowerBound(pKey);
  PageID cpID;
  if(pg == _iKeyVec.size())
  {
    cpID = _iChildVec[pg].first;
  }
  else if(Less(pKey, _iKeyVec[pg], _iKeyType))
  {
    cpID = _iChildVec[pg].first;
  }
  else
  {
    cpID = _iChildVec[pg+1].first;
  }
  NodePage *childpage = new NodePage(cpID);
  childpage->Insert(pKey, iPair);
  if(childpage->Full())
  {
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
```

删除和插入的概念类似，只是在子节点空的时候要进行一些处理。如果当前`_iKeyVec.size()>1`那么我就删除指向叶节点的`_iChildVec`项和对应的`_iKeyVec`项。如果当前节点只剩下一个Key, 那么需要判断刚才删除的时候左孩子还是右孩子，如果时候右孩子，就必须判断左孩子是否为空，若左孩子也是空的则清空`_iKeyVec`。如果左孩子不为空，则将左孩子的一半送给右孩子。

如果删除后子节点非空则和插入一样，修改以下`_iKeyVec`里当前位置的Key，确认时候分支里最小的Key。

```c++
// Delete 
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
                    _iChildVec.pop_back();
                    _iKeyVec.clear();
                    std::pair<std::vector<Field *>, std::vector<PageSlotID>> halfelem = leftsib->PopHalf();
                    NodePage *newchildpage = new NodePage(_nKeyLen, _iKeyType, childpage->GetBLeaf(),
                                                          halfelem.first, halfelem.second);
                    _iKeyVec.push_back(newchildpage->FirstKey());
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
        }
        else if(pos > 0)
        {
            _iKeyVec[pos - 1] = childpage->FirstKey();
        }
    }
```

### Index模块

Index模块中的函数要比NodePage中简单，构建Index的时候只需要构建一个根节点和记录根节点的PageID即可。

插入，删除，和更新时，也是通过记录的根节点PageID构建根节点，对其进行插入或删除或更新。插入时如果根节点满了，则用PopHalf函数获得一半的Key Value对，构建一个兄弟节点，将Key Value对放进去，然后再构建一个新的根节点来指向原本的根节点和兄弟节点。更新新的根节点的PageID，则插入就完成了。

```c++
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
```

## 实现难点与框架问题

本次实验的最大的难点我认为在于插入后子节点为满的时候和删除后子节点为空的处理。插入后满了时，主要要注意对满的子节点进行分裂。NodePage原本的设计将PopHalf函数设为private函数，也就是说当前的节点只能命令自己从内部进行分裂，可是这样一来会产生很多矛盾和错误。因为我时候从Index模块中控制我的Index树，如果我把PopHalf设为private，那只能在当前节点插入之后判断自己是否满了，如果满了就对自己进行分裂，创建兄弟节点和新的父亲节点。可是这样的话，假设我在NodePage里创建了新的根节点，返回到Index中，Index时候完全不知道的，也就是说Index无法记录新的根节点的PageID。因此，分裂必须得时候由上层节点控制下层节点进行分裂，所以PopHalf要改成public函数。在插入后也要注意每个Key要示意该分支最小的节点的Key，所以每次进行插入都要对Key进行赋值。因此我们用递归来查找该分支下最小的Key。为此我增加了FirstKey函数。

 在实验的开始也是对NodePage的简易B+树的实现方法有点疑惑，可是确定了中间节点为空时候的操作后一切就明确了。只是因为这棵树不会删除空的中间节点和调整树的结构，所以在空间意义上还有待优化的。