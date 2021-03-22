#include "string_field.h"

namespace thdb {

StringField::StringField(Size nSize) { _sData = String("\0", nSize); }

StringField::StringField(const String &sData) { _sData = sData; }

void StringField::SetData(const uint8_t *src, Size nSize) {
  char *_pData = new char[nSize + 1];
  _pData[nSize] = '\0';
  memcpy(_pData, src, nSize);
  _sData = String(_pData);
  delete[] _pData;
}

void StringField::GetData(uint8_t *dst, Size nSize) const {
  memcpy(dst, _sData.c_str(), nSize);
}

FieldType StringField::GetType() const { return FieldType::STRING_TYPE; }

String StringField::ToString() const { return _sData; }

String StringField::GetString() const { return _sData; }

bool operator==(const StringField &a, const StringField &b) {
  return a.GetString() == b.GetString();
}

bool operator<(const StringField &a, const StringField &b) {
  return a.GetString() < b.GetString();
}

bool operator<=(const StringField &a, const StringField &b) {
  return (a < b) || (a == b);
}

bool operator>(const StringField &a, const StringField &b) { return !(a <= b); }

bool operator>=(const StringField &a, const StringField &b) { return !(a < b); }

bool operator!=(const StringField &a, const StringField &b) {
  return !(a == b);
}

}  // namespace thdb