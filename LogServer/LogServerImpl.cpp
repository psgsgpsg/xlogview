#include "StdAfx.h"
#include "LogServerImpl.h"


#include "../Common/Impl/XLogMgr.h"
#include "../Common/LogServer.h"


//////////////////////////////////////////////////////////////////////////

namespace 
{
    BOOL FilterWindowMessage(UINT message, DWORD dwValue)
    {
        // 添加消息过滤，允许接受低级别的Explorer发送WM_TASKBARCREATED消息
        typedef BOOL (WINAPI FAR *ChangeWindowMessageFilter_PROC)(UINT,DWORD);
        ChangeWindowMessageFilter_PROC pfnChangeWindowMessageFilter;
        pfnChangeWindowMessageFilter = (ChangeWindowMessageFilter_PROC)::GetProcAddress (::GetModuleHandle(_T("USER32")), "ChangeWindowMessageFilter");
        BOOL bResult = FALSE;
        if(pfnChangeWindowMessageFilter != NULL)
        {
            bResult = pfnChangeWindowMessageFilter(message, dwValue);
        }
        else
        {
            bResult = TRUE;
        }
        return bResult;
    }
    LPCTSTR g_szLogServerPropName = _T("LJWFDSQGT");
}

//////////////////////////////////////////////////////////////////////////
CLogServerImpl::CLogServerImpl()
{
    m_hLogWnd = NULL;
    m_pLogListener = NULL;
}

CLogServerImpl::~CLogServerImpl()
{
    ;
}

BOOL CLogServerImpl::StartLogServer(LPCTSTR szName)
{
    BOOL bResult = FALSE;

    FilterWindowMessage(WM_COPYDATA, TRUE);

    // Register Class
    {
        WNDCLASSEX WndClassEx = {};
        WndClassEx.cbSize = sizeof(WndClassEx);
        WndClassEx.lpszClassName = LOG_SERVER_PORT_SIG;
        WndClassEx.lpfnWndProc = (WNDPROC)&CLogServerImpl::LogServerWndProc;

        ATOM atomResult = RegisterClassEx(&WndClassEx);

        DWORD dwError = GetLastError();
        bResult = (atomResult != 0 || dwError == ERROR_CLASS_ALREADY_EXISTS);
    }
    if(!bResult)
        return FALSE;

    // Create Window
    {
        m_hLogWnd = CreateWindow(LOG_SERVER_PORT_SIG, szName, WS_POPUP, 0, 0, 0, 0, HWND_MESSAGE, 0, GetModuleHandle(NULL), 0);
        AssocObj(m_hLogWnd, this);
    }

    return (m_hLogWnd != NULL);
}

void CLogServerImpl::SetLogListener(ILogListener* pListener)
{
    m_pLogListener = pListener;
}

void CLogServerImpl::StopLogServer()
{
    if(m_pLogListener)
    {
        m_pLogListener->Release();
        m_pLogListener = NULL;
    }
}

CLogServerImpl* CLogServerImpl::GetAssocedObj(HWND hWnd)
{
    CLogServerImpl* pLogServer = (CLogServerImpl*)GetProp(hWnd, g_szLogServerPropName);
    return pLogServer;
}

void CLogServerImpl::AssocObj(HWND hWnd, CLogServerImpl* pObj)
{
    SetProp(hWnd, g_szLogServerPropName, (HANDLE)pObj);
}

LRESULT CLogServerImpl::LogServerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if(message == WM_COPYDATA)
    {
        CLogServerImpl* pLogServer = GetAssocedObj(hWnd);
        if(pLogServer != NULL)
        {
            pLogServer->OnLogArrived(wParam, lParam);
        }
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

void CLogServerImpl::OnLogArrived(WPARAM wParam, LPARAM lParam)
{
    if(m_pLogListener == NULL)
        return;

    COPYDATASTRUCT* pData = (COPYDATASTRUCT*)lParam;
    if(pData == NULL || pData->dwData != LOG_IPC_MAGIC_NUM)
        return;

    stLogData data;
    if(LogViewInternal::LogInfo::ParseLog(pData->lpData, pData->cbData, data))
    {
        AppendLog(data);
    }
}

void CLogServerImpl::AppendLog(const stLogData& data)
{
    if(m_pLogListener)
        m_pLogListener->OnLogArrived(data);
}

//////////////////////////////////////////////////////////////////////////

namespace
{
    CLogServerImpl* g_LogServer = NULL;
}

BOOL XStartLogServer(LPCTSTR szName)
{
    if(g_LogServer)
        return TRUE;

    g_LogServer = new CLogServerImpl;
    return g_LogServer->StartLogServer(szName);
}

void XSetLogListener(ILogListener* pListener)
{
    if(g_LogServer)
        g_LogServer->SetLogListener(pListener);
}

void XStopLogServer()
{
    if(g_LogServer)
    {
        g_LogServer->StopLogServer();
        delete g_LogServer;
        g_LogServer = NULL;
    }
}

void XAppendLog(const stLogData& log, void* pData)
{
    if(g_LogServer)
        g_LogServer->AppendLog(log);
}

BOOL XOpenLogFile(LPCTSTR szFile, NewLogCallback callback, void* pData)
{
    return LogViewInternal::XLogMgr::OpenXLog(szFile, callback, pData);
}

BOOL XSaveLogFile(LPCTSTR szFile, QueryLogCallback callback, void* pData)
{
    return LogViewInternal::XLogMgr::SaveXLog(szFile, callback, pData);
}
