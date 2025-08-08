
// MainFrm.h : CMainFrame 클래스의 인터페이스
//

#pragma once

#include "FlatSplitterWnd.h"
#include "SplashScreenEx.h"
#include "PreProcessor.h"
#include "TE_LIST.h"//형태소 분석 후(중의성분석포함) 리스트 정보 분류별 파싱


#define REPEAT_CHK_TIMER_ID 1394

class CMainFrame : public CFrameWnd
{
	
protected: // serialization에서만 만들어집니다.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 특성입니다.
public:

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CToolBar			m_wndToolBar;
	CStatusBar			m_wndStatusBar;
	CToolBar			m_wndHotToolBar;
	CBitmap				m_bitMap;//ToolBar 비트맵
// Attributes
protected:
	CFlatSplitterWnd	m_wndSplitter;
	CSplashScreenEx*	m_pSplashMsgWindow;

public:
	CPreProcessor*		m_preProcessor;//형태소분석 전 전처리 관련 클래스
	TE_LIST*			m_pte_list;

public:
	CString m_strExePath;//exe실행파일위치

public:
	void ShowSplashMsgWindow(CString strLoadingExplan);
	void HideSplashMsgWindow();

public:
	void DisplayStatisticsAnalysisMorphemeList();//형태소 분석 결과 통계 관련 리스트 출력
	void DisplayAnalysisMorphemeList();//형태소 분석 결과 리스트 출력
	
	

// 생성된 메시지 맵 함수
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


