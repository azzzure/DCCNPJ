#pragma once

//
// Created by ��Ȼ on 2019/6/1.
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

#define _QUIT 1 //ͨ��
#define _OPENCHANNEL  2
#define _LIST_CHANNEL 3
#define _JOIN_CHANNEL 4
#define _LIST_USER 5
#define _PRIVATE_MSG 6
#define _LEAVE 7
#define _MESSAGE 8
#define _FIRST_CONNECT 9

#define _MY_PORT  56666
using  namespace std;

//���ͻ��˷��������ݽ���Ϊ����ṹ

struct User {
	SOCKADDR_IN clientAddr;
	//��ǰ�û����׽��ֵ�ַ
	char id[128];
	//(����������)��ǰ�û���id
	User * nextMember;
	//(����������)��һ���û���ID
	int currentChannel;
	//(����������)������ID
	//ÿ���ͻ��������ӳɹ�֮�󣬶��ᱻ����Ϊ����һ���ṹ
};

struct Data {
	int command;
	//�������������
	User * guid;
	//Ψһ��ʶ����ÿ���ͻ������Լ���Ψһ��ʶ������һ���ڷ������ָ��User��ָ��
	int p1;
	//��һ���������������ڴ���guid��ָ��channelname��λ�õȵȣ�ȡ���ھ�������
	int p2;
	//�ڶ���������ͬ��
	char message[1400];//���������ı����û����ȵ��ַ����ĵط���
};





void help_message();

void Exit();
int command_parse(string command);
int data_parse(Data data);
void init(SOCKET clientSocket, SOCKADDR_IN addr);
DWORD WINAPI Fun(LPVOID lpParamter);
#endif //CHATROOM_CHARROOM_H
