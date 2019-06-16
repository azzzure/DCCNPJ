#pragma once
#ifndef CHATROOM_CHATROOM_H
#define CHATROOM_CHATROOM_H
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
#define _MAX_STRING_LENTH 31 
using  namespace std;
struct Channel;
//将客户端发来的数据解析为这个结构


struct User {
	SOCKADDR_IN clientAddr;
	//当前用户的套接字地址

	char id[_MAX_STRING_LENTH];
	//(在聊天室中)当前用户的id

	User * nextMember;
	//(在聊天室中)下一个用户的ID

	Channel * currentChannel;
	//(在聊天室中)聊天室ID
	//每个客户端在连接成功之后，都会被储存为这样一个结构
};
struct Data {
	int command;
	//控制命令的类型
	User * guid;
	//唯一标识符。每个客户端有自己的唯一标识符，是一个在服务端里指向User的指针
	int p1;
	//在返回时，1为成功，0为出错
	int p2;
	//第二个参数传递一些其他信息
	char message[1400];//频道名+ID or ID+聊天信息
};


struct Channel {
	char name[_MAX_STRING_LENTH];
	int channelNum;
	User * firstMember;
};

void help_message();
#endif //CHATROOM_CHATROOM_H