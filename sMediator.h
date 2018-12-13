#pragma once
#include "imediator.h"
#include "IOCPServer.h"
#include "Packdef.h"
#include "CMySql.h"
#include <time.h>
class CTCPMediator;

typedef void (CTCPMediator::* PFUN)(MySocket *pSockex,char* szbuf);



class CTCPMediator :
	public CMediator
{
public:
	CTCPMediator(void);
	~CTCPMediator(void);
public:
	virtual bool Open();
	virtual void Close();
	virtual bool SendData(MySocket *pSockex,char* szbuf,int nLen);
	virtual bool DealData(MySocket *pSockex,char* szbuf);
public:
	void DealRegisterRq(MySocket *pSockex,char* szbuf);
	void DealLoginRq(MySocket *pSockex,char* szbuf);
	void DealUpLoadRq(MySocket *pSockex,char* szbuf);
	void DealUpLoadFileBlockRq(MySocket *pSockex,char* szbuf);
	void DealDownLoadRq(MySocket *pSockex,char* szbuf);
	void DealDownLoadFileBlockRq(MySocket *pSockex,char* szbuf);
	void DealDownLoadFileBlockRs(MySocket *pSockex,char* szbuf);
	void DealShareLinkRq(MySocket *pSockex,char* szbuf);
	void DealGetLinkRq(MySocket *pSockex,char* szbuf);
	void DealPersonalLetterRq(MySocket *pSockex,char* szbuf);
	void DealQuit(MySocket *pSockex,char* szbuf);
	void DealGetFileListRq(MySocket *pSockex,char* szbuf);
	void DealDeleteFileRq(MySocket *pSockex,char* szbuf);
	void GetSelectFileInfo(MySocket *pSockex,INT64 nUserid,INT64 nFileid);
private:
	CNet* m_pTcpNet;
	PFUN m_pProtocolMap[MAX_SOCKETMAP_NUM];
	CMySql  m_mysql;
	char  m_szSystemPath[MAX_PATH];
public:
	
size_t SDBMHash(const char *str)  
{  
    register size_t hash = 0;  
    while (size_t ch = (size_t)*str++)  
    {  
        hash = 65599 * hash + ch;         
        //hash = (size_t)ch + (hash << 6) + (hash << 16) - hash;  
    }  
    return hash;  
}  

INT64 GetFileID(size_t hashkey,INT64 userid)
{

	return (hashkey<<32)+ (DWORD)userid;
}

INT64  GetLinkCode(INT64 nUserId)
{
	DWORD dwCurrentTime = (DWORD)time(NULL);

	return dwCurrentTime*nUserId;
}


public:
typedef struct FileInfo
{
	
	INT64    m_nUserId; //用户ID
	INT64    m_nFileSize;
	INT64    m_nfileID;
	char     m_szFileName[MAX_PATH];
	char     m_szFileMD5[MAX_SIZE];
	char     m_fileType[MAX_TYPE_SIZE];
	char    m_szFileUpTime[MAX_SIZE];
	char    m_szFileLocalPath[MAX_PATH];  //系统存储路径
	FILE    *m_pfile;
	INT64   m_nUpLoadSize; 
   

}FileInfo;


typedef struct FileDownLoadInfo
{
	
	INT64    m_nUserId; //用户ID
	INT64    m_nFileSize;
	INT64    m_nfileID;
	FILE    *m_pfile;
	INT64   m_nDownLoadSize; 
	char    m_szFileName[MAX_PATH];
   

}FileDownLoadInfo;

typedef struct UserInfo
{
	INT64 Userid;
	long  lUserIp;
	bool  bUserState;
	MySocket *pSock;
	//char  szLoginTime[MAX_PATH];

}UserInfo;

public:
map<INT64 , UserInfo*> m_mapUserIdToUserInfo;	
map<INT64 , FileInfo*> m_mapIdToFileInfo;

map<INT64 , FileDownLoadInfo*> m_mapIdToFileDownLoadInfo;

map<INT64 ,STRU_PERSONALLETTER_RQ*> m_mapUserIdToPersonalLetterInfo;
};

