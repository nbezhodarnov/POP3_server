#include "socket/socket.h"

// Конструктор
Socket::Socket(int port_input, const char *ip_input) {
	// Задание переменной для инициализации библиотеки Windows
	#ifdef _WIN32
	library_initialized = false;
	#endif
	
	// Задание семейства, порта и ip-адреса IPv4 для работы с сокетом
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port_input);
	addr.sin_addr.s_addr = inet_addr(ip_input);
	
	// Задание переменных по умолчанию
	stop_symbol = '\0'; // символ остановки
	is_status_ok = true; // статус
	net_id = &sock; // указатель на id для работы с сокетами
	memset(main_buffer, 0, Socket::MAX_BUFFER_SIZE); // буфер
	index = 0; // индекс символа в буфере
	bytes_recv = 0; // количество полученных байтов
	is_from_operator = false; // запуск чтения-отправки через оператор
	
	// Инициализация сокета
	sock = INVALID_SOCKET;
	if (socket_initialize() == false) {
		return;
	}
}

// Инициализация библиотеки сокетов для Windows
#ifdef _WIN32
bool Socket::library_initialize() {
	// Инициализация библиотеки
	if ((library_initialized != true) && (WSAStartup(MAKEWORD(2, 2), &wData) != 0)) {
		// Вывод ошибки инициализации библиотеки
		std::cout << "Error initializing socket library!\n";
		
		// Закрытие библиотеки и сохранение статуса об ошибке
		WSACleanup();
		library_initialized = false;
		is_status_ok = false;
	} else {
		// Сохранение статуса об успешной инициализации
		library_initialized = true;
	}
	return library_initialized;
}
#endif

// Инициализация сокета
bool Socket::socket_initialize() {
	// Проверка инициализации библиотеки для Windows
	#ifdef _WIN32
	if (library_initialize() == false) {
		return false;
	}
	#endif
	
	// Инициализация сокета
	if ((sock == INVALID_SOCKET) && ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)) {
		// Вывод ошибки об инициализации сокета и сохранение статуса об ошибке
		std::cout << "Error initializing socket!\n";
		is_status_ok = false;
		return false;
	}
	return true;
}

// Разрывание соединения
void Socket::disconnect_f() {
	shutdown(*net_id, 2);
	
	// Закрытие сокета
	#ifdef _WIN32
	closesocket(*net_id);
	#else
	close(*net_id);
	#endif
	
	// Инициализация сокета
	socket_initialize();
}

// Задание символа остановки
void Socket::set_stop_symbol(char stop) {
	stop_symbol = stop;
}

// Отправка данных
bool Socket::send_f(const char *message, char stop) {
	// Проверка статуса о соединении
	if ((is_status_ok == false)) {
		return false;
	}
	
	// Задание символа остановки
	if ((stop == '\0') && (stop_symbol != '\0')) {
		stop = stop_symbol;
	}
	
	// Вычисление размера данных в байтах и сохранение начала сообщения
	int size = strchr(message, stop) - message + 1, length = size;
	const char *ptr = message;
	
	// Цикл отправки
	while (length > 0) {
		// Отправка сообщения
		int i = send(*net_id, ptr, length, 0);
		
		//Обработка ошибки
		if (i == SOCKET_ERROR) {
			std::cout << "Error while sending message!\n";
			return false;
		}
		
		// Сдвиг указателя на начало и уменьшение длины отправляемого сообщения
		ptr += i;
		length -= i;
	}
	return true;
}

