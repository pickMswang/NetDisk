
#include "TCPMediator.h"




CTCPMediator::CTCPMediator(void)
{
	m_pTcpNet = new IOCPServer(this);

	strcpy_s(m_szSystemPath,MAX_PATH, "F:\\Disk\\");

	//协议映射表
	m_pProtocolMap[_DEF_PROTOCOL_REGISTER_RQ - _DEF_PROTOCOL_BASE] = &CTCPMediator::DealRegisterRq;
	m_pProtocolMap[_DEF_PROTOCOL_LOGIN_RQ - _DEF_PROTOCOL_BASE] = &CTCPMediator::DealLoginRq;
	m_pProtocolMap[_DEF_PROTOCOL_UPLOAD_RQ - _DEF_PROTOCOL_BASE] = &CTCPMediator::DealUpLoadRq;
	m_pProtocolMap[_DEF_PROTOCOL_UPLOAD_FILEBLOCK_RQ - _DEF_PROTOCOL_BASE] = &CTCPMediator::DealUpLoadFileBlockRq;
	m_pProtocolMap[_DEF_PROTOCOL_DOWNLOAD_RQ - _DEF_PROTOCOL_BASE] = &CTCPMediator::DealDownLoadRq;
	m_pProtocolMap[_DEF_PROTOCOL_DOWNLOAD_FILEBLOCK_RS - _DEF_PROTOCOL_BASE] = &CTCPMediator::DealDownLoadFileBlockRs;
	m_pProtocolMap[_DEF_PROTOCOL_SHARE_RQ - _DEF_PROTOCOL_BASE] = &CTCPMediator::DealShareLinkRq;
	m_pProtocolMap[_DEF_PROTOCOL_GET_RQ - _DEF_PROTOCOL_BASE] = &CTCPMediator::DealGetLinkRq;
	m_pProtocolMap[_DEF_PROTOCOL_PERSONALLETTER_RQ - _DEF_PROTOCOL_BASE] = &CTCPMediator::DealPersonalLetterRq;
	m_pProtocolMap[_DEF_PROTOCOL_QUIT - _DEF_PROTOCOL_BASE] = &CTCPMediator::DealQuit;
	m_pProtocolMap[_DEF_PROTOCOL_GETFILELIST_RQ - _DEF_PROTOCOL_BASE] = &CTCPMediator::DealGetFileListRq;
	m_pProtocolMap[_DEF_PROTOCOL_DELETEFILE_RQ - _DEF_PROTOCOL_BASE] = &CTCPMediator::DealDeleteFileRq;
	
}

//static const PFUN g_ProtocolMap[] = 
//{
//	&CTCPMediator::DealRegisterRq
//};


CTCPMediator::~CTCPMediator(void)
{
	delete m_pTcpNet;
	m_pTcpNet = NULL;
}

bool CTCPMediator::Open()
{
	

	if(!m_mysql.ConnectMySql("localhost","root","123456","0730Disk"))
	{
		return false;
	}

	if(!m_pTcpNet->InitNetWork())
		return false;

	

	return true;
}
	
void CTCPMediator::Close()
{
	m_pTcpNet->UnInitNetWork();

	auto iteDownInfo = m_mapIdToFileDownLoadInfo.begin();
	while(iteDownInfo != m_mapIdToFileDownLoadInfo.end())
	{
		delete iteDownInfo->second;
		iteDownInfo->second = nullptr;
		iteDownInfo++;
	}
	m_mapIdToFileDownLoadInfo.clear();

	auto iteIdToFile = m_mapIdToFileInfo.begin();
	while(iteIdToFile != m_mapIdToFileInfo.end())
	{
		delete iteIdToFile->second;
		iteIdToFile->second = nullptr;
		iteIdToFile++;
	}
	m_mapIdToFileInfo.clear();
	auto iteUserIdToPL = m_mapUserIdToPersonalLetterInfo.begin();
	while(iteUserIdToPL != m_mapUserIdToPersonalLetterInfo.end())
	{
		delete iteUserIdToPL->second;
		iteUserIdToPL->second = nullptr;
		iteUserIdToPL++;
	}
	m_mapUserIdToPersonalLetterInfo.clear();
	auto iteUserIdToUserInfo = m_mapUserIdToUserInfo.begin();
	while(iteUserIdToUserInfo != m_mapUserIdToUserInfo.end())
	{
		delete iteUserIdToUserInfo->second;
		iteUserIdToUserInfo->second = nullptr;
		iteUserIdToUserInfo++;
	}
	m_mapUserIdToUserInfo.clear();

}
	
bool CTCPMediator::SendData(MySocket *pSockex,char* szbuf,int nLen)
{

	if(!m_pTcpNet->SendData(pSockex,szbuf,nLen))
		return false;

	return true;
}
	
