#include "StdAfx.h"
#include "XString.h"
#include <XStrUtil.h>

namespace LogView
{
    namespace Util
    {
        __inline DWORD  HashString(LPCTSTR str)
        {
            // BKDRHash
            DWORD seed = 131; // 31 131 1313 13131 131313 etc..
            DWORD hash = 0;

            while (*str)
            {
                hash = hash * seed + (*str++);
            }

            return (hash & 0x7FFFFFFF);
        }

#pragma warning(disable: 4995)
#pragma warning(disable: 4996)

#define STRING_CHANGED()    \
        {                           \
        m_dwHash = 0;           \
        }

        XString::XString()
        {
            m_nArgPos = 0;
            m_dwHash = 0;
        }

        XString::XString(const XString& str)
        {
            InitString(str);
        }

        XString& XString::operator = (const XString& str)
        {
            InitString(str);
            return (*this);
        }

        XString::~XString()
        {
            ReleaseString();
        }


        //////////////////////////////////////////////////////////////////////////
        XString::XString(const char* str)
        {
            std::wstring wstr = XStrUtil::AnsiToUnicode(str);
            InitString(wstr.c_str());
        }

        XString::XString(const wchar_t* str)
        {
            InitString(str);
        }

        XString::XString(const char* str, size_t nLength)
        {
            std::string strTemp(str, nLength);
            std::wstring wstr = XStrUtil::AnsiToUnicode(strTemp);
            InitString(wstr.c_str());
        }

        XString::XString(const wchar_t* str, size_t nLength)
        {
            std::wstring wstr(str, nLength);
            InitString(wstr.c_str());
        }

        //////////////////////////////////////////////////////////////////////////

        XString& XString::assign(LPCTSTR str, DWORD dwLen)
        {
            STRING_CHANGED();
            m_wstr.assign(str, dwLen);
            return (*this);
        }

        size_t XString::Length() const
        {
            return m_wstr.length();
        }

        int XString::CompareNoncase(LPCTSTR str)
        {
            return _tcsicmp(m_wstr.c_str(), str);
        }

        int XString::FindOneOf(LPCTSTR pszCharSet) const
        {
            LPCTSTR p = _tcspbrk(m_wstr.c_str(), pszCharSet);
            return (p == NULL) ? -1 : (p - m_wstr.c_str());
        }

        XString XString::Left(int nCount) const
        {
            return substr(0, nCount);
        }

        XString XString::Right(int nCount) const
        {
            if (nCount < 0)
                nCount = 0;

            int nLength = Length();
            if(nCount >= nLength)
            {
                return(*this);
            }

            return substr(nLength-nCount, nCount);
        }

        XString& XString::TrimLeft()
        {
            LPCTSTR p = m_wstr.c_str();

            while(_istspace(*p) && (*p))
            {
                ++ p;
            }

            if(p != m_wstr.c_str())
            {
                InitString(p);
            }

            return (*this);
        }

        XString& XString::TrimRight()
        {
            LPCTSTR start = m_wstr.c_str();
            LPCTSTR end = start + m_wstr.length() - 1;

            while(end >= start && _istspace(*end))
            {
                -- end;
            }

            if(end < start)
                resize(0);
            else
                resize(end - start + 1);

            return (*this);
        }

        size_t XString::replace(LPCTSTR szSrc, LPCTSTR szDes)
        {
            if(szSrc == NULL || szSrc[0] == 0 || szDes == NULL)
                return 0;

            size_t nSrcLength = _tcslen(szSrc);
            size_t nDesLength = _tcslen(szDes);

            std::wstring::size_type pos = 0;
            std::wstring::size_type count = 0;
            while( (pos = m_wstr.find(szSrc, pos)) != std::wstring::npos)
            {
                m_wstr.replace(pos, nSrcLength, szDes);
                if(nDesLength > nSrcLength)
                    pos += (nDesLength - nSrcLength) + 1;
                else if(nDesLength < nSrcLength)
                    pos += nDesLength;
                ++ count;
            }
            STRING_CHANGED();
            return count;
        }

        void XString::resize(size_t size)
        {
            m_wstr.resize(size);
            STRING_CHANGED();
        }

