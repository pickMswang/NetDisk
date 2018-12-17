#include "stdafx.h"
#include "Mediator.h"
#include "0804Disk.h"
#include "0804DiskDlg.h"

CMediator::CMediator(void)
{
	m_pNet = new CNet(this);

	m_ProtocolMap[_DEF_PROTOCOL_REGISTER_RS  - _DEF_PROTOCOL_BASE]= &CMediator::DealRegisterRs;
	m_ProtocolMap[_DEF_PROTOCOL_LOGIN_RS  - _DEF_PROTOCOL_BASE]= &CMediator::DealLoginRs;
	m_ProtocolMap[_DEF_PROTOCOL_GETFILELIST_RS  - _DEF_PROTOCOL_BASE]= &CMediator::DealGetFileListRs;
	m_ProtocolMap[_DEF_PROTOCOL_UPLOAD_RS  - _DEF_PROTOCOL_BASE]= &CMediator::DealUpLoadFileRs;
	m_ProtocolMap[_DEF_PROTOCOL_UPLOAD_FILEBLOCK_RS  - _DEF_PROTOCOL_BASE]= &CMediator::DealUpLoadFileBlockRs;
	m_ProtocolMap[_DEF_PROTOCOL_DOWNLOAD_RS  - _DEF_PROTOCOL_BASE]= &CMediator::DealDownLoadFilekRs;
	m_ProtocolMap[_DEF_PROTOCOL_DOWNLOAD_FILEBLOCK_RQ  - _DEF_PROTOCOL_BASE]= &CMediator::DealDownLoadFilekBlockRq;
	m_ProtocolMap[_DEF_PROTOCOL_SHARE_RS  - _DEF_PROTOCOL_BASE]= &CMediator::DealShareLinkRS;
	m_ProtocolMap[_DEF_PROTOCOL_GET_RS  - _DEF_PROTOCOL_BASE]= &CMediator::DealGetLinkRS;
	m_ProtocolMap[_DEF_PROTOCOL_DELETEFILE_RS  - _DEF_PROTOCOL_BASE]= &CMediator::DealDeleteFileRS;

}


CMediator::~CMediator(void)
{
	if(m_pNet)
	{
	//	delete m_pNet;
		m_pNet = nullptr;
	}
}

bool CMediator::Open()
{
	if(!m_pNet->Connect())
	{
		return false;
	}
	return true;
}
void CMediator::Close()
{
	m_pNet->UnConnect();

}

bool CMediator::SendData(char *szbuf,int nlen)
{
	if(!m_pNet->SendData(szbuf,nlen) )
	{
		return false;
	}
	return true;
}

void CMediator::DealData(SOCKET sock, char* szbuf)
{
	int *ptype = (int*)szbuf;

	if(*ptype >= _DEF_PROTOCOL_BASE && *ptype <= _DEF_PROTOCOL_DELETEFILE_RS )
	{
		(this->*m_ProtocolMap[*ptype - _DEF_PROTOCOL_BASE])(szbuf);
	}
	
}


void CMediator::DealRegisterRs(char *szbuf)
{
	STRU_REGISTER_RS *psrr = (STRU_REGISTER_RS *)szbuf;
	char *pResult = nullptr;
	switch (psrr->m_lResult)
	{
	case userid_is_exist:
		pResult = "userid_is_exist";
		break;
	case password_illegal:
		pResult = "password_illegal";
		break;
	case register_sucess:
		pResult = "register_sucess";
		break;
	default:
		break;
	}

	::MessageBox(theApp.m_pMainWnd->m_hWnd,pResult,"温馨提示",MB_OK);
}

void CMediator::DealLoginRs(char* szbuf)
{
	STRU_REGISTERLOGIN_RS *psrr = (STRU_REGISTERLOGIN_RS *)szbuf;
	char *pResult = nullptr;
	switch (psrr->m_lResult)
	{
	case userid_no_exist:
		pResult = "userid_no_exist";
		::MessageBox(theApp.m_pMainWnd->m_hWnd,pResult,"温馨提示",MB_OK);
		break;
	case password_error:
		pResult = "password_error";
		::MessageBox(theApp.m_pMainWnd->m_hWnd,pResult,"温馨提示",MB_OK);
		break;
	case login_sucess:
		{
		  pResult = "login_sucess";
	      theApp.m_pMainWnd->PostMessage(UM_CHANGEMAIN);
		//Sleep(100);
		
		}
		break;
	default:
		break;
	}
}

void   CMediator::DealGetFileListRs(char* szbuf)
{
	STRU_GETFILELIST_RS *psgr = (STRU_GETFILELIST_RS *)szbuf;
    
	((CMyDlgFile*)theApp.m_pMainWnd)->SendMessage(UM_SHOWFILELIST,(WPARAM)psgr,0);


}


