#pragma once
#include <Windows.h>
#include <string>
#include<iostream>

using namespace std;

class occupancy
{
private:

public:

	// 内存 使用率
	DWORD getWin_MemUsage() {
		MEMORYSTATUS ms;
		::GlobalMemoryStatus(&ms);
		return ms.dwMemoryLoad;
	}
	// 时间转换
	static __int64 file_time_2_utc(const FILETIME* ftime)
	{
		LARGE_INTEGER li;

		li.LowPart = ftime->dwLowDateTime;
		li.HighPart = ftime->dwHighDateTime;
		return li.QuadPart;
	}

	// 获得CPU的核数
	static int get_processor_number()
	{
		SYSTEM_INFO info;
		GetSystemInfo(&info);
		return (int)info.dwNumberOfProcessors;
	}

	// 获取进程CPU占用
	int get_cpu_usage(int pid)
	{
		//cpu数量
		static int processor_count_ = -1;
		//上一次的时间
		static __int64 last_time_ = 0;
		static __int64 last_system_time_ = 0;

		FILETIME now;
		FILETIME creation_time;
		FILETIME exit_time;
		FILETIME kernel_time;
		FILETIME user_time;
		__int64 system_time;
		__int64 time;
		__int64 system_time_delta;
		__int64 time_delta;

		int cpu = -1;

		if (processor_count_ == -1)
		{
			processor_count_ = get_processor_number();
		}

		GetSystemTimeAsFileTime(&now);

		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
		if (!GetProcessTimes(hProcess, &creation_time, &exit_time, &kernel_time, &user_time))
		{
			return -1;
		}
		system_time = (file_time_2_utc(&kernel_time) + file_time_2_utc(&user_time)) / processor_count_;
		time = file_time_2_utc(&now);

		if ((last_system_time_ == 0) || (last_time_ == 0))
		{
			last_system_time_ = system_time;
			last_time_ = time;
			return -1;
		}

		system_time_delta = system_time - last_system_time_;
		time_delta = time - last_time_;

		if (time_delta == 0)
			return -1;

		cpu = (int)((system_time_delta * 100 + time_delta / 2) / time_delta);
		last_system_time_ = system_time;
		last_time_ = time;
		return cpu;
	}

};