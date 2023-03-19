#include "TcpConnection.h"
#include<stdio.h>
#include<stdlib.h>
#include"Log.h"

int TcpConnection::processRead(void* arg) {
	TcpConnection* conn = static_cast<TcpConnection*>(arg);
	//接收
	int socket = conn->m_channel->getSocket();
	int count = conn->m_readBuf->socketRead(socket);
	if (count > 0) {//成功接收
		//解析http
#ifdef MSG_SEND_AUTO
		conn->m_channel->writeEventEnable(true);
		conn->m_evLoop->addTask(conn->m_channel, ElemType::MODIFY);
#endif
		bool flag = conn->m_request->parseHttpRequest(conn->m_readBuf, conn->m_response, conn->m_writeBuf, socket);

		if (!flag) {
			//解析失败 回复一个简单的html
			string errMsg = "Http/1.1 400 Bad Request\r\n\r\n";
			conn->m_writeBuf->appendString(errMsg);
		}
	}
	else {
		//断开连接
#ifdef MSG_SEND_AUTO
		conn->m_evLoop->addTask(conn->m_channel, ElemType::DELETE);
#endif
	}
	//断开连接 第一种发送方式中，此处不能断开连接
#ifndef MSG_SEND_AUTO
	conn->m_evLoop->addTask(conn->m_channel, ElemType::DELETE);
#endif
	return 0;
}

int TcpConnection::processWrite(void* arg) {
	TcpConnection* conn = static_cast<TcpConnection*>(arg);
	Debug("开始发送数据(基于写事件发送)...\n");
	//发送数据
	int count = conn->m_writeBuf->sendData(conn->m_channel->getSocket());
	if (count > 0) {
		//判断数据是否被完全发送
		if (conn->m_writeBuf->readableSize() == 0) {
			//1.不再检测写事件 修改channel中保存的事件
			conn->m_channel->writeEventEnable(false);
			//2.修改dispatcher检测的集合
			conn->m_evLoop->addTask(conn->m_channel, ElemType::MODIFY);
			//3.删除这个节点
			conn->m_evLoop->addTask(conn->m_channel, ElemType::DELETE);
		}
	}
	return 0;
}

int TcpConnection::destroy(void* arg){
	TcpConnection* conn = static_cast<TcpConnection*>(arg);
	if (conn != nullptr) {
		delete conn;
	}
	return 0;
}

TcpConnection::TcpConnection(int fd, EventLoop* evLoop)
{
	m_evLoop = evLoop;
	m_readBuf = new Buffer(10240);//初始化10k
	m_writeBuf = new Buffer(10240);
	//http
	m_request = new HttpRequest;
	m_response = new HttpResponse;
	m_name = "Connection-" + to_string(fd);
	m_channel = new Channel(fd, FDEvent::ReadEvent, processRead, processWrite, destroy, this);
	evLoop->addTask(m_channel, ElemType::ADD);

}

TcpConnection::~TcpConnection()
{
	if (m_readBuf && m_readBuf->readableSize() == 0 && m_writeBuf && m_writeBuf->readableSize() == 0) {
		delete m_readBuf;
		delete m_writeBuf;
		delete m_request;
		delete m_response;
		m_evLoop->freeChannel(m_channel);
	}
}
