// TabFourthDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EngineX.h"
#include "TabFourthDialog.h"
#include "afxdialogex.h"


// CTabFourthDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTabFourthDialog, CDialog)

CTabFourthDialog::CTabFourthDialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_TAB_4_DIALOG, pParent)
{

}

CTabFourthDialog::~CTabFourthDialog()
{
}

void CTabFourthDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SUMMARY_EDIT, m_SummaryData);
	DDX_Control(pDX, IDC_ANALYSIS_LIST, m_AnalysisDataList);
	DDX_Control(pDX, IDC_STATIC_ANALYSIS_TIME, m_AnalysisTime);
	DDX_Control(pDX, IDC_EDIT_RESULT_SEARCH_WORDS, m_ResultSearchWord);
}


BEGIN_MESSAGE_MAP(CTabFourthDialog, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_ANALYSIS_LIST, &CTabFourthDialog::OnNMClickAnalysisList)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_ANALYSIS_SAVE_BUTTON, &CTabFourthDialog::OnBnClickedAnalysisSaveButton)
END_MESSAGE_MAP()


// CTabFourthDialog 메시지 처리기입니다.


BOOL CTabFourthDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CRect rect;
	m_AnalysisDataList.GetClientRect(&rect);

	m_AnalysisDataList.DeleteAllItems();

	m_AnalysisDataList.InsertColumn(0, "원문", LVCFMT_CENTER, 150, -1);
	m_AnalysisDataList.InsertColumn(1, "어간", LVCFMT_CENTER, 110, -1);
	m_AnalysisDataList.InsertColumn(2, "어미", LVCFMT_CENTER, 110, -1);
	m_AnalysisDataList.InsertColumn(3, "결과", LVCFMT_CENTER, 100, -1);//성공/실패
	m_AnalysisDataList.InsertColumn(4, "에러정보", LVCFMT_CENTER, 300, -1);//실패시에러상세정보
	m_AnalysisDataList.InsertColumn(5, "알고리즘상세보기", LVCFMT_CENTER, 250, -1);
	m_AnalysisDataList.InsertColumn(6, "문장보기", LVCFMT_CENTER, 100, -1);

	m_AnalysisDataList.SetExtendedStyle(m_AnalysisDataList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_SummaryData.ShowScrollBar(SB_VERT, TRUE);
	
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
						// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CTabFourthDialog::OnNMClickAnalysisList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//http://codingpia.egloos.com/7319497
	if (pNMItemActivate->iItem != -1)
	{
		CRect rect;
		if (pNMItemActivate->iSubItem == 0)
		{
			m_AnalysisDataList.GetItemRect(pNMItemActivate->iItem, rect, LVIR_BOUNDS);
			rect.right = rect.left + m_AnalysisDataList.GetColumnWidth(0);
		}
		else
		{
			m_AnalysisDataList.GetSubItemRect(pNMItemActivate->iItem, pNMItemActivate->iSubItem, LVIR_BOUNDS, rect);
		}
		if(pNMItemActivate->iSubItem == 4)//에러보기
		{
		}
		else if (pNMItemActivate->iSubItem == 5)//알고리즘보기
		{
		}
		else if (pNMItemActivate->iSubItem == 6)//문장보기
		{
		}
		GetDlgItem(IDC_SUMMARY_EDIT)->SetWindowText(m_AnalysisDataList.GetItemText(pNMItemActivate->iItem, pNMItemActivate->iSubItem));
		
	}
	*pResult = 0;
}





void CTabFourthDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CRect rect;
	GetClientRect(&rect);
	if(m_SummaryData.GetSafeHwnd() && m_AnalysisDataList.GetSafeHwnd())
	{
		m_SummaryData.MoveWindow(rect.left+11, rect.top + 40,250, rect.bottom-45);
		m_AnalysisDataList.MoveWindow(rect.left+265, rect.top + 40, rect.right- 250, rect.bottom - 45);
	}
	
	
	
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

