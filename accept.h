
#pragma once

class Accept
{
public:
	Accept();
	~Accept();

	void Start();
	static unsigned int __stdcall ThreadHandler(void* pParam);

private:
	
	static int clientnum;
	static SOCKET serversock;
	struct sockaddr_in serv_addr;

};