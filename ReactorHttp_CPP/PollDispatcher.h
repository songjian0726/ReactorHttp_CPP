#pragma once

#include"Channel.h"
#include"EventLoop.h"
#include"Dispatcher.h"
#include<string>
#include<poll.h>
using namespace std;

class PollDispatcher :public Dispatcher
{
public:
	PollDispatcher(EventLoop* evLoop);
	~PollDispatcher(); //析构函数必须是虚函数
	//添加
	int add() override;//复写关键字 帮助检查函数名等
	//删除
	int remove() override;
	//修改
	int modify() override;
	//事件检测
	int dispatch(int timeout = 2) override;//超时 timeout单位：s

private:
	int m_maxfd;
	struct pollfd *m_fds;
	const int m_maxNode = 1024;

};