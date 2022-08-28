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
    this->operator<<(std::string(Commands::USER) + " " + username + endLine);
}

void Pop3Client::callPassCommand(const std::string &password)
{
    this->operator<<(std::string(Commands::PASS) + " " + password + endLine);
}

void Pop3Client::callQuitCommand()
{
    this->operator<<(std::string(Commands::QUIT) + endLine);
}

void Pop3Client::callStatCommand()
{
    this->operator<<(std::string(Commands::STAT) + endLine);
}

void Pop3Client::callListCommand()
{
    this->operator<<(std::string(Commands::LIST) + endLine);
}

void Pop3Client::callRetrCommand(const int &messageId)
{
    this->operator<<(std::string(Commands::RETR) + " " + std::to_string(messageId) + endLine);
}

std::string Pop3Client::getResponse()
{
    std::string response = "";
    if (!recv_f(response, '\n')) {
        throw std::runtime_error("Error while recieving a message!");
    }
    return response;
}

Pop3Client::~Pop3Client()
{
    disconnect();
}
