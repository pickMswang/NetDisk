#ifndef INET_H
#define INET_H

struct MySocket;

class CNet
{
public:
	CNet() {};
	virtual ~CNet() {};
public:
	virtual bool InitNetWork() = 0;
	virtual void UnInitNetWork() = 0;
	virtual bool SendData(MySocket *pSockex,char* szbuf,int nLen) = 0;
public:
	static long GetVaildIp()
	{
		in_addr addr;
		char szHostName[MAX_PATH] = {0};
		hostent* pHostIpList = NULL;
		if(!gethostname(szHostName,MAX_PATH))
		{
			pHostIpList = gethostbyname(szHostName);
			if(pHostIpList->h_addr_list[0] != 0)
			{
				addr.S_un.S_addr = *(ULONG*)pHostIpList->h_addr_list[0];
			}
		}

		return addr.S_un.S_addr;
	}
};



#endif
