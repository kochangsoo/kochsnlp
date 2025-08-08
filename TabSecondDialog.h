#pragma once


#include	"naobject.h"
#include	"naentry.h"

// CTabSecondDialog 대화 상자입니다.

class CTabSecondDialog : public CDialog
{
	DECLARE_DYNAMIC(CTabSecondDialog)

public:
	CToolTipCtrl m_toolTip;
	int m_imeMode;//IME모드

public:
	CNAEntry* m_endingEntry;
	bool m_bEndingModify;

public:
	void SetCurrentInputMode(char parm_mode);
	char GetCurrentInputMode();
	void GetBitVector(char* pString);

public:
	CTabSecondDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTabSecondDialog();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAB_2_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedEndingFindBtn();
	afx_msg void OnBnClickedEndingAddBtn();
	afx_msg void OnBnClickedEndingModifyBtn();
	afx_msg void OnBnClickedEndingDeleteBtn();
	afx_msg void OnBnClickedEndingCreateBtn();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
