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


EventLoop::EventLoop() :EventLoop(string()) //委托构造函数 注意不能循环调用
{
}

EventLoop::EventLoop(const string threadName)
{
    m_isQuit = true; //默认没有启动
    m_threadID = this_thread::get_id(); //cpp的this_thread命名空间中的get_id()函数 返回当前线程id
    m_threadName = threadName == string() ? "MainThread" : threadName; //string()表示空字符串
    //m_dispatcher = new PollDispatcher(this); //指定分发模型
    m_dispatcher = new EpollDispatcher(this); //指定分发模型
    //m_dispatcher = new SelectDispatcher(this); //指定分发模型
    //清空Map
    m_channelMap.clear();
    //初始化本地socket fd
    int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, m_socketPair);
    if (ret == -1) {
        perror("socketpair");
        exit(0);
    }
#if 0
    //指定evLoop->socketPair[0]发送，[1]接收 将socketPair[1]封装成一个channel
    Channel* channel = new Channel(m_socketPair[1], FDEvent::ReadEvent, readLocalMessage, nullptr, nullptr, this);//只用来读，用来激活分发模型
    //将封装的channel添加到任务队列
#else
    //绑定器 bind()  返回一个可调用对象
    auto obj = bind(&EventLoop::readMessage, this);
    Channel* channel = new Channel(m_socketPair[1], FDEvent::ReadEvent, obj, nullptr, nullptr, this);//只用来读，用来激活分发模型


#endif

    addTask(channel, ElemType::ADD);
}


EventLoop::~EventLoop()
{
}

int EventLoop::run()
{
    m_isQuit = false;
    //比较线程ID是否正常
    if (m_threadID != this_thread::get_id()) {
        return -1;
    }
    //循环
    while (!m_isQuit) {
        m_dispatcher->dispatch(); //超时2s 有默认参数
        processTaskQ();
    }
    return 0;
}

int EventLoop::eventActive(int fd, int event)
{
    if (fd < 0) {
        return -1;
    }
    //取出channel
    Channel* channel = m_channelMap[fd];
    assert(channel->getSocket() == fd);
    if (event & (int)FDEvent::ReadEvent && channel->readCallback) { //读事件
        channel->readCallback(const_cast<void*>(channel->getArg()));
    }
    if (event & (int)FDEvent::WriteEvent && channel->writeCallback) { //写事件
        channel->writeCallback(const_cast<void*>(channel->getArg()));
    }

    return 0;
    return 0;
}

int EventLoop::addTask(Channel* channel, ElemType type)
{
    //加锁，保护共享资源
    m_mutex.lock();
    ChannelElement* node = new ChannelElement;
    node->channel = channel;
    node->type = type;
    m_taskQ.push(node); //添加到任务队列
    m_mutex.unlock();
    //处理节点
    if (m_threadID == this_thread::get_id()) {
        processTaskQ();//当前线程为子线程
    }
    else {
        taskWakeup();//主线程添加任务时 让子线程处理队列中的任务
    }
    return 0;
}

int EventLoop::processTaskQ()
{
    while (!m_taskQ.empty()) {
        m_mutex.lock();//互斥锁只需要锁操作队列的代码
        ChannelElement* node = m_taskQ.front();
        m_taskQ.pop();
        m_mutex.unlock();
        Channel* channel = node->channel;
        if (node->type == ElemType::ADD) {
            //添加
            add(channel);
        }
        else if (node->type == ElemType::DELETE) {
            //删除
            remove(channel);

        }
        else if (node->type == ElemType::MODIFY) {
            //修改
            modify(channel);
        }
        delete node;
    }
    return 0;
}

int EventLoop::add(Channel* channel)
{
    int fd = channel->getSocket();
    //找到fd在channelMap中的位置并存储
    if (m_channelMap.find(fd) == m_channelMap.end()) { //map容器找不到key对应的value时会返回.end()
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
    //删除channel和fd的映射
    auto it = m_channelMap.find(channel->getSocket());
    if (it != m_channelMap.end()) {
        //map中删除键值对用迭代器 it
        m_channelMap.erase(it);
        //关闭fd
        close(channel->getSocket());
        //释放channel
        delete channel;
    }
    return -1;
}

int EventLoop::readLocalMessage(void* arg)
{
    EventLoop* evloop = static_cast<EventLoop*>(arg);
    char buf[256];
    read(evloop->m_socketPair[1], buf, sizeof(buf));//读出即可 无需处理
    return 0;
}

int EventLoop::readMessage()
{
    char buf[256];
    read(m_socketPair[1], buf, sizeof(buf));//读出即可 无需处理
    return 0;
}

void EventLoop::taskWakeup()
{
    const char* msg = "祝看到这句话的人天天开心！";
    write(m_socketPair[0], msg, strlen(msg));
}
