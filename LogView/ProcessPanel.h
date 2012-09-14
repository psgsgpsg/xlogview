#pragma once
#include "afxcmn.h"


// CProcessPanel dialog
struct stThreadOption
{
    DWORD dwId;
    BOOL  bThreadId;
};
typedef std::vector<stThreadOption>  ThreadOptionData;

class CProcessPanel : public CDialog
{
	DECLARE_DYNAMIC(CProcessPanel)

public:
	CProcessPanel(CWnd* pParent = NULL);   // standard constructor
    virtual ~CProcessPanel();

public:
    void AddOption(DWORD dwThreadId, DWORD dwProcId, DWORD dwParam);
    void RemoveAllOptions();

    void GetOptionsThread(ThreadOptionData& vctOptions);

    LogView::Util::XString GetThreadText(DWORD dwThreadId);
    LogView::Util::XString GetProcText(DWORD dwProcId);

    LogView::Util::XString PidToName(DWORD dwProcId);

    void    SyncSubTreeState(HTREEITEM hItem, BOOL bChecked);

// Dialog Data
	enum { IDD = IDD_PANEL_PROCESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
    CTreeCtrl m_ThreadTree;
    afx_msg void OnNMClickTreeThread(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    virtual BOOL OnInitDialog();
    afx_msg void OnClose();
};
