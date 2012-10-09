#pragma once

#include "UIDefine.h"

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
