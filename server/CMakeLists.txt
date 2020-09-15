cmake_minimum_required(VERSION 3.17)
project(leaserver)

set(CMAKE_CXX_STANDARD 14)

set(MYSQL_INC /usr/include/mysql)
set(MYSQL_LIB /usr/lib/mysql)
include_directories(${MYSQL_INC})
link_directories(${MYSQL_LIB})
link_libraries(mysqlclient)

include_directories(/usr/local/include/cjson)
link_directories(/usr/local/lib)
link_libraries(cjson)

include_directories(include)
link_directories(lib)
link_libraries(leadao)

add_executable(leaserver main.cpp)

target_link_libraries(leaserver mysqlclient)

target_link_libraries(leaserver leadao)

find_package(OpenSSL REQUIRED)
target_link_libraries(leaserver OpenSSL::SSL)

target_link_libraries(leaserver cjson)

target_link_libraries(leaserver m)