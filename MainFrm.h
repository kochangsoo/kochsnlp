
// MainFrm.h : CMainFrame Ŭ������ �������̽�
//

#pragma once

#include "FlatSplitterWnd.h"
#include "SplashScreenEx.h"
#include "PreProcessor.h"
#include "TE_LIST.h"//���¼� �м� ��(���Ǽ��м�����) ����Ʈ ���� �з��� �Ľ�


#define REPEAT_CHK_TIMER_ID 1394

class CMainFrame : public CFrameWnd
{
	
protected: // serialization������ ��������ϴ�.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Ư���Դϴ�.
public:

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// �����Դϴ�.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // ��Ʈ�� ������ ���Ե� ����Դϴ�.
	CToolBar			m_wndToolBar;
	CStatusBar			m_wndStatusBar;
	CToolBar			m_wndHotToolBar;
	CBitmap				m_bitMap;//ToolBar ��Ʈ��
// Attributes
protected:
	CFlatSplitterWnd	m_wndSplitter;
	CSplashScreenEx*	m_pSplashMsgWindow;

public:
	CPreProcessor*		m_preProcessor;//���¼Һм� �� ��ó�� ���� Ŭ����
	TE_LIST*			m_pte_list;

public:
	CString m_strExePath;//exe����������ġ

public:
	void ShowSplashMsgWindow(CString strLoadingExplan);
	void HideSplashMsgWindow();

public:
	void DisplayStatisticsAnalysisMorphemeList();//���¼� �м� ��� ��� ���� ����Ʈ ���
	void DisplayAnalysisMorphemeList();//���¼� �м� ��� ����Ʈ ���
	
	

// ������ �޽��� �� �Լ�
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
public:
	afx_msg void OnMnuGetText();
	afx_msg void OnToolbarAnalyze();
	afx_msg void OnToolbarAnalyzeStop();
	afx_msg void OnToolbarDic();
	afx_msg void OnToolbarNewFile();
	afx_msg void OnToolbarFileOpen();
	afx_msg void OnToolbarFileSave();
	afx_msg void OnToolbarResultFile();
	afx_msg void OnToolbarResultView();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
};