        size_t XString::find(LPCTSTR str) const
        {
            if(str == NULL || str[0] == 0)
                return XString::npos;
            return m_wstr.find(str);
        }

        size_t XString::find(TCHAR ch) const
        {
            return m_wstr.find(ch);
        }

        size_t XString::find(LPCTSTR str, size_t pos) const
        {
            if(str == NULL || str[0] == 0)
                return XString::npos;
            return m_wstr.find(str, pos);
        }

        size_t XString::find(TCHAR ch, size_t pos) const
        {
            return m_wstr.find(ch, pos);
        }

        size_t XString::rfind(LPCTSTR str) const
        {
            if(str == NULL || str[0] == 0)
                return XString::npos;
            return m_wstr.rfind(str);
        }

        size_t XString::rfind(TCHAR ch) const
        {
            return m_wstr.rfind(ch);
        }

        size_t XString::rfind(LPCTSTR str, size_t pos) const
        {
            if(str == NULL || str[0] == 0)
                return XString::npos;
            return m_wstr.rfind(str, pos);
        }

        size_t XString::rfind(TCHAR ch, size_t pos) const
        {
            return m_wstr.rfind(ch, pos);
        }

        bool XString::empty() const
        {
            return m_wstr.empty();
        }

        size_t XString::size() const
        {
            return m_wstr.size();
        }

        XString XString::substr(size_t pos) const
        {
            XString strResult = m_wstr.substr(pos).c_str();
            return strResult;
        }

        XString XString::substr(size_t pos, size_t count) const
        {
            if((int)count < 0)
                count = 0;
            XString strResult = m_wstr.substr(pos, count).c_str();
            return strResult;
        }

        //////////////////////////////////////////////////////////////////////////
        XString XString::arg(short n)
        {
            XString str = from(n);
            return ArgImpl(str);
        }

        XString XString::arg(unsigned short n)
        {
            XString str = from(n);
            return ArgImpl(str);
        }

        XString XString::arg(int n)
        {
            XString str = from(n);
            return ArgImpl(str);
        }

        XString XString::arg(unsigned int n)
        {
            XString str = from(n);
            return ArgImpl(str);
        }

        XString XString::arg(long n)
        {
            XString str = from(n);
            return ArgImpl(str);
        }

        XString XString::arg(unsigned long n)
        {
            XString str = from(n);
            return ArgImpl(str);
        }

        XString XString::arg(LONGLONG n)
        {
            XString str = from(n);
            return ArgImpl(str);
        }

        XString XString::arg(ULONGLONG n)
        {
            XString str = from(n);
            return ArgImpl(str);
        }

        XString XString::arg(char n)
        {
            XString str = from(n);
            return ArgImpl(str);
        }

        XString XString::arg(double d)
        {
            XString str = from(d);
            return ArgImpl(str);
        }

        XString XString::arg(const XString& str)
        {
            return ArgImpl(str);
        }

        // 该方法不推荐使用，仅在需要使用...的方法时才可以使用
        XString& XString::Format(LPCTSTR szFormat, va_list* pArgs)
        {
            const int nDefaultBufferSize = 256;
            TCHAR szBuffer[nDefaultBufferSize] = {0};
            int nCount = _vsctprintf(szFormat, *pArgs);
            if(nCount + 1 < nDefaultBufferSize)
            {
                nCount = _vsntprintf(szBuffer, nDefaultBufferSize, szFormat, *pArgs);
                if(nCount > 0)
                    InitString(szBuffer);
            }
            else
            {
                TCHAR* pBuffer = new TCHAR[nCount + 1];
                nCount = _vsntprintf(pBuffer, nCount, szFormat, *pArgs);
                if(nCount > 0)
                {
                    pBuffer[nCount] = 0;
                    InitString(pBuffer);
                }
                delete[] pBuffer;
            }
            return (*this);
        }

        XString& XString::Format(LPCTSTR szFormat, ...)
        {
            va_list args;
            va_start(args, szFormat);
            return Format(szFormat, &args);
        }

