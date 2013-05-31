
// LogViewDlg.h : 头文件
//

#pragma once

#include "../Common/LogDef.h"
#include "afxcmn.h"

#include "ProcessPanel.h"
#include "OptionSelectPanel.h"

#include <vector>

struct stLogUIData
{
    DWORD dwThreadId;
    DWORD dwProcId;
    SYSTEMTIME timeLog;
    CString strLevel;
    CString strFilter;
    CString strLog;
};
typedef std::vector<stLogUIData> LogUIDataVector;

__inline void GetUIData(stLogUIData& uidata, const stLogData& data)
{
    uidata.dwThreadId = data.dwThreadId;
    uidata.dwProcId = data.dwProcId;
    uidata.timeLog = data.timeLog;
    uidata.strLevel = data.szLevel;
    uidata.strFilter = data.szFilter;
    uidata.strLog = data.szLog;
}

// CLogViewDlg 对话框
class CLogViewDlg : public CDialog, public ILogListener
{
// 构造
public:
	CLogViewDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_LOGVIEW_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


public:
    // ILogListener
    virtual void Release();
    virtual void OnLogArrived(const stLogData& info);

protected:
// 内部方法

    // 初始化日志List
    void    InitLogList();

    // 初始化状态栏
    void    InitStatusBar();
    void    UpdateStatusBar();

    // 初始化小面板
    void    InitPanel();

    // 初始化查找对话框
    void    InitFindDlg();

    // 获取某SubItem的文字
    CString GetItemText(int nItem, int nSubItem);
    CString GetItemText(int nItem, ColumnType type);

    // 执行菜单命令，返回内部是否已处理
    BOOL    DoCmdId(WORD wId);

    // 每次发生变化的时候，按照用户选择的Filter对日志进行过滤
    void    DoFilter();

    // 添加一条日志的时候，按照过滤规则进行添加
    BOOL    AppendLogWithFilter(const stLogUIData& info);

    // 一条记录是否符合要求
    BOOL    MatchFilter(const stLogUIData& info);
    BOOL    MatchFilter(const stLogUIData& info,
                            const std::vector<CString>& vctFilter,
                            const std::vector<CString>&    vctLevel,
                            const ThreadOptionData&     vctThread
                            );

    BOOL    SubMatchTextFilter(const stLogUIData& info);

    // 清空ListView的选中项
    void    ClearLogListSelection();

    // 调整ListView数目时调用
    void    UpdateItemCount();
    void    DelayUpdateItemCount();

    // 设置/获取某列Header的参数
    void    SetHeaderParam(int nIndex, LPARAM lParam);
    LPARAM  GetHeaderParam(int nIndex);

    // 根据参数，隐藏或者显示Column
    void    FilterColumn(const ColumnTypeVector& vctColumn);
    // 获取当前正被显示的项目
    void    GetColumn(ColumnTypeVector& vctColumn);
    // 添加一项到Column中。
    void    AddColumn(int nIndex, ColumnType type);
    // 删除一项目
    void    DelColumn(ColumnType type);
    // 找到合适的添加位置
    int     FindIdealColumnIndex(ColumnType type);

// 实现
protected:
    HICON           m_hIcon;
    HACCEL          m_hMainAcce;

    CListCtrl       m_LogList;
    LogUIDataVector m_vctLogInfo;

    typedef std::vector<size_t> LogInfoResultIndex;
    LogInfoResultIndex  m_LogResult;

    std::vector<CString>    m_vctTextFilter;

    CProcessPanel       m_ProcPanel;
    COptionSelectPanel  m_LevelPanel;
    COptionSelectPanel  m_FilterPanel;

    BOOL            m_bEditTextFilterUnApplied;

    CFindReplaceDialog* m_pFindDlg;

    CString         m_strFindWhat;
    BOOL            m_bFindDown;
    BOOL            m_bFindNext;
    BOOL            m_bMatchCase;
    BOOL            m_bMatchWholeWord;

    UINT_PTR        m_uDelayUpdateItemCountTimerId;

    HBRUSH          m_hUnAppliedBk;

    CStatusBar      m_StatusBar;
protected:

    //////////////////////////////////////////////////////////////////////////
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

    afx_msg void OnLvnGetdispinfoListLog(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnSize(UINT nType, int cx, int cy);

    // Panel的回调消息
    afx_msg LRESULT OnFilterPanelMsg(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnLevelPanelMsg(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnProcPanelMsg(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnProcHiddenMsg(WPARAM wParam, LPARAM lParam);

    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnDestroy();
    afx_msg void OnTimer(UINT_PTR nIDEvent);

    afx_msg void OnBnClickedBtnDoFilter();
    afx_msg void OnEnChangeEditTextFilter();

    // 查找替换对话框消息响应
    afx_msg LRESULT OnFindReplaceMsg(WPARAM wParam, LPARAM lParam);
protected:
    // 菜单处理方法

    // 文件菜单
    afx_msg void DoFileOpenCmd();
    afx_msg void DoFileSaveCmd();
    afx_msg void DoFileExitCmd();

    // 编辑菜单
    afx_msg void DoEditClearCmd();
    afx_msg void DoEditCopyCmd();
    afx_msg void DoEditCopyWholeLineCmd();
    afx_msg void DoEditFindCmd();

    // 选项板菜单
    afx_msg void DoPanelFilterCmd();
    afx_msg void DoPanelLevelCmd();
    afx_msg void DoPanelProcessCmd();

    void DoPanelCmdHelper(CDialog* pPanel, WORD wCmdId, WORD wCmdId2 = 0);

    // 选项菜单
    afx_msg void DoOptionTopMostCmd();
    afx_msg void DoOptionAutoScrollCmd();
    afx_msg void DoEnableRegexCmd();
    afx_msg void DoEnableWildcardCmd();
    afx_msg void DoOptionOptions();

    // 帮助菜单
    afx_msg void DoHelpAbout();

public:
    afx_msg void OnNMRClickListLog(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMCustomdrawListLog(NMHDR *pNMHDR, LRESULT *pResult);
};
