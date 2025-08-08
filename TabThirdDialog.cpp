// TabThirdDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "EngineX.h"
#include "TabThirdDialog.h"
#include "afxdialogex.h"

#include "MainFrm.h"
#include "common.h" //������ ����


// CTabThirdDialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CTabThirdDialog, CDialog)

CTabThirdDialog::CTabThirdDialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_TAB_3_DIALOG, pParent)
{
	m_imeMode = 0;
	m_userEntry = NULL;
	m_bUserModify = FALSE;
}

CTabThirdDialog::~CTabThirdDialog()
{
}

void CTabThirdDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTabThirdDialog, CDialog)
	ON_BN_CLICKED(IDC_USER_FIND_BTN, &CTabThirdDialog::OnBnClickedUserFindBtn)
	ON_BN_CLICKED(IDC_USER_ADD_BTN, &CTabThirdDialog::OnBnClickedUserAddBtn)
	ON_BN_CLICKED(IDC_USER_MODIFY_BTN, &CTabThirdDialog::OnBnClickedUserModifyBtn)
	ON_BN_CLICKED(IDC_USER_DELETE_BTN, &CTabThirdDialog::OnBnClickedUserDeleteBtn)
	ON_BN_CLICKED(IDC_USER_CREATE_BTN, &CTabThirdDialog::OnBnClickedUserCreateBtn)
END_MESSAGE_MAP()


// CTabThirdDialog �޽��� ó�����Դϴ�.


void CTabThirdDialog::OnBnClickedUserFindBtn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrm = (CMainFrame *)AfxGetMainWnd();
	CString strWord = _T("");
	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_USER_WORD);
	pEdit->GetWindowText(strWord);     // ���� Unicode�� ���
	if (strWord == _T("")) {
		MessageBox(_T("�˻����� �Է��Ͻñ� �ٶ��ϴ�."), _T("�˻�"), MB_OK);
		pEdit->SetFocus();
	}
	else
	{
		char *strEntryWord = NULL;
		strEntryWord = (char *)(const char *)strWord;
		m_userEntry = pFrm->m_preProcessor->m_pStemEndDic->GetDicWordEntry(strEntryWord, DIC_USER);
		if (m_userEntry != NULL)
		{
			pEdit->SetWindowText(CA2T(m_userEntry->getKey()));
			char* pBitVector = m_userEntry->getBitVector(0);
			if (pBitVector != NULL)
			{
				for (int i = 0; i < USER_DIC_BASIC_FEATURE_NUMBER; i++)
				{
					CButton *pButton = (CButton*)GetDlgItem(40000 + i);
					if (*(pBitVector + i) == '1')
						pButton->SetCheck(1);
					else
						pButton->SetCheck(0);
				}
			}
		}
		else
		{
			m_bUserModify = FALSE;
			MessageBox(_T("��������ǻ������� �ܾ ã�� ���߽��ϴ�."), _T("�˻�����"), MB_OK);
		}
	}
}


void CTabThirdDialog::OnBnClickedUserAddBtn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrm = (CMainFrame *)AfxGetMainWnd();
	CString strWord = _T("");
	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_USER_WORD);
	pEdit->GetWindowText(strWord); // ���� Unicode�� ���
	if (strWord == _T("")) {
		MessageBox(_T("�˻����� �Է��Ͻñ� �ٶ��ϴ�."), _T("�˻�"), MB_OK);
		pEdit->SetFocus();
	}
	else
	{
		m_bUserModify = TRUE;
		CString s = _T("");
		s = strWord + _T("�� ������ �ű� �߰��Ͻðڽ��ϱ�?");
		if (AfxMessageBox(s, MB_YESNO) == IDYES)
		{
			char *strEntryWord = NULL;
			strEntryWord = (char *)(const char *)strWord;
			CNAEntry* pExistUserWord = pFrm->m_preProcessor->m_pStemEndDic->GetDicWordEntry(strEntryWord, DIC_USER);
			if (pExistUserWord != NULL)
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

				BOOL bEntryAdd = pFrm->m_preProcessor->m_pStemEndDic->AddDicWordEntry(pAddEntry, DIC_USER);
				if (bEntryAdd)
				{
					AfxMessageBox(_T("'" + strWord + "' �ܾ [�޸𸮿���������] �߰��Ͽ����ϴ�."));
					m_userEntry = pAddEntry;
				}
				delete[] vector;
				delete pAddWord;
				vector = NULL;
				pAddWord = NULL;

			}
		}
	}
}


void CTabThirdDialog::OnBnClickedUserModifyBtn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (m_userEntry != NULL)
	{
		m_bUserModify = TRUE;
		char* vector = new char[USER_DIC_BASIC_FEATURE_NUMBER];
		memset(vector, 0x00, USER_DIC_BASIC_FEATURE_NUMBER);
		GetBitVector(vector);
		m_userEntry->SetBitVector(0, vector);
		delete[] vector;
		vector = NULL;
		MessageBox(_T("�ش� �ܾ��� ���� ������ [�޸𸮿���������]�����Ͽ����ϴ�."), _T("�����Ϸ�"), MB_ICONERROR);
	}
	else
	{
		m_userEntry = FALSE;
		MessageBox(_T("������ ������ �������� �ʽ��ϴ�."), _T("��������"), MB_ICONERROR);
	}
}


