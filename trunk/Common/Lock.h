#pragma once

//////////////////////////////////////////////////////////////////////////

namespace LogView
{
    namespace Util
    {
        class CAiLock
        {
        public:
            CAiLock()
            {
            }

            virtual ~CAiLock()
            {
            }

        public:
            virtual void Lock() = 0
            {
            }
            virtual void UnLock() = 0
            {
            }
            virtual BOOL TryToLock() = 0
            {
                return FALSE;
            }
        };

        //////////////////////////////////////////////////////////////////////////

        class CAiSection : public CAiLock
        {
        public:
            CAiSection();
            ~CAiSection();

        public:
            virtual void Lock();
            virtual void UnLock();
            virtual BOOL TryToLock();

        protected:
            CRITICAL_SECTION m_Section;
        };

        //////////////////////////////////////////////////////////////////////////

        class CAiMutex : public CAiLock
        {
        public:
            CAiMutex(LPCTSTR szMutexName);
            ~CAiMutex();

        public:
            virtual void Lock();
            virtual void UnLock();
            virtual BOOL TryToLock();

        protected:
            HANDLE  m_hMutex;
        };

        //////////////////////////////////////////////////////////////////////////

        class CAiEvent : public CAiLock
        {
        public:
            CAiEvent(LPCTSTR szEventName);
            ~CAiEvent();

        public:
            virtual void Lock();
            virtual void UnLock();
            virtual BOOL TryToLock();

        protected:
            HANDLE  m_hEvent;
        };

        //////////////////////////////////////////////////////////////////////////

        class CAiAutoLock
        {
        private:
            CAiLock * m_pLock;
        public:

            CAiAutoLock(CAiLock * pLock)
            {
                m_pLock = pLock;
                if(m_pLock)
                {
                    m_pLock->Lock();
                }
            }

            ~CAiAutoLock()
            {
                if(m_pLock)
                {
                    m_pLock->UnLock();
                }
            }
        };

        //////////////////////////////////////////////////////////////////////////
        class CAiAutoTryLock
        {
        private:
            CAiLock * m_pLock;
        public:
            CAiAutoTryLock(CAiLock * pLock)
            {
                m_bLocked = FALSE;
                m_pLock = pLock;
                if(m_pLock)
                {
                    m_bLocked = m_pLock->TryToLock();
                }
            }

            ~CAiAutoTryLock()
            {
                if(m_pLock && m_bLocked)
                {
                    m_pLock->UnLock();
                    m_bLocked = FALSE;
                }
            }

        protected:
            BOOL    m_bLocked;
        };

        //////////////////////////////////////////////////////////////////////////

#define AI_AUTO_LOCK_HELPER2(x,y) x##y
#define AI_AUTO_LOCK_HELPER1(x,y) AI_AUTO_LOCK_HELPER2(x,y)
#define AI_AUTO_LOCK_HELPER(x) AI_AUTO_LOCK_HELPER1(x,__LINE__)

#define AI_AUTO_LOCK(pLock)      LogView::Util::CAiAutoLock    AI_AUTO_LOCK_HELPER(auto_lock_)((pLock));
#define AI_AUTO_TRY_LOCK(pLock)  LogView::Util::CAiAutoTryLock AI_AUTO_LOCK_HELPER(auto_lock_)((pLock));
    }
}