#include "stdafx.h"
#include "Net.h"
#include "Mediator.h"

CNet::CNet(CMediator *pMediator)
{
	m_sockClient = NULL;
	m_hThread = nullptr;
	m_bFlagQuit = true;
	m_pMediator = pMediator;
}


CNet::~CNet(void)
{
}
bool CNet::Connect(char* szip ,short nPort)
{
		 //0.选择类型 --- 加载库
	WORD wVersionRequested;
    WSADATA wsaData;
    int err;


    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
    
		UnConnect();
        return false;
    }

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) 
	{
      
       
        UnConnect();
        return false;
    }
  
	//MultiByteToWideChar
	//1.申请个位置 -- socket 套接字 （位置）
	m_sockClient = socket(AF_INET,SOCK_STREAM ,IPPROTO_TCP );
	if(INVALID_SOCKET  == m_sockClient)
	{
		 UnConnect();
        return false;
	}

	sockaddr_in addr;
	InetPton(AF_INET,szip,(PVOID)&addr.sin_addr.S_un.S_addr);
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(nPort);
	//addr.sin_addr.S_un.S_addr = inet_addr(szip);
	if(SOCKET_ERROR == connect(m_sockClient,(const sockaddr*)&addr,sizeof(addr)))
	{
		 UnConnect();
        return false;
	}

	//创建线程
	 m_hThread = (HANDLE)_beginthreadex(nullptr,0,&ThreadProc,this,0,0);
	return true;
}

unsigned _stdcall  CNet::ThreadProc( void * lpvoid)
{
	CNet *pthis = (CNet*)lpvoid;
	//
	char szbuf[_DEFAULT_SIZE] = {0};
	int nRelReadNum;
	int nPackSize = 0;

	while(pthis->m_bFlagQuit)
	{
		//recv --接大小
		recv(pthis->m_sockClient,(char*)&nPackSize,sizeof(int),0);
		int n = GetLastError();
		char  *pbuffer = new char[nPackSize];
		//ZeroMemory(pbuffer,nPackSize);
		int nTempSize = nPackSize;
		int noffset = 0;
		while(nTempSize)
		{
			//接收数据
			nRelReadNum = recv(pthis->m_sockClient,pbuffer+noffset,nTempSize,0);
			if(nRelReadNum >0)
			{
				nTempSize -= nRelReadNum;
				noffset += nRelReadNum;
			

			}
		}
		
		//处理数据
		pthis->m_pMediator->DealData(pthis->m_sockClient,pbuffer);

		delete []pbuffer;
		pbuffer = nullptr;

	}
	return 0;
}
void CNet::UnConnect()
{
	 WSACleanup();

	 if(m_sockClient)
	 {
		 closesocket(m_sockClient);
		 m_sockClient = NULL;
	 }
	 m_bFlagQuit = false;
	 
	 if(m_hThread)
	 {
		 if(WAIT_TIMEOUT ==  WaitForSingleObject(m_hThread,100))
		 {
			 TerminateThread(m_hThread,-1);
		 }
		 CloseHandle(m_hThread);
		 m_hThread = nullptr;
	 }

}
	
bool CNet::SendData(char *szbuf,int nlen)
{
	if(send(m_sockClient,szbuf,nlen,0)<=0 )
	{
		int n = GetLastError();
		return false;
	}

	return true;
}
