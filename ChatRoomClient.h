#pragma once

//
// Created by 王然 on 2019/6/1.
//

#ifndef CHATROOM_CHATROOMCLIENT_H
#define CHATROOM_CHATROOMCLIENT_H

#include "ChatRoom.h"
#define _INIT 20

void Exit();
int command_parse(string &command);
int data_parse(Data data);
void init(SOCKET clientSocket, SOCKADDR_IN addr);
void reinit(SOCKET clientSocket, SOCKADDR_IN serverAddr);
DWORD WINAPI Fun(LPVOID lpParamter);
#endif //CHATROOM_CHATROOMCLIENT_H
