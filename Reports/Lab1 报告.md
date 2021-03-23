# Lab1 报告

#### 陈昱霏 2017080067

## 实验任务：

1. 阅读代码，对于整体实验框架有一个结构性的理解
   完成
2. 设计底层记录页面组织，完成记录管理的各项基本功能
   完成

## 实现接口思路

- record

  - FixedRecord: 定长记录

    - Store 记录序列化

      ```c++
      Size FixedRecord::Store(uint8_t *dst) const {
        Size pos = 0;
        for(FieldID i=0; i < _iFields.size(); i++)
        {
          //printf("Field Type: %d Size: %u\n", _iTypeVec[i], _iSizeVec[i]);
          _iFields[i]->GetData(dst + pos, _iSizeVec[i]);
          pos += _iSizeVec[i];
        }
        return pos;
        // LAB1 END
      }
      ```

      将每个field按照大小直接传入dst里面，然后按照大小更新下一个field传入的位置，最后位置的更新就得到总大小。

    - Load 记录反序列化

      ```c++
      Size FixedRecord::Load(const uint8_t *src) {
        Clear();
        Size pos = 0;
        for(FieldID i =0; i < _iFields.size(); i++)
        {
          FieldType iType = _iTypeVec[i];
          Field *iField;
          if(iType == FieldType::NONE_TYPE)
          {
            iField = new NoneField();
          }
          else if(iType ==FieldType::INT_TYPE)
          {
            iField = new IntField();
          }
          else if(iType == FieldType::FLOAT_TYPE)
          {
            iField = new FloatField();
          }
          else if(iType == FieldType::STRING_TYPE)
          {
            iField = new StringField(_iSizeVec[i]);
          }
          else
            throw RecordTypeException();
          iField->SetData(src + pos, _iSizeVec[i]);
          SetField(i, iField);
          pos += _iSizeVec[i];
        }
        return pos;
        // LAB1 END
      }
      
      ```

      由于每个Field的类和大小都已经定义了，所以只要按照顺序和大小直接用`SetData`把数据导入field中再在该record中设置该field就可以了。

  - VariableRecord: 用于变长记录的序列化，需要自己设计

    - Store 序列化

      ```c++
      Size VariableRecord::Store(uint8_t *dst) const {
        Size pos = 0;
        for(FieldID i=0; i < _iFields.size(); i++)
        {
          FieldType iType = _iTypeVec[i];
          if(iType == FieldType::NONE_TYPE)
          {
            //store the size
            uint16_t size= _iSizeVec[i];
            memcpy(dst+pos, (uint8_t*)&size, 2);
            pos += 2;
            _iFields[i]->GetData(dst + pos, _iSizeVec[i]);
            pos += _iSizeVec[i];
          }
          else if(iType ==FieldType::INT_TYPE)
          {
            uint16_t size= _iSizeVec[i];
            memcpy(dst+pos, (uint8_t*)&size, 2);
            pos += 2;
            _iFields[i]->GetData(dst + pos, _iSizeVec[i]);
            pos += _iSizeVec[i];
          }
          else if(iType == FieldType::FLOAT_TYPE)
          {
            uint16_t size= _iSizeVec[i];
            memcpy(dst+pos, (uint8_t*)&size, 2);
            pos += 2;
            _iFields[i]->GetData(dst + pos, _iSizeVec[i]);
            pos += _iSizeVec[i]; 
          }
          else if(iType == FieldType::STRING_TYPE)
          {
            StringField *ifield = (StringField*)_iFields[i];
            String s = ifield->GetString();
            uint16_t size= sizeof(s);
            memcpy(dst+pos, (uint8_t*)&size, 2);
            pos += 2;
            _iFields[i]->GetData(dst + pos, size);
            pos += size;
          }
          else
            throw RecordTypeException();
        }
        return pos;
        // LAB1 END
      }
      ```

      和定长序列化其实差不多的概念，只是每个Field的data前面加上两个字节作为该数据的大小。如果该数据和定长数据一样大的话，这种方法其实并不能节省空间，但是这样会使得反序列化比较方便。

    - Load 反序列化

      ```c++
      Size VariableRecord::Load(const uint8_t *src) {
        Clear();
        int pos = 0;
        for(FieldID i =0; i < _iFields.size(); i++)
        {
          uint16_t size = 0;
          memcpy((uint8_t *)&size, src+pos, 2);
          pos+=2;
          FieldType iType = _iTypeVec[i];
          Field *iField;
          if(iType == FieldType::NONE_TYPE)
          {
            iField = new NoneField();
          }
          else if(iType ==FieldType::INT_TYPE)
          {
            iField = new IntField();
          }
          else if(iType == FieldType::FLOAT_TYPE)
          {
            iField = new FloatField();
          }
          else if(iType == FieldType::STRING_TYPE)
          {
            iField = new StringField(size);
          }
          else
            throw RecordTypeException();
          iField->SetData(src + pos, size);
          SetField(i, iField);
          pos = size;
        }
        return pos;
        // LAB1 END
      }
      ```

      在做变长反序列化的时候每个field的前两个字节为大小，后面在根据得到的大小读入数据，剩下的和定长的思路相同。

    

