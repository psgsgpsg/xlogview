
// LogView.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������

#include "../Common/LogServer.h"

// CLogViewApp:
// �йش����ʵ�֣������ LogView.cpp
//

#include "UIDefine.h"

class CLogViewApp : public CWinApp
{
public:
	CLogViewApp();

// ��д
public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()

    afx_msg void OnHelp();
};

extern CLogViewApp theApp;
