

#ifndef _PACKDEF_H
#define  _PACKDEF_H
#pragma pack(1)
#define  MAX_TYPE_SIZE       10
#define  MAX_PASSWORD_LEN    16
#define  MAX_SIZE            45
#define  MAX_SOCKETMAP_NUM   100
#define  _DEF_SQL_LEN        200
#define  MAX_CONTENT_LEN     500
#define  THRESHOLD           100*1024*1024

#define _DEFAULT_PORT        1234
#define _DEFAULT_SIZE        1024
#define  MAX_RQ_COUNT        1000


#define UM_CHANGEMAIN       WM_USER + 1
#define UM_SHOWFILELIST     WM_USER + 2
#define UM_SHARECODE        WM_USER + 3
#define UM_DELETEFILE       WM_USER + 4

//注册请求
#define userid_is_exist      0   
#define password_illegal     1
#define register_sucess      2


//登录请求
#define userid_no_exist      0
#define password_error       1
#define login_sucess         2

//上传请求
#define file_is_exist        0
#define file_uploaded        1
#define file_uploadrq_sucess 2

//上传回复
#define fileblock_failed     0
#define fileblock_success    1


//下载请求
#define file_downrq_failed   0
#define file_downrq_success  1

//分享链接
#define  sharelink_failed    0
#define  sharelink_success   1

//提取链接
#define  getlink_failed    0
#define  getlink_success   1
#define  link_is_myself    2
#define  link_is_geted    3

//删除请求
#define  deletefile_failed    0
#define  deletefile_success   1
///////////////////////////////////////////////////


#define _DEF_PROTOCOL_BASE     2000
//注册

#define _DEF_PROTOCOL_REGISTER_RQ    (_DEF_PROTOCOL_BASE + 0)
#define _DEF_PROTOCOL_REGISTER_RS    (_DEF_PROTOCOL_BASE + 1)

//登录
#define _DEF_PROTOCOL_LOGIN_RQ    (_DEF_PROTOCOL_BASE + 2)
#define _DEF_PROTOCOL_LOGIN_RS    (_DEF_PROTOCOL_BASE + 3)

//上传
#define _DEF_PROTOCOL_UPLOAD_RQ    (_DEF_PROTOCOL_BASE + 4)
#define _DEF_PROTOCOL_UPLOAD_RS    (_DEF_PROTOCOL_BASE + 5)

//上传文件块
#define _DEF_PROTOCOL_UPLOAD_FILEBLOCK_RQ    (_DEF_PROTOCOL_BASE + 6)
#define _DEF_PROTOCOL_UPLOAD_FILEBLOCK_RS    (_DEF_PROTOCOL_BASE + 7)


//下载
#define _DEF_PROTOCOL_DOWNLOAD_RQ    (_DEF_PROTOCOL_BASE + 8)
#define _DEF_PROTOCOL_DOWNLOAD_RS    (_DEF_PROTOCOL_BASE + 9)

//下载文件块
#define _DEF_PROTOCOL_DOWNLOAD_FILEBLOCK_RQ    (_DEF_PROTOCOL_BASE + 10)
#define _DEF_PROTOCOL_DOWNLOAD_FILEBLOCK_RS    (_DEF_PROTOCOL_BASE + 11)


//分享--创建链接
#define _DEF_PROTOCOL_SHARE_RQ    (_DEF_PROTOCOL_BASE + 12)
#define _DEF_PROTOCOL_SHARE_RS    (_DEF_PROTOCOL_BASE + 13)
//提取链接
#define _DEF_PROTOCOL_GET_RQ    (_DEF_PROTOCOL_BASE + 14)
#define _DEF_PROTOCOL_GET_RS    (_DEF_PROTOCOL_BASE + 15)
//私信
#define  _DEF_PROTOCOL_PERSONALLETTER_RQ    (_DEF_PROTOCOL_BASE + 16)

#define  _DEF_PROTOCOL_QUIT      (_DEF_PROTOCOL_BASE + 17)

//获取文件列表
#define _DEF_PROTOCOL_GETFILELIST_RQ    (_DEF_PROTOCOL_BASE + 18)
#define _DEF_PROTOCOL_GETFILELIST_RS    (_DEF_PROTOCOL_BASE + 19)
//删除文件
#define _DEF_PROTOCOL_DELETEFILE_RQ    (_DEF_PROTOCOL_BASE + 20)
#define _DEF_PROTOCOL_DELETEFILE_RS    (_DEF_PROTOCOL_BASE + 21)
//协议包
typedef int PackType;

