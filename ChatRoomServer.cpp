#include "ChatRoomServer.h"
using namespace std;
vector< Channel*> channel;
vector< User* > user;
int main() {

	//int socket_fd;
	//socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	//struct  sockaddr_in ser_addr;
	//ser_addr.sin_addr
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	int serverSocket = socket(PF_INET, SOCK_DGRAM, 0);//创建套接字
	SOCKADDR_IN addr;//定义地址结构体变量
	addr.sin_family = AF_INET;//协议族
	addr.sin_port = htons(_MY_PORT);//端口号
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	bind(serverSocket, (SOCKADDR*)&addr, sizeof(SOCKADDR));
	int len = sizeof(SOCKADDR);
	string command;
	cout << "服务端" << endl;
	help_message();
	int pid = _getpid();
	cout << "pid =" << pid << endl;

	char temp[122] = "hahaha";
	HANDLE LTC = CreateThread(NULL, 0, listen_to_client, (LPVOID*)&serverSocket, 0, NULL);
	//HANDLE CUA = CreateThread(NULL, 0, check_user_alive, (LPVOID*)&serverSocket, 0, NULL);
	CloseHandle(LTC);
	add_to_channel("df");
	//主线程处理用户的输入

	Channel *currentChannel=NULL;
	string channelName;
	User * tempUser;
	while (1) {
		cin >> command;
		int _command = command_parse(command);
		switch (_command) {
		case _QUIT: {
			cout << "gg" << endl;
			Exit();
		}
		case _OPENCHANNEL: {
			cout << "openchannel:" << endl;
			
			cin >> channelName;
			add_to_channel(channelName);
			cout << channelName << endl;
			break;
		}
		case _ENTERCHANNEL:
			cin >> channelName;
			currentChannel = find_channel_by_name(channelName.data());
			if (currentChannel) {
				cout << "成功加入频道"<<currentChannel->name<<endl;
			}
			else {
				cout << "没有这个频道！"<<endl;
			}
			break;
		case _LIST_USER:
			if (currentChannel) {
				tempUser = currentChannel->firstMember;
				if (tempUser) {
					cout << "频道" << currentChannel->name << "中的用户为：";

					while (currentChannel != NULL) {
						cout << tempUser->id << endl;
						tempUser = tempUser->nextMember;
					}
					
				}
				else {
					cout << "频道" << currentChannel->name << "中还没有任何用户"<<endl;
				}
			}
			else {
				cout << "您不在频道中！" << endl;
			}
			break;
		case _KICK:
			//todo
			break;
		case _LEAVE:
			if (currentChannel) {
				cout << "已退出频道" << currentChannel->name << endl;
				currentChannel = NULL;
			}
			else {
				cout << "您不在频道中！" << endl;
			}
			//todo
			break; 
		case _CLOSECHANNEL:
			//todo
			//给频道内用户发送信息：爷走啦
			break;
		default: {
			help_message();
			break;
		}

		}
	}
	return 0;
}
void help_message() {
	cout << "quit 退出本程序" << endl;
	cout << "openchannel channelName :开通新的聊天室" << endl;
	cout << "channels 列出现有的聊天室" << endl;
	cout << "enterchannel channelName 进入现有的聊天室中" << endl;
	cout << "list 列出当前聊天室的所有用户" << endl;
	cout << "kickout userid 将用户踢出聊天室" << endl;
	cout << "leave 离开聊天室" << endl;
	cout << "closechannel channelname 关闭聊天室" << endl;
	cout << "" << endl;
	cout << "" << endl;

}
int data_parse(Data *data) {
	if (_LIST_CHANNEL <= data->command && _FIRST_CONNECT >= data->command) return data->command;
	else return -1;
}
void Exit() {
	cout << "bye bye" << endl;
	exit(0);
}
int command_parse(string command) {
	if (command == "quit") return _QUIT;
	else if (command == "openchannel"|| command == "op")return _OPENCHANNEL;
	else if (command == "enterchannel"||command=="ec")return _ENTERCHANNEL;
	else if (command == "list")return _LIST_USER;
	else if (command == "kick")return _KICK;
	else if (command == "leave")return _LEAVE;
	else if (command == "closechannel"||command=="cc")return _CLOSECHANNEL;

	return 999;
}
Data * make_message(int command, int p1, void * p2) {
 	Data * data = new Data;
	if (data == NULL) {
		cout << "new 错误" << endl;
		exit(-1);
	}
	User * _user;
	switch (command) {
	case (_FIRST_CONNECT):
	{
		data->command = _FIRST_CONNECT;
		data->guid = user.back();
		*data->message = '\0';
		break;
	}
	case _LIST_CHANNEL:
	{
		data->command = _LIST_CHANNEL;
		data->guid = NULL;
		data->p1 = channel.size();//当前channel的数量
		for (int i = 0; i < data->p1; i++) {
			strcpy(data->message + (_MAX_STRING_LENTH + 1) * i, channel[i]->name);
		//超过了等一下再说
		}
		break;
	}
		
	case _JOIN_CHANNEL:
	{
		if (p1) {
			//成功加入了聊天室
			data->command = _JOIN_CHANNEL;
			data->guid = NULL;
			data->p1 = 1;
			*data->message = '\0';

		}
		else {
			//未能成功加入聊天室
			data->command = _JOIN_CHANNEL;
			data->guid = NULL;
			data->p1 = 0;
			*data->message = '\0';
			;
			//todo 返回错误状态
		}
		break;
	}
	case _LIST_USER:
	{
		//p2:频道指针
		if (p1) {
			//在聊天室中
			data->command = _LIST_USER;
			data->guid = NULL;
			data->p1 = 1;
			_user = ((Channel *)p2)->firstMember;
			for (int i = 0; i < data->p1; i++) {
				strcpy(data->message + (_MAX_STRING_LENTH + 1) * i, channel[i]->name);
				if (_user->nextMember != NULL) {
					_user = _user->nextMember;
					//todo：处理超出的情况
				}
				else {
					data->p2 = i+1;
					break;
				}
			}
			;
		}
		else {
			data->command = _LIST_USER;
			data->guid = NULL;
			data->p1 = 0;
			//不在聊天室中,不能完成此命令
			;

		}
		break;
	}
	case _MESSAGE:
		//p2为data指针
		if (p1) {
			data->command = _MESSAGE;
			data->guid = NULL;
			data->p1 = 1;
			strcpy(data->message, ((Data *)p2)->guid->id);
			//第一段拷贝id
			strcpy(data->message+_MAX_STRING_LENTH+1, ((Data *)p2)->message );
			//第二段拷贝信息
		}
		else {
			data->command = _MESSAGE;
			data->guid = NULL;
			data->p1 = 0;
			*data->message = '\0';
		}
		break;
	
	case _PRIVATE_MSG:
		if (p1) {
			data->command = _PRIVATE_MSG;
			data->guid = NULL;
			data->p1 = 1;
			strcpy(data->message, ((Data *)p2)->guid->id);
			//第一段拷贝id
			strcpy(data->message + _MAX_STRING_LENTH + 1, ((Data *)p2)->message+ _MAX_STRING_LENTH + 1);
			//第二段拷贝信息
		}
		else {
			data->command = _PRIVATE_MSG;
			data->guid = NULL;
			data->p1 = 0;
			*data->message = '\0';
		}
		break;
	case _LEAVE:
		if (p1==1) {
			data->command = _PRIVATE_MSG;
			data->guid = NULL;
			data->p1 = 1;
			*data->message = '\0';
		}
		else if(p1==0){
			data->command = _PRIVATE_MSG;
			data->guid = NULL;
			data->p1 = 0;
			*data->message = '\0';
		}
		else if (p1 == 2) {
			data->command = _PRIVATE_MSG;
			data->guid = NULL;
			data->p1 = 2;
			*data->message = '\0';
		}
	default:
		cout << "make了个屁" << endl;
		return NULL;

	}
	return data;
}
//为新的user分配内存空间，并加入vector<user>中
User * add_to_user(SOCKADDR_IN addrClient) {
	User * _user = new User;
	_user->clientAddr = addrClient;
	_user->currentChannel = NULL;
	*_user->id = '\0';
	_user->nextMember = NULL;
	user.push_back(_user);
	return _user;
}
//为新的channel分配内存空间，并加入vector<channel>中
void add_to_channel(string channelName) {
	Channel * _channel = new Channel;
	strcpy(_channel->name, channelName.data());
	_channel->firstMember = NULL;
	_channel->channelNum = channel.size();
	cout << " 创建频道：" << _channel->name << " 频道号：" << _channel->channelNum;
	channel.push_back(_channel);
	cout << endl;
}
Channel * find_channel_by_name(const char * name) {
	int amountOfChannel = channel.size();
	for (int i = 0; i < amountOfChannel; i++) {
		if (!strcmp(name, channel[i]->name)) return  channel[i];
		//两个字符串相等则返回0
	}
	return NULL;
}

