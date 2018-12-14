
// 0804Disk.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include "Mediator.h"

// CMy0804DiskApp:
// 有关此类的实现，请参阅 0804Disk.cpp
//

class CMy0804DiskApp : public CWinApp
{
public:
	CMy0804DiskApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
public:
	CMediator  m_Mediator;
};

extern CMy0804DiskApp theApp;
