#include"Dispatcher.h"
#include<sys/select.h>
#include<stdio.h>
#include<stdlib.h>
#include "SelectDispatcher.h"

SelectDispatcher::SelectDispatcher(EventLoop* evLoop) :Dispatcher(evLoop)
{
	FD_ZERO(&m_readSet);
	FD_ZERO(&m_writeSet);
	m_name = "Select";
}

SelectDispatcher::~SelectDispatcher()
{
	
}

int SelectDispatcher::add()
{
	if (m_channel->getSocket() >= m_maxSize) {
		return -1;
	}
	setFdSet();
	return 0;//成功返回0 失败返回-1
}

int SelectDispatcher::remove()
{
	clearFdSet();
	//通过channel释放对应的tcpconnection资源
	m_channel->destroyCallback(const_cast<void*>(m_channel->getArg()));
	return 0;//成功返回0 失败返回-1
}

int SelectDispatcher::modify()
{
	setFdSet();
	clearFdSet();
	return 0;//成功返回0 失败返回-1
}

int SelectDispatcher::dispatch(int timeout)
{
	struct timeval val;
	val.tv_sec = timeout;
	val.tv_usec = 0;
	fd_set rdtmp = m_readSet;
	fd_set wrtmp = m_writeSet;
	int count = select(m_maxSize, &rdtmp, &wrtmp, NULL, &val);//timeout转化为毫秒 count为变化的个数
	if (count == -1) {
		perror("select");
		exit(0);
	}
	for (int i = 0; i < m_maxSize; ++i) {  //i是select中遍历时的文件描述符
		if (FD_ISSET(i, &rdtmp)) {
			m_evLoop->eventActive(i, (int)FDEvent::ReadEvent);
		}
		if (FD_ISSET(i, &wrtmp)) {
			m_evLoop->eventActive(i, (int)FDEvent::WriteEvent);
		}
	}
	return 0;
}

void SelectDispatcher::setFdSet()
{
	if (m_channel->getEvent() & (int)FDEvent::ReadEvent) {  //将自己定义的读写事件标志转化为该io复用系统的标志
		FD_SET(m_channel->getSocket(), &m_readSet);
	}
	if (m_channel->getEvent() & (int)FDEvent::WriteEvent) {
		FD_SET(m_channel->getSocket(), &m_writeSet);
	}
}

void SelectDispatcher::clearFdSet()
{
	if (m_channel->getEvent() & (int)FDEvent::ReadEvent) {  //将自己定义的读写事件标志转化为该io复用系统的标志
		FD_CLR(m_channel->getSocket(), &m_readSet);
	}
	if (m_channel->getEvent() & (int)FDEvent::WriteEvent) {
		FD_CLR(m_channel->getSocket(), &m_writeSet);
	}
}
