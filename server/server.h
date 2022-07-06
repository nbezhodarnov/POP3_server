#ifndef SERVER_H
#define SERVER_H
#include "includes.h"
#include "socket/socket.h"

// Класс сервера
class Server: public Socket {
public:
	// Конструктор и деструктор
	Server(int port_input = 4234, const char *ip_input = "127.0.0.1", int max_conn = 1);
	~Server();
	
	// Функция запуска сервера
	bool start();
	// Функция смены порта и ip-адреса IPv4
	bool changeinfo(int , const char*);
	// Функция остановки сервера
	void stop();
private:
	// Статус запуска сервера
	bool started;
	// Статус функции bind
	bool bind_started;
	// Статус функции accept
	bool accepted;
	// Количество максимальных подключений
	int max_connections;
	// Идентификатор клиента
	int client;
};

#endif
