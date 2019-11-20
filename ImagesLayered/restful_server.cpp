/*

 */
#include "stdafx.h"
#include "restful_server.h"
#include "../HttpServer/mongoose.h"


static struct mg_serve_http_opts s_http_server_opts;


void Service_HTTP::WebSocket_Broadcast(void *s)
{
	//return ;
	//_decide_encodejpg = !_decide_encodejpg;
	//if (_decide_encodejpg == true)
	//{
	//	
	//	videosend2 *s2 = (videosend2 *)s;
	//	int jpglen = 1920*1080; 
	//	if (_jpg == NULL)
	//	{
	//		_jpg = new char[jpglen];
	//	}

	//	int retlen = plus.EncodeRGB2Jpeg((char*)s2->data, s2->w, s2->h, (s2->w) * 3, _jpg, jpglen);
	//	if(retlen == -1)
	//		return;
		struct mg_connection *c;
		int retlen = 0;
		if (_nc != NULL) {
			for (c = mg_next(_nc->mgr, NULL); c != NULL; c = mg_next(_nc->mgr, c)) {
				if (c != _nc)  /* Don't send to the sender. */
					mg_send_websocket_frame(c, WEBSOCKET_OP_BINARY, _jpg, retlen);
			}
		}
	/*	if (_jpg != NULL)
			delete[]_jpg;*/
	
}

//void Service_HTTP::WebSocket_Broadcast1(void * pUser, uint8_t * data, int len)
//{
//	struct mg_connection *c;
//	if (pUser == NULL)
//		return;
//
//	if (_nc != NULL) {
//		for (c = mg_next(_nc->mgr, NULL); c != NULL; c = mg_next(_nc->mgr, c)) {
//			if (c == pUser)
//				mg_send_websocket_frame(c, WEBSOCKET_OP_TEXT, data, len);
//		}
//	}
//
//}


