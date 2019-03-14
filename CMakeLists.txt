cmake_minimum_required(VERSION 3.8)
project(DistributedLockPostGre)



set(CMAKE_CXX_STANDARD 14)

add_executable(dlocktest main.cpp)


target_link_libraries(
        dlocktest
        glog
        gflags
)