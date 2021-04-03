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
  delete pLow;
  delete pHigh;
  printf("Range[%d,%d): ", nLow, nHigh);
  for (const auto& it : iRes) printf("%u.%u ", it.first, it.second);
  printf("\n");
}

TEST(Lab2, DISABLED_LeafPageTest) {
  NodePage* pLeaf = new NodePage(4, FieldType::INT_TYPE, true);
  PageID nPageID = pLeaf->GetPageID();
  for (int i = 0; i < 9; ++i) {
    pLeaf->Insert(new IntField(i), PageSlotID(0, i));
  }
  pLeaf->Insert(new IntField(5), PageSlotID(1, 5));
  pLeaf->Insert(new IntField(5), PageSlotID(2, 5));
  for (int i = 0; i < 10; ++i) {
    Search(pLeaf, 0, i);
  }
  pLeaf->Delete(new IntField(4));
  Search(pLeaf, 3, 6);
  pLeaf->Delete(new IntField(5), PageSlotID(1, 5));
  Search(pLeaf, 4, 6);
  delete pLeaf;
}

TEST(Lab2, NodePageTest) {
  NodePage* pNode = new NodePage(4, FieldType::INT_TYPE, false);
  PageID nPageID = pNode->GetPageID();
  for (int i = 0; i < 15; ++i) {
    pNode->Insert(new IntField(i), PageSlotID(0, i));
  }
  pNode->Insert(new IntField(5), PageSlotID(1, 5));
  pNode->Insert(new IntField(5), PageSlotID(2, 5));
  for (int i = 0; i < 20; ++i) {
    Search(pNode, 0, i);
  }
  pNode->Delete(new IntField(4));
  Search(pNode, 3, 6);
  pNode->Delete(new IntField(5), PageSlotID(1, 5));
  Search(pNode, 4, 6);
  delete pNode;
}

}  // namespace thdb
