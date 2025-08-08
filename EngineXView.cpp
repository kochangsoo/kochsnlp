
// EngineXView.cpp : CEngineXView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "EngineX.h"
#endif

#include "EngineXDoc.h"
#include "EngineXView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEngineXView

IMPLEMENT_DYNCREATE(CEngineXView, CView)

BEGIN_MESSAGE_MAP(CEngineXView, CView)
	// ǥ�� �μ� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CEngineXView ����/�Ҹ�

CEngineXView::CEngineXView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CEngineXView::~CEngineXView()
{
}

BOOL CEngineXView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CEngineXView �׸���

void CEngineXView::OnDraw(CDC* /*pDC*/)
{
	CEngineXDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
}


// CEngineXView �μ�

BOOL CEngineXView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void CEngineXView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void CEngineXView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}


// CEngineXView ����

#ifdef _DEBUG
void CEngineXView::AssertValid() const
{
	CView::AssertValid();
}

void CEngineXView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEngineXDoc* CEngineXView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEngineXDoc)));
	return (CEngineXDoc*)m_pDocument;
}
#endif //_DEBUG


// CEngineXView �޽��� ó����
