#include "StdAfx.h"
#include "AboutDlg.h"


BOOL CAboutDlg::ms_bAboutVisible = FALSE;

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

void CAboutDlg::ShowAbout()
{
    if(!ms_bAboutVisible)
    {
        ms_bAboutVisible = TRUE;
        CAboutDlg dlg;
        dlg.DoModal();
        ms_bAboutVisible = FALSE;
    }
}

