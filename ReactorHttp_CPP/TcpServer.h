#pragma once
#include"EventLoop.h"
#include"ThreadPool.h"


class TcpServer {
public:
	TcpServer(unsigned short port, int threadNum);
	~TcpServer();
	void setListen();//初始化监听文件描述符
	void run();	//启动TCP服务器	

	static int acceptConnection(void* arg);
private:
	int m_threadNum;
	EventLoop* m_mainLoop;
	ThreadPool* m_threadPool;
	int m_lfd;
	unsigned short m_port;
};

