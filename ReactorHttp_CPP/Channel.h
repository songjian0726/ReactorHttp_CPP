#pragma once
#include<functional>//��װ��
// #include<stdbool.h>  //cpp����֧��bool

using namespace std;

//�����ļ��������Ķ�д�¼�
enum class FDEvent { //ǿ����ö��
	TimeOut = 0x01,
	ReadEvent = 0x02,
	WriteEvent = 0x04
};


//�ص������ĺ���ָ��
//typedef int(*handleFunc)(void* arg);
//using handleFunc = int(*)(void*);  //using ���庯��ָ��


//�ɵ��ö����װ������� ����ָ�룬�ɵ��ö���(��������һ��ʹ��, �º���)
//�õ��ɵ��ö���ĵ�ַ 

class Channel {
public:
	using handleFunc = function<int(void*)>;
	Channel(int fd, FDEvent events, handleFunc readFunc, handleFunc writeFunc, handleFunc destroyFunc, void* arg);
	//�ص�����
	handleFunc readCallback;
	handleFunc writeCallback;
	handleFunc destroyCallback;
	//�޸�fd��д�¼�(���򲻼��)
	void writeEventEnable(bool flag);
	//�ж��Ƿ���Ҫ����ļ���������д�¼�
	bool isWriteEventEnable();
	//˽�г�Ա�ӿ�
	inline int getEvent() {
		return m_events;
	}
	inline int getSocket() {
		return m_fd;
	}
	inline const void* getArg() {  //����constֻ����ַ
		return m_arg;
	}
private:
	//�ļ�������
	int m_fd;
	//��Ӧ���¼�
	int m_events;
	//�ص���������
	void* m_arg;
};

