#include "record/fixed_record.h"

#include <assert.h>

#include "exception/exceptions.h"
#include "field/fields.h"

namespace thdb {

FixedRecord::FixedRecord(Size nFieldSize,
                         const std::vector<FieldType> &iTypeVec,
                         const std::vector<Size> &iSizeVec)
    : Record(nFieldSize), _iTypeVec(iTypeVec), _iSizeVec(iSizeVec) {
  //initialized _iTypeVec and _iSizeVec
  assert(_iTypeVec.size() == nFieldSize);
  assert(_iSizeVec.size() == nFieldSize);
}

Size FixedRecord::Load(const uint8_t *src) {
  Clear();
  
  // LAB1 BEGIN
  // TODO: 反序列化，为定长记录导入各个字段数据
  // TIPS: 利用Field的抽象方法SetData导入数据 
  // TIPS: 基于类型判断构建的指针类型
  //先遍历header的信息
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

Size FixedRecord::Store(uint8_t *dst) const {
  //return -1;  // 开始实验时删除此行
  // LAB1 BEGIN
  // TODO: 序列化，将定长数据转化为特定格式
  // TIPS: 利用Field的抽象方法GetData写出数据
  // TIPS: 基于类型进行dynamic_cast进行指针转化
  //fill in data
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

void FixedRecord::Build(const std::vector<String> &iRawVec) {
  assert(iRawVec.size() == _iTypeVec.size());
  Clear();
  for (FieldID i = 0; i < _iFields.size(); ++i) {
    FieldType iType = _iTypeVec[i];
    if (iRawVec[i] == "NULL") {
      SetField(i, new NoneField());
      continue;
    }
    if (iType == FieldType::INT_TYPE) {
      int nVal = std::stoi(iRawVec[i]);
      SetField(i, new IntField(nVal));
    } else if (iType == FieldType::FLOAT_TYPE) {
      double fVal = std::stod(iRawVec[i]);
      SetField(i, new FloatField(fVal));
    } else if (iType == FieldType::STRING_TYPE) {
      SetField(i, new StringField(iRawVec[i].substr(1, iRawVec[i].size() - 2)));
    } else {
      throw RecordTypeException();
    }
  }
}

}  // namespace thdb