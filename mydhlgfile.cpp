// MyDlgFile.cpp : 实现文件
//

#include "stdafx.h"
#include "0804Disk.h"
#include "MyDlgFile.h"
#include "afxdialogex.h"


// CMyDlgFile 对话框

IMPLEMENT_DYNAMIC(CMyDlgFile, CDialogEx)


//CMyDlgFile::CMyDlgFile(INT64  nUserId)
//{
//	
//}// 标准构造函数


CMyDlgFile::CMyDlgFile(INT64  nUserId,CWnd* pParent /*=NULL*/)
	: CDialogEx(CMyDlgFile::IDD, pParent)
	, m_edtLinkCode(0)
{
	m_nUserId  = nUserId;
}

CMyDlgFile::~CMyDlgFile()
{
}

void CMyDlgFile::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_lstFile);
	DDX_Text(pDX, IDC_EDIT_CODE, m_edtLinkCode);
}


BEGIN_MESSAGE_MAP(CMyDlgFile, CDialogEx)
	ON_MESSAGE(UM_SHOWFILELIST,&CMyDlgFile::OnShowFileList)
	ON_MESSAGE(UM_SHARECODE,&CMyDlgFile::OnShowShareCode)
	ON_MESSAGE(UM_DELETEFILE,&CMyDlgFile::OnDeleteFile)
	ON_BN_CLICKED(IDC_BUTTON1, &CMyDlgFile::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &CMyDlgFile::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CMyDlgFile::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CMyDlgFile::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CMyDlgFile::OnBnClickedButton6)
END_MESSAGE_MAP()


// CMyDlgFile 消息处理程序
LRESULT CMyDlgFile::OnShowFileList(WPARAM wparam,LPARAM lparam)
{
	STRU_GETFILELIST_RS *psgr = (STRU_GETFILELIST_RS *)wparam;
	CString str;
	m_lstFile.InsertItem(0,psgr->m_szFileName);
	str.Format(_T("%lld"),psgr->m_nFileSize);
	m_lstFile.SetItemText(0,1,str);
	m_lstFile.SetItemText(0,2,psgr->m_szFileUpTime);
	return 0;
}

BOOL CMyDlgFile::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_lstFile.InsertColumn(0,"filename",0,100);
	m_lstFile.InsertColumn(1,"filesize",0,100);
	m_lstFile.InsertColumn(2,"fileupTime",0,100);

	m_lstFile.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	STRU_GETFILELIST_RQ sgr;
	sgr.m_nType = _DEF_PROTOCOL_GETFILELIST_RQ;
	sgr.m_nUserId =((CMyDlgFile*)theApp.m_pMainWnd)->m_nUserId;
	theApp.m_Mediator.GetTCPNet()->SendData((char*)&sgr,sizeof(STRU_GETFILELIST_RQ));
	return TRUE;  // return TRUE unless you set the focus to a control

}

std::string CMyDlgFile::FileDigest(const std::string &file) 
{  
  
    std::ifstream in(file.c_str(),  std::ios::binary);  
	int n = GetLastError();
    if (!in)  
        return "";  
  
    MD5 md5;  
    std::streamsize length;  
    char buffer[1024];  
    while (!in.eof()) {  
        in.read(buffer, 1024);  
        length = in.gcount();  
        if (length > 0)  
            md5.update(buffer, length);  
    }  
    in.close();  
    return md5.toString();  
}  

void CMyDlgFile::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	//上传请求

	CFileDialog dlg(TRUE);
	if(IDOK == dlg.DoModal())
	{
		CString strFileName = dlg.GetFileName();
		string sfilePath = dlg.GetPathName();
		string strFileMD5 = "" ;
		STRU_UPLOAD_RQ sur;
		//获得文件大小
		{
		CFile file(dlg.GetPathName(),CFile::modeRead);
		
		sur.m_nFileSize = file.GetLength();
		}
		sur.m_nType = _DEF_PROTOCOL_UPLOAD_RQ;
		strcpy_s(sur.m_fileType,dlg.GetFileExt());
		sur.m_nUserId = m_nUserId;
		strcpy_s(sur.m_szFileCurrentPath,dlg.GetPathName());
		//如果文件大于100M  ，则获得MD5
		if(sur.m_nFileSize > THRESHOLD)
		{
			strFileMD5 =  FileDigest(sfilePath);
		}
		strcpy_s(sur.m_szFileMD5,strFileMD5.c_str());
		strcpy_s(sur.m_szFileName,strFileName);
		
		//MultiByteToWideChar(CP_UTF8,0,strFileName,-1,(LPWSTR)sur.m_szFileName,sizeof(sur.m_szFileName));



		CTime t = CTime::GetCurrentTime();
		strcpy_s(sur.m_szFileUpTime,t.Format("%Y-%m-%d %H:%M:%S"));
		theApp.m_Mediator.SendData((char*)&sur,sizeof(sur));


		//保存要上传的文件信息
		FileInfo *pFileInfo = new FileInfo;
		pFileInfo->fileId = 0;
		pFileInfo->filePos = 0;
		strcpy_s(pFileInfo->m_szFileName,strFileName );
		strcpy_s(pFileInfo->m_szFilePath,sfilePath.c_str());
		pFileInfo->filesize = sur.m_nFileSize;
		strcpy_s(pFileInfo->m_szUpTime,sur.m_szFileUpTime);
		//memcpy();
		theApp.m_Mediator.m_mapFileNameToFileInfo[pFileInfo->m_szFileName] = pFileInfo;
		
	}
}


