
// LogViewDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LogView.h"
#include "LogViewDlg.h"

#include <algorithm>
#include "AboutDlg.h"
#include "../Common/XStrUtil.h"

#include "../Common/XLogMgr.h"
#include "LogViewOptionsDlg.h"
#include <time.h>

#include "Config.h"


#pragma warning(disable: 4996)
#pragma warning(disable: 4018)

#define COLOR_TEXT_FILTER_APPLIED           RGB(0, 128, 0)
#define COLOR_TEXT_FILTER_UN_APPLIED        RGB(255, 0, 0)
#define BKCOLOR_TEXT_FILTER_APPLIED         RGB(255, 255, 255)
#define BKCOLOR_TEXT_FILTER_UN_APPLIED      RGB(212, 212, 212)

const UINT g_arrStatusBarIndicators[] = 
{
    ID_SEPARATOR,
};
const size_t g_nStatusBarIndicatorsCount = _countof(g_arrStatusBarIndicators);


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CLogViewDlg 对话框




CLogViewDlg::CLogViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLogViewDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_hMainAcce = LoadAccelerators(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MAIN_ACCE));

    m_bEditTextFilterUnApplied = TRUE;

    m_pFindDlg = NULL;
    m_bFindDown = FALSE;
    m_bFindNext = TRUE;
    m_bMatchCase = FALSE;
    m_bMatchWholeWord = FALSE;

    m_hUnAppliedBk = CreateSolidBrush(BKCOLOR_TEXT_FILTER_UN_APPLIED);
}

void CLogViewDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_LOG, m_LogList);
}

BEGIN_MESSAGE_MAP(CLogViewDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP

    ON_WM_SIZE()
    ON_WM_GETMINMAXINFO()
    ON_WM_CTLCOLOR()
    ON_WM_DESTROY()

    ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_LOG, &CLogViewDlg::OnLvnGetdispinfoListLog)
    ON_BN_CLICKED(IDC_BTN_DO_FILTER, &CLogViewDlg::OnBnClickedBtnDoFilter)
    ON_EN_CHANGE(IDC_EDIT_TEXT_FILTER, &CLogViewDlg::OnEnChangeEditTextFilter)

    ON_REGISTERED_MESSAGE(g_uFilterPanelCallbackMsg, &CLogViewDlg::OnFilterPanelMsg)
    ON_REGISTERED_MESSAGE(g_uLevelPanelCallbackMsg, &CLogViewDlg::OnLevelPanelMsg)
    ON_REGISTERED_MESSAGE(g_uProcePanelCallbackMsg, &CLogViewDlg::OnProcPanelMsg)
    ON_REGISTERED_MESSAGE(g_uPanelHiddenMsg, &CLogViewDlg::OnProcHiddenMsg)
    ON_REGISTERED_MESSAGE(g_uFindReplaceMsg, &CLogViewDlg::OnFindReplaceMsg)

    ON_NOTIFY(NM_RCLICK, IDC_LIST_LOG, &CLogViewDlg::OnNMRClickListLog)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_LOG, &CLogViewDlg::OnNMCustomdrawListLog)
END_MESSAGE_MAP()


// CLogViewDlg 消息处理程序

BOOL CLogViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

    // TODO: 在此添加额外的初始化代码

    SetWindowPos(NULL, 0, 0, 800, 600, SWP_NOZORDER | SWP_NOMOVE);
    CenterWindow(NULL);

    InitStatusBar();
    InitLogList();
    InitPanel();

    {
        ColumnTypeVector vctColumn;
        CConfig::GetConfig().GetShowType(vctColumn);
        FilterColumn(vctColumn);

        CMenu* pPanelMenu = GetMenu()->GetSubMenu(3);

        BOOL bTopMost = CConfig::GetConfig().GetTopMost();
        SetWindowPos(bTopMost ? &wndTopMost : &wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        pPanelMenu->CheckMenuItem(ID_OPTION_TOPMOST, MF_BYCOMMAND | (bTopMost ? MF_CHECKED : MF_UNCHECKED));

        BOOL bAutoScroll = CConfig::GetConfig().GetAutoScroll();
        pPanelMenu->CheckMenuItem(ID_OPTION_AUTOSCROLL, MF_BYCOMMAND | (bAutoScroll ? MF_CHECKED : MF_UNCHECKED));
    }

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CLogViewDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CLogViewDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CLogViewDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//////////////////////////////////////////////////////////////////////////
void CLogViewDlg::OnLvnGetdispinfoListLog(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
    const LVITEM& _Item = pDispInfo->item;

    if(pDispInfo != NULL && (_Item.mask & LVIF_TEXT) == LVIF_TEXT)
    {
        LogView::Util::XString strText = GetItemText(_Item.iItem, _Item.iSubItem);
        _tcsncpy(_Item.pszText, strText.c_str(), _Item.cchTextMax);
        _Item.pszText[_Item.cchTextMax - 1] = 0;
    }

    *pResult = 0;
}

void CLogViewDlg::OnSize(UINT nType, int cx, int cy)
{
    __super::OnSize(nType, cx, cy);

    int nLeft = 0;
    RECT rcDoFilterRect;
    HWND hWndBtnDoFilter = ::GetDlgItem(GetSafeHwnd(), IDC_BTN_DO_FILTER);
    if(hWndBtnDoFilter != NULL)
    {
        ::GetWindowRect(hWndBtnDoFilter, &rcDoFilterRect);
        ScreenToClient(&rcDoFilterRect);
        nLeft = cx - (rcDoFilterRect.right - rcDoFilterRect.left) - 10;
        ::SetWindowPos(hWndBtnDoFilter, NULL,
            nLeft, rcDoFilterRect.top,
            0, 0,
            SWP_NOZORDER | SWP_NOSIZE);
    }

    RECT rcTextFilterRect;
    HWND hWndEditTextFilter = ::GetDlgItem(GetSafeHwnd(), IDC_EDIT_TEXT_FILTER);
    if(hWndEditTextFilter != NULL)
    {
        ::GetWindowRect(hWndEditTextFilter, &rcTextFilterRect);
        ScreenToClient(&rcTextFilterRect);
        ::SetWindowPos(hWndEditTextFilter, NULL,
            0, 0,
            nLeft - rcTextFilterRect.left - 10, rcTextFilterRect.bottom - rcTextFilterRect.top,
            SWP_NOZORDER | SWP_NOMOVE);
    }

    if(m_LogList.GetSafeHwnd() != NULL)
    {
        m_LogList.MoveWindow(0, 26, cx - 0, cy - 26 - 20);
    }

    RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,ID_INDICATOR_NUM);
}

