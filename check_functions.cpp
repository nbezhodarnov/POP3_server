#ifndef CHECK_FUNCTIONS_H
#define CHECK_FUNCTIONS_H

#include "includes.h"

// Функция вырезания подстроки из строки
char* cat(char* str, int start, int end) {
	char *substring = new char[end - start + 1];
	int i = start;
	for (; (i < end) && (str[i] != '\0'); i++) {
		substring[i - start] = str[i];
	}
	substring[i - start] = '\0';
	return substring;
}

// Функция проверки строки на целое число
bool isdigit(const char *num) {
	if (num[0] == '\0') {
		return false;
	}
	int i = 0;
	if (num[i] == '-') {
		i++;
	}
	while (num[i] != '\0') {
		if (('0' > num[i]) || (num[i] > '9')) {
			return false;
		}
		i++;
	}
	return true;
}

// Функция проверки строки на ip-адрес IPv4
bool ipcheck(char *ip) {
	int i = 0, previous = 0;
	for (int j = 0; j < 4; j++) {
		while (ip[i] != '.' && ip[i] != '\0') {
			i++;
		}
		if ((ip[i] == '\0') && (j != 3)) {
			return false;
		}
		char *numstr = cat(ip, previous, i);
		if (isdigit(numstr)) {
			int number = atoi(numstr);
			if ((number < 0) || (number > 255)) {
				return false;
			}
		}
		delete []numstr;
		i++;
		previous = i;
	}
	i--;
	if (ip[i] != '\0') {
		return false;
	}
	return true;
}

#endif
