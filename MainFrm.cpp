
// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "EngineX.h"

#include "MainFrm.h"
#include "common.h" //공통사용 정보
#include "TopView.h"
#include "BottomView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_MNU_GET_TEXT, &CMainFrame::OnMnuGetText)
	ON_COMMAND(ID_TOOLBAR_ANALYZE, &CMainFrame::OnToolbarAnalyze)
	ON_COMMAND(ID_TOOLBAR_ANALYZE_STOP, &CMainFrame::OnToolbarAnalyzeStop)
	ON_COMMAND(ID_TOOLBAR_DIC, &CMainFrame::OnToolbarDic)
	ON_COMMAND(ID_TOOLBAR_NEW_FILE, &CMainFrame::OnToolbarNewFile)
	ON_COMMAND(ID_TOOLBAR_FILE_OPEN, &CMainFrame::OnToolbarFileOpen)
	ON_COMMAND(ID_TOOLBAR_FILE_SAVE, &CMainFrame::OnToolbarFileSave)
	ON_COMMAND(ID_TOOLBAR_RESULT_FILE, &CMainFrame::OnToolbarResultFile)
	ON_COMMAND(ID_TOOLBAR_RESULT_VIEW, &CMainFrame::OnToolbarResultView)
	ON_WM_TIMER()
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
	m_pSplashMsgWindow = NULL;
	m_preProcessor = NULL;
	m_pte_list = NULL;
	
	
	// exe경로
	TCHAR path[_MAX_PATH];
	GetModuleFileName(NULL, path, sizeof(path));
	m_strExePath = path;
	int i = m_strExePath.ReverseFind('\\');// 실행파일 이름을 지우기 위해서 왼쪽에 있는 '/'를 찾는다.
	m_strExePath = m_strExePath.Left(i);//뒤에 있는 현재 실행파일 이름을 지운다.
}

CMainFrame::~CMainFrame()
{
	if (m_preProcessor != NULL) {
		delete m_preProcessor;
		m_preProcessor = NULL;
	}
	//결과값 OutPut처리
	if (m_pte_list != NULL)
	{
		delete m_pte_list;
		m_pte_list = NULL;
	}
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	/*	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	*/
	if (!m_wndHotToolBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_LEFT/*CBRS_RIGHT*/
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) || !m_wndHotToolBar.LoadToolBar(IDR_ENGINEX_MAINFRAME))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	m_wndHotToolBar.ModifyStyle(0, TBSTYLE_FLAT);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: 도구 모음을 도킹할 수 없게 하려면 이 세 줄을 삭제하십시오.
	/*
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	*/

	//툴바 트루컬러 이미지 추가
	// Set up hot bar image lists.
	CImageList	imageList;
	CBitmap		bitmap;

	// Create and set the normal toolbar image list.
	bitmap.LoadBitmap(IDB_ENGINEX_TOOLBAR_COLD);
	imageList.Create(24, 24, ILC_COLORDDB | ILC_MASK, 8, 1);
	imageList.Add(&bitmap, RGB(255, 0, 255));
	m_wndHotToolBar.SendMessage(TB_SETIMAGELIST, 0, (LPARAM)imageList.m_hImageList);
	imageList.Detach();
	bitmap.Detach();


	// Create and set the hot toolbar image list.
	bitmap.LoadBitmap(IDB_ENGINEX_TOOLBAR_HOT);
	imageList.Create(24, 24, ILC_COLORDDB | ILC_MASK, 8, 1);//갯수
	imageList.Add(&bitmap, RGB(255, 0, 255));
	m_wndHotToolBar.SendMessage(TB_SETHOTIMAGELIST, 0, (LPARAM)imageList.m_hImageList);
	imageList.Detach();
	bitmap.Detach();


	// Add Text to each button
	for (int i = 0; i<m_wndHotToolBar.GetCount(); i++) {
		UINT id = m_wndHotToolBar.GetItemID(i);
		//하단 텍스트 추가
		m_wndHotToolBar.SetButtonText(i, NLP_TOOLBAR_TITLE[i]);
	}

	//세로사이즈 크기에 맞게 이미지(버튼)의 크기를 재 조정한다.
	CRect rect;
	m_wndHotToolBar.GetItemRect(0, &rect);
	m_wndHotToolBar.SetSizes(rect.Size(), CSize(24, 24));

	//타이틀변경
	this->SetWindowText("언어처리솔루션 - EngineX");

	/*
	//형태소 분석 관련 클래스 생성
	//exe경로
	CString strExePath;
	TCHAR path[_MAX_PATH];
	GetModuleFileName(NULL, path, sizeof(path));
	strExePath = path;
	int i = strExePath.ReverseFind('\\');// 실행파일 이름을 지우기 위해서 왼쪽에 있는 '/'를 찾는다.
	strExePath = strExePath.Left(i);//뒤에 있는 현재 실행파일 이름을 지운다.
	m_preProcessor = new CPreProcessor(0, (char*)(const char*)strExePath);
	*/
	
	//OnTimer에서 형태소 분석 처리 형태로 변경함
	SetTimer(REPEAT_CHK_TIMER_ID, 1000, NULL);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.
	cs.style &= ~FWS_ADDTOTITLE;//제목없음 - EngineX 중 제목없음 삭제


	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기



BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (!m_wndSplitter.CreateStatic(this, 2, 1))
	{
		TRACE0("Failed to CreateStaticSplitter\n");
		return FALSE;
	}

	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CTopView), CSize(0, 380), pContext))
	{
		TRACE0("Failed to create first pane\n");
		return FALSE;
	}

	if (!m_wndSplitter.CreateView(1, 0, RUNTIME_CLASS(CBottomView), CSize(0, 270), pContext))
	{
		TRACE0("Failed to create third pane\n");
		return FALSE;
	}

	
	return TRUE;
	//return CFrameWnd::OnCreateClient(lpcs, pContext);
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch (nIDEvent)
	{
	case REPEAT_CHK_TIMER_ID:
		if (m_preProcessor == NULL)
		{
			//*********************************************************************
			//형태소 분석 관련 클래스 생성
			//*********************************************************************
			//exe경로
			CString strExePath;
			TCHAR path[_MAX_PATH];
			GetModuleFileName(NULL, path, sizeof(path));
			strExePath = path;
			int i = strExePath.ReverseFind('\\');// 실행파일 이름을 지우기 위해서 왼쪽에 있는 '/'를 찾는다.
			strExePath = strExePath.Left(i);//뒤에 있는 현재 실행파일 이름을 지운다.
			ShowSplashMsgWindow(_T("사전로딩중....잠시만 기다려 주시기 바랍니다."));
			
			//*******************************************************************
			//전처리 클래스 생성
			//*******************************************************************
			m_preProcessor = new CPreProcessor(0, (char*)(const char*)strExePath);
		
			//*******************************************************************
			//Output결과 Display관련 클래스
			//*******************************************************************
			m_pte_list = new TE_LIST;
			
		}
		else if (m_preProcessor->m_pStemEndDic->m_pStemDictionary->getDicLoadingComplete() > 0
				&& m_preProcessor->m_pStemEndDic->m_pEndingDictionary->getDicLoadingComplete() > 0
				&& m_preProcessor->m_pStemEndDic->m_pUserDictionary->getDicLoadingComplete() > 0)
		{
			HideSplashMsgWindow();
			KillTimer(REPEAT_CHK_TIMER_ID);
		}
		break;
	}
	CFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::ShowSplashMsgWindow(CString strLoadingExplan)
{
	if (m_pSplashMsgWindow == NULL)
	{
		m_pSplashMsgWindow = new CSplashScreenEx();
		m_pSplashMsgWindow->SetOption(this, _T("로딩중...."), 0, CSS_FADE | CSS_CENTERAPP | CSS_SHADOW);//CSS_CENTERSCREEN

		CRect rect;
		GetClientRect(&rect);

		CString strClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW, 0, (HBRUSH)(COLOR_WINDOW + 1));
		m_pSplashMsgWindow->CreateEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
			strClass, _T("SplashScreenExClass"), WS_POPUP, rect.left, rect.top, rect.right, rect.bottom, m_hWnd, NULL, NULL);

		m_pSplashMsgWindow->SetBitmap(IDB_LOADING_BITMAP, 255, 255, 255);
		m_pSplashMsgWindow->SetTextFont(_T("Impact"), 150, CSS_TEXT_BOLD);
		m_pSplashMsgWindow->SetTextColor(RGB(255, 255, 255));
		m_pSplashMsgWindow->SetTextFormat(DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	}

	m_pSplashMsgWindow->Show();
	m_pSplashMsgWindow->SetText(strLoadingExplan);
}


