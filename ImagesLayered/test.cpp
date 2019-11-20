
#include "stdafx.h"
#include "TThreadRunable.h"
#include <string>
#include <iostream>
#include <windows.h>
using namespace std;

class MyClass :public TThreadRunable
{
public:
	MyClass();
	~MyClass();

	void Run() {
		while (true)
		{
			WaitForSignal();
			cout << "test" << endl;
		}
	}

private:

};

MyClass::MyClass()
{
}

MyClass::~MyClass()
{
}

int main1() {
	
	MyClass test;
	test.Start();
	
	while (true)
	{
		Sleep(1000);
		test.Notify();
	}
	
	return 0;
}
