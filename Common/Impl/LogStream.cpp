#include "stdafx.h"
#include "LogStream.h"

#include "LogDef.h"

namespace LogViewInternal
{
    namespace
    {
        static const DWORD g_dwMaxLogCount = 50;
        static const DWORD g_dwSingleLogSize = (MAX_LEVEL_LENGTH + MAX_FILTER_LENGTH + MAX_LOG_LENGTH) * sizeof(TCHAR) + sizeof(size_t) * 3 + sizeof(DWORD) * 2 + sizeof(SYSTEMTIME);
        static BYTE g_LogBuffer[g_dwSingleLogSize * g_dwMaxLogCount] = {0};
        static volatile LONG g_LogBufferBitmap[g_dwMaxLogCount] = {0};
        static volatile LONG g_LogBufferIndex = 0;

        class CPooledBuffer
        {
        public:
            void* GetPooledBuffer()
            {
                void* pBuffer = NULL;
                LONG nIndex = ::InterlockedExchangeAdd(&g_LogBufferIndex, 0);
                BOOL bExceedMax = FALSE;
                for(LONG i=nIndex; TRUE; ++ i)
                {
                    if(!bExceedMax && i == g_dwMaxLogCount)
                    {
                        i = 0;
                        bExceedMax = TRUE;
                    }
                    if(bExceedMax && i == nIndex)
                    {
                        break;
                    }

                    if(::InterlockedExchangeAdd(&g_LogBufferBitmap[i], 1) == 0)
                    {
                        ::InterlockedExchange(&g_LogBufferIndex, (i  + 1 == g_dwMaxLogCount) ? 0 : i + 1);
                        pBuffer = g_LogBuffer + i * g_dwSingleLogSize;
                        break;
                    }
                }

                LogAssert(pBuffer != NULL && _T("GetPooledBuffer"));
                if(pBuffer == NULL)
                {
                    ::SwitchToThread();
                    pBuffer = malloc(g_dwSingleLogSize);
                }
                return pBuffer;
            }
            void ReleasePooledBuffer(void* buffer)
            {
                LogAssert(((LPBYTE)buffer - g_LogBuffer) % g_dwSingleLogSize == 0);
                int nBufferIndex = ((LPBYTE)buffer - g_LogBuffer) / g_dwSingleLogSize;
                LogAssert(nBufferIndex >= 0 && nBufferIndex < g_dwMaxLogCount);
                if(nBufferIndex >= 0 && nBufferIndex < g_dwMaxLogCount)
                    ::InterlockedExchange(&g_LogBufferBitmap[nBufferIndex], 0);
                else
                    free(buffer);
            }

        private:
        } g_PooledBuffer;

        __inline void AddData(stLogInfo& log, LPCVOID data, DWORD dwLen)
        {
            memcpy(log.pBuffer + log.dwPos, data, dwLen);
            log.dwPos += dwLen;
            log.dwRealSize += dwLen;
        }
        __inline void AddData(stLogInfo& log, LPCTSTR data, DWORD dwLen)
        {
            memcpy(log.pBuffer + log.dwPos, data, dwLen);
            *((LPTSTR)((LPBYTE)log.pBuffer + log.dwPos) - 1) = 0;
            log.dwPos += dwLen;
            log.dwRealSize += dwLen;
        }
        __inline void GetData(stLogInfo& log, LPVOID buffer, DWORD dwLen)
        {
            memcpy(buffer, log.pBuffer + log.dwPos, dwLen);
            log.dwPos += dwLen;
            log.dwRealSize += dwLen;
        }
        __inline void GetData(stLogInfo& log, LPCTSTR& szData)
        {
            DWORD dwCount = 0;
            memcpy(&dwCount, log.pBuffer + log.dwPos, sizeof(dwCount));
            log.dwPos += sizeof(dwCount);
            log.dwRealSize += sizeof(dwCount);

            szData = (LPCTSTR)(log.pBuffer + log.dwPos);
            log.dwPos += dwCount;
            log.dwRealSize += dwCount;
        }
    }

    namespace LogInfo
    {
        void InitLog(stLogInfo& log)
        {
            log.dwPos = 0;
            log.dwRealSize = 0;
            log.pBuffer = NULL;
        }