// Получение данных
bool Socket::recv_f(char *out, int n, char stop) {
	// Проверка статуса о соединении
	if ((is_status_ok == false)) {
		return false;
	}
	
	// Задание символа остановки
	if ((stop == '\0') && (stop_symbol != '\0')) {
		stop = stop_symbol;
	}
	
	// Индекс для работы с массивом вывода
	int i = 0;
	
	// Цикл получения
	while (i < n - 1) {
		// Проверка наличия полученных байтов
		if (bytes_recv <= 0) {
			//Очистка буфера
			memset(main_buffer, 0, Socket::MAX_BUFFER_SIZE);
			index = 0;
			
			// Получение данных в буфер
			if ((bytes_recv = recv(*net_id, main_buffer, Socket::MAX_BUFFER_SIZE, 0)) == SOCKET_ERROR) {
				// Обработка ошибки
				std::cout << "Error while recieving message!\n";
				return false;
			}
		}
		
		// Заполнение выходного массива
		for (; (bytes_recv > 0) && (i < n - 1); bytes_recv--, i++, index++) {
			out[i] = main_buffer[index];
			
			// Проверка остановки
			if (out[i] == stop) {
				// Подготовка данных для передачи через оператор
				if ((stop != '\0') && (is_from_operator)) {
					out[i] = '\0';
				} else {
					out[i + 1] = '\0';
				}
				
				bytes_recv--;
				index++;
				return true;
			}
		}
	}
	// Корректное заполнение строки
	out[n - 1] = '\0';
	
	return true;
}

// Обёртка отправки данных из строки std::string
bool Socket::send_f(const std::string &message, char stop) {
	// Добавление остановочного символа
	std::string str = message;
	if (stop != '\0') {
		str += stop;
	} else if (stop_symbol != '\0') {
		str += stop_symbol;
	}
	
	return send_f(str.c_str(), stop);
}

// Обёртка получение данных в строку std::string
bool Socket::recv_f(std::string &message, char stop) {
	// Очистка строки
	message.clear();
	
	// Формирование массива
	char *buf = new char[Socket::MAX_BUFFER_SIZE];
	memset(buf, 0, Socket::MAX_BUFFER_SIZE);
	
	// Получение данных
	if (recv_f(buf, Socket::MAX_BUFFER_SIZE, stop) == false) {
		return false;
	}
	if ((is_from_operator == false) && (stop == '\0') && (stop_symbol != '\0')) {
		stop = stop_symbol;
	}
	message += buf;
	char *ptr = strchr(buf, stop);
	while((ptr == 0) || (ptr == (buf + Socket::MAX_BUFFER_SIZE - 1))) {
		// Случай, если сообщение занимает более 1023 символов
		if (recv_f(buf, Socket::MAX_BUFFER_SIZE, stop) == false) {
			return false;
		}
		message += buf;
		ptr = strchr(buf, stop);
	}
	
	delete []buf;
	return true;
}

// Перегрузка оператора << для отправки данных из строки std::string
Socket& Socket::operator<<(const std::string &message) {
	send_f(message);
	return *this;
}

// Перегрузка оператора << для отправки данных из целого числа
Socket& Socket::operator<<(const long long int &message) {
	char str[30];
	sprintf(str, "%lld", message);
	if (stop_symbol != '\0') {
		int n = strlen(str);
		str[n] = stop_symbol;
		str[n + 1] = '\0';
	}
	send_f(str);
	return *this;
}

// Перегрузка оператора << для отправки данных из числа с плавающей точкой
Socket& Socket::operator<<(const long double &message) {
	char str[100];
	sprintf(str, "%Lf", message);
	if (stop_symbol != '\0') {
		int n = strlen(str);
		str[n] = stop_symbol;
		str[n + 1] = '\0';
	}
	send_f(str);
	return *this;
}

// Перегрузка оператора >> для получения данных в строку std::string
Socket& Socket::operator>>(std::string &message) {
	is_from_operator = true;
	message.clear();
	recv_f(message);
	is_from_operator = false;
	return *this;
}

// Перегрузка оператора >> для получения данных в целое число
Socket& Socket::operator>>(long long int &message) {
	is_from_operator = true;
	std::string str;
	
	// Проверка успешности получения
	if (recv_f(str)) {
		message = stoll(str);
	}
	
	is_from_operator = false;
	return *this;
}

// Перегрузка оператора >> для получения данных в число с плавающей точкой
Socket& Socket::operator>>(long double &message) {
	is_from_operator = true;
	std::string str;
	
	// Проверка успешности получения
	if (recv_f(str)) {
		message = stold(str);
	}
	
	is_from_operator = false;
	return *this;
}

// Деструктор
Socket::~Socket() {
	// Закрытие сокета
	#ifdef _WIN32
	closesocket(sock);
	WSACleanup();
	#else
	close(sock);
	#endif
}
