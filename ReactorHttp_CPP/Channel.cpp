#include "Channel.h"
#include<stdlib.h>
#include"Log.h"


Channel::Channel(int fd, FDEvent events, handleFunc readFunc, handleFunc writeFunc, handleFunc destroyFunc, void* arg)
{
	m_arg = arg;
	m_fd = fd;
	m_events = (int)events;
	readCallback = readFunc;
	writeCallback = writeFunc;
	destroyCallback = destroyFunc;
}

void Channel::writeEventEnable(bool flag)
{
	if (flag) {
		//m_events |= (int)FDEvent::WriteEvent; //c����ǿת
		m_events |= static_cast<int>(FDEvent::WriteEvent);//cpp����ǿ������ת��

	}
	else {
		m_events = m_events & ~(int)FDEvent::WriteEvent;  //λ�� WriteEvent�ķ�
	}
}

bool Channel::isWriteEventEnable()
{
	return m_events & (int)FDEvent::WriteEvent;
}
