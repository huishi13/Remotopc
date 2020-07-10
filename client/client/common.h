#pragma once
//定义一些公共的结构体等

//定义一些宏 ，表示包的具体处理类型
//cmd数据类型
#define PACKET_REQ_CMD 10001 //REQ request CMD  表示cmd类型请求数据 客户端------>服务端
#define PACKET_RLY_CMD 10002 //RLY reply CMD    表示cmd类型回复数据 服务端------>客户端
//键盘数据类型
#define PACKET_REQ_KEYBOARD 20001 //REQ request KEYBOARD 表示键盘类型请求数据 客户端------>服务端
#define PACKET_RLY_KEYBOARD 20002 //RLY reply KEYBOARD   表示键盘类型回复数据 服务端------>客户端
//对齐 按一字节对齐
#pragma pack(push) //表示原有的结构体对齐值大小压栈
#pragma pack(1)
struct MyPacket{
	unsigned int type; //表示包类型
	unsigned int length; //表示长度
	char data[1]; //定义一个一字节的数组， 柔性数组
};
#pragma pack(pop)//表示原有的结构体对齐值大小出栈
