#include <stdio.h>
#include <conio.h>

#include "predef.h"
#include "udpsocket.h"

UDP_Socket::UDP_Socket()
{
	sock = -1;
}

UDP_Socket::~UDP_Socket()
{
	if (sock != -1)
		closesocket(sock);
}

void UDP_Socket::init(int camnum, std::string addr)
{
	sock = socket(AF_INET, SOCK_DGRAM, 0);

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		printf("sock failed\n");
		return;
	}

	memset(&servAddr, 0, sizeof(servAddr));

	servAddr.sin_family = AF_INET;
	//servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_addr.s_addr = inet_addr(addr.c_str());
	servAddr.sin_port = htons(SERVER_UDP_PORT);
	servAddr.sin_port = htons(SERVER_UDP_PORT + camnum);

// 	if (bind(sock, (struct sockaddr*) & servAddr, sizeof(servAddr)) == -1)
// 	{
// 		printf("bind failed\n");
// 		return;
// 	}
}

void UDP_Socket::update()
{
	char buf[UDP_BUFFER];

	int client_addr_size = sizeof(clntAddr);
	//recvfrom(sock, buf, UDP_BUFFER, 0, (struct sockaddr*) & clntAddr, (socklen_t*)&client_addr_size);


}

void UDP_Socket::send(char* buf, int bufsize)
{
	int sentsize = sendto(sock, buf, bufsize, 0, (struct sockaddr*) & servAddr, sizeof(servAddr));

	if (sentsize != bufsize)
	{
		printf("sendto failed.\n");
		return;
	}
}