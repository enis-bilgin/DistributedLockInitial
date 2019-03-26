cmake_minimum_required(VERSION 3.8)
project(DistributedLockPostGre)


# directories
include_directories(
        .
        ..
        src
        psqldb

)

# standard
set(CMAKE_CXX_STANDARD 14)


set(
        PSQLDBFILES
        src/main.cpp
        psqldb/psqldb.cpp
)


# exec
add_executable(dlocktest ${PSQLDBFILES})

# link
target_link_libraries(
        dlocktest
        glog
        gflags
        pqxx
        pq
        pthread
)

