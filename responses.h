#ifndef RESPONSES_H
#define RESPONSES_H


namespace Responses {
    static const char* ERR = "-ERR\r\n";
    static const char* ERR_AUTH = "-ERR you haven't authorized yet\r\n";
    static const char* ERR_COMM = "-ERR unknown command\r\n";
    static const char* ERR_MESG = "-ERR unknown message\r\n";
    static const char* ERR_PASS = "-ERR wrong password\r\n";
    static const char* ERR_USER = "-ERR unknown user\r\n";
    static const char* OK = "+OK\r\n";
    static const char* OK_STAT = "+OK ";
}

#endif // RESPONSES_H
