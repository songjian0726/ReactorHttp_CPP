#include"Dispatcher.h"
#include<sys/epoll.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include "EpollDispatcher.h"



EpollDispatcher::EpollDispatcher(EventLoop* evLoop):Dispatcher(evLoop)
{
	m_epfd = epoll_create(1);
	if (m_epfd == -1) {
		perror("epoll_create");
		exit(0);
	}
	m_events = new struct epoll_event[m_maxNode];
	m_name = "Epoll";
}

EpollDispatcher::~EpollDispatcher()
{
	close(m_epfd);
	delete[] m_events;
}

int EpollDispatcher::add()
{
	int ret = epollCtl(EPOLL_CTL_ADD);
	if (ret == -1) {
		perror("epoll_add");
		exit(0);
	}
	return ret;
}

int EpollDispatcher::remove()
{
	int ret = epollCtl(EPOLL_CTL_DEL);
	if (ret == -1) {
		perror("epoll_delete");
		exit(0);
	}
	//通过channel释放对应的tcpconnection资源
	m_channel->destroyCallback(const_cast<void*>(m_channel->getArg()));
	return ret;
}

int EpollDispatcher::modify()
{
	int ret = epollCtl(EPOLL_CTL_MOD);
	if (ret == -1) {
		perror("epoll_modify");
		exit(0);
	}
	return ret;
}

int EpollDispatcher::dispatch(int timeout)
{
	int count = epoll_wait(m_epfd, m_events, m_maxNode, timeout * 1000);//timeout转化为毫秒 count为变化的个数
	for (int i = 0; i < count; ++i) {
		int events = m_events[i].events;
		int fd = m_events[i].data.fd;
		if (events & EPOLLERR || events & EPOLLHUP) {//EPOLLERR对端断开连接 EPOLLHUP对方断开连接后我方发送信息
			//对方断开连接，下树 删除fd
			//epollRemove(channel, evLoop);
			continue;
		}
		if (events & EPOLLIN) {
			m_evLoop->eventActive(fd, (int)FDEvent::ReadEvent);
		}
		if (events & EPOLLOUT) {
			m_evLoop->eventActive(fd, (int)FDEvent::WriteEvent);
		}
	}
}

int EpollDispatcher::epollCtl(int op)
{
	struct epoll_event ev;
	ev.data.fd = m_channel->getSocket();//data是联合体
	int events = 0;
	if (m_channel->getEvent() & (int)FDEvent::ReadEvent) {  //将自己定义的读写事件标志转化为该io复用系统的标志
		events |= EPOLLIN;
	}
	if (m_channel->getEvent() & (int)FDEvent::WriteEvent) {
		events |= EPOLLOUT;
	}
	ev.events = events;
	int ret = epoll_ctl(m_epfd, op, m_channel->getSocket(), &ev);
	return ret;//失败返回-1}
}
