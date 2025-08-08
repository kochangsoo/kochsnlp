// TabFirstDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "EngineX.h"
#include "TabFirstDialog.h"
#include "afxdialogex.h"

#include "common.h" //������ ����
#include "StemAmbFeatureDialog.h" //���Ǽ���������
#include "MainFrm.h"

// CTabFirstDialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CTabFirstDialog, CDialog)

CTabFirstDialog::CTabFirstDialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_TAB_1_DIALOG, pParent)
{
	m_imeMode = 0;
	m_stemEntry = NULL;
	m_bStemModify = FALSE;
}

CTabFirstDialog::~CTabFirstDialog()
{
}

void CTabFirstDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTabFirstDialog, CDialog)
	ON_BN_CLICKED(IDC_STEM_FIND_BTN, &CTabFirstDialog::OnBnClickedStemFindBtn)
	ON_BN_CLICKED(IDC_STEM_ADD_BTN, &CTabFirstDialog::OnBnClickedStemAddBtn)
	ON_BN_CLICKED(IDC_STEM_MODIFY_BTN, &CTabFirstDialog::OnBnClickedStemModifyBtn)
	ON_BN_CLICKED(IDC_STME_DELETE_BTN, &CTabFirstDialog::OnBnClickedStmeDeleteBtn)
	ON_BN_CLICKED(IDC_STEM_CREATE_BTN, &CTabFirstDialog::OnBnClickedStemCreateBtn)
	ON_BN_CLICKED(IDC_STEM_AMB_MORE_BTN, &CTabFirstDialog::OnBnClickedStemAmbMoreBtn)
END_MESSAGE_MAP()


// CTabFirstDialog �޽��� ó�����Դϴ�.


void CTabFirstDialog::OnBnClickedStemFindBtn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrm = (CMainFrame *)AfxGetMainWnd();
	CString strWord = _T("");
	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_STEM_WORD);
	pEdit->GetWindowText(strWord);     // ���� Unicode�� ���
	if (strWord == _T("")) 
	{
		MessageBox(_T("�˻����� �Է��Ͻñ� �ٶ��ϴ�."), _T("�˻�"), MB_OK);
		pEdit->SetFocus();
	}
	else
	{
		m_strStemAmbFeature = "";//�ʱ�ȭ
		char *strEntryWord = NULL;
		strEntryWord = (char *)(const char *)strWord;
		m_stemEntry = pFrm->m_preProcessor->m_pStemEndDic->GetDicWordEntry(strEntryWord, DIC_STEM);
		if (m_stemEntry != NULL)
		{
			pEdit->SetWindowText(CA2T(m_stemEntry->getKey()));
			//�����Ѱ��� 234�� = ��������(74��) + ���Ǽ�����(160��)
			char* pBitVector = m_stemEntry->getBitVector(0);
			if (pBitVector != NULL)
			{
				for (int i = 0; i < STEM_DIC_BASIC_FEATURE_NUMBER; i++)
				{
					CButton *pButton = (CButton*)GetDlgItem(10000 + i);
					if (*(pBitVector + i) == '1')
						pButton->SetCheck(1);
					else
						pButton->SetCheck(0);
				}
				//���Ǽ� ���� ���� 160��
				m_strStemAmbFeature = pBitVector + STEM_DIC_BASIC_FEATURE_NUMBER;
			}
		}
		else
		{
			InitStemAmbFeature();
			m_bStemModify = FALSE;
			MessageBox(_T("���ۻ������� �ܾ ã�� ���߽��ϴ�."), _T("�˻�����"), MB_OK);
		}
	}
}


void CTabFirstDialog::OnBnClickedStemAddBtn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrm = (CMainFrame *)AfxGetMainWnd();
	CString strWord = _T("");
	//CNAEntry* entry = NULL;
	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_STEM_WORD);
	pEdit->GetWindowText(strWord);     // ���� Unicode�� ���
	if (strWord == _T("")) {
		MessageBox(_T("�˻����� �Է��Ͻñ� �ٶ��ϴ�."), _T("�˻�"), MB_OK);
		pEdit->SetFocus();
	}
	else
	{
		m_bStemModify = TRUE;
		CString s = _T("");
		s = strWord + _T("�� ������ �ű� �߰��Ͻðڽ��ϱ�?");
		if (AfxMessageBox(s, MB_YESNO) == IDYES)
		{
			char *strEntryWord = NULL;
			strEntryWord = (char *)(const char *)strWord;
			CNAEntry* pExistStemWord = pFrm->m_preProcessor->m_pStemEndDic->GetDicWordEntry(strEntryWord, DIC_STEM);
			if (pExistStemWord != NULL)
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
				//pAddEntry->setString(CStringUtil::ConvertUnicodeToMultybyte(strWord), vector);
				pAddEntry->setString(pAddWord, vector);
				
				BOOL bEntryAdd = pFrm->m_preProcessor->m_pStemEndDic->AddDicWordEntry(pAddEntry, DIC_STEM);
				if (bEntryAdd)
				{
					AfxMessageBox(_T("'" + strWord + "' �ܾ [�޸𸮿���������] �߰��Ͽ����ϴ�."));
					m_stemEntry = pAddEntry;
				}
				delete[] vector;
				delete pAddWord;
				vector = NULL;
				pAddWord = NULL;

			}
		}
	}
}


