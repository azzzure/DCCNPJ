
//
// Created by 王然 on 2019/6/1.
//

#ifndef CHATROOM_CHARROOM_H
#define CHATROOM_CHARROOM_H

#include <iostream>
#include<string>
#include <winsock2.h>
#include <sys/types.h>
#include<vector>
#include<process.h>
#include <windows.h>  

#define _QUIT 1
#define _OPENCHANNEL  2
#define _MY_PORT  56666
#define _LIST_CHANNEL 3
#define _JOIN_CHANNEL 4
#define _LIST_USER 5
#define _PRIVATE_MSG 6
#define _LEAVE 7
#define _MESSAGE 8
#define _FIRST_CONNECT 9
using  namespace std;

//将客户端发来的数据解析为这个结构


struct User {
	SOCKADDR_IN clientAddr;
	//当前用户的套接字地址

	char id[128];
	//(在聊天室中)当前用户的id
	User * nextMember;
	//(在聊天室中)下一个用户的ID
	Channel * currentChannel;
	//(在聊天室中)当前用户所在的聊天室
	//每个客户端在连接成功之后，都会被储存为这样一个结构
};
struct Data {
	int command;
	//控制命令的类型
	User * guid;
	//唯一标识符。每个客户端有自己的唯一标识符，是一个在服务端里指向User的指针
	int p1;
	//第一个参数，可能用于传递guid，指向channelname的位置等等，取决于具体的命令。
	int p2;
	//第二个参数，同上
	char message[1400];//放置聊天文本，用户名等等字符串的地方。
};


struct Channel {
	char name[128];
	int channelNum;
	User * firstMember;
};

void help_message();
void Exit();
int command_parse(string command);
int data_parse(Data *data);
Data* make_message(int command,int p1,void* p2);
DWORD WINAPI Fun(LPVOID lpParamter);
User * add_to_user( SOCKADDR_IN addrClient);
void add_to_channel(string channelName);

#endif //CHATROOM_CHARROOM_H
