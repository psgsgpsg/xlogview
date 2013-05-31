#pragma once

#include "../Common/LogServer.h"
#include "../Common/Impl/LogStream.h"

#include <vector>

class CLogServerImpl
{
public:
    CLogServerImpl();
    ~CLogServerImpl();

public:
    BOOL StartLogServer(LPCTSTR szName);
    void SetLogListener(ILogListener* pListener);
    void StopLogServer();
    void AppendLog(const stLogData& data);

protected:
    static CLogServerImpl*  GetAssocedObj(HWND hWnd);
    static void             AssocObj(HWND hWnd, CLogServerImpl* pObj);

protected:
    static LRESULT CALLBACK LogServerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    void OnLogArrived(WPARAM wParam, LPARAM lParam);

protected:
    HWND    m_hLogWnd;
    ILogListener*   m_pLogListener;
};