bool CTCPMediator::DealData(MySocket *pSockex,char* szbuf)
{
	int* nProtocolType = (int*)szbuf;
	

	//(this->*g_ProtocolMap[*nProtocolType - _DEF_PROTOCOL_BASE])(pSockex,szbuf);
	(this->*m_pProtocolMap[*nProtocolType - _DEF_PROTOCOL_BASE])(pSockex,szbuf);
	return true;
}

void CTCPMediator::DealRegisterRq(MySocket *pSockex,char* szbuf)
{
	  //序列化  反序列化
	  //注册请求
	  STRU_REGISTER_RQ *psrr = (STRU_REGISTER_RQ *)szbuf;
	 
	  //校验账号是否存在，如果存在，则注册失败
	  // select useid  from t_user where userid =  psrr->m_nUserId;
	  list<string> lststrid;
	  
	  
	  char szsql[_DEF_SQL_LEN] ={0};
	  char szInsertSql[_DEF_SQL_LEN]={0};


	  sprintf_s(szsql,"select userid  from t_user where userid = %lld",psrr->m_nUserId);

	  //定义回复包
	  STRU_REGISTER_RS srr;
	  srr.m_nType = _DEF_PROTOCOL_REGISTER_RS;
	  srr.m_nUserId = psrr->m_nUserId;

	  //如果当前用户数据不存在,加入数据库

	  if(m_mysql.SelectMySql(szsql,1,lststrid))
	  {
		   if(lststrid.size() > 0 )
		   {
			   //此用户名存在
			   srr.m_lResult = userid_is_exist;
		   }
		    else
	       {
			  // 校验密码长度 注册成功，将数据写到数据库中
			   if(psrr->m_nPasswordLen <6 || psrr->m_nPasswordLen >16)
			   {
					srr.m_lResult = password_illegal;
			   }
			   else
			   {
				     char szPath[MAX_PATH] = {0};
			   		 srr.m_lResult = register_sucess;
					 sprintf_s(szPath,"%s%lld",m_szSystemPath,psrr->m_nUserId);

		             CreateDirectory(szPath,NULL);
					//将数据写到数据库中
					//  insert into t_user values(psrr->m_nUserId,psrr->m_szPassword)
					sprintf_s(szInsertSql,"insert into t_user values(%lld,'%s')",psrr->m_nUserId,psrr->m_szPassword);

					m_mysql.UpdateMySql(szInsertSql);
			   }
		
  
		    }

	  }
	

	  m_pTcpNet->SendData(pSockex,(char*)&srr,sizeof(srr));

}

void CTCPMediator::DealLoginRq(MySocket *pSockex,char* szbuf)
{

	//校验用户名和密码 ，如果都正确，则登录成功
	STRU_REGISTERLOGIN_RQ *psrr= (STRU_REGISTERLOGIN_RQ *)szbuf;

	//select 密码 from t_user where userid = psrr->userid;

	char szsql[_DEF_SQL_LEN]= {0};
	list<string> lststr;
	sprintf_s(szsql,"select userpassword from t_user where userid = %lld",psrr->m_nUserId);
	STRU_REGISTERLOGIN_RS srr;
	srr.m_nType = _DEF_PROTOCOL_LOGIN_RS;
	srr.m_nUserId = psrr->m_nUserId;
	
	if(m_mysql.SelectMySql(szsql,1,lststr))
	{
		if(lststr.size() >0 )
		{
			string strpassword = lststr.front();
			lststr.pop_front();

			if(0 == strcmp(strpassword.c_str(), psrr->m_szPassword))
			{
				 srr.m_lResult = login_sucess;
				 UserInfo *pNewInfo = new UserInfo;
				 pNewInfo->Userid = psrr->m_nUserId;
				 pNewInfo->lUserIp = psrr->m_lHostIp;
				 pNewInfo->bUserState = true;
				 pNewInfo->pSock = pSockex;
				 m_mapUserIdToUserInfo[pNewInfo->Userid] = pNewInfo;

				 //检验有没有私信
				 auto ite = m_mapUserIdToPersonalLetterInfo.find(psrr->m_nUserId);
				 if(ite != m_mapUserIdToPersonalLetterInfo.end())
				 {
					 //如果有，则直接发送
					 m_pTcpNet->SendData(pSockex,(char*)ite->second,sizeof(STRU_PERSONALLETTER_RQ));

					 delete ite->second;
					 ite->second = nullptr;

					 m_mapUserIdToPersonalLetterInfo.erase(ite);
				 }
			}
			else
			{
				 srr.m_lResult = password_error;
			}
		}
		else
		{
			 srr.m_lResult = userid_no_exist;
		}

		m_pTcpNet->SendData(pSockex,(char*)&srr,sizeof(srr));
	}



}

