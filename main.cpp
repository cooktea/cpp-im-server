#include "logic_server.h"
#include "db_server.h"
#include "CONST.h"
#include <stdio.h>
#include <iostream>
#include <unistd.h>

void start_logic_server(int port)
{
    logic_server::ServerImpl logic_server;
    logic_server.Run(port);
}

void start_db_server(int port)
{
    db_server::ServerImpl db_server;
    db_server.Run(port);
}

int main()
{
    const int logic_server_port[] = {18800, 18801};
    const int db_server_port[] = {18900};
    for(int i=0; i<sizeof(logic_server_port)/sizeof(logic_server_port[0]); i++){
        pid_t fpid = fork();
        std::cout << getpid() << " try fork pid " << fpid << std::endl;
        if (fpid < 0) {
            std::cout << "Error while fork logic server, port: " << logic_server_port[i] << std::endl;
        }
        else if (fpid == 0) {  
            std::cout << "Success start logic server, port: " << logic_server_port[i] << " pid :" << getpid() << std::endl;
            start_logic_server(logic_server_port[i]);
        }  
        else {  
            std::cout << getpid() << "success fork pid " << fpid << std::endl;
        }  
    }
    for(int i=0; i<sizeof(db_server_port)/sizeof(db_server_port[0]); i++){
        pid_t fpid = fork();
        std::cout << getpid() << "try fork pid " << fpid << std::endl;
        if (fpid < 0) {
            std::cout << "Error while fork db server, port: " << db_server_port[i] << std::endl;
        }
        else if (fpid == 0) {  
            std::cout << "Success start db server, port: " << db_server_port[i] << " pid :" << getpid() << std::endl;
            start_db_server(db_server_port[i]);
        }  
        else {  
            std::cout << getpid() << "success fork pid " << fpid << std::endl;
        }  
    }
    while(true){
        sleep(1000);
    }
    return 0;
}
