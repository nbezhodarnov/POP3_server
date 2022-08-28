#ifndef POP3CLIENT_H
#define POP3CLIENT_H


#include <string>

#include "client/client.h"

class Pop3Client : public Client
{
public:
    Pop3Client(int port_input = 2110, const char *ip_input = "127.0.0.1");
    void callUserCommand(const std::string &username);
    void callPassCommand(const std::string &password);
    void callQuitCommand();
    void callStatCommand();
    void callListCommand();
    void callRetrCommand(const int &messageId);
    std::string getResponse();
    ~Pop3Client();
};

#endif // POP3CLIENT_H
