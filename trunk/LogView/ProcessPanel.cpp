// ProcessPanel.cpp : implementation file
//

#include "stdafx.h"
#include "LogView.h"
#include "ProcessPanel.h"
#include <Tlhelp32.h>

#include "UIDefine.h"

// CProcessPanel dialog

IMPLEMENT_DYNAMIC(CProcessPanel, CDialog)

CProcessPanel::CProcessPanel(CWnd* pParent /*=NULL*/)
	: CDialog(CProcessPanel::IDD, pParent)
{

}

CProcessPanel::~CProcessPanel()
{
}

void CProcessPanel::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TREE_THREAD, m_ThreadTree);
}


BEGIN_MESSAGE_MAP(CProcessPanel, CDialog)
    ON_NOTIFY(NM_CLICK, IDC_TREE_THREAD, &CProcessPanel::OnNMClickTreeThread)
    ON_WM_SIZE()
    ON_WM_CLOSE()
END_MESSAGE_MAP()


// CProcessPanel message handlers


//////////////////////////////////////////////////////////////////////////

void CProcessPanel::AddOption(DWORD dwThreadId, DWORD dwProcId, DWORD dwParam)
{
    HTREEITEM hNextItem = m_ThreadTree.GetRootItem();
    HTREEITEM hProcItem = hNextItem;

    if(hNextItem != NULL)
    {
        DWORD dwTreeProdId = m_ThreadTree.GetItemData(hNextItem);

        while(dwProcId != dwTreeProdId && hNextItem != NULL)
        {
            hProcItem = hNextItem;
            hNextItem = m_ThreadTree.GetNextSiblingItem(hProcItem);
            if(hProcItem != NULL)
                dwTreeProdId = m_ThreadTree.GetItemData(hProcItem);
        }
        hProcItem = (dwProcId == dwTreeProdId) ? hProcItem : NULL;
    }
    if(hProcItem == NULL)
    {
        // 该进程还不存在
        hProcItem = m_ThreadTree.InsertItem(GetProcText(dwProcId).c_str(), NULL, TVI_LAST);
        m_ThreadTree.SetItemData(hProcItem, dwProcId);
        m_ThreadTree.SetCheck(hProcItem, TRUE);

        HTREEITEM hThreadItem = m_ThreadTree.InsertItem(GetThreadText(dwThreadId).c_str(), hProcItem, TVI_LAST);
        m_ThreadTree.SetItemData(hThreadItem, dwThreadId);
        m_ThreadTree.SetCheck(hThreadItem, TRUE);
    }
    else
    {
        HTREEITEM hThreadItem = m_ThreadTree.GetChildItem(hProcItem);
        DWORD dwTreeThreadId = m_ThreadTree.GetItemData(hThreadItem);

        while (hThreadItem != NULL && dwTreeThreadId != dwThreadId)
        {
            hThreadItem = m_ThreadTree.GetNextItem(hThreadItem, TVGN_NEXT);
            if(hThreadItem == NULL)
                break;
            dwTreeThreadId = m_ThreadTree.GetItemData(hThreadItem);
        }

        if(dwTreeThreadId != dwThreadId)
        {
            hThreadItem = m_ThreadTree.InsertItem(GetThreadText(dwThreadId).c_str(), hProcItem, TVI_LAST);
            m_ThreadTree.SetItemData(hThreadItem, dwThreadId);
            m_ThreadTree.SetCheck(hThreadItem, TRUE);
        }
    }
}

void CProcessPanel::RemoveAllOptions()
{
    m_ThreadTree.DeleteAllItems();
}

void CProcessPanel::GetOptionsThread(ThreadOptionData& vctOptions)
{
    // 该Vector里只保存线程Id
    vctOptions.clear();

    DWORD dwProcId = 0;
    DWORD dwThreadId = 0;
    stThreadOption option;

    HTREEITEM hThreadItem = NULL;
    HTREEITEM hProcItem = m_ThreadTree.GetNextItem(TVI_ROOT, TVGN_CHILD);

    while(hProcItem != NULL)
    {
        dwProcId = m_ThreadTree.GetItemData(hProcItem);
        hThreadItem = m_ThreadTree.GetNextItem(hProcItem, TVGN_CHILD);
        while(hThreadItem != NULL)
        {
            dwThreadId = m_ThreadTree.GetItemData(hThreadItem);
            if(dwThreadId == 0 && m_ThreadTree.GetCheck(hProcItem))
            {
                option.dwId = dwProcId;
                option.bThreadId = FALSE;
                vctOptions.push_back(option);
            }
            else if(dwThreadId != 0 && m_ThreadTree.GetCheck(hThreadItem))
            {
                option.dwId = dwThreadId;
                option.bThreadId = TRUE;
                vctOptions.push_back(option);
            }
            hThreadItem = m_ThreadTree.GetNextItem(hThreadItem, TVGN_NEXT);
        }

        hProcItem = m_ThreadTree.GetNextItem(hProcItem, TVGN_NEXT);
    }
}

