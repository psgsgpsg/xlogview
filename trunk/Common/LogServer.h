#pragma once

#include "LogDef.h"
#include "LogImplDef.h"

class ILogListener
{
public:
    virtual ~ILogListener(){}

    virtual void Release() = 0;
    virtual void OnLogArrived(const LogView::Util::stLogInfo& info) = 0;
};

BOOL InitLogServer(LPCTSTR szName);
void AddLogListener(ILogListener* pListener);
void DelLogListener(ILogListener* pListener);
void UnInitLogServer();
void OnLogArrived(const LogView::Util::stLogInfo& info);
void ShowExistedLogServer(LPCTSTR szName);

BOOL StartLogOutputDebugString();
void StopLogOutputDebugString();


// Á´½Ó¿âÄÚÈÝ
#ifndef _NO_X_OUTPUT_LOG_

#ifdef  _DEBUG

    // Debug
    #if defined(_DLL)
        #pragma comment(lib, "dLogServer.lib")
    #else
        #pragma comment(lib, "sdLogServer.lib")
    #endif  // _X_STATIC_OUTOUT_LOG_

#else

    // Release
    #if defined(_DLL)
        #pragma comment(lib, "rLogServer.lib")
    #else
        #pragma comment(lib, "srLogServer.lib")
    #endif  // _X_STATIC_OUTOUT_LOG_

#endif  // _DEBUG


#endif  // _NO_X_OUTPUT_LOG_