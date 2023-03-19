#pragma once
#include"Dispatcher.h"
#include<thread>
#include"TcpConnection.h"
#include<mutex>
#include<queue>
#include<map>
using namespace std;
enum class ElemType:char { ADD, DELETE, MODIFY };
//����������еĽڵ�
struct ChannelElement{
	ElemType type;  //��Channel�Ĵ�����
	Channel* channel;
};

class Dispatcher;//ǰ������

class EventLoop{
public:
	//��ʼ��
	EventLoop();
	EventLoop(const string threadName);//�вι���
	~EventLoop(); //���������ڼ�EventLoop���ᱻ�رգ����Բ���Ҫ����������Ҫ�ṩ��������

	//������Ӧ��ģ��
	int run();

	//���������fd
	int eventActive(int fd, int event);

	//������������������
	int addTask(Channel* channel, ElemType type);

	//������������е�����
	int processTaskQ();

	//����dispatcher�еĽڵ�
	int add(Channel* channel);
	int remove(Channel* channel);
	int modify(Channel* channel);

	//�ͷ�channel
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
	//����ָ��ָ������ʵ�� poll epoll select
	Dispatcher* m_dispatcher;

	//������� ����ʵ��
	queue<ChannelElement*> m_taskQ;

	//map
	map<int, Channel*> m_channelMap;

	//�߳�id��name, mutex
	thread::id m_threadID;
	string m_threadName;
	mutex m_mutex;//������
	int m_socketPair[2]; //���ڱ���ͨ�ŵ�socket fd

};