void CTCPMediator::DealUpLoadRq(MySocket *pSockex,char* szbuf)
{
	STRU_UPLOAD_RQ *psur = (STRU_UPLOAD_RQ *)szbuf;
	STRU_UPLOAD_RS sur;
	sur.m_nType = _DEF_PROTOCOL_UPLOAD_RS;
	char szsql[_DEF_SQL_LEN] = {0};
	list<string> lststr;
	INT64 nFileID;
	sprintf_s(szsql,"select t_file.fileid from t_file \
inner join t_user_file on t_file.fileid = t_user_file.fileid \
where filename = '%s' and userid = %lld;",psur->m_szFileName,psur->m_nUserId);

	//1.判断是否有同名文件---当前人
	if(m_mysql.SelectMySql(szsql,1,lststr))
	{
		if(lststr.size() >0)
		{
			//同名文件存在  ----   判断这个文件是用户在断点续传还是已经传了同名文件
			sur.m_nfileId = 0;
			sur.m_lResult = file_is_exist;

			auto ite = m_mapIdToFileInfo.begin();
			while(ite != m_mapIdToFileInfo.end())
			{
				//断点续传
				if(0 == strcmp(ite->second->m_szFileName ,psur->m_szFileName) &&
					ite->second->m_nUserId == psur->m_nUserId)
				 {
					 //如果是断点续传，则需要告诉用户 文件请求发送成功，并且告诉用户文件ID 、文件上传的位置
					 sur.m_lResult = file_uploadrq_sucess;
					 sur.m_nfileId = ite->second->m_nfileID;
					 sur.m_nUpLoadPos = ite->second->m_nUpLoadSize;
					 sur.m_nUserId = ite->second->m_nUserId;
					 strcpy_s(sur.m_szFileName,MAX_PATH,ite->second->m_szFileName);
					 strcpy_s(sur.m_szFilePath,MAX_PATH,psur->m_szFileCurrentPath);
					 break;
				 }
			    
				ite++;
			}



		}
		else
		{
			sur.m_lResult = file_uploadrq_sucess;
			//如果大于阈值
			if(psur->m_nFileSize  > THRESHOLD)
			{
				list<string> lstfile;
			    //2.判断文件服务器是否存在，如果存在，直接映射 (文件id)
				ZeroMemory(szsql,sizeof(szsql));

				sprintf_s(szsql,"select fileid from t_file where fileMD5 = '%s';",psur->m_szFileMD5);

				if(m_mysql.SelectMySql(szsql,1,lstfile))
				{
					  if(lstfile.size() > 0 )
					 {
						 string strfileid = lstfile.front();
						
						 lstfile.pop_front();
						 //说明文件在服务器中已存在,直接映射 (文件id)
						 sur.m_lResult = file_uploaded;
						 //insert into t_user_file values();
						 ZeroMemory(szsql,sizeof(szsql));
						 sprintf_s(szsql,"insert into t_user_file values(%lld,%lld)",psur->m_nUserId, _atoi64(strfileid.c_str()));
						 
						 m_mysql.UpdateMySql(szsql);
						 //关于当前文件的引用计数+1
						  ZeroMemory(szsql,sizeof(szsql));
						  sprintf_s(szsql,"select filecount from t_file where fileid = %lld", _atoi64(strfileid.c_str()));
						  list<string> lstCount;
						  m_mysql.SelectMySql(szsql,1,lstCount);
						  INT64 nCount = _atoi64(lstCount.front().c_str());
						  lstCount.pop_front();
						  nCount = nCount + 1;
						  ZeroMemory(szsql,sizeof(szsql));
						  sprintf_s(szsql,"update t_file set filecount = %lld where fileid= %lld;" ,nCount, _atoi64(strfileid.c_str()));
						  m_mysql.UpdateMySql(szsql);

					 }
					 
				}
			}
			

			if(sur.m_lResult == file_uploadrq_sucess)
			{
				
				//3.如果不存在，创建文件
				char szpath[MAX_PATH] = {0};
				char stemp[MAX_PATH] = {0};
				sprintf_s(stemp,"%s\\%lld\\\\%s",m_szSystemPath,psur->m_nUserId,psur->m_szFileName);
				sprintf_s(szpath,"%s%lld\\%s",m_szSystemPath,psur->m_nUserId,psur->m_szFileName);
				//文件ID 
			 	nFileID = GetFileID(SDBMHash(psur->m_szFileName),psur->m_nUserId);
			
				FILE * pfile  = NULL;
				fopen_s(&pfile,szpath,"wb");

				FileInfo *pNewInfo = new FileInfo;
				
				pNewInfo->m_nUserId= psur->m_nUserId;
				pNewInfo->m_nfileID = nFileID;
				pNewInfo->m_nFileSize= psur->m_nFileSize;
				pNewInfo->m_pfile = pfile;
				pNewInfo->m_nUpLoadSize = 0;
				strcpy_s(pNewInfo->m_fileType,psur->m_fileType);
				strcpy_s(pNewInfo->m_szFileName,psur->m_szFileName);
				strcpy_s(pNewInfo->m_szFileLocalPath,szpath);
				strcpy_s(pNewInfo->m_szFileMD5,psur->m_szFileMD5);
				strcpy_s(pNewInfo->m_szFileUpTime,psur->m_szFileUpTime);

				m_mapIdToFileInfo[nFileID] = pNewInfo;

				
				sur.m_nfileId = nFileID;
				sur.m_nUpLoadPos = 0;
				sur.m_nFileSize =  psur->m_nFileSize;
				strcpy_s(sur.m_szFileName,psur->m_szFileName);
				strcpy_s(sur.m_szFilePath,psur->m_szFileCurrentPath);

			   char szsql[_DEF_SQL_LEN] = {0};
			
			   
			 //将文件信息写入数据库
			 sprintf_s(szsql,"insert into t_file values(%lld,'%s',%lld,'%s','%s','%s','%s',%lld)",pNewInfo->m_nfileID,pNewInfo->m_szFileName,0,
				 pNewInfo->m_szFileMD5,pNewInfo->m_fileType,pNewInfo->m_szFileUpTime,stemp,1);

			 m_mysql.UpdateMySql(szsql);

			 //将文件与用户t_user_file
			 ZeroMemory(szsql,_DEF_SQL_LEN);
			 sprintf_s(szsql,"insert into t_user_file values(%lld,%lld)",pNewInfo->m_nUserId,pNewInfo->m_nfileID);

			  m_mysql.UpdateMySql(szsql);
			}
		
			  
			

		}

	}
	

	//4.回复
	m_pTcpNet->SendData(pSockex,(char*)&sur,sizeof(sur));
	 
}

