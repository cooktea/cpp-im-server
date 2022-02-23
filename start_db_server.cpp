#include "db_server.h"


int main() {
    db_server::ServerImpl db_server;
    db_server.Run(18900);
    return 0;
}
