#ifndef SQLGENERATOR_H_
#define SQLGENERATOR_H_

#include <chrono>
#include <random>
#include <vector>

#include "defines.h"

namespace thdb {

String StringGenerator(std::mt19937 &generator) {
  static const char alphanum[] =
      "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";
  auto len = static_cast<uint32_t>(1 + generator() % 10);
  String tmp_s;
  tmp_s.reserve(len);
  for (uint32_t i = 0; i < len; i++) {
    tmp_s += alphanum[generator() % (sizeof(alphanum) - 1)];
  }
  return tmp_s;
}

std::vector<String> SqlGenerator(size_t data_num, size_t sample_num) {
  std::vector<String> sqls;
  sqls.push_back("CREATE TABLE Persons(ID INT, FirstName VARCHAR(20), LastName VARCHAR(20), Temperature FLOAT);");
  auto seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::mt19937 generator(seed);

  for (size_t i = 0; i < data_num; i++) {
    String sql_insert = "INSERT INTO Persons VALUES(";
    sql_insert += std::to_string(i) + ",";
    sql_insert += "'" + StringGenerator(generator) + "',";
    sql_insert += "'" + StringGenerator(generator) + "',";
    sql_insert += std::to_string(36 + std::generate_canonical<float, 32>(generator));
    sql_insert += ");";
    sqls.push_back(sql_insert);
  }
  sqls.push_back("ALTER TABLE Persons ADD INDEX(ID);");
  sqls.push_back("SELECT * FROM Persons;");
  for (size_t i = 0; i < sample_num; i++) {
    String sql_sample = "SELECT * FROM Persons WHERE Persons.ID = " + std::to_string(generator() % data_num) + ";";
    sqls.push_back(sql_sample);
  }
  sqls.push_back("DROP TABLE Persons;");
  return sqls;
}

}  // namespace thdb

#endif  // SQLGENERATOR_H_
