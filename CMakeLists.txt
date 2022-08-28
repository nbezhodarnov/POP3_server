cmake_minimum_required(VERSION 3.18) 

project(pop3server)

include_directories(./)

add_executable(pop3serverd includes.h commands.h responses.h pop3server/main.cpp server/server.cpp socket/socket.h socket/socket.cpp check_functions.cpp)

if(WIN32)
     target_link_libraries(pop3serverd wsock32 ws2_32)
endif()

enable_testing()

add_executable(POP3_server_tester tester/client/client.h tester/client/client.cpp tester/pop3client.h tester/pop3client.cpp tester/test1/test1main.cpp socket/socket.h socket/socket.cpp)
target_include_directories(POP3_server_tester PRIVATE "${PROJECT_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}/tester")

if(WIN32)
     target_link_libraries(POP3_server_tester wsock32 ws2_32)
endif()

add_custom_command(TARGET pop3serverd PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/data/" $<TARGET_FILE_DIR:pop3serverd>)
add_test(NAME Test1 COMMAND POP3_server_tester)
