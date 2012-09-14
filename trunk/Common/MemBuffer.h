#pragma once



namespace LogView
{
    namespace Util
    {
        class CMemBuffer
        {
        public:
            CMemBuffer();
            ~CMemBuffer();

            void*   GetBuffer();
            void*   GetBuffer(DWORD dwSize);
            void    ReleaseBuffer();

            template <typename T>
            operator T()
            {
                return (T)GetBuffer();
            }

        protected:
            BYTE*   m_pBuffer;
            DWORD   m_dwSize;

        protected:
            void*   GetBufferImpl(DWORD dwSize);
        };

    }
}