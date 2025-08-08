#pragma once


#include	"naobject.h"
#include	"naentry.h"


// CTabFirstDialog 대화 상자입니다.
class CTabFirstDialog : public CDialog
{
	DECLARE_DYNAMIC(CTabFirstDialog)

public:
	CToolTipCtrl m_toolTip;
	int m_imeMode;//IME모드

public:
	CNAEntry* m_stemEntry;
	CString m_strStemAmbFeature;//사전의 중의성 자질 정보
	bool m_bStemModify;

public:
	void SetCurrentInputMode(char parm_mode);
	char GetCurrentInputMode();
	void InitStemAmbFeature();//중의성자질 160개 초기화
	void GetBitVector(char* pString);

public:
	CTabFirstDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTabFirstDialog();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAB_1_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
