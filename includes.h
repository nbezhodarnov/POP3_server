#ifndef INCLUDES_H
#define INCLUDES_H

// Библиотеки для работы со строками и вводом-выводом
#include <iostream>
#include <cstring>
#include <string>
#include <cctype>
#include <cstdlib>
#include <cstdio>

// Библиотека для работы с сокетами на Windows
#ifdef _WIN32

#include <winsock2.h>

// Библиотеки для работы с сокетами на Linux
#else

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#endif

// Объявление функций для работы со строками
char* cat(char* str, int start, int end);
bool isdigit(const char *num);
bool ipcheck(char *ip);

#endif
