#pragma once

#include"Channel.h"
#include"EventLoop.h"
#include"Dispatcher.h"
#include<string>
#include<sys/select.h>
using namespace std;

class SelectDispatcher :public Dispatcher
{
public:
	SelectDispatcher(EventLoop* evLoop);
	~SelectDispatcher(); //析构函数必须是虚函数
	//添加
	int add() override;//复写关键字 帮助检查函数名等
	//删除
	int remove() override;
	//修改
	int modify() override;
	//事件检测
	int dispatch(int timeout = 2) override;//超时 timeout单位：s
private:
	void setFdSet();
	void clearFdSet();


private:
	fd_set m_readSet;
	fd_set m_writeSet;
	const int m_maxSize = 1024;
};