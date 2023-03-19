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
	m_workerThreads.clear();//���vector
}

ThreadPool::~ThreadPool()//ͬ�� ��Ϊ�����������ڼ����������Բ��ṩ
{
	for (auto item : m_workerThreads) {
		delete item;
	}
}

void ThreadPool::run()
{
	assert(!m_isStart);//�����ʱ�̳߳��Ѿ�������
	if (m_mainLoop->getThreadID() != this_thread::get_id()) { //���ִ������������̲߳������߳�
		exit(0);
	}
	m_isStart = true;
	if (m_threadNum > 0) {
		for (int i = 0; i < m_threadNum; ++i) {
			WorkerThread* subThread = new WorkerThread(i);
			subThread->run();
			m_workerThreads.push_back(subThread); //����洢�̵߳�ַ��vector
		}
	}
}

EventLoop* ThreadPool::takeWorkerEventLoop()
{
	assert(m_isStart);
	if (m_mainLoop->getThreadID() != this_thread::get_id()) { //���ִ������������̲߳������߳�
		exit(0);
	}
	//���̳߳�����һ���ӽ��̣�Ȼ��ȡ������ķ�Ӧ��ʵ��
	EventLoop* evLoop = m_mainLoop; //��ʼ��evLoopΪ���̣߳����û�����߳̾�ʹ�����̴߳���
	if (m_threadNum > 0) {
		evLoop = m_workerThreads[m_index]->getEventLoop();
		m_index = ++m_index % m_threadNum;//ѭ��index����������ȷ����ÿһ�����߳�
	}
	return evLoop;
}