void Service_HTTP::handle_api2(string uri, struct mg_connection *nc, struct http_message *hm)
{
	Service_HTTP *sh = (Service_HTTP*)nc->user_data;
	if (sh == NULL)
		return;
	handler_map& hmap = sh->GetHandleMap();
	handler_map::iterator iter;
	iter = hmap.find(uri);
	if (iter == hmap.end())
	{
		mg_serve_http(nc, hm, s_http_server_opts); /* Serve static content */
		return;
	}
	route_request & rr = iter->second;
	const char * param = rr._param.GetFirstParam();
	while (param != NULL)
	{
		char buffer[255];
		mg_get_http_var(&hm->query_string, param, buffer, sizeof(buffer));
		rr._param.SetParamValue(&buffer[0]);
		//rr._param.setParam(string(param), string(buffer));
		param = rr._param.GetNextParam();
	}
	

	/* Send headers */
	//mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n");
	mg_send_response_line(nc, 200, "Access-Control-Allow-Origin: *");
	//允许哪些url可以跨域请求到本域
	//mg_printf(nc, "Access-Control-Allow-Origin:*\r\n");
	//允许的请求方法，一般是GET,POST,PUT,DELETE,OPTIONS
	mg_printf(nc, "Access-Control-Allow-Methods:POST,OPTIONS,GET\r\n");
	//允许哪些请求头可以跨域
	mg_printf(nc, "Access-Control-Allow-Headers:x-requested-with,content-type\r\n");
	if (rr.func != nullptr)
	{
		rr.func(rr._param);
	}

	string	reply = "{\"ret\":0}";
	if(!rr._param.retString.empty())
		reply = rr._param.retString;
	mg_printf(nc, "Content-Length:%u\r\n\r\n%s\r\n", (uint32_t)reply.size(), reply.c_str());
	//mg_printf(nc, "HTTP/1.1 200 OK\r\niConnection: close\r\nContent-Type: text/html\r\nContent-Length: %u\r\n\r\n%s\r\n",
	nc->flags |= MG_F_SEND_AND_CLOSE;
	//mg_send(nc, "", 0);
}
#if 0
void Service_HTTP::handle_api(string uri, struct mg_connection *nc, struct http_message *hm)
{
	Service_HTTP *sh = (Service_HTTP*)nc->user_data;
	if (sh == NULL)
		return;

	//const char * param = sh->_param.GetFirstParam();
	if (uri.compare("/start") == 0)
	{
		char host[64];
		char uid[64];
		char message[64];
		mg_get_http_var(&hm->query_string, "serverhost", host, sizeof(host));
		mg_get_http_var(&hm->query_string, "uid", uid, sizeof(uid));
		mg_get_http_var(&hm->query_string, "message", message, sizeof(message));
		/* Send headers */
		//mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n");
		mg_send_response_line(nc, 200, "Access-Control-Allow-Origin: *");
		//允许哪些url可以跨域请求到本域
		mg_printf(nc, "Access-Control-Allow-Origin:*\r\n");
		//允许的请求方法，一般是GET,POST,PUT,DELETE,OPTIONS
		mg_printf(nc, "Access-Control-Allow-Methods:POST\r\n");
		//允许哪些请求头可以跨域
		mg_printf(nc, "Access-Control-Allow-Headers:x-requested-with,content-type\r\n");
		string reply = "{\"result\":0}";
		mg_printf(nc, "Content-Length:%u\r\n\r\n%s\r\n", (uint32_t)reply.size(), reply.c_str());
		//mg_printf(nc, "HTTP/1.1 200 OK\r\niConnection: close\r\nContent-Type: text/html\r\nContent-Length: %u\r\n\r\n%s\r\n",
		nc->flags |= MG_F_SEND_AND_CLOSE;
		mg_send(nc, "", 0);
		//mg_printf_http_chunk(nc, "{ \"result\": %ld }", 0);
		//mg_send_http_chunk(nc, "", 0); /* Send empty chunk, the end of response */

		
		re_param request;
		request.uri = uri;
		request.pUser = NULL;
		request.host = host;
		request.uid = uid;
		request.message = message;
		if (sh->handlestart != nullptr)
			sh->handlestart(request);
		//it->second(request);
		OutputDebugString(L"start video to");
		//callback(__void, host, uid, message);
		
	}
	else if (uri.compare("/stop") == 0) //停止
	{
		char message[64];

		mg_get_http_var(&hm->query_string, "message", message, sizeof(message));
		/* Send headers */
		//mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n");
		mg_send_response_line(nc, 200, "Access-Control-Allow-Origin: *");
		//允许哪些url可以跨域请求到本域
		mg_printf(nc, "Access-Control-Allow-Origin:*\r\n");
		//允许的请求方法，一般是GET,POST,PUT,DELETE,OPTIONS
		mg_printf(nc, "Access-Control-Allow-Methods:POST\r\n");
		//允许哪些请求头可以跨域
		mg_printf(nc, "Access-Control-Allow-Headers:x-requested-with,content-type\r\n");
		string reply = "{\"result\":0}";
		mg_printf(nc, "Content-Length:%u\r\n\r\n%s\r\n", (uint32_t)reply.size(), reply.c_str());
		//mg_printf(nc, "HTTP/1.1 200 OK\r\niConnection: close\r\nContent-Type: text/html\r\nContent-Length: %u\r\n\r\n%s\r\n",
		nc->flags |= MG_F_SEND_AND_CLOSE;

		mg_send(nc, "", 0);
		//mg_printf(nc, "{ \"result\": %ld }", 0);
		//mg_send(nc, "", 0);
		//mg_printf_http_chunk(nc, "{ \"result\": %ld }", 0);
		//mg_send_http_chunk(nc, "", 0); /* Send empty chunk, the end of response */

		
			//auto it = sh->GetHandleMap().find(uri);
			//if (sh->GetHandleMap().end() == it)
			//	return;
			re_param request;
			request.uri = uri;
			request.pUser = NULL;
			request.message = message;
			if(sh->handlestop!=nullptr)
				sh->handlestop(request);
			OutputDebugString(L"stop video");
	}
	//else if(uri.compare("getmicrophone"))
	//获取
}
#endif



