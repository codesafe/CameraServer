
#include "clientmanager.h"
#include "clients.h"



ClientManager* ClientManager::_instance = NULL;

ClientManager:: ClientManager()
{
	for (int i=0; i< MAX_CLIENT; i++)
	{
		clientlist[i] = NULL;
	}
}

ClientManager::~ClientManager()
{
}

int ClientManager::AddClient(Clients* client)
{
	int pos = FindEmpty();
	if (pos < 0)
	{
		printf("Error not found empty pos\n");
		return -1;
	}

	clientlist[pos] = client;
	return pos;
}

void ClientManager::RemoveClient(Clients* client)
{
	for (int i = 0; i < MAX_CLIENT; i++)
	{
		if (clientlist[i] == client)
		{
			printf("Removed Client.\n");
			clientlist[i] = NULL;
			return;
		}
	}

	printf("Not found for Remove\n");
}

void ClientManager::SendPacket(char packet, char command)
{
	for (int i=0; i< MAX_CLIENT; i++)
	{
		if(clientlist[i] == NULL) continue;
		char data[10];
		//clientlist[i]->sendpacket(i, packet, data, 10);
		//clientlist[i]->sendpacketImmediate(i, packet, data, 10);
		clientlist[i]->sendudp(i, packet, command);
	}
}

int ClientManager::FindEmpty()
{
	for (int i = 0; i < MAX_CLIENT; i++)
	{
		if (clientlist[i] == NULL) return i;
	}

	return -1;
}