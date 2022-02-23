#include "logic_server.h"

int main() {
    logic_server::ServerImpl logic_server;
    logic_server.Run(18800);
    return 0;
}
