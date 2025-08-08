// TabSecondDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EngineX.h"
#include "TabSecondDialog.h"
#include "afxdialogex.h"

#include "MainFrm.h"
#include "common.h" //공통사용 정보

// CTabSecondDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTabSecondDialog, CDialog)

CTabSecondDialog::CTabSecondDialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_TAB_2_DIALOG, pParent)
{
	m_imeMode = 0;
	m_endingEntry = NULL;
	m_bEndingModify = FALSE;
}

CTabSecondDialog::~CTabSecondDialog()
{
}

void CTabSecondDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTabSecondDialog, CDialog)
	ON_BN_CLICKED(IDC_ENDING_FIND_BTN, &CTabSecondDialog::OnBnClickedEndingFindBtn)
	ON_BN_CLICKED(IDC_ENDING_ADD_BTN, &CTabSecondDialog::OnBnClickedEndingAddBtn)
	ON_BN_CLICKED(IDC_ENDING_MODIFY_BTN, &CTabSecondDialog::OnBnClickedEndingModifyBtn)
	ON_BN_CLICKED(IDC_ENDING_DELETE_BTN, &CTabSecondDialog::OnBnClickedEndingDeleteBtn)
	ON_BN_CLICKED(IDC_ENDING_CREATE_BTN, &CTabSecondDialog::OnBnClickedEndingCreateBtn)
END_MESSAGE_MAP()


// CTabSecondDialog 메시지 처리기입니다.


void CTabSecondDialog::OnBnClickedEndingFindBtn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrm = (CMainFrame *)AfxGetMainWnd();
	CString strWord = _T("");
	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_ENDING_WORD);
	pEdit->GetWindowText(strWord);     // 현재 Unicode인 경우
	if (strWord == _T("")) {
		MessageBox(_T("검색어을 입력하시기 바랍니다."), _T("검색"), MB_OK);
		pEdit->SetFocus();
	}
	else
	{
		char *strEntryWord = NULL;
		strEntryWord = (char *)(const char *)strWord;
		m_endingEntry = pFrm->m_preProcessor->m_pStemEndDic->GetDicWordEntry(strEntryWord, DIC_ENDING);
		if (m_endingEntry != NULL)
		{
			pEdit->SetWindowText(CA2T(m_endingEntry->getKey()));
			char* pBitVector = m_endingEntry->getBitVector(0);
			if (pBitVector != NULL)
			{
				for (int i = 0; i < ENDING_DIC_BASIC_FEATURE_NUMBER; i++)
				{
					CButton *pButton = (CButton*)GetDlgItem(30000 + i);
					if (*(pBitVector + i) == '1')
						pButton->SetCheck(1);
					else
						pButton->SetCheck(0);
				}
			}
		}
		else
		{
			m_bEndingModify = FALSE;
			MessageBox(_T("엔딩사전에서 단어를 찾지 못했습니다."), _T("검색실패"), MB_OK);
		}
	}
}


void CTabSecondDialog::OnBnClickedEndingAddBtn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrm = (CMainFrame *)AfxGetMainWnd();
	CString strWord = _T("");
	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_ENDING_WORD);
	pEdit->GetWindowText(strWord); // 현재 Unicode인 경우
	if (strWord == _T("")) {
		MessageBox(_T("검색어을 입력하시기 바랍니다."), _T("검색"), MB_OK);
		pEdit->SetFocus();
	}
	else
	{
		m_bEndingModify = TRUE;
		CString s = _T("");
		s = strWord + _T("를 사전에 신규 추가하시겠습니까?");
		if (AfxMessageBox(s, MB_YESNO) == IDYES)
		{
			char *strEntryWord = NULL;
			strEntryWord = (char *)(const char *)strWord;
			CNAEntry* pExistEndingWord = pFrm->m_preProcessor->m_pStemEndDic->GetDicWordEntry(strEntryWord, DIC_ENDING);
			if (pExistEndingWord != NULL)
			{
				AfxMessageBox(_T("'" + strWord + "' 단어가 존재합니다."));
			}
			else
			{
				char* vector = new char[256];
				memset(vector, 0x00, 256);
				GetBitVector(vector);
				CNAEntry* pAddEntry = new CNAEntry();
				char* pAddWord = CStringUtil::ConvertCStringTochar(strWord);
				pAddEntry->setString(pAddWord, vector);

				BOOL bEntryAdd = pFrm->m_preProcessor->m_pStemEndDic->AddDicWordEntry(pAddEntry, DIC_ENDING);
				if (bEntryAdd)
				{
					AfxMessageBox(_T("'" + strWord + "' 단어를 [메모리영역에서만] 추가하였습니다."));
					m_endingEntry = pAddEntry;
				}
				delete[] vector;
				delete pAddWord;
				vector = NULL;
				pAddWord = NULL;

			}
		}
	}
}


