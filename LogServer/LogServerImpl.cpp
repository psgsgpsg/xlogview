#include "StdAfx.h"
#include "LogServerImpl.h"


#include "../Common/LogServer.h"

static const UINT g_uShowMainFrame = RegisterWindowMessage(_T("ShowMainFrame_8D855A82_BEE8_4fdf_BF3C_F39D94F6A9B2"));


//////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////
CLogServerImpl::CLogServerImpl()
{
    m_hLogWnd = NULL;
    m_uTimerId = 1234;
}

CLogServerImpl::~CLogServerImpl()
{
    ;
}

BOOL CLogServerImpl::InitLogServer(LPCTSTR szName)
{
    BOOL bResult = FALSE;

    FilterWindowMessage(g_uShowMainFrame, TRUE);
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
        m_uTimerId = SetTimer(m_hLogWnd, m_uTimerId, 500, NULL);

        m_strServerName = szName;
    }

    // m_WinDebugServer.Init();

    return (m_hLogWnd != NULL);
}

void CLogServerImpl::AddLogListener(ILogListener* pListener)
{
    m_vctLogListener.push_back(pListener);
}

void CLogServerImpl::DelLogListener(ILogListener* pListener)
{
    ILogListener* pTemp = NULL;
    LogListenerVector::iterator IteListener = m_vctLogListener.begin();
    for(; IteListener != m_vctLogListener.end(); ++ IteListener)
    {
        pTemp = (*IteListener);
        if(pTemp == pListener)
        {
            pTemp->Release();
            m_vctLogListener.erase(IteListener);
            break;
        }
    }
}

void CLogServerImpl::UnInitLogServer()
{
    m_WinDebugServer.UnInit();

    ILogListener* pTemp = NULL;
    LogListenerVector::iterator IteListener = m_vctLogListener.begin();
    for(; IteListener != m_vctLogListener.end(); ++ IteListener)
    {
        pTemp = (*IteListener);
        pTemp->Release();
    }
    m_vctLogListener.clear();
}

CLogServerImpl* CLogServerImpl::GetObject(LPCTSTR szName)
{
    CLogServerImpl* pLogServer = NULL;
    HWND hLogServerWnd = FindWindowEx(NULL, NULL, LOG_SERVER_PORT_SIG, szName);

    DWORD dwPid = 0;
    while(hLogServerWnd != NULL && GetWindowThreadProcessId(hLogServerWnd, &dwPid) != 0 && dwPid != GetCurrentProcessId())
    {
        hLogServerWnd = FindWindowEx(NULL, hLogServerWnd, LOG_SERVER_PORT_SIG, szName);
    }

    if(hLogServerWnd != NULL)
    {
        pLogServer = GetAssocedObj(hLogServerWnd);
    }
    return pLogServer;
}

HWND CLogServerImpl::GetLogServerImplWnd()
{
    return m_hLogWnd;
}

CLogServerImpl* CLogServerImpl::GetAssocedObj(HWND hWnd)
{
    CLogServerImpl* pLogServer = (CLogServerImpl*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    return pLogServer;
}

void CLogServerImpl::AssocObj(HWND hWnd, CLogServerImpl* pObj)
{
    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pObj);
}