void CTCPMediator::DealUpLoadFileBlockRq(MySocket *pSockex,char* szbuf)
{
	STRU_UPLOAD_FILEBLOCK_RQ *psufr = (STRU_UPLOAD_FILEBLOCK_RQ *)szbuf;

	//文件块--- 块ID，文件内容，file ID ，内容长度，userid
	auto ite =  m_mapIdToFileInfo.find(psufr->m_nFileId);
	FileInfo *pInfo = ite->second;
	STRU_UPLOAD_FILEBLOCK_RS sufr;
	sufr.m_nType = _DEF_PROTOCOL_UPLOAD_FILEBLOCK_RS;
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
			fwrite(psufr->m_szFileContent,sizeof(char),psufr->m_nBlockLen,pInfo->m_pfile);
		    sufr.m_lResult = fileblock_success;
		   pInfo->m_nUpLoadSize += psufr->m_nBlockLen;
		}
		
		
		//文件尾
		if(psufr->m_nBlockLen < MAX_CONTENT_LEN||pInfo->m_nUpLoadSize == pInfo->m_nFileSize)
		{
			 fclose(ite->second->m_pfile);
			 //将数据更新到数据库中

			 char szsql[_DEF_SQL_LEN] = {0};
			 sprintf_s(szsql,"update t_file set filesize =%lld where fileid = %lld",pInfo->m_nFileSize,pInfo->m_nfileID);
			  m_mysql.UpdateMySql(szsql);
			///////////////////////////////////////////////////////////////
			 ////将文件信息写入数据库
			 //sprintf_s(szsql,"insert into t_file values(%lld,'%s',%lld,'%s','%s','%s','%s')",pInfo->m_nfileID,pInfo->m_szFileName,pInfo->m_nFileSize,
				// pInfo->m_szFileMD5,pInfo->m_fileType,pInfo->m_szFileUpTime,pInfo->m_szFileLocalPath);

			 //m_mysql.UpdateMySql(szsql);

			 ////将文件与用户t_user_file
			 //ZeroMemory(szsql,_DEF_SQL_LEN);
			 //sprintf_s(szsql,"insert into t_user_file values(%lld,%lld)",psufr->m_nUserId,pInfo->m_nfileID);

			 // m_mysql.UpdateMySql(szsql);

			 delete ite->second;
			 ite->second = nullptr;
			 m_mapIdToFileInfo.erase(ite);

		}
	}

	

	//回复
	
	sufr.m_nUserId = psufr->m_nUserId;
	sufr.m_nFileId = psufr->m_nFileId;
	//sufr.m_nBlockId = psufr->m_nBlockId;
	sufr.m_nBlockLen = psufr->m_nBlockLen;

	m_pTcpNet->SendData(pSockex,(char*)&sufr,sizeof(sufr));


}


