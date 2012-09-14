#include "stdafx.h"
#include "XLogMgr.h"

#include "FileHelper.h"

#pragma warning(disable: 4996)

#define LOG_FILE_HEADER     'XLFH'
#define LOG_FILE_VERSION    1


//////////////////////////////////////////////////////////////////////////
namespace LogView
{
    namespace Util
    {

        namespace
        {
            BOOL WriteLogHeader(CFileHelper& file)
            {
                DWORD dwWritten = 0;
                stLogFileHeader header =
                {
                    LOG_FILE_HEADER,
                    LOG_FILE_VERSION,
                };

                BOOL bResult = FALSE;

                return file.WriteImpl(header);
            }

            BOOL WriteLog(CFileHelper& file, const stLogInfo& info)
            {
                return (
                    !file.Write(info.dwThreadId)             // ThreadId
                    || !file.Write(info.dwProcId)            // ProcId
                    || !file.WriteImpl(info.tTime)           // Time
                    || !file.Write(info.uLevel)              // Level
                    || !file.Write(info.strFilter.c_str())   // Filter
                    || !file.Write(info.strLog.c_str())      // Log
                    );
            }
        }

        namespace XLogMgr
        {
            BOOL OpenXLog(LPCTSTR szFile, NewLogCallback callback)
            {
                CFileHelper _File;

                if(!_File.OpenForRead(szFile))
                    return FALSE;

                BOOL bResult = FALSE;
                DWORD dwRead = 0;
                stLogInfo info;
                stLogFileHeader header = {0};

                do 
                {
                    // Read File Header
                    if(!_File.ReadImpl(header) || header.dwHeader != LOG_FILE_HEADER || header.FileVersion != LOG_FILE_VERSION)
                        break;

                    // Read Log
                    while(TRUE)
                    {
                        if(
                            !_File.Read(info.dwThreadId)     // ThreadId
                            || !_File.Read(info.dwProcId)    // ProcId
                            || !_File.ReadImpl(info.tTime)       // Time
                            || !_File.Read(info.uLevel)      // Level
                            || !_File.Read(info.strFilter)   // Filter
                            || !_File.Read(info.strLog)      // Log
                            )
                        {
                            break;
                        }
                        callback(info);
                    }

                    bResult = TRUE;
                } while (FALSE);

                return bResult;
            }

            BOOL SaveXLog(const LogInfoVector& vctLogInfo, LPCTSTR szFile)
            {
                CFileHelper _File;

                if(!_File.OpenForWrite(szFile))
                    return FALSE;

                DWORD dwWritten = 0;
                if(!WriteLogHeader(_File))
                    return FALSE;

                BOOL bResult = FALSE;

                LogInfoVector::const_iterator IteLog = vctLogInfo.begin();
                for(; IteLog != vctLogInfo.end(); ++ IteLog)
                {
                    const stLogInfo& info = *IteLog;

                    if(!WriteLog(_File, info))
                        break;
                }

                bResult = (IteLog == vctLogInfo.end());

                return bResult;
            }

        }

        //////////////////////////////////////////////////////////////////////////

        XLogAppender::XLogAppender()
        {
        }

        XLogAppender::~XLogAppender()
        {
            m_File.Close();
        }

        BOOL XLogAppender::OpenXLog(LPCTSTR szLog, BOOL bAppend)
        {
            m_File.Close();
            if(!m_File.Open(szLog, GENERIC_READ | GENERIC_WRITE, bAppend ? OPEN_ALWAYS: CREATE_ALWAYS))
                return FALSE;

            stLogFileHeader header = {0};

            // Check File Header
            if(!m_File.ReadImpl(header) || header.dwHeader != LOG_FILE_HEADER || header.FileVersion != LOG_FILE_VERSION)
            {
                if(!WriteLogHeader(m_File))
                    return FALSE;
            }

            return TRUE;
        }

        BOOL XLogAppender::AppendLog(const stLogInfo& log)
        {
            return WriteLog(m_File, log);
        }

        void XLogAppender::Close()
        {
            m_File.Close();
        }

    }
}