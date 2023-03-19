#include "WorkerThread.h"
#include<stdio.h>


WorkerThread::WorkerThread(int index)
{
	m_evLoop = nullptr;
	m_thread = nullptr;
	m_threadID = thread::id(); //thread::id()初始化一个无效id
	m_name = "SubThread-" + to_string(index); //to_string(int) 整数转换为字符串
}

WorkerThread::~WorkerThread()//子线程属于线程池，线程池运行期间不会销毁线程，此析构函数也就不会被调用  可以不提供
{
	if (m_thread != nullptr) {
		delete m_thread;
	}
}

void WorkerThread::run()
{
	//创建子线程
	m_thread = new thread(&WorkerThread::running, this); //让子线程做的函数指针，函数的所有者
	//阻塞主线程到子线程结束
	unique_lock<mutex> locker(m_mutex);
	while (m_evLoop == nullptr) {
		m_cond.wait(locker);
	}
}

void WorkerThread::running()
{
	m_mutex.lock();
	m_evLoop = new EventLoop(m_name);
	m_mutex.unlock();
	m_cond.notify_one();//唤醒主进程  .notify_one()唤醒一个 all唤醒所有
	m_evLoop->run();
}
