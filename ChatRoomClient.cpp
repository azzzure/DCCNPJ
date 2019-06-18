#include "ChatRoomClient.h"
#include <winsock2.h>
using namespace std;
User * guid = NULL;
User * user = new User;
int threadSema = 1;
int main(int argc, char * argv[]) {

	//int socket_fd;
	//socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	//struct  sockaddr_in ser_addr;
	//ser_addr.sin_addr
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET clientSocket = socket(PF_INET, SOCK_DGRAM, 0);//�����׽���
	SOCKADDR_IN serverAddr;//�����ַ�ṹ�����
	serverAddr.sin_family = AF_INET;//Э����
	serverAddr.sin_port = htons(_MY_PORT);//�˿ں�
	serverAddr.sin_addr.S_un.S_addr = inet_addr("192.168.31.24");
	//bind(clientSocket, (SOCKADDR*)&addr, sizeof(SOCKADDR));


	char receiveBuff[1400];
	memset(receiveBuff, 0, 1400);
	char sendBuff[1400];
	SOCKADDR_IN _addrClient;//???
	int len = sizeof(SOCKADDR);
	string command;
	cout << "�ͻ���" << endl;
	init(clientSocket, serverAddr);
	//��ʼ�����õ�GUID

	HANDLE hThread = CreateThread(NULL, 0, Fun, (LPVOID*)&clientSocket, 0, NULL);
	//CloseHandle(hThread);

	//�ȴ����Ա�׼���������
	while (1) {
		cin >> command;
		int _command = command_parse(command);
		int p1 = 0; int p2 = 0;
		string channelname, userid, friendid, privateMessage;
		Data * data = new Data;
		switch (_command) {
		case _QUIT: {
			cout << "gg" << endl;

			Exit();
		}
		case _LIST_CHANNEL:
			cout << "�г�Ƶ��" << endl;
			data->command = _LIST_CHANNEL;
			data->guid = guid;
			*data->message = '\0';
			sendto(clientSocket, (char *)data, 28, 0, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
			break;
		case _JOIN_CHANNEL:
			cout << "����Ƶ��" << endl;
			cin >> channelname;
			cin >> userid;
			strcpy(user->id, userid.data());
			//todo �Ϸ��Լ��
			data->command = _JOIN_CHANNEL;
			data->guid = guid;

			strcpy(data->message, channelname.data());
			strcpy(data->message + _MAX_STRING_LENTH + 1, userid.data());
			sendto(clientSocket, (char *)data, 120, 0, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
			break;

		case _LIST_USER:
			cout << "�г��û�" << endl;
			data->command = _LIST_USER;
			data->guid = guid;
			*data->message = '\0';
			sendto(clientSocket, (char *)data, 28, 0, (SOCKADDR *)&serverAddr, sizeof(serverAddr));

			break;
		case _PRIVATE_MSG:
			//message��һ����id���ڶ�������Ϣ
			cout << "˽��" << endl;
			cin >> friendid;
			getline(cin, privateMessage);
			data->command = _PRIVATE_MSG;
			data->guid = guid;
			strcpy(data->message, friendid.data());
			strcpy(data->message + _MAX_STRING_LENTH + 1, privateMessage.data());
			sendto(clientSocket, (char *)data, 1416, 0, (SOCKADDR *)&serverAddr, sizeof(serverAddr));

			break;
		case _LEAVE:
			cout << "�˳�������" << endl;
			data->command = _LEAVE;
			data->guid = guid;
			*data->message = '\0';
			sendto(clientSocket, (char *)data, 1416, 0, (SOCKADDR *)&serverAddr, sizeof(serverAddr));

			break;
		case _MESSAGE:
			data->command = _MESSAGE;
			data->guid = guid;
			strcpy(data->message, command.data());
			sendto(clientSocket, (char *)data, 1416, 0, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
			break;

		case _INIT:
			cout << "���³�ʼ��" << endl;
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
	cout << "�ͻ��˰�����Ϣ" << endl;
	cout << "join channel username ��username����������channel��" << endl;
	cout << "channels �г����е�������" << endl;
	cout << "list �г���ǰ�����ҵ������û�" << endl;
	cout << "msg userid your_message_here ���û�����˽����Ϣ" << endl;
	cout << "leave �뿪������" << endl;
	cout << "quit �˳�������" << endl;
	cout << "" << endl;

}
int data_parse(Data *data) {
	if (_QUIT <= data->command&& _FIRST_CONNECT >= data->command) return data->command;
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
	Data *data = new Data;
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
			cout << "�ɹ����ӵ�����ˣ������guidΪ" << hex << data->guid << endl;
			return;
		}
	}
}
void reinit(SOCKET clientSocket, SOCKADDR_IN serverAddr) {
	Data *data = new Data;
	data->command = _FIRST_CONNECT;
	*(data->message) = '\0';
	int len = sizeof(SOCKADDR);
	char receiveBuff[30];

	int k = sendto(clientSocket, (char *)data, 30, 0, (SOCKADDR *)&serverAddr, len);

}

//�̣߳����ڽ��ܷ���������������Ϣ
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
				cout << "�յ�����������Ϣ���г�������" << endl;
				for (int i = 0; i < data->p1; i++) {
					cout << data->message + i * (_MAX_STRING_LENTH + 1) << endl;
				}
				break;
			case _JOIN_CHANNEL:
				cout << "�յ�����������Ϣ��";
				if (data->p1==1) {
					cout << "�ɹ������������ң�" << endl;;

				}
				else if(data->p1==0){
					cout << "���������������. ����ʧ��" << endl;
				}
				else if (data->p1 == 2) {
					cout << "id�ظ��ˣ�����ʧ��" << endl;
				}
				break;
			case _LIST_USER:
				cout << "�յ�����������Ϣ��";
				if (data->p1) {
					cout << "�г������û�" << endl;
					for (int i = 0; i < data->p2; i++) {
						cout << data->message + i * (_MAX_STRING_LENTH + 1) << endl;
					}
				}
				else {
					cout << "ʧ�ܣ������ܲ�����������" << endl;
				}
				break;
			case _PRIVATE_MSG:
				//��һ����id���ڶ��ε�message
				cout << "�յ�����������Ϣ��˽��" << endl;
				if (data->p1) {
					cout << "[" << data->message << "]��˽��: \n\t" << data->message + _MAX_STRING_LENTH + 1 << endl;
				}
				else {
					cout << "����û���������" << endl;
				}
				break;
			case _LEAVE:
				cout << "�յ�����������Ϣ��";
				if (data->p1 == 1) {
					cout << "�뿪������" << endl;
				}
				else if (data->p1 == 0) {
					cout << "����ǰ�����κ���������" << endl;
				}
				else if (data->p1 == 2) {
					cout << "��������Ա��Ƶ�����߳�" << endl;
				}
				else if (data->p1 == 3) {
					cout << "����Ա��ɢ��Ƶ��" << endl;
				}
				break;
			case _MESSAGE:
				if (data->p1) {
					if (strcmp(data->message, user->id)) cout << "[" << data->message << "]: \n\t" << data->message + _MAX_STRING_LENTH + 1 << endl;
				}
				else {
					cout << "���������ܲ�����������" << endl;
				}
				//	sendto(serverSocket, temp, 10, 0, (SOCKADDR *)&serverAddr, len);
				break;
			case _FIRST_CONNECT:
				cout << "�ɹ����ӵ�����ˣ������guidΪ" << hex << data->guid << endl;
				guid = data->guid;
				break;
			case _QUIT:
				cout << "���������˳���"<< endl;
				guid = data->guid;
				exit(0);
				break;
			default:
				cout << "���������Ͳ�����Ϣ" << endl;
				break;
			}
		}
	}
	cout << "�����̱߳���ֹ��" << endl;
	return 0;
}