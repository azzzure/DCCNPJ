#include "ChatRoomClient.h"
#include <winsock2.h>
using namespace std;
User * guid;
int main() {

	//int socket_fd;
	//socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	//struct  sockaddr_in ser_addr;
	//ser_addr.sin_addr
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET clientSocket = socket(PF_INET, SOCK_DGRAM, 0);//创建套接字
	SOCKADDR_IN serverAddr;//定义地址结构体变量
	serverAddr.sin_family = AF_INET;//协议族
	serverAddr.sin_port = htons(_MY_PORT);//端口号
	serverAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	//bind(clientSocket, (SOCKADDR*)&addr, sizeof(SOCKADDR));


	char receiveBuff[1400];
	memset(receiveBuff, 0, 1400);
	char sendBuff[1400];
	SOCKADDR_IN _addrClient;//???
	int len = sizeof(SOCKADDR);
	string command;
	cout << "客户端" << endl;

	init(clientSocket, serverAddr);
	//初始化，得到GUID

	HANDLE hThread = CreateThread(NULL, 0, Fun, (LPVOID*)&clientSocket, 0, NULL);
	//CloseHandle(hThread);

	while (1) {
		cin >> command;
		int _command = command_parse(command);
		int p1 = 0; int p2 = 0;
		string channelname, userid;
		Data * data = new Data;
		switch (_command) {
		case _QUIT: {
			cout << "gg" << endl;

			Exit();
		}
		case _LIST_CHANNEL:
			cout << "列出频道" << endl;
			data->command = _LIST_CHANNEL;
			data->guid = guid;
			*data->message = '\0';
			sendto(clientSocket, (char *)data, 28, 0, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
			break;
		case _JOIN_CHANNEL:
			cout << "加入频道" << endl;
			cin >> channelname;
			cin >> userid;
			p1 = channelname.size();
			p2 = userid.size();
			
			data->command = _JOIN_CHANNEL;
			data->guid = guid;
			data->p1 = 0;
			data->p2 = p1 + 1;
			strcpy(data->message, channelname.data());
			strcpy(data->message + p1 + 1, userid.data());
			sendto(clientSocket, (char *)data, 1400, 0, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
			break;
		
		case _LIST_USER:
			break;
		case _PRIVATE_MSG:
			break;
		case _LEAVE:
			break;
		case _MESSAGE:
			data->command = _MESSAGE;
			strcpy( data->message,command.data());
			sendto(clientSocket, (char *)data, 1400, 0, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
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
	cout << "客户端帮助信息" << endl;
	cout << "join channel username 以username加入聊天室channel中" << endl;
	cout << "channels 列出现有的聊天室" << endl;
	cout << "list 列出当前聊天室的所有用户" << endl;
	cout << "msg userid your_message_here 给用户发送私人消息" << endl;
	cout << "leave 离开聊天室" << endl;
	cout << "quit 退出本程序" << endl;
	cout << "" << endl;

}
int data_parse(Data *data) {
	if (_LIST_CHANNEL <= data->command&& _FIRST_CONNECT >= data->command) return data->command;
	else return -1;
}
void Exit() {
	cout << "bye bye" << endl;
	exit(0);
	//todo
}
int command_parse(string command) {
	if (command == "quit") return _QUIT;
	else if (command == "channels")return _LIST_CHANNEL;
	else if (command == "join")return _JOIN_CHANNEL;
	else if (command == "list")return _LIST_USER;
	else if (command == "msg")return _PRIVATE_MSG;
	else if (command == "leave") return _LEAVE;
	else if (command == "help") return 99;

	return _MESSAGE;
}
void init(SOCKET clientSocket, SOCKADDR_IN serverAddr) {
	Data *data=new Data;
	data->command = _FIRST_CONNECT;
	*(data->message) = '\0';
	int len = sizeof(SOCKADDR);
	char receiveBuff[30];
	
	int k = sendto(clientSocket, (char *)data, 30, 0, (SOCKADDR *)&serverAddr, len);
	while (1) {
		int recvDataSize = recvfrom(clientSocket, receiveBuff, 30, 0, (SOCKADDR*)&serverAddr, &len);
		if (recvDataSize != -1) {
			data = (Data*)receiveBuff;
			guid = data->guid;
			cout << "成功连接到服务端！分配的guid为"<<hex<< data->guid<< endl;
			return; 
		}
	}
}

//线程，用于接受服务器发回来的消息
DWORD WINAPI Fun(LPVOID lpParamter)
{
	SOCKADDR_IN clientAddr;//???
	int len = sizeof(SOCKADDR);
	char receiveBuff[1420];
	char sendBuff[1420];
	char temp[122] = "hahaha";
	SOCKET _clientSocket = *(SOCKET *)lpParamter;
	while (1) {
		int recvDataSize = recvfrom(_clientSocket, receiveBuff, 1420, 0, (SOCKADDR*)&clientAddr, &len);
		
		if (recvDataSize != -1) {
			struct Data *data = (Data *)receiveBuff;
			struct Data *sendMessage;
			int clientCommand = data_parse((Data *)receiveBuff);
			switch (clientCommand) {
			case _LIST_CHANNEL:
				cout << "收到服务器的消息：列出聊天室" << endl;
				for (int i = 0; i < data->p1; i++) {
					cout << data->message+i * 129 << endl;
				}
				break;
			case _JOIN_CHANNEL:
				cout << "收到服务器的消息：加入了聊天室" << endl;
				if (data->p1) {
					//加入了聊天室
					;
				}
				else {
					cout << "加入错误，可能是没有对应的聊天室" << endl;
				}
				break;
			case _LIST_USER:
				cout << "收到服务器的消息：列出所有用户" << endl;
				break;
			case _PRIVATE_MSG:
				cout << "收到服务器的消息：私聊" << endl;
				break;
			case _LEAVE:
				cout << "收到服务器的消息：离开" << endl;
				break;
			case _MESSAGE:


				cout << data->message << endl;

			//	sendto(serverSocket, temp, 10, 0, (SOCKADDR *)&serverAddr, len);
				break;
			
				break;

			default:
				cout << "服务器发了个屁" << endl;
				break;
			}
		}
	}
	return 0;
}
