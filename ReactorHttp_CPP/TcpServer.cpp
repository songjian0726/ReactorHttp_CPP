#include "TcpServer.h"
#include"TcpConnection.h"
#include<arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#include"Log.h"

int TcpServer::acceptConnection(void* arg){
	TcpServer* server = static_cast<TcpServer*>(arg);
	//accept提取
	int cfd = accept(server->m_lfd, NULL, NULL);
	//从线程池中选出一个子线程接收处理cfd
	EventLoop* evLoop = server->m_threadPool->takeWorkerEventLoop();
	//cfd放到TcpConnection中
	TcpConnection* conn = new TcpConnection(cfd, evLoop);
	return 0;
}


TcpServer::TcpServer(unsigned short port, int threadNum)
{
	m_port = port;
	m_mainLoop = new EventLoop(); //主线程的EventLoop不需要传参
	m_threadNum = threadNum;
	m_threadPool = new ThreadPool(m_mainLoop, threadNum);
	setListen();
}

TcpServer::~TcpServer()//可以不提供
{
}

void TcpServer::setListen()
{
	//1.创建监听的fd
	m_lfd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_lfd == -1) {
		perror("socket");
		return;
	}
	//2.设置端口复用
	int opt = 1;//代表可以端口复用
	int ret = setsockopt(m_lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
	if (ret == -1) {  //设置失败返回-1
		perror("setsockopt");
		return;
	}
	//3.绑定
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(m_port);
	addr.sin_addr.s_addr = INADDR_ANY; //0 表示任意ip  如果指定ip需要转换成大端格式
	ret = bind(m_lfd, (struct sockaddr*)&addr, sizeof addr);
	if (ret == -1) {
		perror("bind");
		return;
	}
	//4.设置监听
	ret = listen(m_lfd, 128); //128即可
	if (ret == -1) {
		perror("listen");
		return;
	}
}

void TcpServer::run()
{
	//启动线程池
	m_threadPool->run();
	//初始化一个channel实例
	Channel* channel = new Channel(m_lfd,FDEvent::ReadEvent, acceptConnection, nullptr, nullptr, this);//待补充
	//添加检测任务
	m_mainLoop->addTask(channel, ElemType::ADD); //将channel ADD到mainLoop中
	//启动反应堆模型
	m_mainLoop->run();
}

