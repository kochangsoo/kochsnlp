// TabFourthDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "EngineX.h"
#include "TabFourthDialog.h"
#include "afxdialogex.h"


// CTabFourthDialog ��ȭ �����Դϴ�.

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


// CTabFourthDialog �޽��� ó�����Դϴ�.


BOOL CTabFourthDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	CRect rect;
	m_AnalysisDataList.GetClientRect(&rect);

	m_AnalysisDataList.DeleteAllItems();

	m_AnalysisDataList.InsertColumn(0, "����", LVCFMT_CENTER, 150, -1);
	m_AnalysisDataList.InsertColumn(1, "�", LVCFMT_CENTER, 110, -1);
	m_AnalysisDataList.InsertColumn(2, "���", LVCFMT_CENTER, 110, -1);
	m_AnalysisDataList.InsertColumn(3, "���", LVCFMT_CENTER, 100, -1);//����/����
	m_AnalysisDataList.InsertColumn(4, "��������", LVCFMT_CENTER, 300, -1);//���нÿ���������
	m_AnalysisDataList.InsertColumn(5, "�˰���󼼺���", LVCFMT_CENTER, 250, -1);
	m_AnalysisDataList.InsertColumn(6, "���庸��", LVCFMT_CENTER, 100, -1);

	m_AnalysisDataList.SetExtendedStyle(m_AnalysisDataList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_SummaryData.ShowScrollBar(SB_VERT, TRUE);
	
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
						// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CTabFourthDialog::OnNMClickAnalysisList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
		if(pNMItemActivate->iSubItem == 4)//��������
		{
		}
		else if (pNMItemActivate->iSubItem == 5)//�˰��򺸱�
		{
		}
		else if (pNMItemActivate->iSubItem == 6)//���庸��
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
	
	
	
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}

//���¼� �м� ��� ���� ���� �� ���� �˻��� ���� ��� ���� ���� �ϴ� ���
void CTabFourthDialog::OnBnClickedAnalysisSaveButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	char programpath[_MAX_PATH];
	//GetModuleFileName(NULL, programpath, _MAX_PATH);
	//GetCurrentDirectory(_MAX_PATH, programpath);//���� ���� ���
	strcpy(programpath, "D:\\�����,������,����ö");
	CString strFileName = "";
	CTime cTime = CTime::GetCurrentTime(); // ���� �ý������κ��� ��¥ �� �ð��� ��� �´�.
	CString strDate, strTime; // ��ȯ�Ǵ� ��¥�� �ð��� ������ CString ���� ����
	strDate.Format("%04d_%02d_%02d_", cTime.GetYear(),cTime.GetMonth(),cTime.GetDay()); // ���� ����� ��ȯ
	strTime.Format("%02d%02d%02d", cTime.GetHour(),cTime.GetMinute(),cTime.GetSecond()); // ���� �ú��� ��ȯ
	strFileName.Format("%s%s%s%s%s", programpath,"\\", strDate,strTime,"_���¼Һм����[����_999].txt");

	
	
	m_ResultSearchWord.SetWindowText("������,�����,����ö");//�ӽð� �ο�
	BOOL bDisplaySummary = FALSE;
	CString strSearchWord = "";
	m_ResultSearchWord.GetWindowText(strSearchWord);
	strSearchWord.Replace(" ", NULL);//��������
	int nSplitNumToken = 0;
	CString * splitWordTokenList = NULL;//���ڿ� �и� ���� ����
	int* splitWordTokenFindNumberList = NULL;//���ڿ��� ã�� ���� ����

	//�˻���ã�� ������ ���� �� ó��
	if (strSearchWord != "")
	{
		bDisplaySummary = TRUE;
		splitWordTokenList = Split(strSearchWord, ',', nSplitNumToken);//�� ���� ������ ��� �� ��
		splitWordTokenFindNumberList = new int[nSplitNumToken];
		for (int i = 0; i < nSplitNumToken; i++)
		{
			splitWordTokenFindNumberList[i] = 0;//�ʱ�ȭ
		}
	}
	
	CStdioFile fileWrite;
	//if (fileWrite.Open("D:\\fileWrite.txt", CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite))
	if (fileWrite.Open(strFileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite))
	{
		//Ÿ��Ʋ ����	
		fileWrite.SeekToEnd();
		CString strHeadResult = "";
		strHeadResult = "����\t\t\t\t�\t\t\t���\n";
		fileWrite.WriteString(strHeadResult);
		for (int nIndex = 0; nIndex < m_AnalysisDataList.GetItemCount(); nIndex++)
		{
			//ù��° �÷��� ���鼭 �Է��� ���� ���� ���� ã���ϴ�.
			CString strResult = "";
			CString strOrg = m_AnalysisDataList.GetItemText(nIndex, 0);
			CString strStem = m_AnalysisDataList.GetItemText(nIndex, 1);
			CString strEnding = m_AnalysisDataList.GetItemText(nIndex, 2);

			//������� ����
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
			//������ �� ��� �� ������ ���߱� ���� ���Ƿ� ������
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
			strResult += "����� �˻��� ���� ��� ����";
			strResult += "\n\n=============================================================================\n";
			fileWrite.WriteString(strResult);
			for (int i = 0; i < nSplitNumToken; i++)
			{
				fileWrite.SeekToEnd();
				CString strResult = "";
				strResult.Format("%s\t\t\t\t%d%s", splitWordTokenList[i], splitWordTokenFindNumberList[i],"\n");
				fileWrite.WriteString(strResult);
			}
			MessageBox("���ϻ����Ϸ�", "������ ���������� �����Ǿ����ϴ�", MB_OK);
		}
	}
	fileWrite.Close();

	//����Ʈ�迭 ����
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


