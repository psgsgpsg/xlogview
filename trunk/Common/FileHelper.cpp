#include "stdafx.h"
#include "FileHelper.h"

namespace LogView
{
    namespace Util
    {
        CFileHelper::CFileHelper()
        {
            m_hFile = INVALID_HANDLE_VALUE;
        }

        CFileHelper::~CFileHelper()
        {
            Close();
        }

        BOOL CFileHelper::Open(LPCTSTR szFilePath, DWORD dwAccess, DWORD dwFlags)
        {
            m_hFile = CreateFile(szFilePath, dwAccess, 0, NULL, dwFlags, 0, NULL);
            return (m_hFile != INVALID_HANDLE_VALUE);
        }

        BOOL CFileHelper::OpenForRead(LPCTSTR szFilePath)
        {
            return Open(szFilePath, GENERIC_READ, OPEN_EXISTING);
        }

        BOOL CFileHelper::OpenForWrite(LPCTSTR szFilePath)
        {
            return Open(szFilePath, GENERIC_WRITE, CREATE_ALWAYS);
        }

        void CFileHelper::Close()
        {
            if(m_hFile != INVALID_HANDLE_VALUE)
            {
                CloseHandle(m_hFile);
                m_hFile = INVALID_HANDLE_VALUE;
            }
        }

        BOOL CFileHelper::Read(LONG& l)
        {
            return ReadImpl(l);
        }

        BOOL CFileHelper::Read(int& n)
        {
            return ReadImpl(n);
        }

        BOOL CFileHelper::Read(size_t& n)
        {
            return ReadImpl(n);
        }

        BOOL CFileHelper::Read(XString& s)
        {
            DWORD dwLen = 0;
            if(ReadImpl(dwLen))
            {
                wchar_t* pData = (wchar_t*)m_Buffer.GetBuffer(dwLen);
                if(ReadFileImpl(m_hFile, pData, dwLen))
                {
                    s = pData;
                    return TRUE;
                }
            }
            return FALSE;
        }

        BOOL CFileHelper::Read(std::string& s)
        {
            DWORD dwLen = 0;
            if(ReadImpl(dwLen))
            {
                char* pData = (char*)m_Buffer.GetBuffer(dwLen);
                if(ReadFileImpl(m_hFile, pData, dwLen + 1))
                {
                    s = pData;
                    return TRUE;
                }
            }
            return FALSE;
        }

        BOOL CFileHelper::Read(std::wstring& s)
        {
            DWORD dwLen = 0;
            if(ReadImpl(dwLen))
            {
                wchar_t* pData = (wchar_t*)m_Buffer.GetBuffer(dwLen);
                if(ReadFileImpl(m_hFile, pData, dwLen))
                {
                    s = pData;
                    return TRUE;
                }
            }
            return FALSE;
        }

        BOOL CFileHelper::Read(DWORD& d)
        {
            return ReadImpl(d);
        }

        BOOL CFileHelper::Read(unsigned short& us)
        {
            return ReadImpl(us);
        }


        BOOL CFileHelper::Write(char* s)
        {
            int nLen = strlen(s) + 1;
            if(WriteImpl(nLen))
                return WriteFileImpl(m_hFile, (LPVOID)s, nLen);
            return FALSE;
        }

        BOOL CFileHelper::Write(wchar_t* s)
        {
            int nLen = (wcslen(s) + 1) * sizeof(wchar_t);
            if(WriteImpl(nLen))
                return WriteFileImpl(m_hFile, (LPVOID)s, nLen);
            return FALSE;
        }

        BOOL CFileHelper::Write(const char* s)
        {
            int nLen = strlen(s) + 1;
            if(WriteImpl(nLen))
                return WriteFileImpl(m_hFile, (LPVOID)s, nLen);
            return FALSE;
        }

        BOOL CFileHelper::Write(const wchar_t* s)
        {
            int nLen = (wcslen(s) + 1) * sizeof(wchar_t);
            if(WriteImpl(nLen))
                return WriteFileImpl(m_hFile, (LPVOID)s, nLen);
            return FALSE;
        }

        BOOL CFileHelper::Write(LONG l)
        {
            return WriteImpl(l);
        }

        BOOL CFileHelper::Write(int n)
        {
            return WriteImpl(n);
        }

        BOOL CFileHelper::Write(size_t n)
        {
            return WriteImpl(n);
        }

        BOOL CFileHelper::Write(const std::string& s)
        {
            int nLen = s.size() + 1;
            if(WriteImpl(nLen))
                return WriteFileImpl(m_hFile, (LPVOID)s.c_str(), nLen);
            return FALSE;
        }

        BOOL CFileHelper::Write(const std::wstring& s)
        {
            int nLen = (s.size() + 1) * 2;
            if(WriteImpl(nLen))
                return WriteFileImpl(m_hFile, (LPVOID)s.c_str(), nLen);
            return FALSE;
        }

        BOOL CFileHelper::Write(const XString& s)
        {
            int nLen = (s.Length() + 1) * sizeof(wchar_t);
            if(WriteImpl(nLen))
                return WriteFileImpl(m_hFile, (LPVOID)s.GetData(), nLen);
            return FALSE;
        }

        BOOL CFileHelper::Write(DWORD d)
        {
            return WriteImpl(d);
        }

        BOOL CFileHelper::Write(unsigned short us)
        {
            return WriteImpl(us);
        }


        BOOL CFileHelper::ReadFileImpl(HANDLE hFile, LPVOID pData, DWORD dwSize)
        {
            DWORD dwRead = 0;
            if(!ReadFile(hFile, pData, dwSize, &dwRead, NULL) || dwRead != dwSize)
                return FALSE;
            return TRUE;
        }

        BOOL CFileHelper::WriteFileImpl(HANDLE hFile, LPVOID pData, DWORD dwSize)
        {
            DWORD dwWritten = 0;
            if(!WriteFile(hFile, pData, dwSize, &dwWritten, NULL) || dwWritten != dwSize)
                return FALSE;
            return TRUE;
        }

    }
}