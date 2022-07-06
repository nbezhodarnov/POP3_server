#include <fstream> // Библиотека для работы с файлами
#include <errno.h> // Библиотека для обработки системных ошибок
#include "server/server.h"

// Список возможных статусов
namespace Status {
	static std::string ERR = "-ERR\r\n";
	static std::string ERR_AUTH = "-ERR you haven't authorized yet\r\n";
	static std::string ERR_COMM = "-ERR unknown command\r\n";
	static std::string ERR_MESG = "-ERR unknown message\r\n";
	static std::string ERR_PASS = "-ERR wrong password\r\n";
	static std::string ERR_USER = "-ERR unknown user\r\n";
	static std::string OK = "+OK\r\n";
	static std::string OK_STAT = "+OK ";
}

int main() {
	// Запуск сервера
	Server pop(2110, "0.0.0.0");
	
	// Объявление переменных для работы
	std::string message;
	std::string command;
	std::string nick;
	std::string password;
	std::string temp;
	std::string answer;
	std::ifstream users_list;
	std::ifstream user_messages;
	bool authorized = false;
	
	// Отправка статуса ОК клиенту
	pop << Status::OK;
	
	// Цикл сервера
	while(true) {
		
		// Получение сообщения от клиента
		pop.set_stop_symbol('\n');
		pop >> message;
		pop.set_stop_symbol('\0');
		
		// Обработка случая неожиданного отключения клиента
		if (errno == ECONNRESET) {
			std::cout << "Client unexpetedly disconnected\n";
			
			// Перезапуск сервера
			pop.stop();
			while (!pop.start());
			
			// Отправка статуса ОК клиенту
			pop << Status::OK;
			
			// Возврат переменных к исходному состоянию
			nick.clear();
			password.clear();
			authorized = false;
			
			// Возврат к началу цикла
			continue;
		}
		
		// Проверка длины сообщения
		if (message.length() < 5) {
			// Отправка ошибки о некорректной команде
			pop << Status::ERR_COMM;
			continue;
		}
		
		// Вычленение команды из сообщения
		message.resize(message.length() - 1);
		command = message.substr(0, 4);
		
		// Команда USER
		if (command == "USER") {
			// Обработка случая пустого логина
			if (message.length() < 6) {
				std::cout << "Client is trying to authorize by \n";
				pop << Status::ERR_USER;
				std::cout << "Error! No such user!\n";
				continue;
			}
			
			std::cout << "Client is trying to authorize by " << message.substr(5) << '\n';
			
			// Отмена авторизации
			authorized = false;
			nick.clear();
			password.clear();
			
			// Открытие файла списка пользователей
			users_list.open("passwd");
			
			// Поиск логина в списке
			bool user_exist = false;
			for (std::getline(users_list, temp); !users_list.eof(); std::getline(users_list, temp)) {
				if (message.substr(5) == temp.substr(0, temp.find(':'))) {
					user_exist = true;
					// Занесение логина и пароля
					nick = temp.substr(0, temp.find(':'));
					password = temp.substr(temp.find(':') + 1, temp.length() - temp.find(':') - 1);
					break;
				}
			}
			users_list.close();
			
			// Проверка результатов поиска
			if (user_exist) {
				// Пользователь найден
				pop << Status::OK;
				std::cout << "User exist\n";
			} else {
				// Пользователь не найден
				pop << Status::OK;
				std::cout << "Error! No such user!\n";
			}
			
			// Возврат к началу цикла
			continue;
			
		// Команда PASS
		} else if (command == "PASS") {
			std::cout << "Client sent password\n";
			
			// Обработка случая пустого пароля
			if (message.length() < 6) {
				pop << Status::ERR_PASS;
				std::cout << "Error! Wrong password!\n";
				continue;
			}
			
			// Проверка логина (задан ли) и пароля
			if ((nick.empty()) || (message.substr(5) != password)) {
				pop << Status::ERR_PASS;
				std::cout << "Error! Wrong password!\n";
			} else {
				authorized = true;
				pop << Status::OK;
				std::cout << "Authorization completed\n";
			}
			
			// Возврат к началу цикла
			continue;
			
		// Команда QUIT
		} else if (command == "QUIT") {
			std::cout << "Client disconnected\n";
			
			// Отправка статуса ОК клиенту
			pop << Status::OK;
			
			// Перезапуск сервера
			pop.stop();
			while (!pop.start());
			
			// Отправка статуса ОК клиенту
			pop << Status::OK;
			
			// Возврат переменных к исходному состоянию
			nick.clear();
			password.clear();
			authorized = false;
			
			// Возврат к началу цикла
			continue;
		}
		
		// Проверка авторизации
		if (authorized) {
			// Команда STAT
			if (command == "STAT") {
				std::cout << "Client is getting stats\n";
				
				// Открытие файла писем пользователя
				std::string file = "user_messages/";
				file += nick;
				user_messages.open(file);
				
				// Проверка открытия файла
				if (user_messages.is_open()) {
					// Вычисление количества сообщений и их общий размер
					int overall = 0, id = 0;
					for (std::getline(user_messages, temp); !user_messages.eof(); std::getline(user_messages, temp)) {
						// Вычисление общего размера
						overall += temp.length() + 1;
						
						// Проверка окончания письма
						if (temp == ".") {
							id++;
						}
					}
					user_messages.close();
					
					// Формирование ответа
					answer = Status::OK_STAT;
					answer += std::to_string(id) + ' ';
					answer += std::to_string(overall) + "\r\n";
					
					// Отправка ответа клиенту
					pop << answer;
				} else {
					// Случай отсутствия писем
					pop << Status::OK_STAT + "0 0\r\n";
				}
				
			// Команда LIST
			} else if (command == "LIST") {
				std::cout << "Client is getting list of messages\n";
				
				// Открытие файла писем пользователя
				std::string file = "user_messages/";
				file += nick;
				user_messages.open(file);
				
				// Проверка открытия файла
				if (user_messages.is_open()) {
					int current_message = 0, id = 0;
					
					// Проверка наличия параметра команды
					if ((message.length() > 5) && (isdigit(message.substr(5).c_str()))) {
						// Вычленение параметра
						int required = std::stoi(message.substr(5));
						
						// Чтение данных о требуемом письме
						bool exist = false;
						for (std::getline(user_messages, temp); !user_messages.eof(); std::getline(user_messages, temp)) {
							// Вычисление размера письма
							current_message += temp.length() + 1;
							
							// Проверка окончания письма
							if (temp == ".") {
								id++;
								
								// Проверка идентификатора письма
								if (id == required) {
									if (exist == false) {
										exist = true;
										
										// Заполнение ответа
										answer = Status::OK_STAT;
										answer += std::to_string(id) + ' ';
										answer += std::to_string(current_message) + "\r\n";
									}
									break;
								}
								
								// Обнуление размера письма
								current_message = 0;
							}
						}
						
						// Проверка существования письма
						if (exist == false) {
							answer = Status::ERR_MESG;
						}
						
					// Случай отсутствия параметра
					} else {
						answer = Status::OK_STAT.substr(0, 3) + "\r\n";
						
						// Чтение данных обо всех письмах
						for (std::getline(user_messages, temp); !user_messages.eof(); std::getline(user_messages, temp)) {
							// Вычисление размера письма
							current_message += temp.length() + 1;
							
							// Проверка окончания письма
							if (temp == ".") {
								id++;
								
								// Заполнение ответа
								answer += std::to_string(id) + ' ';
								answer += std::to_string(current_message) + "\r\n";
								
								// Обнуление размера письма
								current_message = 0;
							}
						}
						
						// Формирование окончания сообщения
						answer += ".\r\n";
					}
					
					// Закрытие файла
					user_messages.close();
					
					// Отправка сообщения
					pop << answer;
					
				// Случай отсутствия писем
				} else {
					pop << Status::OK.substr(0, 3) + "\r\n.\r\n";
				}
				
			// Команда RETR
			} else if (command == "RETR") {
				std::cout << "Client is getting a message\n";
				
				// Открытие файла писем пользователя
				std::string file = "user_messages/";
				file += nick;
				user_messages.open(file);
				
				// Проверка открытия файла
				if (user_messages.is_open()) {
					// Проверка наличия параметра
					if ((message.length() > 5) && (isdigit(message.substr(5).c_str()))) {
						// Поиск и чтение письма
						int required = std::stoi(message.substr(5));
						int id = 1;
						bool exist = false;
						for (std::getline(user_messages, temp); !user_messages.eof(); std::getline(user_messages, temp)) {
							// Проверка идентификатора письма
							if (id == required) {
								// Формирование ответа
								if (exist == false) {
									exist = true;
									answer = Status::OK_STAT.substr(0, 3) + "\r\n";
								}
								answer += temp + "\r\n";
							}
							
							// Проверка окончания письма
							if (temp == ".") {
								id++;
								
								// Выход по нахождению письма
								if (exist) {
									break;
								}
							}
						}
						
						// Закрытие файла писем пользователя
						user_messages.close();
						
						// Проверка наличия письма
						if (exist == false) {
							answer = Status::ERR_MESG;
						}
						
					// Случай отсутсвтвия параметра
					} else {
						answer = Status::ERR_MESG;
					}
					
					// Отправка сообщения клиенту
					pop << answer;
					
				// Случай отсутствия писем
				} else {
					pop << Status::ERR_MESG;
				}
				
			// Случай неопознанной команды
			} else {
				pop << Status::ERR_COMM;
			}
			
			// Очистка ответа
			answer.clear();
			
		// Случай отсутствия авторизации
		} else {
			// Случай попытки получения доступа к данным без авторизации
			if (command == "LIST" || command == "RETR" || command == "STAT") {
				pop << Status::ERR_AUTH;
				std::cout << "Client is trying to access data without authorization\n";
				
			// Случай неопознанной команды
			} else {
				pop << Status::ERR_COMM;
			}
		}
	}
}
