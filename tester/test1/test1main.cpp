#include "pop3client.h"
#include "responses.h"

#include <cassert>

int main() {
    Pop3Client client;
    client.callUserCommand("tiger");
    assert(client.getResponse() == Responses::OK && "Error while accessing existed users!");
    client.callUserCommand("unregisteredUser");
    assert(client.getResponse() == Responses::OK && "Leaking information about unexisted users!");
    client.callQuitCommand();
    return 0;
}