void  add_user_to_channel(Channel *_channel, User *_user) {
	User * lastChannelMember;
	//将用户加入该聊天室的成员名单中
	if (_channel->firstMember == NULL) _channel->firstMember = _user;
	else {
		lastChannelMember = _channel->firstMember;

		while (lastChannelMember->nextMember != NULL) {
			lastChannelMember = lastChannelMember->nextMember;
		}
		lastChannelMember->nextMember = _user;
	}
	return;
}
void  delete_user_to_channel(Channel * _channel, User *_user) {
	
	User * lastChannelMember;
	//第一个就是_user
	if (_channel->firstMember == _user) _channel->firstMember = _user->nextMember;
	else {
		//_user在中间
		lastChannelMember = _channel->firstMember;

		while (lastChannelMember->nextMember != NULL) {
			lastChannelMember = lastChannelMember->nextMember;
		}
		lastChannelMember->nextMember = _user;
	}
	return;
}

//接受并处理来自客户的信息
DWORD WINAPI listen_to_client(LPVOID lpParamter)
{
	SOCKADDR_IN addrClient;//储存客户端的连接信息
	int len = sizeof(SOCKADDR);
	char _receiveBuff[1416];
	char _sendBuff[1416];
	char temp[32] = "hahaha";
	//可能会用到的
	SOCKET serverSocket = *(SOCKET *)lpParamter;

	//线程2处理来自客户端的连接
	while (1) {
		int recvDataSize = recvfrom(serverSocket, _receiveBuff, 1416, 0, (SOCKADDR*)&addrClient, &len);
		User * _user;
		Channel *_channel;
		if (recvDataSize != -1) {
			struct Data *data = (Data *)_receiveBuff;
			struct Data *sendMessage;
			

			int clientCommand =((Data *)_receiveBuff)->command;
			switch (clientCommand) {
			case _LIST_CHANNEL: 
			{
				cout << "用户要求列出聊天室" << endl;
				sendMessage = make_message(_LIST_CHANNEL, 0, NULL);
				sendto(serverSocket, (char*)sendMessage, 1416, 0, (SOCKADDR *)&addrClient, len);
				delete sendMessage;
				break; 
			}
			case _JOIN_CHANNEL:
			{
				cout << "用户要求加入聊天室" << endl;
				//在data中,以message为起始点,前31位为channelname,后31位为id
				_channel = find_channel_by_name(data->message);
				if (_channel) {
					//如果有这个聊天室
					add_user_to_channel(_channel, data->guid);
					//处理用户的相应数据
					_user = data->guid;
					strcpy(_user->id, data->message + _MAX_STRING_LENTH+1);
					_user->nextMember = NULL;
					_user->currentChannel = _channel;
					//发消息
					sendMessage = make_message(_JOIN_CHANNEL, 1, NULL);
					sendto(serverSocket, (char*)sendMessage, 100, 0, (SOCKADDR *)&addrClient, len);
					delete sendMessage;
				}
				else {
					//如果没有这个聊天室
					sendMessage = make_message(_JOIN_CHANNEL, 0, NULL);
					sendto(serverSocket, (char*)sendMessage, 30, 0, (SOCKADDR *)&addrClient, len);
					delete sendMessage;
					;
				}
				break;
			}
			case _LIST_USER:
			{
				cout << "用户要求列出用户" << endl;
				_user = data->guid;
				_channel = _user->currentChannel;
				if (_channel) {
					sendMessage = make_message(_LIST_USER, 1, _channel);
					sendto(serverSocket, (char*)sendMessage, 1416, 0, (SOCKADDR *)&addrClient, len);
					delete sendMessage;
				}
				else {
					//当前不在任何频道中
					sendMessage = make_message(_LIST_USER, 0, NULL);
					sendto(serverSocket, (char*)sendMessage, 30, 0, (SOCKADDR *)&addrClient, len);
					delete sendMessage;
				}
				break;
			}
			case _PRIVATE_MSG:
				//message中第一段是对方ID，第二段是私聊内容
		
				cout << "用户要求私聊" << endl;
				_user = data->guid;
				_channel = _user->currentChannel;
				strcpy(temp, data->message);
				User * _friend;
				//找到对应的用户
				_friend = _channel->firstMember;

					while (_friend!= NULL) {
						if (!strcmp(_friend->id, temp)) {
							break;
						}
						cout << _friend->id << endl;
						_friend = _friend->nextMember;

					}
					if (_friend != NULL) {
						sendMessage =make_message(_PRIVATE_MSG, 1, data);
						strcpy(sendMessage->message, _friend->id);
						sendto(serverSocket, (char*)sendMessage, 1416, 0, (SOCKADDR *)&_friend->clientAddr, len);

					}
					else {
						sendMessage =make_message(_PRIVATE_MSG, 0, NULL);
						sendto(serverSocket, (char*)sendMessage, 1416, 0, (SOCKADDR *)&addrClient, len);

					}
					delete sendMessage;
				
				break;
			case _LEAVE:
				cout << "用户要求离开聊天室" << endl;
				_user = data->guid;
				_channel = _user->currentChannel;
				if (_channel) {
					//发送消息
					sendMessage = make_message(_LEAVE, 1, data);
					sendto(serverSocket, (char*)sendMessage, 28, 0, (SOCKADDR *)&_friend->clientAddr, len);
					_user->currentChannel = NULL;
					*_user->id = '\0';
					_user->nextMember = NULL;
					//处理当前频道中的用户

				}
				else {
					sendMessage = make_message(_LEAVE, 0, data);
					sendto(serverSocket, (char*)sendMessage, 28, 0, (SOCKADDR *)&_friend->clientAddr, len);

				}
				delete sendMessage;
				break;
			case _MESSAGE:
				//在data中,以message就是用户发的信息
				cout << "用户发了消息" << endl;
				_user = data->guid;
				_channel = _user->currentChannel;
					if (_channel) {
						sendMessage=make_message(_MESSAGE, 1, data);
						User * currentUser = _channel->firstMember;
						while (currentUser != NULL) {
							sendto(serverSocket, (char *)sendMessage, 1416, 0, (SOCKADDR *)&(currentUser->clientAddr), len);
							
							currentUser = currentUser->nextMember;
						}
						
					}
					else {
						sendMessage = make_message(_MESSAGE, 0, NULL);
						sendto(serverSocket, (char *)sendMessage, 30, 0, (SOCKADDR *)&addrClient, len);
						

					}
		
			//		delete sendMessage;
					break;
			case _FIRST_CONNECT:
				_user = add_to_user(addrClient);
				sendMessage = make_message(_FIRST_CONNECT, 0, NULL);
				sendto(serverSocket, (char*)sendMessage, 30, 0, (SOCKADDR *)&addrClient, len);
				cout << "新用户的加入，为其分配GUID为" << _user << endl;
				delete sendMessage;
				break;

			default:
				cout << "用户放了个屁" << endl;
				break;
			}
		}
	}
}
