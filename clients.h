#pragma once

#include <Windows.h>
#include <winsock.h>
#include <deque>

#define MAX_CLIENT		8
#define SOCKET_BUFFER	4096

struct ClientInfo
{
	int clientnum;
	SOCKET sock;

};

struct SocketBuffer
{
	int		totalsize;		// 전체 길이
	int		currentsize;	// 진행중인 버퍼 위치 (송/수신 모두)
	char	buffer[SOCKET_BUFFER];
	SocketBuffer()
	{
		totalsize = 0;
		currentsize = 0;
		memset(buffer, 0, SOCKET_BUFFER);
	}
};

#if 0

class Network;

class Clients
{
public:
	Clients();
	~Clients();

	void Start(ClientInfo* info);

	static unsigned int __stdcall ThreadHandler(void* pParam);

private:

	static Network network[MAX_CLIENT];

}


#else

class Clients
{
public:
	Clients();
	~Clients();

	void Start(ClientInfo* info);

	static unsigned int __stdcall ThreadHandler(void* pParam);
	static int UpdateSocket(int clientnum);
	static void recvdone(int clientnum);
	static bool sendpacket(int clientnum, char packet, char* data, int datasize);

	static bool	recvpacket(int clientnum, SocketBuffer* buffer);
	static void	parsepacket(int clientnum, SocketBuffer* buffer);

private:
	static SOCKET	socket[MAX_CLIENT];
	static SocketBuffer sendbuffer[MAX_CLIENT];
	static SocketBuffer recvbuffer[MAX_CLIENT];
	static std::deque<SocketBuffer>	recvbufferlist[MAX_CLIENT];

	static CRITICAL_SECTION sendlock[MAX_CLIENT];

};

#endif