void CTabSecondDialog::OnBnClickedEndingModifyBtn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_endingEntry != NULL)
	{
		m_bEndingModify = TRUE;
		char* vector = new char[ENDING_DIC_BASIC_FEATURE_NUMBER];
		memset(vector, 0x00, ENDING_DIC_BASIC_FEATURE_NUMBER);
		GetBitVector(vector);
		m_endingEntry->SetBitVector(0, vector);
		delete[] vector;
		vector = NULL;
		MessageBox(_T("해당 단어의 자질 정보를 [메모리영역에서만]수정하였습니다."), _T("수정완료"), MB_ICONERROR);
	}
	else
	{
		m_endingEntry = FALSE;
		MessageBox(_T("수정할 정보가 존재하지 않습니다."), _T("수정오류"), MB_ICONERROR);
	}
}


void CTabSecondDialog::OnBnClickedEndingDeleteBtn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrm = (CMainFrame *)AfxGetMainWnd();
	if (m_endingEntry != NULL)
	{
		m_bEndingModify = TRUE;
		char* key = new char[1024];
		memset(key, 0x00, 1024);
		m_endingEntry->getKey(key);
		CString s = _T("");
		s.Format(_T("'%s' 단어를 정말로 삭제하겠습니까?"), CA2T(key));
		if (AfxMessageBox(s, MB_YESNO) == IDYES)
		{
			pFrm->m_preProcessor->m_pStemEndDic->RemoveDicWordEntry(m_endingEntry, DIC_ENDING);
			m_endingEntry = NULL;
		}
		delete[] key;
		key = NULL;
	}
}


void CTabSecondDialog::OnBnClickedEndingCreateBtn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	bool bFileSave = false;
	CString strCreateFileName;
	CMainFrame *pFrm = (CMainFrame *)AfxGetMainWnd();
	if (m_bEndingModify == TRUE)
	{
		if (GetFileAttributes(pFrm->m_strExePath + "\\Dictionary\\Backup") == FILE_ATTRIBUTE_DIRECTORY)//저장할 디렉토리 확인
		{
			//CopyFile(원본 파일 경로, 결과 파일 결로, 덮어쓰기 여부);
			//덮어쓰기여부는 TRUE는 fail, FALSE는 덮어쓰기
			CTime cTime = CTime::GetCurrentTime(); // 현재 시스템으로부터 날짜 및 시간을 얻어 온다.
			CString strDate, strTime; // 반환되는 날짜와 시간을 저장할 CString 변수 선언
			strDate.Format("%04d_%02d_%02d_%02d_%02d_%02d", cTime.GetYear(), cTime.GetMonth(), cTime.GetDay(), cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond());
			if (pFrm->m_preProcessor->m_pStemEndDic->m_pEndingDictionary != NULL)
			{
				//기존파일 복사하여 Backup 디렉토리에 저장한다.
				CopyFile(pFrm->m_strExePath + "\\Dictionary\\" + pFrm->m_preProcessor->m_pStemEndDic->m_strEndingDicFileName, pFrm->m_strExePath + "\\Dictionary\\backup\\" + strDate + "_" + pFrm->m_preProcessor->m_pStemEndDic->m_strEndingDicFileName, FALSE);
				strCreateFileName = pFrm->m_strExePath + "\\Dictionary\\" + pFrm->m_preProcessor->m_pStemEndDic->m_strEndingDicFileName;
				if (pFrm->m_preProcessor->m_pStemEndDic->m_nEncryption == 0)
				{
					//암호화하지않고 사전 저장 및 기존 파일 다른이름으로 저장
					if (pFrm->m_preProcessor->m_pStemEndDic->m_pEndingDictionary->storeDictionary(pFrm->m_preProcessor->m_pStemEndDic->m_strEndingDicFileName, DIC_ENDING, 0) == 0)
					{
						bFileSave = true;
					}
				}
				else
				{
					//암호화하여 사전 저장 및 기존 파일 다른이름으로 저장
					if (pFrm->m_preProcessor->m_pStemEndDic->m_pEndingDictionary->encStoreDictionary(pFrm->m_preProcessor->m_pStemEndDic->m_strEndingDicFileName, DIC_ENDING, 0) == 0)
					{
						bFileSave = true;
					}
				}
			}
		}
		else
		{
			AfxMessageBox("백업할 디렉토리가 존재하지 않습니다.");
		}
		if (bFileSave == true)
		{
			AfxMessageBox("파일저장 성공");
			m_bEndingModify = FALSE;
		}
	}
}


