#pragma once

#include <vector>

typedef enum
{
    PANEL_UNKNOWN,
    PANEL_PROCESS,
    PANEL_FILTER,
    PANEL_LEVEL,
} PanelType;

typedef enum
{
    Column_Unknown,
    Column_Index,
    Column_Time,
    Column_ProcId,
    Column_ThreadId,
    Column_Level,
    Column_Filter,
    Column_Log,
    Column_Max,
} ColumnType;

typedef std::vector<ColumnType> ColumnTypeVector;

class CColumnTypeSortHelper
{
public:

    bool operator() (ColumnType type1, ColumnType type2)
    {
        return type1 <= type2;
    }

};


// wParam: PanelType
const UINT g_uPanelHiddenMsg          = ::RegisterWindowMessage(_T("PanelHiddenMsg"));

// wParam: Filter Index
// lParam: dwParam
const UINT g_uFilterPanelCallbackMsg  = ::RegisterWindowMessage(_T("FilterCallbackMsg"));

// wParam: Proc Id
// lParam: Thread Id
const UINT g_uProcePanelCallbackMsg   = ::RegisterWindowMessage(_T("ProcCallbackMsg"));

// wParam: Level Index
// lParam: dwParam
const UINT g_uLevelPanelCallbackMsg  = ::RegisterWindowMessage(_T("LevelCallbackMsg"));


const UINT g_uFindReplaceMsg = ::RegisterWindowMessage(FINDMSGSTRING);