        //////////////////////////////////////////////////////////////////////////
        XString XString::from(short n)
        {
            TCHAR szBuffer[10];
            _sntprintf(szBuffer, 10, _T("%hd"), n);
            return XString(szBuffer);
        }

        XString XString::from(unsigned short n)
        {
            TCHAR szBuffer[10];
            _sntprintf(szBuffer, 10, _T("%hu"), n);
            return XString(szBuffer);
        }

        XString XString::from(int n)
        {
            TCHAR szBuffer[15];
            _sntprintf(szBuffer, 15, _T("%d"), n);
            return XString(szBuffer);
        }

        XString XString::from(unsigned int n)
        {
            TCHAR szBuffer[15];
            _sntprintf(szBuffer, 15, _T("%u"), n);
            return XString(szBuffer);
        }

        XString XString::from(long n)
        {
            TCHAR szBuffer[15];
            _sntprintf(szBuffer, 15, _T("%ld"), n);
            return XString(szBuffer);
        }

        XString XString::from(unsigned long n)
        {
            TCHAR szBuffer[15];
            _sntprintf(szBuffer, 15, _T("%u"), n);
            return XString(szBuffer);
        }

        XString XString::from(LONGLONG n)
        {
            TCHAR szBuffer[40];
            _sntprintf(szBuffer, 40, _T("%I64d"), n);
            return XString(szBuffer);
        }

        XString XString::from(ULONGLONG n)
        {
            TCHAR szBuffer[40];
            _sntprintf(szBuffer, 40, _T("%I64u"), n);
            return XString(szBuffer);
        }

        XString XString::from(char n)
        {
            TCHAR szBuffer[5];
            _sntprintf(szBuffer, 5, _T("%c"), n);
            return XString(szBuffer);
        }

        XString XString::from(double d)
        {
            TCHAR szBuffer[50];
            _sntprintf(szBuffer, 50, _T("%lf"), d);
            return XString(szBuffer);
        }

        //////////////////////////////////////////////////////////////////////////
        bool XString::operator == (const XString& str) const
        {
            return (GetHashValue() == str.GetHashValue()) && (m_wstr == str.m_wstr);
        }

        bool XString::operator != (const XString& str) const
        {
            return !((*this) == str);
        }

        bool XString::operator < (const XString& str) const
        {
            return this->m_wstr < str.m_wstr;
        }

        bool XString::operator > (const XString& str) const
        {
            return this->m_wstr > str.m_wstr;
        }

        bool XString::operator == (const TCHAR str[]) const
        {
            return m_wstr == str;
        }

        bool XString::operator != (const TCHAR str[]) const
        {
            return m_wstr != str;
        }

        bool XString::operator < (const TCHAR str[]) const
        {
            return m_wstr < str;
        }

        bool XString::operator > (const TCHAR str[]) const
        {
            return m_wstr > str;
        }

        TCHAR& XString::operator [] (size_t nIndex)
        {
            STRING_CHANGED();
            return m_wstr[nIndex];
        }

        TCHAR& XString::at(size_t nIndex)
        {
            STRING_CHANGED();
            return m_wstr[nIndex];
        }

        //////////////////////////////////////////////////////////////////////////
        XString& XString::operator += (const XString& str)
        {
            m_wstr += str.m_wstr;
            STRING_CHANGED();
            return (*this);
        }

        XString& XString::operator += (const wchar_t str[])
        {
            m_wstr += str;
            STRING_CHANGED();
            return (*this);
        }

        XString& XString::operator += (const wchar_t ch)
        {
            m_wstr += ch;
            STRING_CHANGED();
            return (*this);
        }

        XString XString::operator + (const XString& str)
        {
            XString result((m_wstr + str.GetData()).c_str());
            return result;
        }

        XString XString::operator + (const wchar_t str[])
        {
            XString result((m_wstr + str).c_str());
            return result;
        }

        XString XString::operator + (const wchar_t ch)
        {
            XString result((m_wstr + ch).c_str());
            return result;
        }

        XString& XString::operator = (const wchar_t str[])
        {
            m_wstr = str;
            STRING_CHANGED();
            return (*this);
        }

        //////////////////////////////////////////////////////////////////////////
        XString::operator const wchar_t* () const
        {
            return m_wstr.c_str();
        }

