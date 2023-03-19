#pragma once
#include"WorkerThread.h"
#include"EventLoop.h"
#include<vector>
class ThreadPool {
public:
	ThreadPool(EventLoop* mainLoop, int count);
	~ThreadPool();

	//启动线程池
	void run();

	//取出线程池中某个子线程的反应堆实例
	EventLoop* takeWorkerEventLoop();
	//主线程的反应堆
private:
	EventLoop* m_mainLoop;
	bool m_isStart;
	int m_threadNum; //线程池中的线程数
	vector<WorkerThread*> m_workerThreads;
	int m_index;
};
