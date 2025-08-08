// TopView.cpp : ���� �����Դϴ�.


#include "stdafx.h"
#include "EngineX.h"
#include "TopView.h"


// CTopView

IMPLEMENT_DYNCREATE(CTopView, CView)

CTopView::CTopView()
{

}

CTopView::~CTopView()
{
}

BEGIN_MESSAGE_MAP(CTopView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CTopView �׸����Դϴ�.

void CTopView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: ���⿡ �׸��� �ڵ带 �߰��մϴ�.
}


// CTopView �����Դϴ�.

#ifdef _DEBUG
void CTopView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CTopView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CTopView �޽��� ó�����Դϴ�.





int CTopView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.
	m_wndEdit.Create(WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_VSCROLL | ES_AUTOVSCROLL, CRect(0, 0, 0, 0), this, 0);


	m_wndEdit.SetLimitText(0);
	
	//value returned: 2147483646 (0x7FFFFFFE) 


	CFont font;
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfCharSet = 0x81;// �ѱ� Character Set
	lf.lfHeight = 18;// 18 point ũ��
	strcpy(lf.lfFaceName, "����ü"); // �ü�ü�� font setting
	font.CreateFontIndirect(&lf);
	m_wndEdit.SetFont((CFont*)&font, TRUE);

	return 0;
}


void CTopView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	m_wndEdit.MoveWindow(10, 10, cx-10, cy-10);
}


int CTopView::GetText()
{
	CString strText;
	m_wndEdit.GetWindowText(strText);
	MessageBox(strText);
	return 0;
}


BOOL CTopView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if (pMsg->message == WM_KEYDOWN && ::GetKeyState(VK_CONTROL) < 0)
	{
		switch (pMsg->wParam)
		{
		case 'Z':
			m_wndEdit.Undo();
			return TRUE;
		case 'X':
			m_wndEdit.Cut();
			return TRUE;
		case 'C':
			m_wndEdit.Copy();
			return TRUE;
		case 'V':
			m_wndEdit.Paste();
			return TRUE;
		case 'A':
			m_wndEdit.SetSel(0, -1);
			return TRUE;
		}
	}
	return CView::PreTranslateMessage(pMsg);
}
