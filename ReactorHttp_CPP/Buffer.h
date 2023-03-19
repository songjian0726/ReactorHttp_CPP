#pragma once
#include <string>
using namespace std;

class Buffer {
public:
	Buffer(int size);
	~Buffer();
	//��ʼ��
	//�ṹ������
	void extendRoom(int size);
	//�鿴buf�п�д���ڴ�����
	inline int writeableSize() { return m_capacity - m_writePos; };
	//�鿴buf�пɶ����ڴ�����
	int readableSize() { return m_writePos - m_readPos; };
	//д�ڴ�ĺ���  ֱ��д��������׽�������
	//ֱ��д�ڴ�
	int appendString(const char* data, int size);
	int appendString(const char* data);
	int appendString(const string data);
	//�����׽�������
	int socketRead(int fd);
	//����\r\nȡ��һ��(�ҵ�\r\n�����ݿ��е�λ��)
	char* findCRLF();
	//��������
	int sendData(int socket);
	inline char* data() { return m_data + m_readPos; };

	inline int readPosIncrease(int count) {
		m_readPos += count;
		return m_readPos;
	}

private:
	char* m_data;
	int m_capacity;
	int m_readPos = 0;
	int m_writePos = 0;
};

