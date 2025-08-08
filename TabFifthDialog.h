#pragma once
#include "afxwin.h"
#include "afxcmn.h"


#include "TE_LIST.h"//형태소 분석 후(중의성분석포함) 리스트 정보 분류별 파싱

// CTabFifthDialog 대화 상자입니다.

class CTabFifthDialog : public CDialog
{
	DECLARE_DYNAMIC(CTabFifthDialog)

public:
	CTabFifthDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTabFifthDialog();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAB_5_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCbnSelchangeStatisticsCombo();
	afx_msg void OnCbnSelchangeViewCombo();
public:
	virtual BOOL OnInitDialog();

public:
	TE_DATA *m_pte_data;
	void f_print_statistic(int view_statistic);//통계
	void f_print_out(int view_out);//보기

public:
	CEdit m_StatisticsData;
	CListCtrl m_StatisticsDataList;
	CComboBox m_StatisticsCombo;
	CComboBox m_ViewCombo;
	CEdit m_FindEdit;
	
	
	
	
};