- page

  - LinkedPage: 链表页面

    - PushBack 插入页面

      ```c++
      bool LinkedPage::PushBack(LinkedPage *pPage) {
        if (!pPage) return false;
        if(this->_nNextID == NULL_PAGE)
        {
          this->SetNextID(pPage->GetPageID());
          //printf("set nextID: %u\n", this->_nNextID);
          pPage->SetPrevID(this->_nPageID);
          pPage->SetNextID(NULL_PAGE);
        }
        else
        {
          LinkedPage *nextpage = new LinkedPage(this->_nNextID);
          pPage->SetNextID(this->_nNextID);
          pPage->SetPrevID(this->_nPageID);
          this->SetNextID(pPage->GetPageID());
          nextpage->SetPrevID(pPage->GetPageID());
          delete nextpage;
        }
        // TIPS：正确设置当前页面和pPage的PrevID以及NextID
        // LAB1 END
        return true;
      }
      ```

      这里主要的任务就是插入新的页面。如果该页面后面为空，把该页面的`NextID`设为`pPage`的ID，把`pPage`的`PrevID`设为当前的ID即可，如果当前页面之后的页面不为空，那就要对当前页面和下一个页面和插入页面的`NextID`和`PrevID`分别做调整。具体如上方的代码。

    - PopBack 移除页面

      ```c++
      PageID LinkedPage::PopBack() {
        PageID nextID = this->_nNextID;
        if(nextID != NULL_PAGE)
        {
          
          LinkedPage *nextpage = new LinkedPage(nextID);
          if(nextpage->GetNextID() != NULL_PAGE)
          {
            this->SetNextID(nextpage->GetNextID());
            LinkedPage *nnext = new LinkedPage(nextpage->GetNextID());
            nnext->SetPrevID(this->_nPageID);
            delete nnext;
          }
          else
            this->SetNextID(NULL_PAGE);
          delete nextpage;
          MiniOS::GetOS()->DeletePage(nextID);
        }
        return nextID;
        
      }
      ```

      和PushBack的思路类似，只是概念相反，如果当前页面指向的页面`nextpage`不为空，则将当前页面的`NextID`设为`nextpage`的`NextID`，将`nextpage`之后的页面的`PrevID`设为当前ID，然后直接删除`nextpage`即可。

  - RecordPage: 定长记录存储页面

    - GetRecord 页面内获取记录

      ```c++
      uint8_t *RecordPage::GetRecord(SlotID nSlotID) {
        uint8_t *dst = new uint8_t[_nFixed];
        GetData(dst,_nFixed, BITMAP_OFFSET + BITMAP_SIZE + nSlotID *_nFixed);
        return dst;
        // LAB1 END
      }
      ```

      因为record大小已有定义，我初始化一个大小为`_nFixed`的数组，然后直接把数据导入数组就可以，这里主要要注意offset的设置不要错。

    - InsertRecord 页面内插入记录

      ```c++
      SlotID RecordPage::InsertRecord(const uint8_t *src) {
        //printf("in recordpage insert\n");
        for(SlotID sid = 0; sid < _nCap; sid++)
        {
          if(!HasRecord(sid))
          {  
            _pUsed->Set(sid);
            //printf("page %u Set %d\n", _nPageID,sid);
            SetData(src, _nFixed, BITMAP_OFFSET + BITMAP_SIZE + sid * _nFixed);
            return sid;
          } 
        }
        //printf("record page return -1\n");
        return -1;
      }
      ```

      在插入record的时候，首先寻找空槽，找到空槽后把数据传入槽中即可，如果没有空槽返回-1。

    - DeleteRecord 页面内删除记录

      ```c++
      void RecordPage::DeleteRecord(SlotID nSlotID) {
        _pUsed->Unset(nSlotID);
      }
      ```

      删除数据的话不需要对硬盘进行任何改变，只要把bitmap里代表当前位置的地方设为0就行，下次查询的时候直接覆盖当前的数据。

    - UpdateRecord 页面内更新记录

      ```c++
      void RecordPage::UpdateRecord(SlotID nSlotID, const uint8_t *src) {
        if (!_pUsed->Get(nSlotID)) throw RecordPageException(nSlotID);
        SetData(src, _nFixed, BITMAP_OFFSET + BITMAP_SIZE + nSlotID * _nFixed);
      }
      ```

      这里主要就是判断一下该槽是否有数据，如果是空槽那就不能用UpdateRecord。不是空槽的话，其实直接把数据传到槽里就行。

  - ToastPage: 用于变长记录存储的页面，需要自己设计

    由于概念有点复杂，代码有点长，这里我就不加代码了，基本的思路就是header中设一个slot directory，记录该槽的记录的大小和offset，我称为`Slot`类（`std::pair<Size, PageOffset>`）。记录从`page`的尾部往前加，当slot directory和记录之间没有空隙的时候，该页为满。在slot directory的最前面设置槽的个数，第二位设置一个offset指向可以插入新数据的地方，我称为`free_offset`，代表free space。因为槽的个数不定，所以我用`vector`来记录。

    插入的话，直接在`free_offset`的地方插入记录，将`free_offset`更改即可。删除的话就比较麻烦。对边行记录的话，可以选择频繁更新和隔一段时间更新。频繁更新的话，就是每次删除的时候整理一下记录，我就是采用频繁更新的方法。我用的方法就是最简单的，把当前删除的槽后面的槽全部往前移，为了快速获取当前槽后面都是哪些槽，我用了一个`std::map<PageOffset, SlotID>`来记录。具体的实现我就不多做解释了。

    由于页面中数据的摆放顺序更改，我对`SetData`和`GetData`也做了相应的重定义。

