// ImagesLayered.cpp : 定义控制台应用程序的入口点。
#include "stdafx.h"
#include "ImagesLayered.h"
#include <iostream>
#include "restful_server.h"

int main(int argc, char* argv[])
{
	Service_HTTP _http_server;
	http_receive _http_receive;

	_http_receive.Init(&_http_server);

	_http_receive.StartAllThread();

	_http_server.Start();

	_http_server.Join();

	_http_receive.StopAllThread();

	return 0;
}