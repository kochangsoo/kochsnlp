
// MainFrm.cpp : CMainFrame Ŭ������ ����
//

#include "stdafx.h"
#include "EngineX.h"

#include "MainFrm.h"
#include "common.h" //������ ����
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
	ID_SEPARATOR,           // ���� �� ǥ�ñ�
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame ����/�Ҹ�

CMainFrame::CMainFrame()
{
	// TODO: ���⿡ ��� �ʱ�ȭ �ڵ带 �߰��մϴ�.
	m_pSplashMsgWindow = NULL;
	m_preProcessor = NULL;
	m_pte_list = NULL;
	
	
	// exe���
	TCHAR path[_MAX_PATH];
	GetModuleFileName(NULL, path, sizeof(path));
	m_strExePath = path;
	int i = m_strExePath.ReverseFind('\\');// �������� �̸��� ����� ���ؼ� ���ʿ� �ִ� '/'�� ã�´�.
	m_strExePath = m_strExePath.Left(i);//�ڿ� �ִ� ���� �������� �̸��� �����.
}

CMainFrame::~CMainFrame()
{
	if (m_preProcessor != NULL) {
		delete m_preProcessor;
		m_preProcessor = NULL;
	}
	//����� OutPutó��
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
		TRACE0("���� ������ ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}
	*/
	if (!m_wndHotToolBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_LEFT/*CBRS_RIGHT*/
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) || !m_wndHotToolBar.LoadToolBar(IDR_ENGINEX_MAINFRAME))
	{
		TRACE0("���� ������ ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}
	m_wndHotToolBar.ModifyStyle(0, TBSTYLE_FLAT);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("���� ǥ������ ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: ���� ������ ��ŷ�� �� ���� �Ϸ��� �� �� ���� �����Ͻʽÿ�.
	/*
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	*/

	//���� Ʈ���÷� �̹��� �߰�
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
	imageList.Create(24, 24, ILC_COLORDDB | ILC_MASK, 8, 1);//����
	imageList.Add(&bitmap, RGB(255, 0, 255));
	m_wndHotToolBar.SendMessage(TB_SETHOTIMAGELIST, 0, (LPARAM)imageList.m_hImageList);
	imageList.Detach();
	bitmap.Detach();


	// Add Text to each button
	for (int i = 0; i<m_wndHotToolBar.GetCount(); i++) {
		UINT id = m_wndHotToolBar.GetItemID(i);
		//�ϴ� �ؽ�Ʈ �߰�
		m_wndHotToolBar.SetButtonText(i, NLP_TOOLBAR_TITLE[i]);
	}

	//���λ����� ũ�⿡ �°� �̹���(��ư)�� ũ�⸦ �� �����Ѵ�.
	CRect rect;
	m_wndHotToolBar.GetItemRect(0, &rect);
	m_wndHotToolBar.SetSizes(rect.Size(), CSize(24, 24));

	//Ÿ��Ʋ����
	this->SetWindowText("���ó���ַ�� - EngineX");

	/*
	//���¼� �м� ���� Ŭ���� ����
	//exe���
	CString strExePath;
	TCHAR path[_MAX_PATH];
	GetModuleFileName(NULL, path, sizeof(path));
	strExePath = path;
	int i = strExePath.ReverseFind('\\');// �������� �̸��� ����� ���ؼ� ���ʿ� �ִ� '/'�� ã�´�.
	strExePath = strExePath.Left(i);//�ڿ� �ִ� ���� �������� �̸��� �����.
	m_preProcessor = new CPreProcessor(0, (char*)(const char*)strExePath);
	*/
	
	//OnTimer���� ���¼� �м� ó�� ���·� ������
	SetTimer(REPEAT_CHK_TIMER_ID, 1000, NULL);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.
	cs.style &= ~FWS_ADDTOTITLE;//������� - EngineX �� ������� ����


	return TRUE;
}

// CMainFrame ����

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


// CMainFrame �޽��� ó����



BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	switch (nIDEvent)
	{
	case REPEAT_CHK_TIMER_ID:
		if (m_preProcessor == NULL)
		{
			//*********************************************************************
			//���¼� �м� ���� Ŭ���� ����
			//*********************************************************************
			//exe���
			CString strExePath;
			TCHAR path[_MAX_PATH];
			GetModuleFileName(NULL, path, sizeof(path));
			strExePath = path;
			int i = strExePath.ReverseFind('\\');// �������� �̸��� ����� ���ؼ� ���ʿ� �ִ� '/'�� ã�´�.
			strExePath = strExePath.Left(i);//�ڿ� �ִ� ���� �������� �̸��� �����.
			ShowSplashMsgWindow(_T("�����ε���....��ø� ��ٷ� �ֽñ� �ٶ��ϴ�."));
			
			//*******************************************************************
			//��ó�� Ŭ���� ����
			//*******************************************************************
			m_preProcessor = new CPreProcessor(0, (char*)(const char*)strExePath);
		
			//*******************************************************************
			//Output��� Display���� Ŭ����
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
		m_pSplashMsgWindow->SetOption(this, _T("�ε���...."), 0, CSS_FADE | CSS_CENTERAPP | CSS_SHADOW);//CSS_CENTERSCREEN

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
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CString strText;
	CTopView *pWnd = (CTopView *)((CMainFrame *)AfxGetMainWnd())->m_wndSplitter.GetPane(0, 0);
	pWnd->m_wndEdit.GetWindowText(strText);
	MessageBoxA(strText, "������", MB_OK);
	//pWnd->GetText();
	//MessageBox("ID_MNU_GET_TEXT");
}


void CMainFrame::OnToolbarAnalyze()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CString strAllString;
	CTopView *pTopView = (CTopView *)((CMainFrame *)AfxGetMainWnd())->m_wndSplitter.GetPane(0, 0);
	pTopView->m_wndEdit.GetWindowText(strAllString);


	CStatic *pStatic = NULL;
	CBottomView *pBottomView = NULL;
	pBottomView = (CBottomView *)((CMainFrame *)AfxGetMainWnd())->m_wndSplitter.GetPane(1, 0);
	if (pBottomView != NULL)
	{
		pStatic = &(pBottomView->m_pTabFourthDialog->m_AnalysisTime);
		//���� �м� �ð� �ʱ�ȭ(��.����
		pStatic->SetWindowText("0.000");
	}

	if (m_preProcessor != NULL && strAllString != "")
	{
		m_preProcessor->DivideDocSpace((char *)(const char *)strAllString);
		//���¼� �м� ��� ��� ���
		DisplayStatisticsAnalysisMorphemeList();
		//���¼� �м� ��� ����Ʈ ���
		DisplayAnalysisMorphemeList();
		
	}
	else
	{
		MessageBox("�м� ��� ������ �������� �ʽ��ϴ�.");
	}
}


void CMainFrame::OnToolbarAnalyzeStop()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}


void CMainFrame::OnToolbarDic()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}


void CMainFrame::OnToolbarNewFile()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}


