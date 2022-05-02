#include "LogThread.h"
#include "../../Network.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <concurrent_priority_queue.h>
#include <fstream>
#include <windows.h>

using namespace std;


void LogThread()
{
	if ((_waccess(L"Log", 0) == -1))
		CreateDirectory(L"Log", NULL);

	tm locTime;
	time_t curTime;
	string fileName;
	curTime = time(NULL);
	localtime_s(&locTime ,&curTime);
	char y[6],m[3], d[3], h[3], minTime[3], sec[3];
	ZeroMemory(y, sizeof(y));
	ZeroMemory(m, sizeof(m));
	ZeroMemory(d, sizeof(d));
	ZeroMemory(h, sizeof(h));
	ZeroMemory(minTime, sizeof(minTime));
	ZeroMemory(sec, sizeof(sec));
	_itoa_s(locTime.tm_year + 1900, y, 10);
	_itoa_s(locTime.tm_mon + 1, m, 10);
	_itoa_s(locTime.tm_mday , d, 10);
	_itoa_s(locTime.tm_hour , h, 10);
	_itoa_s(locTime.tm_min , minTime, 10);
	_itoa_s(locTime.tm_sec , sec, 10);
	fileName += "Log/[LogFile]";
	fileName += y;	
	fileName += "-";
	fileName += m;
	fileName += "-";
	fileName += d;
	fileName += "-";
	fileName += h;
	fileName += "-";
	fileName += minTime;
	fileName += "-";
	fileName += sec;
	fileName += ".txt";
	ofstream logFile;
	logFile.open(fileName.c_str());
	if (logFile.fail())
	{
		cout << "file didn't opened!" << endl;
	}
	logFile.close();

	while (true)
	{

		logFile.open(fileName.c_str(),ios::app);
		if (logFile.fail())
		{
			cout << "file didn't opened!" << endl;
		}
		while (true)
		{
			Log LogTxt;
			if (!logger.try_pop(LogTxt))
			{
				logFile.close();
				break;
			}
			logFile << LogTxt.logtxt.c_str() << endl;
	
		}
		this_thread::sleep_for(chrono::duration_cast<chrono::milliseconds>(1000ms));
	}
}