        void ReleaseLog(stLogInfo& log)
        {
            if(log.pBuffer != NULL)
            {
                g_PooledBuffer.ReleasePooledBuffer(log.pBuffer);
            }
        }

        LPTSTR GetLogBuffer(stLogInfo& log, LPCTSTR szLevel, LPCTSTR szFilter)
        {
            LogViewInternal::LogInfo::InitLog(log);

            size_t nLevelLen = _tcslen(szLevel);
            nLevelLen = (nLevelLen < MAX_LEVEL_LENGTH) ? nLevelLen : MAX_LEVEL_LENGTH;
            nLevelLen = (1 + nLevelLen) * sizeof(TCHAR);

            size_t nFilterLen = _tcslen(szFilter);
            nFilterLen = (nFilterLen < MAX_FILTER_LENGTH) ? nFilterLen : MAX_FILTER_LENGTH;
            nFilterLen = (1 + nFilterLen) * sizeof(TCHAR);

            DWORD dwThreadId = ::GetCurrentThreadId();
            DWORD dwProcId = ::GetCurrentProcessId();
            SYSTEMTIME t = {0};
            ::GetLocalTime(&t);

            size_t nBufferLen =
                sizeof(dwThreadId)       // dwThreadId
                + sizeof(dwProcId)     // dwProcId
                + sizeof(t)    // time
                + sizeof(DWORD) + nLevelLen     // sizeof level, including null
                + sizeof(DWORD) + nFilterLen    // sizeof filter, including null
                + sizeof(DWORD) + (1 + MAX_LOG_LENGTH) * sizeof(TCHAR);      // sizeof log, including null

            log.pBuffer = (LPBYTE)g_PooledBuffer.GetPooledBuffer();
            if(log.pBuffer == NULL)
                return NULL;

            AddData(log, &dwThreadId, sizeof(dwThreadId));
            AddData(log, &dwProcId, sizeof(dwProcId));
            AddData(log, &t, sizeof(t));
            AddData(log, &nLevelLen, sizeof(nLevelLen));
            AddData(log, szLevel, nLevelLen);
            AddData(log, &nFilterLen, sizeof(nFilterLen));
            AddData(log, szFilter, nFilterLen);

            return (LPTSTR)(log.pBuffer + log.dwPos + sizeof(size_t));
        }

        void SetLog(stLogInfo& log, LPCTSTR szLog)
        {
            size_t nLogLen = _tcslen(szLog);
            nLogLen = (nLogLen < MAX_LOG_LENGTH) ? nLogLen : MAX_LOG_LENGTH;
            nLogLen = (1 + nLogLen) * sizeof(TCHAR);

            *((LPTSTR)((LPBYTE)log.pBuffer + log.dwPos) - 1) = 0;

            *(size_t*)((LPBYTE)log.pBuffer + log.dwPos) = nLogLen;
            log.dwPos += sizeof(size_t) + nLogLen;
            log.dwRealSize = log.dwPos;
        }

        LPCTSTR GetLogLevel(const stLogInfo& log)
        {
            const DWORD dwLevelOffset = sizeof(DWORD) + sizeof(DWORD) + sizeof(SYSTEMTIME) + sizeof(size_t);
            return (LPCTSTR)(log.pBuffer + dwLevelOffset);
        }

        DWORD ParseLog(LPVOID pBuffer, DWORD dwBufferLen, stLogData& data)
        {
            if(dwBufferLen < sizeof(data.dwThreadId) + sizeof(data.dwProcId) + sizeof(data.timeLog))
                return 0;

            stLogInfo info;
            info.dwPos = info.dwRealSize = 0;
            info.pBuffer = (LPBYTE)pBuffer;
            GetData(info, (LPVOID)&data.dwThreadId, sizeof(data.dwThreadId));
            GetData(info, (LPVOID)&data.dwProcId, sizeof(data.dwProcId));
            GetData(info, (LPVOID)&data.timeLog, sizeof(data.timeLog));
            GetData(info, data.szLevel);
            GetData(info, data.szFilter);
            GetData(info, data.szLog);
            return info.dwRealSize;
        }
    };
}