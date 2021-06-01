#include "manager/recovery_manager.h"
#include <fstream>
#include <sstream>
using namespace std;
namespace thdb {

RecoveryManager::RecoveryManager(TransactionManager* _transactionManager,
    TableManager* _tableManager, WALManager* _walManager):
    transactionManager(_transactionManager),
    tableManager(_tableManager),
    walManager(_walManager){
   
}
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



}  // namespace thdb
