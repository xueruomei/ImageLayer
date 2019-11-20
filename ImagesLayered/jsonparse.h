#pragma once
#include <stdio.h>

#include "cJSON.h"
#include <string>

using namespace std;

typedef struct retReturn
{
	int width;
	int height;
}retReturn;



class jsonparse
{
	cJSON * root = NULL;
	cJSON * item = NULL;//cjson����
public:
	jsonparse::jsonparse()
	{}
	int parse(const char * jsonStr)
	{
		root = cJSON_Parse(jsonStr);
		return root != NULL ? 0 : -1;
	}

	int getType(string & type)
	{
		cJSON * jtype = cJSON_GetObjectItem(root, "type");
		if (jtype == NULL)
			return -1;
		else
			type = jtype->valuestring;
		return 0;
	}
	//�õ�·��path
	int GetRoutePath(string & path)
	{
		//{"type":"push","path":"/live/qianbo"};
		if (root == NULL)
			return -1;
		cJSON * data = cJSON_GetObjectItem(root, "width");
		if (data == NULL)
			return -1;

		cJSON * objpath = cJSON_GetObjectItem(data, "height");
		if (objpath != NULL)
		{
			if (objpath->valuestring != NULL)
			{
				path = objpath->valuestring;
				return 0;
			}
		}
		return -1;
	}

	int GetUserInfo( string &userId, string &userName)
	{
		//#define _GetItem(x) cJSON * x = 
		if (root == NULL)
			return -1;
		//pushorpull = -1;
		//��ȡ�ֶ�ֵ
		//cJSON_GetObjectltem���ص���һ��cJSON�ṹ���������ǿ���ͨ���������ؽṹ��ķ�ʽѡ�񷵻����ͣ�
		cJSON * data = cJSON_GetObjectItem(root, "data");
		if (data == NULL)
			return -1;

		//cJSON * rid = cJSON_GetObjectItem(data, "roomId");
		cJSON * uid = cJSON_GetObjectItem(data, "userId");
		cJSON * una = cJSON_GetObjectItem(data, "userName");
		//if (rid != NULL && rid->valuestring != NULL)
		//{
		//	roomId = rid->valuestring;
		//}
		/*if (uid != NULL && uid->valuestring != NULL)
		{
			userId = uid->valuestring;
		}
		if (una != NULL && una->valuestring != NULL)
		{
			userName = una->valuestring;
		}*/
		
		if (uid != NULL && una != NULL)
		{
			if (uid->valuestring && una->valuestring != NULL)
			{
				//roomId = rid->valuestring;
				userId = uid->valuestring;
				userName = una->valuestring;
				if (userId.empty() || userName.empty())
				{
					return -1;
				}
				//cJSON * pushpull = cJSON_GetObjectItem(data, "type");
				//if (pushpull != NULL)
				//{
				//	pushorpull = pushpull->valueint;
				//}
				return 0;
			}
			return -1;
		}
		return -1;
	}

	jsonparse::~jsonparse()
	{
		if (root != NULL)
			cJSON_Delete(root);
	}

	string json_create(int ret,const char * type, const char *info)
	{
	
		cJSON *json = cJSON_CreateObject();

		cJSON_AddItemToObject(json, "ret", cJSON_CreateNumber(ret));
		cJSON_AddItemToObject(json, "type", cJSON_CreateString(type));
		cJSON_AddItemToObject(json, "info", cJSON_CreateString(info));

		//��json�ṹ��ʽ����������
		string rets = cJSON_Print(json);

		//�ͷ�json�ṹ��ռ�õ��ڴ�
		cJSON_Delete(json);
		return rets;
	}
};