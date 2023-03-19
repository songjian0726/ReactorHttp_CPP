#pragma once
#include"WorkerThread.h"
#include"EventLoop.h"
#include<vector>
class ThreadPool {
public:
	ThreadPool(EventLoop* mainLoop, int count);
	~ThreadPool();

	//�����̳߳�
	void run();

	//ȡ���̳߳���ĳ�����̵߳ķ�Ӧ��ʵ��
	EventLoop* takeWorkerEventLoop();
	//���̵߳ķ�Ӧ��
private:
	EventLoop* m_mainLoop;
	bool m_isStart;
	int m_threadNum; //�̳߳��е��߳���
	vector<WorkerThread*> m_workerThreads;
	int m_index;
};