void  CMediator::DealUpLoadFileRs(char* szbuf)
{
	//上传文件块回复
	STRU_UPLOAD_RS *psur = (STRU_UPLOAD_RS *)szbuf;
	 auto ite = m_mapFileNameToFileInfo.find(psur->m_szFileName);
	 FileInfo *pFileInfo = ite->second;

	 if(psur->m_lResult == file_uploadrq_sucess)
	 {
		    pFileInfo->fileId = psur->m_nfileId;
		    fopen_s(&pFileInfo->pFile,pFileInfo->m_szFilePath,"rb");
			int n = GetLastError();
			if(psur->m_nUpLoadPos)
			{
				_fseeki64(pFileInfo->pFile,psur->m_nUpLoadPos,0);
			}
			m_mapFileIdToFileInfo[pFileInfo->fileId ] = pFileInfo;
			//todo
			m_mapFileNameToFileInfo.erase(ite);
			//读文件内容，并发送
			DealUpLoadFileBlockRq((char*)pFileInfo);
	 }
	 else if(psur->m_lResult == file_is_exist ||file_uploaded ==  psur->m_lResult )
	 {
		 char *pResult = "file_uploaded";
		 if(psur->m_lResult == file_is_exist)
		 {
			  ::MessageBox(theApp.m_pMainWnd->m_hWnd,"file_is_exist","温馨提示",MB_OK);
		 }
		 
		 delete ite->second;
		 ite->second = nullptr;

		 m_mapFileNameToFileInfo.erase(ite);

	 }

	  
}

void  CMediator::DealUpLoadFileBlockRq(char* szbuf)
{
	FileInfo *pFileInfo = (FileInfo *)szbuf;
	STRU_UPLOAD_FILEBLOCK_RQ sufr;
	sufr.m_nType = _DEF_PROTOCOL_UPLOAD_FILEBLOCK_RQ;
	sufr.m_nFileId = pFileInfo->fileId;
	sufr.m_nUserId = ((CMyDlgFile*)theApp.m_pMainWnd)->m_nUserId;
	INT64 nRelReadNum = fread_s(sufr.m_szFileContent,sizeof(sufr.m_szFileContent),sizeof(char),MAX_CONTENT_LEN,pFileInfo->pFile);
	sufr.m_nBlockLen = nRelReadNum;
	m_pNet->SendData((char*)&sufr,sizeof(sufr));
}

void  CMediator::DealUpLoadFileBlockRs(char* szbuf)
{
	STRU_UPLOAD_FILEBLOCK_RS *psufr = (STRU_UPLOAD_FILEBLOCK_RS *)szbuf;
	auto ite = m_mapFileIdToFileInfo.find(psufr->m_nFileId);
	FileInfo *pInfo = ite->second;
	STRU_UPLOAD_FILEBLOCK_RQ sufr;
	sufr.m_nType = _DEF_PROTOCOL_UPLOAD_FILEBLOCK_RQ;
	sufr.m_nFileId = pInfo->fileId;
	sufr.m_nUserId = ((CMyDlgFile*)theApp.m_pMainWnd)->m_nUserId;
	
	

	if(psufr->m_lResult == fileblock_failed)
	{
		//指针移动上次的位置，重新传
		_fseeki64(pInfo->pFile,pInfo->filePos,0);
	}
	else
	{
		 //如果成功，则传下一个文件块
		pInfo->filePos += psufr->m_nBlockLen;

		if(psufr->m_nBlockLen < MAX_CONTENT_LEN||pInfo->filePos == pInfo->filesize)
		{
			fclose(pInfo->pFile);
		    
			auto ite = m_mapFileIdToFileInfo.find(pInfo->fileId);
			if(ite != m_mapFileIdToFileInfo.end())
			{
				STRU_GETFILELIST_RS sgr;
				sgr.m_nFileSize = pInfo->filesize;
				strcpy_s(sgr.m_szFileName,MAX_PATH,pInfo->m_szFileName);
				strcpy_s(sgr.m_szFileUpTime,sizeof(sgr.m_szFileUpTime),pInfo->m_szUpTime);
			    ((CMyDlgFile*)theApp.m_pMainWnd)->SendMessage(UM_SHOWFILELIST,(WPARAM)&sgr,0);
				delete ite->second;
				ite->second = nullptr;
				m_mapFileIdToFileInfo.erase(ite);
			}
			return;
		}                     

	}



	sufr.m_nBlockLen = fread_s(sufr.m_szFileContent,sizeof(sufr.m_szFileContent),sizeof(char),MAX_CONTENT_LEN,pInfo->pFile);

	m_pNet->SendData((char*)&sufr,sizeof(sufr));


}


