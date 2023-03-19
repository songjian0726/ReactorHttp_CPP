#pragma once

#include"Channel.h"
#include"EventLoop.h"
#include"Dispatcher.h"
#include<string>
#include"sys/epoll.h"
using namespace std;

class EpollDispatcher :public Dispatcher
{
public:
	EpollDispatcher(EventLoop* evLoop);
	~EpollDispatcher(); //析构函数必须是虚函数
	//添加
	int add() override;//复写关键字 帮助检查函数名等
	//删除
	int remove() override;
	//修改
	int modify() override;
	//事件检测
	int dispatch(int timeout = 2) override;//超时 timeout单位：s
private:
	int epollCtl(int op);

private:
	int m_epfd;
	struct epoll_event* m_events;
	const int m_maxNode = 520;

};