
//
// Created by Õı»ª on 2019/6/1.
//

#ifndef CHATROOM_CHATROOMSERVER_H
#define CHATROOM_CHATROOMSERVER_H
#include "ChatRoom.h"


struct Memember {
	
};

#define _ENTERCHANNEL 11
#define _KICK 12
#define _CLOSECHANNEL 13

void Exit();
int command_parse(string command);
int data_parse(Data *data);
Data* make_message(int command,int p1,void * p2 );
DWORD WINAPI listen_to_client(LPVOID lpParamter);
DWORD WINAPI check_user_alive(LPVOID lpParamter);
User * add_to_user( SOCKADDR_IN addrClient);
void  add_user_to_channel(Channel *_channel, User * _user);
void  delete_user_to_channel(Channel *_channel, User * _user);
void add_to_channel(string channelName);
Channel * find_channel_by_name(const char *);
#endif //CHATROOM_CHATROOMSERVER_H