void CMainFrame::HideSplashMsgWindow()
{
	if (m_pSplashMsgWindow != NULL)
	{
		m_pSplashMsgWindow->Hide();
		m_pSplashMsgWindow = NULL;
	}
}


void CMainFrame::OnMnuGetText()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString strText;
	CTopView *pWnd = (CTopView *)((CMainFrame *)AfxGetMainWnd())->m_wndSplitter.GetPane(0, 0);
	pWnd->m_wndEdit.GetWindowText(strText);
	MessageBoxA(strText, "내용얻기", MB_OK);
	//pWnd->GetText();
	//MessageBox("ID_MNU_GET_TEXT");
}


void CMainFrame::OnToolbarAnalyze()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString strAllString;
	CTopView *pTopView = (CTopView *)((CMainFrame *)AfxGetMainWnd())->m_wndSplitter.GetPane(0, 0);
	pTopView->m_wndEdit.GetWindowText(strAllString);


	CStatic *pStatic = NULL;
	CBottomView *pBottomView = NULL;
	pBottomView = (CBottomView *)((CMainFrame *)AfxGetMainWnd())->m_wndSplitter.GetPane(1, 0);
	if (pBottomView != NULL)
	{
		pStatic = &(pBottomView->m_pTabFourthDialog->m_AnalysisTime);
		//최종 분석 시간 초기화(시.분초
		pStatic->SetWindowText("0.000");
	}

	if (m_preProcessor != NULL && strAllString != "")
	{
		m_preProcessor->DivideDocSpace((char *)(const char *)strAllString);
		//형태소 분석 결과 통계 출력
		DisplayStatisticsAnalysisMorphemeList();
		//형태소 분석 결과 리스트 출력
		DisplayAnalysisMorphemeList();
		
	}
	else
	{
		MessageBox("분석 대상 문장이 존재하지 않습니다.");
	}
}


void CMainFrame::OnToolbarAnalyzeStop()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CMainFrame::OnToolbarDic()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CMainFrame::OnToolbarNewFile()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CMainFrame::OnToolbarFileOpen()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CMainFrame::OnToolbarFileSave()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CMainFrame::OnToolbarResultFile()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CMainFrame::OnToolbarResultView()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


//형태소 분석 결과 통계 관련 리스트 출력
void CMainFrame::DisplayStatisticsAnalysisMorphemeList()
{
	//형태소 분석 결과 리스트 반영
	CBottomView *pBottomView = NULL;
	pBottomView = (CBottomView *)((CMainFrame *)AfxGetMainWnd())->m_wndSplitter.GetPane(1, 0);
	if (pBottomView != NULL && m_preProcessor != NULL)
	{
		((CMainFrame *)AfxGetMainWnd())->m_pte_list->f_migration(m_preProcessor);
		pBottomView->m_pTabFifthDialog->m_pte_data = ((CMainFrame *)AfxGetMainWnd())->m_pte_list->f_get_te_data();
		//통계-전체
		pBottomView->m_pTabFifthDialog->f_print_statistic(_VIEW_STATISTIC_ALL);
		//보기-전체
		pBottomView->m_pTabFifthDialog->f_print_out(_VIEW_OUT_ALL);
	}
}

