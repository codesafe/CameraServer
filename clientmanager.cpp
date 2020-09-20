
#include "clientmanager.h"
#include "clients.h"



ClientManager* ClientManager::_instance = NULL;

ClientManager:: ClientManager()
{
}

ClientManager::~ClientManager()
{
}

void ClientManager::AddClient(Clients* client)
{
	clientlist.push_back(client);
}

void ClientManager::SendPacket(int packet)
{
	for (int i=0; i< (int)clientlist.size(); i++)
	{
		char data[10];
		clientlist[i]->sendpacket(i, packet, data, 10);
	}
}