BOOL CLogViewDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    if(HIWORD(wParam) == 0 || HIWORD(wParam) == 1)
    {
        if(DoCmdId(LOWORD(wParam)))
            return TRUE;
    }
    return __super::OnCommand(wParam, lParam);
}

// Panel的回调消息
LRESULT CLogViewDlg::OnFilterPanelMsg(WPARAM wParam, LPARAM lParam)
{
    DoFilter();
    SetActiveWindow();
    return 0;
}

LRESULT CLogViewDlg::OnLevelPanelMsg(WPARAM wParam, LPARAM lParam)
{
    DoFilter();
    SetActiveWindow();
    return 0;
}

LRESULT CLogViewDlg::OnProcPanelMsg(WPARAM wParam, LPARAM lParam)
{
    DoFilter();
    SetActiveWindow();
    return 0;
}

LRESULT CLogViewDlg::OnProcHiddenMsg(WPARAM wParam, LPARAM lParam)
{
    CMenu* pPanelMenu = GetMenu()->GetSubMenu(2);
    if(pPanelMenu == NULL)
        return 0;

    switch(wParam)
    {
    case PANEL_PROCESS:
        pPanelMenu->CheckMenuItem(ID_PANEL_PROCESS, MF_BYCOMMAND | MF_UNCHECKED);
        break;
    case PANEL_FILTER:
        pPanelMenu->CheckMenuItem(ID_PANEL_FILTER, MF_BYCOMMAND | MF_UNCHECKED);
        break;
    case PANEL_LEVEL:
        pPanelMenu->CheckMenuItem(ID_PANEL_LEVEL, MF_BYCOMMAND | MF_UNCHECKED);
        break;
    }

    return 0;
}

LRESULT CLogViewDlg::OnFindReplaceMsg(WPARAM wParam, LPARAM lParam)
{
    CFindReplaceDialog* pFindDlg = CFindReplaceDialog::GetNotifier(lParam);

    // 正在关闭
    if(pFindDlg->IsTerminating())
    {
        delete[] pFindDlg->m_fr.lpstrFindWhat;
        pFindDlg->m_fr.lpstrFindWhat = NULL;
        pFindDlg->m_fr.wFindWhatLen = 0;
        m_pFindDlg = NULL;
        return 0;
    }

    int nItemCount = m_LogList.GetItemCount();
    if(nItemCount < 2)
        return 0;

    m_bFindDown = pFindDlg->SearchDown();
    m_bFindNext = pFindDlg->FindNext();
    m_bMatchCase = pFindDlg->MatchCase();
    m_bMatchWholeWord = pFindDlg->MatchWholeWord();

    int nStart = m_LogList.GetNextItem(-1, LVNI_SELECTED);
    int nEnd = m_bFindDown ? nItemCount - 1 : 0;

    if(nStart == -1)
        nStart = m_bFindDown ? 0 : nItemCount - 1;
    else
        nStart = m_bFindDown ? nStart + 1 : nStart - 1;

    CString strFindWhat(pFindDlg->GetFindString());
    m_strFindWhat = strFindWhat;

    int nNextItem = -1;
    int i = nStart;
    while(TRUE)
    {
        if(m_bFindDown)
        {
            if(i > nEnd)
                break;
        }
        else
        {
            if(i < nEnd)
                break;
        }
        
        CString strLog = GetItemText(i, Column_Log).c_str();

        if(m_bMatchCase)
        {
            // Case-Sensitive
            if(m_bMatchWholeWord)
            {
                if(strLog == strFindWhat)
                {
                    nNextItem = i;
                    break;
                }
            }
            else
            {
                if(strLog.Find(strFindWhat) != -1)
                {
                    nNextItem = i;
                    break;
                }
            }
        }
        else
        {
            // Non Case-Sensitive

            CharLower(strLog.GetBuffer(strLog.GetLength()));
            strLog.ReleaseBuffer(strLog.GetLength());

            CharLower(strFindWhat.GetBuffer(strFindWhat.GetLength()));
            strFindWhat.ReleaseBuffer(strFindWhat.GetLength());

            if(m_bMatchWholeWord)
            {
                if(strLog == strFindWhat)
                {
                    nNextItem = i;
                    break;
                }
            }
            else
            {
                if(strLog.Find(strFindWhat) != -1)
                {
                    nNextItem = i;
                    break;
                }
            }
        }

        i = m_bFindDown ? i + 1 : i - 1;
    }

    if(nNextItem != -1)
    {
        ClearLogListSelection();
        m_LogList.SetItemState(nNextItem, LVIS_SELECTED, LVIS_SELECTED);
        m_LogList.EnsureVisible(nNextItem, FALSE);
    }
    else
    {
        AfxMessageBox(_T("No More Matched Items."), MB_OK | MB_ICONWARNING);
    }

    return 0;
}

void CLogViewDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    __super::OnGetMinMaxInfo(lpMMI);
    lpMMI->ptMinTrackSize.x = 300;
    lpMMI->ptMinTrackSize.y = 300;
}

void CLogViewDlg::OnBnClickedBtnDoFilter()
{
    CString strTextFilter;
    CString strToken;
    GetDlgItemText(IDC_EDIT_TEXT_FILTER, strTextFilter);

    m_vctTextFilter.clear();

    int curPos = 0;
    strToken = strTextFilter.Tokenize(_T(";"),curPos);
    while (strToken != _T(""))
    {
        m_vctTextFilter.push_back((LPCTSTR)strToken);
        strToken = strTextFilter.Tokenize(_T(";"), curPos);
    };

    DoFilter();
    m_bEditTextFilterUnApplied = FALSE;
    GetDlgItem(IDC_EDIT_TEXT_FILTER)->Invalidate(TRUE);
}

//////////////////////////////////////////////////////////////////////////

// ILogListener
void CLogViewDlg::Release()
{
    m_LogResult.clear();
    m_vctLogInfo.clear();
}

void CLogViewDlg::OnLogArrived(const LogView::Util::stLogInfo& info)
{
    if(GetSafeHwnd() == NULL || !IsWindow(GetSafeHwnd()))
        return;

    DWORD nLogCount = m_vctLogInfo.size();
    DWORD nMaxLogCount = CConfig::GetConfig().GetMaxLogCount();
    if(nLogCount >= nMaxLogCount)
    {
        if(CConfig::GetConfig().IsClearMaxCountReached())
            DoEditClearCmd();
        else
            return;
    }

    CString strLevel;
    strLevel.Format(_T("%u"), info.uLevel);

    // 过滤
    m_ProcPanel.AddOption(info.dwThreadId, info.dwProcId, 0);
    m_LevelPanel.AddOption(strLevel, info.uLevel);
    m_FilterPanel.AddOption(info.strFilter.c_str(), 0);

    BOOL bAppended = AppendLogWithFilter(info);
    if(bAppended && CConfig::GetConfig().GetAutoScroll())
        m_LogList.EnsureVisible(m_LogList.GetItemCount() - 1, FALSE);

    UpdateStatusBar();
}

//////////////////////////////////////////////////////////////////////////
// 内部方法
void CLogViewDlg::InitLogList()
{
    struct HeaderParam
    {
        int         nIndex;
        LPCTSTR     szText;
        int         nFormat;
        int         nWidth;
        ColumnType  eType;
    };

    const HeaderParam Param[] =
    {
        0, _T("#"),      LVCFMT_LEFT,   30,     Column_Index,
        1, _T("Time"),   LVCFMT_LEFT,   90,     Column_Time,
        2, _T("Proc"),   LVCFMT_RIGHT,  50,     Column_ProcId,
        3, _T("Thread"), LVCFMT_RIGHT,  60,     Column_ThreadId,
        4, _T("Level"),  LVCFMT_RIGHT,  50,     Column_Level,
        5, _T("Filter"), LVCFMT_LEFT,   60,     Column_Filter,
        6, _T("Log"),    LVCFMT_LEFT,   400,    Column_Log,
    };
    const int nParamCount = _countof(Param);

    int nIndex = 0;
    for(int i=0; i<nParamCount; ++ i)
    {
        nIndex = m_LogList.InsertColumn(Param[i].nIndex, Param[i].szText, Param[i].nFormat, Param[i].nWidth);
        SetHeaderParam(nIndex, Param[i].eType);
    }

    m_LogList.SetExtendedStyle(m_LogList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_HEADERDRAGDROP);
}

// 初始化状态栏
void CLogViewDlg::InitStatusBar()
{
    m_StatusBar.CreateEx(this,SBT_TOOLTIPS,WS_CHILD|WS_VISIBLE|CBRS_BOTTOM,AFX_IDW_STATUS_BAR);
    m_StatusBar.SetIndicators(g_arrStatusBarIndicators, g_nStatusBarIndicatorsCount);

    m_StatusBar.SetPaneInfo(0, ID_SEPARATOR, SBPS_NORMAL | SBPS_STRETCH, 0);

    UpdateStatusBar();

    RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,ID_INDICATOR_NUM);
}

void CLogViewDlg::UpdateStatusBar()
{
    CString strStatus;
    strStatus.Format(_T("显示%d条日志 / 共%d条日志"), m_LogResult.size(), m_vctLogInfo.size());
    m_StatusBar.SetPaneText(0, strStatus);
}

// 初始化小面板
void CLogViewDlg::InitPanel()
{
    RECT rcWnd;
    GetWindowRect(&rcWnd);

    m_LevelPanel.Create(IDD_PANEL_SELECT, this);
    m_LevelPanel.SetWindowText(_T("Level"));
    m_LevelPanel.SetPanelType(PANEL_LEVEL);
    m_LevelPanel.SetWindowPos(NULL, rcWnd.right - 150, rcWnd.top + 50, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_HIDEWINDOW);

    m_FilterPanel.Create(IDD_PANEL_SELECT, this);
    m_FilterPanel.SetWindowText(_T("Filter"));
    m_FilterPanel.SetPanelType(PANEL_FILTER);
    m_FilterPanel.SetWindowPos(NULL, rcWnd.right - 150, rcWnd.top + 150, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_HIDEWINDOW);

    m_ProcPanel.Create(IDD_PANEL_PROCESS, this);
    m_ProcPanel.SetWindowPos(NULL, rcWnd.right - 150, rcWnd.top + 250, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_HIDEWINDOW);
}

