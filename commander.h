#ifndef _COMMANDER_
#define _COMMANDER_

#include <process.h>
#include <windows.h>
#include <vector>

#define SOCKET_BUFFER 4096

struct Command
{
	int	camnum;
	char packet;
	char data[SOCKET_BUFFER];

	Command()
	{
		camnum = -1;
		packet = 0;
		memset(data, 0, SOCKET_BUFFER);
	}
};



class Commander
{
public:
	static Commander* instance()
	{
		if (_instance == NULL)
			_instance = new Commander();
		return _instance;
	}

	void	addcommand(int camnum, char packet, char* data, int datalen)
	{
		EnterCriticalSection(&lock);

		Command com;
		com.camnum = camnum;
		com.packet = packet;
		memcpy(com.data, data, datalen);
		commandlist.push_back(com);

		LeaveCriticalSection(&lock);
	}

	bool getcommand(Command &com)
	{
		EnterCriticalSection(&lock);
		if (commandlist.empty())
		{
			LeaveCriticalSection(&lock);
			return false;
		}
		com.camnum = commandlist[0].camnum;
		com.packet = commandlist[0].packet;
		memcpy(com.data, commandlist[0].data, SOCKET_BUFFER);
		commandlist.erase(commandlist.begin());

		LeaveCriticalSection(&lock);
		return true;
	}


private:
	Commander()
	{
		InitializeCriticalSection(&lock);
	}

	~Commander() {}

	static Commander* _instance;

	CRITICAL_SECTION lock;
	std::vector<Command>	commandlist;
};




#endif