BOOL CALLBACK ShowWindowCallback(HWND hWnd, LPARAM)
{
    if(IsWindowVisible(hWnd) && GetParent(hWnd) == NULL)
    {
        ::ShowWindow(hWnd, SW_SHOWNORMAL);
        ::SetForegroundWindow(hWnd);
        ::SetActiveWindow(hWnd);
    }
    return TRUE;
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
    else if(g_uShowMainFrame == message)
    {
        EnumThreadWindows(GetCurrentThreadId(), &ShowWindowCallback, 0);
    }
    else if(WM_TIMER == message)
    {
        CLogServerImpl* pLogServer = GetAssocedObj(hWnd);
        if(pLogServer != NULL && wParam == pLogServer->m_uTimerId)
        {
            pLogServer->OnOutputDebugStringTimer();
        }
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

void CLogServerImpl::OnLogArrived(WPARAM wParam, LPARAM lParam)
{
    COPYDATASTRUCT* pData = (COPYDATASTRUCT*)lParam;
    if(pData == NULL)
        return;

    DWORD dwThreadId = 0;
    LogView::Util::CDataBuffer data;
    data.Reset((LPCTSTR)pData->lpData, pData->cbData / sizeof(TCHAR));

    LogView::Util::stLogInfo info;
    data >> info;

    OnLogArrived(info);
}

void CLogServerImpl::OnOutputDebugStringTimer()
{
    AI_AUTO_LOCK(&m_WinDebugServer.m_Lock);

    BufferVector& _Buffer = m_WinDebugServer.m_BufferVector;
    BufferVector::iterator IteData = _Buffer.begin();
    for(; IteData != _Buffer.end(); ++ IteData)
    {
        const stInnerDBWinBuffer& data = *IteData;

        LogView::Util::stLogInfo info;

        info.dwProcId = data.dwProcessId;
        info.dwThreadId = 0;
        info.strFilter = _T("OutputDebugString");
        info.strLog = data.strData;
        info.uLevel = 1234;

        OnLogArrived(info);
    }
    _Buffer.clear();
}

void CLogServerImpl::OnLogArrived(const LogView::Util::stLogInfo& info)
{
    ILogListener* pTemp = NULL;
    LogListenerVector::iterator IteListener = m_vctLogListener.begin();
    for(; IteListener != m_vctLogListener.end(); ++ IteListener)
    {
        pTemp = (*IteListener);
        pTemp->OnLogArrived(info);
    }
}

void CLogServerImpl::ShowExistedLogServer(LPCTSTR szName)
{
    HWND hLogServerWnd = FindWindowEx(NULL, NULL, LOG_SERVER_PORT_SIG, szName);

    DWORD dwPid = 0;
    while(hLogServerWnd != NULL)
    {
        ::PostMessage(hLogServerWnd, g_uShowMainFrame, 0, 0);
        hLogServerWnd = FindWindowEx(NULL, hLogServerWnd, LOG_SERVER_PORT_SIG, szName);
    }
}

BOOL CLogServerImpl::StartLogOutputDebugString()
{
    return m_WinDebugServer.Init();
}

void CLogServerImpl::StopLogOutputDebugString()
{
    m_WinDebugServer.UnInit();
}

HWND GetLogServerImplWnd()
{
    CLogServerImpl* pLogServer = CLogServerImpl::GetObject(NULL);
    return pLogServer->GetLogServerImplWnd();
}
//////////////////////////////////////////////////////////////////////////

BOOL InitLogServer(LPCTSTR szName)
{
    CLogServerImpl* pLogServer = new CLogServerImpl;
    return pLogServer->InitLogServer(szName);
}

void AddLogListener(ILogListener* pListener)
{
    CLogServerImpl* pLogServer = CLogServerImpl::GetObject(NULL);
    if(pLogServer != NULL)
        pLogServer->AddLogListener(pListener);
}

void DelLogListener(ILogListener* pListener)
{
    CLogServerImpl* pLogServer = CLogServerImpl::GetObject(NULL);
    if(pLogServer != NULL)
        pLogServer->DelLogListener(pListener);
}

void UnInitLogServer()
{
    CLogServerImpl* pLogServer = CLogServerImpl::GetObject(NULL);
    if(pLogServer != NULL)
        delete pLogServer;
}

void OnLogArrived(const LogView::Util::stLogInfo& info)
{
    CLogServerImpl* pLogServer = CLogServerImpl::GetObject(NULL);
    if(pLogServer != NULL)
        pLogServer->OnLogArrived(info);
}

void ShowExistedLogServer(LPCTSTR szName)
{
    CLogServerImpl::ShowExistedLogServer(szName);
}

BOOL StartLogOutputDebugString()
{
    CLogServerImpl* pLogServer = CLogServerImpl::GetObject(NULL);
    if(pLogServer != NULL)
        return pLogServer->StartLogOutputDebugString();
    return FALSE;
}

void StopLogOutputDebugString()
{
    CLogServerImpl* pLogServer = CLogServerImpl::GetObject(NULL);
    if(pLogServer != NULL)
        pLogServer->StopLogOutputDebugString();
}