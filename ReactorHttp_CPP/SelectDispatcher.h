#pragma once

#include"Channel.h"
#include"EventLoop.h"
#include"Dispatcher.h"
#include<string>
#include<sys/select.h>
using namespace std;

class SelectDispatcher :public Dispatcher
{
public:
	SelectDispatcher(EventLoop* evLoop);
	~SelectDispatcher(); //���������������麯��
	//���
	int add() override;//��д�ؼ��� ������麯������
	//ɾ��
	int remove() override;
	//�޸�
	int modify() override;
	//�¼����
	int dispatch(int timeout = 2) override;//��ʱ timeout��λ��s
private:
	void setFdSet();
	void clearFdSet();


private:
	fd_set m_readSet;
	fd_set m_writeSet;
	const int m_maxSize = 1024;
};