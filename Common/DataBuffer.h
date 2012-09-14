#pragma once


// 本类用于序列化，即将不同的数据类型写入字符串流中
//  或者从流中读取数据

#include "XString.h"

namespace LogView
{
    namespace Util
    {
        class CDataBuffer
        {
        public:
            CDataBuffer();
            ~CDataBuffer();

            XString     ToString();
            LPCTSTR     ToString(DWORD& dwSize);
            void        Reset(LPCTSTR szData = NULL, DWORD dwDataLen = 0);
            BOOL        Readable();

            // Output Data to Stream
            CDataBuffer& operator << (wchar_t* s);
            CDataBuffer& operator << (const wchar_t* s);
            CDataBuffer& operator << (LONG l);
            CDataBuffer& operator << (int n);
            CDataBuffer& operator << (size_t n);
            CDataBuffer& operator << (const std::wstring& s);
            CDataBuffer& operator << (DWORD d);
            CDataBuffer& operator << (unsigned short us);

            // Input Data from Stream
            CDataBuffer& operator >> (LONG& l);
            CDataBuffer& operator >> (int& n);
            CDataBuffer& operator >> (size_t& n);
            CDataBuffer& operator >> (std::wstring& s);
            CDataBuffer& operator >> (XString& s);
            CDataBuffer& operator >> (DWORD& d);
            CDataBuffer& operator >> (unsigned short& us);

        protected:
            void        PushData(const wchar_t* szData);
            XString     PopData();

        protected:
            XString     m_strData;

            size_t      m_nReadPos;
        };
    }
}