void   CMediator::DealDownLoadFilekRs(char* szbuf)
{
	STRU_DOWNLOAD_RS *psdr = (STRU_DOWNLOAD_RS *)szbuf;
	//auto ite = m_mapFileIdToFileInfo.find(psdr->m_nFileId);
	CMyDlgFile *pDlg =   (CMyDlgFile*)theApp.m_pMainWnd;

	auto ite = pDlg->m_mapFileNameToPath.find(psdr->m_szFileName);
	FileDownLoadInfo *pInfo = ite->second;
	//如果收到下载回复
	if(psdr->m_lResult == file_downrq_success)
	{
		pInfo->m_nFileId = psdr->m_nFileId;
		m_mapFileIdToFileDownLoadInfo[psdr->m_nFileId] = pInfo;
		//如果下载请求成功
		//sprintf_s(pInfo->m_szFilePath,"%s\\%s",pInfo->m_szFilePath,psdr->m_szFileName);
		fopen_s(&pInfo->m_pFile,pInfo->m_szFilePath,"wb");
	}
	else if(psdr->m_lResult == file_downrq_failed)
	{
		//失败
		delete ite->second;
		ite->second = nullptr;
		pDlg->m_mapFileNameToPath.erase(ite);
	}

}


void  CMediator::DealDownLoadFilekBlockRq(char* szbuf)
{
	//下载文件块请求---则接收文件，并且发送回复
	STRU_UPLOADDOWNLOAD_FILEBLOCK_RQ *psufr = (STRU_UPLOADDOWNLOAD_FILEBLOCK_RQ *)szbuf;
	auto ite = m_mapFileIdToFileDownLoadInfo.find(psufr->m_nFileId);
	FileDownLoadInfo *pInfo = ite->second;
	STRU_UPLOAD_FILEBLOCK_RS sufr;
	sufr.m_nType = _DEF_PROTOCOL_DOWNLOAD_FILEBLOCK_RS;
	//FileInfo *pFileInfo =  m_mapIdToFileInfo[psufr->m_nFileId];
	if(ite->second == NULL)
	{
		//失败
		sufr.m_lResult = fileblock_failed;

	}
	else
	{
		if(psufr->m_nBlockLen !=0)
		{
			fwrite(psufr->m_szFileContent,sizeof(char),psufr->m_nBlockLen,pInfo->m_pFile);
		    sufr.m_lResult = fileblock_success;
			pInfo->m_nDownPos += psufr->m_nBlockLen;
		}
		
		
		//文件尾
		if(psufr->m_nBlockLen < MAX_CONTENT_LEN||pInfo->m_nDownPos == pInfo->m_nFileSize)
		{
			 fclose(ite->second->m_pFile);
			 //将数据更新到数据库中


			 delete ite->second;
			 ite->second = nullptr;
			 m_mapFileIdToFileDownLoadInfo.erase(ite);

		}
	}

	

	//回复
	
	sufr.m_nUserId = psufr->m_nUserId;
	sufr.m_nFileId = psufr->m_nFileId;
	//sufr.m_nBlockId = psufr->m_nBlockId;
	sufr.m_nBlockLen = psufr->m_nBlockLen;

	
	m_pNet->SendData((char*)&sufr,sizeof(sufr));

}

void  CMediator::DealShareLinkRS(char* szbuf)
{
	STRU_SHARELINK_RS *pssr = (STRU_SHARELINK_RS*)szbuf;
	if(pssr->m_lResult == sharelink_success)
	{
		((CMyDlgFile*)theApp.m_pMainWnd)->SendMessage(UM_SHARECODE,(WPARAM)&pssr->m_lLinkCode,0);
	}
	else
	{
		::MessageBox(theApp.m_pMainWnd->m_hWnd,"sharelink_failed","提示",MB_OK);
	}


}

void  CMediator::DealGetLinkRS(char* szbuf)
{
	STRU_GETLINK_RS *psgr = (STRU_GETLINK_RS *)szbuf;
	char *pResult = nullptr;
	switch (psgr->m_lResult)
	{
	case getlink_failed:
		pResult = "getlink_failed";
			break;
	case getlink_success:
		pResult = "getlink_success";
			break;
	case link_is_myself:
		pResult = "link_is_myself";
			break;
	case link_is_geted:
		pResult = "link_is_geted";
			break;
	default:
		break;
	}
	::MessageBox(theApp.m_pMainWnd->m_hWnd,pResult,"tip",MB_OK);
	 
}


void  CMediator::DealDeleteFileRS(char* szbuf)
{
	STRU_DELETE_RS *psdr = (STRU_DELETE_RS*)szbuf;

	if(psdr->m_lResult == deletefile_success)
	{
	   ((CMyDlgFile*)theApp.m_pMainWnd)->SendMessage(UM_DELETEFILE,0,0);
	}
	else
	{
		::MessageBox(theApp.m_pMainWnd->m_hWnd,"deletefile_failed","tip",MB_OK);
	}
}
