﻿#include <cstdio>
#include<unistd.h>
#include<stdlib.h>
#include"TcpServer.h"
#include"Log.h"

int main(int argc, char* argv[])
{
#if 0
    if (argc < 3)
    {
        printf("./a.out port path\n");
        return -1;
    }
    unsigned short port = atoi(argv[1]);
    // 切换服务器的工作路径
    chdir(argv[2]);
#else
    chdir("/home/sj/sources");
    unsigned short port = 10000;
#endif
    //启动服务器
    TcpServer* server = new TcpServer(port, 4);
    server->run();
    Debug("main中启动了服务器...\n");
    return 0;
}