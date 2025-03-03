#include <iostream>
#include <stdio.h>
#include <unistd.h>

#include "server.h"


using namespace std;

int main()
{

    Server* server = new Server();

    server->startServer();

    return 0;
}