//注册请求\登录请求
typedef struct STRU_REGISTER_RQ
{
	STRU_REGISTER_RQ()
	{
		m_nType = _DEF_PROTOCOL_REGISTER_RQ;
		m_nUserId = 0;
		ZeroMemory(m_szPassword,MAX_PASSWORD_LEN);
		m_nPasswordLen = 0;
		m_lHostIp = 0;
	}
	PackType m_nType;   //包类型
	INT64    m_nUserId; //用户ID
	char     m_szPassword[MAX_PASSWORD_LEN];  //密码
	int     m_nPasswordLen;  //密码长度
	long    m_lHostIp;   //本机Ip

}STRU_REGISTERLOGIN_RQ;

//注册回复
typedef struct STRU_REGISTER_RS
{
	STRU_REGISTER_RS()
	{
		m_nType= _DEF_PROTOCOL_REGISTER_RS;
		m_nUserId = 0;
		m_lResult = 0;
	}
	PackType m_nType;   //包类型
	INT64    m_nUserId; //用户ID
	long     m_lResult ; //注册结果
   

}STRU_REGISTERLOGIN_RS;

//上传请求

typedef struct STRU_UPLOAD_RQ
{
	STRU_UPLOAD_RQ()
	{
		m_nType = _DEF_PROTOCOL_UPLOAD_RQ;
		m_nUserId = 0;
		m_nFileSize =0;
		ZeroMemory(m_szFileName,MAX_PATH);
		ZeroMemory(m_szFileMD5,MAX_SIZE);
		ZeroMemory(m_fileType,MAX_TYPE_SIZE);
		ZeroMemory(m_szFileUpTime,MAX_SIZE);
		ZeroMemory(m_szFileCurrentPath,MAX_PATH);
	}
	PackType m_nType;   //包类型
	INT64    m_nUserId; //用户ID
	INT64    m_nFileSize;
	char     m_szFileName[MAX_PATH];
	char     m_szFileMD5[MAX_SIZE];
	char     m_fileType[MAX_TYPE_SIZE];
	char    m_szFileUpTime[MAX_SIZE];
	char    m_szFileCurrentPath[MAX_PATH];
   

}STRU_UPLOAD_RQ;

//上传回复
typedef struct STRU_UPLOAD_RS
{
	STRU_UPLOAD_RS()
	{
		m_nType = _DEF_PROTOCOL_UPLOAD_RS;
		m_nUserId = 0;
		m_lResult =0;
		m_nUpLoadPos =0;

	}
	PackType m_nType;   //包类型
	INT64    m_nUserId; //用户ID
	INT64    m_nfileId; //文件ID
	long     m_lResult;
	INT64    m_nUpLoadPos; 
	INT64    m_nFileSize;
	char     m_szFileName[MAX_PATH];
	char     m_szFilePath[MAX_PATH];

}STRU_UPLOAD_RS;

//上传文件块请求

typedef struct STRU_UPLOAD_FILEBLOCK_RQ
{
	STRU_UPLOAD_FILEBLOCK_RQ()
	{   
		m_nType = _DEF_PROTOCOL_UPLOAD_FILEBLOCK_RQ;
		m_nUserId = 0;
		m_nFileId =0;
		m_nBlockLen =0;
		m_nBlockId = 0;
		ZeroMemory(m_szFileContent,MAX_CONTENT_LEN);


	}
	PackType m_nType;   //包类型
	INT64    m_nUserId; //用户ID
	INT64    m_nFileId;
	INT64    m_nBlockId;
	char     m_szFileContent[MAX_CONTENT_LEN];
	INT64    m_nBlockLen;
	
   

}STRU_UPLOADDOWNLOAD_FILEBLOCK_RQ;


//上传文件块回复包
typedef struct STRU_UPLOAD_FILEBLOCK_RS
{
	STRU_UPLOAD_FILEBLOCK_RS()
	{
		m_nType = _DEF_PROTOCOL_UPLOAD_FILEBLOCK_RS;
		m_nUserId = 0;
		m_nFileId =0;
		m_nBlockLen =0;
		m_lResult = 0;
	}
	PackType m_nType;   //包类型
	INT64    m_nUserId; //用户ID
	INT64    m_nFileId;
	INT64    m_nBlockLen;
	long     m_lResult; 

}STRU_UPLOADDOWNLOAD_FILEBLOCK_RS;


//下载文件请求


typedef struct STRU_DOWNLOAD_RQ
{
	STRU_DOWNLOAD_RQ()
	{
		m_nType = _DEF_PROTOCOL_DOWNLOAD_RQ;
		m_nUserId = 0;
		ZeroMemory(m_szFileName,sizeof(m_szFileName));
	}
	PackType m_nType;   //包类型
	INT64    m_nUserId; //用户ID
	char     m_szFileName[MAX_PATH];
  
}STRU_DOWNLOAD_RQ;