        std::string XString::GetStdString()
        {
            return XStrUtil::UnicodeToAnsi(m_wstr.c_str());
        }

        std::wstring XString::GetStdWString()
        {
            return m_wstr;
        }

        const wchar_t* XString::GetData() const
        {
            return m_wstr.c_str();
        }

        const wchar_t* XString::c_str() const
        {
            return m_wstr.c_str();
        }

        //////////////////////////////////////////////////////////////////////////

        XString::iterator XString::begin()
        {
            STRING_CHANGED();
            return m_wstr.begin();
        }

        XString::const_iterator XString::begin() const
        {
            return m_wstr.begin();
        }

        XString::iterator XString::end()
        {
            STRING_CHANGED();
            return m_wstr.end();
        }

        XString::const_iterator XString::end() const
        {
            return m_wstr.end();
        }

        // STL compatibility
        void XString::push_back(wchar_t ch)
        {
            append(ch);
        }

        void XString::push_back(const XString &str)
        {
            append(str);
        }

        void XString::push_front(wchar_t ch)
        {
            insert(m_wstr.begin(), ch);
        }

        void XString::push_front(const XString &str)
        {
            insert(m_wstr.begin(), str);
        }

        XString& XString::erase(size_t _Off, size_t _Count /* = npos */)
        {
            m_wstr.erase(_Off, _Count);
            return (*this);
        }

        //////////////////////////////////////////////////////////////////////////

        XString& XString::insert(iterator ite, char ch)
        {
            XString str(&ch, 1);
            m_wstr.insert(ite, str.m_wstr.begin(), str.m_wstr.end());
            STRING_CHANGED();
            return (*this);
        }

        XString& XString::insert(iterator ite, wchar_t ch)
        {
            m_wstr.insert(ite, ch);
            STRING_CHANGED();
            return (*this);
        }

        XString& XString::insert(iterator ite, const XString& str)
        {
            m_wstr.insert(ite, str.m_wstr.begin(), str.m_wstr.end());
            STRING_CHANGED();
            return (*this);
        }

        XString& XString::append(char ch)
        {
            STRING_CHANGED();
            return insert(m_wstr.end(), ch);
        }

        XString& XString::append(wchar_t ch)
        {
            STRING_CHANGED();
            return insert(m_wstr.end(), ch);
        }

        XString& XString::append(const XString& str)
        {
            STRING_CHANGED();
            return insert(m_wstr.end(), str);
        }

        XString XString::ToLower()
        {
            return XStrUtil::ToLower(*this);
        }

        XString XString::ToUpper()
        {
            return XStrUtil::ToUpper(*this);
        }

        //////////////////////////////////////////////////////////////////////////
        XString& XString::InitString(const XString& str)
        {
            if((&str) != this)
            {
                m_wstr = str.m_wstr;
                m_nArgPos = str.m_nArgPos;
                m_dwHash = str.m_dwHash;
            }
            return (*this);
        }

        XString& XString::InitString(const TCHAR str[])
        {
            m_wstr = str;
            m_nArgPos = 0;
            m_dwHash = 0;
            return (*this);
        }

        void XString::ReleaseString()
        {
            ;
        }

        XString XString::ArgImpl(const XString& str)
        {
            XString strTemp, strResult;
            strTemp.Format(_T("%%%d"), ++ m_nArgPos);

            std::wstring::size_type pos = m_wstr.find(strTemp.m_wstr);
            if(pos == std::wstring::npos)
                return (*this);

            strResult.m_wstr = m_wstr.substr(0, pos);
            strResult.m_wstr += str.m_wstr;
            strResult.m_wstr += m_wstr.substr(pos + strTemp.Length());
            strResult.m_nArgPos = m_nArgPos;
            return strResult;
        }

        DWORD XString::GetHashValue() const
        {
            if(m_dwHash == 0)
            {
                m_dwHash = HashString(m_wstr.c_str());
            }
            return m_dwHash;
        }

        //////////////////////////////////////////////////////////////////////////
        bool operator == (const wchar_t str[], const XString& xstr)
        {
            return XString(str) == xstr;
        }

    }
}