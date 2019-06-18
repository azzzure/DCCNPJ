# DCCNPJ

复旦大学 数据通信与计算机网络 期末PJ

实现一个基于UDP的网络聊天室，要求如下：

ChatServer（服务方）程序

该聊天室服务方程序要求能够支持如下命令（供服务方管理员使用，所有命令以/开始。
  1./openchannel channelName
  开通新的聊天室

  2./channels 
  列出服务方所有开通的聊天室。

  3./enterchannel channelName 
  进入到某个聊天室channel中。这时可以用list，kickout userid命令。直到使用leave命令离开该聊天室。

  4./list 
  列出用户当前所在聊天室的所有用户列表。

  5./kickout userid 
  将当前所在聊天室的某个活跃用户踢出该聊天室，并且向该聊天室中的所有活跃用户发送将某个用户踢出该聊天室的消息。 

  6./leave
  离开聊天室。

  7. /closechannel channelname
  关闭某个的聊天室，并且向该聊天室中的所有活跃用户发送关闭该聊天室的消息。

ChatClient（客户方）程序

该聊天室客户方程序要求能够支持如下命令（所有命令以/开始）和服务方程序交互：
  1./channels
  列出服务方当前活跃的聊天室。

  2./join channel username
  以username为名加入到聊天室channel中，服务方给该用户分配一个唯一标识，以后的聊天应该包含该ID。在加入聊天室后，用户发送的任何信息（控制信息除外，它 由服务方解析来执行相应的功能）将被其他聊天室的用户收到，注意这些普通的聊天信息允许丢失，不需要采取可靠的传输机制。

  3./list
  列出用户当前所在聊天室的所有用户列表。

  4./msg userid your_message_here 
  给用户发送私人聊天的信息（其他用户不会收到）。

  5./leave
  离开聊天室。



