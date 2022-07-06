#ifndef SOCKET_H
#define SOCKET_H
#include "includes.h"

// Определение констант ошибок работы с сокетами для Linux
#if !defined(_WIN32) && !defined(__APPLE__)
static const int INVALID_SOCKET = -1;
static const int SOCKET_ERROR = -1;
#endif

// Определение класса обёртки сокета
class Socket {
public:
	// Конструктор и деструктор
	Socket(int port_input = 4234, const char *ip_input = "127.0.0.1");
	~Socket();
	
	// Функция задания остановочного символа
	void set_stop_symbol(char);
	
	// Функции отправки и получения строк
	bool send_f(const char*, char stop = '\0');
	bool recv_f(char*, int, char stop = '\0');
	bool send_f(const std::string&, char stop = '\0');
	bool recv_f(std::string&, char stop = '\0');
	
	// Перегрузка операторов << и >>
	Socket& operator<<(const std::string&);
	Socket& operator<<(const long long int&);
	Socket& operator<<(const long double&);
	Socket& operator>>(std::string&);
	Socket& operator>>(long long int&);
	Socket& operator>>(long double&);
	
protected:
	// Функция разрыва соединения
	void disconnect_f();
	
	// Функция инициализации сокета
	bool socket_initialize();
	
	// Сокет
	int sock;
	// Указательна id для отправки
	int *net_id;
	// Структура с информацией об ip-адресе и порте
	struct sockaddr_in addr;
	// Символ остановки
	char stop_symbol;
	// Статус соединения
	bool is_status_ok;
	
private:
	// Функция иниациализации библиотеки для Windows
	#ifdef _WIN32
	bool library_initialize();
	#endif
	
	// Объекты для инициализации библиотеки
	#ifdef _WIN32
	WSAData wData;
	bool library_initialized;
	#endif
	
	// Индекс символа в буфере, на котором произошла остановка
	int index;
	// Количество полученных, но не прочитанных байтов
	int bytes_recv;
	// Статус запуска функций чтения-отправки посредством операторов
	bool is_from_operator;
	
	// Максимальный размер буфера
	static const int MAX_BUFFER_SIZE = 1024;
	// Буфер
	char main_buffer[Socket::MAX_BUFFER_SIZE];
};

#endif
