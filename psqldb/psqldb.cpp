#include "psqldb.hpp"

namespace psqldb {

    // initate connection
    void PsqlDb::init(machineConfig& machineconfig, dbCredentials& dbcredentials) {

        //
        try {
            connHandler = std::make_shared<pqxx::connection>(dbcredentials.getConnString().c_str());
            if(connHandler->is_open()){
                LOG(INFO) << "Connected Database " << dbcredentials.dbname;

                createTableIfNotExists();
                prepareAllStatements();

                // Test Info Table
                insertInfoTbl(machineconfig.procname, machineconfig.lockId, machineconfig.numTopics, machineconfig.topics);

                // These Topics configs pass it
                // Test Lock Table
                std::string topic1 = "S3T1";
                std::string topic2 = "S3T2";
                insertLockTbl(111, topic1, true, true, true);
                insertLockTbl(222, topic2, true, true, false);

                connHandler->disconnect();
                LOG(INFO) << "Disconnected Database ";
            } else{
                LOG(INFO) << "Connection to DB FAIL";
            }

        } catch (const std::exception& exp){
            LOG(INFO) << exp.what();
        }

        //deleteTable();
    }


    // Create Table
    void PsqlDb::createTableIfNotExists() {
        pqxx::work W(*connHandler.get());
        W.exec(CREATEINFOTABLE);
        W.exec(CREATETOPICTABLE);
        W.commit();
    }


    // Delete Table careful detach all processes before this
    void PsqlDb::deleteTable() {
        pqxx::work W(*connHandler.get()); // transactional object
        W.exec(DROPTOPICTABLE); // exec sql
        W.commit(); // commit
        LOG(INFO) << "Table Deleted Successfully";

    }

    // Prep Insert Into Lock Table
    void PsqlDb::prepInsertTopicTbl(){
        connHandler->prepare("insertlocktbl", PREPINSERTTOPIC); // statement template
    }

    // Prep Insert Info Table
    void PsqlDb::prepInsertInfoTbl() {
        connHandler->prepare("insertinfotbl", PREPINSERTINFO); // statement template
    }

    // Insert Into Lock Table with Prepared Statement
    void PsqlDb::insertLockTbl(const int lockid, const std::string &topicname, const bool write_access, const bool read_access, const bool primary_up) {
        lockguard lock(this,lockid);
        pqxx::work W(*connHandler.get());
        auto W_invocation = W.prepared("insertlocktbl");
        invokeDynamic(W_invocation, lockid, topicname.c_str(), write_access, read_access, primary_up);
        pqxx::result res = W_invocation.exec();
        W.commit();
    }

    // Insert Into Info Table with Prepared Statement
    void PsqlDb::insertInfoTbl(const std::string &procname, const int lockid, const int numtopics, const std::string& topics) {
        lockguard lock(this,lockid);
        pqxx::work W(*connHandler.get());
        auto W_invocation = W.prepared("insertinfotbl");
        invokeDynamic(W_invocation, procname.c_str(), lockid, numtopics, topics.c_str());
        W_invocation.exec();
        W.commit();
    }

    // db advisory lock
    void PsqlDb::prepLock() {
        connHandler->prepare("lock", PREPLOCK);
    }

    // db advisory unlock
    void PsqlDb::prepUnlock() {
        connHandler->prepare("unlock", PREPUNLOCK);
    }

    // lock
    void PsqlDb::lock(const int lockid) const {
        pqxx::work W(*connHandler.get());
        auto W_invocation = W.prepared("lock");
        invokeDynamic(W_invocation,lockid);
        W_invocation.exec();
        W.commit();
    }

    // unlock
    void PsqlDb::unlock(const int lockid) const {
        pqxx::work W(*connHandler.get());
        auto W_invocation = W.prepared("unlock");
        invokeDynamic(W_invocation,lockid);
        W_invocation.exec();
        W.commit();
    }

    // prepare statements
    void PsqlDb::prepareAllStatements() {
        prepInsertInfoTbl();
        prepInsertTopicTbl();
        prepLock();
        prepUnlock();
    }


}