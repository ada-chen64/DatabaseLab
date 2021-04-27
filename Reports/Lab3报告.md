# Lab3

陈昱霏 2017080067

## 实验任务：

本次实验有2个任务：

1. 合并Lab3新增的实验框架代码，保证合并后不影响Lab1,2原始测试
2. 实现一种或多种JOIN算法以完成至少支持**2表JOIN**的过程。

**Sort Merge Join的实现难度相对较低，同时可以取得一个不错的性能。如果能结合部分列上存在索引，能进一步优化Sort Merge Join性能**

## Lab3 样例

两个表的JOIN示例

```sh
> SELECT * FROM students;
 +--------+------------+-----------+
 | stu_id | first_name | last_name | 
 +--------+------------+-----------+
 |      1 |      James |     Smith | 
 |      2 |    Michael |   Johnson | 
 |      3 |     Thomas |     Brown | 
 +--------+------------+-----------+

> SELECT * FROM students_courses;
 +--------+-----------+
 | stu_id | course_id | 
 +--------+-----------+
 |      1 |         1 | 
 |      2 |         3 | 
 |      3 |         2 | 
 |      1 |         3 | 
 +--------+-----------+

> SELECT * FROM students, students_courses WHERE students.stu_id = students_courses.stu_id;
 +--------+------------+-----------+--------+-----------+
 | stu_id | first_name | last_name | stu_id | course_id | 
 +--------+------------+-----------+--------+-----------+
 |      1 |      James |     Smith |      1 |         1 | 
 |      1 |      James |     Smith |      1 |         3 | 
 |      2 |    Michael |   Johnson |      2 |         3 | 
 |      3 |     Thomas |     Brown |      3 |         2 | 
 +--------+------------+-----------+--------+-----------+

```

## Lab3 需要实现的接口

必要的顶层接口：

- system
  - instance
    - Join: 实现JOIN算法的顶层接口

## 实现方法

由于本次实验的数据库实现的时候简单的功能，暂且不考虑多个Join Condition，也只考虑两个Table。所以在获得Join Condition的时候先将其中的两个Table名取出，获得该表。然后将对应的列名取出，获得该列在对应的表格里存在在Record里的哪一个Field。

```c++
std::vector<String> totcolNames;
std::vector<Record *> newTable;
//printf("JOIN CONDITIONS: %d\n", iJoinConds.size());
JoinCondition *joinCond = dynamic_cast<JoinCondition*> (iJoinConds[0]);
String sTableA = joinCond->sTableA;//outer
String sTableB = joinCond->sTableB; //inner
String sColA = joinCond->sColA;
String sColB = joinCond->sColB;
FieldID iColA = GetColID(sTableA, sColA);
FieldID iColB = GetColID(sTableB, sColB);
Table *tableA = GetTable(sTableA);
Table *tableB = GetTable(sTableB);
```

我们不妨设A为外围B为内围。输出的Record的列就按照先A后B的格式存放。

```c++
std::vector<String> aColNames = tableA->GetColumnNames();
std::vector<String> bColNames = tableB->GetColumnNames();
totcolNames.insert(totcolNames.begin(), bColNames.begin(), bColNames.end());
totcolNames.insert(totcolNames.begin(), aColNames.begin(), aColNames.end());
```



Join的实现的最基本的方法是Nested Loop算法。由于我们本次实验只需要考虑两个Table，所以Nested Loop也只需要考虑两层for循环。基本的逻辑如下：

```
for recordA in TableA:
	for recordB in TableB:
		if recordA and recordB satisfies Join Condition:
			emit_join_result(recordA, recordB)
```

所以代码为：

```c++
for(auto iPairA: iResultMap[sTableA])
{
    Record *aRecord = GetRecord(sTableA, iPairA);
    Field *aField = aRecord->GetField(iColA);
    FieldType aType = aField->GetType();
    for(auto iPairB: iResultMap[sTableB])
    {
        Record *bRecord = GetRecord(sTableB, iPairB);
        Field *bField = bRecord->GetField(iColB);
        FieldType bType = bField->GetType();
        assert(bType == aType);
        if(Equal(aField, bField, aType))
        {
            //satisfies join cond
            Record *newRec = aRecord->Copy();
            newRec->Add(bRecord);
            newTable.push_back(newRec);
    	}
    }
}
```

然而该算法在执行上最大的缺点就是慢。执行的结果如下：

```
Database Init.
Build Finish.
[==========] Running 4 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 4 tests from Lab3
[ RUN      ] Lab3.BasicJoinTest
[       OK ] Lab3.BasicJoinTest (59 ms)
[ RUN      ] Lab3.IndexJoinTest
[       OK ] Lab3.IndexJoinTest (16 ms)
[ RUN      ] Lab3.DuplicateJoinTest
[       OK ] Lab3.DuplicateJoinTest (17 ms)
[ RUN      ] Lab3.RandomTest
[       OK ] Lab3.RandomTest (17357 ms)
[----------] 4 tests from Lab3 (17454 ms total)

[----------] Global test environment tear-down
[==========] 4 tests from 1 test suite ran. (17455 ms total)
[  PASSED  ] 4 tests.

```

所以我把Nested Loop算法换位Heap Join算法。Heap Join的算法的概念如下：

```
for recordA in TableA:
	hash_table[recordA.key] = recordA
for recordB in TableB:
	record = hash_tale[recordB.key]
	if(record AND record.key == recordB.key):
		emit_join_result(record, recordB)
```

然而我要考虑如果在TableA中有多个相同key的record，所以我把哈希的value改为vector<Record*>。这里要注意Key要用String，因为用Field指针是不对等的。

```c++
std::map<String, std::vector<Record*>> hashmap;
for(auto iPair: iResultMap[sTableA])
{
    Record *aRecord = GetRecord(sTableA, iPair);
    Field *aField = aRecord->GetField(iColA);
    if(hashmap.find(aField->ToString()) == hashmap.end())
    {
        hashmap[aField->ToString()] = {aRecord};
    }
    else
        hashmap[aField->ToString()].push_back(aRecord);

}
for(auto iPair: iResultMap[sTableB])
{
    Record *bRecord = GetRecord(sTableB, iPair);
    Field *bField = bRecord->GetField(iColA);
    std::vector<Record*> aRecords = hashmap[bField->ToString()];
    for(auto record: aRecords)
    {
        Record *newRec = record->Copy();
        newRec->Add(bRecord);
        newTable.push_back(newRec);
    }
}
```

这样执行起来就快多了。结果如下：

```
Database Init.
Build Finish.
[==========] Running 4 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 4 tests from Lab3
[ RUN      ] Lab3.BasicJoinTest
[       OK ] Lab3.BasicJoinTest (32 ms)
[ RUN      ] Lab3.IndexJoinTest
[       OK ] Lab3.IndexJoinTest (12 ms)
[ RUN      ] Lab3.DuplicateJoinTest
[       OK ] Lab3.DuplicateJoinTest (9 ms)
[ RUN      ] Lab3.RandomTest
[       OK ] Lab3.RandomTest (3735 ms)
[----------] 4 tests from Lab3 (3789 ms total)

[----------] Global test environment tear-down
[==========] 4 tests from 1 test suite ran. (3789 ms total)
[  PASSED  ] 4 tests.

```

可见用Heap Join算法比Nested Loop要快很多。