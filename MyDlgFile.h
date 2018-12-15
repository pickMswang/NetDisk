#pragma once

#include "resource.h"
#include "afxcmn.h"
#include <string>
#include <fstream>
#include <map>
#include "md5.h"
// CMyDlgFile 对话框
struct FileDownLoadInfo
{
char m_szFilePath[MAX_PATH];
INT64 m_nFileSize;
FILE *m_pFile;
INT64 m_nDownPos;
INT64 m_nFileId;
};

class CMyDlgFile : public CDialogEx
{
	DECLARE_DYNAMIC(CMyDlgFile)

public:
	CMyDlgFile(INT64  nUserId,CWnd* pParent = NULL);   // 标准构造函数
	//CMyDlgFile(INT64  nUserId);   // 标准构造函数
	virtual ~CMyDlgFile();

// 对话框数据
	enum { IDD = IDD_DLG_FILE };
	LRESULT OnShowFileList(WPARAM wparam,LPARAM lparam);
	LRESULT OnShowShareCode(WPARAM wparam,LPARAM lparam);
	LRESULT OnDeleteFile(WPARAM wparam,LPARAM lparam);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	std::string FileDigest(const std::string &file);
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_lstFile;
	INT64     m_nUserId;
	int       m_nIndex;
	
	std::map<CString,FileDownLoadInfo*> m_mapFileNameToPath;
	
	ULONGLONG m_edtLinkCode;
	
};
