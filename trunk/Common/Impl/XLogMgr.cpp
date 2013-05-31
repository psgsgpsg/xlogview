#include "stdafx.h"
#include "XLogMgr.h"


#define LOG_FILE_HEADER     'XLFH'
#define LOG_FILE_VERSION    2


//////////////////////////////////////////////////////////////////////////
namespace LogViewInternal
{

    namespace
    {
        template < class T >
        BOOL ReadHelper(HANDLE hFile, T& value)
        {
            DWORD dwRead = 0;
            return (::ReadFile(hFile, (LPVOID)(&value), sizeof(value), &dwRead, NULL) && dwRead == sizeof(value));
        }
        template < class T >
        BOOL WriteHelper(HANDLE hFile, const T& value)
        {
            DWORD dwWritten = 0;
            return (::WriteFile(hFile, (LPCVOID)&value, sizeof(value), &dwWritten, NULL) && dwWritten == sizeof(value));
        }
        BOOL WriteString(HANDLE hFile, const TCHAR* value)
        {
            size_t nLen = (1 + _tcslen(value)) * sizeof(TCHAR);
            if(WriteHelper(hFile, nLen))
            {
                DWORD dwWritten = 0;
                return (::WriteFile(hFile, (LPCVOID)value, nLen, &dwWritten, NULL) && dwWritten == nLen);
            }
            return FALSE;
        }
        BOOL WriteLogHeader(HANDLE hFile)
        {
            DWORD dwWritten = 0;
            stLogFileHeader header =
            {
                LOG_FILE_HEADER,
                LOG_FILE_VERSION,
            };

            return WriteHelper(hFile, header);
        }

        BOOL WriteLog(HANDLE hFile, const stLogData& info)
        {
            return (
                WriteHelper(hFile, info.dwThreadId)
                && WriteHelper(hFile, info.dwProcId)
                && WriteHelper(hFile, info.timeLog)
                && WriteString(hFile, info.szLevel)
                && WriteString(hFile, info.szFilter)
                && WriteString(hFile, info.szLog)
                );
        }
        BOOL WriteLog(HANDLE hFile, const stLogInfo& info)
        {
            DWORD dwWritten = 0;
            return (::WriteFile(hFile, (LPCVOID)info.pBuffer, info.dwRealSize, &dwWritten, NULL) && dwWritten == info.dwRealSize);
        }
    }

    namespace XLogMgr
    {
        BOOL OpenXLog(LPCTSTR szFile, NewLogCallback callback, void* pParam)
        {
            HANDLE hFile = ::CreateFile(szFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
            if(hFile == INVALID_HANDLE_VALUE)
                return FALSE;

            DWORD dwSizeHigh = 0, dwSizeLow = 0;

            HANDLE hMap = NULL;
            LPBYTE pData = NULL;

            BOOL bResult = FALSE;
            DWORD dwRead = 0;
            stLogData info;
            stLogFileHeader header = {0};

            do 
            {
                dwSizeLow = ::GetFileSize(hFile, &dwSizeHigh);
                if(dwSizeLow <= sizeof(header))
                    break;

                hMap = ::CreateFileMapping(hFile, NULL, PAGE_READONLY, dwSizeHigh, dwSizeLow, NULL);
                if(hMap == NULL)
                    break;

                pData = (LPBYTE)::MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, dwSizeLow);
                if(pData == NULL)
                    break;

                // Read File Header
                if(!ReadHelper(hFile, header) || header.dwHeader != LOG_FILE_HEADER || header.FileVersion != LOG_FILE_VERSION)
                    break;

                pData += sizeof(header);
                dwSizeLow -= sizeof(header);
                // Read Log
                DWORD dwLogSize = 0;
                while(dwSizeLow > 0)
                {
                    dwLogSize = LogViewInternal::LogInfo::ParseLog(pData, dwSizeLow, info);
                    if(dwLogSize <= 0 || dwLogSize > dwSizeLow)
                        break;

                    callback(info, pParam);
                    pData += dwLogSize;
                    dwSizeLow -= dwLogSize;
                }

                bResult = TRUE;
            } while (FALSE);


            if(pData != NULL)
                ::UnmapViewOfFile(pData);
            if(hMap != NULL)
                ::CloseHandle(hMap);
            if(hFile != INVALID_HANDLE_VALUE)
                ::CloseHandle(hFile);

            return bResult;
        }

        BOOL SaveXLog(LPCTSTR szFile, QueryLogCallback callback, void* pData)
        {
            HANDLE hFile = ::CreateFile(szFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);

            if(hFile == INVALID_HANDLE_VALUE)
                return FALSE;

            DWORD dwWritten = 0;
            if(!WriteLogHeader(hFile))
                return FALSE;

            BOOL bResult = FALSE;

            size_t nIndex = 0;
            stLogData data;
            while(callback(nIndex, data, pData))
            {
                bResult = WriteLog(hFile, data);
                if(!bResult)
                    break;
                ++ nIndex;
            }

            ::CloseHandle(hFile);
            if(!bResult)
                ::DeleteFile(szFile);

            return bResult;
        }

    }

    //////////////////////////////////////////////////////////////////////////

    XLogAppender::XLogAppender()
    {
        m_hFile = INVALID_HANDLE_VALUE;
    }

    XLogAppender::~XLogAppender()
    {
        Close();
    }

    BOOL XLogAppender::OpenXLog(LPCTSTR szFile, BOOL bAppend)
    {
        Close();
        m_hFile = ::CreateFile(szFile, GENERIC_READ | GENERIC_WRITE, 0, NULL, bAppend ? OPEN_ALWAYS: CREATE_ALWAYS, 0, NULL);
        if(m_hFile == INVALID_HANDLE_VALUE)
            return FALSE;

        stLogFileHeader header = {0};

        // Check File Header
        if(!ReadHelper(m_hFile, header) || header.dwHeader != LOG_FILE_HEADER || header.FileVersion != LOG_FILE_VERSION)
        {
            if(!WriteLogHeader(m_hFile))
                return FALSE;
        }

        if(bAppend)
        {
            DWORD dwSize = ::GetFileSize(m_hFile, NULL);
            ::SetFilePointer(m_hFile, dwSize, NULL, FILE_BEGIN);
        }

        return TRUE;
    }

    BOOL XLogAppender::AppendLog(const stLogInfo& log)
    {
        return WriteLog(m_hFile, log);
    }

    void XLogAppender::Close()
    {
        if(m_hFile != INVALID_HANDLE_VALUE)
        {
            ::CloseHandle(m_hFile);
            m_hFile = INVALID_HANDLE_VALUE;
        }
    }
}