void Service_HTTP::ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
	struct http_message *hm = (struct http_message *) ev_data;
	//传送文件地址和转化的目的地址
	//   /api/t2pdf
	switch (ev) {

	case MG_EV_WEBSOCKET_HANDSHAKE_DONE:
		//OutputDebugString(L"connection is here");
		/* New websocket connection. Tell everybody. */
		//broadcast(nc, mg_mk_str("++ joined"));
		break;

		//websocket frame,接收到浏览器信息
	case MG_EV_WEBSOCKET_FRAME: {
		struct websocket_message *wm = (struct websocket_message *) ev_data;
		/* New websocket message. Tell everybody. */
		struct mg_str d = { (char *)wm->data, wm->size };
		Service_HTTP * sh = (Service_HTTP *)nc->user_data;
		//if (sh->_ws_timestamp == 0)
		//{
			sh->_ws_timestamp = GetTickCount();
			//OutMessage("the timestamp is:%d", sh->_ws_timestamp);
		//}
		//broadcast(nc, d);
		break;
	}

	case MG_EV_HTTP_REQUEST:
	{
		string uri = string(hm->uri.p, hm->uri.len);
		handle_api2(uri, nc, hm);
		//if (mg_vcmp(&hm->uri, "/start") == 0) {
		//	handle_api2(uri, nc, hm);
		//}
		//else if (mg_vcmp(&hm->uri, "/stop") == 0) {
		//	handle_api2(uri, nc, hm);
		//}
		//else
		//{
		//	mg_serve_http(nc, hm, s_http_server_opts); /* Serve static content */
		//}
		break;
	}
	case MG_EV_CLOSE:
	{
		if (is_websocket(nc))
			OutputDebugString(L"websocket out");
		break;
	}
	default:
		break;
	}
}


void Service_HTTP::Stop()
{
	TThreadRunable::Stop();
	Join();
}
#if 0
int httpserver(const char *port, void * pUser, MessageCallback callback) {
	struct mg_mgr mgr;
	struct mg_connection *nc;
	struct mg_bind_opts bind_opts;
	const char *err_str;
#ifdef MG_ENABLE_SSL
	const char *ssl_cert = NULL;
#endif
	///g_pagecallback = callback;
	mg_mgr_init(&mgr, NULL);
	mgr.user_data = callback;
	//s_http_server_opts.document_root = "./";

	/* Set HTTP server options */
	memset(&bind_opts, 0, sizeof(bind_opts));
	bind_opts.error_string = &err_str;

	nc = mg_bind_opt(&mgr, port, ev_handler, bind_opts);
	if (nc == NULL) {
		fprintf(stderr, "Error starting server on port %s: %s\n", port,
			*bind_opts.error_string);
		return -1;
	}
	__void = pUser;
	//ppparam * xparam = malloc(sizeof(ppparam));
	//xparam->pUser = pUser;
	//xparam->mcb = callback;
	nc->user_data = callback;

	mg_set_protocol_http_websocket(nc);
	s_http_server_opts.enable_directory_listing = "no";

	//printf("Starting RESTful server on port %s\n", port);
		//s_http_server_opts.document_root);
	for (;;) {
		if (__exit == 1)
			break;
		mg_mgr_poll(&mgr, 50);
	}
	mg_mgr_free(&mgr);

	return 0;
}
#endif


void Service_HTTP::Run()
{
	mg_mgr_init(&_mgr, NULL);
	_nc = mg_bind(&_mgr, s_http_port, ev_handler);
	_nc->user_data = this;
	mg_set_protocol_http_websocket(_nc);
	s_http_server_opts.document_root = "./public";  // Serve current directory
	s_http_server_opts.enable_directory_listing = "yes";
	//printf("Started on port %s\n", s_http_port);
	_ws_timestamp = GetTickCount();
	while (!IsStop()) {
		mg_mgr_poll(&_mgr, 50);
		uint32_t now = GetTickCount();
		if (now - _ws_timestamp > 1000 * 60)
		{
			if (_cb_2interface != NULL)
			{
				//501心跳停止
				//_cb_2interface(501);
				_ws_timestamp = now;
			}
			//给界面发送信息停止推流
		}
	}
	mg_mgr_free(&_mgr);

	 
}