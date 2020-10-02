#pragma once

#include <vector>
#include "predef.h"

class Clients;
class ClientManager
{
public:
	static ClientManager* instance()
	{
		if (_instance == NULL)
			_instance = new ClientManager();

		return _instance;
	}


	int AddClient(Clients* client);
	void RemoveClient(Clients* client);

	void SendPacket(char packet, char command);

private:

	int FindEmpty();

	ClientManager();
	~ClientManager();

	static ClientManager* _instance;

	//std::vector< Clients*> clientlist;
	Clients* clientlist[MAX_CLIENT];
	
};
