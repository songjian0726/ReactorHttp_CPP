#pragma once
#include<functional>//包装器
// #include<stdbool.h>  //cpp本身支持bool

using namespace std;

//定义文件描述符的读写事件
enum class FDEvent { //强类型枚举
	TimeOut = 0x01,
	ReadEvent = 0x02,
	WriteEvent = 0x04
};


//回调函数的函数指针
//typedef int(*handleFunc)(void* arg);
//using handleFunc = int(*)(void*);  //using 定义函数指针


//可调用对象包装器打包： 函数指针，可调用对象(可以像函数一样使用, 仿函数)
//得到可调用对象的地址 

class Channel {
public:
	using handleFunc = function<int(void*)>;
	Channel(int fd, FDEvent events, handleFunc readFunc, handleFunc writeFunc, handleFunc destroyFunc, void* arg);
	//回调函数
	handleFunc readCallback;
	handleFunc writeCallback;
	handleFunc destroyCallback;
	//修改fd的写事件(检测或不检测)
	void writeEventEnable(bool flag);
	//判断是否需要检测文件描述符的写事件
	bool isWriteEventEnable();
	//私有成员接口
	inline int getEvent() {
		return m_events;
	}
	inline int getSocket() {
		return m_fd;
	}
	inline const void* getArg() {  //返回const只读地址
		return m_arg;
	}
private:
	//文件描述符
	int m_fd;
	//对应的事件
	int m_events;
	//回调函数参数
	void* m_arg;
};

