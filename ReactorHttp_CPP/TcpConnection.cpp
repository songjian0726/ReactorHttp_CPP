#include "TcpConnection.h"
#include<stdio.h>
#include<stdlib.h>
#include"Log.h"

int TcpConnection::processRead(void* arg) {
	TcpConnection* conn = static_cast<TcpConnection*>(arg);
	//����
	int socket = conn->m_channel->getSocket();
	int count = conn->m_readBuf->socketRead(socket);
	if (count > 0) {//�ɹ�����
		//����http
#ifdef MSG_SEND_AUTO
		conn->m_channel->writeEventEnable(true);
		conn->m_evLoop->addTask(conn->m_channel, ElemType::MODIFY);
#endif
		bool flag = conn->m_request->parseHttpRequest(conn->m_readBuf, conn->m_response, conn->m_writeBuf, socket);

		if (!flag) {
			//����ʧ�� �ظ�һ���򵥵�html
			string errMsg = "Http/1.1 400 Bad Request\r\n\r\n";
			conn->m_writeBuf->appendString(errMsg);
		}
	}
	else {
		//�Ͽ�����
#ifdef MSG_SEND_AUTO
		conn->m_evLoop->addTask(conn->m_channel, ElemType::DELETE);
#endif
	}
	//�Ͽ����� ��һ�ַ��ͷ�ʽ�У��˴����ܶϿ�����
#ifndef MSG_SEND_AUTO
	conn->m_evLoop->addTask(conn->m_channel, ElemType::DELETE);
#endif
	return 0;
}

int TcpConnection::processWrite(void* arg) {
	TcpConnection* conn = static_cast<TcpConnection*>(arg);
	Debug("��ʼ��������(����д�¼�����)...\n");
	//��������
	int count = conn->m_writeBuf->sendData(conn->m_channel->getSocket());
	if (count > 0) {
		//�ж������Ƿ���ȫ����
		if (conn->m_writeBuf->readableSize() == 0) {
			//1.���ټ��д�¼� �޸�channel�б�����¼�
			conn->m_channel->writeEventEnable(false);
			//2.�޸�dispatcher���ļ���
			conn->m_evLoop->addTask(conn->m_channel, ElemType::MODIFY);
			//3.ɾ������ڵ�
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
	m_readBuf = new Buffer(10240);//��ʼ��10k
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
