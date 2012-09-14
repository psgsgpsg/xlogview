#pragma once

#include <vector>
#include <string>

namespace LogView
{
    namespace Util
    {
        class XString
        {
        public:
            XString();
            XString(const XString& str);
            XString& operator = (const XString& str);
            ~XString();

        public:
            XString(const char* str);
            XString(const wchar_t* str);
            XString(const char* str, size_t nLength);
            XString(const wchar_t* str, size_t nLength);

            static const size_t npos = (size_t)-1;

        public:
            XString& assign(LPCTSTR str, DWORD dwLen);

            size_t  Length() const;
            int     CompareNoncase(LPCTSTR str);

            int     FindOneOf(LPCTSTR pszCharSet) const;

            XString Left(int nCount) const;
            XString Right(int nCount) const;

            XString& TrimLeft();
            XString& TrimRight();

            size_t  replace(LPCTSTR szSrc, LPCTSTR szDes);
            void    resize(size_t size);

            size_t  find(LPCTSTR str) const;
            size_t  find(TCHAR ch) const;
            size_t  find(LPCTSTR str, size_t pos) const;
            size_t  find(TCHAR ch, size_t pos) const;

            size_t  rfind(LPCTSTR str) const;
            size_t  rfind(TCHAR ch) const;
            size_t  rfind(LPCTSTR str, size_t pos) const;
            size_t  rfind(TCHAR ch, size_t pos) const;

            bool    empty() const;
            size_t  size() const;

            XString substr(size_t pos) const;
            XString substr(size_t pos, size_t count) const;

        public:
            XString arg(short n);
            XString arg(unsigned short n);
            XString arg(int n);
            XString arg(unsigned int n);
            XString arg(long n);
            XString arg(unsigned long n);
            XString arg(LONGLONG n);
            XString arg(ULONGLONG n);
            XString arg(char n);
            XString arg(double d);
            XString arg(const XString& str);

            // 该方法不推荐使用，仅在需要使用...的方法时才可以使用
            XString& __cdecl Format(LPCTSTR szFormat, va_list* pArgs);
            XString& __cdecl Format(LPCTSTR szFormat, ...);

        public:
            static XString from(short n);
            static XString from(unsigned short n);
            static XString from(int n);
            static XString from(unsigned int n);
            static XString from(long n);
            static XString from(unsigned long n);
            static XString from(LONGLONG n);
            static XString from(ULONGLONG n);
            static XString from(char n);
            static XString from(double d);

        public:
            bool operator == (const XString& str) const;
            bool operator != (const XString& str) const;
            bool operator < (const XString& str) const;
            bool operator > (const XString& str) const;

            bool operator == (const TCHAR str[]) const;
            bool operator != (const TCHAR str[]) const;
            bool operator < (const TCHAR str[]) const;
            bool operator > (const TCHAR str[]) const;

            TCHAR& operator [] (size_t nIndex);
            TCHAR& at(size_t nIndex);

        public:
            XString& operator += (const XString& str);
            XString& operator += (const wchar_t str[]);
            XString& operator += (const wchar_t ch);

            XString  operator + (const XString& str);
            XString  operator + (const wchar_t str[]);
            XString  operator + (const wchar_t ch);

            XString& operator = (const wchar_t str[]);

        public:
            operator const wchar_t* () const;
            std::string GetStdString ();
            std::wstring GetStdWString();

            const wchar_t* GetData() const;
            const wchar_t* c_str() const;

        public:
            // STL typedef
            typedef std::wstring::iterator          iterator;
            typedef std::wstring::const_iterator    const_iterator;

            iterator begin();
            const_iterator begin() const;
            iterator end();
            const_iterator end() const;

            // STL compatibility
            void push_back(wchar_t ch);
            void push_back(const XString &str);
            void push_front(wchar_t ch);
            void push_front(const XString &str);
            XString& erase(size_t _Off, size_t _Count = npos);

        public:
            XString& insert(iterator ite, char ch);
            XString& insert(iterator ite, wchar_t ch);
            XString& insert(iterator ite, const XString& str);

            XString& append(char ch);
            XString& append(wchar_t ch);
            XString& append(const XString& str);

        public:
            XString  ToLower();
            XString  ToUpper();

        protected:
            XString& InitString(const XString& str);
            XString& InitString(const TCHAR str[]);
            void     ReleaseString();

            DWORD    GetHashValue() const;
            XString  ArgImpl(const XString& str);

        private:
            std::wstring    m_wstr;
            mutable DWORD   m_dwHash;
            size_t          m_nArgPos;
        };

        bool operator == (const wchar_t* str, const XString& xstr);

        typedef std::vector<XString> XStringVector;

        // bool operator < (const XString& str1, const XString& str2);
    }
}