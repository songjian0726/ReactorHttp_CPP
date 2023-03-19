#pragma once
#include"Dispatcher.h"
#include<thread>
#include"TcpConnection.h"
#include<mutex>
#include<queue>
#include<map>
using namespace std;
enum class ElemType:char { ADD, DELETE, MODIFY };
//定义任务队列的节点
struct ChannelElement{
	ElemType type;  //对Channel的处理方法
	Channel* channel;
};

class Dispatcher;//前向引用

class EventLoop{
public:
	//初始化
	EventLoop();
	EventLoop(const string threadName);//有参构造
	~EventLoop(); //程序运行期间EventLoop不会被关闭，所以不需要析构，不需要提供析构函数

	//启动反应堆模型
	int run();

	//处理被激活的fd
	int eventActive(int fd, int event);

	//向任务队列中添加任务
	int addTask(Channel* channel, ElemType type);

	//处理任务队列中的任务
	int processTaskQ();

	//处理dispatcher中的节点
	int add(Channel* channel);
	int remove(Channel* channel);
	int modify(Channel* channel);

	//释放channel
	int freeChannel(Channel* channel);
	inline thread::id getThreadID() {
		return m_threadID;
	}
	static int readLocalMessage(void* arg);
	int readMessage();

private:
	void taskWakeup();


private:
	bool m_isQuit;
	//父类指针指向子类实例 poll epoll select
	Dispatcher* m_dispatcher;

	//任务队列 链表实现
	queue<ChannelElement*> m_taskQ;

	//map
	map<int, Channel*> m_channelMap;

	//线程id，name, mutex
	thread::id m_threadID;
	string m_threadName;
	mutex m_mutex;//互斥锁
	int m_socketPair[2]; //用于本地通信的socket fd

};