BOOL CTabSecondDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_toolTip.Create(GetActiveWindow(), TTS_ALWAYSTIP | TTS_BALLOON);

	m_toolTip.SetDelayTime(TTDT_INITIAL, 500);

	m_toolTip.SetMaxTipWidth(400);

	//툴팁 삽입
	for (int i = 0; i < ENDING_DIC_BASIC_FEATURE_NUMBER; i++) {
		//resource.h에 엔딩사전의 체크박스를 30000으로 재 정의함
		CString str(ENDING_FEATURE_DESCRIPT[i]);
		m_toolTip.AddTool(GetDlgItem(30000 + i), str);
	}

	m_toolTip.Activate(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CTabSecondDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	m_toolTip.RelayEvent(pMsg);

	//IME가 한글 모드가 아닌 경우
	if (m_imeMode == 0)
	{
		SetCurrentInputMode(1);
	}

	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_ENDING_WORD)->m_hWnd) {
		// 눌러진 키가 엔터키인가? 
		if (pMsg->wParam == VK_RETURN)
		{
			// 엔터키 입력 후 하고자 하는 기능 코드 추가. 
			OnBnClickedEndingFindBtn();
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CTabSecondDialog::GetBitVector(char* pString)
{
	if (pString != NULL)
	{
		int nEndingBasicLen = 0;
		for (int i = 0; i < ENDING_DIC_BASIC_FEATURE_NUMBER; i++)
		{
			CButton *pButton = (CButton*)GetDlgItem(30000 + i);
			if (pButton->GetCheck() == BST_CHECKED)
			{
				pString[nEndingBasicLen++] = '1';
			}
			else
			{
				pString[nEndingBasicLen++] = '0';
			}
		}
		pString[nEndingBasicLen + ENDING_DIC_BASIC_FEATURE_NUMBER] = 0;

	}
}

// 현재 IME의 입력 모드를 반환한다. 0: 영문, 1: 한글
char CTabSecondDialog::GetCurrentInputMode()
{
	DWORD conv, sentence;

	HIMC h_imc = ImmGetContext(m_hWnd);
	// IME(input method editor) 상태를 얻는다.
	ImmGetConversionStatus(h_imc, &conv, &sentence);

	ImmReleaseContext(m_hWnd, h_imc);

	if (conv & IME_CMODE_LANGUAGE) return 1;  // 한글 상태임... 

	return 0;
}

// 현재 IME의 입력 모드를 설정한다. 0: 영문, 1: 한글
void CTabSecondDialog::SetCurrentInputMode(char parm_mode)
{
	DWORD conv, sentence;

	HIMC h_imc = ImmGetContext(m_hWnd);
	// IME(input method editor) 상태를 얻는다.
	ImmGetConversionStatus(h_imc, &conv, &sentence);

	if (parm_mode == 1) {
		// 영문 입력 상태였다면 상태를 제거한다.
		conv = conv & (~IME_CMODE_CHARCODE);
		// 한글 입력 상태로 설정한다.
		conv = conv | IME_CMODE_LANGUAGE;
		m_imeMode = 1;
	}
	else {
		// 한글 입력 상태였다면 상태를 제거한다.
		conv = conv & (~IME_CMODE_LANGUAGE);
		// 영문 입력 상태로 설정한다.
		conv = conv | IME_CMODE_CHARCODE;
		m_imeMode = 0;
	}

	// IME 상태를 설정한다.
	ImmSetConversionStatus(h_imc, conv, sentence);
	ImmReleaseContext(m_hWnd, h_imc);


}