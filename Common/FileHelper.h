#pragma once


#include "MemBuffer.h"

namespace LogView
{
    namespace Util
    {
        class CFileHelper
        {
        public:
            CFileHelper();
            ~CFileHelper();

            BOOL Open(LPCTSTR szFilePath, DWORD dwAccess, DWORD dwFlags);
            BOOL OpenForRead(LPCTSTR szFilePath);
            BOOL OpenForWrite(LPCTSTR szFilePath);
            void Close();

            BOOL Read(LONG& l);
            BOOL Read(int& n);
            BOOL Read(size_t& n);
            BOOL Read(std::string& s);
            BOOL Read(std::wstring& s);
            BOOL Read(XString& s);
            BOOL Read(DWORD& d);
            BOOL Read(unsigned short& us);

            BOOL Write(char* s);
            BOOL Write(wchar_t* s);
            BOOL Write(const char* s);
            BOOL Write(const wchar_t* s);
            BOOL Write(LONG l);
            BOOL Write(int n);
            BOOL Write(size_t n);
            BOOL Write(const std::string& s);
            BOOL Write(const XString& s);
            BOOL Write(const std::wstring& s);
            BOOL Write(DWORD d);
            BOOL Write(unsigned short us);

            template<typename T>
            BOOL ReadImpl(T& data)
            {
                return ReadFileImpl(m_hFile, &data, sizeof(data));
            }

            template<typename T>
            BOOL WriteImpl(const T data)
            {
                return WriteFileImpl(m_hFile, (LPVOID)&data, sizeof(data));
            }

            BOOL ReadFileImpl(HANDLE hFile, LPVOID pData, DWORD dwSize);
            BOOL WriteFileImpl(HANDLE hFile, LPVOID pData, DWORD dwSize);

            BOOL ReadString();
            BOOL WriteString();

        protected:
            HANDLE      m_hFile;
            XString     m_strFileName;
            CMemBuffer  m_Buffer;
        };

    }
}