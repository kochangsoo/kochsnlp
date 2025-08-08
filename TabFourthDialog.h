#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CTabFourthDialog ��ȭ �����Դϴ�.

class CTabFourthDialog : public CDialog
{
	DECLARE_DYNAMIC(CTabFourthDialog)

public:
	CTabFourthDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CTabFourthDialog();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAB_4_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_SummaryData;
	CListCtrl m_AnalysisDataList;
	CStatic m_AnalysisTime;
	CEdit m_ResultSearchWord;
	//CBrush m_backBrush;


public:
	virtual BOOL OnInitDialog();
	afx_msg void OnNMClickAnalysisList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedAnalysisSaveButton();

public:
	int GetCountChar(CString Ps_Str, TCHAR Pc_Char);
	CString* Split(CString Ps_Str, TCHAR Pc_Separator, int& Ri_Length);
};
