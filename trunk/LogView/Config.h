#pragma once

#include "UIDefine.h"

#define DEFAULT_SHOW_TYPES                  _T("")
#define DEFAULT_MAX_LOG_COUNT               1000000
#define DEFAULT_CLEAR_MAX_COUNT_REACHED     TRUE
#define DEFAULT_TOPMOST                     FALSE
#define DEFAULT_AUTOSCROLL                  TRUE
#define DEFAULT_CLEAR_FILTER_WHEN_CLEAR_LOG FALSE
#define DEFAULT_ENABLE_REGEX                FALSE

class CConfig
{
    CConfig(void);
    CConfig& operator = (CConfig& config);
    CConfig(const CConfig& config);
public:
    ~CConfig(void);

public:
    static CConfig& GetConfig();

public:
    void    ReloadConfig();
    void    SaveConfig();

public:
    void    GetShowType(ColumnTypeVector& vctColumn) const;
    void    SetShowType(const ColumnTypeVector& vctColumn);

    DWORD   GetMaxLogCount() const;
    void    SetMaxLogCount(DWORD dwMaxLogCount);

    BOOL    IsClearMaxCountReached() const;
    void    SetClearMaxCountReached(BOOL bClear);

    BOOL    GetTopMost() const;
    void    SetTopMost(BOOL bTopMost);

    BOOL    GetAutoScroll() const;
    void    SetAutoScroll(BOOL bAutoScroll);

    BOOL    IsClearFilterWhenClearLog() const;
    void    SetClearFilterWhenClearLog(BOOL bClear);

    BOOL    IsRegexEnabled() const;
    void    SetRegexEnabled(BOOL bEnable);

protected:
    ColumnTypeVector    m_vctShowTypes;
    DWORD               m_dwMaxLogCount;
    BOOL                m_bClearMaxCountReached;
    BOOL                m_bTopMost;
    BOOL                m_bAutoScroll;
    BOOL                m_bClearFilterWhenClearLog;
    BOOL                m_bEnableRegex;
};
