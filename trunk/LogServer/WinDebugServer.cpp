#include "StdAfx.h"
#include "WinDebugServer.h"
#include "../Common/LogServer.h"

#include "LogServerImpl.h"
#include "../Common/XStrUtil.h"

#include <process.h>

LPCTSTR DBWinMutex = _T("DBWinMutex");
LPCTSTR DBWIN_BUFFER = _T("DBWIN_BUFFER");
LPCTSTR DBWIN_BUFFER_READY = _T("DBWIN_BUFFER_READY");
LPCTSTR DBWIN_DATA_READY = _T("DBWIN_DATA_READY");

const DWORD g_dwEventDataWaitDelay = 200;

CWinDebugServer::CWinDebugServer(void)
{
    m_hDBWinMutex = NULL;
    m_hDBMonBuffer = NULL;
    m_hEventBufferReady = NULL;
    m_hEventDataReady = NULL;

    m_bWinDebugMonStopped = FALSE;
    m_hWinDebugMonitorThread = NULL;

    m_bServerInitSucceeded = FALSE;
}

CWinDebugServer::~CWinDebugServer(void)
{
    m_bServerInitSucceeded = FALSE;
}

BOOL CWinDebugServer::Init()
{
    if(m_bServerInitSucceeded)
        return m_bServerInitSucceeded;

    m_bServerInitSucceeded = FALSE;
    do 
    {
        m_hDBWinMutex = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, DBWinMutex);
        if(m_hDBWinMutex == NULL)
            break;

        m_hEventBufferReady = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, DBWIN_BUFFER_READY);
        if(m_hEventBufferReady == NULL)
        {
            m_hEventBufferReady = ::CreateEvent(NULL, FALSE, FALSE, DBWIN_BUFFER_READY);
            if(m_hEventBufferReady == NULL)
                break;
        }
        else
        {
            break;
        }

        m_hEventDataReady = ::OpenEvent(SYNCHRONIZE, FALSE, DBWIN_DATA_READY);
        if(m_hEventDataReady == NULL)
        {
            m_hEventDataReady = ::CreateEvent(NULL, FALSE, FALSE, DBWIN_DATA_READY);
            if (m_hEventDataReady == NULL)
                break;
        }

        m_hDBMonBuffer = ::OpenFileMapping(FILE_MAP_READ, FALSE, DBWIN_BUFFER);
        if(m_hDBMonBuffer == NULL)
        {
            m_hDBMonBuffer = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(struct stDBWinBuffer), DBWIN_BUFFER);
            if (m_hDBMonBuffer == NULL)
                break;
        }

        m_bServerInitSucceeded = TRUE;
    } while (FALSE);

    if(m_bServerInitSucceeded)
    {
        m_hWinDebugMonitorThread = (HANDLE)_beginthreadex(0, 0, &CWinDebugServer::WinDebugMonitorThread, (void*)this, 0, 0);
    }
    else
    {
        UnInit();
    }

    return m_bServerInitSucceeded;
}

void CWinDebugServer::UnInit()
{
    m_bServerInitSucceeded = FALSE;

    if(m_hWinDebugMonitorThread != NULL)
    {
        m_bWinDebugMonStopped = TRUE;
        WaitForSingleObject(m_hWinDebugMonitorThread, INFINITE);
        CloseHandle(m_hWinDebugMonitorThread);
        m_hWinDebugMonitorThread = NULL;
    }

    if(m_hEventDataReady != NULL)
    {
        WaitForSingleObject(m_hEventDataReady, INFINITE);
        CloseHandle(m_hEventDataReady);
        m_hEventDataReady = NULL;
    }

    if(m_hEventBufferReady != NULL)
    {
        SetEvent(m_hEventBufferReady);
        CloseHandle(m_hEventBufferReady);
        m_hEventBufferReady = NULL;
    }

    if(m_hDBMonBuffer != NULL)
    {
        CloseHandle(m_hDBMonBuffer);
        m_hDBMonBuffer = NULL;
    }

    if(m_hDBWinMutex != NULL)
    {
        CloseHandle(m_hDBWinMutex);
        m_hDBWinMutex = NULL;
    }
}

unsigned CWinDebugServer::WinDebugMonitorThread(void *pData)
{
    CWinDebugServer* pThis = (CWinDebugServer*)pData;
    pThis->WinDebugMonitorProc();
    return 0;
}

void CWinDebugServer::WinDebugMonitorProc()
{
    DWORD dwResult = 0;
    stDBWinBuffer* pBuffer = NULL;
    while(!m_bWinDebugMonStopped)
    {
        SetEvent(m_hEventBufferReady);
        dwResult = ::WaitForSingleObject(m_hEventDataReady, g_dwEventDataWaitDelay);
        if(dwResult == WAIT_OBJECT_0)
        {
            pBuffer = (stDBWinBuffer*)MapViewOfFile(m_hDBMonBuffer, SECTION_MAP_READ, 0, 0, 0);
            if(pBuffer != NULL)
            {
                OutputWinDebugString(pBuffer->dwProcessId, pBuffer->data);
                UnmapViewOfFile(pBuffer);
            }
        }
    }
}

void CWinDebugServer::OutputWinDebugString(DWORD dwProcId, const char *str)
{
    if(dwProcId == GetCurrentProcessId())
    {
        return;
    }

    AI_AUTO_LOCK(&m_Lock);

    stInnerDBWinBuffer buffer;
    buffer.dwProcessId = dwProcId;
    buffer.strData = LogView::Util::XStrUtil::AnsiToStr(str);

    m_BufferVector.push_back(buffer);
}