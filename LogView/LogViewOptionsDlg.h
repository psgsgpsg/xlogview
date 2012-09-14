#pragma once

#include "UIDefine.h"

// CLogViewOptionsDlg dialog

class CLogViewOptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(CLogViewOptionsDlg)

public:
	CLogViewOptionsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLogViewOptionsDlg();

    void    SetColumnChoice(const ColumnTypeVector& vctColumn);
    void    GetColumnChoice(ColumnTypeVector& vctColumn);

// Dialog Data

    enum { IDD = IDD_LOGVIEW_OPTIONS };

protected:
    HICON            m_hIcon;
    ColumnTypeVector m_vctColumn;
    int              m_nMaxLogCount;

    ColumnType  IdToType(UINT uChkBtnId);
    UINT        TypeToId(ColumnType type);

    void        InitChkBtnStatus();
    void        SaveChkBtnStatus();

protected:
    virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnBnClickedOk();
};
