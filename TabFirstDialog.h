#pragma once


#include	"naobject.h"
#include	"naentry.h"


// CTabFirstDialog ��ȭ �����Դϴ�.
class CTabFirstDialog : public CDialog
{
	DECLARE_DYNAMIC(CTabFirstDialog)

public:
	CToolTipCtrl m_toolTip;
	int m_imeMode;//IME���

public:
	CNAEntry* m_stemEntry;
	CString m_strStemAmbFeature;//������ ���Ǽ� ���� ����
	bool m_bStemModify;

public:
	void SetCurrentInputMode(char parm_mode);
	char GetCurrentInputMode();
	void InitStemAmbFeature();//���Ǽ����� 160�� �ʱ�ȭ
	void GetBitVector(char* pString);

public:
	CTabFirstDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CTabFirstDialog();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAB_1_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStemFindBtn();
	afx_msg void OnBnClickedStemAddBtn();
	afx_msg void OnBnClickedStemModifyBtn();
	afx_msg void OnBnClickedStmeDeleteBtn();
	afx_msg void OnBnClickedStemCreateBtn();
	afx_msg void OnBnClickedStemAmbMoreBtn();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
