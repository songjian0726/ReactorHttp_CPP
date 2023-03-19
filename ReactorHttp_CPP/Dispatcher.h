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
	virtual ~Dispatcher(); //析构函数必须是虚函数
	//添加
	virtual int add();
	//删除
	virtual int remove();
	//修改
	virtual int modify();
	//事件检测
	virtual int dispatch(int timeout = 2);//超时 timeout单位：s

	inline void setChannel(Channel* channel) {
		m_channel = channel;
	}
protected:
	string m_name = string();
	Channel* m_channel;
	EventLoop* m_evLoop;
};