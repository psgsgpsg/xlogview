
// LogViewDlg.h : ͷ�ļ�
//

#pragma once

#include "../Common/LogDef.h"
#include "afxcmn.h"

#include "ProcessPanel.h"
#include "OptionSelectPanel.h"

// CLogViewDlg �Ի���
class CLogViewDlg : public CDialog, public ILogListener
{
// ����
public:
	CLogViewDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_LOGVIEW_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


public:
    // ILogListener
    virtual void Release();
    virtual void OnLogArrived(const LogView::Util::stLogInfo& info);

protected:
// �ڲ�����

    // ��ʼ����־List
    void    InitLogList();

    // ��ʼ��״̬��
    void    InitStatusBar();
    void    UpdateStatusBar();

    // ��ʼ��С���
    void    InitPanel();

    // ��ʼ�����ҶԻ���
    void    InitFindDlg();

    // ��ȡĳSubItem������
    LogView::Util::XString GetItemText(int nItem, int nSubItem);
    LogView::Util::XString GetItemText(int nItem, ColumnType type);

    // ִ�в˵���������ڲ��Ƿ��Ѵ���
    BOOL    DoCmdId(WORD wId);

    // ÿ�η����仯��ʱ�򣬰����û�ѡ���Filter����־���й���
    void    DoFilter();

    // ���һ����־��ʱ�򣬰��չ��˹���������
    BOOL    AppendLogWithFilter(const LogView::Util::stLogInfo& info);

    // һ����¼�Ƿ����Ҫ��
    BOOL    MatchFilter(const LogView::Util::stLogInfo& info);
    BOOL    MatchFilter(const LogView::Util::stLogInfo& info,
                            const std::vector<LogView::Util::XString>& vctFilter,
                            const std::vector<UINT>&    vctLevel,
                            const ThreadOptionData&     vctThread
                            );

    // ���ListView��ѡ����
    void    ClearLogListSelection();

    // ����ListView��Ŀʱ����
    void    SetListItemCount(DWORD dwCount);

    // ����/��ȡĳ��Header�Ĳ���
    void    SetHeaderParam(int nIndex, LPARAM lParam);
    LPARAM  GetHeaderParam(int nIndex);

    // ���ݲ��������ػ�����ʾColumn
    void    FilterColumn(const ColumnTypeVector& vctColumn);
    // ��ȡ��ǰ������ʾ����Ŀ
    void    GetColumn(ColumnTypeVector& vctColumn);
    // ���һ�Column�С�
    void    AddColumn(int nIndex, ColumnType type);
    // ɾ��һ��Ŀ
    void    DelColumn(ColumnType type);
    // �ҵ����ʵ����λ��
    int     FindIdealColumnIndex(ColumnType type);

// ʵ��
protected:
    HICON           m_hIcon;
    HACCEL          m_hMainAcce;

    CListCtrl       m_LogList;
    LogView::Util::LogInfoVector   m_vctLogInfo;

    LogView::Util::LogInfoResultIndex  m_LogResult;

    std::vector<LogView::Util::XString>    m_vctTextFilter;

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
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

    // Panel�Ļص���Ϣ
    afx_msg LRESULT OnFilterPanelMsg(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnLevelPanelMsg(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnProcPanelMsg(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnProcHiddenMsg(WPARAM wParam, LPARAM lParam);

    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnDestroy();

    afx_msg void OnBnClickedBtnDoFilter();
    afx_msg void OnEnChangeEditTextFilter();

    // �����滻�Ի�����Ϣ��Ӧ
    afx_msg LRESULT OnFindReplaceMsg(WPARAM wParam, LPARAM lParam);
protected:
    // �˵�������

    // �ļ��˵�
    void DoFileOpenCmd();
    void DoFileSaveCmd();
    void DoFileExitCmd();

    // �༭�˵�
    void DoEditClearCmd();
    void DoEditCopyCmd();
    void DoEditCopyWholeLineCmd();
    void DoEditFindCmd();

    // ѡ���˵�
    void DoPanelFilterCmd();
    void DoPanelLevelCmd();
    void DoPanelProcessCmd();

    void DoPanelCmdHelper(CDialog* pPanel, WORD wCmdId, WORD wCmdId2 = 0);

    // ѡ��˵�
    void DoOptionTopMostCmd();
    void DoOptionAutoScrollCmd();
    void DoOptionLogOutputDebugStringCmd();
    void DoOptionOptions();

    // �����˵�
    void DoHelpAbout();
public:
    afx_msg void OnNMRClickListLog(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMCustomdrawListLog(NMHDR *pNMHDR, LRESULT *pResult);
};