//형태소 분석 결과 파일 저장 및 다중 검색에 대한 통계 정보 파일 하단 출력
void CTabFourthDialog::OnBnClickedAnalysisSaveButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	char programpath[_MAX_PATH];
	//GetModuleFileName(NULL, programpath, _MAX_PATH);
	//GetCurrentDirectory(_MAX_PATH, programpath);//현재 폴더 경로
	strcpy(programpath, "D:\\이재명,남경필,전해철");
	CString strFileName = "";
	CTime cTime = CTime::GetCurrentTime(); // 현재 시스템으로부터 날짜 및 시간을 얻어 온다.
	CString strDate, strTime; // 반환되는 날짜와 시간을 저장할 CString 변수 선언
	strDate.Format("%04d_%02d_%02d_", cTime.GetYear(),cTime.GetMonth(),cTime.GetDay()); // 현재 년월일 반환
	strTime.Format("%02d%02d%02d", cTime.GetHour(),cTime.GetMinute(),cTime.GetSecond()); // 현재 시분초 반환
	strFileName.Format("%s%s%s%s%s", programpath,"\\", strDate,strTime,"_형태소분석결과[엑셀_999].txt");

	
	
	m_ResultSearchWord.SetWindowText("남경필,이재명,전해철");//임시값 부여
	BOOL bDisplaySummary = FALSE;
	CString strSearchWord = "";
	m_ResultSearchWord.GetWindowText(strSearchWord);
	strSearchWord.Replace(" ", NULL);//공백제거
	int nSplitNumToken = 0;
	CString * splitWordTokenList = NULL;//문자열 분리 갯수 보관
	int* splitWordTokenFindNumberList = NULL;//문자열의 찾은 갯수 보관

	//검색내찾을 정보가 있을 시 처리
	if (strSearchWord != "")
	{
		bDisplaySummary = TRUE;
		splitWordTokenList = Split(strSearchWord, ',', nSplitNumToken);//와 같은 식으로 사용 한 후
		splitWordTokenFindNumberList = new int[nSplitNumToken];
		for (int i = 0; i < nSplitNumToken; i++)
		{
			splitWordTokenFindNumberList[i] = 0;//초기화
		}
	}
	
	CStdioFile fileWrite;
	//if (fileWrite.Open("D:\\fileWrite.txt", CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite))
	if (fileWrite.Open(strFileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite))
	{
		//타이틀 지정	
		fileWrite.SeekToEnd();
		CString strHeadResult = "";
		strHeadResult = "원문\t\t\t\t어간\t\t\t어미\n";
		fileWrite.WriteString(strHeadResult);
		for (int nIndex = 0; nIndex < m_AnalysisDataList.GetItemCount(); nIndex++)
		{
			//첫번째 컬럼을 돌면서 입력한 값과 같은 값을 찾습니다.
			CString strResult = "";
			CString strOrg = m_AnalysisDataList.GetItemText(nIndex, 0);
			CString strStem = m_AnalysisDataList.GetItemText(nIndex, 1);
			CString strEnding = m_AnalysisDataList.GetItemText(nIndex, 2);

			//통계정보 생성
			if (bDisplaySummary == TRUE)
			{
				for (int i = 0; i < nSplitNumToken; i++)
				{
					if (strStem.Find(splitWordTokenList[i]) >= 0)
					{
						splitWordTokenFindNumberList[i] += 1;
					}
				}
			}
			fileWrite.SeekToEnd();
			strResult = strOrg + "\t\t\t\t" + strStem + "\t\t\t" + strEnding + "\n";
			//원문이 긴 경우 탭 간격을 맞추기 위해 임의로 조정함
			if(strlen(strOrg) > 10 && strlen(strOrg) < 20)
				strResult = strOrg + "\t\t\t" + strStem + "\t\t"  + strEnding + "\n";
			else if(strlen(strOrg) >= 20)
				strResult = strOrg + "\t\t" + strStem + "\t\t" + strEnding + "\n";
			
			fileWrite.WriteString(strResult);
		}

		if (bDisplaySummary == TRUE)
		{
			fileWrite.SeekToEnd();
			CString strResult = "";
			strResult = "\n\n=============================================================================\n";
			strResult += "결과내 검색에 대한 통계 정보";
			strResult += "\n\n=============================================================================\n";
			fileWrite.WriteString(strResult);
			for (int i = 0; i < nSplitNumToken; i++)
			{
				fileWrite.SeekToEnd();
				CString strResult = "";
				strResult.Format("%s\t\t\t\t%d%s", splitWordTokenList[i], splitWordTokenFindNumberList[i],"\n");
				fileWrite.WriteString(strResult);
			}
			MessageBox("파일생성완료", "파일이 정상적으로 생성되었습니다", MB_OK);
		}
	}
	fileWrite.Close();

	//포인트배열 삭제
	if (splitWordTokenList != NULL)
	{
		delete[] splitWordTokenList;
	}
	if (splitWordTokenFindNumberList != NULL)
	{
		delete[] splitWordTokenFindNumberList;
	}
}

CString* CTabFourthDialog::Split(CString Ps_Str, TCHAR Pc_Separator, int& Ri_Length)
{
	Ri_Length = GetCountChar(Ps_Str, Pc_Separator) + 1;
	if (Ps_Str[0] == Pc_Separator) Ri_Length--;
	if (Ps_Str[Ps_Str.GetLength() - 1] == Pc_Separator) Ri_Length--;
	CString* Rsa_Str = new CString[Ri_Length];

	int Li_Count = 0;
	CString Ls_Token;

	for (int i = 0; i < Ri_Length; i++)
	{
		AfxExtractSubString(Ls_Token, Ps_Str, Li_Count, Pc_Separator);
		Rsa_Str[Li_Count++] = Ls_Token;
	}

	return Rsa_Str;
}

int CTabFourthDialog::GetCountChar(CString Ps_Str, TCHAR Pc_Char)
{
	int Li_Count = 0;
	for (int i = 0; i < Ps_Str.GetLength(); i++)
	{
		if (Ps_Str[i] == Pc_Char) Li_Count++;
	}
	return Li_Count;
}


