#pragma once

#include "DataBuffer.h"

namespace LogView
{
    namespace Util
    {
        struct stLogTime
        {
            stLogTime()
            {
                SYSTEMTIME LocalTime = {0};
                GetLocalTime(&LocalTime);
                uHour    = LocalTime.wHour;
                uMin     = LocalTime.wMinute;
                uSecond  = LocalTime.wSecond;
                uMilli   = LocalTime.wMilliseconds;
            }
            unsigned short uHour;
            unsigned short uMin;
            unsigned short uSecond;
            unsigned short uMilli;
        };

        struct stLogInfo
        {
            DWORD       dwThreadId;
            DWORD       dwProcId;
            stLogTime   tTime;
            UINT        uLevel;
            XString    strFilter;
            XString    strLog;
        };

        typedef std::vector<stLogInfo> LogInfoVector;
        typedef std::vector<size_t>    LogInfoResultIndex;

        __inline CDataBuffer& operator << (CDataBuffer& _DataBuffer, const stLogTime& LogTime)
        {
            _DataBuffer << LogTime.uHour;
            _DataBuffer << LogTime.uMin;
            _DataBuffer << LogTime.uSecond;
            _DataBuffer << LogTime.uMilli;
            return _DataBuffer;
        }

        __inline CDataBuffer& operator >> (CDataBuffer& _DataBuffer, stLogTime& LogTime)
        {
            _DataBuffer >> LogTime.uHour;
            _DataBuffer >> LogTime.uMin;
            _DataBuffer >> LogTime.uSecond;
            _DataBuffer >> LogTime.uMilli;
            return _DataBuffer;
        }


        __inline CDataBuffer& operator << (CDataBuffer& _DataBuffer, const stLogInfo& info)
        {
            _DataBuffer << info.dwThreadId;
            _DataBuffer << info.dwProcId;
            _DataBuffer << info.uLevel;
            _DataBuffer << info.strFilter;
            _DataBuffer << info.strLog;
            return _DataBuffer;
        }

        __inline CDataBuffer& operator >> (CDataBuffer& _DataBuffer, stLogInfo& info)
        {
            _DataBuffer >> info.dwThreadId;
            _DataBuffer >> info.dwProcId;
            _DataBuffer >> info.uLevel;
            _DataBuffer >> info.strFilter;
            _DataBuffer >> info.strLog;
            return _DataBuffer;
        }

    }
}