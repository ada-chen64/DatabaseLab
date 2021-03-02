#include "gtest/gtest.h"

#include "utils/bitmap.h"

namespace thdb {

TEST(Lab1, BitmapTest) {
    Bitmap *bitmap = new Bitmap(11);
    for (Size i = 0 ; i < 11; ++i)
        ASSERT_EQ(bitmap->Get(i), false);
    bitmap->Set(1);
    ASSERT_EQ(bitmap->Get(1), true);
    ASSERT_EQ(bitmap->Get(2), false);
    ASSERT_EQ(bitmap->Get(0), false);
    bitmap->Unset(1);
    ASSERT_EQ(bitmap->Get(1), false);
    delete bitmap;
}

} // namespace thdb
