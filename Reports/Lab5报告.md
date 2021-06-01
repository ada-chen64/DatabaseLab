# Lab5

陈昱霏 2017080067

## Lab5 实验说明

本次实验主题为数据库故障恢复，要求同学们通过 WAL 机制，实现简单的故障恢复功能，实验前可先复习一下 WAL，Steal/No-steal，Force/No-force 策略的相关理论。

实验前先合并 lab5 新增的实验代码，若合并出现冲突需手动解决，合并后确保不影响前 4 次实验测试的正确性。

## 框架主要变更

新增类：RecoveryManager。

## Lab5 需要实现的接口

- manager

  - RecoveryManager：恢复管理器
    - Redo
    - Undo

本次实验为本学期最后一次实验，实现灵活性较大，只要不影响外部接口，可根据自己的实现方案调整内部逻辑，实现过程中可能需要设计新的类。

## Lab5实现思路

我是用的是Steal/No Force策略，使用WAL来记录数据库里所发生的改变。我的log格式如下

ActionType#TableName#PageID#SlotID#OldRecord#NewRecord

其中ActionType分INSERT, DELETE, UPDATE,以及COMMIT。

我创建了一个WALManager类，其中记录了我的log的路径LOGPATH。

```c++
class WALManager{
 public:
  WALManager();
  ~WALManager() = default;
  void writeLog(String tableName, PageID nPageID, SlotID nSlotID,
            ActionType action, String oldRecord, String newRecord);
  void deleteLog();
  String LOGPATH = "LOG.txt";
};
```

writeLog函数中使用了fstream按照上面的log格式写入日志中。

```c++
void WALManager::writeLog(String tableName, PageID nPageID, SlotID nSlotID,
                          ActionType action, String oldRecord, String newRecord){
    ofstream wfile;
    wfile.open(LOGPATH, ios_base::app);
    if(action == ActionType::INSERT_TYPE)
        wfile << "INSERT#";
    else if(action == ActionType::UPDATE_TYPE)
        wfile << "UPDATE#";
    else if(action == ActionType::DELETE_TYPE)
        wfile << "DELETE#";
    else if(action == ActionType::COMMIT_TYPE)
        wfile<< "COMMIT#";
    wfile << tableName << "#";
    wfile << nPageID << "#";
    wfile << nSlotID << "#";
    wfile << oldRecord << "#";
    wfile << newRecord << endl;
}
```

每当txn开始的时候我会调用deleteLog清除日志，这样日志不受上一个日志的影响。

在Instance中的Insert，Delete，Update操作中我都会使用writeLog来记录日志。TransactionManager中的commit操作中也会调用writeLog。

在Instance重启时会初始化TableManager，TransactionManager，WALManager，以及RecoveryManager。我将初始化的TableManager，TransactionManager，和WALManager通过构造函数送入RecoveryManager，它就可以调用Table和日志。



**Redo**的意义是如果表格在调用commit之后还没有来得及更新所有该更新的地方之前就崩溃掉，可以在重启时将该完成的操作都完成。

**Undo**的意义是如果在表格没有commit的时候突然崩溃，那么写入或者删除操作就不算数，必须退回。

在RecoveryManager中的Redo函数，我们读取Log中所有行，将每一行拆分为vector\<String\>，作为我们log中的一项。在处理读入的时候我们检查有没有commit操作。如果没有commit操作的话，Redo作废直接return。如果有commit操作那我们会按照顺序在表格中一一检查。对于Insert来说，用日志中的PageID和SlotID从表格中读取record，如果该record和日志中的record吻合则不需要任何操作，如果不吻合，或者该record是一个空的话，那么调用Insert。



```c++
void RecoveryManager::Redo() {
    bool hasCOMMIT = false;
    int numRecords = 0;
    vector<String> log_line;
    vector<vector<String>> log_Act;
    ifstream log_read;
    log_read.open(walManager->LOGPATH.c_str());
    String line;
    while(getline(log_read, line))
    {
        log_line.push_back(line);
        
    }
    for(int i = 0; i < log_line.size(); i++)
    {
        vector<String> log_rec;
        stringstream l_line(log_line[i]);
        String seg;
        while(getline(l_line, seg, '#'))
        {
            log_rec.push_back(seg);
        }
        if(log_rec[0].compare("COMMIT") == 0)
            hasCOMMIT = true;
        else if(log_rec[0].compare("INSERT")== 0)
            numRecords++;
        else if(log_rec[0].compare("DELETE")== 0)
            numRecords--;
        log_Act.push_back(log_rec);
    }
    if(!hasCOMMIT)
        return;

    for(int i = 0; i <log_Act.size(); i++)
    {
        if(log_Act[i][0].compare("COMMIT") == 0)
            continue;
        if(log_Act[i][0].compare("INSERT") == 0)
        {
            Table* _ptable = tableManager->GetTable(log_Act[i][tablename]);
            Record *precord = _ptable->GetRecord(stoi(log_Act[i][pagenum]), 
                        stoi(log_Act[i][slotnum]));

            if(precord->ToString().compare(log_Act[i][newrec]) != 0)
            {
                vector<String> Rawvec;
                stringstream fields(log_Act[i][newrec]);
                String seg;
                while(getline(fields, seg, ' '))
                {
                    Rawvec.push_back(seg);
                }
                Record *nRecord = _ptable->EmptyRecord();
                nRecord->Build(Rawvec);
                _ptable->InsertRecord(nRecord);
            }
        }
    }
    
}
```

Undo在读取日志上步骤与Redo一样，只是后面的操作相反。如果Undo读到Commit的话直接退出，否则就按逆顺序将所有操作都反执行一遍，也就是说如果读到Insert就Delete，如果读到Update就更新回久版本。

```c++
void RecoveryManager::Undo() {
    bool hasCOMMIT = false;
    int numRecords = 0;
    vector<String> log_line;
    vector<vector<String>> log_Act;
    ifstream log_read;
    log_read.open(walManager->LOGPATH.c_str());
    String line;
    while(getline(log_read, line))
    {
        log_line.push_back(line);    
    }

    for(int i = 0; i < log_line.size(); i++)
    {
        vector<String> log_rec;
        stringstream l_line(log_line[i]);
        String seg;
        while(getline(l_line, seg, '#'))
        {
            log_rec.push_back(seg);
        }
        if(log_rec[0].compare("COMMIT") == 0)
            hasCOMMIT = true;
        else if(log_rec[0].compare("INSERT")== 0)
            numRecords++;
        else if(log_rec[0].compare("DELETE")== 0)
            numRecords--;
        log_Act.push_back(log_rec);
    }
    for(int i = log_Act.size() - 1; i >= 0; i--)
    {
        
        if(log_Act[i][0].compare("COMMIT") == 0)
            return;
        else if(log_Act[i][0].compare("INSERT") == 0)
        {
            Table* _ptable = tableManager->GetTable(log_Act[i][tablename]);
            _ptable->DeleteRecord(stoi(log_Act[i][pagenum]), stoi(log_Act[i][slotnum]));
        }
    }
}
```

由于本次实验的测试只考虑Insert，我就只考虑了Insert。之后还需要添加Update，Delete，等操作。