#ifndef DBPSQL_HPP
#define DBPSQL_HPP

#include <utility>
#include <datatypes.h>


#include <psqlstatements.hpp>
#include <string>
#include <glog/logging.h>
#include <pqxx/pqxx>
#include <unordered_map>

#include <thread>
#include <chrono>

/**
 * PostgreSQL Database Implementation
 * Steps
 *   transactional object acquiring from connection handler
 *   prepare statements
 *   invoke necessary parameters
 *   execute command
 *   commit into DB()
 *
 * */
namespace psqldb {

    /**
     * Invoke Recursively for multiple C++ params
     *
     * */
    // single data invocation
    template <typename T>
    pqxx::prepare::invocation& invokeDynamic(pqxx::prepare::invocation& inv, T data){
        return inv(data);
    }

    // recursive invocation
    template <typename T, typename ... All>
    pqxx::prepare::invocation& invokeDynamic(pqxx::prepare::invocation& inv, T data, All... all) {
        return invokeDynamic(invokeDynamic(inv,data),all...);
    }

    class PsqlDb {
    private:
        std::shared_ptr<pqxx::connection> connHandler;

        // basic table create / drop
        void createTableIfNotExists();
        void dropLockTable();
        void dropInfoTable();

        void updateRecordByLockId(int lockid); // update record time / to hold lock
        void updateRecordPrimary(int lockid, bool primaryup); // primary comes back up

        // prepare table statements
        void prepareAllStatements();
        void prepInsertInfoTbl();
        void prepInsertTopicTbl();
        void prepUpdateRecByLockId();
        void prepUpdatePrimUp();
        void prepLock();
        void prepUnlock();

        // prep functions
        void insertLockTbl(const int lockid, const std::string &topicname, const bool write_access, const bool read_access, const bool primary_up);
        void insertInfoTbl(const std::string &procname, const int lockid, const int numtopics, const std::string& topics);

        // lock functions
        void lock(const int lockid) const ;
        void unlock(const int lockid) const ;

        // RAII lock wrapper
        class lockguard {
        public:
            explicit lockguard(const psqldb::PsqlDb* ptrdb, const int lockid) :
                    ptrDb_(ptrdb), lockid_(lockid) {
                // lock on construction
                ptrdb->lock(lockid_);
            }

            ~lockguard() {
                ptrDb_->unlock(lockid_);
            }

        private:
            lockguard(const lockguard&) = delete; // delete this
            const psqldb::PsqlDb* ptrDb_;
            const int lockid_;
        };


    public:
        ~PsqlDb()  {
             connHandler->disconnect();
             LOG(INFO) << "DB Disconnected!";
        }

        // initialize
        bool init(machineConfig& machineconfig, dbCredentials& dbcredentials, std::vector<topic>& topics);



//        void lockPrimaryTopic(int lockid) {
//            /**
//             * it locks this record without expiration for primary
//             * sets primary up to true
//             * updates time every 100 milliseconds
//             * */
//            uint32_t counter=0;
//            lockguard locker_lockid(this, lockid);
//            LOG(INFO) << "Lock Acquired for PRIMARY TOPIC! " << lockid;
//            for(;;) {
//                if(++counter % 10000 == 0) {
//                    // updateRecord();
//                    // readRecord if write access false stop
//                    break;
//                } // if
//            }
//            LOG(INFO) << "Lock Released for PRIMARY TOPIC! " << lockid;
//        }
//
//        void lockSecondaryTopic(int lockid) {
//            uint32_t counter=0;
//            // read record time
//            lockguard locker_lockid(this, lockid);
//            LOG(INFO) << "Lock Acquired for SECONDARY TOPIC!" << lockid;
//            for(;;) {
//                if(++counter % 10000 == 0) {
//                    // updateRecord(); Timer write / read access no cares
//                    // Update Record
//                    // Sleep for 200 milliseconds
//                    break;
//                } // if
//            } // break ends here
//        }



    };

}


#endif