// 初始化查找对话框
void CLogViewDlg::InitFindDlg()
{
    m_pFindDlg = new CFindReplaceDialog;

    m_pFindDlg->m_fr.lStructSize = sizeof(m_pFindDlg->m_fr);
    m_pFindDlg->m_fr.hwndOwner = m_hWnd;

    if(m_bFindDown)
        m_pFindDlg->m_fr.Flags |= FR_DOWN;
    if(m_bFindNext)
        m_pFindDlg->m_fr.Flags |= FR_FINDNEXT;
    if(m_bMatchCase)
        m_pFindDlg->m_fr.Flags |= FR_MATCHCASE;
    if(m_bMatchWholeWord)
        m_pFindDlg->m_fr.Flags |= FR_WHOLEWORD;

    m_pFindDlg->m_fr.lpstrFindWhat = new TCHAR[MAX_PATH];
    m_pFindDlg->m_fr.wFindWhatLen = MAX_PATH * sizeof(TCHAR);
    _tcsncpy(m_pFindDlg->m_fr.lpstrFindWhat, m_strFindWhat, MAX_PATH);
    m_pFindDlg->m_fr.lpstrFindWhat[MAX_PATH - 1] = 0;

    m_pFindDlg->Create(TRUE, _T(""), _T(""), FR_DOWN, this);
}

LogView::Util::XString CLogViewDlg::GetItemText(int nItem, int nSubItem)
{
    if(nItem > m_LogResult.size() || m_LogResult[nItem] >= m_vctLogInfo.size())
        return LogView::Util::XString();

    ColumnType type = (ColumnType)GetHeaderParam(nSubItem);
    return GetItemText(nItem, type);
}

LogView::Util::XString CLogViewDlg::GetItemText(int nItem, ColumnType type)
{
    LogView::Util::XString strResult;
    const LogView::Util::stLogInfo& info = m_vctLogInfo[m_LogResult[nItem]];

    switch(type)
    {
    case Column_Index:
        {
            // #
            strResult = LogView::Util::XString(_T("%1")).arg(nItem);
            break;
        }
    case Column_Time:
        {
            // Time
            strResult.Format(_T("%02d:%02d:%02d.%03d"), info.tTime.uHour, info.tTime.uMin, info.tTime.uSecond, info.tTime.uMilli);
            break;
        }
    case Column_ProcId:
        {
            // Proc
            strResult = LogView::Util::XString(_T("%1")).arg(info.dwProcId);
            break;
        }
    case Column_ThreadId:
        {
            // Thread
            strResult = LogView::Util::XString(_T("%1")).arg(info.dwThreadId);
            break;
        }
    case Column_Level:
        {
            // Level
            strResult = LogView::Util::XString(_T("%1")).arg(info.uLevel);
            break;
        }
    case Column_Filter:
        {
            // Filter
            strResult = info.strFilter;
            break;
        }
    case Column_Log:
        {
            // Log
            strResult = info.strLog;
            break;
        }
    default:
        {
            ASSERT(FALSE && _T("GetItemText Failed"));
        }
    }

    return strResult;
}

// 执行菜单命令，返回内部是否已处理
BOOL CLogViewDlg::DoCmdId(WORD wId)
{
    BOOL bHandled = TRUE;
    switch(wId)
    {
        // 文件菜单
    case ID_FILE_OPEN:
        DoFileOpenCmd();
        break;
    case ID_FILE_SAVE:
        DoFileSaveCmd();
        break;
    case ID_FILE_EXIT:
        DoFileExitCmd();
        break;

        // 编辑菜单
    case ID_EDIT_CLEAR:
        DoEditClearCmd();
        break;
    case ID_EDIT_COPY:
        DoEditCopyCmd();
        break;
    case ID_EDIT_COPYWHOLELINE:
        DoEditCopyWholeLineCmd();
        break;
    case ID_EDIT_FIND:
        DoEditFindCmd();
        break;

        // 选项板菜单
    case ID_PANEL_FILTER:
        DoPanelFilterCmd();
        break;
    case ID_PANEL_LEVEL:
        DoPanelLevelCmd();
        break;
    case ID_PANEL_PROCESS:
        DoPanelProcessCmd();
        break;

        // 选项菜单
    case ID_OPTION_TOPMOST:
        DoOptionTopMostCmd();
        break;
    case ID_OPTION_AUTOSCROLL:
        DoOptionAutoScrollCmd();
        break;
    case ID_OPTION_LOGOUTPUTDEBUGSTRING:
        DoOptionLogOutputDebugStringCmd();
        break;
    case ID_OPTION_OPTIONS:
        DoOptionOptions();
        break;

        // 帮助菜单
    case ID_HELP_ABOUT:
        DoHelpAbout();
        break;

    default:
        bHandled = FALSE;
        break;
    }

    return bHandled;
}

// 每次发生变化的时候，按照用户选择的Filter对日志进行过滤
void CLogViewDlg::DoFilter()
{
    m_LogResult.clear();

    std::vector<LogView::Util::XString> vctFilter;
    std::vector<UINT>    vctLevel;
    ThreadOptionData     vctThread;

    m_FilterPanel.GetOptionsText(vctFilter);
    m_LevelPanel.GetOptionsParam(vctLevel);
    m_ProcPanel.GetOptionsThread(vctThread);

    size_t uTotalLogCount = m_vctLogInfo.size();
    for(size_t i=0; i<uTotalLogCount; ++ i)
    {
        const LogView::Util::stLogInfo& info = m_vctLogInfo[i];
        if(MatchFilter(info, vctFilter, vctLevel, vctThread))
        {
            m_LogResult.push_back(i);
        }
    }

    SetListItemCount(m_LogResult.size());
}