void  CTCPMediator::DealDownLoadRq(MySocket *pSockex,char* szbuf)
{
	STRU_DOWNLOAD_RQ *psdr = (STRU_DOWNLOAD_RQ *)szbuf;
	FileDownLoadInfo *pNewInfo = nullptr;
	STRU_DOWNLOAD_RS sdr;
	sdr.m_nType = _DEF_PROTOCOL_DOWNLOAD_RS;
	sdr.m_nUserId = psdr->m_nUserId;
	sdr.m_lResult = file_downrq_failed;
	//下载请求
	char szsql[_DEF_SQL_LEN] = {0};
	list<string> lststr;
	sprintf_s(szsql,"select t_file.fileid,filepath,filesize from t_file \
inner join t_user_file on t_file.fileid = t_user_file.fileid \
and t_user_file.userid = %lld  and t_file.filename  ='%s'",psdr->m_nUserId,psdr->m_szFileName);

	if(m_mysql.SelectMySql(szsql,3,lststr))
	{
		//检查文件是否存在，回复客户端成功还是失败
		if(lststr.size() >0)
		{
			string strFileid = lststr.front();
			lststr.pop_front();
			string strFilePath = lststr.front();
			lststr.pop_front();
			string strSize = lststr.front();
			lststr.pop_front();
			sdr.m_nFileId = _atoi64(strFileid.c_str());
			sdr.m_lResult = file_downrq_success;
			strcpy_s(sdr.m_szFileName,MAX_PATH,psdr->m_szFileName);
			//打开文件（文件指针  文件ID ,文件位置，文件大小，用户id）
			FILE *pFile = nullptr;
			fopen_s(&pFile,strFilePath.c_str(),"rb");

			pNewInfo = new FileDownLoadInfo;
			pNewInfo->m_nfileID = sdr.m_nFileId;
			pNewInfo->m_nFileSize = _atoi64(strSize.c_str());
			pNewInfo->m_nDownLoadSize = 0;
			pNewInfo->m_nUserId = psdr->m_nUserId;
			pNewInfo->m_pfile = pFile;
			strcpy_s(pNewInfo->m_szFileName,MAX_PATH,psdr->m_szFileName);

			m_mapIdToFileDownLoadInfo[pNewInfo->m_nfileID] = pNewInfo;
			


		}
	}
	
	m_pTcpNet->SendData(pSockex,(char*)&sdr,sizeof(sdr));

	if(sdr.m_lResult == file_downrq_success)
	{
		////发送第一个文件块 -- 
		DealDownLoadFileBlockRq(pSockex,(char*)pNewInfo);

	}
	
}

void CTCPMediator::DealDownLoadFileBlockRq(MySocket *pSockex,char* szbuf)
{
	FileDownLoadInfo *pInfo = (FileDownLoadInfo *)szbuf;
	//文件内容 内容长度 文件Id,文件位置--传输请求
	STRU_UPLOADDOWNLOAD_FILEBLOCK_RQ sufr;
	sufr.m_nType = _DEF_PROTOCOL_DOWNLOAD_FILEBLOCK_RQ;
	
	INT64 nRelReadNum = (size_t)fread_s(sufr.m_szFileContent,sizeof(sufr.m_szFileContent),sizeof(char),MAX_CONTENT_LEN,pInfo->m_pfile);
	//if(nRelReadNum == pInfo->m_nFileSize)
	//{
	

	//}
	sufr.m_nBlockLen = nRelReadNum;
	sufr.m_nFileId = pInfo->m_nfileID;
	sufr.m_nUserId = pInfo->m_nUserId;
	


	m_pTcpNet->SendData(pSockex,(char*)&sufr,sizeof(sufr));

}



