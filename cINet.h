#pragma once
#include <WinSock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib,"Ws2_32.lib")
#define  _DEF_SERVER_IP     "192.168.1.251"
#define  _DEF_PORT      1234
class CMediator;
class CNet
{
public:
	CNet(CMediator *pMediator);
	~CNet(void);
public:
	bool Connect(char* szip =_DEF_SERVER_IP,short nPort = _DEF_PORT);
	void UnConnect();

	bool SendData(char *szbuf,int nlen);
	static unsigned _stdcall ThreadProc( void * );
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
private:
	SOCKET m_sockClient;
	HANDLE m_hThread;
	bool   m_bFlagQuit;
	CMediator *m_pMediator;

};

