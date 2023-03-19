#pragma once
#include"Buffer.h"
#include"HttpResponse.h"
#include<map>
#include<functional>
using namespace std;

//当前的解析状态
enum class PrecessState:char {
	ParseReqLine,
	ParseReqHeaders,
	ParseReqBody,
	ParseReqDone
};

class HttpRequest {
public:
	HttpRequest();

	//重置httpRequest结构体
	void reset();

	//获取处理状态State
	inline PrecessState getState() { return m_curState; };

	inline void setState(PrecessState state) { m_curState = state; };

	//添加请求头
	void addHeader(string key, string value);

	//根据key获得对应的value
	string getHeader(const string key);

	//解析请求行
	bool parseRequestLine(Buffer* readBuf); 

	//解析请求头
	bool parseRequestHeader(Buffer* readBuf);

	//解析http请求  解析是把http请求拆分
	bool parseHttpRequest(Buffer* readBuf, HttpResponse* response, Buffer* sendBuf, int socket);

	//处理http请求 处理是根据request结构体里的数据做出相应反馈
	bool processHttpRequest(HttpResponse* response);

	//解码字符串
	string decodeMsg(string from);


	const string getFileType(const string name);

	static void sendFile(string fileName, Buffer* sendBuf, int cfd);
	static void sendDir(string fileName, Buffer* sendBuf, int cfd);

	inline void setMethod(string method) { m_method = method; };
	inline void setUrl(string url) { m_url = url; };
	inline void setVersion(string version) { m_version = version; };
	
private:
	char* splitRequestLine(const char* start, const char* end, const char* sub, function<void(string)> callback);
	int hexToDec(char c);

private:
	string m_method;
	string m_url;
	string m_version;
	map<string, string> m_reqHeaders;//存储请求头中的键值对
	PrecessState m_curState;//当前状态
};


