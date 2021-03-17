#include "page/leaf_page.h"

#include <iostream>

#include "field/fields.h"
#include "gtest/gtest.h"
#include "macros.h"

namespace thdb {

TEST(Lab2, LeafPageTest) {
  LeafPage* pLeaf = new LeafPage(4, FieldType::INT_TYPE);
  PageID nPageID = pLeaf->GetPageID();
  for (int i = 0; i < 9; ++i) {
    pLeaf->Insert(new IntField(i), PageSlotID(0, i));
  }
  for (int i = 0; i < 10; ++i) {
    Field* pLow = new IntField(0);
    Field* pHigh = new IntField(i);
    auto iRes = pLeaf->Range(pLow, pHigh);
    delete pLow;
    delete pHigh;
    printf("Range[0,%d]: ", i);
    for (const auto& it : iRes) printf("%u.%u ", it.first, it.second);
    printf("\n");
  }
  delete pLeaf;
  pLeaf = new LeafPage(nPageID);
  delete pLeaf;
}

}  // namespace thdb