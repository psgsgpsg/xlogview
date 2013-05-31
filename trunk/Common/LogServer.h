#pragma once

#include "LogDef.h"

class ILogListener
{
public:
    virtual ~ILogListener(){}

    virtual void Release(){}
    virtual void OnLogArrived(const stLogData& log){}
};


BOOL XStartLogServer(LPCTSTR szName);
void XStopLogServer();
void XSetLogListener(ILogListener* pListener);
void XAppendLog(const stLogData& log, void* pData);


BOOL XOpenLogFile(LPCTSTR szFile, NewLogCallback callback, void* pData);
BOOL XSaveLogFile(LPCTSTR szFile, QueryLogCallback callback, void* pData);


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