
#include <process.h>
#include <windows.h>
#include <winsock.h>
#include "clients.h"
#include "logger.h"


SOCKET	Clients::socket[MAX_CLIENT] = { -1, };
SocketBuffer Clients::sendbuffer[MAX_CLIENT];
SocketBuffer Clients::recvbuffer[MAX_CLIENT];
std::deque<SocketBuffer> Clients::recvbufferlist[MAX_CLIENT];

CRITICAL_SECTION Clients::sendlock[MAX_CLIENT];

Clients::Clients()
{

}

Clients::~Clients()
{

}

void Clients::Start(ClientInfo *info)
{
	socket[info->clientnum] = -1;
	InitializeCriticalSection(&sendlock[info->clientnum]);

	_beginthreadex(NULL, 0, ThreadHandler, (void*)info, 0, NULL);

}

unsigned int __stdcall Clients::ThreadHandler(void* pParam)
{
	ClientInfo *info = (ClientInfo*)pParam;
	int clientnum = info->clientnum;
	socket[clientnum] = info->sock;

	unsigned long arg = 1;
	if (ioctlsocket(socket[clientnum], FIONBIO, &arg) != 0) return -1;

	while (1)
	{
		UpdateSocket(clientnum);

		Sleep(10);
	}

	return 0;
}

void Clients::UpdateSocket(int clientnum)
{
	fd_set read_flags, write_flags;
	struct timeval waitd;          // the max wait time for an event
	int sel;
	SOCKET clisock = socket[clientnum];

	waitd.tv_sec = 0;
	waitd.tv_usec = 0;
	FD_ZERO(&read_flags);
	FD_ZERO(&write_flags);
	FD_SET(clisock, &read_flags);

	sel = select(clisock + 1, &read_flags, &write_flags, (fd_set*)0, &waitd);
	if (FD_ISSET(clisock, &read_flags))
	{
		FD_CLR(clisock, &read_flags);

		char in[SOCKET_BUFFER];
		memset(&in, 0, sizeof(in));
		int recvsize = recv(clisock, in, SOCKET_BUFFER, 0);
		if (recvsize > 0)
		{
			if (recvbuffer[clientnum].totalsize > 0)
			{
				// �ڿ� �̾� �޾ƾ���
				memcpy(recvbuffer[clientnum].buffer + recvbuffer[clientnum].totalsize, in, recvsize);
				recvbuffer[clientnum].totalsize += recvsize;
				recvdone(clientnum);
			}
			else
			{
				// ó�� ����
				recvbuffer[clientnum].totalsize = recvsize;
				memcpy(recvbuffer[clientnum].buffer, in, recvsize);
				recvdone(clientnum);
			}


		}
		else
		{
			// disconnected
			closesocket(clisock);
			clisock = -1;
			Log::getInstance()->log("Client disconnected!!");
		}
	}


	// �������� ������ �����ٷ� ����
	if (sendbuffer[clientnum].totalsize > 0)
		FD_SET(clisock, &write_flags);

	// ����
	if (FD_ISSET(clisock, &write_flags))
	{
		FD_CLR(clisock, &write_flags);
		int sendsize = ::send(clisock, sendbuffer[clientnum].buffer + sendbuffer[clientnum].currentsize, sendbuffer[clientnum].totalsize - sendbuffer[clientnum].currentsize, 0);
		if (sendbuffer[clientnum].totalsize == sendbuffer[clientnum].currentsize + sendsize)
		{
			sendbuffer[clientnum].totalsize = -1;
			sendbuffer[clientnum].currentsize = 0;
			memset(sendbuffer[clientnum].buffer, 0, SOCKET_BUFFER);
		}
		else
		{
			sendbuffer[clientnum].currentsize += sendsize;
		}
	}

}

void	Clients::recvdone(int clientnum)
{
	while (1)
	{
		if (recvbuffer[clientnum].totalsize >= sizeof(int) + sizeof(char))	// data size + packet
		{
			int datasize = (int&)*recvbuffer[clientnum].buffer;
			if (recvbuffer[clientnum].totalsize >= sizeof(int) + sizeof(char) + datasize)
			{
				SocketBuffer buffer;
				buffer.totalsize = sizeof(int) + sizeof(char) + datasize;
				memcpy(buffer.buffer, recvbuffer[clientnum].buffer, buffer.totalsize);
				recvbufferlist[clientnum].push_back(buffer);

				recvbuffer[clientnum].totalsize -= buffer.totalsize;

				// �����ִ°� �ִ°�?
				if (recvbuffer[clientnum].totalsize > 0)
				{
					char tempbuffer[SOCKET_BUFFER] = { 0, };
					memcpy(tempbuffer, recvbuffer[clientnum].buffer + buffer.totalsize, recvbuffer[clientnum].totalsize);
					memcpy(recvbuffer[clientnum].buffer, tempbuffer, SOCKET_BUFFER);
				}
			}
			else
				break;
		}
		else
			break;
	}
}

bool	Clients::sendpacket(int clientnum, char packet, char* data, int datasize)
{
	if (socket[clientnum] == -1) return false;

	EnterCriticalSection(&sendlock[clientnum]);

	sendbuffer[clientnum].currentsize = 0;
	sendbuffer[clientnum].totalsize = sizeof(int) + sizeof(char) + datasize;

	memcpy(sendbuffer[clientnum].buffer, (void*)&datasize, sizeof(int));
	memcpy(sendbuffer[clientnum].buffer + sizeof(int), &packet, sizeof(char));
	memcpy(sendbuffer[clientnum].buffer + sizeof(int) + sizeof(char), data, datasize);

	LeaveCriticalSection(&sendlock[clientnum]);

	return true;
}