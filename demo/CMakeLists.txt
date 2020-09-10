cmake_minimum_required(VERSION 3.17)
project(demo)

set(CMAKE_CXX_STANDARD 14)

set(MYSQL_INC /usr/include/mysql)
set(MYSQL_LIB /usr/lib/mysql)
include_directories(${MYSQL_INC})
link_directories(${MYSQL_LIB})
link_libraries(mysqlclient)

include_directories(include)
link_directories(lib)
link_libraries(lesdao)

add_executable(demo main.cpp)

target_link_libraries(demo mysqlclient)
target_link_libraries(demo lesdao)