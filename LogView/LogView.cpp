
// LogView.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "LogView.h"
#include "LogViewDlg.h"

#include "AboutDlg.h"
#include "Config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLogViewApp

BEGIN_MESSAGE_MAP(CLogViewApp, CWinApp)
    ON_COMMAND(ID_HELP, &CLogViewApp::OnHelp)
END_MESSAGE_MAP()


// CLogViewApp ����

CLogViewApp::CLogViewApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CLogViewApp ����

CLogViewApp theApp;


// CLogViewApp ��ʼ��

BOOL CLogViewApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

    HANDLE hMutex = CreateMutex(NULL, FALSE, LOG_SERVER_PORT_NAME _T("LogView_46B03C6E_1843_4d76_81BB_A64BC07F8D2E"));
    if(hMutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
        return FALSE;

    CConfig::GetConfig().ReloadConfig();

    // ��ʼ����־�������
    XStartLogServer(LOG_SERVER_PORT_NAME);

    CLogViewDlg dlg;
    XSetLogListener(&dlg);

	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();

	return FALSE;
}

int CLogViewApp::ExitInstance()
{
    XStopLogServer();

    CConfig::GetConfig().SaveConfig();

    return CWinApp::ExitInstance();
}

void CLogViewApp::OnHelp()
{
    CAboutDlg dlg;
    dlg.DoModal();
}
