#pragma once
#include<pthread.h>
#include"EventLoop.h"
#include<mutex>
#include<condition_variable>
using namespace std;

class WorkerThread{
public:
	WorkerThread(int index);
	~WorkerThread();
	void run();
	inline EventLoop* getEventLoop()
	{
		return m_evLoop;
	}

private:
	void running();

private:
	thread* m_thread;//保存线程
	thread::id m_threadID;//线程ID
	string m_name;
	mutex m_mutex;//互斥锁
	condition_variable m_cond;//条件变量
	EventLoop* m_evLoop; //反应堆模型
};


//初始化