- Table

  - Table

    - GetRecord 表内获取记录

      ```c++
      Record *Table::GetRecord(PageID nPageID, SlotID nSlotID) {
        RecordPage *rp = new RecordPage(nPageID); //构造页面
        uint8_t *rec = rp->GetRecord(nSlotID);
        FixedRecord *fr = new FixedRecord(pTable->GetFieldSize(), pTable->GetTypeVec(),
          pTable->GetSizeVec());
        fr->Load(rec);
        delete rp;
        delete[] rec;
        return fr;
      }
      ```

      为了得到在`nPageID`页`nSlotID`槽的记录，我首先构造`RecordPage`页面，这个构造函数直接从硬盘里读入数据，我通过`RecordPage`自带的`GetRecord`函数就可以得到记录的数据，但是我需要构造一个`Record`类，所以我构造了一个`FixedRecord`，用Load将数据传进去，就得到一个记录类。

    - InsertRecord 表内插入记录

      ```c++
      PageSlotID Table::InsertRecord(Record *pRecord) {
        PageID pid = _nNotFull;
        RecordPage *page = new RecordPage(pid);
        uint8_t * rec = new uint8_t[PAGE_SIZE];
        pRecord->Store(rec);
        SlotID slot = page->InsertRecord(rec);
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
        return std::pair<PageID, SlotID>(pid, slot);
       
      }
      ```
      
      这里首先是要注意要把Record类序列化。我获得有空槽的页面之后，将序列化的数据输入到页面中，然后如果页面满了，我用`NextNotFull`来设置下一个有空的页面。注意一定要在`NextNotFull`函数之前delete构造的页面否则页面的bitmap不会更新，到了`NextNotFull`就会判断该也未满而不设置新的`_nNotFull`。
      
    - DeleteRecord 表内删除记录
    
  ```c++
      void Table::DeleteRecord(PageID nPageID, SlotID nSlotID) {
    RecordPage *rp = new RecordPage(nPageID);
        rp->DeleteRecord(nSlotID);
    _nNotFull = nPageID;
        delete rp;
      }
  ```
  
      如果要删除数据的话，直接构造当前页面，用RecordPage自带的删除函数删除记录，注意一定要delete构造的页面否则页面不会更新。
  
    - UpdateRecord 表内更新记录
  
  ```c++
      void Table::UpdateRecord(PageID nPageID, SlotID nSlotID,
                           const std::vector<Transform> &iTrans) {
        RecordPage *page = new RecordPage(nPageID);
    // TIPS: 构建Record对象，利用Record::SetField更新Record对象
        Record *record = GetRecord(nPageID, nSlotID);
        uint8_t * rec = new uint8_t[PAGE_SIZE];
        for(int i = 0; i < iTrans.size(); i++)
        {
          FieldID pos = iTrans[i].GetPos();
          Field *_ifield = iTrans[i].GetField();
          record->SetField(pos, _ifield);
        }
        
        record->Store(rec);
        page->UpdateRecord(nSlotID, rec);
        delete page;
        delete record;
      }
  ```
  
      这里获得页面和记录之后，通过`iTrans`得到需要改的`Field`的位置和心得`field`的数据，然后直接用`SetField`来设置即可，注意最后要讲记录序列化存回硬盘。
  
    - SearchRecord 记录条件检索
  
  ```c++
      std::vector<PageSlotID> Table::SearchRecord(Condition *pCond) {
    std::vector<PageSlotID> matches;
        PageID nBegin = _nHeadID;
    int totalrecords = 0;
        while(nBegin != NULL_PAGE)
        {
          RecordPage *page = new RecordPage(nBegin);
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
          }
          nBegin = page->GetNextID();
          delete page;
        }
        return matches;
      }
  ```
  
      虽然有点简单粗暴，我就是按顺序查找每一个记录，判断是否符合condition，如果符合的话，直接加到序列里即可。
  
    - NextNotFull 更新非满页面的页面编号，必要时插入新的页面
    
      ```c++
      void Table::NextNotFull() {
        PageID pid = _nHeadID;
      while (pid != NULL_PAGE) {
          RecordPage *rp = new RecordPage(pid);
      if(!rp->Full())
          {
        _nNotFull = pid;
            delete rp;
            return;
          }
          pid = rp->GetNextID();
          delete rp;
        }
        RecordPage *tailpage = new RecordPage(_nTailID);
        RecordPage *newpage = new RecordPage(pTable->GetTotalSize(), true);
        tailpage->PushBack(newpage);
        _nTailID = newpage->GetPageID();
        _nNotFull = newpage->GetPageID();
        delete tailpage;
        delete newpage;
      }
      ```
    
      这里也是按照顺序查询每个页面是否满，如果未满直接设为_nNotFull，否则创建新的页面, 用PushBack来加入链表里。

