#include "logic_server.h"

int main()
{
    ServerImpl *logic_server = new ServerImpl();
    logic_server->Run();
    return 0;
}
