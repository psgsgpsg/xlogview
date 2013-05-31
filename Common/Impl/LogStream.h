#pragma once

#include "LogDef.h"

namespace LogViewInternal
{
    struct stLogInfo
    {
        DWORD   dwPos;
        LPBYTE  pBuffer;
        DWORD   dwRealSize;
    };

    namespace LogInfo
    {
        void InitLog(stLogInfo& log);
        void ReleaseLog(stLogInfo& log);

        LPTSTR GetLogBuffer(stLogInfo& log, LPCTSTR szLevel, LPCTSTR szFilter);
        void SetLog(stLogInfo& log, LPCTSTR szLog);
        LPCTSTR GetLogLevel(const stLogInfo& log);

        DWORD ParseLog(LPVOID pBuffer, DWORD dwBufferLen, stLogData& data);
    };
}