#pragma once

#include"EventLoop.h"
#include"Buffer.h"
#include"Channel.h"
#include"HttpRequest.h"
#include"HttpResponse.h"
using namespace std;

//#define MSG_SEND_AUTO  //�������ַ��ͷ�ʽ һ����ȫ����buf��һ���� ��һ���Ƕ�һ�㷢һ��

class EventLoop;

class TcpConnection {
public:
	TcpConnection(int fd, EventLoop* evLoop);
	~TcpConnection();
	static int processRead(void* arg);
	static int processWrite(void* arg);
	static int destroy(void* arg);

private:
	string m_name;
	EventLoop* m_evLoop;
	Channel* m_channel;
	Buffer* m_readBuf;
	Buffer* m_writeBuf;
	//httpЭ��
	HttpRequest* m_request;
	HttpResponse* m_response;
};

