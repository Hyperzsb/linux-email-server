cmake_minimum_required(VERSION 3.17)
project(libleadao)

set(CMAKE_CXX_STANDARD 14)

add_library(leadao leadao.h leadao.cpp type.h type.cpp)