// 添加一条日志的时候，按照过滤规则进行添加
BOOL CLogViewDlg::AppendLogWithFilter(const LogView::Util::stLogInfo& info)
{
    m_vctLogInfo.push_back(info);

    // 是否符合Fileter要求
    if(MatchFilter(info))
    {
        m_LogResult.push_back(m_vctLogInfo.size() - 1);
        SetListItemCount(m_LogResult.size()); 
    }

    return TRUE;
}

// 一条记录是否符合要求
BOOL CLogViewDlg::MatchFilter(const LogView::Util::stLogInfo& info)
{
    std::vector<LogView::Util::XString> vctFilter;
    std::vector<UINT>    vctLevel;
    ThreadOptionData    vctThread;

    m_FilterPanel.GetOptionsText(vctFilter);
    m_LevelPanel.GetOptionsParam(vctLevel);
    m_ProcPanel.GetOptionsThread(vctThread);

    return MatchFilter(info, vctFilter, vctLevel, vctThread);
}

BOOL CLogViewDlg::MatchFilter(const LogView::Util::stLogInfo& info,
                    const std::vector<LogView::Util::XString>& vctFilter,
                    const std::vector<UINT>&    vctLevel,
                    const ThreadOptionData&     vctThread)
{
    BOOL bMatch = FALSE;

    // Text Filter
    if(m_vctTextFilter.size() > 0)
    {
        bMatch = FALSE;
        LogView::Util::XString strTempLog;
        LogView::Util::XString strTempTextFilter;
        std::vector<LogView::Util::XString>::const_iterator IteTextFilter = m_vctTextFilter.begin();
        for(; IteTextFilter != m_vctTextFilter.end(); ++ IteTextFilter)
        {
            strTempLog = info.strLog;
            strTempTextFilter = *IteTextFilter;
            std::transform(strTempLog.begin(), strTempLog.end(), strTempLog.begin(), ::_totlower);
            std::transform(strTempTextFilter.begin(), strTempTextFilter.end(), strTempTextFilter.begin(), ::_totlower);
            if(strTempLog.find(strTempTextFilter) != LogView::Util::XString::npos)
            {
                bMatch = TRUE;
                break;
            }
        }
        if(!bMatch)
            return FALSE;
    }

    // Filter
    bMatch = FALSE;
    std::vector<LogView::Util::XString>::const_iterator IteFilter = vctFilter.begin();
    for(; IteFilter != vctFilter.end(); ++ IteFilter)
    {
        const LogView::Util::XString& strFilter = *IteFilter;
        if(_tcsicmp(strFilter.c_str(), info.strFilter.c_str()) == 0)
        {
            bMatch = TRUE;
            break;
        }
    }
    if(!bMatch)
        return FALSE;

    // Level
    bMatch = FALSE;
    std::vector<UINT>::const_iterator IteLevel = vctLevel.begin();
    for(; IteLevel != vctLevel.end(); ++ IteLevel)
    {
        const UINT& uLevel = *IteLevel;
        if(uLevel == info.uLevel)
        {
            bMatch = TRUE;
            break;
        }
    }
    if(!bMatch)
        return FALSE;

    // Threads
    bMatch = FALSE;
    ThreadOptionData::const_iterator IteThread = vctThread.begin();
    for(; IteThread != vctThread.end(); ++ IteThread)
    {
        const stThreadOption& option = *IteThread;
        if(option.bThreadId && option.dwId == info.dwThreadId)
        {
            bMatch = TRUE;
            break;
        }
        else if(!option.bThreadId && option.dwId == info.dwProcId)
        {
            bMatch = TRUE;
            break;
        }
    }
    if(!bMatch)
        return FALSE;

    return TRUE;
}

void CLogViewDlg::ClearLogListSelection()
{
    int nItem = -1;
    while( (nItem = m_LogList.GetNextItem(nItem, LVNI_SELECTED)) != -1)
    {
        m_LogList.SetItemState(nItem, 0, LVIS_SELECTED);
    }
}

// 调整ListView数目时调用
void CLogViewDlg::SetListItemCount(DWORD dwCount)
{
    m_LogList.SetItemCount(dwCount);
    UpdateStatusBar();
}


// 设置/获取某列Header的参数
void CLogViewDlg::SetHeaderParam(int nIndex, LPARAM lParam)
{
    CHeaderCtrl* pHeader = m_LogList.GetHeaderCtrl();
    if(pHeader != NULL)
    {
        HDITEM hdItem = {0};
        hdItem.mask = HDI_LPARAM;
        hdItem.lParam = lParam;
        pHeader->SetItem(nIndex, &hdItem);
    }
}

LPARAM CLogViewDlg::GetHeaderParam(int nIndex)
{
    CHeaderCtrl* pHeader = m_LogList.GetHeaderCtrl();
    if(pHeader != NULL)
    {
        HDITEM hdItem = {0};
        hdItem.mask = HDI_LPARAM;
        pHeader->GetItem(nIndex, &hdItem);
        return hdItem.lParam;
    }
    return 0;
}

