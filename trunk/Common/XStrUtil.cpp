#include "stdafx.h"
#include "XStrUtil.h"

#include <algorithm>

#include "XString.h"

#pragma warning(disable: 4996)

namespace LogView
{
    namespace Util
    {
        namespace XStrUtil
        {
            //
            XString ToLower(const XString& str)
            {
                XString strResult;
                strResult.resize(str.Length());
                std::transform(str.begin(), str.end(), strResult.begin(), ::_totlower);
                return strResult;
            }

            XString ToUpper(const XString& str)
            {
                XString strResult;
                strResult.resize(str.Length());
                std::transform(str.begin(), str.end(), strResult.begin(), ::_totupper);
                return strResult;
            }

            // Conversion
            std::wstring  MultiByteToUnicode(const char* pString, UINT uCodePage)
            {
                std::wstring wstrResult;

                int nStringLen = strlen(pString);
                int nBufferSize = ::MultiByteToWideChar(uCodePage, 0, pString, nStringLen, NULL, 0);
                if(nBufferSize > 0)
                {
                    wchar_t* pBuffer = new wchar_t[nBufferSize + 1];
                    nBufferSize = ::MultiByteToWideChar(uCodePage, 0, pString, nStringLen, pBuffer, nBufferSize);
                    if(nBufferSize > 0)
                    {
                        pBuffer[nBufferSize] = 0;
                        wstrResult = pBuffer;
                    }
                    delete[] pBuffer;
                }
                return wstrResult;
            }

            std::string   UnicodeToMultiByte(const wchar_t* pString, UINT uCodePage)
            {
                std::string strResult;

                int nStringLen = wcslen(pString);
                int nBufferSize = ::WideCharToMultiByte(uCodePage, 0, pString, nStringLen, 0, 0, 0, 0);
                if(nBufferSize > 0)
                {
                    char* pBuffer = new char[nBufferSize + 1];
                    nBufferSize = ::WideCharToMultiByte(uCodePage, 0, pString, nStringLen, pBuffer, nBufferSize, 0, 0);
                    if(nBufferSize > 0)
                    {
                        pBuffer[nBufferSize] = 0;
                        strResult = pBuffer;
                    }
                    delete[] pBuffer;
                }

                return strResult;
            }

            std::wstring AnsiToUnicode(const char* pString)
            {
                return MultiByteToUnicode(pString, 936);
            }

            std::wstring AnsiToUnicode(const std::string pString)
            {
                return AnsiToUnicode(pString.c_str());
            }

            std::string  UnicodeToAnsi(const wchar_t* pString)
            {
                return UnicodeToMultiByte(pString, 936);
            }

            std::string  UnicodeToAnsi(const std::wstring pString)
            {
                return UnicodeToAnsi(pString.c_str());
            }

            std::string StrToAnsi(LPCTSTR pString)
            {
#ifdef  _UNICODE
                return UnicodeToAnsi(pString);
#else   //_UNICODE
                returnstd::string(pString);
#endif  //_UNICODE
            }

            std::string StrToAnsi(XString pString)
            {
                return StrToAnsi(pString.GetData());
            }

            XString AnsiToStr(const char* pString)
            {
#ifdef  _UNICODE
                return XString(AnsiToUnicode(pString).c_str());
#else   //_UNICODE
                returnstd::string(pString);
#endif  //_UNICODE
            }

            XString AnsiToStr(const std::string pString)
            {
                return AnsiToStr(pString.c_str());
            }

            std::string  UnicodeToUtf8(const wchar_t* pString)
            {
                return UnicodeToMultiByte(pString, CP_UTF8);
            }

            std::string  UnicodeToUtf8(const std::wstring pString)
            {
                return UnicodeToUtf8(pString.c_str());
            }

            std::wstring  Utf8ToUnicode(const char* pString)
            {
                return MultiByteToUnicode(pString, CP_UTF8);
            }

            std::wstring  Utf8ToUnicode(const std::string pString)
            {
                return Utf8ToUnicode(pString.c_str());
            }

            std::string StrToUtf8(LPCTSTR pString)
            {
#ifdef  _UNICODE
                return UnicodeToUtf8(pString);
#else   //_UNICODE
                std::wstring wstrResult = AnsiToUnicode(pString);
                return UnicodeToUtf8(wstrResult.c_str());
#endif  //_UNICODE
            }

            XString Utf8ToStr(const char* pString)
            {
#ifdef  _UNICODE
                return XString(Utf8ToUnicode(pString).c_str());
#else   //_UNICODE
                std::wstring wstrResult = Utf8ToUnicode(pString);
                return UnicodeToAnsi(wstrResult.c_str());
#endif  //_UNICODE
            }


