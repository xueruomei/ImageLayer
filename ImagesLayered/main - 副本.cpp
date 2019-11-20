// ImagesLayered.cpp : �������̨Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include "ImagesLayered.h"
#include <iostream>
#include "restful_server.h"

int main(int argc, char* argv[])
{
	if (argc <3) {
		cout << "please input the file" << endl;
		return -1;
	}
	Service_HTTP _httpserver;
	route_request re1;
	re1._param.insertParam("originPath");//ԭͼƬ�Ĵ���·��
	re1._param.insertParam("fileName");//ԭͼ������
	re1._param.insertParam("reFilePath");//������ͼƬ�Ĵ���·��
	re1._param.insertParam("cols");//���
	re1._param.insertParam("rows");//�߶�

	re1.func = [](request_param &rp) // func1;
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
		char url[256];
		//cout << _originPath << " " << _fileName << " " << _reFilePath << " " << _cols << " " << _rows << endl;
		wrapper ImageLayer;
		ImageLayer.func_ImageLayer_all(_originPath, _fileName, _reFilePath, _cols, _rows);
		rp.retString = "{\"ret\":\"0\"}";
		return;
	};
	_httpserver.RegisterHandler("/receive", re1);

	_httpserver.Start();
	_httpserver.Join();
	return 0;
}