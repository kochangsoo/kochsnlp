//warning C4996 : 'strcpy' : This function or variable may be unsafe.
#pragma warning ( disable : 4996 )

#pragma once


// CTopView ���Դϴ�.

class CTopView : public CView
{
	DECLARE_DYNCREATE(CTopView)

protected:
	CTopView();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~CTopView();

// Attributes
public:
	CEdit m_wndEdit;

public:
	virtual void OnDraw(CDC* pDC);      // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	int GetText();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


