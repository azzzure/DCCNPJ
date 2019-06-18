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

	int serverSocket = socket(PF_INET, SOCK_DGRAM, 0);//�����׽���
	SOCKADDR_IN addr;//�����ַ�ṹ�����
	addr.sin_family = AF_INET;//Э����
	addr.sin_port = htons(_MY_PORT);//�˿ں�
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	bind(serverSocket, (SOCKADDR*)&addr, sizeof(SOCKADDR));
	int len = sizeof(SOCKADDR);
	string command;
	cout << "�����" << endl;
	help_message();
	int pid = _getpid();
	cout << "pid =" << pid << endl;

	char temp[122] = "hahaha";
	HANDLE LTC = CreateThread(NULL, 0, listen_to_client, (LPVOID*)&serverSocket, 0, NULL);
	CloseHandle(LTC);
	//Ĭ��Ƶ��df
	add_to_channel("df");

	//���̴߳������Ա������
	Channel *currentChannel = NULL;
	string channelName, userName;
	User * tempUser,*tempUser2;
	while (1) {
		int channelNum;
		struct Data *sendMessage;
		User *temp;
		cin >> command;
		int _command = command_parse(command);
		switch (_command) {
		case _QUIT: {
			cout << "gg" << endl;
			
			int num = user.size();
			for (int i = 0; i < num; i++) {
				temp = user[i];
				sendMessage = make_message(_QUIT, 0, NULL);
				sendto(serverSocket, (char*)sendMessage, 30, 0, (SOCKADDR *)&temp->clientAddr, len);

			}
			Exit();
		}
		case _OPENCHANNEL: {
			

			cin >> channelName;
			if (add_to_channel(channelName)) {
				cout << "����Ƶ��ʧ��";
			}
			else {
				
				cout << "openchannel:";
				cout << channelName << endl;
			}
			break;
		}
		case _ENTERCHANNEL:
			cin >> channelName;
			currentChannel = find_channel_by_name(channelName.data());
			if (currentChannel) {
				cout << "�ɹ�����Ƶ��" << currentChannel->name << endl;
			}
			else {
				cout << "û�����Ƶ����" << endl;
			}
			break;
		case _LIST_USER:
			if (currentChannel) {
				tempUser = currentChannel->firstMember;
				if (tempUser) {
					cout << "Ƶ��" << currentChannel->name << "�е��û�Ϊ��" << endl;

					while (tempUser != NULL) {
						cout << tempUser->id << endl;
						tempUser = tempUser->nextMember;
					}

				}
				else {
					cout << "Ƶ��" << currentChannel->name << "�л�û���κ��û�" << endl;
				}
			}
			else {
				cout << "������Ƶ���У�" << endl;
			}
			break;
		case _KICK:
			//todo ���û��߳�Ƶ��
			cin >> userName;
				if (currentChannel) {
					tempUser=delete_user_from_channel(currentChannel, userName.data());
					sendMessage=make_message(_LEAVE,2,NULL);
					sendto(serverSocket, (char*)sendMessage, 30, 0, (SOCKADDR *)&tempUser->clientAddr, len);
					delete sendMessage;
				}
				else {
					cout << "������Ƶ���У�" << endl;
				}

				break;
		case _LEAVE:
			if (currentChannel) {
				cout << "���˳�Ƶ��" << currentChannel->name << endl;
				currentChannel = NULL;
			}
			else {
				cout << "������Ƶ���У�" << endl;
			}
			
			break;
		case _LIST_CHANNEL:
			 ;
			if (channel.size() == 0) {
				cout << "��ǰû���κ�Ƶ��" << endl;
			}
			for (int i = 0; i < channel.size(); i++) {
				cout << channel[i]->name << endl;
			}
			
			break;
		case _CLOSECHANNEL:
			cin >> channelName;
			currentChannel = find_channel_by_name(channelName.data());
			if (currentChannel) {
				cout << "�ر�Ƶ����" << endl;
				sendMessage = make_message(_LEAVE, 3, NULL);
				
				tempUser = currentChannel->firstMember;
				while (tempUser != NULL) {
					tempUser2 = tempUser;
					tempUser = tempUser->nextMember;
					delete_user_from_channel(currentChannel, tempUser2->id);
					
					sendto(serverSocket, (char*)sendMessage, 30, 0, (SOCKADDR *)&tempUser2->clientAddr, len);
					
				}	
				delete sendMessage;

				channelNum = find_channel_by_name2(currentChannel->name);
				if (channelNum != -1) {
					channel.erase(channel.begin() + channelNum);
				}

			}
			else {
				cout << "û�����Ƶ����" << endl;
			}
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
	cout << "quit �˳�������" << endl;
	cout << "openchannel channelName :��ͨ�µ�������" << endl;
	cout << "channels �г����е�������" << endl;
	cout << "enterchannel channelName �������е���������" << endl;
	cout << "list �г���ǰ�����ҵ������û�" << endl;
	cout << "kickout userid ���û��߳�������" << endl;
	cout << "leave �뿪������" << endl;
	cout << "closechannel channelname �ر�������" << endl;
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
	else if (command == "openchannel" || command == "op")return _OPENCHANNEL;
	else if (command == "channels" )return _LIST_CHANNEL;
	else if (command == "enterchannel" || command == "ec")return _ENTERCHANNEL;
	else if (command == "list")return _LIST_USER;
	else if (command == "kickout")return _KICK;
	else if (command == "leave")return _LEAVE;
	else if (command == "closechannel" || command == "cc")return _CLOSECHANNEL;

	return 999;
}
Data * make_message(int command, int p1, void * p2) {
	int ii = 0;
	Data * data = new Data;
	if (data == NULL) {
		cout << "new ����" << endl;
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
		data->p1 = channel.size();//��ǰchannel������
		for (int i = 0; i < data->p1; i++) {
			strcpy(data->message + (_MAX_STRING_LENTH + 1) * i, channel[i]->name);
			//�����˵�һ����˵
		}
		break;
	}

	case _JOIN_CHANNEL:
	{
		if (p1==1) {
			//�ɹ�������������
			data->command = _JOIN_CHANNEL;
			data->guid = NULL;
			data->p1 = 1;
			*data->message = '\0';

		}
		else if(p1==0){
			//δ�ܳɹ�����������
			data->command = _JOIN_CHANNEL;
			data->guid = NULL;
			data->p1 = 0;
			*data->message = '\0';
			;
			
		}
		else if (p1 == 2) {
			//�����ظ�
			data->command = _JOIN_CHANNEL;
			data->guid = NULL;
			data->p1 = 2;
			*data->message = '\0';
			;
			//todo ���ش���״̬
		}
		break;
	}
	case _LIST_USER:
	{
		//p2:Ƶ��ָ��
		if (p1) {
			//����������
			data->command = _LIST_USER;
			data->guid = NULL;
			data->p1 = 1;
			_user = ((Channel *)p2)->firstMember;
			ii = 0;
			while(_user!=NULL){
				
				strcpy(data->message + (_MAX_STRING_LENTH + 1) * ii, _user->id);
				_user = _user->nextMember;
				ii++;
			}
			data->p2 = ii;
			;
		}
		else {
			data->command = _LIST_USER;
			data->guid = NULL;
			data->p1 = 0;
			//������������,������ɴ�����
			;

		}
		break;
	}
	case _MESSAGE:
		//p2Ϊdataָ��
		if (p1) {
			data->command = _MESSAGE;
			data->guid = NULL;
			data->p1 = 1;
			strcpy(data->message, ((Data *)p2)->guid->id);
			//��һ�ο���id
			strcpy(data->message + _MAX_STRING_LENTH + 1, ((Data *)p2)->message);
			//�ڶ��ο�����Ϣ
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
			//��һ�ο���id
			strcpy(data->message + _MAX_STRING_LENTH + 1, ((Data *)p2)->message + _MAX_STRING_LENTH + 1);
			//�ڶ��ο�����Ϣ
		}
		else {
			data->command = _PRIVATE_MSG;
			data->guid = NULL;
			data->p1 = 0;
			*data->message = '\0';
		}
		break;
	case _LEAVE:
		if (p1 == 1) {
			data->command = _LEAVE;
			data->guid = NULL;
			data->p1 = 1;
			*data->message = '\0';
		}
		else if (p1 == 0) {
			data->command = _LEAVE;
			data->guid = NULL;
			data->p1 = 0;
			*data->message = '\0';
		}
		else if (p1 == 2) {
			data->command = _LEAVE;
			data->guid = NULL;
			data->p1 = 2;
			*data->message = '\0';
		}
		else if (p1 == 3) {
			data->command = _LEAVE;
			data->guid = NULL;
			data->p1 = 3;
			*data->message = '\0';
		}
		break;
	case _QUIT:
		data->command = _QUIT;
		data->guid = NULL;

		*data->message = '\0';
		break;

	default:
		cout << "��Ϣ���ʹ���" << endl;
		return NULL;

	}
	return data;
}
//Ϊ�µ�user�����ڴ�ռ䣬������vector<user>��
User * add_to_user(SOCKADDR_IN addrClient) {
	User * _user = new User;
	_user->clientAddr = addrClient;
	_user->currentChannel = NULL;
	*_user->id = '\0';
	_user->nextMember = NULL;
	user.push_back(_user);
	return _user;
}
//Ϊ�µ�channel�����ڴ�ռ䣬������vector<channel>��
int add_to_channel(string channelName) {
	if (!find_channel_by_name(channelName.data())) {


		Channel * _channel = new Channel;
		strcpy(_channel->name, channelName.data());
		_channel->firstMember = NULL;
		_channel->channelNum = channel.size();
		cout << " ����Ƶ����" << _channel->name << " Ƶ���ţ�" << _channel->channelNum;
		channel.push_back(_channel);
		cout << endl;
		return 0;
	}
	else {
		return -1;
	}
}
Channel * find_channel_by_name(const char * name) {
	int amountOfChannel = channel.size();
	for (int i = 0; i < amountOfChannel; i++) {
		if (!strcmp(name, channel[i]->name)) return  channel[i];
		//�����ַ�������򷵻�0
	}
	return NULL;
}

