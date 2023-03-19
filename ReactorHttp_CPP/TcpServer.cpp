#include "TcpServer.h"
#include"TcpConnection.h"
#include<arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#include"Log.h"

int TcpServer::acceptConnection(void* arg){
	TcpServer* server = static_cast<TcpServer*>(arg);
	//accept��ȡ
	int cfd = accept(server->m_lfd, NULL, NULL);
	//���̳߳���ѡ��һ�����߳̽��մ���cfd
	EventLoop* evLoop = server->m_threadPool->takeWorkerEventLoop();
	//cfd�ŵ�TcpConnection��
	TcpConnection* conn = new TcpConnection(cfd, evLoop);
	return 0;
}


TcpServer::TcpServer(unsigned short port, int threadNum)
{
	m_port = port;
	m_mainLoop = new EventLoop(); //���̵߳�EventLoop����Ҫ����
	m_threadNum = threadNum;
	m_threadPool = new ThreadPool(m_mainLoop, threadNum);
	setListen();
}

TcpServer::~TcpServer()//���Բ��ṩ
{
}

void TcpServer::setListen()
{
	//1.����������fd
	m_lfd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_lfd == -1) {
		perror("socket");
		return;
	}
	//2.���ö˿ڸ���
	int opt = 1;//������Զ˿ڸ���
	int ret = setsockopt(m_lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
	if (ret == -1) {  //����ʧ�ܷ���-1
		perror("setsockopt");
		return;
	}
	//3.��
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(m_port);
	addr.sin_addr.s_addr = INADDR_ANY; //0 ��ʾ����ip  ���ָ��ip��Ҫת���ɴ�˸�ʽ
	ret = bind(m_lfd, (struct sockaddr*)&addr, sizeof addr);
	if (ret == -1) {
		perror("bind");
		return;
	}
	//4.���ü���
	ret = listen(m_lfd, 128); //128����
	if (ret == -1) {
		perror("listen");
		return;
	}
}

void TcpServer::run()
{
	//�����̳߳�
	m_threadPool->run();
	//��ʼ��һ��channelʵ��
	Channel* channel = new Channel(m_lfd,FDEvent::ReadEvent, acceptConnection, nullptr, nullptr, this);//������
	//��Ӽ������
	m_mainLoop->addTask(channel, ElemType::ADD); //��channel ADD��mainLoop��
	//������Ӧ��ģ��
	m_mainLoop->run();
}

