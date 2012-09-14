
// LogView.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号

#include "../Common/LogServer.h"

// CLogViewApp:
// 有关此类的实现，请参阅 LogView.cpp
//

#include "UIDefine.h"

class CLogViewApp : public CWinApp
{
public:
	CLogViewApp();

// 重写
public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()

    afx_msg void OnHelp();
};

extern CLogViewApp theApp;
