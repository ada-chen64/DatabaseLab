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
  auto seed =
      0;  // std::chrono::system_clock::now().time_since_epoch().count();
  std::mt19937 generator(seed);
  String table_name = StringGenerator(generator);
  String sql_create_table = "CREATE TABLE " + table_name + "(";

  uint32_t column_num = 1 + generator() % 10;
  std::vector<String> column_names;
  column_names.reserve(column_num);
  std::vector<String> column_types;
  column_types.reserve(column_num);

  String types[3] = {"INT", "FLOAT", "VARCHAR(50)"};
  for (uint32_t i = 0; i < column_num; i++) {
    column_names.push_back(StringGenerator(generator));
    column_types.push_back(types[generator() % 3]);
  }
  for (uint32_t i = 0; i < column_num; i++) {
    sql_create_table += " " + column_names[i] + " " + column_types[i];
    if (i != column_num - 1) {
      sql_create_table += ",";
    }
  }
  sql_create_table += ");";
  sqls.push_back(sql_create_table);

  uint32_t data_num = 1 + generator() % 100;
  for (uint32_t i = 0; i < data_num; i++) {
    String sql_insert = "INSERT INTO " + table_name + " VALUES(";
    for (uint32_t j = 0; j < column_num; j++) {
      if (column_types[j] == "INT") {
        sql_insert += std::to_string(generator() % 1000);
      } else if (column_types[j] == "FLOAT") {
        sql_insert +=
            std::to_string(std::generate_canonical<float, 32>(generator));
      } else if (column_types[j] == "VARCHAR(50)") {
        sql_insert += "'" + StringGenerator(generator) + "'";
      }
      if (j != column_num - 1) {
        sql_insert += ",";
      }
    }
    sql_insert += ");";
    sqls.push_back(sql_insert);
  }
  String sql_select = "SELECT * FROM " + table_name + ";";
  sqls.push_back(sql_select);
  String sql_drop_table = "DROP TABLE " + table_name + ";";
  sqls.push_back(sql_drop_table);
  return sqls;
}

}  // namespace thdb

#endif  // SQLGENERATOR_H_