int find_channel_by_name2(const char * name) {
	int amountOfChannel = channel.size();
	for (int i = 0; i < amountOfChannel; i++) {
		if (!strcmp(name, channel[i]->name)) return  i;
		//�����ַ�������򷵻�0
	}
	return -1;
}

//���û�����������ҵĳ�Ա�����У�������Ƿ���������ͬ��������
int  add_user_to_channel(Channel *_channel, User *_user,char * id) {
	User * lastChannelMember;
	int memberNum = 0;//��¼��Ա����
	if (_channel->firstMember == NULL) { 
		_channel->firstMember = _user; 
		return 0;
	}
	else {
		lastChannelMember = _channel->firstMember;

		while (1) {
			if (!strcmp(lastChannelMember->id, id)) {
				return -1;
			}
			if (lastChannelMember->nextMember != NULL) {
				lastChannelMember = lastChannelMember->nextMember;
				memberNum++;
			}
			else {
				break;
			}
			if (memberNum > 40)return -1;
		}
		lastChannelMember->nextMember = _user;
	}
	return 0;
}
User * delete_user_from_channel(Channel * _channel,const char *_user) {
	//�ҵ���ɾ���û���ǰһ�����û�,�������
	User * preMember, * targetMember;
	//�������,��һ������_user
	if (!strcmp(_channel->firstMember->id, _user)) {
		targetMember = _channel->firstMember;
		_channel->firstMember = targetMember->nextMember;
		*targetMember->id = '\0';
		targetMember->nextMember = NULL;
		targetMember->currentChannel = NULL;
		return targetMember;
	}

	//
	preMember = _channel->firstMember;
	while (preMember->nextMember != NULL) {
		if (!strcmp(preMember->nextMember->id, _user)) {
			//�ҵ���
			targetMember = preMember->nextMember;
			break;
		}
		preMember = preMember->nextMember;
	}

	if (preMember->nextMember == NULL) {
		//û�ҵ�
		return NULL;
	}
	else {
		preMember->nextMember = targetMember->nextMember;
		*targetMember->id = '\0';
		targetMember->nextMember = NULL;
		targetMember->currentChannel = NULL;
		return targetMember;
	}

}