// 根据参数，隐藏或者显示Column
void CLogViewDlg::FilterColumn(const ColumnTypeVector& vctColumn)
{
    if(vctColumn.size() == 0)
    {
        return;
    }

    CConfig::GetConfig().SetShowType(vctColumn);

    ColumnTypeVector curVctColumn;
    GetColumn(curVctColumn);

    ColumnTypeVector backupColumn = vctColumn;

    // 找到差异的项
    ColumnType type;
    ColumnTypeVector::iterator IteCurColumn;
    ColumnTypeVector::iterator IteColumn;

    ColumnTypeVector vctAdd;
    ColumnTypeVector vctDel;

    // 找到应该被删除的项目
    for(IteCurColumn=curVctColumn.begin(); IteCurColumn != curVctColumn.end(); ++ IteCurColumn)
    {
        type = *IteCurColumn;
        IteColumn = std::find(backupColumn.begin(), backupColumn.end(), type);

        if(IteColumn == backupColumn.end())
        {
            // 该项应该被删除
            vctDel.push_back(type);
        }
        else
        {
            // 该项目被保留
            backupColumn.erase(IteColumn);
        }
    }

    // 找到应该添加的项目
    for(IteColumn = backupColumn.begin(); IteColumn != backupColumn.end(); ++IteColumn)
    {
        type = *IteColumn;
        vctAdd.push_back(type);
    }
    std::sort(vctAdd.begin(), vctAdd.end());

    m_LogList.SetRedraw(FALSE);

    // 逐个删除
    for(size_t i=0; i < vctDel.size(); ++i)
    {
        type = vctDel[i];
        DelColumn(type);
    }

    size_t nHeaderCount = m_LogList.GetHeaderCtrl()->GetItemCount();
    // 逐个添加
    for(size_t i=0; i < vctAdd.size(); ++i)
    {
        type = vctAdd[i];
        AddColumn(nHeaderCount + i, type);
    }

    m_LogList.SetRedraw(TRUE);
    m_LogList.Invalidate(TRUE);
    m_LogList.UpdateWindow();
}

// 获取当前正被显示的项目
void CLogViewDlg::GetColumn(ColumnTypeVector& vctColumn)
{
    vctColumn.clear();

    ColumnType type;
    size_t nHeaderCount = m_LogList.GetHeaderCtrl()->GetItemCount();

    for(size_t i=0; i<nHeaderCount; ++ i)
    {
        type = (ColumnType)GetHeaderParam(i);
        vctColumn.push_back(type);
    }
}

// 添加一项到Column中。
void CLogViewDlg::AddColumn(int nIndex, ColumnType type)
{
    nIndex = FindIdealColumnIndex(type);
    switch(type)
    {
    case Column_Index:
        nIndex = m_LogList.InsertColumn(nIndex, _T("#"),      LVCFMT_LEFT,    30);
        SetHeaderParam(nIndex, Column_Index);
        break;

    case Column_Time:
        nIndex = m_LogList.InsertColumn(nIndex, _T("Time"),   LVCFMT_LEFT,   80);
        SetHeaderParam(nIndex, Column_Time);
        break;

    case Column_ProcId:
        nIndex = m_LogList.InsertColumn(nIndex, _T("Proc"),   LVCFMT_RIGHT,   50);
        SetHeaderParam(nIndex, Column_ProcId);
        break;

    case Column_ThreadId:
        nIndex = m_LogList.InsertColumn(nIndex, _T("Thread"), LVCFMT_RIGHT,   60);
        SetHeaderParam(nIndex, Column_ThreadId);
        break;

    case Column_Level:
        nIndex = m_LogList.InsertColumn(nIndex, _T("Level"),  LVCFMT_RIGHT,   50);
        SetHeaderParam(nIndex, Column_Level);
        break;

    case Column_Filter:
        nIndex = m_LogList.InsertColumn(nIndex, _T("Filter"), LVCFMT_LEFT,    60);
        SetHeaderParam(nIndex, Column_Filter);
        break;

    case Column_Log:
        nIndex = m_LogList.InsertColumn(5, _T("Log"),    LVCFMT_LEFT,    400);
        SetHeaderParam(nIndex, Column_Log);
        break;
    }
}

// 删除一项目
void CLogViewDlg::DelColumn(ColumnType type)
{
    ColumnType headerType;
    size_t nHeaderCount = m_LogList.GetHeaderCtrl()->GetItemCount();
    for(size_t i=0; i<nHeaderCount; ++ i)
    {
        headerType = (ColumnType)GetHeaderParam(i);
        if(headerType == type)
        {
            m_LogList.DeleteColumn(i);
            break;
        }
    }
}

// 找到合适的添加位置
int CLogViewDlg::FindIdealColumnIndex(ColumnType type)
{
    LPARAM lType = (LPARAM)type;
    LPARAM lParam = 0;
    size_t nHeaderCount = m_LogList.GetHeaderCtrl()->GetItemCount();

    size_t nIndex = 0;
    for(nIndex=0; nIndex<nHeaderCount; ++ nIndex)
    {
        lParam = GetHeaderParam(nIndex);
        if(lType < lParam)
            break;
    }
    return nIndex;
}

//////////////////////////////////////////////////////////////////////////
// 菜单处理方法