LogView::Util::XString CProcessPanel::GetThreadText(DWORD dwThreadId)
{
    LogView::Util::XString strThread = LogView::Util::XString(_T("%1")).arg(dwThreadId);
    return strThread;
}

LogView::Util::XString CProcessPanel::GetProcText(DWORD dwProcId)
{
    LogView::Util::XString strProcName = PidToName(dwProcId);
    LogView::Util::XString strProc;
    strProc.Format(_T("%s(%d)"), strProcName.c_str(), dwProcId);
    return strProc;
}

LogView::Util::XString CProcessPanel::PidToName(DWORD dwProcId)
{
    LogView::Util::XString strResult;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(hSnap == INVALID_HANDLE_VALUE)
        return strResult;

    PROCESSENTRY32 proc = {0};
    proc.dwSize = sizeof(proc);
    do 
    {
        if(!Process32First(hSnap, &proc))
            break;

        do 
        {
            if(proc.th32ProcessID == dwProcId)
            {
                strResult = proc.szExeFile;
                break;
            }
        } while (Process32Next(hSnap, &proc));

    } while (FALSE);
    CloseHandle(hSnap);
    return strResult;
}

void CProcessPanel::OnNMClickTreeThread(NMHDR *pNMHDR, LRESULT *pResult)
{
    *pResult = 0;

    POINT PtCursor;
    UINT uFlags = 0;

    GetCursorPos(&PtCursor);
    m_ThreadTree.ScreenToClient(&PtCursor);
    HTREEITEM hItem = m_ThreadTree.HitTest(PtCursor, &uFlags);
    if(hItem != NULL && ((uFlags & TVHT_ONITEMSTATEICON) == TVHT_ONITEMSTATEICON))
    {
        BOOL bChecked = !m_ThreadTree.GetCheck(hItem);
        SyncSubTreeState(hItem, bChecked);

        DWORD dwProcId = 0;
        DWORD dwThreadId = 0;
        HTREEITEM hProcItem = m_ThreadTree.GetParentItem(hItem);
        if(hProcItem == NULL)
        {
            dwProcId = m_ThreadTree.GetItemData(hItem);
            dwThreadId = 0;
        }
        else
        {
            dwProcId = m_ThreadTree.GetItemData(hProcItem);
            dwThreadId = m_ThreadTree.GetItemData(hItem);
        }

        HWND hWndParent = ::GetParent(GetSafeHwnd());
        ::PostMessage(hWndParent,
            g_uProcePanelCallbackMsg,
            dwProcId,
            dwThreadId);
    }
}

void CProcessPanel::SyncSubTreeState(HTREEITEM hItem, BOOL bChecked)
{
    if(m_ThreadTree.GetParentItem(hItem) == NULL)
    {
        // 为进程节点
        BOOL bThreadItemChecked = TRUE;
        HTREEITEM hThreadItem = m_ThreadTree.GetNextItem(hItem, TVGN_CHILD);

        while(hThreadItem)
        {
            m_ThreadTree.SetCheck(hThreadItem, bChecked);
            hThreadItem = m_ThreadTree.GetNextItem(hThreadItem, TVGN_NEXT);
        }
    }
    else
    {
        // 线程节点
        HTREEITEM hProcItem = m_ThreadTree.GetParentItem(hItem);

        BOOL bAllChecked = TRUE;
        BOOL bItemChecked = FALSE;
        HTREEITEM hThreadItem = m_ThreadTree.GetNextItem(hProcItem, TVGN_CHILD);
        while(hThreadItem)
        {
            if(hItem == hThreadItem)
            {
                bItemChecked = bChecked;
            }
            else
            {
                bItemChecked = m_ThreadTree.GetCheck(hThreadItem);
            }
            if(!bItemChecked)
            {
                bAllChecked = FALSE;
                break;
            }
            hThreadItem = m_ThreadTree.GetNextItem(hThreadItem, TVGN_NEXT);
        }
        m_ThreadTree.SetCheck(hProcItem, bAllChecked);
    }
}

void CProcessPanel::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    if(m_ThreadTree.GetSafeHwnd() != NULL)
    {
        m_ThreadTree.MoveWindow(0, 0, cx - 0, cy - 0);
    }
}

BOOL CProcessPanel::OnInitDialog()
{
    CDialog::OnInitDialog();

    SetWindowPos(NULL, 0, 0, 179, 182, SWP_NOZORDER | SWP_NOMOVE);

    // 解决本窗口第一次可见时，Tree里的Item逻辑上已经被Checked，但是显示时却没有Checked的问题
    m_ThreadTree.ModifyStyle(TVS_CHECKBOXES, 0);
    m_ThreadTree.ModifyStyle(0, TVS_CHECKBOXES);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CProcessPanel::OnClose()
{
    HWND hWndParent = ::GetParent(GetSafeHwnd());
    ::PostMessage(hWndParent,
        g_uPanelHiddenMsg,
        PANEL_PROCESS,
        0);

    CDialog::OnClose();
}
