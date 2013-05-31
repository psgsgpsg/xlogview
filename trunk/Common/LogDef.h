#pragma once



#include <vector>

#define MAX_LEVEL_LENGTH        30
#define MAX_FILTER_LENGTH       30
#define MAX_LOG_LENGTH          512


#define LOG_IPC_MAGIC_NUM       0x82374196
#define LOG_SERVER_PORT_SIG     _T("X_LOG_SERVER_{20F08D95-3A1F-456a-80A7-818FC09F7C29}")
#define LOG_SERVER_PORT_NAME    _T("X_LOG_SERVER_NAME_{833543E3-20D2-4b97-B26F-443CF4A7BC6E}")



struct stLogData
{
    DWORD dwThreadId;
    DWORD dwProcId;
    SYSTEMTIME timeLog;
    LPCTSTR szLevel;
    LPCTSTR szFilter;
    LPCTSTR szLog;
};
typedef std::vector<stLogData> LogDataVector;



typedef void (*NewLogCallback)(const stLogData& log, void* pData);
typedef BOOL (*QueryLogCallback)(int nIndex, stLogData& log, void* pData);


