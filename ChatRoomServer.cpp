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
	char receiveBuff[1420];
	char sendBuff[1420];
	SOCKADDR_IN _addrClient;//???
	int len = sizeof(SOCKADDR);
	string command;
	cout << "服务端" << endl;
	help_message();
	int pid=_getpid();
	cout <<"pid ="<< pid << endl;
	
	char temp[122] = "hahaha";
	HANDLE hThread = CreateThread(NULL, 0, Fun, (LPVOID*)&serverSocket, 0, NULL);
	CloseHandle(hThread);

//主线程处理用户的输入
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
			string channelName;
			cin >> channelName;
			add_to_channel(channelName);
			cout << channelName << endl;
			break;
		}
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
	if (_LIST_CHANNEL<=data->command && _FIRST_CONNECT >=data->command) return data->command;
	else return -1;
}
void Exit() {
	cout << "bye bye" << endl;
	exit(0);
}
int command_parse(string command) {
	if (command == "quit") return _QUIT;
	else if (command == "openchannel")return _OPENCHANNEL;

	return 999;
}
Data * make_message(int command,int p1,void * p2) {
	Data * data = new Data;
	switch (command) {
	case (_FIRST_CONNECT):
		data->command = _FIRST_CONNECT;
		data->guid = user.back();
		*data->message = '\0';
		break;
	case _LIST_CHANNEL:
		data->command = _LIST_CHANNEL;
		data->guid = NULL;
		data->p1 = channel.size();//当前channel的数量
		for (int i = 0; i < data->p1;i++ ) {
			strcpy(data->message + 129 * i, channel[i]->name);

		}
		break;
		//超过了等一下再说
	case _JOIN_CHANNEL:
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
	case _LIST_USER:
			if(p1){
				//不在聊天室中,不能完成此命令
			}
			else{
				//在聊天室中
			}
		break;
	default:
		cout << "make了个屁" << endl;
		return NULL;

	}
	return data;
}
User * add_to_user(SOCKADDR_IN addrClient) {
	User * _user = new User;
	_user->clientAddr = addrClient;
	_user->currentChannel = NULL;
	*_user->id = '\0';
	_user->nextMember = NULL;
	user.push_back(_user);
	return _user;
}
void add_to_channel(string channelName) {
	Channel * _channel = new Channel;
	strcpy(_channel->name, channelName.data());
	_channel->firstMember = NULL;
	_channel->channelNum = channel.size();
	cout << " 创建频道：" << _channel->name << " 频道号：" << _channel->channelNum;
	channel.push_back(_channel);
	cout << endl;
}
Channel * find_channel_by_name(char * name) {
	int amountOfChannel=channel.size();
	for (int i = 0; i < amountOfChannel; i++) {
		if (!strcmp(name, channel[i]->name)) return  channel[i];
		//两个字符串相等则返回0
	}
	return NULL;
}
DWORD WINAPI Fun(LPVOID lpParamter)
{
	SOCKADDR_IN addrClient;//储存客户端的连接信息
	int len = sizeof(SOCKADDR);
	char _receiveBuff[1420];
	char _sendBuff[1420];
	char temp[122] = "hahaha";
	SOCKET serverSocket = *(SOCKET *)lpParamter;

//线程2处理来自客户端的连接
	while (1) {
		int recvDataSize = recvfrom(serverSocket, _receiveBuff, 1400, 0, (SOCKADDR*)&addrClient, &len);
		User * _user;
		Channel *_channel;
		if (recvDataSize != -1) {
			struct Data *data = (Data *)_receiveBuff;
			struct Data *sendMessage;
			
			int clientCommand = data_parse((Data *)_receiveBuff);
			switch (clientCommand) {
			case _LIST_CHANNEL:
				cout << "用户要求列出聊天室" << endl;
				sendMessage=make_message(_LIST_CHANNEL, 0,NULL);
				sendto(serverSocket, (char*)sendMessage, 1420, 0, (SOCKADDR *)&addrClient, len);
				delete sendMessage;
				break;
			case _JOIN_CHANNEL:
				cout << "用户要求加入聊天室" << endl;
				//在data中,以message为起始点,p1指向channelname,p2指向id
				 _channel = find_channel_by_name(data->message);
				if (_channel) {
					//如果有这个聊天室
					User * lastChannelMember;
					//将用户加入该聊天室的成员名单中
					if (_channel->firstMember == NULL) _channel->firstMember = data->guid;
					else {
						lastChannelMember = _channel->firstMember;

						while (lastChannelMember->nextMember != NULL) {
							lastChannelMember = lastChannelMember->nextMember;
						}
						lastChannelMember->nextMember = data->guid;
					}
					//处理用户的相应数据
					_user=data->guid;
					strcmp(_user->id ,data->message+data->p2);
					_user->nextMember=NULL;
					_user->currentChannel=_channel;
					//发消息
					sendMessage = make_message(_JOIN_CHANNEL, 1,NULL);
				}
				else {
					//如果没有这个聊天室
					sendMessage = make_message(_JOIN_CHANNEL, 0,NULL);
					sendto(serverSocket, (char*)sendMessage, 30, 0, (SOCKADDR *)&addrClient, len);
					;
				}
				break;
			case _LIST_USER:
				cout << "用户要求列出用户" << endl;
				_user=data->guid;
				_channel=_user->currentChannel
				if(_channel){
					sendMessage=make_message(_LIST_USER,0,_channel);
					sendto(serverSocket, (char*)sendMessage, 1420, 0, (SOCKADDR *)&addrClient, len);
				}
				else{
					//当前不在任何频道中
					sendMessage=make_message(_LIST_USER,1,NULL);
					sendto(serverSocket, (char*)sendMessage, 30, 0, (SOCKADDR *)&addrClient, len);
				}
				break;
			case _PRIVATE_MSG:
				cout << "用户要求私聊" << endl;
				break;
			case _LEAVE:
				cout << "用户要求离开" << endl;
				break;
			case _MESSAGE:
				cout << "用户发了消息" << endl;
				_user=data->guid;
				_channel=_user->currentChannel
				if(_channel){

				}
				else{

				}
				cout << data->message << endl;
				sendto(serverSocket, temp, 10, 0, (SOCKADDR *)&addrClient, len);
				break;
			case _FIRST_CONNECT:
				_user = 	add_to_user(addrClient);
				sendMessage = make_message(_FIRST_CONNECT, 0,NULL);
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

