#pragma once


// 打印Log
BOOL OutputLogImpl(LPCTSTR szLevel, LPCTSTR szFilter, LPCTSTR szFormat, ...);

// 设置显示Log保存文件路径和级别，指定的Level在szLevel中存在则认为需要输出
void SetOutputLogPathImpl(LPCTSTR szPath, LPCTSTR szLevel);

#if defined(_DEBUG) || defined(_X_OUTPUT_LOG_)

#define XLog(level, filter, format, ...)    OutputLogImpl((level),  (filter), (format), __VA_ARGS__)
#define XLogInfo(filter, format, ...)       XLog(_T("info"),        (filter), (format), __VA_ARGS__)
#define XLogWarn(filter, format, ...)       XLog(_T("warn"),        (filter), (format), __VA_ARGS__)
#define XLogError(filter, format, ...)      XLog(_T("error"),       (filter), (format), __VA_ARGS__)

#define XSetOutputLogPath(path, level)      SetOutputLogPathImpl(path, level)

#else   // _DEBUG

#define XLog(level, filter, format, ...)    __noop
#define XLogInfo(filter, format, ...)       __noop
#define XLogWarn(filter, format, ...)       __noop
#define XLogError(filter, format, ...)      __noop

#define XSetOutputLogPath(path, level)      __noop

#endif  // _DEBUG


// 链接库内容
#ifndef _NO_X_OUTPUT_LOG_

#ifdef  _DEBUG

    // Debug
    #if defined(_DLL)
        #pragma comment(lib, "dLogClient.lib")
    #else
        #pragma comment(lib, "sdLogClient.lib")
    #endif  // _X_STATIC_OUTOUT_LOG_

#else

    // Release
    #if defined(_DLL)
        #pragma comment(lib, "rLogClient.lib")
    #else
        #pragma comment(lib, "srLogClient.lib")
    #endif  // _X_STATIC_OUTOUT_LOG_

#endif  // _DEBUG


#endif  // _NO_X_OUTPUT_LOG_