//형태소 분석 결과 리스트 출력
void CMainFrame::DisplayAnalysisMorphemeList()
{
	//형태소 분석 결과 리스트 반영
	CBottomView *pBottomView = NULL;
	SENTENCE* pSentence = NULL;
	CListCtrl *pListCtrl = NULL;
	CEdit  *pEdit = NULL;
	CStatic *pStatic = NULL;
	int nListRowNum = 0;
	pBottomView = (CBottomView *)((CMainFrame *)AfxGetMainWnd())->m_wndSplitter.GetPane(1, 0);
	//pBottomView->m_pTabFourthDialog->GetDlgItem(IDC_SUMMARY_EDIT)->SetWindowText("dddddd");

	if (m_preProcessor != NULL)
	{
		pSentence = m_preProcessor->m_pDocVector->GoSentenceFirst();
		pListCtrl = &(pBottomView->m_pTabFourthDialog->m_AnalysisDataList);
		pEdit = &(pBottomView->m_pTabFourthDialog->m_SummaryData);
		pStatic = &(pBottomView->m_pTabFourthDialog->m_AnalysisTime);

		//최종 분석 시간
		double dblTimer = m_preProcessor->GetAnalysisResultTime();
		CString strTimerNum;//시.분초
		strTimerNum.Format(_T("%5.3f"), dblTimer);

		pStatic->SetWindowText(strTimerNum);

		//형태소분석탭의 에디터 및 리스트 컨트롤 초기화
		pListCtrl->DeleteAllItems();
		pEdit->SetWindowText("");
		if (pBottomView != NULL && pSentence != NULL)//문장리스트의 첫번째문장
		{
			while (pSentence != m_preProcessor->m_pDocVector->m_pTailSentence)
			{
				H_NODE* pNode = NULL;
				pNode = pSentence->pHead->pNext;

				if (pNode != NULL)//토큰리스트첫번째원소
				{
					while (pNode != pSentence->pTail)
					{
						if (pNode->nSkip == 0)
						{
							if (pListCtrl != NULL)
							{
								//pBottomView->m_pTabFourthDialog->m_AnalysisDataList.InsertItem(i, pNode->pHToken->strOriginalSource);
								pListCtrl->InsertItem(nListRowNum, pNode->pHToken->strOriginalSource);
								pListCtrl->SetItem(nListRowNum, 1, LVIF_TEXT, pNode->pHToken->subStem->strResult, 0, 0, 0, NULL);
								pListCtrl->SetItem(nListRowNum, 2, LVIF_TEXT, pNode->pHToken->subEnding->strResult, 0, 0, 0, NULL);
								pListCtrl->SetItem(nListRowNum, 3, LVIF_TEXT, "실패", 0, 0, 0, NULL);
								if (pNode->pHToken->nKindError == 0)
								{
									pListCtrl->SetItem(nListRowNum, 3, LVIF_TEXT, "성공", 0, 0, 0, NULL);
								}
								#ifdef _DEBUG
								pListCtrl->SetItem(nListRowNum, 4, LVIF_TEXT, pNode->pHToken->strAnalysisError, 0, 0, 0, NULL);//에러정보
								pListCtrl->SetItem(nListRowNum, 5, LVIF_TEXT, pNode->pHToken->strFlowChartDebug, 0, 0, 0, NULL);//형태소분석로직정보
								#endif			
								pListCtrl->SetItem(nListRowNum, 6, LVIF_TEXT, "보기", 0, 0, 0, NULL);

								nListRowNum += 1;
							}
						}
						pNode = pNode->pNext;
					}
				}
				pSentence = pSentence->pNext;
			}

			pBottomView->m_tabCtrl.SetCurSel(3);
			pBottomView->m_nSelctedTab = 3;
			pBottomView->m_pTabFirstDialog->ShowWindow(SW_HIDE);
			pBottomView->m_pTabSecondDialog->ShowWindow(SW_HIDE);
			pBottomView->m_pTabThirdDialog->ShowWindow(SW_HIDE);
			pBottomView->m_pTabFourthDialog->ShowWindow(SW_SHOW);
		}
	}
}





/*
VIEW_OUT_ALL,//모두출력

VIEW_OUT_N,//명사
VIEW_OUT_NUM,//수사
VIEW_OUT_PRO,//대명사
VIEW_OUT_V,//동사
VIEW_OUT_STA,//형용사
VIEW_OUT_UNC,//관형사
VIEW_OUT_ADV,//부사
VIEW_OUT_IC,//감탄사
VIEW_OUT_SYMBOL,//기호
VIEW_OUT_NUMB,//숫자
VIEW_OUT_FOREIGN,//외국어
VIEW_OUT_UNREGISTE,//미등록명사

VIEW_OUT_NEND,//조사
VIEW_OUT_VEND,//어미
VIEW_OUT_CEND//조사어미복합체
*/



void CMainFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//최소 사이즈 지정
	lpMMI->ptMinTrackSize.x = 1500; lpMMI->ptMinTrackSize.y = 795;
	lpMMI->ptMaxTrackSize.x = 1500; lpMMI->ptMaxTrackSize.y = 795;
	CFrameWnd::OnGetMinMaxInfo(lpMMI);
}
