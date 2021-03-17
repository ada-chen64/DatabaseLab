#include "page/leaf_page.h"

#include <iostream>

#include "field/fields.h"
#include "gtest/gtest.h"
#include "macros.h"

namespace thdb {

void Search(LeafPage* pLeaf, int nLow, int nHigh) {
  Field* pLow = new IntField(nLow);
  Field* pHigh = new IntField(nHigh);
  auto iRes = pLeaf->Range(pLow, pHigh);
  delete pLow;
  delete pHigh;
  printf("Range[%d,%d]: ", nLow, nHigh);
  for (const auto& it : iRes) printf("%u.%u ", it.first, it.second);
  printf("\n");
}

TEST(Lab2, LeafPageTest) {
  LeafPage* pLeaf = new LeafPage(4, FieldType::INT_TYPE);
  PageID nPageID = pLeaf->GetPageID();
  for (int i = 0; i < 9; ++i) {
    pLeaf->Insert(new IntField(i), PageSlotID(0, i));
  }
  pLeaf->Insert(new IntField(5), PageSlotID(1, 5));
  pLeaf->Insert(new IntField(5), PageSlotID(2, 5));
  for (int i = 0; i < 10; ++i) {
    Search(0, i);
  }
  pLeaf->Delete(new IntField(4));
  Search(3, 6);
  delete pLeaf;
  pLeaf = new LeafPage(nPageID);
  delete pLeaf;
}

}  // namespace thdb