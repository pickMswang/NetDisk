#pragma once

#include "Packdef.h"
#include "Net.h"
#include "MyDlgFile.h"
#include <map>
using namespace std;
class CMediator;
typedef void (CMediator::*PFUN)(char*);

struct FileInfo
{
	INT64 fileId;
	INT64 filePos;
	FILE *pFile;
	INT64  filesize;
	char m_szFileName[MAX_PATH];
	char m_szFilePath[MAX_PATH];
	char m_szUpTime[MAX_PATH];
};
class CMediator
{
public:
	CMediator(void);
	~CMediator(void);
public:
	bool  Open();
	void  Close();
    bool  SendData(char *szbuf,int nlen);
	void  DealData(SOCKET sock, char* szbuf);
	void  DealRegisterRs(char *szbuf);
	void  DealLoginRs(char* szbuf);
	void  DealGetFileListRs(char* szbuf);
	void  DealUpLoadFileRs(char* szbuf);
	void  DealUpLoadFileBlockRq(char* szbuf);
	void  DealUpLoadFileBlockRs(char* szbuf);
	void  DealDownLoadFilekRs(char* szbuf);
	void  DealDownLoadFilekBlockRq(char* szbuf); 
	void  DealShareLinkRS(char* szbuf);
	void  DealGetLinkRS(char* szbuf);
	void  DealDeleteFileRS(char* szbuf);
private:
	CNet *m_pNet;
	PFUN m_ProtocolMap[MAX_SOCKETMAP_NUM];
public:
	CNet *GetTCPNet()
	{
		return m_pNet;
	}

	map<CString,FileInfo*> m_mapFileNameToFileInfo;
	map<INT64,FileInfo*> m_mapFileIdToFileInfo;
	map<INT64,FileDownLoadInfo*> m_mapFileIdToFileDownLoadInfo;
};

