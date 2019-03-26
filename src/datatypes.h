#ifndef DATATYPES_HPP
#define DATATYPES_HPP

#include <string>

namespace psqldb {
    const std::string space = " ";

    /**
     * machine Configs To check in DB
     * */
    struct machineConfig {
        std::string procname;
        int numtopics;
        std::string prim_recs; // comma separated topics
        std::string sec_recs;
        explicit machineConfig(const std::string& procname, const int numtopics, const std::string& prim_recs, const std::string& sec_recs) :
                procname(procname),  numtopics(numtopics), prim_recs(prim_recs), sec_recs(sec_recs) {}
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

        explicit dbCredentials(const std::string &host, const std::string &dbname, const std::string &port,
                               const std::string &user, const std::string &password) :
                host(host), dbname(dbname), port(port), user(user), password(password) {

            // build connection string
            connectString =
                    "host=" + host + space +
                    "dbname=" + dbname + space +
                    "port=" + port + space +
                    "user=" + user + space +
                    "password=" + password;
        }

        const std::string getConnString() {
            return connectString;
        }
    };

    // Test Topic
    struct topic {
        int lockid;
        std::string topic;
        bool write;
        bool read;
        bool primary;
    };

}

#endif