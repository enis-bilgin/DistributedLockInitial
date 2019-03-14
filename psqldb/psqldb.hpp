#ifndef DBPSQL_HPP
#define DBPSQL_HPP

#include <utility>


#include <psqlstatements.hpp>
#include <string>
#include <glog/logging.h>
#include <pqxx/pqxx>


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

    const std::string space = " ";

    /**
     * machine Configs To check in DB
     * */
    struct machineConfig {
        std::string procname;
        int lockId;
        int numTopics;
        std::string topics; // comma separated topics
        explicit machineConfig(std::string procname, const int lockId, const int numtopics, const std::string& topics) :
        procname(std::move(procname)), lockId(lockId), numTopics(numtopics), topics(std::move(topics)) {}
    };


    /**
     * Db Credentials
     * */
    struct dbCredentials {

        std::string host;
        std::string dbname;
        std::string port;
        std::string user;
        std::string password;

        // DB Connector String
        std::string connectString;

        explicit dbCredentials(const std::string& host, const std::string& dbname, const std::string& port, const std::string& user, const std::string& password) :
        host(host), dbname(dbname), port(port), user(user), password(password) {

            // build connection string
            connectString =
                    "host="    + host     + space +
                    "dbname="  + dbname   + space +
                    "port="    + port     + space +
                    "user="    + user     + space +
                    "password="+ password;
        }

        const std::string getConnString(){
            return connectString;
        }
    };


    class PsqlDb {
    private:
        std::shared_ptr<pqxx::connection> connHandler;

        // basic table create / drop
        void createTableIfNotExists();
        void deleteTable();

        // prepare table statements
        void prepareAllStatements();
        void prepInsertInfoTbl();
        void prepInsertTopicTbl();
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
        // initialize
        void init(machineConfig& machineconfig, dbCredentials& dbcredentials);
    };








}




#endif