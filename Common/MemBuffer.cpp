#include "stdafx.h"
#include "MemBuffer.h"

namespace LogView
{
    namespace Util
    {
        CMemBuffer::CMemBuffer()
        {
            m_pBuffer = NULL;
            m_dwSize = 0;
        }

        CMemBuffer::~CMemBuffer()
        {
            ReleaseBuffer();
        }

        void* CMemBuffer::GetBuffer()
        {
            return GetBufferImpl(m_dwSize);
        }

        void* CMemBuffer::GetBuffer(DWORD dwSize)
        {
            return GetBufferImpl(dwSize);
        }

        void CMemBuffer::ReleaseBuffer()
        {
            if(m_pBuffer != NULL)
            {
                delete[] m_pBuffer;
                m_pBuffer = NULL;
                m_dwSize = 0;
            }
        }

        void* CMemBuffer::GetBufferImpl(DWORD dwSize)
        {
            if(dwSize > m_dwSize)
            {
                ReleaseBuffer();
                m_pBuffer = new BYTE[dwSize];
                if(m_pBuffer != NULL)
                    m_dwSize = dwSize;
            }
            return (void*)m_pBuffer;
        }
    }
}