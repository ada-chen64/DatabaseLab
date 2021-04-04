#include "index/index.h"

#include <iostream>

#include "field/fields.h"
#include "gtest/gtest.h"
#include "macros.h"

namespace thdb {

void Search(Index* pIndex, int nLow, int nHigh) {
  auto iRes = pIndex->Range(new IntField(nLow), new IntField(nHigh));
  printf("Range[%d,%d):", nLow, nHigh);
  for (const auto& iPair : iRes) printf("%u:%u ", iPair.first, iPair.second);
  printf("\n");
}

// 此测试函数没有任何关于结果的检测，仅用于帮助调试
TEST(Lab2, DISABLED_IndexTest) {
  Index* pIndex = new Index(FieldType::INT_TYPE);
  for (int i = 2; i < 10000; ++i) {
    pIndex->Insert(new IntField(i), {0, i});
  }
  Search(pIndex, 5423, 5435);
  delete pIndex;
}

}  // namespace thdb
