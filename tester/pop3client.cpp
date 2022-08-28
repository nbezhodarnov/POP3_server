#include "pop3client.h"

#include "commands.h"
#include "responses.h"

const char *endLine = "\r\n";

Pop3Client::Pop3Client(int port_input, const char *ip_input) : Client(port_input, ip_input)
{
    while(!connectnet());
    if (getResponse() != Responses::OK) {
        throw std::runtime_error("Error while connecting to server!");
    }
}

void Pop3Client::callUserCommand(const std::string &username)
{
    send_f(std::string(Commands::USER) + " " + username + endLine, '\n');
}

void Pop3Client::callPassCommand(const std::string &password)
{
    send_f(std::string(Commands::PASS) + " " + password + endLine, '\n');
}

void Pop3Client::callQuitCommand()
{
    send_f(std::string(Commands::QUIT) + endLine, '\n');
}

void Pop3Client::callStatCommand()
{
    send_f(std::string(Commands::STAT) + endLine, '\n');
}

void Pop3Client::callListCommand()
{
    send_f(std::string(Commands::LIST) + endLine, '\n');
}

void Pop3Client::callRetrCommand(const int &messageId)
{
    send_f(std::string(Commands::RETR) + " " + std::to_string(messageId) + endLine, '\n');
}

std::string Pop3Client::getResponse()
{
    std::string response = "";
    if (!recv_f(response)) {
        throw std::runtime_error("Error while recieving a message!");
    }
    return response;
}

Pop3Client::~Pop3Client()
{
    disconnect();
}
