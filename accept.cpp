
#include <process.h>
#include <winsock.h>
#include "accept.h"
#include "logger.h"
#include "clients.h"
#include "clientmanager.h"

#define PORT 8888
#define SOCKET_BUFFER 4096


SOCKET Accept::serversock = -1;
int Accept::clientnum = 0;

Accept::Accept()
{
	clientnum = 0;
}

Accept::~Accept()
{}

void Accept::Start()
{
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

	_beginthreadex(NULL, 0, ThreadHandler, (void*)NULL, 0, NULL);

}


unsigned int __stdcall Accept::ThreadHandler(void* pParam)
{

	struct sockaddr_in cli_addr;

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

		ClientInfo info;
		info.clientnum = clientnum++;
		info.sock = clisock;
		Log::getInstance()->log("New Client !!\n");


		Clients* client = new Clients();
		client->Start(&info);

		ClientManager::instance()->AddClient(client);
	}

	return 0;
}
