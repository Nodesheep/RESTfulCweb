cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 11)

project(CWEB)

LINK_DIRECTORIES("/usr/local/Cellar/mysql/8.0.30/lib")

INCLUDE_DIRECTORIES("/usr/local/Cellar/mysql/8.0.30/include")

add_executable(cweb main.cpp)