void CTabFirstDialog::OnBnClickedStemModifyBtn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (m_stemEntry != NULL)
	{
		m_bStemModify = TRUE;
		char* vector = new char[STEM_DIC_ALL_FEATURE_NUMBER];
		memset(vector, 0x00, STEM_DIC_ALL_FEATURE_NUMBER);
		GetBitVector(vector);
		m_stemEntry->SetBitVector(0, vector);
		delete[] vector;
		vector = NULL;
		MessageBox(_T("�ش� �ܾ��� ���� ������ [�޸𸮿���������]�����Ͽ����ϴ�."), _T("�����Ϸ�"), MB_ICONERROR);
	}
	else
	{
		m_bStemModify = FALSE;
		MessageBox(_T("������ ������ �������� �ʽ��ϴ�."), _T("��������"), MB_ICONERROR);
	}
}


void CTabFirstDialog::OnBnClickedStmeDeleteBtn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrm = (CMainFrame *)AfxGetMainWnd();
	if (m_stemEntry != NULL)
	{
		m_bStemModify = TRUE;
		char* key = new char[1024];
		memset(key, 0x00, 1024);
		m_stemEntry->getKey(key);
		CString s = _T("");
		s.Format(_T("'%s' �ܾ ������ �����ϰڽ��ϱ�?"), CA2T(key));
		if (AfxMessageBox(s, MB_YESNO) == IDYES)
		{
			pFrm->m_preProcessor->m_pStemEndDic->RemoveDicWordEntry(m_stemEntry, DIC_STEM);
			m_stemEntry = NULL;
			InitStemAmbFeature();
		}
		delete[] key;
		key = NULL;
	}
}

