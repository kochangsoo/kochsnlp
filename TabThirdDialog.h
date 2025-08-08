#pragma once

#include	"naobject.h"
#include	"naentry.h"
#include	"StringUtil.h"

// CTabThirdDialog 대화 상자입니다.
class CTabThirdDialog : public CDialog
{
	DECLARE_DYNAMIC(CTabThirdDialog)

public:
	CToolTipCtrl m_toolTip;
	int m_imeMode;//IME모드

public:
	CNAEntry* m_userEntry;
	BOOL m_bUserModify;

public:
	void SetCurrentInputMode(char parm_mode);
	char GetCurrentInputMode();
	void GetBitVector(char* pString);


public:
	CTabThirdDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTabThirdDialog();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAB_3_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
