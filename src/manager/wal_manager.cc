#include "manager/wal_manager.h"
#include "defines.h"
#include <fstream>
using namespace std;

namespace thdb{

    WALManager::WALManager(){

    }
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
    void WALManager::deleteLog(){
        ofstream wfile;
        wfile.open(LOGPATH, ofstream::out | ofstream::trunc);
        wfile.close();
    }

}