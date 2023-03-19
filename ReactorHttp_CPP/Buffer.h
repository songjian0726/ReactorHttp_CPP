#pragma once
#include <string>
using namespace std;

class Buffer {
public:
	Buffer(int size);
	~Buffer();
	//初始化
	//结构体扩容
	void extendRoom(int size);
	//查看buf中可写的内存容量
	inline int writeableSize() { return m_capacity - m_writePos; };
	//查看buf中可读的内存容量
	int readableSize() { return m_writePos - m_readPos; };
	//写内存的函数  直接写，或接收套接字数据
	//直接写内存
	int appendString(const char* data, int size);
	int appendString(const char* data);
	int appendString(const string data);
	//接收套接字数据
	int socketRead(int fd);
	//根据\r\n取出一行(找到\r\n在数据块中的位置)
	char* findCRLF();
	//发送数据
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

