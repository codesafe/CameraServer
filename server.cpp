#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include <winsock.h>
#include <iostream>

#include "packet.h"
#include "logger.h"
#include "clients.h"
#include "accept.h"
#include "clientmanager.h"
#include "commander.h"

#pragma comment (lib, "ws2_32.lib")


bool getInput(char* c)
{
	if (_kbhit())
	{
		*c = _getch();
		return true;
	}
	return false;
}

int main()
{
	Accept accepter;
	accepter.Start();

	while(1)
	{
		char key = ' ';
		while (!getInput(&key))
		{
			bool ret = true;
			while (ret)
			{
				Command com;
				ret = Commander::instance()->getcommand(com);

				if (com.packet == PACKET_UPLOAD_PROGRESS)
				{
					int value = (int&) * (com.data);
					printf("Upload progress %d : %d\n", com.camnum, value);
				}
				else if (com.packet == PACKET_UPLOAD_DONE)
				{
					printf("Upload done %d\n", com.camnum);
				}
			}

			Sleep(1);
		}

		printf("Input = %d\n", key);
		if (key == '0')
		{
			ClientManager::instance()->SendPacket(PACKET_HALFPRESS, 0);
		}
		else if (key == '1')
		{
			ClientManager::instance()->SendPacket(PACKET_SHOT, 1);
		}
	}

	return 0;
}