//���ܲ��������Կͻ�����Ϣ
DWORD WINAPI listen_to_client(LPVOID lpParamter)
{
	SOCKADDR_IN addrClient;//����ͻ��˵�������Ϣ
	int len = sizeof(SOCKADDR);
	char _receiveBuff[1416];
	char _sendBuff[1416];
	char temp[32] = "hahaha";
	//���ܻ��õ���
	SOCKET serverSocket = *(SOCKET *)lpParamter;

	//�߳�2�������Կͻ��˵�����
	while (1) {
		int recvDataSize = recvfrom(serverSocket, _receiveBuff, 1416, 0, (SOCKADDR*)&addrClient, &len);
		User * _user;
		Channel *_channel;
		if (recvDataSize != -1) {
			struct Data *data = (Data *)_receiveBuff;
			struct Data *sendMessage;


			int clientCommand = ((Data *)_receiveBuff)->command;
			switch (clientCommand) {
			case _LIST_CHANNEL:
			{
				cout << "�û�Ҫ���г�������" << endl;
				sendMessage = make_message(_LIST_CHANNEL, 0, NULL);
				sendto(serverSocket, (char*)sendMessage, 1416, 0, (SOCKADDR *)&addrClient, len);
				delete sendMessage;
				break;
			}
			case _JOIN_CHANNEL:
			{
				cout << "�û�Ҫ�����������" << endl;
				//��data��,��messageΪ��ʼ��,ǰ31λΪchannelname,��31λΪid
				_channel = find_channel_by_name(data->message);
				if (_channel) {
					//��������������
					if (add_user_to_channel(_channel, data->guid, data->message + _MAX_STRING_LENTH + 1)) {
						//˵�������ظ���
						sendMessage = make_message(_JOIN_CHANNEL, 2, NULL);
						sendto(serverSocket, (char*)sendMessage, 100, 0, (SOCKADDR *)&addrClient, len);
						delete sendMessage;
					}
					else {
						//�����û�����Ӧ����
						_user = data->guid;
						strcpy(_user->id, data->message + _MAX_STRING_LENTH + 1);
						_user->nextMember = NULL;
						_user->currentChannel = _channel;
						//����Ϣ
						sendMessage = make_message(_JOIN_CHANNEL, 1, NULL);
						sendto(serverSocket, (char*)sendMessage, 100, 0, (SOCKADDR *)&addrClient, len);
						delete sendMessage;
					}
				}
				else {
					//���û�����������
					sendMessage = make_message(_JOIN_CHANNEL, 0, NULL);
					sendto(serverSocket, (char*)sendMessage, 30, 0, (SOCKADDR *)&addrClient, len);
					delete sendMessage;
					;
				}
				break;
			}
			case _LIST_USER:
			{
				cout << "�û�Ҫ���г��û�" << endl;
				_user = data->guid;
				_channel = _user->currentChannel;
				if (_channel) {
					sendMessage = make_message(_LIST_USER, 1, _channel);
					sendto(serverSocket, (char*)sendMessage, 1416, 0, (SOCKADDR *)&addrClient, len);
					delete sendMessage;
				}
				else {
					//��ǰ�����κ�Ƶ����
					sendMessage = make_message(_LIST_USER, 0, NULL);
					sendto(serverSocket, (char*)sendMessage, 30, 0, (SOCKADDR *)&addrClient, len);
					delete sendMessage;
				}
				break;
			}
			case _PRIVATE_MSG:
				//message�е�һ���ǶԷ�ID���ڶ�����˽������

				cout << "�û�Ҫ��˽��" << endl;
				_user = data->guid;
				_channel = _user->currentChannel;
				strcpy(temp, data->message);
				User * _friend;
				//�ҵ���Ӧ���û�
				_friend = _channel->firstMember;

				while (_friend != NULL) {
					if (!strcmp(_friend->id, temp)) {
						break;
					}
					cout << _friend->id << endl;
					_friend = _friend->nextMember;

				}
				if (_friend != NULL) {
					sendMessage = make_message(_PRIVATE_MSG, 1, data);
					strcpy(sendMessage->message, _user->id);
					sendto(serverSocket, (char*)sendMessage, 1416, 0, (SOCKADDR *)&_friend->clientAddr, len);

				}
				else {
					sendMessage = make_message(_PRIVATE_MSG, 0, NULL);
					sendto(serverSocket, (char*)sendMessage, 1416, 0, (SOCKADDR *)&addrClient, len);

				}
				delete sendMessage;

				break;
			case _LEAVE:
				cout << "�û�Ҫ���뿪������" << endl;
				_user = data->guid;
				_channel = _user->currentChannel;
				if (_channel) {
					//������Ϣ
					sendMessage = make_message(_LEAVE, 1, data);
					sendto(serverSocket, (char*)sendMessage, 28, 0, (SOCKADDR *)&_user->clientAddr, len);
					_user->currentChannel = NULL;
					*_user->id = '\0';
					 delete_user_from_channel(_channel, _user->id);
				}
				else {
					sendMessage = make_message(_LEAVE, 0, data);
					sendto(serverSocket, (char*)sendMessage, 28, 0, (SOCKADDR *)&_user->clientAddr, len);
				}
				delete sendMessage;
				break;
			case _MESSAGE:
				//��data��,��message�����û�������Ϣ
				cout << "�û�������Ϣ" << endl;
				_user = data->guid;
				_channel = _user->currentChannel;
				if (_channel) {
					sendMessage = make_message(_MESSAGE, 1, data);
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
				cout << "���û��ļ��룬Ϊ�����GUIDΪ" << _user << endl;
				delete sendMessage;
				break;

			default:
				cout << "�û����˸�ƨ" << endl;
				break;
			}
		}
	}
}