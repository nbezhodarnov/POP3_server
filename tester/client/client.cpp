#include "client/client.h"

// Конструктор
Client::Client(int port_input, const char *ip_input): Socket(port_input, ip_input) {
	// Задание статуса по умолчанию
	connected = false;
	// Подключение к серверу
	connectnet();
}

// Деструктор
Client::~Client() {
}

// Подключение к серверу
bool Client::connectnet() {
	// Проверка статуса сокета
	if ((is_status_ok == false) && (socket_initialize() == false)) {
		return false;
	}
	
	// Подключение
	if ((connected == false) && (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)) {
		// Вывод ошибки подключения и сохранения статуса об ошибке
		std::cout << "Error while connection being processed!\n";
		is_status_ok = false;
		return false;
	}
	
	// Сохранение статуса об успешном подключении
	connected = true;
	
	return true;
}

// Изменение порта и ip-адреса IPv4
bool Client::changeinfo(int port_input, const char *ip_input) {
	// Отключение от сервера
	disconnect();
	
	// Изменение информации
	addr.sin_port = htons(port_input);
	addr.sin_addr.s_addr = inet_addr(ip_input);
	
	// Подключение
	return connectnet();
}

// Отключение от сервера
void Client::disconnect() {
	// Отключение
	disconnect_f();
	
	// Сохранение статуса об отключении
	connected = false;
	is_status_ok = false;
}
