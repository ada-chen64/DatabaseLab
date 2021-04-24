#include <algorithm>

#include "backend/backend.h"
#include "gtest/gtest.h"
#include "system/instance.h"

namespace thdb {

bool CheckEqual(std::vector<String> lhs, std::vector<String> rhs) {
  std::sort(lhs.begin(), lhs.end());
  std::sort(rhs.begin(), rhs.end());
  EXPECT_EQ(lhs.size(), rhs.size());
  for (size_t i = 0; i < lhs.size(); i++) {
    if (lhs[i] != rhs[i]) {
      return false;
    }
  }
  return true;
}

void CheckResults(Instance *pDB, const std::vector<String> &iSQLVec, const std::vector<String> results) {
  ASSERT_EQ(iSQLVec.size(), results.size());
  for (uint32_t i = 0; i < iSQLVec.size(); i++) {
    std::vector<Result *> iResVec = Execute(pDB, iSQLVec[i]);
    // 由于测试 sql 每行只有一条 sql 语句，因此 iResVec 的大小始终为 1.
    EXPECT_EQ(iResVec.size(), 1);
    // 比较 sql 执行结果是否一致
    // CheckEqual(iResVec[0]->ToVector(), results[i]);
  }
}

void ExecuteBatch(Instance *pDB, const std::vector<String> &iSQLVec) {
  for (const auto &sql : iSQLVec) {
    Execute(pDB, sql);
  }
}

TEST(Lab3, BasicJoinTest) {
  Instance *pDB = new Instance();
  std::vector<String> create_and_insert = {
      "CREATE TABLE students(stu_id INT, first_name VARCHAR(20), last_name VARCHAR(20));",  // NOLINT
      "CREATE TABLE students_courses(stu_id INT, course_id INT);",                          // NOLINT
      "INSERT INTO students VALUES(1, 'James', 'Smith');",                                  // NOLINT
      "INSERT INTO students VALUES(2, 'Michael', 'Johnson');",                              // NOLINT
      "INSERT INTO students VALUES(3, 'Thomas', 'Brown');",                                 // NOLINT
      "INSERT INTO students_courses VALUES(1, 1);",                                         // NOLINT
      "INSERT INTO students_courses VALUES(2, 3);",                                         // NOLINT
      "INSERT INTO students_courses VALUES(3, 2);",                                         // NOLINT
      "INSERT INTO students_courses VALUES(1, 3);",                                         // NOLINT // NOLINT
  };
  std::vector<String> select = {
      "SELECT * FROM students, students_courses WHERE students.stu_id = students_courses.stu_id;",  // NOLINT
  };
  std::vector<String> results_1 = {
      "1,James,Smith,1,1",      // NOLINT
      "1,James,Smith,1,3",      // NOLINT
      "2,Michael,Johnson,2,3",  // NOLINT
      "3,Thomas,Brown,3,2"      // NOLINT
  };
  std::vector<String> results_2 = {
      "1,1,1,James,Smith",      // NOLINT
      "1,3,1,James,Smith",      // NOLINT
      "2,3,2,Michael,Johnson",  // NOLINT
      "3,2,3,Thomas,Brown"      // NOLINT
  };
  std::vector<String> drop = {
      "DROP TABLE students;",         // NOLINT
      "DROP TABLE students_courses;"  // NOLINT
  };
  ExecuteBatch(pDB, create_and_insert);
  std::vector<String> execute_results = Execute(pDB, select[0])[0]->ToVector();
  EXPECT_TRUE(CheckEqual(execute_results, results_1) || CheckEqual(execute_results, results_2));
  ExecuteBatch(pDB, drop);
  delete pDB;
}

TEST(Lab3, DISABLED_MultiTableJoinTest) {
  Instance *pDB = new Instance();
  std::vector<String> create_and_insert = {
      "CREATE TABLE students(stu_id INT, first_name VARCHAR(20), last_name VARCHAR(20));",  // NOLINT
      "CREATE TABLE students_courses(stu_id INT, course_id INT);",                          // NOLINT
      "CREATE TABLE courses(course_id INT, course_name VARCHAR(30));",                      // NOLINT
      "INSERT INTO students VALUES(1, 'James', 'Smith');",                                  // NOLINT
      "INSERT INTO students VALUES(2, 'Michael', 'Johnson');",                              // NOLINT
      "INSERT INTO students VALUES(3, 'Thomas', 'Brown');",                                 // NOLINT
      "INSERT INTO students_courses VALUES(1, 1);",                                         // NOLINT
      "INSERT INTO students_courses VALUES(2, 3);",                                         // NOLINT
      "INSERT INTO students_courses VALUES(3, 2);",                                         // NOLINT
      "INSERT INTO students_courses VALUES(1, 3);",                                         // NOLINT
      "INSERT INTO courses VALUES(1, 'calculus');",                                         // NOLINT
      "INSERT INTO courses VALUES(2, 'linear algebra');",                                   // NOLINT
      "INSERT INTO courses VALUES(3, 'database system');"                                   // NOLINT
  };
  std::vector<String> select = {
      "SELECT * FROM students, students_courses, courses "
      "WHERE students.stu_id = students_courses.stu_id "
      "AND students_courses.course_id = courses.course_id;",  // NOLINT
  };
  std::vector<String> results = {
      "1,James,Smith,1,1,1,calculus",             // NOLINT
      "2,Michael,Johnson,2,3,3,database system",  // NOLINT
      "3,Thomas,Brown,3,2,2,linear algebra",      // NOLINT
      "1,James,Smith,1,3,3,database system"       // NOLINT
  };
  std::vector<String> drop = {
      "DROP TABLE students;",          // NOLINT
      "DROP TABLE students_courses;",  // NOLINT
      "DROP TABLE courses"             // NOLINT
  };
  ExecuteBatch(pDB, create_and_insert);
  std::vector<String> execute_results = Execute(pDB, select[0])[0]->ToVector();
  EXPECT_TRUE(CheckEqual(execute_results, results));
  ExecuteBatch(pDB, drop);
  delete pDB;
}

TEST(Lab3, IndexJoinTest) {
  Instance *pDB = new Instance();
  std::vector<String> create_and_insert = {
      "CREATE TABLE students(stu_id INT, first_name VARCHAR(20), last_name VARCHAR(20));",  // NOLINT
      "CREATE TABLE students_courses(stu_id INT, course_id INT);",                          // NOLINT
      "ALTER TABLE students ADD INDEX(stu_id);",                                            // NOLINT
      "ALTER TABLE students_courses ADD INDEX(stu_id);",                                    // NOLINT
      "ALTER TABLE students_courses ADD INDEX(course_id);",                                 // NOLINT
      "INSERT INTO students VALUES(1, 'James', 'Smith');",                                  // NOLINT
      "INSERT INTO students VALUES(2, 'Michael', 'Johnson');",                              // NOLINT
      "INSERT INTO students VALUES(3, 'Thomas', 'Brown');",                                 // NOLINT
      "INSERT INTO students_courses VALUES(1, 1);",                                         // NOLINT
      "INSERT INTO students_courses VALUES(2, 3);",                                         // NOLINT
      "INSERT INTO students_courses VALUES(3, 2);",                                         // NOLINT
      "INSERT INTO students_courses VALUES(1, 3);",                                         // NOLINT
  };
  std::vector<String> select = {
      "SELECT * FROM students, students_courses WHERE students.stu_id = students_courses.stu_id;",  // NOLINT
  };
  std::vector<String> results_1 = {
      "1,James,Smith,1,1",      // NOLINT
      "1,James,Smith,1,3",      // NOLINT
      "2,Michael,Johnson,2,3",  // NOLINT
      "3,Thomas,Brown,3,2"      // NOLINT
  };
  std::vector<String> results_2 = {
      "1,1,1,James,Smith",      // NOLINT
      "1,3,1,James,Smith",      // NOLINT
      "2,3,2,Michael,Johnson",  // NOLINT
      "3,2,3,Thomas,Brown"      // NOLINT
  };
  std::vector<String> drop = {
      "DROP TABLE students;",         // NOLINT
      "DROP TABLE students_courses;"  // NOLINT
  };
  ExecuteBatch(pDB, create_and_insert);
  std::vector<String> execute_results = Execute(pDB, select[0])[0]->ToVector();
  EXPECT_TRUE(CheckEqual(execute_results, results_1) || CheckEqual(execute_results, results_2));
  ExecuteBatch(pDB, drop);
  delete pDB;
}

TEST(Lab3, DuplicateJoinTest) {
  Instance *pDB = new Instance();
  std::vector<String> create_and_insert = {
      "CREATE TABLE A(id INT, info VARCHAR(20));",  // NOLINT
      "CREATE TABLE B(id INT, text VARCHAR(20));",  // NOLINT                                        // NOLINT
      "INSERT INTO A VALUES(1, 'A_a');",            // NOLINT
      "INSERT INTO A VALUES(1, 'A_b');",            // NOLINT
      "INSERT INTO A VALUES(1, 'A_c');",            // NOLINT
      "INSERT INTO A VALUES(2, 'A_d');",            // NOLINT
      "INSERT INTO A VALUES(2, 'A_e');",            // NOLINT
      "INSERT INTO A VALUES(2, 'A_f');",            // NOLINT
      "INSERT INTO B VALUES(1, 'B_a');",            // NOLINT
      "INSERT INTO B VALUES(1, 'B_b');",            // NOLINT
      "INSERT INTO B VALUES(2, 'B_c');",            // NOLINT
      "INSERT INTO B VALUES(2, 'B_d');",            // NOLINT
  };
  std::vector<String> select = {"SELECT * FROM A, B WHERE A.id = B.id;"};
  std::vector<String> results_1 = {"1,A_a,1,B_a", "1,A_a,1,B_b", "1,A_b,1,B_a", "1,A_b,1,B_b",
                                   "1,A_c,1,B_a", "1,A_c,1,B_b", "2,A_d,2,B_c", "2,A_d,2,B_d",
                                   "2,A_e,2,B_c", "2,A_e,2,B_d", "2,A_f,2,B_c", "2,A_f,2,B_d"};
  std::vector<String> results_2 = {"1,B_a,1,A_a", "1,B_b,1,A_a", "1,B_a,1,A_b", "1,B_b,1,A_b",
                                   "1,B_a,1,A_c", "1,B_b,1,A_c", "2,B_c,2,A_d", "2,B_d,2,A_d",
                                   "2,B_c,2,A_e", "2,B_d,2,A_e", "2,B_d,2,A_f", "2,B_d,2,A_f"};
  std::vector<String> drop = {
      "DROP TABLE A;",  // NOLINT
      "DROP TABLE B;"   // NOLINT
  };
  ExecuteBatch(pDB, create_and_insert);
  std::vector<String> execute_results = Execute(pDB, select[0])[0]->ToVector();
  EXPECT_TRUE(CheckEqual(execute_results, results_1) || CheckEqual(execute_results, results_2));
  ExecuteBatch(pDB, drop);
  delete pDB;
}

}  // namespace thdb
