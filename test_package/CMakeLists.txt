cmake_minimum_required(VERSION 3.15)
project(PackageTest CXX)

find_package(IODD_Model CONFIG REQUIRED)

add_executable(example example.cpp)
target_link_libraries(example IODD_Model::IODD_Model)