void  CTCPMediator::DealDownLoadFileBlockRs(MySocket *pSockex,char* szbuf)
{
	STRU_UPLOAD_FILEBLOCK_RS *psufr = (STRU_UPLOAD_FILEBLOCK_RS *)szbuf;
	auto ite = m_mapIdToFileDownLoadInfo.find(psufr->m_nFileId);
	FileDownLoadInfo *pInfo = ite->second;
	STRU_UPLOADDOWNLOAD_FILEBLOCK_RQ sufr;
	sufr.m_nType = _DEF_PROTOCOL_DOWNLOAD_FILEBLOCK_RQ;
	sufr.m_nFileId = pInfo->m_nfileID;
	sufr.m_nUserId = pInfo->m_nUserId;
	//如果文件块客户端接收失败，再次发送
	if(psufr->m_lResult == fileblock_failed)
	{
		//移动文件指针pInfo->m_nDownLoadSize
		 _fseeki64(pInfo->m_pfile,pInfo->m_nDownLoadSize,SEEK_SET);
		INT64 nRelReadNum = (size_t)fread_s(sufr.m_szFileContent,sizeof(sufr.m_szFileContent),sizeof(char),MAX_CONTENT_LEN,pInfo->m_pfile);
		sufr.m_nBlockLen = nRelReadNum;
		//读文件内容并发送
		m_pTcpNet->SendData(pSockex,(char*)&sufr,sizeof(sufr));
		return;
	}
	
	

	pInfo->m_nDownLoadSize += psufr->m_nBlockLen;

	if(pInfo->m_nDownLoadSize == pInfo->m_nFileSize)
	{
		fclose(pInfo->m_pfile);
		
		auto ite = m_mapIdToFileDownLoadInfo.find(pInfo->m_nfileID);
		if(ite != m_mapIdToFileDownLoadInfo.end())
		{
			delete ite->second;
			ite->second = nullptr;
			m_mapIdToFileDownLoadInfo.erase(ite);
		}

		return;
		
	}
	else
	{
		 	INT64 nRelReadNum = (size_t)fread_s(sufr.m_szFileContent,sizeof(sufr.m_szFileContent),sizeof(char),MAX_CONTENT_LEN,pInfo->m_pfile);
			sufr.m_nBlockLen = nRelReadNum;
	}


	
	

	m_pTcpNet->SendData(pSockex,(char*)&sufr,sizeof(sufr));


}


void CTCPMediator::DealShareLinkRq(MySocket *pSockex,char* szbuf)
{

	STRU_SHARELINK_RQ *pssr =(STRU_SHARELINK_RQ *)szbuf;
	//分享链接请求
	//通过用户要共享的文件名 获得文件的id 以及文件的存储路径
	char szsql[_DEF_SQL_LEN] = {0};
	list<string>  lststr;
	sprintf_s(szsql,"select t_file.fileid,filepath  from t_file \
inner join t_user_file on t_file.fileid = t_user_file.fileid \
and t_user_file.userid = %lld  and t_file.filename  ='%s'",pssr->m_nUserId,pssr->m_szFileName);
	STRU_SHARELINK_RS ssr;
	ssr.m_nType = _DEF_PROTOCOL_SHARE_RS;
	ssr.m_lResult = sharelink_failed;
	if(m_mysql.SelectMySql(szsql,2,lststr))
	{
	      //发送分享回复--包含文件id、分享结果成功还是失败
		if(lststr.size() >0)
		{
			string strfileid = lststr.front();
			lststr.pop_front();
			
			ssr.m_lResult = sharelink_success;
			//ssr.m_nFileId = _atoi64(strfileid.c_str());
			

			ZeroMemory(szsql,sizeof(szsql));
			//判断当前这个人对于这个文件有没有分享过
			list<string> lstFile;
			sprintf_s(szsql,"SELECT linkcode FROM 0730disk.t_share_file where fileid = %lld and userid = %lld;",
				_atoi64(strfileid.c_str()),pssr->m_nUserId);
			m_mysql.SelectMySql(szsql,1,lstFile);
			
			if(lstFile.size() == 0  )
			{
				ssr.m_lLinkCode = GetLinkCode(pssr->m_nUserId);
				// userid  文件ID  文件路径  提取码 插入数据库 --- 共享表
			   ZeroMemory(szsql,sizeof(szsql));
			   sprintf_s(szsql,"insert into t_share_file values(%lld,%lld,%lld)",pssr->m_nUserId, _atoi64(strfileid.c_str()),ssr.m_lLinkCode);
			   m_mysql.UpdateMySql(szsql);
			}
			else
			{
				ssr.m_lLinkCode =_atoi64(lstFile.front().c_str());
				lstFile.pop_front();

			}
			

			
			
		}

	}


	m_pTcpNet->SendData(pSockex,(char*)&ssr,sizeof(ssr));


	

	


}


