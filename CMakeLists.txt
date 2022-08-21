cmake_minimum_required(VERSION 3.18) 

project(pop3server)

include_directories(./)

add_executable(pop3serverd includes.h pop3server/main.cpp server/server.cpp socket/socket.cpp check_functions.cpp)

if(WIN32)
     target_link_libraries(pop3serverd wsock32 ws2_32)
endif()
