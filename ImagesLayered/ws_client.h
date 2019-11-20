#pragma once
// Compile with:
// g++ -std=gnu++0x example-client-cpp11.cpp -o example-client-cpp11

#include "easywsclient.hpp"
#ifdef _WIN32
#pragma comment( lib, "ws2_32" )
#include <WinSock2.h>
#endif
#include <assert.h>
#include <stdio.h>
#include <string>
#include <memory>
#include <iostream>
#include "TThreadRunable.h"

using easywsclient::WebSocket;
typedef int(*callback_message_recv)(void * pUser,
	const char * message, int len);

class ws_class :public TThreadRunable
{
	WebSocket *ws = NULL;// (WebSocket::from_url("ws://localhost:8126/foo"));
public:
	static int InitSock()
	{
#ifdef _WIN32
		INT rc;
		WSADATA wsaData;

		rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (rc) {
			printf("WSAStartup Failed.\n");
			return -1;
		}
#endif

	}
	static void UnInitSock()
	{
		WSACleanup();
	}
	callback_message_recv _recv = NULL;
public:
	int Start(const char * url, callback_message_recv recv)
	{
		ws = WebSocket::from_url(url);
		if (ws == NULL)
			return -1;
		_recv = recv;
		TThreadRunable::Start();
		return 0;
	}
	int send(char * str)
	{
		if (str != NULL)
		{
			ws->send(str);
			//ws->poll();
		}
	}
	void Run()
	{
		while (ws->getReadyState() != WebSocket::CLOSED) {
			//WebSocket::pointer wsp = &*ws; // <-- because a unique_ptr cannot be copied into a lambda
			if (IsStop())
				break;
			ws->poll();
			ws->dispatch([this](const std::string & message) {
				if (_recv != NULL)
					_recv(NULL, message.c_str(), strlen(message.c_str()));
			});
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
		std::cout << "server exit" << endl;
	}
};

//int recv1(void *pUser, const char * mes, int len)
//{
//	printf(">>> %s\n", mes);
//	return 0;
//}

//int main()
//{
//	ws_class::InitSock();
//	ws_class ws;
//	ws.Start("ws://localhost:8126/foo",recv1);
//	for (int i = 0; i < 10; i++)
//	{
//		ws.send("qianbo");
//		Sleep(50);
//	}
//	ws.Join();
//     
//    // N.B. - unique_ptr will free the WebSocket instance upon return:
//    return 0;
//}