void CTCPMediator::DealGetLinkRq(MySocket *pSockex,char* szbuf)
{
	//提取码
	STRU_GETLINK_RQ *psgr = (STRU_GETLINK_RQ *)szbuf;
	STRU_GETLINK_RS sgr;
	sgr.m_nType = _DEF_PROTOCOL_GET_RS;
	sgr.m_lResult = getlink_failed;
	// userid  文件ID  文件路径  提取码 插入数据库 --- 共享表
	char szsql[_DEF_SQL_LEN] = {0};
	list<string> lststr;
	string strFileid;
	string strUserid;
	sprintf_s(szsql,"select fileid,userid from t_share_file where linkcode = %lld",psgr->m_lLinkCode);

	if(m_mysql.SelectMySql(szsql,2,lststr))
	{
		if(lststr.size() >0)
		{
			sgr.m_lResult = getlink_success;
			//将文件ID 与userid  ---加入到数据库
			 strFileid = lststr.front();
			lststr.pop_front();
			 strUserid = lststr.front();
			lststr.pop_front();
			//查看分享链接的人是不是自己，如果是自己则提示自己
			if(_atoi64(strUserid.c_str()) == psgr->m_nUserId)
			{
				sgr.m_lResult = link_is_myself;
				
			}
			else
			{
				 //查看自己是不是提取过，
				list<string> lstFileID;
				ZeroMemory(szsql,sizeof(szsql));
				sprintf_s(szsql,"select fileid from t_user_file where userid = %lld and fileid = %lld",psgr->m_nUserId,_atoi64(strFileid.c_str()));
				m_mysql.SelectMySql(szsql,1,lstFileID);
				if(lstFileID.size() >0)
				{
					sgr.m_lResult = link_is_geted;
				}
				else
				{
					ZeroMemory(szsql,sizeof(szsql));
		           	sprintf_s(szsql,"insert into t_user_file values(%lld,%lld)",psgr->m_nUserId,_atoi64(strFileid.c_str()));

		        	m_mysql.UpdateMySql(szsql);
					//关于当前文件的引用计数+1
					ZeroMemory(szsql,sizeof(szsql));
					sprintf_s(szsql,"select filecount from t_file where fileid = %lld", _atoi64(strFileid.c_str()));
					list<string> lstCount;
					m_mysql.SelectMySql(szsql,1,lstCount);
					INT64 nCount = _atoi64(lstCount.front().c_str());
					lstCount.pop_front();
					nCount = nCount + 1;
					ZeroMemory(szsql,sizeof(szsql));
					sprintf_s(szsql,"update t_file set filecount = %lld where fileid= %lld;" ,nCount, _atoi64(strFileid.c_str()));
					m_mysql.UpdateMySql(szsql);
					//获得文件信息，并发送
                    
				}

			
			}
			

			

		}
	}


	m_pTcpNet->SendData(pSockex,(char*)&sgr,sizeof(sgr));

	//如果提取链接成功，则发送获取指定文件信息发送
	if(sgr.m_lResult == getlink_success)
	{
	  GetSelectFileInfo(pSockex,psgr->m_nUserId,_atoi64(strFileid.c_str()));
	}



}

void CTCPMediator::GetSelectFileInfo(MySocket *pSockex,INT64 nUserid,INT64 nFileid)
{
	char szsql[_DEF_SQL_LEN] = {0};
	STRU_GETFILELIST_RS sgr;
	sgr.m_nType = _DEF_PROTOCOL_GETFILELIST_RS;
	sprintf_s(szsql,"select filename,filesize,fileuptime from t_file \
inner join t_user_file on t_file.fileid = t_user_file.fileid \
where userid =%lld and t_file.fileid = %lld",nUserid,nFileid);
	list<string> lststr;
	if(m_mysql.SelectMySql(szsql,3,lststr))
	{
		auto ite = lststr.begin();
		while(ite != lststr.end())
		{
			strcpy_s(sgr.m_szFileName,sizeof(sgr.m_szFileName),(*ite).c_str());
			ite++;
			sgr.m_nFileSize = _atoi64((*ite).c_str());
			ite++;
			strcpy_s(sgr.m_szFileUpTime,sizeof(sgr.m_szFileUpTime),(*ite).c_str());
			ite++;

			m_pTcpNet->SendData(pSockex,(char*)&sgr,sizeof(sgr));
			//Sleep(10);--为了解决粘包的问题，解决方案，在iocpserver中，send 大小 send 内容
		}
	}
}

void CTCPMediator::DealPersonalLetterRq(MySocket *pSockex,char* szbuf)
{

	STRU_PERSONALLETTER_RQ *pspr = (STRU_PERSONALLETTER_RQ *)szbuf;

	auto ite =  m_mapUserIdToUserInfo.find(pspr->m_nUserId);
	UserInfo *pInfo = ite->second;
	 //私信 （用户ID .用户IP .用户状态、用户登录时间）
	if(ite != m_mapUserIdToUserInfo.end())
	{
		//判断对方是否在线，如果在线 ，直接发送
		m_pTcpNet->SendData(pInfo->pSock,szbuf,sizeof(STRU_PERSONALLETTER_RQ));
	}
	else
	{
		//如果对方不在线，在服务器中记录
		STRU_PERSONALLETTER_RQ *pNewInfo = new STRU_PERSONALLETTER_RQ;
		pNewInfo->m_nUserId = pspr->m_nUserId;
		pNewInfo->m_nTargetUserId = pspr->m_nTargetUserId;
		pNewInfo->m_nType = pspr->m_nType;
		memcpy(pNewInfo->m_szBuffer,pspr->m_szBuffer,sizeof(pNewInfo->m_szBuffer));
		
		m_mapUserIdToPersonalLetterInfo[pspr->m_nTargetUserId] = pNewInfo;

	}

	




}


