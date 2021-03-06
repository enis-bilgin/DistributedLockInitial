#include <iostream>

// glog
#include <glog/logging.h>
#include <gflags/gflags.h>

#include <psqldb.hpp>

#include <chrono>
#include <thread>
#include <atomic>
#include <future>

#include <atomic>


/******************************************
 *  TEST APPLICATION FOR DISTRIBUTED LOCK *
 ******************************************/
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
            2,
            "notice/ebilgin,aaa,bbb,ccc,dddd,eee,ffff,ggg,hhh,kkk,llll,mmmm,nnnn,dddd,ccccc,fffff",
            "sec/ebilgin,sec/aaa,sec/bbb,sec/ccc,sec/dddd,sec/eee,sec/ffff,sec/ggg,sec/hhh,sec/kkk,sec/llll,sec/mmmm,sec/nnnn,sec/dddd,sec/ccccc,sec/fffff"
    );

    psqldb::PsqlDb databaseDev;

    // Some sample topics
    // lockid, topicname, WRT, READ, IsPrimary
    auto topic1 = psqldb::recordertest{111,"recordername1", true, false, true};
    auto topic2 = psqldb::recordertest{222,"recordername2", true, true, false};

    std::vector<psqldb::recordertest> topics;
    topics.push_back(topic1);
    topics.push_back(topic2);



    // initialize and connect database
    if(databaseDev.init(machineConfig, devCredentials, topics)){
        LOG(INFO) << "Database Initialize SUCCESS";
        /*TEST SOME FILE STUFF
         * FAULT TOLERANCE (primary)
         * LOAD BALANCING (secondary)
         */

/**Act as primary*/
//        for(;;){
//            psqldb::PsqlDb::lockguard locking(&databaseDev, 111);
//            databaseDev.updateRecordPrimary(111, true);
//            databaseDev.updateRecordByLockId(111);
//            LOG(INFO) << "PRIMARY WRITING ";
//             // locking as primary
//        }


/**Act as secondary*/
//        for(;;) {
//            psqldb::PsqlDb::lockguard locking(&databaseDev, 111);
//            if(!databaseDev.isPrimaryUp(111) || databaseDev.timeElapsedRec(111) > 300) {
//            databaseDev.updateRecordPrimary(111, false);
//            databaseDev.updateRecordByLockId(111);
//                LOG(INFO) << "SECONDARY WRITING ";
//            }
//        }



    } else{
        LOG(INFO) << "Database Initialize FAIL";
        return 0;
    }


    return 0;
}