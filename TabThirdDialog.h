#pragma once

#include	"naobject.h"
#include	"naentry.h"
#include	"StringUtil.h"

// CTabThirdDialog ��ȭ �����Դϴ�.
class CTabThirdDialog : public CDialog
{
	DECLARE_DYNAMIC(CTabThirdDialog)

public:
	CToolTipCtrl m_toolTip;
	int m_imeMode;//IME���

public:
	CNAEntry* m_userEntry;
	BOOL m_bUserModify;

public:
	void SetCurrentInputMode(char parm_mode);
	char GetCurrentInputMode();
	void GetBitVector(char* pString);


public:
	CTabThirdDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CTabThirdDialog();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAB_3_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedUserFindBtn();
	afx_msg void OnBnClickedUserAddBtn();
	afx_msg void OnBnClickedUserModifyBtn();
	afx_msg void OnBnClickedUserDeleteBtn();
	afx_msg void OnBnClickedUserCreateBtn();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
};
