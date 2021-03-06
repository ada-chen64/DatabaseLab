#ifndef SQLGENERATOR_H_
#define SQLGENERATOR_H_

#include <chrono>
#include <random>
#include <vector>

#include "defines.h"

namespace thdb {

String StringGenerator(std::mt19937 &generator) {
  static const char alpha[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";
  static const char alphanum[] =
      "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";
  auto len = static_cast<uint32_t>(1 + generator() % 10);
  String tmp_s;
  tmp_s.reserve(len);
  tmp_s += alpha[generator() % (sizeof(alpha) - 1)];
  for (uint32_t i = 0; i < len - 1; i++) {
    tmp_s += alphanum[generator() % (sizeof(alphanum) - 1)];
  }
  return tmp_s;
}

std::vector<String> SqlGenerator() {
  std::vector<String> sqls;
  auto seed = 0;  // std::chrono::system_clock::now().time_since_epoch().count();
  std::mt19937 generator(seed);

  uint32_t data_num = 1 + generator() % 100;
  for (uint32_t i = 0; i < data_num; i++) {
    String sql_insert = "INSERT INTO Persons VALUES(";
    sql_insert += std::to_string(generator() % 10000) + ",";
    sql_insert += std::to_string(std::generate_canonical<float, 32>(generator));
    sql_insert += "'" + StringGenerator(generator) + "'";
    sql_insert += ");";
    sqls.push_back(sql_insert);
  }
  sqls.push_back("SELECT * FROM Persons;");
  sqls.push_back("DROP TABLE Persons;");
  return sqls;
}

}  // namespace thdb

#endif  // SQLGENERATOR_H_
