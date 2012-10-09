#include "StdAfx.h"
#include "Config.h"

#pragma warning(disable: 4996)


#define DEFAULT_SHOW_TYPES                  _T("")
#define DEFAULT_MAX_LOG_COUNT               1000000
#define DEFAULT_CLEAR_MAX_COUNT_REACHED     TRUE
#define DEFAULT_TOPMOST                     FALSE
#define DEFAULT_AUTOSCROLL                  TRUE
#define DEFAULT_CLEAR_FILTER_WHEN_CLEAR_LOG FALSE
#define DEFAULT_ENABLE_REGEX                FALSE

CConfig::CConfig(void)
{
    m_vctShowTypes.push_back(Column_Index);
    m_vctShowTypes.push_back(Column_Time);
    m_vctShowTypes.push_back(Column_ProcId);
    m_vctShowTypes.push_back(Column_ThreadId);
    m_vctShowTypes.push_back(Column_Level);
    m_vctShowTypes.push_back(Column_Filter);
    m_vctShowTypes.push_back(Column_Log);

    m_dwMaxLogCount = DEFAULT_MAX_LOG_COUNT;
    m_bClearMaxCountReached = DEFAULT_CLEAR_MAX_COUNT_REACHED;
    m_bTopMost = DEFAULT_TOPMOST;
    m_bAutoScroll = DEFAULT_AUTOSCROLL;
    m_bClearFilterWhenClearLog = DEFAULT_CLEAR_FILTER_WHEN_CLEAR_LOG;
    m_bEnableRegex = DEFAULT_ENABLE_REGEX;
}

CConfig::~CConfig(void)
{
}

//////////////////////////////////////////////////////////////////////////
CConfig& CConfig::GetConfig()
{
    static CConfig instance;
    return instance;
}

//////////////////////////////////////////////////////////////////////////

#define CONFIG_APP_NAME                     _T("LogView")
#define CONFIG_KEY_SHOWTYPES                _T("ShowTypes")
#define CONFIG_KEY_MAX_LOG_COUNT            _T("MaxLogCount")
#define CONFIG_KEY_CLEAR_MAX_LOG_REACHED    _T("ClearMaxLogReached")
#define CONFIG_KEY_TOPMOST                  _T("TopMost")
#define CONFIG_KEY_AUTOSCROLL               _T("AutoScroll")
#define CONFIG_KEY_CLEAR_FILTER             _T("ClearFilterWhenClearLog")
#define CONFIG_KEY_ENABLE_REGEX             _T("EnableRegex")

void CConfig::ReloadConfig()
{
    TCHAR szBuffer[50] = {0};

    TCHAR szConfigFilePath[MAX_PATH] = {0};
    ::GetModuleFileName(NULL, szConfigFilePath, MAX_PATH);
    *(_tcsrchr(szConfigFilePath, _T('\\')) + 1) = 0;
    _tcsncat(szConfigFilePath, _T("LogView.ini"), MAX_PATH);

    // ShowTypes
    m_vctShowTypes.clear();
    ::GetPrivateProfileString(CONFIG_APP_NAME, CONFIG_KEY_SHOWTYPES, DEFAULT_SHOW_TYPES, szBuffer, 50, szConfigFilePath);
    if(szBuffer[0] == 0)
    {
        m_vctShowTypes.push_back(Column_Index);
        m_vctShowTypes.push_back(Column_Time);
        m_vctShowTypes.push_back(Column_ProcId);
        m_vctShowTypes.push_back(Column_ThreadId);
        m_vctShowTypes.push_back(Column_Level);
        m_vctShowTypes.push_back(Column_Filter);
        m_vctShowTypes.push_back(Column_Log);
    }
    else
    {
        DWORD dwType = 0;
        TCHAR* pToken = _tcstok(szBuffer, _T(","));
        while(pToken != NULL)
        {
            dwType = _ttoi(pToken);
            if(dwType < Column_Max && dwType > Column_Unknown)
                m_vctShowTypes.push_back((ColumnType)dwType);
            pToken = _tcstok(NULL, _T(","));
        }
    }

    // 
#define STRING_NUM(x)   #x
    ::GetPrivateProfileString(CONFIG_APP_NAME,
        CONFIG_KEY_MAX_LOG_COUNT,
        _T(STRING_NUM(DEFAULT_MAX_LOG_COUNT)),
        szBuffer,
        50,
        szConfigFilePath);
    _sntscanf(szBuffer, MAX_PATH, _T("%u"), &m_dwMaxLogCount);

    ::GetPrivateProfileString(CONFIG_APP_NAME,
        CONFIG_KEY_CLEAR_MAX_LOG_REACHED,
        DEFAULT_CLEAR_MAX_COUNT_REACHED ? _T("true") : _T("false"),
        szBuffer,
        50,
        szConfigFilePath);
    m_bClearMaxCountReached = (_tcsicmp(szBuffer, _T("true")) == 0);

    ::GetPrivateProfileString(CONFIG_APP_NAME,
        CONFIG_KEY_TOPMOST,
        DEFAULT_TOPMOST ? _T("true") : _T("false"),
        szBuffer,
        50,
        szConfigFilePath);
    m_bTopMost = (_tcsicmp(szBuffer, _T("true")) == 0);

    ::GetPrivateProfileString(CONFIG_APP_NAME,
        CONFIG_KEY_AUTOSCROLL,
        DEFAULT_AUTOSCROLL ? _T("true") : _T("false"),
        szBuffer,
        50,
        szConfigFilePath);
    m_bAutoScroll = (_tcsicmp(szBuffer, _T("true")) == 0);

    ::GetPrivateProfileString(CONFIG_APP_NAME,
        CONFIG_KEY_CLEAR_FILTER,
        DEFAULT_CLEAR_FILTER_WHEN_CLEAR_LOG ? _T("true") : _T("false"),
        szBuffer,
        50,
        szConfigFilePath);
    m_bClearFilterWhenClearLog = (_tcsicmp(szBuffer, _T("true")) == 0);

    ::GetPrivateProfileString(CONFIG_APP_NAME,
        CONFIG_KEY_ENABLE_REGEX,
        DEFAULT_ENABLE_REGEX ? _T("true") : _T("false"),
        szBuffer,
        50,
        szConfigFilePath);
    m_bEnableRegex = (_tcsicmp(szBuffer, _T("true")) == 0);
}

