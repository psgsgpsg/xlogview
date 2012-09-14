#pragma once
#include "../Common/LogServer.h"

#include <vector>
#include "WinDebugServer.h"

class CLogServerImpl
{
public:
    CLogServerImpl();
    ~CLogServerImpl();

public:
    BOOL InitLogServer(LPCTSTR szName);
    void AddLogListener(ILogListener* pListener);
    void DelLogListener(ILogListener* pListener);
    void UnInitLogServer();
    void OnLogArrived(const LogView::Util::stLogInfo& info);

    BOOL StartLogOutputDebugString();
    void StopLogOutputDebugString();

    static CLogServerImpl*  GetObject(LPCTSTR szSig);

    HWND GetLogServerImplWnd();

    static void ShowExistedLogServer(LPCTSTR szName);

protected:
    static CLogServerImpl*  GetAssocedObj(HWND hWnd);
    static void             AssocObj(HWND hWnd, CLogServerImpl* pObj);

protected:
    static LRESULT CALLBACK LogServerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    void OnLogArrived(WPARAM wParam, LPARAM lParam);
    void OnOutputDebugStringTimer();

protected:
    HWND    m_hLogWnd;
    LogView::Util::XString m_strServerName;

    typedef std::vector<ILogListener*>  LogListenerVector;
    LogListenerVector   m_vctLogListener;

    UINT                m_uTimerId;
    CWinDebugServer     m_WinDebugServer;
};

HWND GetLogServerImplWnd();