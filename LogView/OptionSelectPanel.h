#pragma once


// COptionSelectPanel dialog
#include "UIDefine.h"

class COptionSelectPanel : public CDialog
{
	DECLARE_DYNAMIC(COptionSelectPanel)

public:
	COptionSelectPanel(CWnd* pParent = NULL);   // standard constructor
	virtual ~COptionSelectPanel();

public:
    void SetPanelType(PanelType eType);
    void AddOption(LPCTSTR szText, DWORD dwParam);
    void RemoveAllOptions();

    void GetOptionsText(std::vector<CString>& vctOptions);

    void GetTextSize(LPCTSTR szText, SIZE& sizeText);

    template<typename T>
    void GetOptionsParam(std::vector<T>& vctOptions)
    {
        vctOptions.clear();

        UINT uCheck = 0;
        HWND hWndChild = NULL;
        hWndChild = ::GetWindow(GetSafeHwnd(), GW_CHILD);
        while( hWndChild != NULL)
        {
            uCheck = ::SendMessage(hWndChild, BM_GETCHECK, 0, 0);
            if(uCheck == BST_CHECKED)
                vctOptions.push_back((T)GetWindowLongPtr(hWndChild, GWLP_USERDATA));
            hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT);
        }
    }

// Dialog Data
	enum { IDD = IDD_PANEL_SELECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

    afx_msg void OnCheckOptions(UINT uOptionId);

protected:
    PanelType   m_ePanelType;
public:
    afx_msg void OnClose();
};
