#pragma once

#include <string>
#include<opencv2/opencv.hpp> 
#include <opencv2/core.hpp> //图像数据结构的核心头文件
#include <opencv2/highgui.hpp> //图形接口函数的highgui头文件
#include "restful_server.h"
#include "TThreadRunable.h"
#include "ws_client.h"
#include "occupancy.h"
#include "jsonutil.h"
using namespace std;

//分割图片的参数
typedef struct layerParams
{
	string originFilePath;//原图片的磁盘路径
	string fileName;//图片名称
	string reFilePath;//新生成图片的磁盘路径
	int _rows; //配置文件中规定图片高度
	int _cols;//配置文件中规定图片宽度
	int lay_01_rows;//用来生成第一层图高度
	int lay_01_cols;//用来生成第一层图宽度
	int rows;//原图的高度
	int cols;//原图的宽度

}layerParams;

typedef struct occpancy_params {
	int memory;
	int cpu;
	int pid;
}occpancy_params;

class wrapper :public TThreadRunable
{
private:
	mutex _lock;
	queue<layerParams *> _queue_params;
public:
	//分割第一层图片
	void func_ImageLayer_First(layerParams &layer_params);
	//分割第二层图片
	void func_ImageLayer_Second(layerParams &layer_params);
	//分割图片的总入口
	int func_ImageLayer_all(string origin_dirpath, string file_name, string set_dirpath, int _cols, int _rows);
	//往队列插入任务
	void Insert(layerParams *_layer_params) {
		_lock.lock();
		_queue_params.push(_layer_params);
		_lock.unlock();
		Notify();
	}
	void server_status(occpancy_params &_occpancy_params) {

		ws_class::InitSock();
		ws_class ws;
		//ws://iotserver.vip/ws
		ws.Start("ws://192.168.111.12/ws", [](void *pUser, const char * mes, int len)
		{
			cout << " reveive:::" << mes << endl;
			return 0;
		});

		string json = "{";
		json += yy("cpu", _occpancy_params.cpu);
		json += ",";
		json += yy("memory", _occpancy_params.memory);
		json += ",";
		json += yy("pid", _occpancy_params.pid);
		json += "}";
		cout << "json::" << json << endl;
		//json.c_str()
		ws.send((char *)json.c_str());
		ws.Join();
		return;
	}
	void Run() {
		while (true)
		{
			if (IsStop()) {
				break;
			}
			WaitForSignal();
			layerParams * layer_params = NULL;
			_lock.lock();
			if (_queue_params.size() >0) {
				layer_params = _queue_params.front();
				_queue_params.pop();
			}
			_lock.unlock();	
			if (layer_params != NULL)
			{
				func_ImageLayer_all(layer_params->originFilePath, layer_params->fileName, layer_params->reFilePath, layer_params->cols, layer_params->rows);
				delete layer_params;
			}
			//内存使用率
			occupancy _occupancy;
			//Double Word  4个字节 
			DWORD memory = _occupancy.getWin_MemUsage(); 
			DWORD ProcessID = GetCurrentProcessId();
			int cpu = _occupancy.get_cpu_usage(ProcessID);
			occpancy_params _occpancy_params;
			_occpancy_params.cpu = cpu;
			_occpancy_params.memory = memory;
			_occpancy_params.pid = ProcessID;
			server_status(_occpancy_params);
		}
	}
	void Stop()
	{
		TThreadRunable::Stop();
		Notify();
		Join();
	}
};
class http_receive
{
private:
#define THREAD_NUM 4
	//线程池概念
	wrapper _run_wrapper[THREAD_NUM];
	int64_t _lastThread = 0;
public:

	void Init(Service_HTTP *_http_server) {
		route_request re1;
		re1._param.insertParam("originPath");//原图片的磁盘路径
		re1._param.insertParam("fileName");//原图的名称
		re1._param.insertParam("reFilePath");//生辰个图片的磁盘路径
		re1._param.insertParam("cols");//宽度
		re1._param.insertParam("rows");//高度
		re1.func = [this](request_param &rp) // func1;
		{
			string _originPath = rp.getParam("originPath");
			string _fileName = rp.getParam("fileName");
			string _reFilePath = rp.getParam("reFilePath");
			string cols = rp.getParam("cols");
			string rows = rp.getParam("rows");
			if (_originPath.empty() || _fileName.empty() || _reFilePath.empty() || cols.empty() || rows.empty()) {
				cout << "please argv" << endl;
				return;
			}
			int _cols = 0;
			int _rows = 0;
			try
			{
				_cols = atoi(rp.getParam("cols"));
				_rows = atoi(rp.getParam("rows"));
			}
			catch (...)
			{
				_cols = 1280;
				_rows = 960;
				cout << " wrong ....default..." << endl;
			}
			rp.retString = "{\"ret\":\"0\"}";
			layerParams * _player_params = new layerParams();
			_player_params->originFilePath = _originPath;
			_player_params->fileName = _fileName;
			_player_params->reFilePath = _reFilePath;
			_player_params->cols = _cols;
			_player_params->rows = _rows;
			this->InsertParam(_player_params);//插入队列
		};
		_http_server->RegisterHandler("/receive", re1);
		
		//server_status();
	}

	void InsertParam(layerParams *_layer_params)
	{
		 _lastThread ++ ;
		 if (_lastThread == 4) {
			 _lastThread = 0;
		 }
		_run_wrapper[_lastThread].Insert(_layer_params);
	}

	void StartAllThread() {
		
		for (int i = 0; i < THREAD_NUM; i++) {
			_run_wrapper[i].Start();
		}
	}

	void StopAllThread() {
		
		for (int j = 0; j < THREAD_NUM; j++) {
			_run_wrapper[j].Stop();
		}
	}
};

