#include "StdAfx.h"

#include "../Common/LogClient.h"
#include "../Common/LogDef.h"

#include "../Common/Impl/XLogMgr.h"

#include <queue>
#include <cassert>

//////////////////////////////////////////////////////////////////////////

namespace
{
    BOOL g_bNeedSaveFile = FALSE;
    LogViewInternal::tstring g_strLocalLogFileLevel = _T("");
}

//////////////////////////////////////////////////////////////////////////

namespace LogViewInternal
{
    namespace Impl
    {

        HWND FindLogServer()
        {
            HWND hWnd = FindWindow(LOG_SERVER_PORT_SIG, LOG_SERVER_PORT_NAME);
            return hWnd;
        }

        class CLock
        {
        public:
            CLock(bool bManual)
            {
                m_Lock = ::CreateEvent(NULL, bManual, TRUE, NULL);
            }
            ~CLock()
            {
                ::CloseHandle(m_Lock);
                m_Lock = NULL;
            }
            BOOL Wait()
            {
                return (WAIT_OBJECT_0 == ::WaitForSingleObject(m_Lock, INFINITE));
            }
            void SetSignal(BOOL bSignaled)
            {
                if(bSignaled)
                    ::SetEvent(m_Lock);
                else
                    ::ResetEvent(m_Lock);
            }
        private:
            HANDLE  m_Lock;
        };

        class CAutoLock
        {
        public:
            CAutoLock(CLock* pLock)
            {
                m_pLock = pLock;
                m_pLock->Wait();
            }
            ~CAutoLock()
            {
                m_pLock->SetSignal(TRUE);
            }
        private:
            CLock*  m_pLock;
        };

        class CLogQueue
        {
        public:
            CLogQueue() : m_EmptyLock(true), m_DataLock(false)
            {
                m_EmptyLock.SetSignal(FALSE);
            }
            ~CLogQueue()
            {
            }
            void Destroy()
            {
                m_EmptyLock.SetSignal(TRUE);
            }
            void PushLog(LogViewInternal::stLogInfo& log)
            {
                CAutoLock lock(&m_DataLock);

                m_LogQueue.push(log);
                m_EmptyLock.SetSignal(TRUE);
            }
            void PopLog(LogViewInternal::stLogInfo& log)
            {
                CAutoLock lock(&m_DataLock);

                log = m_LogQueue.front();
                m_LogQueue.pop();
                if(m_LogQueue.empty())
                    m_EmptyLock.SetSignal(FALSE);
            }
            BOOL Wait()
            {
                return m_EmptyLock.Wait();
            }

        private:
            typedef std::queue<LogViewInternal::stLogInfo> LogQueue;
            LogQueue    m_LogQueue;
            CLock       m_DataLock;
            CLock       m_EmptyLock;
        };

        class CLogBuffer
        {
            CLogBuffer(const CLogBuffer&);
            CLogBuffer& operator =(const CLogBuffer&);
        public:
            CLogBuffer()
            {
                m_bStopLogThread = FALSE;
                m_hLogThread = (HANDLE)_beginthreadex(0, 0, &LogThreadProc, (void*)this, 0, 0);
            }
            ~CLogBuffer()
            {
                if(m_hLogThread != NULL)
                {
                    m_bStopLogThread = TRUE;
                    m_LogQueue.Destroy();

                    ::WaitForSingleObject(m_hLogThread, INFINITE);
                    ::CloseHandle(m_hLogThread);
                    m_hLogThread = NULL;
                }
            }

            void SetOutputLogPath(LPCTSTR szPath, LPCTSTR szLevel)
            {
                if(szPath == NULL || szLevel == NULL)
                {
                    g_bNeedSaveFile = FALSE;
                }
                else
                {
                    g_strLocalLogFileLevel = szLevel;
                    g_bNeedSaveFile = m_LogAppender.OpenXLog(szPath, TRUE);
                }
            }

            void PushLog(LogViewInternal::stLogInfo& log)
            {
                m_LogQueue.PushLog(log);
            }

        protected:
            static unsigned WINAPI LogThreadProc(void* pParam)
            {
                HWND hLogServerWnd = FindLogServer();
                CLogBuffer* pLogBuffer = (CLogBuffer*)pParam;
                LogViewInternal::stLogInfo log;

                while(pLogBuffer->m_LogQueue.Wait() && !pLogBuffer->m_bStopLogThread)
                {
                    pLogBuffer->m_LogQueue.PopLog(log);

                    if(!::IsWindow(hLogServerWnd))
                        hLogServerWnd = FindLogServer();

                    if(hLogServerWnd != NULL)
                    {
                        COPYDATASTRUCT CopyData = {0};
                        CopyData.dwData = LOG_IPC_MAGIC_NUM;
                        CopyData.cbData = log.dwRealSize;
                        CopyData.lpData = log.pBuffer;
                        ::SendMessage(hLogServerWnd, WM_COPYDATA, (WPARAM)hLogServerWnd, (LPARAM)&CopyData);
                    }
                    if(g_bNeedSaveFile)
                    {
                        BOOL bNeedSave = (g_strLocalLogFileLevel == _T("*"));
                        if(!bNeedSave)
                        {
                            LPCTSTR szLevel = LogViewInternal::LogInfo::GetLogLevel(log);
                            bNeedSave = (g_strLocalLogFileLevel.find(szLevel) != LogViewInternal::tstring::npos);
                        }
                        if(bNeedSave)
                            pLogBuffer->m_LogAppender.AppendLog(log);
                    }

                    LogViewInternal::LogInfo::ReleaseLog(log);
                }
                return 0;
            }

        protected:
            CLogQueue       m_LogQueue;

            HANDLE          m_Lock;
            volatile BOOL   m_bStopLogThread;
            HANDLE          m_hLogThread;

            LogViewInternal::XLogAppender    m_LogAppender;
        };
    }
}


//////////////////////////////////////////////////////////////////////////

LogViewInternal::Impl::CLogBuffer& GetLogBuffer()
{
    static LogViewInternal::Impl::CLogBuffer g_LogBuffer;
    return g_LogBuffer;
}

// 设置显示Log保存文件路径和级别
void SetOutputLogPathImpl(LPCTSTR szPath, LPCTSTR szLevel)
{
    GetLogBuffer().SetOutputLogPath(szPath, szLevel);
}

//////////////////////////////////////////////////////////////////////////
// 打印Log
BOOL OutputLogImpl(LPCTSTR szLevel, LPCTSTR szFilter, LPCTSTR szFormat, ...)
{
    LogViewInternal::stLogInfo log;
    LPTSTR szLog = LogViewInternal::LogInfo::GetLogBuffer(log, szLevel, szFilter);
    if(szLog == NULL)
        return FALSE;

    va_list vlist;
    va_start(vlist, szFormat);

    HRESULT hResult = StringCchVPrintf(szLog, MAX_LOG_LENGTH, szFormat, vlist);
    if(!SUCCEEDED(hResult))
        return FALSE;

    szLog[MAX_LOG_LENGTH - 1] = 0;
    LogViewInternal::LogInfo::SetLog(log, szLog);

    GetLogBuffer().PushLog(log);

    return TRUE;
}
