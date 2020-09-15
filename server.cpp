
#include <Windows.h>
#include <winsock.h>
#include <iostream>

#include "logger.h"
#include "clients.h"

#define PORT 8888
#define SOCKET_BUFFER 4096

#pragma comment (lib, "ws2_32.lib")

int main()
{
	int clientnum = 0;

	SOCKET serversock;
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		Log::getInstance()->log("error\r\n");

	serversock = socket(AF_INET, SOCK_STREAM, 0);
	if (serversock == INVALID_SOCKET)
		Log::getInstance()->log("ERROR opening socket\n");

	ZeroMemory(&serv_addr, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(PORT);

	int status = bind(serversock, (struct sockaddr*) & serv_addr, sizeof(struct sockaddr_in));
	if (status == SOCKET_ERROR)
		Log::getInstance()->log("Bind Error\n");

	listen(serversock, 5);
	Log::getInstance()->log("Socket created!\n");



	while (1)
	{
		ZeroMemory(&cli_addr, sizeof(struct sockaddr_in));
		int sockLen = sizeof(struct sockaddr_in);
		SOCKET clisock = accept(serversock, (struct sockaddr*) & cli_addr, &sockLen);

		if (clisock == INVALID_SOCKET)
		{
			Log::getInstance()->log("Accept Error");
			closesocket(serversock);
			//WSACleanup();
			continue;
		}

		// client socket --> nonblock
		unsigned long arg = 1;
		ioctlsocket(clisock, FIONBIO, &arg);
		Log::getInstance()->log("new Client !!");

		ClientInfo info;
		info.clientnum = clientnum++;
		info.sock = clisock;

		Clients* client = new Clients();
		client->Start(&info);

		Sleep(100);
	}

	return 0;
}
