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
//���ͻ��˷��������ݽ���Ϊ����ṹ


struct User {
	SOCKADDR_IN clientAddr;
	//��ǰ�û����׽��ֵ�ַ

	char id[_MAX_STRING_LENTH];
	//(����������)��ǰ�û���id

	User * nextMember;
	//(����������)��һ���û���ID

	Channel * currentChannel;
	//(����������)������ID
	//ÿ���ͻ��������ӳɹ�֮�󣬶��ᱻ����Ϊ����һ���ṹ
};
struct Data {
	int command;
	//�������������
	User * guid;
	//Ψһ��ʶ����ÿ���ͻ������Լ���Ψһ��ʶ������һ���ڷ������ָ��User��ָ��
	int p1;
	//�ڷ���ʱ��1Ϊ�ɹ���0Ϊ����
	int p2;
	//�ڶ�����������һЩ������Ϣ
	char message[1400];//Ƶ����+ID or ID+������Ϣ
};


struct Channel {
	char name[_MAX_STRING_LENTH];
	int channelNum;
	User * firstMember;
};

void help_message();
#endif //CHATROOM_CHATROOM_H