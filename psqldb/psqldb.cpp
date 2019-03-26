#include "psqldb.hpp"

namespace psqldb {

    // initate connection
    bool PsqlDb::init(machineConfig& machineconfig, dbCredentials& dbcredentials, std::vector<recordertest>& topics) {

        try {
            connHandler = std::make_shared<pqxx::connection>(dbcredentials.getConnString().c_str());
            if(connHandler->is_open()){
                LOG(INFO) << "Connected Database " << dbcredentials.dbname;

                createTableIfNotExists();
                prepareAllStatements();

                // Info Table Test
                insertInfoTbl(machineconfig.procname, machineconfig.numrecorders, machineconfig.prim_recs, machineconfig.sec_recs);

                // Insert some topics
                for(const auto &each : topics)
                    insertLockTbl(each.lockid,each.topic,each.write,each.read,each.primary);

                // db good
                return true;
            } else {
                return false;
            }
        } catch (const std::exception& exp){
            LOG(INFO) << exp.what();
        }
    }

    /**
     * Functions to manage Database
     * */
    // Create Table
    void PsqlDb::createTableIfNotExists() {
        pqxx::work W(*connHandler.get());
        W.exec(CREATEINFOTABLE);
        W.exec(CREATETOPICTABLE);
        W.commit();
    }

    // Delete Table !!: other processes might use this table careful
    void PsqlDb::dropLockTable() {
        pqxx::work W(*connHandler.get()); // transactional object
        W.exec(DROPLOCKTABLE); // exec sql
        W.commit(); // commit
        LOG(INFO) << "Lock Table Deleted Successfully";
    }

    void PsqlDb::dropInfoTable() {
        pqxx::work W(*connHandler.get()); // transactional object
        W.exec(DROPINFOTABLE); // exec sql
        W.commit(); // commit
        LOG(INFO) << "Info Table Deleted Successfully";
    }

    // Insert Into Lock Table with Prepared Statement
    void PsqlDb::insertLockTbl(const int lockid, const std::string &topicname, const bool write_access, const bool read_access, const bool primary_up) {
        lockguard lockguard1(this, lockid);
        pqxx::work W(*connHandler.get());
        auto W_invocation = W.prepared("insertlocktbl");
        invokeDynamic(W_invocation, lockid, topicname.c_str(), write_access, read_access, primary_up);
        pqxx::result res = W_invocation.exec();
        W.commit();
    }

    // Insert Into Info Table with Prepared Statement
    void PsqlDb::insertInfoTbl(const std::string &procname, const int numtopics, const std::string &primrec, const std::string &secrec) {
        lockguard lockguard1(this, 111); // reserved constant lock number for info table
        pqxx::work W(*connHandler.get());
        auto W_invocation = W.prepared("insertinfotbl");
        invokeDynamic(W_invocation, procname.c_str(), numtopics, primrec.c_str(), secrec.c_str());
        W_invocation.exec();
        W.commit();
    }

    // update record by lock id
    void PsqlDb::updateRecordByLockId(int lockid) {
        lockguard lockguard1(this, lockid);
        pqxx::work W(*connHandler.get());
        auto W_invocation = W.prepared("updatebylockid");
        invokeDynamic(W_invocation, lockid);
        auto result = W_invocation.exec();
        W.commit();

    }

    // update primary up or down
    void PsqlDb::updateRecordPrimary(int lockid, bool primaryup) {
        lockguard lockguard1(this, lockid);
        pqxx::work W(*connHandler.get());
        auto W_invocation = W.prepared("updateprimup");
        invokeDynamic(W_invocation, lockid, primaryup);
        auto result = W_invocation.exec();
        W.commit();
    }


    // Is Primary Up
    bool PsqlDb::isPrimaryUp(int lockid) {
        lockguard lockguard1(this, lockid);
        pqxx::nontransaction N(*connHandler.get());
        auto N_invocation = N.prepared("isprimaryup");
        invokeDynamic(N_invocation, lockid);
        pqxx::result R = N_invocation.exec();
        return R.empty() ? false :  R.begin()[0].as<bool>();
    }

    // Chk Read Access
    bool PsqlDb::readAccess(int lockid) {
        lockguard lockguard1(this, lockid);
        pqxx::nontransaction N(*connHandler.get());
        auto N_invocation = N.prepared("isprimaryup");
        invokeDynamic(N_invocation, lockid);
        pqxx::result R = N_invocation.exec();
        return R.empty() ? false :  R.begin()[0].as<bool>();
    }

    // Chk Write Access
    bool PsqlDb::writeAccess(int lockid) {
        lockguard lockguard1(this, lockid);
        pqxx::nontransaction N(*connHandler.get());
        auto N_invocation = N.prepared("readaccess");
        invokeDynamic(N_invocation, lockid);
        pqxx::result R = N_invocation.exec();
        return R.empty() ? false :  R.begin()[0].as<bool>();
    }

    // Time Difference in Microseconds
    int PsqlDb::timeElapsedRec(int lockid) {
        lockguard lockguard1(this, lockid);
        pqxx::nontransaction N(*connHandler.get());
        auto N_invocation = N.prepared("timediffmicros");
        invokeDynamic(N_invocation, lockid);
        pqxx::result R = N_invocation.exec();
        // time Elapsed in microseconds divide by 1000 to get milliseconds
        return R.empty() ? 0 :  (R.begin()[0].as<int>() / 1000);
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


    /**
     * Prepare Staments
     * */

    // Prep Insert Into Lock Table
    void PsqlDb::prepInsertTopicTbl(){
        connHandler->prepare("insertlocktbl", PREPINSERTRECORDER); // statement template
    }

    // Prep Insert Info Table
    void PsqlDb::prepInsertInfoTbl() {
        connHandler->prepare("insertinfotbl", PREPINSERTINFO); // statement template
    }

    // db advisory lock
    void PsqlDb::prepLock() {
        connHandler->prepare("lock", PREPLOCK);
    }

    // db advisory unlock
    void PsqlDb::prepUnlock() {
        connHandler->prepare("unlock", PREPUNLOCK);
    }

    // prep update by lock id (update time)
    void PsqlDb::prepUpdateRecByLockId() {
        connHandler->prepare("updatebylockid", PREPUPDATEDBYLOCKID);
    }

    // prep update (primary up / down) by lock id
    void PsqlDb::prepUpdatePrimUp() {
        connHandler->prepare("updateprimup", PREPUPDATEPRIMARYUP);
    }

    void PsqlDb::prepTimeDiff() {
        connHandler->prepare("timediffmicros", PREPTIMEDIFFMICROS);
    }

    // prep is primary up by lock id
    void PsqlDb::prepIsPrimaryUp() {
        connHandler->prepare("isprimaryup", PREPISPRIMARYUP);
    }

    // prep is read access
    void PsqlDb::prepReadAccess() {
        connHandler->prepare("readaccess", PREPREADACCESS);
    }

    // prep is write access
    void PsqlDb::prepWriteAccess() {
        connHandler->prepare("writeaccess", PREPWRITEACCESS);
    }

    // prepare statements
    void PsqlDb::prepareAllStatements() {
        prepInsertInfoTbl();
        prepInsertTopicTbl();
        prepUpdateRecByLockId();
        prepUpdatePrimUp();
        prepIsPrimaryUp();
        prepReadAccess();
        prepWriteAccess();
        prepTimeDiff();
        prepLock();
        prepUnlock();
    }

}