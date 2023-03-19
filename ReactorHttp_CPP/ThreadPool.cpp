#include "ThreadPool.h"
#include<assert.h>
#include<stdlib.h>
#include"Log.h"





ThreadPool::ThreadPool(EventLoop* mainLoop, int count)
{
	m_index = 0;
	m_isStart = false;
	m_mainLoop = mainLoop;
	m_threadNum = count;
	m_workerThreads.clear();//清空vector
}

ThreadPool::~ThreadPool()//同理 因为不会在运行期间析构，可以不提供
{
	for (auto item : m_workerThreads) {
		delete item;
	}
}

void ThreadPool::run()
{
	assert(!m_isStart);//如果此时线程池已经在运行
	if (m_mainLoop->getThreadID() != this_thread::get_id()) { //如果执行这个函数的线程不是主线程
		exit(0);
	}
	m_isStart = true;
	if (m_threadNum > 0) {
		for (int i = 0; i < m_threadNum; ++i) {
			WorkerThread* subThread = new WorkerThread(i);
			subThread->run();
			m_workerThreads.push_back(subThread); //加入存储线程地址的vector
		}
	}
}

EventLoop* ThreadPool::takeWorkerEventLoop()
{
	assert(m_isStart);
	if (m_mainLoop->getThreadID() != this_thread::get_id()) { //如果执行这个函数的线程不是主线程
		exit(0);
	}
	//从线程池中找一个子进程，然后取出里面的反应堆实例
	EventLoop* evLoop = m_mainLoop; //初始化evLoop为主线程，如果没有子线程就使用主线程处理
	if (m_threadNum > 0) {
		evLoop = m_workerThreads[m_index]->getEventLoop();
		m_index = ++m_index % m_threadNum;//循环index，将任务均匀分配给每一个子线程
	}
	return evLoop;
}
