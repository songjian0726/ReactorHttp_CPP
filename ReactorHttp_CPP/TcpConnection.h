#pragma once

#include"EventLoop.h"
#include"Buffer.h"
#include"Channel.h"
#include"HttpRequest.h"
#include"HttpResponse.h"
using namespace std;

//#define MSG_SEND_AUTO  //控制两种发送方式 一种是全读到buf中一起发送 另一种是读一点发一点

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
	//http协议
	HttpRequest* m_request;
	HttpResponse* m_response;
};

