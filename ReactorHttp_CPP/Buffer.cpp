#define _GNU_SOURCE
#include "Buffer.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<strings.h>
#include<sys/uio.h>
#include<unistd.h>
#include<sys/socket.h>


Buffer::Buffer(int size):m_capacity(size)
{
	m_data = (char*)malloc(size);
	bzero(m_data, size);
}

Buffer::~Buffer()
{
	if (m_data != nullptr) {
		free(m_data);
	}
}

void Buffer::extendRoom(int size)
{
	if (writeableSize() >= size) {//内存够写
		return;
	}
	else if (m_readPos + writeableSize() >= size) {
		//得到未读内存大小
		int readable = readableSize();
		//移动内存
		memcpy(m_data, m_data + m_readPos, readable);
		//更新位置
		m_readPos = 0;
		m_writePos = readable;
	}
	else {//内存不够用 扩容
		void* temp = realloc(m_data, m_capacity + size);
		if (temp == NULL) {
			return;
		}
		memset((char*)temp + m_capacity, 0, size);
		//更新数据
		m_data = static_cast<char*>(temp);
		m_capacity += size;
	}
}


int Buffer::appendString(const char* data, int size)
{
	if (data == nullptr || size <= 0) {//非法情况
		return -1;
	}
	//扩容
	extendRoom(size);//保证空间足够
	memcpy(m_data + m_writePos, data, size);//从buffer->data的写指针位置开始写
	m_writePos += size;//更新写指针位置
	return 0;
}

int Buffer::appendString(const char* data)
{
	int size = strlen(data);
	int ret = appendString(data, size);
	return ret;
}

int Buffer::appendString(const string data)
{
	int ret = appendString(data.data());
	return ret;
}

int Buffer::socketRead(int fd)
{
	struct iovec vec[2]; //readv需要的结构体数组
	//初始化数组元素
	int writeable = writeableSize();
	vec[0].iov_base = m_data + m_writePos;
	vec[0].iov_len = writeable;

	char* tmpbuf = (char*)malloc(40960);//第二块空间手动申请40k  需要释放
	vec[1].iov_base = m_data + m_writePos;
	vec[1].iov_len = 40960;

	int result = readv(fd, vec, 2);//2表示2块内存
	if (result == -1) { //readv失败返回-1
		return -1;
	}
	else if (result <= writeable) {
		//此时数据全在vec[0] 即(buffer->data)里
		m_writePos += result;//移动写指针
	}
	else {//此时vec[1]里有部分数据，应当拓展buffer->data并拷贝进去 注意此时data写满，写指针应当指向末尾
		m_writePos = m_capacity;
		appendString(tmpbuf, result - writeable);
	}
	free(tmpbuf);
	return result;
}

char* Buffer::findCRLF()
{
	char* ptr = static_cast<char*>(memmem(m_data + m_readPos, readableSize(), "\r\n", 2));
	return ptr;
}

int Buffer::sendData(int socket)
{
	//判断buffer是否有数据
	int readable = readableSize();
	if (readable > 0) {
		int count = send(socket, m_data + m_readPos, readable, MSG_NOSIGNAL);
		if (count > 0) {
			m_readPos += count;
			usleep(1);
		}
		return count;
	}
	return 0;
}
