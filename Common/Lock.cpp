#include "stdafx.h"
#include "Lock.h"

#pragma warning(disable: 4996)

namespace LogView
{
    namespace Util
    {
        //////////////////////////////////////////////////////////////////////////
        CAiSection::CAiSection()
        {
            InitializeCriticalSection(&m_Section);
        }

        CAiSection::~CAiSection()
        {
            DeleteCriticalSection(&m_Section);
        }

        void CAiSection::Lock()
        {
            EnterCriticalSection(&m_Section);
            CAiLock::Lock();
        }

        void CAiSection::UnLock()
        {
            LeaveCriticalSection(&m_Section);
            CAiLock::UnLock();
        }

        BOOL CAiSection::TryToLock()
        {
            BOOL bResult = TryEnterCriticalSection(&m_Section);
            if(bResult)
                CAiLock::TryToLock();
            return bResult;
        }

        //////////////////////////////////////////////////////////////////////////

        CAiMutex::CAiMutex(LPCTSTR szMutexName)
        {
            m_hMutex = CreateMutex(NULL, FALSE, szMutexName);
        }

        CAiMutex::~CAiMutex()
        {
            if(m_hMutex != NULL)
            {
                CloseHandle(m_hMutex);
                m_hMutex = NULL;
            }
        }

        void CAiMutex::Lock()
        {
            if(m_hMutex != NULL)
            {
                WaitForSingleObject(m_hMutex, INFINITE);
                CAiLock::Lock();
            }
        }

        void CAiMutex::UnLock()
        {
            if(m_hMutex != NULL)
            {
                ReleaseMutex(m_hMutex);
                CAiLock::UnLock();
            }
        }

        BOOL CAiMutex::TryToLock()
        {
            BOOL bResult = FALSE;
            if(m_hMutex != NULL)
                bResult = WaitForSingleObject(m_hMutex, 0) == WAIT_OBJECT_0;
            if(bResult)
                CAiLock::TryToLock();
            return bResult;
        }

        //////////////////////////////////////////////////////////////////////////

        CAiEvent::CAiEvent(LPCTSTR szEventName)
        {
            m_hEvent = CreateEvent(NULL, FALSE, TRUE, szEventName);
        }

        CAiEvent::~CAiEvent()
        {
            if(m_hEvent != NULL)
            {
                CloseHandle(m_hEvent);
                m_hEvent = NULL;
            }
        }

        void CAiEvent::Lock()
        {
            if(m_hEvent != NULL)
            {
                WaitForSingleObject(m_hEvent, INFINITE);
                CAiLock::Lock();
            }
        }

        void CAiEvent::UnLock()
        {
            if(m_hEvent != NULL)
            {
                SetEvent(m_hEvent);
                CAiLock::UnLock();
            }
        }

        BOOL CAiEvent::TryToLock()
        {
            BOOL bResult = FALSE;
            if(m_hEvent != NULL)
                bResult = WaitForSingleObject(m_hEvent, 0) == WAIT_OBJECT_0;
            if(bResult)
                CAiLock::TryToLock();
            return bResult;
        }

    }
}