            size_t SplitString(const XString& strText, LPCTSTR szToken, XStrVector& vctResult, BOOL bCaseSensitive /* = FALSE */, BOOL bIncludeEmpty /* = FALSE */)
            {
                vctResult.clear();

                XString strTarget = strText;
                XString strToken(szToken);
                if(!bCaseSensitive)
                    std::transform(strTarget.begin(), strTarget.end(), strTarget.begin(), ::_totlower);
                if(!bCaseSensitive)
                    std::transform(strToken.begin(), strToken.end(), strToken.begin(), ::_totlower);

                size_t nTokenLenght = strToken.Length();

                size_t pos = strTarget.find(strToken);
                if(pos == XString::npos)
                {
                    if(!strText.empty() || bIncludeEmpty)
                        vctResult.push_back(strText);
                }
                else
                {
                    size_t posLast = 0 - nTokenLenght;
                    size_t pos = strTarget.find(strToken, 0);
                    XString strTemp;
                    do 
                    {
                        strTemp = strText.substr(posLast + nTokenLenght, pos - posLast - nTokenLenght);
                        if(!strTemp.empty() || bIncludeEmpty)
                            vctResult.push_back(strTemp);
                        posLast = pos;
                        pos = strTarget.find(strToken, pos + 1);
                    } while (pos != XString::npos);

                    if(posLast != strText.Length() - nTokenLenght)
                    {
                        strTemp = strText.substr(posLast + nTokenLenght);
                        if(!strTemp.empty() || bIncludeEmpty)
                            vctResult.push_back(strTemp);
                    }
                }

                return vctResult.size();
            }

            size_t SplitString(const XString& strString, const XStrVector& vctTokens, XStrVector& vctResult, BOOL bCaseSensitive, BOOL bIncludeEmpty)
            {
                vctResult.clear();

                if(strString.empty())
                {
                    if(bIncludeEmpty)
                        vctResult.push_back(strString);
                    return vctResult.size();
                }

                if(vctTokens.size() == 0)
                {
                    if(bIncludeEmpty || !strString.empty())
                        vctResult.push_back(strString);
                    return vctResult.size();
                }

                size_t nPos = 0, nLastPos = 0;

                XString strTarget = strString;
                XStrVector vctTargetTokens(vctTokens);

                if(!bCaseSensitive)
                {
                    std::transform(strTarget.begin(), strTarget.end(), strTarget.begin(), ::_totlower);
                    XStrVector::iterator IteToken = vctTargetTokens.begin();
                    for(; IteToken != vctTargetTokens.end(); ++ IteToken)
                    {
                        XString& strToken = *IteToken;
                        std::transform(strToken.begin(), strToken.end(), strToken.begin(), ::_totlower);
                    }
                }

                XString strToken;
                size_t nTokenIndex = 0;
                strToken = vctTokens[nTokenIndex];
                nPos = strTarget.find(strToken);
                if(nPos == XString::npos)
                {
                    if(bIncludeEmpty || !strString.empty())
                        vctResult.push_back(strString);
                    return vctResult.size();
                }

                XString strTemp;
                size_t nTokenCount = vctTokens.size();

                for(; nPos != XString::npos; nPos = strTarget.find(strToken, nPos))
                {
                    strTemp = strString.substr(nLastPos, nPos - nLastPos);
                    if(bIncludeEmpty || !strTemp.empty())
                        vctResult.push_back(strTemp);
                    nPos += strToken.Length();
                    nLastPos = nPos;
                    nTokenIndex = (nTokenIndex + 1) % nTokenCount;
                    strToken = vctTokens[nTokenIndex];
                }
                nTokenIndex = (nTokenIndex == 0) ? (nTokenCount - 1) : (nTokenIndex - 1);
                strToken = vctTokens[nTokenIndex];
                strTemp = strString.substr(nLastPos);
                if(strTemp != strToken)
                {
                    if(bIncludeEmpty || !strTemp.empty())
                        vctResult.push_back(strTemp);
                }

                return vctResult.size();
            }

            LONG StringToLong(LPCTSTR str)
            {
                return _ttol(str);
            }

            BOOL StringToBool(LPCTSTR str)
            {
                return (_ttol(str) != 0)
                    || (_tcsicmp(str, _T("true")) == 0);
            }

            BOOL StringToRect(LPCTSTR str, RECT* prcValue)
            {
                XString strTemp(str);

                strTemp.replace(_T(" "), _T(""));

                size_t pos = 0;

                // Left
                prcValue->left = _ttoi(strTemp.GetData());

                // Top
                pos = strTemp.find(_T(','), pos);
                if(pos == XString::npos)
                    return FALSE;
                ++ pos;
                prcValue->top = _ttoi(strTemp.GetData() + pos);

                // Right
                pos = strTemp.find(_T(','), pos);
                if(pos == XString::npos)
                    return FALSE;
                ++ pos;
                prcValue->right = _ttoi(strTemp.GetData() + pos);

                // Bottom
                pos = strTemp.find(_T(','), pos);
                if(pos == XString::npos)
                    return FALSE;
                ++ pos;
                prcValue->bottom = _ttoi(strTemp.GetData() + pos);

                return TRUE;
            }

            BOOL StringToSize(LPCTSTR str, SIZE* pSizeValue)
            {
                XString strTemp(str);

                strTemp.replace(_T(" "), _T(""));

                size_t pos = 0;

                // cx
                pSizeValue->cx = _ttoi(strTemp.GetData());

                // cy
                pos = strTemp.find(_T(','), pos);
                if(pos == XString::npos)
                    return FALSE;
                ++ pos;
                pSizeValue->cy = _ttoi(strTemp.GetData() + pos);

                return TRUE;
            }

            BOOL StringToColor(LPCTSTR str, COLORREF* pClrValue)
            {
                unsigned int Red = 0;
                unsigned int Green = 0;
                unsigned int Blue = 0;
                int nResult = _sntscanf(str + 1, 6, _T("%02x%02x%02x"), &Red, &Green, &Blue);
                if(nResult != 3)
                    return FALSE;

                *pClrValue = RGB(Red, Green, Blue);
                return TRUE;
            }

        }

    }
}