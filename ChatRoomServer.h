
//
// Created by Õı»ª on 2019/6/1.
//

#ifndef CHATROOM_CHATROOMSERVER_H
#define CHATROOM_CHATROOMSERVER_H
#include "ChatRoom.h"


struct Memember {
	
};



void Exit();
int command_parse(string command);
int data_parse(Data *data);
Data* make_message(int command,int p1 );
DWORD WINAPI Fun(LPVOID lpParamter);
User * add_to_user( SOCKADDR_IN addrClient);
void add_to_channel(string channelName);

#endif //CHATROOM_CHATROOMSERVER_H