void CTabFirstDialog::OnBnClickedStemCreateBtn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	bool bFileSave = false;
	CString strCreateFileName;
	CMainFrame *pFrm = (CMainFrame *)AfxGetMainWnd();
	if (m_bStemModify == TRUE)
	{
		if (GetFileAttributes(pFrm->m_strExePath + "\\Dictionary\\Backup") == FILE_ATTRIBUTE_DIRECTORY)//������ ���丮 Ȯ��
		{
			//CopyFile(���� ���� ���, ��� ���� ���, ����� ����);
			//����⿩�δ� TRUE�� fail, FALSE�� �����
			CTime cTime = CTime::GetCurrentTime(); // ���� �ý������κ��� ��¥ �� �ð��� ��� �´�.
			CString strDate, strTime; // ��ȯ�Ǵ� ��¥�� �ð��� ������ CString ���� ����
			strDate.Format("%04d_%02d_%02d_%02d_%02d_%02d", cTime.GetYear(), cTime.GetMonth(), cTime.GetDay(), cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond());
				if (pFrm->m_preProcessor->m_pStemEndDic->m_pStemDictionary != NULL)
				{
					//�������� �����Ͽ� Backup ���丮�� �����Ѵ�.
					CopyFile(pFrm->m_strExePath + "\\Dictionary\\" + pFrm->m_preProcessor->m_pStemEndDic->m_strStemDicFileName, pFrm->m_strExePath + "\\Dictionary\\backup\\" + strDate + "_" + pFrm->m_preProcessor->m_pStemEndDic->m_strStemDicFileName, FALSE);
					strCreateFileName = pFrm->m_strExePath + "\\Dictionary\\" + pFrm->m_preProcessor->m_pStemEndDic->m_strStemDicFileName;
					if (pFrm->m_preProcessor->m_pStemEndDic->m_nEncryption == 0)
					{
						//��ȣȭ�����ʰ� ���� ���� �� ���� ���� �ٸ��̸����� ����
						if (pFrm->m_preProcessor->m_pStemEndDic->m_pStemDictionary->storeDictionary(pFrm->m_preProcessor->m_pStemEndDic->m_strStemDicFileName, DIC_STEM, 0) == 0)
						{
							bFileSave = true;
						}
					}
					else
					{
						//��ȣȭ�Ͽ� ���� ���� �� ���� ���� �ٸ��̸����� ����
						if (pFrm->m_preProcessor->m_pStemEndDic->m_pStemDictionary->encStoreDictionary(pFrm->m_preProcessor->m_pStemEndDic->m_strStemDicFileName, DIC_STEM, 0) == 0)
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
		if(bFileSave == true)
		{
			AfxMessageBox("�������� ����");
			InitStemAmbFeature();
		m_bStemModify = FALSE;
		}
	}
}


void CTabFirstDialog::OnBnClickedStemAmbMoreBtn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CStemAmbFeatureDialog _stemAmbFeatureDialog;
	// StemAmbFeatureDialog ��ȭ���ڰ� �����Ǳ� ���� �Ѱ��� �����͸� �Ѱ��־�� �Ѵ�.
	// ��ȭ���ڰ� �����Ǹ鼭 ȣ��Ǵ� WM_INITDIALOG �޽������� �ش� ��ȭ������ ��Ʈ�ѿ� ���� �������ټ� �ִ�.
	_stemAmbFeatureDialog.SetAmbFeatureData(m_strStemAmbFeature);
	int return_value = _stemAmbFeatureDialog.DoModal();
	if (IDOK == return_value || IDCANCEL == return_value) {
		// StemAmbFeatureDialog ��ȭ������ "Ȯ��" ��ư�� ������ DoModal �Լ��� IDOK��� ���� ��ȯ�ϰ� �ȴ�.
		// StemAmbFeatureDialog�� ������� ���� ��� ������ ������ ���� �̿��ؾ��Ѵ�.
		_stemAmbFeatureDialog.GetAmbFeatureData(m_strStemAmbFeature);
	}
}


BOOL CTabFirstDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	m_toolTip.Create(GetActiveWindow(), TTS_ALWAYSTIP | TTS_BALLOON);
	m_toolTip.SetDelayTime(TTDT_INITIAL, 500);
	m_toolTip.SetMaxTipWidth(400);
	//���� ����
	for (int i = 0; i < STEM_DIC_BASIC_FEATURE_NUMBER; i++) {
		//IDC_CHECK0 ���� 10000�� resource.h�� �� ������
		CString str(STEM_FEATURE_DESCRIPT[i]);
		m_toolTip.AddTool(GetDlgItem(10000 + i), str);
	}
	m_toolTip.Activate(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


BOOL CTabFirstDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	m_toolTip.RelayEvent(pMsg);
	//IME�� �ѱ� ��尡 �ƴ� ���
	if (m_imeMode == 0)
	{
		SetCurrentInputMode(1);
	}

	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_STEM_WORD)->m_hWnd) {
		// ������ Ű�� ����Ű�ΰ�? 
		if (pMsg->wParam == VK_RETURN)
		{
			// ����Ű �Է� �� �ϰ��� �ϴ� ��� �ڵ� �߰�. 
			OnBnClickedStemFindBtn();
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

// ���� IME�� �Է� ��带 ��ȯ�Ѵ�. 0: ����, 1: �ѱ�
char CTabFirstDialog::GetCurrentInputMode()
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
void CTabFirstDialog::SetCurrentInputMode(char parm_mode)
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

void CTabFirstDialog::GetBitVector(char* pString)
{
	if (pString != NULL)
	{
		int nStemBasicLen = 0;
		for (int i = 0; i < STEM_DIC_BASIC_FEATURE_NUMBER; i++)
		{
			CButton *pButton = (CButton*)GetDlgItem(10000 + i);
			if (pButton->GetCheck() == BST_CHECKED)
			{
				pString[nStemBasicLen++] = '1';
			}
			else
			{
				pString[nStemBasicLen++] = '0';
			}
		}
		char ambArrayFeature[161] = { 0 };
		size_t CharactersConverted = 0;
		if (ambArrayFeature == "") { InitStemAmbFeature(); }
		strcat(pString, ambArrayFeature);
		pString[nStemBasicLen + STEM_DIC_AMB_FEATURE_NUMBER] = 0;
		
	}
}

void CTabFirstDialog::InitStemAmbFeature()
{
	m_strStemAmbFeature = "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
}