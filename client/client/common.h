#pragma once
//����һЩ�����Ľṹ���

//����һЩ�� ����ʾ���ľ��崦������
//cmd��������
#define PACKET_REQ_CMD 10001 //REQ request CMD  ��ʾcmd������������ �ͻ���------>�����
#define PACKET_RLY_CMD 10002 //RLY reply CMD    ��ʾcmd���ͻظ����� �����------>�ͻ���
//������������
#define PACKET_REQ_KEYBOARD 20001 //REQ request KEYBOARD ��ʾ���������������� �ͻ���------>�����
#define PACKET_RLY_KEYBOARD 20002 //RLY reply KEYBOARD   ��ʾ�������ͻظ����� �����------>�ͻ���
//���� ��һ�ֽڶ���
#pragma pack(push) //��ʾԭ�еĽṹ�����ֵ��Сѹջ
#pragma pack(1)
struct MyPacket{
	unsigned int type; //��ʾ������
	unsigned int length; //��ʾ����
	char data[1]; //����һ��һ�ֽڵ����飬 ��������
};
#pragma pack(pop)//��ʾԭ�еĽṹ�����ֵ��С��ջ
