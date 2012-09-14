#pragma once

#include <vector>
#include "XString.h"

namespace LogView
{
    namespace Util
    {
        typedef std::vector<XString>    XStrVector;
        namespace XStrUtil
        {
            // 
            XString       ToLower(const XString& str);
            XString       ToUpper(const XString& str);

            // Conversion
            std::wstring  MultiByteToUnicode(const char* pString, UINT uCodePage);
            std::string   UnicodeToMultiByte(const wchar_t* pString, UINT uCodePage);

            std::wstring  AnsiToUnicode(const char* pString);
            std::wstring  AnsiToUnicode(const std::string pString);
            std::string   UnicodeToAnsi(const wchar_t* pString);
            std::string   UnicodeToAnsi(const std::wstring pString);

            std::string   StrToAnsi(LPCTSTR pString);
            std::string   StrToAnsi(XString pString);
            XString       AnsiToStr(const char* pString);
            XString       AnsiToStr(const std::string pString);

            std::string   UnicodeToUtf8(const wchar_t* pString);
            std::string   UnicodeToUtf8(const std::wstring pString);
            std::wstring  Utf8ToUnicode(const char* pString);
            std::wstring  Utf8ToUnicode(const std::string pString);

            std::string   StrToUtf8(LPCTSTR pString);
            XString       Utf8ToStr(const char* pString);

            size_t        SplitString(const XString& strText, LPCTSTR szToken, XStrVector& vctResult, BOOL bCaseSensitive = FALSE, BOOL bIncludeEmpty = FALSE);
            size_t        SplitString(const XString& strString, const XStrVector& vctTokens, XStrVector& vctResult, BOOL bCaseSensitive = FALSE, BOOL bIncludeEmpty = FALSE);

            LONG          StringToLong(LPCTSTR str);
            BOOL          StringToBool(LPCTSTR str);

            BOOL          StringToRect(LPCTSTR str, RECT* prcValue);
            BOOL          StringToSize(LPCTSTR str, SIZE* pSizeValue);

            BOOL          StringToColor(LPCTSTR str, COLORREF* pClrValue);
        }

    }
}