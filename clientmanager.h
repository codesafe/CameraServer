#pragma once

#include <vector>

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


	void AddClient(Clients* client);
	void SendPacket(int packet);

private:
	ClientManager();
	~ClientManager();

	static ClientManager* _instance;

	std::vector< Clients*> clientlist;

	
};
