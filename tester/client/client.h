#ifndef CLIENT_H
#define CLIENT_H
#include "includes.h"
#include "socket/socket.h"

// Класс клиента
class Client: public Socket {
public:
	// Конструктор и деструктор
	Client(int port_input = 4234, const char *ip_input = "127.0.0.1");
	~Client();
	
	// Функция подключения к серверу
	bool connectnet();
	// Функция смены порта и ip-адреса IPv4
	bool changeinfo(int , const char*);
	// Функция отключения от сервера
	void disconnect();
private:
	// Статус подключения
	bool connected;
};

#endif
