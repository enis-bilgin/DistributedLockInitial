#include <iostream>

// glog
#include <glog/logging.h>
#include <gflags/gflags.h>

#include <psqldb.hpp>


// Test Topic
struct topic {
    std::string topic;
    bool write;
    bool read;
    bool primary;
};


int main (int argc, char* argv[]) {

    // google logging
    gflags::ParseCommandLineFlags(&argc,&argv,true);
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = true;

    /**
     * DB Credentials
     * --------------
     * string host
     * string dbname
     * string port
     * string user
     * string password
     * */
    psqldb::dbCredentials devCredentials(
            "ebilgin-dev.c1kxbz1rv8k8.us-east-1.rds.amazonaws.com", // endpoint
            "ebilgindevdb", // dbname
            "5432", // port
            "ebilgin", // user
            "12345678"
    );

    /**
     * Instance Config
     * --------------
     * string machinename // AWS_machine_1, .._2, ..
     * int lockid
     * int numoftopics to subscribe
     * string topic numbers // S3T1,S3T2, ...
     * */
    psqldb::machineConfig machineConfig(
            "AWSMachineOne",
            123,
            2,
            "T1"
    );


    LOG(INFO) << devCredentials.getConnString();

    psqldb::PsqlDb databaseDev;

    // initialize and connect database
    databaseDev.init(machineConfig, devCredentials);

    LOG(INFO) << __PRETTY_FUNCTION__;
    return 0;
}