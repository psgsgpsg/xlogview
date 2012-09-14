#include "StdAfx.h"
#include "AboutDlg.h"


CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
    m_linkEmail.SetLink(_T("mailto:huhangscu@gmail.com"));
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LINK_EMAIL, m_linkEmail);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()
