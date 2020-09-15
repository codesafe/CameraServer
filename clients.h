#pragma once

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
	int		totalsize;		// ��ü ����
	int		currentsize;	// �������� ���� ��ġ (��/���� ���)
	char	buffer[SOCKET_BUFFER];
	SocketBuffer()
	{
		totalsize = 0;
		currentsize = 0;
		memset(buffer, 0, SOCKET_BUFFER);
	}
};


class Clients
{
public:
	Clients();
	~Clients();

	void Start(ClientInfo* info);

	static unsigned int __stdcall ThreadHandler(void* pParam);
	static void UpdateSocket(int clientnum);
	static void recvdone(int clientnum);
	static bool sendpacket(int clientnum, char packet, char* data, int datasize);

private:

	static SOCKET	socket[MAX_CLIENT];
	static SocketBuffer sendbuffer[MAX_CLIENT];
	static SocketBuffer recvbuffer[MAX_CLIENT];
	static std::deque<SocketBuffer>	recvbufferlist[MAX_CLIENT];

	static CRITICAL_SECTION sendlock[MAX_CLIENT];

};
