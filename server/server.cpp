#include "server/server.h"

// Конструктор
Server::Server(int port_input, const char *ip_input, int max_conn): Socket(port_input, ip_input) {
	// Инициализация максимального числа соединений
	max_connections = max_conn;
	
	// Задание статусов по умолчанию
	started = false; // старта сервера
	bind_started = false; // функции bind
	accepted = false; // функции accept
	
	// Запуск сервера
	start();
}

// Деструктор
Server::~Server() {
}

// Запуск сервера
bool Server::start() {
	// Проверка статуса сокета
	if ((is_status_ok == false) && (socket_initialize() == false)) {
		return false;
	}
	
	// Запуск функции bind
	if ((bind_started == false) && (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)) {
		// Обработка ошибки
		std::cout << "Error while binding!\n";
		is_status_ok = false;
		return false;
	}
	bind_started = true;
	
	// Запуск прослушивания
	if ((started == false) && (listen(sock, max_connections) == SOCKET_ERROR)) {
		// Обработка ошибки
		std::cout << "Error while listening!\n";
		is_status_ok = false;
		return false;
	}
	started = true;
	
	// Запуск функции accept
	if ((accepted == false) && ((client = accept(sock, 0, 0)) == INVALID_SOCKET)) {
		// Обработка ошибки
		std::cout << "Error while accepting connection!\n";
		is_status_ok = false;
		return false;
	}
	
	// Сохранение идентификатора клиента для передачи данных и статуса об успешности подключения
	net_id = &client;
	is_status_ok = true;
	accepted = true;
	
	return true;
}

// Изменение порта и ip-адреса IPv4
bool Server::changeinfo(int port_input, const char *ip_input) {
	// Остановка сервера
	stop();
	
	// Изменение данных
	addr.sin_port = htons(port_input);
	addr.sin_addr.s_addr = inet_addr(ip_input);
	
	// Запуск сервера
	return start();
}

// Остановка сервера
void Server::stop() {
	// Остановка
	disconnect_f();
	
	// Сохранение статуса об остановке
	started = false;
	accepted = false;
	is_status_ok = false;
}
