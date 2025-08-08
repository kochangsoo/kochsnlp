// TabSecondDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "EngineX.h"
#include "TabSecondDialog.h"
#include "afxdialogex.h"

#include "MainFrm.h"
#include "common.h" //������ ����

// CTabSecondDialog ��ȭ �����Դϴ�.

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


// CTabSecondDialog �޽��� ó�����Դϴ�.


void CTabSecondDialog::OnBnClickedEndingFindBtn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrm = (CMainFrame *)AfxGetMainWnd();
	CString strWord = _T("");
	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_ENDING_WORD);
	pEdit->GetWindowText(strWord);     // ���� Unicode�� ���
	if (strWord == _T("")) {
		MessageBox(_T("�˻����� �Է��Ͻñ� �ٶ��ϴ�."), _T("�˻�"), MB_OK);
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
			MessageBox(_T("������������ �ܾ ã�� ���߽��ϴ�."), _T("�˻�����"), MB_OK);
		}
	}
}


void CTabSecondDialog::OnBnClickedEndingAddBtn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrm = (CMainFrame *)AfxGetMainWnd();
	CString strWord = _T("");
	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_ENDING_WORD);
	pEdit->GetWindowText(strWord); // ���� Unicode�� ���
	if (strWord == _T("")) {
		MessageBox(_T("�˻����� �Է��Ͻñ� �ٶ��ϴ�."), _T("�˻�"), MB_OK);
		pEdit->SetFocus();
	}
	else
	{
		m_bEndingModify = TRUE;
		CString s = _T("");
		s = strWord + _T("�� ������ �ű� �߰��Ͻðڽ��ϱ�?");
		if (AfxMessageBox(s, MB_YESNO) == IDYES)
		{
			char *strEntryWord = NULL;
			strEntryWord = (char *)(const char *)strWord;
			CNAEntry* pExistEndingWord = pFrm->m_preProcessor->m_pStemEndDic->GetDicWordEntry(strEntryWord, DIC_ENDING);
			if (pExistEndingWord != NULL)
			{
				AfxMessageBox(_T("'" + strWord + "' �ܾ �����մϴ�."));
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
					AfxMessageBox(_T("'" + strWord + "' �ܾ [�޸𸮿���������] �߰��Ͽ����ϴ�."));
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (m_endingEntry != NULL)
	{
		m_bEndingModify = TRUE;
		char* vector = new char[ENDING_DIC_BASIC_FEATURE_NUMBER];
		memset(vector, 0x00, ENDING_DIC_BASIC_FEATURE_NUMBER);
		GetBitVector(vector);
		m_endingEntry->SetBitVector(0, vector);
		delete[] vector;
		vector = NULL;
		MessageBox(_T("�ش� �ܾ��� ���� ������ [�޸𸮿���������]�����Ͽ����ϴ�."), _T("�����Ϸ�"), MB_ICONERROR);
	}
	else
	{
		m_endingEntry = FALSE;
		MessageBox(_T("������ ������ �������� �ʽ��ϴ�."), _T("��������"), MB_ICONERROR);
	}
}


void CTabSecondDialog::OnBnClickedEndingDeleteBtn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrm = (CMainFrame *)AfxGetMainWnd();
	if (m_endingEntry != NULL)
	{
		m_bEndingModify = TRUE;
		char* key = new char[1024];
		memset(key, 0x00, 1024);
		m_endingEntry->getKey(key);
		CString s = _T("");
		s.Format(_T("'%s' �ܾ ������ �����ϰڽ��ϱ�?"), CA2T(key));
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	bool bFileSave = false;
	CString strCreateFileName;
	CMainFrame *pFrm = (CMainFrame *)AfxGetMainWnd();
	if (m_bEndingModify == TRUE)
	{
		if (GetFileAttributes(pFrm->m_strExePath + "\\Dictionary\\Backup") == FILE_ATTRIBUTE_DIRECTORY)//������ ���丮 Ȯ��
		{
			//CopyFile(���� ���� ���, ��� ���� ���, ����� ����);
			//����⿩�δ� TRUE�� fail, FALSE�� �����
			CTime cTime = CTime::GetCurrentTime(); // ���� �ý������κ��� ��¥ �� �ð��� ��� �´�.
			CString strDate, strTime; // ��ȯ�Ǵ� ��¥�� �ð��� ������ CString ���� ����
			strDate.Format("%04d_%02d_%02d_%02d_%02d_%02d", cTime.GetYear(), cTime.GetMonth(), cTime.GetDay(), cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond());
			if (pFrm->m_preProcessor->m_pStemEndDic->m_pEndingDictionary != NULL)
			{
				//�������� �����Ͽ� Backup ���丮�� �����Ѵ�.
				CopyFile(pFrm->m_strExePath + "\\Dictionary\\" + pFrm->m_preProcessor->m_pStemEndDic->m_strEndingDicFileName, pFrm->m_strExePath + "\\Dictionary\\backup\\" + strDate + "_" + pFrm->m_preProcessor->m_pStemEndDic->m_strEndingDicFileName, FALSE);
				strCreateFileName = pFrm->m_strExePath + "\\Dictionary\\" + pFrm->m_preProcessor->m_pStemEndDic->m_strEndingDicFileName;
				if (pFrm->m_preProcessor->m_pStemEndDic->m_nEncryption == 0)
				{
					//��ȣȭ�����ʰ� ���� ���� �� ���� ���� �ٸ��̸����� ����
					if (pFrm->m_preProcessor->m_pStemEndDic->m_pEndingDictionary->storeDictionary(pFrm->m_preProcessor->m_pStemEndDic->m_strEndingDicFileName, DIC_ENDING, 0) == 0)
					{
						bFileSave = true;
					}
				}
				else
				{
					//��ȣȭ�Ͽ� ���� ���� �� ���� ���� �ٸ��̸����� ����
					if (pFrm->m_preProcessor->m_pStemEndDic->m_pEndingDictionary->encStoreDictionary(pFrm->m_preProcessor->m_pStemEndDic->m_strEndingDicFileName, DIC_ENDING, 0) == 0)
					{
						bFileSave = true;
					}
				}
			}
		}
		else
		{
			AfxMessageBox("����� ���丮�� �������� �ʽ��ϴ�.");
		}
		if (bFileSave == true)
		{
			AfxMessageBox("�������� ����");
			m_bEndingModify = FALSE;
		}
	}
}


BOOL CTabSecondDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_toolTip.Create(GetActiveWindow(), TTS_ALWAYSTIP | TTS_BALLOON);

	m_toolTip.SetDelayTime(TTDT_INITIAL, 500);

	m_toolTip.SetMaxTipWidth(400);

	//���� ����
	for (int i = 0; i < ENDING_DIC_BASIC_FEATURE_NUMBER; i++) {
		//resource.h�� ���������� üũ�ڽ��� 30000���� �� ������
		CString str(ENDING_FEATURE_DESCRIPT[i]);
		m_toolTip.AddTool(GetDlgItem(30000 + i), str);
	}

	m_toolTip.Activate(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


BOOL CTabSecondDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	m_toolTip.RelayEvent(pMsg);

	//IME�� �ѱ� ��尡 �ƴ� ���
	if (m_imeMode == 0)
	{
		SetCurrentInputMode(1);
	}

	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_ENDING_WORD)->m_hWnd) {
		// ������ Ű�� ����Ű�ΰ�? 
		if (pMsg->wParam == VK_RETURN)
		{
			// ����Ű �Է� �� �ϰ��� �ϴ� ��� �ڵ� �߰�. 
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

// ���� IME�� �Է� ��带 ��ȯ�Ѵ�. 0: ����, 1: �ѱ�
char CTabSecondDialog::GetCurrentInputMode()
{
	DWORD conv, sentence;

	HIMC h_imc = ImmGetContext(m_hWnd);
	// IME(input method editor) ���¸� ��´�.
	ImmGetConversionStatus(h_imc, &conv, &sentence);

	ImmReleaseContext(m_hWnd, h_imc);

	if (conv & IME_CMODE_LANGUAGE) return 1;  // �ѱ� ������... 

	return 0;
}

// ���� IME�� �Է� ��带 �����Ѵ�. 0: ����, 1: �ѱ�
void CTabSecondDialog::SetCurrentInputMode(char parm_mode)
{
	DWORD conv, sentence;

	HIMC h_imc = ImmGetContext(m_hWnd);
	// IME(input method editor) ���¸� ��´�.
	ImmGetConversionStatus(h_imc, &conv, &sentence);

	if (parm_mode == 1) {
		// ���� �Է� ���¿��ٸ� ���¸� �����Ѵ�.
		conv = conv & (~IME_CMODE_CHARCODE);
		// �ѱ� �Է� ���·� �����Ѵ�.
		conv = conv | IME_CMODE_LANGUAGE;
		m_imeMode = 1;
	}
	else {
		// �ѱ� �Է� ���¿��ٸ� ���¸� �����Ѵ�.
		conv = conv & (~IME_CMODE_LANGUAGE);
		// ���� �Է� ���·� �����Ѵ�.
		conv = conv | IME_CMODE_CHARCODE;
		m_imeMode = 0;
	}

	// IME ���¸� �����Ѵ�.
	ImmSetConversionStatus(h_imc, conv, sentence);
	ImmReleaseContext(m_hWnd, h_imc);


}