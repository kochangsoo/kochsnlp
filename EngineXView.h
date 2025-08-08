
// EngineXView.h : CEngineXView Ŭ������ �������̽�
//

#pragma once


class CEngineXView : public CView
{
protected: // serialization������ ��������ϴ�.
	CEngineXView();
	DECLARE_DYNCREATE(CEngineXView)

// Ư���Դϴ�.
public:
	CEngineXDoc* GetDocument() const;

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// �����Դϴ�.
public:
	virtual ~CEngineXView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // EngineXView.cpp�� ����� ����
inline CEngineXDoc* CEngineXView::GetDocument() const
   { return reinterpret_cast<CEngineXDoc*>(m_pDocument); }
#endif