void CMainFrame::OnToolbarFileOpen()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}


void CMainFrame::OnToolbarFileSave()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}


void CMainFrame::OnToolbarResultFile()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}


void CMainFrame::OnToolbarResultView()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}


//���¼� �м� ��� ��� ���� ����Ʈ ���
void CMainFrame::DisplayStatisticsAnalysisMorphemeList()
{
	//���¼� �м� ��� ����Ʈ �ݿ�
	CBottomView *pBottomView = NULL;
	pBottomView = (CBottomView *)((CMainFrame *)AfxGetMainWnd())->m_wndSplitter.GetPane(1, 0);
	if (pBottomView != NULL && m_preProcessor != NULL)
	{
		((CMainFrame *)AfxGetMainWnd())->m_pte_list->f_migration(m_preProcessor);
		pBottomView->m_pTabFifthDialog->m_pte_data = ((CMainFrame *)AfxGetMainWnd())->m_pte_list->f_get_te_data();
		//���-��ü
		pBottomView->m_pTabFifthDialog->f_print_statistic(_VIEW_STATISTIC_ALL);
		//����-��ü
		pBottomView->m_pTabFifthDialog->f_print_out(_VIEW_OUT_ALL);
	}
}

//���¼� �м� ��� ����Ʈ ���
void CMainFrame::DisplayAnalysisMorphemeList()
{
	//���¼� �м� ��� ����Ʈ �ݿ�
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

		//���� �м� �ð�
		double dblTimer = m_preProcessor->GetAnalysisResultTime();
		CString strTimerNum;//��.����
		strTimerNum.Format(_T("%5.3f"), dblTimer);

		pStatic->SetWindowText(strTimerNum);

		//���¼Һм����� ������ �� ����Ʈ ��Ʈ�� �ʱ�ȭ
		pListCtrl->DeleteAllItems();
		pEdit->SetWindowText("");
		if (pBottomView != NULL && pSentence != NULL)//���帮��Ʈ�� ù��°����
		{
			while (pSentence != m_preProcessor->m_pDocVector->m_pTailSentence)
			{
				H_NODE* pNode = NULL;
				pNode = pSentence->pHead->pNext;

				if (pNode != NULL)//��ū����Ʈù��°����
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
								pListCtrl->SetItem(nListRowNum, 3, LVIF_TEXT, "����", 0, 0, 0, NULL);
								if (pNode->pHToken->nKindError == 0)
								{
									pListCtrl->SetItem(nListRowNum, 3, LVIF_TEXT, "����", 0, 0, 0, NULL);
								}
								#ifdef _DEBUG
								pListCtrl->SetItem(nListRowNum, 4, LVIF_TEXT, pNode->pHToken->strAnalysisError, 0, 0, 0, NULL);//��������
								pListCtrl->SetItem(nListRowNum, 5, LVIF_TEXT, pNode->pHToken->strFlowChartDebug, 0, 0, 0, NULL);//���¼Һм���������
								#endif			
								pListCtrl->SetItem(nListRowNum, 6, LVIF_TEXT, "����", 0, 0, 0, NULL);

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
VIEW_OUT_ALL,//������

VIEW_OUT_N,//���
VIEW_OUT_NUM,//����
VIEW_OUT_PRO,//����
VIEW_OUT_V,//����
VIEW_OUT_STA,//�����
VIEW_OUT_UNC,//������
VIEW_OUT_ADV,//�λ�
VIEW_OUT_IC,//��ź��
VIEW_OUT_SYMBOL,//��ȣ
VIEW_OUT_NUMB,//����
VIEW_OUT_FOREIGN,//�ܱ���
VIEW_OUT_UNREGISTE,//�̵�ϸ��

VIEW_OUT_NEND,//����
VIEW_OUT_VEND,//���
VIEW_OUT_CEND//�����̺���ü
*/



void CMainFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	//�ּ� ������ ����
	lpMMI->ptMinTrackSize.x = 1500; lpMMI->ptMinTrackSize.y = 795;
	lpMMI->ptMaxTrackSize.x = 1500; lpMMI->ptMaxTrackSize.y = 795;
	CFrameWnd::OnGetMinMaxInfo(lpMMI);
}
