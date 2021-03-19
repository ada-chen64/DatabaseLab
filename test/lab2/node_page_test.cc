#include "page/node_page.h"

#include <iostream>

#include "field/fields.h"
#include "gtest/gtest.h"
#include "macros.h"

namespace thdb {

void Search(NodePage* pNode, int nLow, int nHigh) {
  Field* pLow = new IntField(nLow);
  Field* pHigh = new IntField(nHigh);
  auto iRes = pNode->Range(pLow, pHigh);
  printf("Range[%d,%d]: ", nLow, nHigh);
  for (const auto& it : iRes) printf("%u.%u ", it.first, it.second);
}

TEST(Lab2, NodePageTest) {
  NodePage* pNode = new NodePage(4, FieldType::INT_TYPE, true);
  PageID nPageID = pNode->GetPageID();
  for (int i = 3; i < 7; ++i) {
    pNode->Insert(new IntField(i), PageSlotID(0, i));
    printf("Insert %d\n", i);
  }
  for (int i = 0; i < 10; ++i) {
    Search(pNode, i, i);
  }
  delete pNode;
}

}  // namespace thdb