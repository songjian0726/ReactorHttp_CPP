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
	if (writeableSize() >= size) {//�ڴ湻д
		return;
	}
	else if (m_readPos + writeableSize() >= size) {
		//�õ�δ���ڴ��С
		int readable = readableSize();
		//�ƶ��ڴ�
		memcpy(m_data, m_data + m_readPos, readable);
		//����λ��
		m_readPos = 0;
		m_writePos = readable;
	}
	else {//�ڴ治���� ����
		void* temp = realloc(m_data, m_capacity + size);
		if (temp == NULL) {
			return;
		}
		memset((char*)temp + m_capacity, 0, size);
		//��������
		m_data = static_cast<char*>(temp);
		m_capacity += size;
	}
}


int Buffer::appendString(const char* data, int size)
{
	if (data == nullptr || size <= 0) {//�Ƿ����
		return -1;
	}
	//����
	extendRoom(size);//��֤�ռ��㹻
	memcpy(m_data + m_writePos, data, size);//��buffer->data��дָ��λ�ÿ�ʼд
	m_writePos += size;//����дָ��λ��
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
	struct iovec vec[2]; //readv��Ҫ�Ľṹ������
	//��ʼ������Ԫ��
	int writeable = writeableSize();
	vec[0].iov_base = m_data + m_writePos;
	vec[0].iov_len = writeable;

	char* tmpbuf = (char*)malloc(40960);//�ڶ���ռ��ֶ�����40k  ��Ҫ�ͷ�
	vec[1].iov_base = m_data + m_writePos;
	vec[1].iov_len = 40960;

	int result = readv(fd, vec, 2);//2��ʾ2���ڴ�
	if (result == -1) { //readvʧ�ܷ���-1
		return -1;
	}
	else if (result <= writeable) {
		//��ʱ����ȫ��vec[0] ��(buffer->data)��
		m_writePos += result;//�ƶ�дָ��
	}
	else {//��ʱvec[1]���в������ݣ�Ӧ����չbuffer->data��������ȥ ע���ʱdataд����дָ��Ӧ��ָ��ĩβ
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
	//�ж�buffer�Ƿ�������
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
