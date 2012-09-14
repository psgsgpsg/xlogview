// OptionSelectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LogView.h"
#include "OptionSelectPanel.h"

#include "UIDefine.h"

#pragma warning(disable: 4018)

// COptionSelectPanel dialog

IMPLEMENT_DYNAMIC(COptionSelectPanel, CDialog)

COptionSelectPanel::COptionSelectPanel(CWnd* pParent /*=NULL*/)
	: CDialog(COptionSelectPanel::IDD, pParent)
{
    m_ePanelType = PANEL_UNKNOWN;
}

COptionSelectPanel::~COptionSelectPanel()
{
}

void COptionSelectPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COptionSelectPanel, CDialog)
    ON_CONTROL_RANGE(BN_CLICKED, IDC_OPTION_CHKBTN_START_ID, IDC_OPTION_CHKBTN_END_ID, &COptionSelectPanel::OnCheckOptions)
    ON_WM_CLOSE()
END_MESSAGE_MAP()


// COptionSelectPanel message handlers


//////////////////////////////////////////////////////////////////////////

void COptionSelectPanel::SetPanelType(PanelType eType)
{
    m_ePanelType = eType;
}

void COptionSelectPanel::AddOption(LPCTSTR szText, DWORD dwParam)
{
    // 检查选项是否已经存在
    TCHAR szChildText[MAX_PATH];
    HWND hWndChild = NULL;
    size_t nChildCount = 0;
    hWndChild = ::GetWindow(GetSafeHwnd(), GW_CHILD);
    while( hWndChild != NULL)
    {
        ++ nChildCount;
        ::GetWindowText(hWndChild, szChildText, MAX_PATH);
        if(_tcsicmp(szChildText, szText) == 0)
            return;
        hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT);
    }

    const int nLeftMargin = 10;
    const int nRightMargin = 10;

    const int nTopMost = 10;
    const int nButtonHeight = 20;

    SIZE SizeText;
    GetTextSize(szText, SizeText);

    // 添加控件
    HWND hWnd = ::CreateWindow(
        _T("Button"), szText,
        BS_AUTOCHECKBOX | WS_CHILD | WS_VISIBLE,
        nLeftMargin, nTopMost + nChildCount * nButtonHeight,
        SizeText.cx + 20, nButtonHeight,
        GetSafeHwnd(),
        (HMENU)(IDC_OPTION_CHKBTN_START_ID + nChildCount),
        NULL, 0);
    ::SendMessage(hWnd, WM_SETFONT, (WPARAM)::SendMessage(GetSafeHwnd(), WM_GETFONT, 0, 0), TRUE);
    SetWindowLongPtr(hWnd, GWLP_USERDATA, dwParam);
    ::SendMessage(hWnd, BM_SETCHECK, BST_CHECKED, 0);

    // Sync Size
    ++ nChildCount;

    RECT rcClient;
    GetClientRect(&rcClient);
    int nWidth = rcClient.right - rcClient.left;
    int nHeight = rcClient.bottom - rcClient.top;
    int nTotalButtonHeight = nTopMost + nChildCount * nButtonHeight;

    int nHorzDiff = 0;
    int nVertDiff = 0;
    if(nTotalButtonHeight > nHeight)
    {
        nVertDiff = nTotalButtonHeight - nHeight;
    }
    if(SizeText.cx + nRightMargin + nLeftMargin > nWidth)
    {
        nHorzDiff = SizeText.cx + nRightMargin + nLeftMargin - nWidth;
    }

    if(nVertDiff != 0 || nHorzDiff != 0)
    {
        GetWindowRect(&rcClient);
        SetWindowPos(NULL,
            0, 0,
            rcClient.right - rcClient.left + nHorzDiff, rcClient.bottom - rcClient.top + nVertDiff,
            SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }
}

void COptionSelectPanel::RemoveAllOptions()
{
    // 清空控件
    HWND hWndChild = NULL;
    while( (hWndChild = ::GetWindow(GetSafeHwnd(), GW_CHILD)) != NULL)
    {
        ::DestroyWindow(hWndChild);
    }
}

void COptionSelectPanel::GetOptionsText(std::vector<LogView::Util::XString>& vctOptions)
{
    vctOptions.clear();

    UINT uCheck = 0;
    HWND hWndChild = NULL;
    TCHAR szBuffer[MAX_PATH];
    hWndChild = ::GetWindow(GetSafeHwnd(), GW_CHILD);
    while( hWndChild != NULL)
    {
        uCheck = ::SendMessage(hWndChild, BM_GETCHECK, 0, 0);
        if(uCheck == BST_CHECKED)
        {
            ::GetWindowText(hWndChild, szBuffer, MAX_PATH);
            vctOptions.push_back(szBuffer);
        }
        hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT);
    }
}

void COptionSelectPanel::GetTextSize(LPCTSTR szText, SIZE& sizeText)
{
    CDC* pDC = GetDC();

    GetTextExtentPoint32(pDC->GetSafeHdc(), szText, _tcslen(szText), &sizeText);

    ReleaseDC(pDC);
}

void COptionSelectPanel::OnCheckOptions(UINT uOptionId)
{
    UINT uMsg = 0;
    switch(m_ePanelType)
    {
    case PANEL_FILTER:
        {
            uMsg = g_uFilterPanelCallbackMsg;
            break;
        }
    case PANEL_LEVEL:
        {
            uMsg = g_uLevelPanelCallbackMsg;
            break;
        }
    }

    if(uMsg != 0)
    {
        HWND hWnd = ::GetDlgItem(GetSafeHwnd(), uOptionId);
        if(hWnd != NULL)
        {
            HWND hWndParent = ::GetParent(GetSafeHwnd());
            ::PostMessage(hWndParent,
                uMsg,
                uOptionId - IDC_OPTION_CHKBTN_START_ID,
                GetWindowLongPtr(hWnd, GWLP_USERDATA));
        }
    }
}

void COptionSelectPanel::OnClose()
{
    HWND hWndParent = ::GetParent(GetSafeHwnd());
    ::PostMessage(hWndParent,
        g_uPanelHiddenMsg,
        m_ePanelType,
        0);

    CDialog::OnClose();
}