void CConfig::SaveConfig()
{
    TCHAR szTemp[50] = {0};
    TCHAR szBuffer[50] = {0};

    TCHAR szConfigFilePath[MAX_PATH] = {0};
    ::GetModuleFileName(NULL, szConfigFilePath, MAX_PATH);
    *(_tcsrchr(szConfigFilePath, _T('\\')) + 1) = 0;
    _tcsncat(szConfigFilePath, _T("LogView.ini"), MAX_PATH);

    for(size_t i=0; i<m_vctShowTypes.size(); ++ i)
    {
        if(i == m_vctShowTypes.size() - 1)
            _sntprintf(szTemp, 50, _T("%d"), m_vctShowTypes[i]);
        else
            _sntprintf(szTemp, 50, _T("%d,"), m_vctShowTypes[i]);
        _tcsncat(szBuffer, szTemp, 50);
    }

    ::WritePrivateProfileString(CONFIG_APP_NAME, CONFIG_KEY_SHOWTYPES,      szBuffer, szConfigFilePath);

    _sntprintf(szBuffer, MAX_PATH, _T("%u"), m_dwMaxLogCount);
    ::WritePrivateProfileString(CONFIG_APP_NAME, CONFIG_KEY_MAX_LOG_COUNT,  szBuffer, szConfigFilePath);

    _sntprintf(szBuffer, MAX_PATH, _T("%s"), m_bClearMaxCountReached ? _T("true") : _T("false"));
    ::WritePrivateProfileString(CONFIG_APP_NAME, CONFIG_KEY_CLEAR_MAX_LOG_REACHED, szBuffer, szConfigFilePath);

    _sntprintf(szBuffer, MAX_PATH, _T("%s"), m_bTopMost ? _T("true") : _T("false"));
    ::WritePrivateProfileString(CONFIG_APP_NAME, CONFIG_KEY_TOPMOST,        szBuffer, szConfigFilePath);

    _sntprintf(szBuffer, MAX_PATH, _T("%s"), m_bAutoScroll ? _T("true") : _T("false"));
    ::WritePrivateProfileString(CONFIG_APP_NAME, CONFIG_KEY_AUTOSCROLL,     szBuffer, szConfigFilePath);

    _sntprintf(szBuffer, MAX_PATH, _T("%s"), m_bClearFilterWhenClearLog ? _T("true") : _T("false"));
    ::WritePrivateProfileString(CONFIG_APP_NAME, CONFIG_KEY_CLEAR_FILTER,   szBuffer, szConfigFilePath);

    _sntprintf(szBuffer, MAX_PATH, _T("%s"), m_bEnableRegex ? _T("true") : _T("false"));
    ::WritePrivateProfileString(CONFIG_APP_NAME, CONFIG_KEY_ENABLE_REGEX,   szBuffer, szConfigFilePath);
}


//////////////////////////////////////////////////////////////////////////
void CConfig::GetShowType(ColumnTypeVector& vctColumn) const
{
    vctColumn = m_vctShowTypes;
}

void CConfig::SetShowType(const ColumnTypeVector& vctColumn)
{
    m_vctShowTypes = vctColumn;
}

DWORD CConfig::GetMaxLogCount() const
{
    return m_dwMaxLogCount;
}

void CConfig::SetMaxLogCount(DWORD dwMaxLogCount)
{
    m_dwMaxLogCount = dwMaxLogCount;
}

BOOL CConfig::IsClearMaxCountReached() const
{
    return m_bClearMaxCountReached;
}

void CConfig::SetClearMaxCountReached(BOOL bClear)
{
    m_bClearMaxCountReached = bClear;
}

BOOL CConfig::GetTopMost() const
{
    return m_bTopMost;
}

void CConfig::SetTopMost(BOOL bTopMost)
{
    m_bTopMost = bTopMost;
}

BOOL CConfig::GetAutoScroll() const
{
    return m_bAutoScroll;
}

void CConfig::SetAutoScroll(BOOL bAutoScroll)
{
    m_bAutoScroll = bAutoScroll;
}

BOOL CConfig::IsClearFilterWhenClearLog() const
{
    return m_bClearFilterWhenClearLog;
}

void CConfig::SetClearFilterWhenClearLog(BOOL bClear)
{
    m_bClearFilterWhenClearLog = bClear;
}

BOOL CConfig::IsRegexEnabled() const
{
    return m_bEnableRegex;
}

void CConfig::SetRegexEnabled(BOOL bEnable)
{
    m_bEnableRegex = bEnable;
}
