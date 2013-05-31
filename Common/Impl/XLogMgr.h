#pragma once

#include "LogStream.h"

namespace LogViewInternal
{
    struct stLogFileHeader
    {
        DWORD dwHeader;
        DWORD FileVersion;
    };

    namespace XLogMgr
    {
        BOOL OpenXLog(LPCTSTR szFile, NewLogCallback callback, void* pData);
        BOOL SaveXLog(LPCTSTR szFile, QueryLogCallback callback, void* pData);

    };

    class XLogAppender
    {
    public:
        XLogAppender();
        ~XLogAppender();

        BOOL OpenXLog(LPCTSTR szFile, BOOL bAppend);
        BOOL AppendLog(const stLogInfo& log);

    protected:
        void Close();

    protected:
        HANDLE m_hFile;
    };
}
