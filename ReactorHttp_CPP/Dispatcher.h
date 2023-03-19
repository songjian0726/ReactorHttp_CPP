#pragma once
#include"Channel.h"
#include"EventLoop.h"
#include<cstdio>
#include<string>
using namespace std;

class EventLoop;

class Dispatcher
{
public:
	Dispatcher(EventLoop* evLoop);
	virtual ~Dispatcher(); //���������������麯��
	//���
	virtual int add();
	//ɾ��
	virtual int remove();
	//�޸�
	virtual int modify();
	//�¼����
	virtual int dispatch(int timeout = 2);//��ʱ timeout��λ��s

	inline void setChannel(Channel* channel) {
		m_channel = channel;
	}
protected:
	string m_name = string();
	Channel* m_channel;
	EventLoop* m_evLoop;
};