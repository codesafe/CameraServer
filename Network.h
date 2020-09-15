#include <Windows.h>
#include <winsock.h>
#include <deque>
#include "logger.h"

#define PORT 8888
#define SOCKET_BUFFER 4096


// packet
#define RESPONSE_OK		0x05
#define RESPONSE_FAIL	0x06

#define CLIENT_LOG_INFO		0x0a
#define CLIENT_LOG_WARN		0x0b
#define CLIENT_LOG_ERR		0x0c

#define FORCEPATCH			0x0A
#define DEVICERESET			0x10
#define	PLAYMOTION			0x20

#define WHEEL_FORWARD		0x30
#define WHEEL_BACKWARD		0x31
#define WHEEL_STOP			0x32
#define WHEEL_TURNLEFT		0x33
#define WHEEL_TURNRIGHT		0x34

#define DISPLAY_PIC			0x40



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


class Network
{
public:
	Network()
    {
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
			Log::getInstance()->log("error\r\n");

		serversock = socket(AF_INET, SOCK_STREAM, 0);
		if (serversock == INVALID_SOCKET)
			Log::getInstance()->log("ERROR opening socket");

		ZeroMemory(&serv_addr, sizeof(struct sockaddr_in));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(PORT);

		int status = bind(serversock, (struct sockaddr*)&serv_addr, sizeof(struct sockaddr_in));
		if (status == SOCKET_ERROR)
			Log::getInstance()->log("Bind Error\n");

		listen(serversock, 5);
		Log::getInstance()->log("Socket created!");

		clisock = -1;
    }

    ~Network()
    {
    }

    void run()
    {
		if (clisock == -1)
		{
			ZeroMemory(&cli_addr, sizeof(struct sockaddr_in));
			int sockLen = sizeof(struct sockaddr_in);
			clisock = accept(serversock, (struct sockaddr*)&cli_addr, &sockLen);
			if (clisock == INVALID_SOCKET)
			{
				Log::getInstance()->log("Accept Error");
				closesocket(serversock);
				//WSACleanup();
				return;
			}

			// client socket --> nonblock
			unsigned long arg = 1;
			if (ioctlsocket(clisock, FIONBIO, &arg) != 0) return;
			Log::getInstance()->log("new Client !!");
		}
		else
		{

			fd_set read_flags, write_flags;
			struct timeval waitd;          // the max wait time for an event
			int sel;

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
					if (recvbuffer.totalsize > 0)
					{
						// 뒤에 이어 받아야함
						memcpy(recvbuffer.buffer+recvbuffer.totalsize, in, recvsize);
						recvbuffer.totalsize += recvsize;
						recvdone();
					}
					else
					{
						// 처음 받음
						recvbuffer.totalsize = recvsize;
						memcpy(recvbuffer.buffer, in, recvsize);
						recvdone();
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


			// 보낼것이 있으면 보낸다로 설정
			if (sendbuffer.totalsize > 0)
				FD_SET(clisock, &write_flags);

			// 보냄
			if (FD_ISSET(clisock, &write_flags))
			{
				FD_CLR(clisock, &write_flags);
				int sendsize = ::send(clisock, sendbuffer.buffer + sendbuffer.currentsize, sendbuffer.totalsize - sendbuffer.currentsize, 0);
				if (sendbuffer.totalsize == sendbuffer.currentsize + sendsize)
				{
					sendbuffer.totalsize = -1;
					sendbuffer.currentsize = 0;
					memset(sendbuffer.buffer, 0, SOCKET_BUFFER);
				}
				else
				{
					sendbuffer.currentsize += sendsize;
				}
			}

		}


		// parse recvpacket
		if (!recvbufferlist.empty())
		{
			for (size_t i = 0; i < recvbufferlist.size(); i++)
			{
				parse(&recvbufferlist[i]);
			}
			recvbufferlist.clear();
		}
    }

	void	recvdone()
	{
		while (1)
		{
			if (recvbuffer.totalsize >= sizeof(int) + sizeof(char))	// data size + packet
			{
				int datasize = (int&)*recvbuffer.buffer;
				if (recvbuffer.totalsize >= sizeof(int) + sizeof(char) + datasize)
				{
					SocketBuffer buffer;
					buffer.totalsize = sizeof(int) + sizeof(char) + datasize;
					memcpy(buffer.buffer, recvbuffer.buffer, buffer.totalsize);
					recvbufferlist.push_back(buffer);
					
					recvbuffer.totalsize -= buffer.totalsize;

					// 남아있는게 있는가?
					if (recvbuffer.totalsize > 0)
					{
						char tempbuffer[SOCKET_BUFFER] = { 0, };
						memcpy(tempbuffer, recvbuffer.buffer+buffer.totalsize, recvbuffer.totalsize);
						memcpy(recvbuffer.buffer, tempbuffer, SOCKET_BUFFER);
					}
				}
				else
					break;
			}
			else
				break;
		}
	}

	bool	sendpacket(char packet, char *data, int datasize)
	{
		if (clisock == -1) return false;

		sendbuffer.currentsize = 0;
		sendbuffer.totalsize = sizeof(int) + sizeof(char) + datasize;

		memcpy(sendbuffer.buffer, (void*)&datasize, sizeof(int));
		memcpy(sendbuffer.buffer+sizeof(int), &packet, sizeof(char));
		memcpy(sendbuffer.buffer+sizeof(int)+sizeof(char), data, datasize);

		return true;
	}

	void parse(SocketBuffer *recvbuf)
	{
		char packet = (char&)*(recvbuf->buffer + sizeof(int));
		int datapos = sizeof(char) + sizeof(int);

		switch (packet)
		{
			case RESPONSE_OK :
			{
				Log::getInstance()->log(log);
			}
			break;

			case CLIENT_LOG_INFO:
			{
				Log::getInstance()->clientlog(log);
			}
			break;
			case CLIENT_LOG_WARN:
			{
				Log::getInstance()->clientlog(log);
			}
			break;
			case CLIENT_LOG_ERR:
			{
				Log::getInstance()->clientlog(log);
			}
			break;

			default :
			{
				Log::getInstance()->clientlog(log);
			}
		}
	}


private:

	std::deque<SocketBuffer>	recvbufferlist;
	SocketBuffer sendbuffer;
	SocketBuffer recvbuffer;

	
	SOCKET serversock;
	SOCKET clisock;
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
};
