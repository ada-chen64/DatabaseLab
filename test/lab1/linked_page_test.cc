#include "page/linked_page.h"

#include <iostream>

#include "gtest/gtest.h"
#include "macros.h"

namespace thdb {

TEST(Lab1, LinkedPageTest) {
  LinkedPage *pPage1 = new LinkedPage();
  EXPECT_EQ(pPage1->GetPrevID(), NULL_PAGE);
  EXPECT_EQ(pPage1->GetNextID(), NULL_PAGE);
  LinkedPage *pPage3 = new LinkedPage();

  pPage1->PushBack(pPage3);
  EXPECT_EQ(pPage1->GetPrevID(), NULL_PAGE);
  EXPECT_EQ(pPage1->GetNextID(), pPage3->GetPageID());
  EXPECT_EQ(pPage3->GetPrevID(), pPage1->GetPageID());
  EXPECT_EQ(pPage3->GetNextID(), NULL_PAGE);

  LinkedPage *pPage2 = new LinkedPage();
  std::cout << pPage1->GetPageID() << pPage2->GetPageID() << pPage3->GetPageID()
            << std::endl;
  pPage1->PushBack(pPage2);
  EXPECT_EQ(pPage1->GetPrevID(), NULL_PAGE);
  EXPECT_EQ(pPage1->GetNextID(), pPage2->GetPageID());
  EXPECT_EQ(pPage2->GetPrevID(), pPage1->GetPageID());
  EXPECT_EQ(pPage2->GetNextID(), pPage3->GetPageID());
  EXPECT_EQ(pPage3->GetPrevID(), pPage2->GetPageID());
  EXPECT_EQ(pPage3->GetNextID(), NULL_PAGE);

  delete pPage2;
  pPage1->PopBack();
  EXPECT_EQ(pPage1->GetPrevID(), NULL_PAGE);
  EXPECT_EQ(pPage1->GetNextID(), pPage3->GetPageID());
  EXPECT_EQ(pPage3->GetPrevID(), pPage1->GetPageID());
  EXPECT_EQ(pPage3->GetNextID(), NULL_PAGE);

  delete pPage3;
  pPage1->PopBack();
  EXPECT_EQ(pPage1->GetPrevID(), NULL_PAGE);
  EXPECT_EQ(pPage1->GetNextID(), NULL_PAGE);

  delete pPage1;
}

}  // namespace thdb
