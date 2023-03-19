#include "EventLoop.h"
#include<assert.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include"Log.h"
#include<thread>
#include"SelectDispatcher.h"
#include"EpollDispatcher.h"
#include"PollDispatcher.h"


EventLoop::EventLoop() :EventLoop(string()) //ί�й��캯�� ע�ⲻ��ѭ������
{
}

EventLoop::EventLoop(const string threadName)
{
    m_isQuit = true; //Ĭ��û������
    m_threadID = this_thread::get_id(); //cpp��this_thread�����ռ��е�get_id()���� ���ص�ǰ�߳�id
    m_threadName = threadName == string() ? "MainThread" : threadName; //string()��ʾ���ַ���
    //m_dispatcher = new PollDispatcher(this); //ָ���ַ�ģ��
    m_dispatcher = new EpollDispatcher(this); //ָ���ַ�ģ��
    //m_dispatcher = new SelectDispatcher(this); //ָ���ַ�ģ��
    //���Map
    m_channelMap.clear();
    //��ʼ������socket fd
    int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, m_socketPair);
    if (ret == -1) {
        perror("socketpair");
        exit(0);
    }
#if 0
    //ָ��evLoop->socketPair[0]���ͣ�[1]���� ��socketPair[1]��װ��һ��channel
    Channel* channel = new Channel(m_socketPair[1], FDEvent::ReadEvent, readLocalMessage, nullptr, nullptr, this);//ֻ����������������ַ�ģ��
    //����װ��channel��ӵ��������
#else
    //���� bind()  ����һ���ɵ��ö���
    auto obj = bind(&EventLoop::readMessage, this);
    Channel* channel = new Channel(m_socketPair[1], FDEvent::ReadEvent, obj, nullptr, nullptr, this);//ֻ����������������ַ�ģ��


#endif

    addTask(channel, ElemType::ADD);
}


EventLoop::~EventLoop()
{
}

int EventLoop::run()
{
    m_isQuit = false;
    //�Ƚ��߳�ID�Ƿ�����
    if (m_threadID != this_thread::get_id()) {
        return -1;
    }
    //ѭ��
    while (!m_isQuit) {
        m_dispatcher->dispatch(); //��ʱ2s ��Ĭ�ϲ���
        processTaskQ();
    }
    return 0;
}

int EventLoop::eventActive(int fd, int event)
{
    if (fd < 0) {
        return -1;
    }
    //ȡ��channel
    Channel* channel = m_channelMap[fd];
    assert(channel->getSocket() == fd);
    if (event & (int)FDEvent::ReadEvent && channel->readCallback) { //���¼�
        channel->readCallback(const_cast<void*>(channel->getArg()));
    }
    if (event & (int)FDEvent::WriteEvent && channel->writeCallback) { //д�¼�
        channel->writeCallback(const_cast<void*>(channel->getArg()));
    }

    return 0;
    return 0;
}

int EventLoop::addTask(Channel* channel, ElemType type)
{
    //����������������Դ
    m_mutex.lock();
    ChannelElement* node = new ChannelElement;
    node->channel = channel;
    node->type = type;
    m_taskQ.push(node); //��ӵ��������
    m_mutex.unlock();
    //����ڵ�
    if (m_threadID == this_thread::get_id()) {
        processTaskQ();//��ǰ�߳�Ϊ���߳�
    }
    else {
        taskWakeup();//���߳��������ʱ �����̴߳�������е�����
    }
    return 0;
}

int EventLoop::processTaskQ()
{
    while (!m_taskQ.empty()) {
        m_mutex.lock();//������ֻ��Ҫ���������еĴ���
        ChannelElement* node = m_taskQ.front();
        m_taskQ.pop();
        m_mutex.unlock();
        Channel* channel = node->channel;
        if (node->type == ElemType::ADD) {
            //���
            add(channel);
        }
        else if (node->type == ElemType::DELETE) {
            //ɾ��
            remove(channel);

        }
        else if (node->type == ElemType::MODIFY) {
            //�޸�
            modify(channel);
        }
        delete node;
    }
    return 0;
}

int EventLoop::add(Channel* channel)
{
    int fd = channel->getSocket();
    //�ҵ�fd��channelMap�е�λ�ò��洢
    if (m_channelMap.find(fd) == m_channelMap.end()) { //map�����Ҳ���key��Ӧ��valueʱ�᷵��.end()
        m_channelMap.insert(make_pair(fd, channel));
        m_dispatcher->setChannel(channel);
        int ret = m_dispatcher->add();
        return ret;
    }
    return -1;
}

int EventLoop::remove(Channel* channel)
{
    int fd = channel->getSocket();
    if (m_channelMap.find(fd) == m_channelMap.end()) {
        return -1;
    }
    m_dispatcher->setChannel(channel);
    int ret = m_dispatcher->remove();
    return ret;
}

int EventLoop::modify(Channel* channel)
{
    int fd = channel->getSocket();
    if (m_channelMap.find(fd) == m_channelMap.end()) {
        return -1;
    }
    m_dispatcher->setChannel(channel);
    int ret = m_dispatcher->modify();
    return ret;
}

int EventLoop::freeChannel(Channel* channel)
{
    //ɾ��channel��fd��ӳ��
    auto it = m_channelMap.find(channel->getSocket());
    if (it != m_channelMap.end()) {
        //map��ɾ����ֵ���õ����� it
        m_channelMap.erase(it);
        //�ر�fd
        close(channel->getSocket());
        //�ͷ�channel
        delete channel;
    }
    return -1;
}

int EventLoop::readLocalMessage(void* arg)
{
    EventLoop* evloop = static_cast<EventLoop*>(arg);
    char buf[256];
    read(evloop->m_socketPair[1], buf, sizeof(buf));//�������� ���账��
    return 0;
}

int EventLoop::readMessage()
{
    char buf[256];
    read(m_socketPair[1], buf, sizeof(buf));//�������� ���账��
    return 0;
}

void EventLoop::taskWakeup()
{
    const char* msg = "ף������仰�������쿪�ģ�";
    write(m_socketPair[0], msg, strlen(msg));
}
