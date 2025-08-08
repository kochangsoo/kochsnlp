#pragma once
#include "afxwin.h"
#include "afxcmn.h"


#include "TE_LIST.h"//���¼� �м� ��(���Ǽ��м�����) ����Ʈ ���� �з��� �Ľ�

// CTabFifthDialog ��ȭ �����Դϴ�.

class CTabFifthDialog : public CDialog
{
	DECLARE_DYNAMIC(CTabFifthDialog)

public:
	CTabFifthDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CTabFifthDialog();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAB_5_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCbnSelchangeStatisticsCombo();
	afx_msg void OnCbnSelchangeViewCombo();
public:
	virtual BOOL OnInitDialog();

public:
	TE_DATA *m_pte_data;
	void f_print_statistic(int view_statistic);//���
	void f_print_out(int view_out);//����

public:
	CEdit m_StatisticsData;
	CListCtrl m_StatisticsDataList;
	CComboBox m_StatisticsCombo;
	CComboBox m_ViewCombo;
	CEdit m_FindEdit;
	
	
	
	
};
