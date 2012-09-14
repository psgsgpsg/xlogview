// LogViewOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LogView.h"
#include "LogViewOptionsDlg.h"

#include "Config.h"

// CLogViewOptionsDlg dialog

IMPLEMENT_DYNAMIC(CLogViewOptionsDlg, CDialog)

CLogViewOptionsDlg::CLogViewOptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLogViewOptionsDlg::IDD, pParent)
    , m_nMaxLogCount(0)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CLogViewOptionsDlg::~CLogViewOptionsDlg()
{
}

void CLogViewOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_MAX_LOG_COUNT, m_nMaxLogCount);
	DDV_MinMaxInt(pDX, m_nMaxLogCount, 10, 100000);
}


BEGIN_MESSAGE_MAP(CLogViewOptionsDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CLogViewOptionsDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CLogViewOptionsDlg message handlers

struct stIdAndType
{
    UINT        uChkBtnId;
    ColumnType  type;
};
stIdAndType g_arrIdAndType[] =
{
    IDC_CHKBTN_INDEX,       Column_Index,
    IDC_CHKBTN_TIME,        Column_Time,
    IDC_CHKBTN_PROCID,      Column_ProcId,
    IDC_CHKBTN_THREADID,    Column_ThreadId,
    IDC_CHKBTN_LEVEL,       Column_Level,
    IDC_CHKBTN_FILTER,      Column_Filter,
    IDC_CHKBTN_LOG,         Column_Log,
};
const size_t g_uIdAndTypeCount = _countof(g_arrIdAndType);

void CLogViewOptionsDlg::SetColumnChoice(const ColumnTypeVector& vctColumn)
{
    m_vctColumn = vctColumn;
}

void CLogViewOptionsDlg::GetColumnChoice(ColumnTypeVector& vctColumn)
{
    vctColumn = m_vctColumn;
}

BOOL CLogViewOptionsDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon(m_hIcon, TRUE);			// 设置大图标
    SetIcon(m_hIcon, FALSE);		// 设置小图标

    // TODO: 在此添加额外的初始化代码

    InitChkBtnStatus();

    if(CConfig::GetConfig().IsClearMaxCountReached())
        CheckDlgButton(IDC_RADIO_CLEAR, BST_CHECKED);
    else
        CheckDlgButton(IDC_RADIO_STOP, BST_CHECKED);

    BOOL bClearFilter = CConfig::GetConfig().IsClearFilterWhenClearLog();
    CheckDlgButton(IDC_CHKBTN_CLEAR_FILTER, bClearFilter ? BST_CHECKED : BST_UNCHECKED);

    m_nMaxLogCount = CConfig::GetConfig().GetMaxLogCount();
    UpdateData(FALSE);

    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

ColumnType CLogViewOptionsDlg::IdToType(UINT uChkBtnId)
{
    ColumnType type = Column_Unknown;
    for(size_t i=0; i<g_uIdAndTypeCount; ++ i)
    {
        if(g_arrIdAndType[i].uChkBtnId == uChkBtnId)
        {
            type = g_arrIdAndType[i].type;
            break;
        }
    }
    return type;
}

UINT CLogViewOptionsDlg::TypeToId(ColumnType type)
{
    UINT uChkBtnId = 0;
    for(size_t i=0; i<g_uIdAndTypeCount; ++ i)
    {
        if(g_arrIdAndType[i].type == type)
        {
            uChkBtnId = g_arrIdAndType[i].uChkBtnId;
            break;
        }
    }
    return uChkBtnId;
}

void CLogViewOptionsDlg::InitChkBtnStatus()
{
    ColumnType type;
    UINT uChkBtnId;
    ColumnTypeVector::iterator IteColumn = m_vctColumn.begin();
    for(; IteColumn != m_vctColumn.end(); ++ IteColumn)
    {
        type = *IteColumn;
        uChkBtnId = TypeToId(type);
        if(uChkBtnId != 0)
        {
            ::CheckDlgButton(m_hWnd, uChkBtnId, BST_CHECKED);
        }
    }
}

void CLogViewOptionsDlg::SaveChkBtnStatus()
{
    m_vctColumn.clear();

    ColumnType type;
    for(size_t i=0; i<g_uIdAndTypeCount; ++ i)
    {
        if(::IsDlgButtonChecked(m_hWnd, g_arrIdAndType[i].uChkBtnId) == BST_CHECKED)
        {
            type = IdToType(g_arrIdAndType[i].uChkBtnId);
            if(type != Column_Unknown)
                m_vctColumn.push_back(type);
        }
    }
}

void CLogViewOptionsDlg::OnBnClickedOk()
{
    SaveChkBtnStatus();

    if(m_vctColumn.size() <= 0)
    {
        AfxMessageBox(_T("There should be one choice at lease."), MB_OK | MB_ICONWARNING);
        return;
    }

    UpdateData(TRUE);
    CConfig::GetConfig().SetMaxLogCount(m_nMaxLogCount);

    BOOL bClear = IsDlgButtonChecked(IDC_RADIO_CLEAR);
    CConfig::GetConfig().SetClearMaxCountReached(bClear);

    BOOL bClearFilter = IsDlgButtonChecked(IDC_CHKBTN_CLEAR_FILTER);
    CConfig::GetConfig().SetClearFilterWhenClearLog(bClearFilter);

    OnOK();
}
