
// LogViewDlg.h : ͷ�ļ�
//

#pragma once

#include "../Common/LogDef.h"
#include "afxcmn.h"

#include "ProcessPanel.h"
#include "OptionSelectPanel.h"

#include <vector>

struct stLogUIData
{
    DWORD dwThreadId;
    DWORD dwProcId;
    SYSTEMTIME timeLog;
    CString strLevel;
    CString strFilter;
    CString strLog;
};
typedef std::vector<stLogUIData> LogUIDataVector;

__inline void GetUIData(stLogUIData& uidata, const stLogData& data)
{
    uidata.dwThreadId = data.dwThreadId;
    uidata.dwProcId = data.dwProcId;
    uidata.timeLog = data.timeLog;
    uidata.strLevel = data.szLevel;
    uidata.strFilter = data.szFilter;
    uidata.strLog = data.szLog;
}

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
    virtual void OnLogArrived(const stLogData& info);

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
    CString GetItemText(int nItem, int nSubItem);
    CString GetItemText(int nItem, ColumnType type);

    // ִ�в˵���������ڲ��Ƿ��Ѵ���
    BOOL    DoCmdId(WORD wId);

    // ÿ�η����仯��ʱ�򣬰����û�ѡ���Filter����־���й���
    void    DoFilter();

    // ���һ����־��ʱ�򣬰��չ��˹���������
    BOOL    AppendLogWithFilter(const stLogUIData& info);

    // һ����¼�Ƿ����Ҫ��
    BOOL    MatchFilter(const stLogUIData& info);
    BOOL    MatchFilter(const stLogUIData& info,
                            const std::vector<CString>& vctFilter,
                            const std::vector<CString>&    vctLevel,
                            const ThreadOptionData&     vctThread
                            );

    BOOL    SubMatchTextFilter(const stLogUIData& info);

    // ���ListView��ѡ����
    void    ClearLogListSelection();

    // ����ListView��Ŀʱ����
    void    UpdateItemCount();
    void    DelayUpdateItemCount();

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
    LogUIDataVector m_vctLogInfo;

    typedef std::vector<size_t> LogInfoResultIndex;
    LogInfoResultIndex  m_LogResult;

    std::vector<CString>    m_vctTextFilter;

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

    UINT_PTR        m_uDelayUpdateItemCountTimerId;

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

    // Panel�Ļص���Ϣ
    afx_msg LRESULT OnFilterPanelMsg(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnLevelPanelMsg(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnProcPanelMsg(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnProcHiddenMsg(WPARAM wParam, LPARAM lParam);

    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnDestroy();
    afx_msg void OnTimer(UINT_PTR nIDEvent);

    afx_msg void OnBnClickedBtnDoFilter();
    afx_msg void OnEnChangeEditTextFilter();

    // �����滻�Ի�����Ϣ��Ӧ
    afx_msg LRESULT OnFindReplaceMsg(WPARAM wParam, LPARAM lParam);
protected:
    // �˵�������

    // �ļ��˵�
    afx_msg void DoFileOpenCmd();
    afx_msg void DoFileSaveCmd();
    afx_msg void DoFileExitCmd();

    // �༭�˵�
    afx_msg void DoEditClearCmd();
    afx_msg void DoEditCopyCmd();
    afx_msg void DoEditCopyWholeLineCmd();
    afx_msg void DoEditFindCmd();

    // ѡ���˵�
    afx_msg void DoPanelFilterCmd();
    afx_msg void DoPanelLevelCmd();
    afx_msg void DoPanelProcessCmd();

    void DoPanelCmdHelper(CDialog* pPanel, WORD wCmdId, WORD wCmdId2 = 0);

    // ѡ��˵�
    afx_msg void DoOptionTopMostCmd();
    afx_msg void DoOptionAutoScrollCmd();
    afx_msg void DoEnableRegexCmd();
    afx_msg void DoEnableWildcardCmd();
    afx_msg void DoOptionOptions();

    // �����˵�
    afx_msg void DoHelpAbout();

public:
    afx_msg void OnNMRClickListLog(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMCustomdrawListLog(NMHDR *pNMHDR, LRESULT *pResult);
};
