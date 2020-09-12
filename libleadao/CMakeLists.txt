cmake_minimum_required(VERSION 3.17)
project(libleadao)

set(CMAKE_CXX_STANDARD 14)

add_library(leadao leadao.cpp leadao.h type.h)