// 文件菜单
void CLogViewDlg::DoFileOpenCmd()
{
    if(m_LogList.GetItemCount() > 0)
    {
        if(AfxMessageBox(_T("There are unsaved logs.\r\nDo you want to save them?"), MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
        {
            DoFileSaveCmd();
            return;
        }
    }

    CFileDialog Dlg(TRUE);
    Dlg.m_ofn.lpstrFilter = _T("XLog\0*.xlog\0All Files\0*.*\0\0");
    if(Dlg.DoModal() == IDOK)
    {
        CString strPathName = Dlg.GetPathName();
        if(!LogView::Util::XLogMgr::OpenXLog(strPathName, &::OnLogArrived))
        {
            AfxMessageBox(_T("Failed to Load File.\r\nWrong Version of LogView?"), MB_OK | MB_ICONWARNING);
        }
    }

}

void CLogViewDlg::DoFileSaveCmd()
{
    if(m_LogList.GetItemCount() <= 0)
        return;

    CTime curTime = CTime::GetCurrentTime();
    CString strPathName;
    strPathName.Format(_T("XLog_%04d_%02d_%02d_%02d_%02d_%02d"),
        curTime.GetYear(),
        curTime.GetMonth(),
        curTime.GetDay(),
        curTime.GetHour(),
        curTime.GetMinute(),
        curTime.GetSecond());

    CFileDialog Dlg(FALSE);
    Dlg.m_ofn.lpstrFilter = _T("XLog\0*.xlog\0");
    Dlg.m_ofn.lpstrFile = strPathName.GetBuffer(MAX_PATH);
    if(Dlg.DoModal() == IDOK)
    {
        strPathName.ReleaseBuffer();
        strPathName = Dlg.GetPathName();
        if(strPathName.Find(_T(".")) == -1)
        {
            strPathName += _T(".xlog");
        }
        if(!LogView::Util::XLogMgr::SaveXLog(m_vctLogInfo, strPathName))
        {
            AfxMessageBox(_T("Failed to Save File.\r\nCheck if the file is valid?"), MB_OK | MB_ICONWARNING);
        }
    }
    strPathName.ReleaseBuffer();
}

void CLogViewDlg::DoFileExitCmd()
{
    PostQuitMessage(0);
}

// 编辑菜单
void CLogViewDlg::DoEditClearCmd()
{
    m_LogResult.clear();
    m_vctLogInfo.clear();
    SetListItemCount(0);

    m_ProcPanel.RemoveAllOptions();
    m_LevelPanel.RemoveAllOptions();

    if(CConfig::GetConfig().IsClearFilterWhenClearLog())
        m_FilterPanel.RemoveAllOptions();
}

void CLogViewDlg::DoEditCopyCmd()
{
    CString strData;
    CString strLog;

    int nItem = -1;
    while( (nItem = m_LogList.GetNextItem(nItem, LVNI_SELECTED)) != -1)
    {
        strLog      = GetItemText(nItem, Column_Log).c_str();
        strData += strLog + _T("\r\n");
    }

    if(OpenClipboard())
    {
        HANDLE hData = GlobalAlloc(GMEM_MOVEABLE, (strData.GetLength() + 1) * sizeof(TCHAR));
        if(hData != NULL)
        {
            void* pData = GlobalLock(hData);
            memcpy(pData, (LPCTSTR)strData, (strData.GetLength() + 1) * sizeof(TCHAR));
            GlobalUnlock(hData);

            SetClipboardData(CF_UNICODETEXT, hData);
            CloseClipboard();
        }
    }
}

void CLogViewDlg::DoEditCopyWholeLineCmd()
{
    CString strData;

    CString strIndex;
    CString strTime;
    CString strProc;
    CString strThread;
    CString strLevel;
    CString strFilter;
    CString strLog;

    int nItem = -1;
    while( (nItem = m_LogList.GetNextItem(nItem, LVNI_SELECTED)) != -1)
    {
        strIndex    = GetItemText(nItem, Column_Index).c_str();
        strTime     = GetItemText(nItem, Column_Time).c_str();
        strProc     = GetItemText(nItem, Column_ProcId).c_str();
        strThread   = GetItemText(nItem, Column_ThreadId).c_str();
        strLevel    = GetItemText(nItem, Column_Level).c_str();
        strFilter   = GetItemText(nItem, Column_Filter).c_str();
        strLog      = GetItemText(nItem, Column_Log).c_str();

        strData += strIndex + _T("\t");
        strData += strTime + _T("\t");
        strData += strProc + _T("\t");
        strData += strThread + _T("\t");
        strData += strLevel + _T("\t");
        strData += strFilter + _T("\t");
        strData += strLog + _T("\t");
        strData += _T("\r\n");
    }

    if(OpenClipboard())
    {
        HANDLE hData = GlobalAlloc(GMEM_MOVEABLE, (strData.GetLength() + 1) * sizeof(TCHAR));
        if(hData != NULL)
        {
            void* pData = GlobalLock(hData);
            memcpy(pData, (LPCTSTR)strData, (strData.GetLength() + 1) * sizeof(TCHAR));
            GlobalUnlock(hData);

            SetClipboardData(CF_UNICODETEXT, hData);
            CloseClipboard();
        }
    }
}

void CLogViewDlg::DoEditFindCmd()
{
    if(m_pFindDlg == NULL)
    {
        InitFindDlg();
    }
    m_pFindDlg->ShowWindow(SW_SHOW);
}

// 选项板菜单
void CLogViewDlg::DoPanelFilterCmd()
{
    DoPanelCmdHelper(&m_FilterPanel, ID_PANEL_FILTER);
}

void CLogViewDlg::DoPanelLevelCmd()
{
    DoPanelCmdHelper(&m_LevelPanel, ID_PANEL_LEVEL);
}

void CLogViewDlg::DoPanelProcessCmd()
{
    DoPanelCmdHelper(&m_ProcPanel, ID_PANEL_PROCESS);
}

void CLogViewDlg::DoPanelCmdHelper(CDialog* pPanel, WORD wCmdId, WORD wCmdId2 /* = 0 */)
{
    BOOL bPanelVisible = pPanel->IsWindowVisible();

    pPanel->ShowWindow(bPanelVisible ? SW_HIDE : SW_SHOWNOACTIVATE);

    CMenu* pPanelMenu = GetMenu()->GetSubMenu(2);
    pPanelMenu->CheckMenuItem(wCmdId, MF_BYCOMMAND | (bPanelVisible ? MF_UNCHECKED : MF_CHECKED));

    if(wCmdId2 != 0)
        pPanelMenu->CheckMenuItem(wCmdId2, MF_BYCOMMAND | (bPanelVisible ? MF_UNCHECKED : MF_CHECKED));
}


// 选项菜单
void CLogViewDlg::DoOptionTopMostCmd()
{
    BOOL bTopMost = !CConfig::GetConfig().GetTopMost();

    CMenu* pPanelMenu = GetMenu()->GetSubMenu(3);
    pPanelMenu->CheckMenuItem(ID_OPTION_TOPMOST, MF_BYCOMMAND | (bTopMost ? MF_CHECKED : MF_UNCHECKED));

    CConfig::GetConfig().SetTopMost(bTopMost);
    SetWindowPos(bTopMost ? &wndTopMost : &wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void CLogViewDlg::DoOptionAutoScrollCmd()
{
    BOOL bAutoScroll = !CConfig::GetConfig().GetAutoScroll();

    CMenu* pPanelMenu = GetMenu()->GetSubMenu(3);
    pPanelMenu->CheckMenuItem(ID_OPTION_AUTOSCROLL, MF_BYCOMMAND | (bAutoScroll ? MF_CHECKED : MF_UNCHECKED));

    CConfig::GetConfig().SetAutoScroll(bAutoScroll);
}

void CLogViewDlg::DoOptionLogOutputDebugStringCmd()
{
    CMenu* pPanelMenu = GetMenu()->GetSubMenu(3);

    BOOL bChecked = ((pPanelMenu->GetMenuState(ID_OPTION_LOGOUTPUTDEBUGSTRING, MF_BYCOMMAND) & MF_CHECKED) == MF_CHECKED);

    if(!bChecked)
    {
        if(AfxMessageBox(_T("This feature is under testing\r\nare you sure to use this?"), MB_OKCANCEL | MB_ICONWARNING) != IDOK)
            return;

        if(IsDebuggerPresent())
        {
            AfxMessageBox(_T("This Option can't take effect when LogView is debugged."), MB_OK | MB_ICONWARNING);
            bChecked = FALSE;
            return;
        }
        bChecked = StartLogOutputDebugString();
        if(!bChecked)
            AfxMessageBox(_T("Start Debug Output Log Failed\r\nCheck if there is tools of the same kind, like DebugView, etc."), MB_OK | MB_ICONWARNING);
    }
    else
    {
        bChecked = FALSE;
        StopLogOutputDebugString();
    }

    pPanelMenu->CheckMenuItem(ID_OPTION_LOGOUTPUTDEBUGSTRING, MF_BYCOMMAND | (bChecked ? MF_CHECKED : MF_UNCHECKED));
}

void CLogViewDlg::DoOptionOptions()
{
    CLogViewOptionsDlg Dlg;

    ColumnTypeVector vctColumn;
    GetColumn(vctColumn);
    Dlg.SetColumnChoice(vctColumn);

    if(Dlg.DoModal() == IDOK)
    {
        ColumnTypeVector vctColumn;
        Dlg.GetColumnChoice(vctColumn);
        FilterColumn(vctColumn);
    }
}

// 帮助菜单
void CLogViewDlg::DoHelpAbout()
{
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
}


HBRUSH CLogViewDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);

    if(nCtlColor == CTLCOLOR_EDIT)
    {
        if(m_bEditTextFilterUnApplied)
        {
            pDC->SetTextColor(COLOR_TEXT_FILTER_UN_APPLIED);
            pDC->SetBkColor(BKCOLOR_TEXT_FILTER_UN_APPLIED);
            return m_hUnAppliedBk;
        }
        else
        {
            pDC->SetTextColor(COLOR_TEXT_FILTER_APPLIED);
            pDC->SetBkColor(BKCOLOR_TEXT_FILTER_APPLIED);
        }
    }

    return hbr;
}

void CLogViewDlg::OnEnChangeEditTextFilter()
{
    m_bEditTextFilterUnApplied = TRUE;
    GetDlgItem(IDC_EDIT_TEXT_FILTER)->Invalidate(TRUE);
}

BOOL CLogViewDlg::PreTranslateMessage(MSG* pMsg)
{
    if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
        return TRUE;

    if(TranslateAccelerator(GetSafeHwnd(), m_hMainAcce, pMsg))
        return TRUE;

    return __super::PreTranslateMessage(pMsg);
}

void CLogViewDlg::OnDestroy()
{
    // TODO: Add your message handler code here and/or call default

    m_pFindDlg = NULL;
    __super::OnDestroy();
}

void CLogViewDlg::OnNMRClickListLog(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    *pResult = 0;

    POINT Pt;
    GetCursorPos(&Pt);
    CMenu menu;
    menu.LoadMenu(IDR_MAIN_MENU);
    menu.GetSubMenu(1)->TrackPopupMenu(TPM_LEFTBUTTON | TPM_RIGHTBUTTON, Pt.x, Pt.y, this);
}

void CLogViewDlg::OnNMCustomdrawListLog(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLVCUSTOMDRAW pNMCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);

    *pResult = CDRF_DODEFAULT;

    if(pNMCD->nmcd.dwDrawStage == CDDS_PREPAINT)
    {
        *pResult = CDRF_NOTIFYITEMDRAW;
    }
    else if(pNMCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
    {
        if(pNMCD->nmcd.dwItemSpec == m_LogList.GetNextItem(pNMCD->nmcd.dwItemSpec - 1, LVNI_SELECTED))
        {
            pNMCD->clrTextBk = RGB(0x88, 0x88, 0x00);
            pNMCD->clrText = RGB(0xFF, 0xFF, 0xFF);
            pNMCD->nmcd.uItemState &= (~CDIS_FOCUS);
            pNMCD->nmcd.uItemState &= (~CDIS_SELECTED);
        }
        else
        {
            if(pNMCD->nmcd.dwItemSpec % 2 == 1)
            {
                pNMCD->clrTextBk = RGB(0xE8, 0xE8, 0xE8);
            }
            else
            {
                pNMCD->clrTextBk = RGB(0xFF, 0xFF, 0xFF);
            }
        }
    }
}
