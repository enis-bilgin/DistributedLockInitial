#include <iostream>

// glog
#include <glog/logging.h>
#include <gflags/gflags.h>




int main (int argc, char* argv[]) {

    // google logging
    gflags::ParseCommandLineFlags(&argc,&argv,true);
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = true;



    // do some postgre cool stuff here!


    LOG(INFO) << __PRETTY_FUNCTION__;
    return 0;
}