void CTabThirdDialog::OnBnClickedUserDeleteBtn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrm = (CMainFrame *)AfxGetMainWnd();
	if (m_userEntry != NULL)
	{
		m_bUserModify = TRUE;
		char* key = new char[1024];
		memset(key, 0x00, 1024);
		m_userEntry->getKey(key);
		CString s = _T("");
		s.Format(_T("'%s' �ܾ ������ �����ϰڽ��ϱ�?"), CA2T(key));
		if (AfxMessageBox(s, MB_YESNO) == IDYES)
		{
			pFrm->m_preProcessor->m_pStemEndDic->RemoveDicWordEntry(m_userEntry, DIC_USER);
			m_userEntry = NULL;
		}
		delete[] key;
		key = NULL;
	}
}


void CTabThirdDialog::OnBnClickedUserCreateBtn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	bool bFileSave = false;
	CString strCreateFileName;
	CMainFrame *pFrm = (CMainFrame *)AfxGetMainWnd();
	if (m_bUserModify == TRUE)
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
				CopyFile(pFrm->m_strExePath + "\\Dictionary\\" + pFrm->m_preProcessor->m_pStemEndDic->m_strUserDicFileName, pFrm->m_strExePath + "\\Dictionary\\backup\\" + strDate + "_" + pFrm->m_preProcessor->m_pStemEndDic->m_strUserDicFileName, FALSE);
				strCreateFileName = pFrm->m_strExePath + "\\Dictionary\\" + pFrm->m_preProcessor->m_pStemEndDic->m_strUserDicFileName;
				if (pFrm->m_preProcessor->m_pStemEndDic->m_nEncryption == 0)
				{
					//��ȣȭ�����ʰ� ���� ���� �� ���� ���� �ٸ��̸����� ����
					if (pFrm->m_preProcessor->m_pStemEndDic->m_pUserDictionary->storeDictionary(pFrm->m_preProcessor->m_pStemEndDic->m_strUserDicFileName, DIC_USER, 0) == 0)
					{
						bFileSave = true;
					}
				}
				else
				{
					//��ȣȭ�Ͽ� ���� ���� �� ���� ���� �ٸ��̸����� ����
					if (pFrm->m_preProcessor->m_pStemEndDic->m_pUserDictionary->encStoreDictionary(pFrm->m_preProcessor->m_pStemEndDic->m_strUserDicFileName, DIC_USER, 0) == 0)
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
			m_bUserModify = FALSE;
		}
	}
}


BOOL CTabThirdDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	m_toolTip.RelayEvent(pMsg);

	//IME�� �ѱ� ��尡 �ƴ� ���
	if (m_imeMode == 0)
	{
		SetCurrentInputMode(1);
	}

	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_USER_WORD)->m_hWnd) {
		// ������ Ű�� ����Ű�ΰ�? 
		if (pMsg->wParam == VK_RETURN)
		{
			// ����Ű �Է� �� �ϰ��� �ϴ� ��� �ڵ� �߰�. 
			OnBnClickedUserFindBtn();
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}


BOOL CTabThirdDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_toolTip.Create(GetActiveWindow(), TTS_ALWAYSTIP | TTS_BALLOON);

	m_toolTip.SetDelayTime(TTDT_INITIAL, 500);

	m_toolTip.SetMaxTipWidth(400);

	//���� ����
	for (int i = 0; i < USER_DIC_BASIC_FEATURE_NUMBER; i++) {
		//resource.h�� ���������� üũ�ڽ��� 30000���� �� ������
		CString str(USER_FEATURE_DESCRIPT[i]);
		m_toolTip.AddTool(GetDlgItem(40000 + i), str);
	}

	m_toolTip.Activate(TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

// ���� IME�� �Է� ��带 ��ȯ�Ѵ�. 0: ����, 1: �ѱ�
char CTabThirdDialog::GetCurrentInputMode()
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
void CTabThirdDialog::SetCurrentInputMode(char parm_mode)
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

void CTabThirdDialog::GetBitVector(char* pString)
{
	if (pString != NULL)
	{
		int nUserBasicLen = 0;
		for (int i = 0; i < USER_DIC_BASIC_FEATURE_NUMBER; i++)
		{
			CButton *pButton = (CButton*)GetDlgItem(40000 + i);
			if (pButton->GetCheck() == BST_CHECKED)
			{
				pString[nUserBasicLen++] = '1';
			}
			else
			{
				pString[nUserBasicLen++] = '0';
			}
		}
		pString[nUserBasicLen + USER_DIC_BASIC_FEATURE_NUMBER] = 0;

	}
}