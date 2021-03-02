#include "gtest/gtest.h"

#include "page/linked_page.h"
#include "macros.h"

namespace thdb {

TEST(Lab1, StartTest) {
    LinkedPage *pPage1 = new LinkedPage();
    EXPECT_EQ(pPage1->GetPrevID(), NULL_PAGE);
    EXPECT_EQ(pPage1->GetNextID(), NULL_PAGE);
    LinkedPage *pPage2 = new LinkedPage();
    pPage1->PushBack(pPage2);
    EXPECT_EQ(pPage1->GetPageID(), pPage2->GetPrevID());
    EXPECT_EQ(pPage1->GetNextID(), pPage2->GetPageID());
    EXPECT_EQ(pPage1->GetPrevID(), NULL_PAGE);
    EXPECT_EQ(pPage2->GetNextID(), NULL_PAGE);
    delete pPage2;
    pPage1->PopBack();
    EXPECT_EQ(pPage1->GetNextID(), NULL_PAGE);
    delete pPage1;
}

} // namespace thdb