void CTCPMediator::DealQuit(MySocket *pSockex,char* szbuf)
{
      STRU_QUIT *psq = ( STRU_QUIT *)szbuf;

	 auto ite=  m_mapUserIdToUserInfo.find(psq->m_nUserId);

	 if(ite != m_mapUserIdToUserInfo.end())
	 {
		 delete ite->second;
		 ite->second = nullptr;
		 m_mapUserIdToUserInfo.erase(ite);
	 }



}

void CTCPMediator::DealGetFileListRq(MySocket *pSockex,char* szbuf)
{
	STRU_GETFILELIST_RQ *psgr = (STRU_GETFILELIST_RQ *)szbuf;
	//获得文件列表信息--并回复
	char szsql[_DEF_SQL_LEN] = {0};
	STRU_GETFILELIST_RS sgr;
	sgr.m_nType = _DEF_PROTOCOL_GETFILELIST_RS;
	sprintf_s(szsql,"select filename,filesize,fileuptime from t_file \
inner join t_user_file on t_file.fileid = t_user_file.fileid \
where userid =%lld ",psgr->m_nUserId);
	list<string> lststr;
	if(m_mysql.SelectMySql(szsql,3,lststr))
	{
		auto ite = lststr.begin();
		while(ite != lststr.end())
		{
			strcpy_s(sgr.m_szFileName,sizeof(sgr.m_szFileName),(*ite).c_str());
			ite++;
			sgr.m_nFileSize = _atoi64((*ite).c_str());
			ite++;
			strcpy_s(sgr.m_szFileUpTime,sizeof(sgr.m_szFileUpTime),(*ite).c_str());
			ite++;

			m_pTcpNet->SendData(pSockex,(char*)&sgr,sizeof(sgr));
			//Sleep(10);--为了解决粘包的问题，解决方案，在iocpserver中，send 大小 send 内容
		}
	}

}

void CTCPMediator::DealDeleteFileRq(MySocket *pSockex,char* szbuf)
{
	STRU_DELETE_RQ *psdr = (STRU_DELETE_RQ *)szbuf;
	STRU_DELETE_RS sdr;
	sdr.m_nType = _DEF_PROTOCOL_DELETEFILE_RS;
	sdr.m_nUserId = psdr->m_nUserId;
	sdr.m_lResult = deletefile_failed;
	char szsql[_DEF_SQL_LEN] = {0};
	sprintf_s(szsql,"select t_file.fileid from t_file inner join t_user_file on \
t_file.fileid = t_user_file.fileid where userid = %lld  and filename = '%s'",psdr->m_nUserId,psdr->m_szFileName);
	list<string> lststr;
	if(m_mysql.SelectMySql(szsql,1,lststr))
	{
		if(lststr.size() >0)
		{
			sdr.m_lResult = deletefile_success;
			
			string strFileid = lststr.front();
			lststr.pop_front();

			ZeroMemory(szsql,sizeof(szsql));
			sprintf_s(szsql,"delete from t_user_file where fileid = %lld and userid = %lld",_atoi64(strFileid.c_str()),psdr->m_nUserId);
			m_mysql.UpdateMySql(szsql);

			//获得文件的引用计数
				//关于当前文件的引用计数+1
			ZeroMemory(szsql,sizeof(szsql));
			sprintf_s(szsql,"select filecount from t_file where fileid = %lld", _atoi64(strFileid.c_str()));
			list<string> lstCount;
			m_mysql.SelectMySql(szsql,1,lstCount);
			INT64 nCount = _atoi64(lstCount.front().c_str());
			lstCount.pop_front();
			if(nCount == 1)
			{
				ZeroMemory(szsql,sizeof(szsql));
				sprintf_s(szsql,"delete from t_file where fileid = %lld",_atoi64(strFileid.c_str()));
			     m_mysql.UpdateMySql(szsql);
			}
			else
			{
				nCount = nCount - 1;
				ZeroMemory(szsql,sizeof(szsql));
				sprintf_s(szsql,"update t_file set filecount = %lld where fileid= %lld;" ,nCount, _atoi64(strFileid.c_str()));
				m_mysql.UpdateMySql(szsql);

			}
			



			
		}
	}

	m_pTcpNet->SendData(pSockex,(char*)&sdr,sizeof(sdr));
}







