#pragma once

#include "LogImplDef.h"
#include "FileHelper.h"

namespace LogView
{
    namespace Util
    {
        struct stLogFileHeader
        {
            DWORD dwHeader;
            DWORD FileVersion;
        };

        namespace XLogMgr
        {
            typedef void (*NewLogCallback)(const stLogInfo& info);
            BOOL OpenXLog(LPCTSTR szFile, NewLogCallback callback);
            BOOL SaveXLog(const LogInfoVector& vctLogInfo, LPCTSTR szFile);

        };

        class XLogAppender
        {
        public:
            XLogAppender();
            ~XLogAppender();

            BOOL OpenXLog(LPCTSTR szLog, BOOL bAppend);
            BOOL AppendLog(const stLogInfo& log);

        protected:
            void Close();

        protected:
            CFileHelper m_File;
        };

    }
}