//下载文件回复
typedef struct STRU_DOWNLOAD_RS
{
	STRU_DOWNLOAD_RS()
	{
		m_nType = _DEF_PROTOCOL_DOWNLOAD_RS;
		m_nUserId = 0;
		m_nFileId = 0;
		m_lResult = 0;
	}
	PackType m_nType;   //包类型
	INT64    m_nUserId; //用户ID
	INT64    m_nFileId;
	long     m_lResult;   
	char     m_szFileName[MAX_PATH];
  
}STRU_DOWNLOAD_RS;


//分享链接
typedef struct STRU_SHARELINK_RQ
{
	STRU_SHARELINK_RQ()
	{
		m_nType = _DEF_PROTOCOL_SHARE_RQ;
		m_nUserId= 0;
		ZeroMemory(m_szFileName,sizeof(m_szFileName));

	}
	PackType m_nType;   //包类型
	INT64    m_nUserId; //用户ID
	char     m_szFileName[MAX_PATH];
  
}STRU_SHARELINK_RQ;

//分享链接
typedef struct STRU_SHARELINK_RS
{
	STRU_SHARELINK_RS()
	{
		m_nType = _DEF_PROTOCOL_SHARE_RS;
		m_nUserId = 0;
		ZeroMemory(m_szFileName,sizeof(m_szFileName));
		m_lResult = 0;
		m_lLinkCode = 0;
	}
	PackType m_nType;   //包类型
	INT64    m_nUserId; //用户ID
	char     m_szFileName[MAX_PATH];
	long     m_lResult;
	INT64    m_lLinkCode;
  
}STRU_SHARELINK_RS;

//提取链接
typedef struct STRU_GETLINK_RQ
{
	STRU_GETLINK_RQ()
	{
		m_nType = _DEF_PROTOCOL_GET_RQ;
		m_nUserId = 0;
		m_lLinkCode = 0;
	}
	PackType m_nType;   //包类型
	INT64    m_nUserId; //用户ID
	INT64    m_lLinkCode;  
  
}STRU_GETLINK_RQ;

//提取回复
typedef struct STRU_GETLINK_RS
{
	STRU_GETLINK_RS()
	{
		m_nType = _DEF_PROTOCOL_GET_RS;
		m_nUserId = 0;
		m_lResult = 0;
	}
	PackType m_nType;   //包类型
	INT64    m_nUserId; //用户ID
	long     m_lResult;
  
}STRU_GETLINK_RS;

//私信请求
typedef struct STRU_PERSONALLETTER_RQ
{
	STRU_PERSONALLETTER_RQ()
	{
		m_nType = _DEF_PROTOCOL_PERSONALLETTER_RQ;
		m_nUserId = 0;
		m_nTargetUserId = 0;
		ZeroMemory(m_szBuffer,MAX_CONTENT_LEN);
	}
	PackType m_nType;   //包类型
	INT64    m_nUserId; //用户ID
	INT64    m_nTargetUserId; 
	char     m_szBuffer[MAX_CONTENT_LEN];
  
}STRU_PERSONALLETTER_RQ;



//退出通知
typedef struct STRU_QUIT
{
	STRU_QUIT()
	{
		m_nType = _DEF_PROTOCOL_QUIT;
		m_nUserId = 0;
		
	}
	PackType m_nType;   //包类型
	INT64    m_nUserId; //用户ID
	
  
}STRU_QUIT;
//获取文件列表
typedef struct STRU_GETFILELIST_RQ
{
	STRU_GETFILELIST_RQ()
	{
		m_nType = _DEF_PROTOCOL_GETFILELIST_RQ;
		m_nUserId = 0;
		
	}
	PackType m_nType;   //包类型
	INT64    m_nUserId; //用户ID
	
  
}STRU_GETFILELIST_RQ;

typedef struct STRU_GETFILELIST_RS
{
	STRU_GETFILELIST_RS()
	{
		m_nType = _DEF_PROTOCOL_GETFILELIST_RS;
		m_nUserId = 0;
		
	}
	PackType m_nType;   //包类型
	INT64    m_nUserId; //用户ID
	INT64    m_nFileSize;
	char     m_szFileName[MAX_PATH];
	char    m_szFileUpTime[MAX_SIZE];
	
  
}STRU_GETFILELIST_RS;
//删除文件
typedef struct STRU_DELETE_RQ
{
	STRU_DELETE_RQ()
	{
		m_nType = _DEF_PROTOCOL_DELETEFILE_RQ;
		m_nUserId = 0;
		
	}
	PackType m_nType;   //包类型
	INT64    m_nUserId; //用户ID
	char     m_szFileName[MAX_PATH];
	
	
  
}STRU_DELETE_RQ;

typedef struct STRU_DELETE_RS
{
	STRU_DELETE_RS()
	{
		m_nType = _DEF_PROTOCOL_DELETEFILE_RS;
		m_nUserId = 0;
		
	}
	PackType m_nType;   //包类型
	INT64    m_nUserId; //用户ID
	long     m_lResult;
	
	
  
}STRU_DELETE_RS;

#endif
