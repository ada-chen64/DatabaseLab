#include "field/float_field.h"
#include "gtest/gtest.h"

namespace thdb {

TEST(Lab1, FieldTest) {
  Field *float_field = new FloatField();
  delete float_field;
}

}  // namespace thdb
