#ifndef _RESTFUL_SERVER_H_
#define _RESTFUL_SERVER_H_
#define HAVE_STRUCT_TIMESPEC


/*
使用

void CMeetingDlg::StartHttpCallback(request_param &rp)
{
       
}
handler hd1 = std::bind(&CMeetingDlg::StartHttpCallback, this, std::placeholders::_1);
_httpserver.RegisterHandler("/start", hd1);


Service_HTTP _httpserver;

route_request re1;
re1._param.insertParam("param1");
re1._param.insertParam("param2");
re1._param.insertParam("param3");
re1._param.insertParam("param4");

//handler2 func1 = std::bind(&CMeetingDlg::StartHttpCallback, this, std::placeholders::_1);
re1.func = [](request_param &rp) // func1;
{
string p1 = rp.getParam("param1");
string p2 = rp.getParam("param2");
string p3 = rp.getParam("param3");
if (p1.empty() || p2.empty())
return;
char url[256];
//if (ret != 0)
rp.retString = "{\"ret\":\"-1\"}";
return;
};
route_request re2;
re2._param.insertParam("param1");
re2._param.insertParam("param2");
re2.func = [](request_param & rp)
{
const char * m = rp.getParam("param1");
const char * v = rp.getParam("param2");
if (v != NULL)
{
int nv = atoi(v);
//SetSystemVolume(nv);
}
if (m != NULL)
{
int nm = atoi(m);
//SetMicrophoneValue(nm);
}
rp.retString = "{\"ret\":0}";
};

_httpserver.RegisterHandler("/test1", re1);
_httpserver.RegisterHandler("/test2", re2);
*/

#include "../HttpServer/mongoose.h"
#include "TThreadRunable.h"
#include <string>
#include <functional>
#include <map>
using namespace std;

typedef struct request_param
{
	//返回的
	string retString;
	map<string, string> params;
	void insertParam(string key)
	{
		params[key] = "null";
	}
	void setParam(string key, string value)
	{
		params[key] = value;
	}
	const char* getParam(string key)
	{
		if (params.find(key) != params.end())
		{
			return params[key].c_str();
		}
		return NULL;
	}
	int getSize()
	{
		return (int)params.size();
	}
	map<string, string>::iterator iter;
	const char * GetFirstParam()
	{
		iter = params.begin();
		if (iter == params.end())
			return NULL;
		return iter->first.c_str();
	}
	void SetParamValue(const char *value)
	{
		if (iter != params.end())
			iter->second = value;
	}

	const char * GetNextParam()
	{
		iter++;
		if (iter == params.end())
		{
			return NULL;
		}
		else
		{
			return iter->first.c_str();
		}
	}
}request_param;

using handler2 = std::function<void(request_param &rp)>;
using ws_MessageCallback = std::function<void(int num)>;
//typedef void(*ws_MessageCallback)(int num);

typedef struct route_request
{
	request_param _param;
	handler2 func;
}route_request;


class Service_HTTP :public TThreadRunable
{
	char * _jpg = NULL;
protected:
	using handler_map = std::map<std::string, route_request>;
	handler_map _handlers;
	//webscoket 1分钟没有连接，代表与浏览器断开
public:
	uint32_t _ws_timestamp = 0;
	//界面调用
	ws_MessageCallback _cb_2interface;

	handler_map &GetHandleMap()
	{
		return _handlers;
	}
	~Service_HTTP()
	{
		if (_jpg != NULL)
			delete[]_jpg;
	}
	char *s_http_port = "9090";
	struct mg_serve_http_opts _s_http_server_opts;
	struct mg_mgr _mgr;
	struct mg_connection * _nc = NULL;
	bool RegisterHandler(ws_MessageCallback wshandle)
	{
		_cb_2interface = wshandle;
		return true;
	}
	bool RegisterHandler(std::string uri, route_request f) {
		auto it = _handlers.find(uri);
		if (_handlers.end() != it)
			return false;

		return _handlers.emplace(uri, f).second;
	}


	void UnRegisterHandler(std::string uri) {
		auto it = _handlers.find(uri);
		if (_handlers.end() != it)
			_handlers.erase(it);
	}

public:

	//static void handle_api(string uri, struct mg_connection *nc, struct http_message *hm);
	static void handle_api2(string uri, struct mg_connection *nc, struct http_message *hm);

	static void ev_handler(struct mg_connection *nc, int ev, void *ev_data);
	static int is_websocket(const struct mg_connection *nc) {
		return nc->flags & MG_F_IS_WEBSOCKET;
	}

	
//广播该视频数据
	bool _decide_encodejpg = false;
	void WebSocket_Broadcast(void * s2);
	//void WebSocket_Broadcast1(void * pUser, uint8_t * data, int len);
public:
	
	void Stop();
	void Run();
	
	//DGIWin32Plus plus;
};
#endif
