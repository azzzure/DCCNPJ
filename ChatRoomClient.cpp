#include "ChatRoomClient.h"
#include <winsock2.h>
using namespace std;
User * guid=NULL;
User * user = new User;
int threadSema = 1;
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
	serverAddr.sin_addr.S_un.S_addr = inet_addr("192.168.31.24");
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

	//等待来自标准输入的命令
	while (1) {
		cin >> command;
		int _command = command_parse(command);
		int p1 = 0; int p2 = 0;
		string channelname, userid,friendid,privateMessage;
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
			strcpy(user->id , userid.data());
			//todo 合法性检查
			data->command = _JOIN_CHANNEL;
			data->guid = guid;

			strcpy(data->message, channelname.data());
			strcpy(data->message + _MAX_STRING_LENTH + 1, userid.data());
			sendto(clientSocket, (char *)data, 120, 0, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
			break;
		
		case _LIST_USER:
			cout << "列出用户" << endl;
			data->command = _LIST_USER;
			data->guid = guid;
			*data->message = '\0';
			sendto(clientSocket, (char *)data, 28, 0, (SOCKADDR *)&serverAddr, sizeof(serverAddr));

			break;
		case _PRIVATE_MSG:
			//message第一段是id，第二段是消息
			cout << "私聊" << endl;
			cin >> friendid;
			getline(cin,privateMessage);
			data->command = _PRIVATE_MSG;
			data->guid = guid;
			strcpy(data->message, friendid.data());
			strcpy(data->message + _MAX_STRING_LENTH + 1, privateMessage.data());
			sendto(clientSocket, (char *)data, 1416, 0, (SOCKADDR *)&serverAddr, sizeof(serverAddr));

			break;
		case _LEAVE:
			cout << "退出聊天室"<<endl;
			data->command = _LEAVE;
			data->guid = guid;
			*data->message = '\0';
			sendto(clientSocket, (char *)data, 1416, 0, (SOCKADDR *)&serverAddr, sizeof(serverAddr));

			break;
		case _MESSAGE:
			data->command = _MESSAGE;
			data->guid = guid;
			strcpy( data->message,command.data());
			sendto(clientSocket, (char *)data, 1416, 0, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
			break;

		case _INIT :
			cout << "重新初始化" << endl;
			reinit(clientSocket, serverAddr);
			//WaitForSingleObject(hThread, INFINITE);
			
			 hThread = CreateThread(NULL, 0, Fun, (LPVOID*)&clientSocket, 0, NULL);
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
int command_parse(string & command) {

	if (command == "quit") return _QUIT;
	else if (command == "channels")return _LIST_CHANNEL;
	else if (command == "join")return _JOIN_CHANNEL;
	else if (command == "list")return _LIST_USER;
	else if (command == "msg")return _PRIVATE_MSG;
	else if (command == "leave") return _LEAVE;
	else if (command == "init") return _INIT;
	else if (command == "msg") return _PRIVATE_MSG;
	else if (command == "help") return 99;
	string temp;
	getline(cin, temp);
	command += temp;
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
void reinit(SOCKET clientSocket, SOCKADDR_IN serverAddr){
	Data *data = new Data;
	data->command = _FIRST_CONNECT;
	*(data->message) = '\0';
	int len = sizeof(SOCKADDR);
	char receiveBuff[30];

	int k = sendto(clientSocket, (char *)data, 30, 0, (SOCKADDR *)&serverAddr, len);
	
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
					cout << data->message+i * (_MAX_STRING_LENTH+1) << endl;
				}
				break;
			case _JOIN_CHANNEL:
				cout << "收到服务器的消息：";
				if (data->p1) {
					cout<<"成功加入了聊天室！";

				}
				else {
					cout << "加入错误." << endl;
				}
				break;
			case _LIST_USER:
				cout << "收到服务器的消息：" ;
				if (data->p1) {
					cout << "列出所有用户" << endl;
					for (int i = 0; i > data->p2; i++) {
						cout << data->message + i * (_MAX_STRING_LENTH + 1) << endl;
					}
				}
				else {
					cout << "失败，您可能不在聊天室中" << endl;
				}
				break;
			case _PRIVATE_MSG:
				//第一段是id，第二段的message
				cout << "收到服务器的消息：私聊" << endl;
				if (data->p1) {
					 cout << "[" << data->message << "]的私聊: \n\t" << data->message + _MAX_STRING_LENTH + 1 << endl;
				}
				else {
					cout << "错误，没有这个朋友" << endl;
				}
				break;
			case _LEAVE:
				cout << "收到服务器的消息：" ;
				if (data->p1==1) {
					cout << "离开聊天室"<<endl;
				}
				else if(data->p1==0){
					cout << "您当前不在任何聊天室中" << endl;
				}
				else if (data->p1 == 2) {
					cout << "您被管理员从频道中踢出" << endl;
				}
				break;
			case _MESSAGE:
				if (data->p1) {
					if (strcmp(data->message, user->id)) cout << "[" << data->message << "]: \n\t" << data->message + _MAX_STRING_LENTH + 1 << endl;
				}
				else {
					cout << "错误，您可能不在聊天室中"<<endl;
				}
			//	sendto(serverSocket, temp, 10, 0, (SOCKADDR *)&serverAddr, len);
				break;
			case _FIRST_CONNECT:
				cout << "成功连接到服务端！分配的guid为" << hex << data->guid << endl;
				guid = data->guid;
				break;
			default:
				cout << "服务器发了个屁" << endl;
				break;
			}
		}
	}
	cout << "监听线程被终止了" << endl;
	return 0;
}
