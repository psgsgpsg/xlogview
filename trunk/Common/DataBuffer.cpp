#include "stdafx.h"
#include "DataBuffer.h"

#pragma warning(disable: 4996)

namespace LogView
{
    namespace Util
    {
        CDataBuffer::CDataBuffer()
        {
            m_nReadPos = 0;
        }

        CDataBuffer::~CDataBuffer()
        {
            ;
        }

        XString CDataBuffer::ToString()
        {
            return m_strData;
        }

        LPCTSTR CDataBuffer::ToString(DWORD& dwSize)
        {
            dwSize = m_strData.size();
            return m_strData.c_str();
        }

        void CDataBuffer::Reset(LPCTSTR szData /* = NULL */, DWORD dwDataLen /* = 0 */)
        {
            m_strData = _T("");
            if(szData != NULL)
            {
                m_strData.assign(szData, dwDataLen);
            }
            m_nReadPos = 0;
        }

        BOOL CDataBuffer::Readable()
        {
            return m_nReadPos < m_strData.size();
        }

        //////////////////////////////////////////////////////////////////////////
        // Output Data to Stream

        CDataBuffer& CDataBuffer::operator << (wchar_t* s)
        {
            PushData(s);
            return (*this);
        }

        CDataBuffer& CDataBuffer::operator << (const wchar_t* s)
        {
            PushData(s);
            return (*this);
        }


        CDataBuffer& CDataBuffer::operator << (LONG l)
        {
            XString strTemp;
            strTemp;
            strTemp.Format(_T("%d"), l);
            PushData(strTemp.c_str());
            return (*this);
        }

        CDataBuffer& CDataBuffer::operator << (int n)
        {
            XString strTemp;
            strTemp.Format(_T("%d"), n);
            PushData(strTemp.c_str());
            return (*this);
        }

        CDataBuffer& CDataBuffer::operator << (size_t n)
        {
            XString strTemp;
            strTemp.Format(_T("%d"), n);
            PushData(strTemp.c_str());
            return (*this);
        }

        CDataBuffer& CDataBuffer::operator << (const std::wstring& s)
        {
            PushData(s.c_str());
            return (*this);
        }

        CDataBuffer& CDataBuffer::operator << (DWORD d)
        {
            XString strTemp;
            strTemp.Format(_T("%u"), d);
            PushData(strTemp.c_str());
            return (*this);
        }

        CDataBuffer& CDataBuffer::operator << (unsigned short us)
        {
            XString strTemp;
            strTemp.Format(_T("%us"), us);
            PushData(strTemp.c_str());
            return (*this);
        }

        //////////////////////////////////////////////////////////////////////////
        // Input Data from Stream

        CDataBuffer& CDataBuffer::operator >> (LONG& l)
        {
            if(Readable())
            {
                XString strTemp = PopData();
                _sntscanf(strTemp.c_str(), strTemp.size(), _T("%d"), &l);
            }
            return (*this);
        }

        CDataBuffer& CDataBuffer::operator >> (int& n)
        {
            if(Readable())
            {
                XString strTemp = PopData();
                _sntscanf(strTemp.c_str(), strTemp.size(), _T("%d"), &n);
            }
            return (*this);
        }

        CDataBuffer& CDataBuffer::operator >> (size_t& n)
        {
            if(Readable())
            {
                XString strTemp = PopData();
                _sntscanf(strTemp.c_str(), strTemp.size(), _T("%u"), &n);
            }
            return (*this);
        }

        CDataBuffer& CDataBuffer::operator >> (std::wstring& s)
        {
            if(Readable())
            {
                s = PopData();
            }
            return (*this);
        }

        CDataBuffer& CDataBuffer::operator >> (XString& s)
        {
            if(Readable())
            {
                s = PopData();
            }
            return (*this);
        }

        CDataBuffer& CDataBuffer::operator >> (DWORD& d)
        {
            if(Readable())
            {
                XString strTemp = PopData();
                _sntscanf(strTemp.c_str(), strTemp.size(), _T("%u"), &d);
            }
            return (*this);
        }

        CDataBuffer& CDataBuffer::operator >> (unsigned short& us)
        {
            if(Readable())
            {
                XString strTemp = PopData();
                _sntscanf(strTemp.c_str(), strTemp.size(), _T("%us"), &us);
            }
            return (*this);
        }

        void CDataBuffer::PushData(const wchar_t* szData)
        {
            XString strTemp = szData;
            if(strTemp.empty())
            {
                m_strData.push_back(_T('\0'));
            }
            else
            {
                m_strData += strTemp;
            }
            m_strData.push_back(_T('\0'));
        }

        XString CDataBuffer::PopData()
        {
            XString strResult;
            if(Readable())
            {
                size_t nPos = m_strData.find(_T('\0'), m_nReadPos);
                strResult = m_strData.substr(m_nReadPos, nPos - m_nReadPos);
                if(strResult.empty())
                {
                    ++ nPos;
                }
                m_nReadPos = nPos + 1;
            }
            return strResult;
        }

    }
}