void CMyDlgFile::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	//获得当前选中项
	int nIndex = m_lstFile.GetSelectionMark();
	if(-1 == nIndex)return;

	FileDownLoadInfo *pNewInfo = new FileDownLoadInfo;
	CString strFileName =  m_lstFile.GetItemText(nIndex,0);
	CFileDialog dlg(FALSE,0,strFileName);
	if(IDOK != dlg.DoModal())
	{
		return ;
	}

	CString strPath = dlg.GetPathName();


	
	
	strcpy_s(pNewInfo->m_szFilePath,MAX_PATH,strPath);
	pNewInfo->m_nDownPos = 0;
	pNewInfo->m_nFileSize = _atoi64(m_lstFile.GetItemText(nIndex,1));
	m_mapFileNameToPath[strFileName] = pNewInfo;
	//文件下载请求
	STRU_DOWNLOAD_RQ sdr;
	sdr.m_nType = _DEF_PROTOCOL_DOWNLOAD_RQ;
	sdr.m_nUserId = m_nUserId;
	strcpy_s(sdr.m_szFileName,strFileName);

	theApp.m_Mediator.GetTCPNet()->SendData((char*)&sdr,sizeof(STRU_DOWNLOAD_RQ));

}


void CMyDlgFile::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	STRU_SHARELINK_RQ ssr;
	ssr.m_nType = _DEF_PROTOCOL_SHARE_RQ;
	ssr.m_nUserId = m_nUserId;
	int nIndex = m_lstFile.GetSelectionMark();
	if(-1 == nIndex)return;
	CString strFileName =  m_lstFile.GetItemText(nIndex,0);
	strcpy_s(ssr.m_szFileName,MAX_PATH,strFileName);

	theApp.m_Mediator.GetTCPNet()->SendData((char*)&ssr,sizeof(STRU_SHARELINK_RQ));
}

//分享链接
LRESULT CMyDlgFile::OnShowShareCode(WPARAM wparam,LPARAM lparam)
{
	m_edtLinkCode  = *(INT64*)wparam;

	UpdateData(FALSE);

	return 0;
}

//提取链接
void CMyDlgFile::OnBnClickedButton5()
{
	UpdateData();
	// TODO: 在此添加控件通知处理程序代码
	STRU_GETLINK_RQ sgr;
	sgr.m_nType = _DEF_PROTOCOL_GET_RQ;
	sgr.m_lLinkCode = m_edtLinkCode;
	sgr.m_nUserId = m_nUserId;

	theApp.m_Mediator.GetTCPNet()->SendData((char*)&sgr,sizeof(STRU_GETLINK_RQ));
}


void CMyDlgFile::OnBnClickedButton6()
{
	// TODO: 在此添加控件通知处理程序代码
	STRU_DELETE_RQ sdr;
	sdr.m_nType = _DEF_PROTOCOL_DELETEFILE_RQ;
	sdr.m_nUserId = m_nUserId;
	 m_nIndex = m_lstFile.GetSelectionMark();
	if(-1 == m_nIndex)return;
	CString strFileName =  m_lstFile.GetItemText(m_nIndex,0);
	strcpy_s(sdr.m_szFileName,MAX_PATH,strFileName);

	theApp.m_Mediator.GetTCPNet()->SendData((char*)&sdr,sizeof(STRU_DELETE_RQ));

}

LRESULT CMyDlgFile::OnDeleteFile(WPARAM wparam,LPARAM lparam)
{
	m_lstFile.DeleteItem(m_nIndex);
	m_nIndex = 0;
	return 0;
}
