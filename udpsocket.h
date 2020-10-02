#pragma once

//#include <sys/socket.h>
//#include <arpa/inet.h> //inet_addr
#include <Windows.h>
#include <winsock.h>

#include <string>

class UDP_Socket
{
public:
	UDP_Socket();
	~UDP_Socket();

	void init(int camnum, std::string addr);
	void update();
	void send(char* buf, int bufsize);


private:

	int	sock;
	struct sockaddr_in servAddr;
	struct sockaddr_in clntAddr;

};