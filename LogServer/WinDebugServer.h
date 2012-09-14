#pragma once

#include <Lock.h>

#include <vector>
#include "XString.h"

//////////////////////////////////////////////////////////////////////////
struct stDBWinBuffer
{
    DWORD   dwProcessId;
    char    data[4096-sizeof(DWORD)];
};

struct stInnerDBWinBuffer
{
    DWORD   dwProcessId;
    LogView::Util::XString strData;
};
typedef std::vector<stInnerDBWinBuffer> BufferVector;
//////////////////////////////////////////////////////////////////////////
class CWinDebugServer
{
public:
    CWinDebugServer(void);
    ~CWinDebugServer(void);

    BOOL Init();
    void UnInit();

    BufferVector    m_BufferVector;
    LogView::Util::CAiSection      m_Lock;

public:
    virtual void OutputWinDebugString(DWORD dwProcId, const char *str);

protected:
    static unsigned WINAPI WinDebugMonitorThread(void *pData);

    void WinDebugMonitorProc();

protected:
    HANDLE  m_hDBWinMutex;
    HANDLE  m_hDBMonBuffer;
    HANDLE  m_hEventBufferReady;
    HANDLE  m_hEventDataReady;

    BOOL    m_bWinDebugMonStopped;
    HANDLE  m_hWinDebugMonitorThread;

    BOOL    m_bServerInitSucceeded;
};
