#pragma once


#include	"naobject.h"
#include	"naentry.h"

// CTabSecondDialog ��ȭ �����Դϴ�.

class CTabSecondDialog : public CDialog
{
	DECLARE_DYNAMIC(CTabSecondDialog)

public:
	CToolTipCtrl m_toolTip;
	int m_imeMode;//IME���

public:
	CNAEntry* m_endingEntry;
	bool m_bEndingModify;

public:
	void SetCurrentInputMode(char parm_mode);
	char GetCurrentInputMode();
	void GetBitVector(char* pString);

public:
	CTabSecondDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CTabSecondDialog();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAB_2_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
