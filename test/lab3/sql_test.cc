#include "backend/backend.h"
#include "gtest/gtest.h"
#include "system/instance.h"

namespace thdb {

void check_equal(Instance *pDB, const std::vector<String> &iSQLVec, const std::vector<String> results) {
  ASSERT_EQ(iSQLVec.size(), results.size());
  for (uint32_t i = 0; i < iSQLVec.size(); i++) {
    std::vector<Result *> iResVec = Execute(pDB, iSQLVec[i]);
    // 由于测试 sql 每行只有一条 sql 语句，因此 iResVec 的大小始终为 1.
    EXPECT_EQ(iResVec.size(), 1);
    // 比较 sql 执行结果是否一致
    EXPECT_EQ(iResVec[0]->ToString(), results[i]);
  }
}

TEST(Lab3, BasicJoinTest) {
  Instance *pDB = new Instance();
  std::vector<String> iSQLVec = {
      "CREATE TABLE students(stu_id INT, first_name VARCHAR(20), last_name VARCHAR(20));",          // NOLINT
      "CREATE TABLE students_courses(stu_id INT, course_id INT);",                                  // NOLINT
      "INSERT INTO students VALUES(1, 'James', 'Smith');",                                          // NOLINT
      "INSERT INTO students VALUES(2, 'Michael', 'Johnson');",                                      // NOLINT
      "INSERT INTO students VALUES(3, 'Thomas', 'Brown');",                                         // NOLINT
      "INSERT INTO students_courses VALUES(1, 1);",                                                 // NOLINT
      "INSERT INTO students_courses VALUES(2, 3);",                                                 // NOLINT
      "INSERT INTO students_courses VALUES(3, 2);",                                                 // NOLINT
      "INSERT INTO students_courses VALUES(1, 3);",                                                 // NOLINT
      "SELECT * FROM students, students_courses WHERE students.stu_id = students_courses.stu_id;",  // NOLINT
      "DROP TABLE students;",                                                                       // NOLINT
      "DROP TABLE students_courses;"                                                                // NOLINT
  };
  std::vector<String> results = {
      "1\n",                                                                                // NOLINT
      "1\n",                                                                                // NOLINT
      "1\n",                                                                                // NOLINT
      "1\n",                                                                                // NOLINT
      "1\n",                                                                                // NOLINT
      "1\n",                                                                                // NOLINT
      "1\n",                                                                                // NOLINT
      "1\n",                                                                                // NOLINT
      "1\n",                                                                                // NOLINT
      "1,James,Smith,1,1\n2,Michael,Johnson,2,3\n3,Thomas,Brown,3,2\n1,James,Smith,1,3\n",  // NOLINT
      "1\n",                                                                                // NOLINT
      "1\n"                                                                                 // NOLINT
  };
  check_equal(pDB, iSQLVec, results);
  delete pDB;
}

TEST(Lab3, DISABLED_MultiTableJoinTest) {
  Instance *pDB = new Instance();
  std::vector<String> iSQLVec = {
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
      "INSERT INTO courses VALUES(3, 'database system');",                                  // NOLINT
      "SELECT * FROM students, students_courses, courses WHERE students.stu_id = students_courses.stu_id AND "
      "students_courses.course_id = courses.course_id;",  // NOLINT
      "DROP TABLE students;",                             // NOLINT
      "DROP TABLE students_courses;",                     // NOLINT
      "DROP TABLE courses;"                               // NOLINT
  };
  std::vector<String> results = {
      "1\n",  // NOLINT
      "1\n",  // NOLINT
      "1\n",  // NOLINT
      "1\n",  // NOLINT
      "1\n",  // NOLINT
      "1\n",  // NOLINT
      "1\n",  // NOLINT
      "1\n",  // NOLINT
      "1\n",  // NOLINT
      "1\n",  // NOLINT
      "1\n",  // NOLINT
      "1\n",  // NOLINT
      "1\n",  // NOLINT
      "1,James,Smith,1,1,calculus\n2,Michael,Johnson,2,3,database system\n3,Thomas,Brown,3,2,linear "
      "algebra\n1,James,Smith,1,3,database system\n",  // NOLINT
      "1\n",                                           // NOLINT
      "1\n",                                           // NOLINT
      "1\n"                                            // NOLINT
  };
  check_equal(pDB, iSQLVec, results);
  delete pDB;
}

TEST(Lab3, IndexJoinTest) {
  Instance *pDB = new Instance();
  std::vector<String> iSQLVec = {
      "CREATE TABLE students(stu_id INT, first_name VARCHAR(20), last_name VARCHAR(20));",  // NOLINT
      "CREATE TABLE students_courses(stu_id INT, course_id INT);",                          // NOLINT
      "CREATE TABLE courses(course_id INT, course_name VARCHAR(30));",                      // NOLINT
      "ALTER TABLE students ADD INDEX(stu_id);",                                            // NOLINT
      "ALTER TABLE students_courses ADD INDEX(stu_id);",                                    // NOLINT
      "ALTER TABLE students_courses ADD INDEX(course_id);",                                 // NOLINT
      "ALTER TABLE courses ADD INDEX(course_id);",                                          // NOLINT
      "INSERT INTO students VALUES(1, 'James', 'Smith');",                                  // NOLINT
      "INSERT INTO students VALUES(2, 'Michael', 'Johnson');",                              // NOLINT
      "INSERT INTO students VALUES(3, 'Thomas', 'Brown');",                                 // NOLINT
      "INSERT INTO students_courses VALUES(1, 1);",                                         // NOLINT
      "INSERT INTO students_courses VALUES(2, 3);",                                         // NOLINT
      "INSERT INTO students_courses VALUES(3, 2);",                                         // NOLINT
      "INSERT INTO students_courses VALUES(1, 3);",                                         // NOLINT
      "INSERT INTO courses VALUES(1, 'calculus');",                                         // NOLINT
      "INSERT INTO courses VALUES(2, 'linear algebra');",                                   // NOLINT
      "INSERT INTO courses VALUES(3, 'database system');",                                  // NOLINT
      "SELECT * FROM students, students_courses, courses WHERE students.stu_id = students_courses.stu_id AND "
      "students_courses.course_id = courses.course_id;",  // NOLINT
      "DROP TABLE students;",                             // NOLINT
      "DROP TABLE students_courses;",                     // NOLINT
      "DROP TABLE courses;"                               // NOLINT
  };
  std::vector<String> results = {
      "1\n",  // NOLINT
      "1\n",  // NOLINT
      "1\n",  // NOLINT
      "1\n",  // NOLINT
      "1\n",  // NOLINT
      "1\n",  // NOLINT
      "1\n",  // NOLINT
      "1\n",  // NOLINT
      "1\n",  // NOLINT
      "1\n",  // NOLINT
      "1\n",  // NOLINT
      "1\n",  // NOLINT
      "1\n",  // NOLINT
      "1\n",  // NOLINT
      "1\n",  // NOLINT
      "1\n",  // NOLINT
      "1\n",  // NOLINT
      "1,James,Smith,1,1,calculus\n2,Michael,Johnson,2,3,database system\n3,Thomas,Brown,3,2,linear "
      "algebra\n1,James,Smith,1,3,database system\n",  // NOLINT
      "1\n",                                           // NOLINT
      "1\n",                                           // NOLINT
      "1\n"                                            // NOLINT
  };
  check_equal(pDB, iSQLVec, results);
  delete pDB;
}

}  // namespace thdb
