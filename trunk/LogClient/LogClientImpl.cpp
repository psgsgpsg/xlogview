#include "StdAfx.h"

#include "../Common/LogClient.h"
#include "../Common/LogDef.h"
#include "Lock.h"
#include <DataBuffer.h>
#include <process.h>
#include <queue>
#include "../Common/XLogMgr.h"

#include <strsafe.h>
#pragma warning(disable: 4996)

//////////////////////////////////////////////////////////////////////////

static TCHAR g_szServerName[MAX_PATH] = LOG_SERVER_PORT_NAME;
static BOOL  g_bNeedSaveFile = FALSE;
static DWORD g_dwLocalLogFileLevel = XLOG_LEVEL_ALL;

//////////////////////////////////////////////////////////////////////////

namespace LogView
{
    namespace Impl
    {
        using namespace Util;

        HWND FindLogServer()
        {
            HWND hWnd = FindWindow(LOG_SERVER_PORT_SIG, g_szServerName);
            return hWnd;
        }


        class CLogBuffer
        {
            struct stBufferInfo
            {
                XString buffer;
                DWORD    dwThreadId;
            };

            CLogBuffer(const CLogBuffer&);
            CLogBuffer& operator =(const CLogBuffer&);
        public:
            CLogBuffer(){}
            ~CLogBuffer()
            {
                if(m_hLogThread != NULL)
                {
                    ::TerminateThread(m_hLogThread, 0);
                    m_hLogThread = NULL;
                }
            }

            void SetOutputServer(LPCTSTR szName)
            {
                AI_AUTO_LOCK(&m_Lock);
                _tcsncpy(g_szServerName, szName, MAX_PATH);
            }

            void SetOutputLogPath(LPCTSTR szPath, DWORD dwLogLevel)
            {
                AI_AUTO_LOCK(&m_Lock);
                g_dwLocalLogFileLevel = dwLogLevel;
                g_bNeedSaveFile = (g_dwLocalLogFileLevel != XLOG_LEVEL_NONE);

                m_LogAppender.OpenXLog(szPath, TRUE);
            }

            void PushLog(XString& buffer)
            {
                stBufferInfo info;
                info.buffer = buffer;
                info.dwThreadId = ::GetCurrentThreadId();

                HWND hWnd = FindLogServer();
                COPYDATASTRUCT CopyData = {0};
                CopyData.dwData = info.dwThreadId;
                CopyData.cbData = (info.buffer.size() + 1) * sizeof(TCHAR);
                CopyData.lpData = (LPVOID)info.buffer.c_str();
                ::SendMessage(hWnd, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&CopyData);
            }

        protected:
            static unsigned WINAPI LogThreadProc(void* pParam)
            {
                CLogBuffer* pLogBuffer = (CLogBuffer*)pParam;
                pLogBuffer->OnLogThread();
                return 0;
            }

            void OnLogThread()
            {
                while(TRUE)
                {
                    Sleep(200);

                    {
                        AI_AUTO_LOCK(&m_Lock);

                        if(m_LogBufferQueue.size() > 0)
                        {
                            HWND hWnd = FindLogServer();

                            if(hWnd == NULL && !g_bNeedSaveFile)
                            {
                                while(!m_LogBufferQueue.empty())
                                    m_LogBufferQueue.pop();
                                continue;
                            }

                            size_t nLogCount = 0;
                            for(size_t nLogCount=0; nLogCount<10 && !m_LogBufferQueue.empty(); ++ nLogCount)
                            {
                                stBufferInfo& info = m_LogBufferQueue.front();
                                if(hWnd != NULL)
                                {
                                    COPYDATASTRUCT CopyData = {0};
                                    CopyData.dwData = info.dwThreadId;
                                    CopyData.cbData = (info.buffer.size() + 1) * sizeof(TCHAR);
                                    CopyData.lpData = (LPVOID)info.buffer.c_str();
                                    ::SendMessage(hWnd, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&CopyData);
                                }

                                if(g_bNeedSaveFile)
                                {
                                    stLogInfo log;
                                    CDataBuffer Data;
                                    Data.Reset(info.buffer.GetData(), info.buffer.Length());
                                    Data >> log.dwProcId >> log.dwThreadId >> log.uLevel >> log.strFilter >> log.strLog;
                                    m_LogAppender.AppendLog(log);
                                }
                                m_LogBufferQueue.pop();
                            }
                        }
                    }
                }
            }

        protected:
            typedef std::queue<stBufferInfo> LogBufferQueue;
            LogBufferQueue  m_LogBufferQueue;

            HANDLE          m_hLogThread;
            CAiSection      m_Lock;
            XLogAppender    m_LogAppender;
        };
    }
}


//////////////////////////////////////////////////////////////////////////

LogView::Impl::CLogBuffer& GetLogBuffer()
{
    static LogView::Impl::CLogBuffer g_LogBuffer;
    return g_LogBuffer;
}

// 设置接收服务器名字
void SetOutputServerImpl(LPCTSTR szName)
{
    GetLogBuffer().SetOutputServer(szName);
}

// 设置显示Log保存文件路径和级别
void SetOutputLogPathImpl(LPCTSTR szPath, DWORD dwLogLevel)
{
    GetLogBuffer().SetOutputLogPath(szPath, dwLogLevel);
}

//////////////////////////////////////////////////////////////////////////
// 打印Log
BOOL OutputLogImpl(UINT uLevel, LPCTSTR szFilter, LPCTSTR szFormat, ...)
{
    if(g_dwLocalLogFileLevel == XLOG_LEVEL_NONE)
        return TRUE;

    va_list vlist;
    va_start(vlist, szFormat);

    TCHAR szBuffer[MAX_LOG_LENGTH] = {0};
    HRESULT hResult = StringCchVPrintf(szBuffer, MAX_LOG_LENGTH, szFormat, vlist);
    if(!SUCCEEDED(hResult))
        return FALSE;

    std::wstring str1;
    LogView::Util::XString str2;
    LogView::Util::CDataBuffer data;

    data << GetCurrentThreadId();
    data << GetCurrentProcessId();
    data << uLevel << szFilter << szBuffer;

    LogView::Util::XString strMsg = data.ToString();
    GetLogBuffer().PushLog(strMsg);

    return TRUE;
}
