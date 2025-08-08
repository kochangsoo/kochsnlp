// TabFifthDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "EngineX.h"
#include "TabFifthDialog.h"
#include "afxdialogex.h"

#include "common.h" //������ ����

// CTabFifthDialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CTabFifthDialog, CDialog)

CTabFifthDialog::CTabFifthDialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_TAB_5_DIALOG, pParent)
{
	m_pte_data = NULL;
}

CTabFifthDialog::~CTabFifthDialog()
{
}

void CTabFifthDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATISTICE_EDIT, m_StatisticsData);
	DDX_Control(pDX, IDC_STATISTICS_COMBO, m_StatisticsCombo);
	DDX_Control(pDX, IDC_VIEW_COMBO, m_ViewCombo);
	DDX_Control(pDX, IDC_FIND_EDIT, m_FindEdit);
	DDX_Control(pDX, IDC__STATISTICS_LIST, m_StatisticsDataList);
}


BEGIN_MESSAGE_MAP(CTabFifthDialog, CDialog)
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_STATISTICS_COMBO, &CTabFifthDialog::OnCbnSelchangeStatisticsCombo)
	ON_CBN_SELCHANGE(IDC_VIEW_COMBO, &CTabFifthDialog::OnCbnSelchangeViewCombo)
END_MESSAGE_MAP()


// CTabFifthDialog �޽��� ó�����Դϴ�.


void CTabFifthDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CRect rect;
	GetClientRect(&rect);
	if (m_StatisticsData.GetSafeHwnd() && m_StatisticsDataList.GetSafeHwnd())
	{
		m_StatisticsData.MoveWindow(rect.left + 11, rect.top + 40, 250, rect.bottom - 45);
		m_StatisticsDataList.MoveWindow(rect.left + 265, rect.top + 40, rect.right - 250, rect.bottom - 45);
	}
}


BOOL CTabFifthDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	
	//���
	m_StatisticsCombo.AddString("��ü");
	m_StatisticsCombo.AddString("�⺻");
	m_StatisticsCombo.AddString("ǰ��");
	m_StatisticsCombo.AddString("�ߺ�����");
	m_StatisticsCombo.SetCurSel(0);
	
	//����
	m_ViewCombo.AddString("��ü");
	m_ViewCombo.AddString("�����");
	m_ViewCombo.AddString("�������");
	m_ViewCombo.AddString("���");
	m_ViewCombo.AddString("����");
	m_ViewCombo.AddString("����");
	m_ViewCombo.AddString("����");
	m_ViewCombo.AddString("�����");
	m_ViewCombo.AddString("������");
	m_ViewCombo.AddString("�λ�");
	m_ViewCombo.AddString("��ź��");
	m_ViewCombo.AddString("��ȣ");
	m_ViewCombo.AddString("����");
	m_ViewCombo.AddString("�ܱ�����");
	m_ViewCombo.AddString("�̵�Ͼ�");
	m_ViewCombo.AddString("����");
	m_ViewCombo.AddString("���");
	m_ViewCombo.AddString("�����̺���ü");
	m_ViewCombo.SetCurSel(0);


	CRect rect;
	m_StatisticsDataList.GetClientRect(&rect);
	m_StatisticsDataList.DeleteAllItems();
	m_StatisticsDataList.InsertColumn(0, "����", LVCFMT_CENTER, 250, -1);
	m_StatisticsDataList.InsertColumn(1, "ǰ��", LVCFMT_CENTER, 250, -1);
	m_StatisticsDataList.InsertColumn(2, "�󵵼�", LVCFMT_CENTER, 150, -1);
	m_StatisticsDataList.InsertColumn(3, "���������", LVCFMT_CENTER, 300, -1);
	m_StatisticsDataList.SetExtendedStyle(m_StatisticsDataList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_StatisticsData.ShowScrollBar(SB_VERT, TRUE);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

//���
void CTabFifthDialog::f_print_statistic(int view_statistic)
{
	m_StatisticsData.SetSel(0, -1, TRUE);
	m_StatisticsData.Clear();

	int pte_comp_count = 0;
	TE_SENTENCE *pte_sentence = NULL;
	TE_TOKEN *pte_token = NULL;
	TE_COMP_TOKEN *pte_comp = NULL;
	char ttt[1024];
	memset(ttt, 0x00, 1024);
	CString str = "";

	if (m_pte_data == NULL)
		return;
	

	
	if (view_statistic == _VIEW_STATISTIC_ALL)
	{
		str += "====��ü���====\r\n";

		sprintf(ttt,
			"\r\n ����: %d"
			"\r\n ����: %d"
			"\r\n �����: %d"
			"\r\n �������: %d"

			"\r\n\r\n ���վ�: %d"
			

			"\r\n\r\n ���: %d"
			"\r\n ����: %d"
			"\r\n ����: %d"
			"\r\n ����: %d"
			"\r\n �����: %d"
			"\r\n �λ�: %d"
			"\r\n ������: %d"
			"\r\n ��ź��: %d"
			"\r\n ����: %d"
			"\r\n �ܱ���: %d"
			"\r\n �̵�Ͼ�: %d"

			"\r\n\r\n ����: %d"
			"\r\n ���: %d"
			"\r\n ������ ����ü: %d",

			m_pte_data->_cnt_sentence,
			m_pte_data->_cnt_token,
			m_pte_data->_cnt_stem,
			m_pte_data->_cnt_end,

			m_pte_data->_cnt_comp,
		

			//stem
			m_pte_data->_cnt_noun + m_pte_data->_cnt_comp,
			m_pte_data->_cnt_numeral,
			m_pte_data->_cnt_pronoun,
			m_pte_data->_cnt_actverb,
			m_pte_data->_cnt_stateverb,
			m_pte_data->_cnt_adv,
			m_pte_data->_cnt_unc,
			m_pte_data->_cnt_interjection,
			m_pte_data->_cnt_numb,
			m_pte_data->_cnt_foreign,

			m_pte_data->_cnt_unregiste,

			//ending
			m_pte_data->_cnt_nend,
			m_pte_data->_cnt_vend,
			m_pte_data->_cnt_cend
		);

		str += ttt;

		str += "\r\n";

		str += "\r\n----�ߺ�-----\r\n";

		pte_token = m_pte_data->_pte_token_head->_next;
		while (pte_token != m_pte_data->_pte_token_tail)
		{
			if (pte_token->_same_count)
			{
				if (pte_token->_same_deputy)
				{
					memset(ttt, 0x00, 1024);
					sprintf(ttt, "\r\n ����: %s: %d", pte_token->_sztoken, pte_token->_same_count + 1);
					str += ttt;
				}
			}
			if (pte_token->_pstem != NULL)
			{
				if (pte_token->_pstem->_same_count)
				{
					if (pte_token->_pstem->_same_deputy)
					{
						memset(ttt, 0x00, 1024);
						sprintf(ttt, "\r\n �����: %s: %d", pte_token->_pstem->_szresult, pte_token->_pstem->_same_count + 1);
						str += ttt;
					}
				}
			}
			if (pte_token->_pend != NULL)
			{
				if (pte_token->_pend->_same_count)
				{
					if (pte_token->_pend->_same_deputy)
					{
						memset(ttt, 0x00, 1024);
						sprintf(ttt, "\r\n �������: %s: %d", pte_token->_pend->_szresult, pte_token->_pend->_same_count + 1);
						str += ttt;
					}
				}
			}
			pte_token = pte_token->_next;
		}


		pte_comp = m_pte_data->_pte_comp_head->_next;
		while (pte_comp != m_pte_data->_pte_comp_tail)
		{
			if (pte_comp->_same_count)
			{
				if (pte_comp->_same_deputy)
				{
					memset(ttt, 0x00, 1024);
					sprintf(ttt, "\r\n �����: %s: %d", pte_comp->_szresult, pte_comp->_same_count + 1);
					str += ttt;
				}
			}
			pte_comp = pte_comp->_next;
		}
	}
	else if (view_statistic == _VIEW_STATISTIC_NORMAL)
	{
		str += "====�⺻���====";

		sprintf(ttt,
			"\r\n\r\n ����: %d"
			"\r\n ����: %d"
			"\r\n �����: %d"
			"\r\n �������: %d"

			"\r\n\r\n ���վ�: %d",

			m_pte_data->_cnt_sentence,
			m_pte_data->_cnt_token,
			m_pte_data->_cnt_stem,
			m_pte_data->_cnt_end,

			m_pte_data->_cnt_comp

			
		);

		str += ttt;
	}
	else if (view_statistic == VIEW_STATISTIC_PUMSA)
	{
		str += "====ǰ�� ���====";

		sprintf(ttt,
			"\r\n\r\n ���: %d"
			"\r\n ����: %d"
			"\r\n ����: %d"
			"\r\n ����: %d"
			"\r\n �����: %d"
			"\r\n �λ�: %d"
			"\r\n ������: %d"
			"\r\n ��ź��: %d"
			"\r\n ����: %d"
			"\r\n �ܱ���: %d"
			"\r\n �̵�Ͼ�: %d"

			"\r\n\r\n ����: %d"
			"\r\n ���: %d"
			"\r\n ������ ����ü: %d",

			//stem
			m_pte_data->_cnt_noun + m_pte_data->_cnt_comp,
			m_pte_data->_cnt_numeral,
			m_pte_data->_cnt_pronoun,
			m_pte_data->_cnt_actverb,
			m_pte_data->_cnt_stateverb,
			m_pte_data->_cnt_adv,
			m_pte_data->_cnt_unc,
			m_pte_data->_cnt_interjection,
			m_pte_data->_cnt_numb,
			m_pte_data->_cnt_foreign,

			m_pte_data->_cnt_unregiste,

			//ending
			m_pte_data->_cnt_nend,
			m_pte_data->_cnt_vend,
			m_pte_data->_cnt_cend
		);

		str += ttt;
	}
	else if (view_statistic == _VIEW_STATISTIC_PLURAL)
	{
		str += "====�ߺ�����====\r\n";

		pte_token = m_pte_data->_pte_token_head->_next;
		while (pte_token != m_pte_data->_pte_token_tail)
		{
			if (pte_token->_same_count)
			{
				if (pte_token->_same_deputy)
				{
					memset(ttt, 0x00, 1024);
					sprintf(ttt, "\r\n ����: %s: %d", pte_token->_sztoken, pte_token->_same_count + 1);
					str += ttt;
				}
			}
			if (pte_token->_pstem != NULL)
			{
				if (pte_token->_pstem->_same_count)
				{
					if (pte_token->_pstem->_same_deputy)
					{
						memset(ttt, 0x00, 1024);
						sprintf(ttt, "\r\n �����: %s: %d", pte_token->_pstem->_szresult, pte_token->_pstem->_same_count + 1);
						str += ttt;
					}
				}
			}
			if (pte_token->_pend != NULL)
			{
				if (pte_token->_pend->_same_count)
				{
					if (pte_token->_pend->_same_deputy)
					{
						memset(ttt, 0x00, 1024);
						sprintf(ttt, "\r\n �������: %s: %d", pte_token->_pend->_szresult, pte_token->_pend->_same_count + 1);
						str += ttt;
					}
				}
			}
			pte_token = pte_token->_next;
		}


		pte_comp = m_pte_data->_pte_comp_head->_next;
		while (pte_comp != m_pte_data->_pte_comp_tail)
		{
			if (pte_comp->_same_count)
			{
				if (pte_comp->_same_deputy)
				{
					memset(ttt, 0x00, 1024);
					sprintf(ttt, "\r\n �����: %s: %d", pte_comp->_szresult, pte_comp->_same_count + 1);
					str += ttt;
				}
			}
			pte_comp = pte_comp->_next;
		}
	}

	m_StatisticsData.SetWindowText(str);
	
}
//����
void CTabFifthDialog::f_print_out(int view_out)
{
	int nListRowNum = 0;
	CString strSameCount = "";
	m_StatisticsDataList.DeleteAllItems();
	if (m_pte_data == NULL)
	{
		return;
	}
	CString str_dis = "";
	char tmp[64];

	TE_TOKEN *pte_token = NULL;
	TE_COMP_TOKEN *pte_comp = NULL;

	if ((m_pte_data->_pte_token_head != NULL) && (m_pte_data->_pte_token_tail != NULL))
	{
		if (view_out == _VIEW_OUT_ALL)
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//���վ�
				{
					if (pte_token->_szcomp_token != NULL)//���վ�
					{
						if (((pte_token->_same_count) && (pte_token->_same_deputy)) || (pte_token->_same_count == 0))
						{
							str_dis = "";
							str_dis += pte_token->_szcomp_token;
							str_dis += " \t";
							str_dis += "���\t";
							memset(tmp, 0x00, sizeof(tmp));
							sprintf(tmp, "%d", pte_token->_same_count + 1);
							str_dis += tmp;
							//����ڻ��� ����Ʈ �÷�
							str_dis += " \t-";
							//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
							
							strSameCount = "";
							strSameCount.Format(_T("%d"), pte_token->_same_count + 1);
							m_StatisticsDataList.InsertItem(nListRowNum, pte_token->_szcomp_token);
							m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, "���", 0, 0, 0, NULL);
							m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
							m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
							nListRowNum += 1;
						}
					}

					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//���վ� ����
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						while (pte_comp != m_pte_data->_pte_comp_tail)
						{
							if (pte_token->_token_offset == pte_comp->_token_offset)
							{
								if (pte_comp->_szresult != NULL)//���վ��
								{
									if (((pte_comp->_same_count) && (pte_comp->_same_deputy)) ||
										(pte_comp->_same_count == 0))
									{
										str_dis = "";
										str_dis += pte_comp->_szresult;
										if ((pte_comp->_pumsa == PUMSA_STEM_V) || (pte_comp->_pumsa == PUMSA_STEM_STA))
											str_dis += "-";
										str_dis += " \t";
										str_dis += pumsa_stem[pte_comp->_pumsa];
										str_dis += " \t";
										memset(tmp, 0x00, sizeof(tmp));
										sprintf(tmp, "%d", pte_comp->_same_count + 1);
										str_dis += tmp;
										//����ڻ������� ������ �������������
										if ((pte_comp->_szfeature != NULL) && (pte_comp->_szfeature[S_T121] == '1'))
										{
											str_dis += " \t";
											//f_user_pumsa_TO_STRING(pte_comp->_szfeature, "PUMSA", "-", tmp, sizeof(tmp), szpath_ini);
											str_dis += tmp;
										}
										else
										{
											str_dis += " \t-";
										}
										//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
										strSameCount = "";
										strSameCount.Format(_T("%d"), pte_comp->_same_count + 1);
										m_StatisticsDataList.InsertItem(nListRowNum, pte_comp->_szresult);
										m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_stem[pte_comp->_pumsa], 0, 0, 0, NULL);
										m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
										m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
										nListRowNum += 1;
									}
								}
							}
							pte_comp = pte_comp->_next;
						}
						if (pte_token->_pend != NULL)
						{
							if (pte_token->_pend->_szresult != NULL)//���վ� ����
							{
								if (((pte_token->_pend->_same_count) && (pte_token->_pend->_same_deputy)) ||
									(pte_token->_pend->_same_count == 0))
								{
									str_dis = "";
									str_dis += pte_token->_pend->_szresult;
									str_dis += " \t";
									str_dis += pumsa_end[pte_token->_pend->_pumsa];
									str_dis += " \t";
									memset(tmp, 0x00, sizeof(tmp));
									sprintf(tmp, "%d", pte_token->_pend->_same_count + 1);
									str_dis += tmp;
									//����ڻ��� �ʵ�� ����Ʈ
									str_dis += " \t-";
									//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
									strSameCount = "";
									strSameCount.Format(_T("%d"), pte_token->_pend->_same_count + 1);
									m_StatisticsDataList.InsertItem(nListRowNum, pte_token->_pend->_szresult);
									m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_end[pte_token->_pend->_pumsa], 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
									nListRowNum += 1;
								}
							}
						}
					}
				}
				else//����,����
				{
					if (pte_token->_pstem != NULL)
					{
						if (pte_token->_pstem->_szresult != NULL)
						{
							if (((pte_token->_pstem->_same_count) && (pte_token->_pstem->_same_deputy)) ||
								(pte_token->_pstem->_same_count == 0))
							{
								str_dis = "";
								str_dis += pte_token->_pstem->_szresult;
								if ((pte_token->_pstem->_pumsa == PUMSA_STEM_V) || (pte_token->_pstem->_pumsa == PUMSA_STEM_STA))
									str_dis += "-";
								str_dis += " \t";
								str_dis += pumsa_stem[pte_token->_pstem->_pumsa];
								str_dis += " \t";
								memset(tmp, 0x00, sizeof(tmp));
								sprintf(tmp, "%d", pte_token->_pstem->_same_count + 1);
								str_dis += tmp;
								if ((pte_token->_pstem->_szfeature != NULL) && (pte_token->_pstem->_szfeature[S_T121] == '1'))
								{
									str_dis += " \t";
									//f_user_pumsa_TO_STRING(pte_token->_pstem->_szfeature, "PUMSA", "-", tmp, sizeof(tmp), szpath_ini);
									str_dis += tmp;
								}
								else
								{
									str_dis += " \t-";
								}
								//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
								strSameCount = "";
								strSameCount.Format(_T("%d"), pte_token->_pstem->_same_count + 1);
								m_StatisticsDataList.InsertItem(nListRowNum, pte_token->_pstem->_szresult);
								m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_stem[pte_token->_pstem->_pumsa], 0, 0, 0, NULL);
								m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
								m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
								nListRowNum += 1;
							}
						}
					}
					if (pte_token->_pend != NULL)
					{
						if (pte_token->_pend->_szresult != NULL)
						{
							if (((pte_token->_pend->_same_count) && (pte_token->_pend->_same_deputy)) ||
								(pte_token->_pend->_same_count == 0))
							{
								str_dis = "";
								str_dis += pte_token->_pend->_szresult;
								str_dis += " \t";
								str_dis += pumsa_end[pte_token->_pend->_pumsa];
								str_dis += " \t";
								memset(tmp, 0x00, sizeof(tmp));
								sprintf(tmp, "%d", pte_token->_pend->_same_count + 1);
								str_dis += tmp;
								//����ڻ��� ������ �÷���
								str_dis += " \t-";
								//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
								strSameCount = "";
								strSameCount.Format(_T("%d"), pte_token->_pend->_same_count + 1);
								m_StatisticsDataList.InsertItem(nListRowNum, pte_token->_pend->_szresult);
								m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_end[pte_token->_pend->_pumsa], 0, 0, 0, NULL);
								m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
								m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
								nListRowNum += 1;
							}
						}
					}
				}
				pte_token = pte_token->_next;
			}
		}
		else if (view_out == _VIEW_OUT_STEM)//���۸� ���
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//���վ�
				{
					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//���վ� ����
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						while (pte_comp != m_pte_data->_pte_comp_tail)
						{
							if (pte_token->_token_offset == pte_comp->_token_offset)
							{
								if (pte_comp->_szresult != NULL)//���վ��
								{
									if (((pte_comp->_same_count) && (pte_comp->_same_deputy)) ||
										(pte_comp->_same_count == 0))
									{
										str_dis = "";
										str_dis += pte_comp->_szresult;
										if ((pte_comp->_pumsa == PUMSA_STEM_V) || (pte_comp->_pumsa == PUMSA_STEM_STA))
											str_dis += "-";
										str_dis += " \t";
										str_dis += pumsa_stem[pte_comp->_pumsa];
										str_dis += " \t";
										memset(tmp, 0x00, sizeof(tmp));
										sprintf(tmp, "%d", pte_comp->_same_count + 1);
										str_dis += tmp;
										if ((pte_comp->_szfeature != NULL) && (pte_comp->_szfeature[S_T121] == '1'))
										{
											str_dis += " \t";
											//f_user_pumsa_TO_STRING(pte_comp->_szfeature, "PUMSA", "-", tmp, sizeof(tmp), szpath_ini);
											str_dis += tmp;
										}
										else
										{
											str_dis += " \t-";
										}
										//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
										strSameCount = "";
										strSameCount.Format(_T("%d"), pte_comp->_same_count + 1);
										m_StatisticsDataList.InsertItem(nListRowNum, pte_comp->_szresult);
										m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_stem[pte_comp->_pumsa], 0, 0, 0, NULL);
										m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
										m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
										nListRowNum += 1;
									}
								}
							}
							pte_comp = pte_comp->_next;
						}
					}
				}
				else//����,����
				{
					if (pte_token->_pstem != NULL)
					{
						if (pte_token->_pstem->_szresult != NULL)
						{
							if (((pte_token->_pstem->_same_count) && (pte_token->_pstem->_same_deputy)) ||
								(pte_token->_pstem->_same_count == 0))
							{
								str_dis = "";
								str_dis += pte_token->_pstem->_szresult;
								if ((pte_token->_pstem->_pumsa == PUMSA_STEM_V) || (pte_token->_pstem->_pumsa == PUMSA_STEM_STA))
									str_dis += "-";
								str_dis += " \t";
								str_dis += pumsa_stem[pte_token->_pstem->_pumsa];
								str_dis += " \t";
								memset(tmp, 0x00, sizeof(tmp));
								sprintf(tmp, "%d", pte_token->_pstem->_same_count + 1);
								str_dis += tmp;

								if ((pte_token->_pstem->_szfeature != NULL) && (pte_token->_pstem->_szfeature[S_T121] == '1'))
								{
									str_dis += " \t";
									//f_user_pumsa_TO_STRING(pte_token->_pstem->_szfeature, "PUMSA", "-", tmp, sizeof(tmp), szpath_ini);
									str_dis += tmp;
								}
								else
								{
									str_dis += " \t-";
								}
								//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
								strSameCount = "";
								strSameCount.Format(_T("%d"), pte_token->_pstem->_same_count + 1);
								m_StatisticsDataList.InsertItem(nListRowNum, pte_token->_pstem->_szresult);
								m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_stem[pte_token->_pstem->_pumsa], 0, 0, 0, NULL);
								m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
								m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
								nListRowNum += 1;
							}
						}
					}
				}
				pte_token = pte_token->_next;
			}
		}
		else if (view_out == _VIEW_OUT_END)//���������
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//���վ�
				{
					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//���վ� ����
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						if (pte_token->_pend != NULL)
						{
							if (pte_token->_pend->_szresult != NULL)//���վ� ����
							{
								if (((pte_token->_pend->_same_count) && (pte_token->_pend->_same_deputy)) ||
									(pte_token->_pend->_same_count == 0))
								{
									str_dis = "";
									str_dis += pte_token->_pend->_szresult;
									str_dis += " \t";
									str_dis += pumsa_end[pte_token->_pend->_pumsa];
									str_dis += " \t";
									memset(tmp, 0x00, sizeof(tmp));
									sprintf(tmp, "%d", pte_token->_pend->_same_count + 1);
									str_dis += tmp;
									//����ڻ��� ����Ʈ �÷���
									str_dis += " \t-";
									//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
									strSameCount = "";
									strSameCount.Format(_T("%d"), pte_token->_pend->_same_count + 1);
									m_StatisticsDataList.InsertItem(nListRowNum, pte_token->_pend->_szresult);
									m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_end[pte_token->_pend->_pumsa], 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
									nListRowNum += 1;
								}
							}
						}
					}
				}
				else//����,����
				{
					if (pte_token->_pend != NULL)
					{
						if (pte_token->_pend->_szresult != NULL)
						{
							if (((pte_token->_pend->_same_count) && (pte_token->_pend->_same_deputy)) ||
								(pte_token->_pend->_same_count == 0))
							{
								str_dis = "";
								str_dis += pte_token->_pend->_szresult;
								str_dis += " \t";
								str_dis += pumsa_end[pte_token->_pend->_pumsa];
								str_dis += " \t";
								memset(tmp, 0x00, sizeof(tmp));
								sprintf(tmp, "%d", pte_token->_pend->_same_count + 1);
								str_dis += tmp;
								//����ڻ��� ����Ʈ �÷���
								str_dis += " \t-";
								//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
								strSameCount = "";
								strSameCount.Format(_T("%d"), pte_token->_pend->_same_count + 1);
								m_StatisticsDataList.InsertItem(nListRowNum, pte_token->_pend->_szresult);
								m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_end[pte_token->_pend->_pumsa], 0, 0, 0, NULL);
								m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
								m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
								nListRowNum += 1;
							}
						}
					}
				}
				pte_token = pte_token->_next;
			}
		}
		else if (view_out == _VIEW_OUT_N)//��縸 ���
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//���վ�
				{
					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//���վ� ����
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						while (pte_comp != m_pte_data->_pte_comp_tail)
						{
							if (pte_token->_token_offset == pte_comp->_token_offset)
							{
								if (pte_comp->_szresult != NULL)//���վ��
								{
									if (((pte_comp->_same_count) && (pte_comp->_same_deputy)) ||
										(pte_comp->_same_count == 0))
									{
										str_dis = "";
										str_dis += pte_comp->_szresult;
										if ((pte_comp->_pumsa == PUMSA_STEM_V) || (pte_comp->_pumsa == PUMSA_STEM_STA))
											str_dis += "-";
										str_dis += " \t";
										str_dis += pumsa_stem[pte_comp->_pumsa];
										str_dis += " \t";
										memset(tmp, 0x00, sizeof(tmp));
										sprintf(tmp, "%d", pte_comp->_same_count + 1);
										str_dis += tmp;
										if ((pte_comp->_szfeature != NULL) && (pte_comp->_szfeature[S_T121] == '1'))
										{
											str_dis += " \t";
											//f_user_pumsa_TO_STRING(pte_comp->_szfeature, "PUMSA", "-", tmp, sizeof(tmp), szpath_ini);
											str_dis += tmp;
										}
										else
										{
											str_dis += " \t-";
										}
										if (pte_comp->_pumsa == PUMSA_STEM_N)
										{
											//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
										}
										strSameCount = "";
										strSameCount.Format(_T("%d"), pte_comp->_same_count + 1);
										m_StatisticsDataList.InsertItem(nListRowNum, pte_comp->_szresult);
										m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_stem[pte_comp->_pumsa], 0, 0, 0, NULL);
										m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
										m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
										nListRowNum += 1;
									}
								}
							}
							pte_comp = pte_comp->_next;
						}
					}
				}
				else//����,����
				{
					if (pte_token->_pstem != NULL)
					{
						if (pte_token->_pstem->_szresult != NULL)
						{
							if (((pte_token->_pstem->_same_count) && (pte_token->_pstem->_same_deputy)) ||
								(pte_token->_pstem->_same_count == 0))
							{
								str_dis = "";
								str_dis += pte_token->_pstem->_szresult;
								if ((pte_token->_pstem->_pumsa == PUMSA_STEM_V) || (pte_token->_pstem->_pumsa == PUMSA_STEM_STA))
									str_dis += "-";
								str_dis += " \t";
								str_dis += pumsa_stem[pte_token->_pstem->_pumsa];
								str_dis += " \t";
								memset(tmp, 0x00, sizeof(tmp));
								sprintf(tmp, "%d", pte_token->_pstem->_same_count + 1);
								str_dis += tmp;
								if ((pte_token->_pstem->_szfeature != NULL) && (pte_token->_pstem->_szfeature[S_T121] == '1'))
								{
									str_dis += " \t";
									//f_user_pumsa_TO_STRING(pte_token->_pstem->_szfeature, "PUMSA", "-", tmp, sizeof(tmp), szpath_ini);
									str_dis += tmp;
								}
								else
								{
									str_dis += " \t-";
								}

								if (pte_token->_pstem->_pumsa == PUMSA_STEM_N)
								{
									//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
									strSameCount = "";
									strSameCount.Format(_T("%d"), pte_token->_pstem->_same_count + 1);
									m_StatisticsDataList.InsertItem(nListRowNum, pte_token->_pstem->_szresult);
									m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_stem[pte_token->_pstem->_pumsa], 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
									nListRowNum += 1;
								}
							}
						}
					}
				}
				pte_token = pte_token->_next;
			}
		}
		else if (view_out == _VIEW_OUT_NUM)//���縸 ���
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//���վ�
				{
					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//���վ� ����
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						while (pte_comp != m_pte_data->_pte_comp_tail)
						{
							if (pte_token->_token_offset == pte_comp->_token_offset)
							{
								if (pte_comp->_szresult != NULL)//���վ��
								{
									if (((pte_comp->_same_count) && (pte_comp->_same_deputy)) ||
										(pte_comp->_same_count == 0))
									{
										str_dis = "";
										str_dis += pte_comp->_szresult;
										if ((pte_comp->_pumsa == PUMSA_STEM_V) || (pte_comp->_pumsa == PUMSA_STEM_STA))
											str_dis += "-";
										str_dis += " \t";
										str_dis += pumsa_stem[pte_comp->_pumsa];
										str_dis += " \t";
										memset(tmp, 0x00, sizeof(tmp));
										sprintf(tmp, "%d", pte_comp->_same_count + 1);
										str_dis += tmp;

										str_dis += " \t";
										//f_user_pumsa_TO_STRING(pte_comp->_szfeature, "PUMSA", "-", tmp, sizeof(tmp), szpath_ini);
										str_dis += tmp;
										if ((pte_comp->_szfeature != NULL) && (pte_comp->_szfeature[S_T121] == '1'))
										{
											str_dis += " \t";
											//f_user_pumsa_TO_STRING(pte_comp->_szfeature, "PUMSA", "-", tmp, sizeof(tmp), szpath_ini);
											str_dis += tmp;
										}
										else
										{
											str_dis += " \t-";
										}

										if (pte_comp->_pumsa == PUMSA_STEM_NUM)
										{
											//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
											strSameCount = "";
											strSameCount.Format(_T("%d"), pte_comp->_same_count + 1);
											m_StatisticsDataList.InsertItem(nListRowNum, pte_comp->_szresult);
											m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_stem[pte_comp->_pumsa], 0, 0, 0, NULL);
											m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
											m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
											nListRowNum += 1;
										}
									}
								}
							}
							pte_comp = pte_comp->_next;
						}
					}
				}
				else//����,����
				{
					if (pte_token->_pstem != NULL)
					{
						if (pte_token->_pstem->_szresult != NULL)
						{
							if (((pte_token->_pstem->_same_count) && (pte_token->_pstem->_same_deputy)) ||
								(pte_token->_pstem->_same_count == 0))
							{
								str_dis = "";
								str_dis += pte_token->_pstem->_szresult;
								if ((pte_token->_pstem->_pumsa == PUMSA_STEM_V) || (pte_token->_pstem->_pumsa == PUMSA_STEM_STA))
									str_dis += "-";
								str_dis += " \t";
								str_dis += pumsa_stem[pte_token->_pstem->_pumsa];
								str_dis += " \t";
								memset(tmp, 0x00, sizeof(tmp));
								sprintf(tmp, "%d", pte_token->_pstem->_same_count + 1);
								str_dis += tmp;

								if ((pte_token->_pstem->_szfeature != NULL) && (pte_token->_pstem->_szfeature[S_T121] == '1'))
								{
									str_dis += " \t";
									//f_user_pumsa_TO_STRING(pte_token->_pstem->_szfeature, "PUMSA", "-", tmp, sizeof(tmp), szpath_ini);
									str_dis += tmp;
								}
								else
								{
									str_dis += " \t-";
								}
								if (pte_token->_pstem->_pumsa == PUMSA_STEM_NUM)
								{
									//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
									strSameCount = "";
									strSameCount.Format(_T("%d"), pte_token->_pstem->_same_count + 1);
									m_StatisticsDataList.InsertItem(nListRowNum, pte_token->_pstem->_szresult);
									m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_stem[pte_token->_pstem->_pumsa], 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
									nListRowNum += 1;
								}
							}
						}
					}
				}
				pte_token = pte_token->_next;
			}
		}
		else if (view_out == _VIEW_OUT_PRO)//���縸 ���
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//���վ�
				{
					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//���վ� ����
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						while (pte_comp != m_pte_data->_pte_comp_tail)
						{
							if (pte_token->_token_offset == pte_comp->_token_offset)
							{
								if (pte_comp->_szresult != NULL)//���վ��
								{
									if (((pte_comp->_same_count) && (pte_comp->_same_deputy)) ||
										(pte_comp->_same_count == 0))
									{
										str_dis = "";
										str_dis += pte_comp->_szresult;
										if ((pte_comp->_pumsa == PUMSA_STEM_V) || (pte_comp->_pumsa == PUMSA_STEM_STA))
											str_dis += "-";
										str_dis += " \t";
										str_dis += pumsa_stem[pte_comp->_pumsa];
										str_dis += " \t";
										memset(tmp, 0x00, sizeof(tmp));
										sprintf(tmp, "%d", pte_comp->_same_count + 1);
										str_dis += tmp;

										if ((pte_comp->_szfeature != NULL) && (pte_comp->_szfeature[S_T121] == '1'))
										{
											str_dis += " \t";
											//f_user_pumsa_TO_STRING(pte_comp->_szfeature, "PUMSA", "-", tmp, sizeof(tmp), szpath_ini);
											str_dis += tmp;
										}
										else
										{
											str_dis += " \t-";
										}

										if (pte_comp->_pumsa == PUMSA_STEM_PRO)
										{
											//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
											strSameCount = "";
											strSameCount.Format(_T("%d"), pte_comp->_same_count + 1);
											m_StatisticsDataList.InsertItem(nListRowNum, pte_comp->_szresult);
											m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_stem[pte_comp->_pumsa], 0, 0, 0, NULL);
											m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
											m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
											nListRowNum += 1;
										}
									}
								}
							}
							pte_comp = pte_comp->_next;
						}
					}
				}
				else//����,����
				{
					if (pte_token->_pstem != NULL)
					{
						if (pte_token->_pstem->_szresult != NULL)
						{
							if (((pte_token->_pstem->_same_count) && (pte_token->_pstem->_same_deputy)) ||
								(pte_token->_pstem->_same_count == 0))
							{
								str_dis = "";
								str_dis += pte_token->_pstem->_szresult;
								if ((pte_token->_pstem->_pumsa == PUMSA_STEM_V) || (pte_token->_pstem->_pumsa == PUMSA_STEM_STA))
									str_dis += "-";
								str_dis += " \t";
								str_dis += pumsa_stem[pte_token->_pstem->_pumsa];
								str_dis += " \t";
								memset(tmp, 0x00, sizeof(tmp));
								sprintf(tmp, "%d", pte_token->_pstem->_same_count + 1);
								str_dis += tmp;

								if ((pte_token->_pstem->_szfeature != NULL) && (pte_token->_pstem->_szfeature[S_T121] == '1'))
								{
									str_dis += " \t";
									//f_user_pumsa_TO_STRING(pte_token->_pstem->_szfeature, "PUMSA", "-", tmp, sizeof(tmp), szpath_ini);
									str_dis += tmp;
								}
								else
								{
									str_dis += " \t-";
								}

								if (pte_token->_pstem->_pumsa == PUMSA_STEM_PRO)
								{
									//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
									strSameCount = "";
									strSameCount.Format(_T("%d"), pte_token->_pstem->_same_count + 1);
									m_StatisticsDataList.InsertItem(nListRowNum, pte_token->_pstem->_szresult);
									m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_stem[pte_token->_pstem->_pumsa], 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
									nListRowNum += 1;
								}
							}
						}
					}
				}
				pte_token = pte_token->_next;
			}
		}
		else if (view_out == _VIEW_OUT_V)//���縸 ���
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//���վ�
				{
					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//���վ� ����
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						while (pte_comp != m_pte_data->_pte_comp_tail)
						{
							if (pte_token->_token_offset == pte_comp->_token_offset)
							{
								if (pte_comp->_szresult != NULL)//���վ��
								{
									if (((pte_comp->_same_count) && (pte_comp->_same_deputy)) ||
										(pte_comp->_same_count == 0))
									{
										str_dis = "";
										str_dis += pte_comp->_szresult;
										if ((pte_comp->_pumsa == PUMSA_STEM_V) || (pte_comp->_pumsa == PUMSA_STEM_STA))
											str_dis += "-";
										str_dis += " \t";
										str_dis += pumsa_stem[pte_comp->_pumsa];
										str_dis += " \t";
										memset(tmp, 0x00, sizeof(tmp));
										sprintf(tmp, "%d", pte_comp->_same_count + 1);
										str_dis += tmp;

										if ((pte_comp->_szfeature != NULL) && (pte_comp->_szfeature[S_T121] == '1'))
										{
											str_dis += " \t";
											//f_user_pumsa_TO_STRING(pte_comp->_szfeature, "PUMSA", "-", tmp, sizeof(tmp), szpath_ini);
											str_dis += tmp;
										}
										else
										{
											str_dis += " \t-";
										}
										if (pte_comp->_pumsa == PUMSA_STEM_V)
										{
											//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
											strSameCount = "";
											strSameCount.Format(_T("%d"), pte_comp->_same_count + 1);
											m_StatisticsDataList.InsertItem(nListRowNum, pte_comp->_szresult);
											m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_stem[pte_comp->_pumsa], 0, 0, 0, NULL);
											m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
											m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
											nListRowNum += 1;
										}
									}
								}
							}
							pte_comp = pte_comp->_next;
						}
					}
				}
				else//����,����
				{
					if (pte_token->_pstem != NULL)
					{
						if (pte_token->_pstem->_szresult != NULL)
						{
							if (((pte_token->_pstem->_same_count) && (pte_token->_pstem->_same_deputy)) ||
								(pte_token->_pstem->_same_count == 0))
							{
								str_dis = "";
								str_dis += pte_token->_pstem->_szresult;
								if ((pte_token->_pstem->_pumsa == PUMSA_STEM_V) || (pte_token->_pstem->_pumsa == PUMSA_STEM_STA))
									str_dis += "-";
								str_dis += " \t";
								str_dis += pumsa_stem[pte_token->_pstem->_pumsa];
								str_dis += " \t";
								memset(tmp, 0x00, sizeof(tmp));
								sprintf(tmp, "%d", pte_token->_pstem->_same_count + 1);
								str_dis += tmp;
								if ((pte_token->_pstem->_szfeature != NULL) && (pte_token->_pstem->_szfeature[S_T121] == '1'))
								{
									str_dis += " \t";
									//f_user_pumsa_TO_STRING(pte_token->_pstem->_szfeature, "PUMSA", "-", tmp, sizeof(tmp), szpath_ini);
									str_dis += tmp;
								}
								else
								{
									str_dis += " \t-";
								}
								if (pte_token->_pstem->_pumsa == PUMSA_STEM_V) 
								{
									//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
									strSameCount = "";
									strSameCount.Format(_T("%d"), pte_token->_pstem->_same_count + 1);
									m_StatisticsDataList.InsertItem(nListRowNum, pte_token->_pstem->_szresult);
									m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_stem[pte_token->_pstem->_pumsa], 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
									nListRowNum += 1;
								}
							}
						}
					}
				}
				pte_token = pte_token->_next;
			}
		}
		else if (view_out == _VIEW_OUT_STA)//����縸 ���
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//���վ�
				{
					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//���վ� ����
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						while (pte_comp != m_pte_data->_pte_comp_tail)
						{
							if (pte_token->_token_offset == pte_comp->_token_offset)
							{
								if (pte_comp->_szresult != NULL)//���վ��
								{
									if (((pte_comp->_same_count) && (pte_comp->_same_deputy)) ||
										(pte_comp->_same_count == 0))
									{
										str_dis = "";
										str_dis += pte_comp->_szresult;
										if ((pte_comp->_pumsa == PUMSA_STEM_V) || (pte_comp->_pumsa == PUMSA_STEM_STA))
											str_dis += "-";
										str_dis += " \t";
										str_dis += pumsa_stem[pte_comp->_pumsa];
										str_dis += " \t";
										memset(tmp, 0x00, sizeof(tmp));
										sprintf(tmp, "%d", pte_comp->_same_count + 1);
										str_dis += tmp;
										if ((pte_comp->_szfeature != NULL) && (pte_comp->_szfeature[S_T121] == '1'))
										{
											str_dis += " \t";
											//f_user_pumsa_TO_STRING(pte_comp->_szfeature, "PUMSA", "-", tmp, sizeof(tmp), szpath_ini);
											str_dis += tmp;
										}
										else
										{
											str_dis += " \t-";
										}
										if (pte_comp->_pumsa == PUMSA_STEM_STA) 
										{
											//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
											strSameCount = "";
											strSameCount.Format(_T("%d"), pte_comp->_same_count + 1);
											m_StatisticsDataList.InsertItem(nListRowNum, pte_comp->_szresult);
											m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_stem[pte_comp->_pumsa], 0, 0, 0, NULL);
											m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
											m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
											nListRowNum += 1;
										}
									}
								}
							}
							pte_comp = pte_comp->_next;
						}
					}
				}
				else//����,����
				{
					if (pte_token->_pstem != NULL)
					{
						if (pte_token->_pstem->_szresult != NULL)
						{
							if (((pte_token->_pstem->_same_count) && (pte_token->_pstem->_same_deputy)) ||
								(pte_token->_pstem->_same_count == 0))
							{
								str_dis = "";
								str_dis += pte_token->_pstem->_szresult;
								if ((pte_token->_pstem->_pumsa == PUMSA_STEM_V) || (pte_token->_pstem->_pumsa == PUMSA_STEM_STA))
									str_dis += "-";
								str_dis += " \t";
								str_dis += pumsa_stem[pte_token->_pstem->_pumsa];
								str_dis += " \t";
								memset(tmp, 0x00, sizeof(tmp));
								sprintf(tmp, "%d", pte_token->_pstem->_same_count + 1);
								str_dis += tmp;
								if ((pte_token->_pstem->_szfeature != NULL) && (pte_token->_pstem->_szfeature[S_T121] == '1'))
								{
									str_dis += " \t";
									//f_user_pumsa_TO_STRING(pte_token->_pstem->_szfeature, "PUMSA", "-", tmp, sizeof(tmp), szpath_ini);
									str_dis += tmp;
								}
								else
								{
									str_dis += " \t-";
								}
								if (pte_token->_pstem->_pumsa == PUMSA_STEM_STA)
								{
									//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
									strSameCount = "";
									strSameCount.Format(_T("%d"), pte_token->_pstem->_same_count + 1);
									m_StatisticsDataList.InsertItem(nListRowNum, pte_token->_pstem->_szresult);
									m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_stem[pte_token->_pstem->_pumsa], 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
									nListRowNum += 1;
								}
							}
						}
					}
				}
				pte_token = pte_token->_next;
			}
		}
		else if (view_out == _VIEW_OUT_UNC)//�����縸 ���
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//���վ�
				{
					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//���վ� ����
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						while (pte_comp != m_pte_data->_pte_comp_tail)
						{
							if (pte_token->_token_offset == pte_comp->_token_offset)
							{
								if (pte_comp->_szresult != NULL)//���վ��
								{
									if (((pte_comp->_same_count) && (pte_comp->_same_deputy)) ||
										(pte_comp->_same_count == 0))
									{
										str_dis = "";
										str_dis += pte_comp->_szresult;
										if ((pte_comp->_pumsa == PUMSA_STEM_V) || (pte_comp->_pumsa == PUMSA_STEM_STA))
											str_dis += "-";
										str_dis += " \t";
										str_dis += pumsa_stem[pte_comp->_pumsa];
										str_dis += " \t";
										memset(tmp, 0x00, sizeof(tmp));
										sprintf(tmp, "%d", pte_comp->_same_count + 1);
										str_dis += tmp;

										if ((pte_comp->_szfeature != NULL) && (pte_comp->_szfeature[S_T121] == '1'))
										{
											str_dis += " \t";
											//f_user_pumsa_TO_STRING(pte_comp->_szfeature, "PUMSA", "-", tmp, sizeof(tmp), szpath_ini);
											str_dis += tmp;
										}
										else
										{
											str_dis += " \t-";
										}

										if (pte_comp->_pumsa == PUMSA_STEM_UNC)
										{
											//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
											strSameCount = "";
											strSameCount.Format(_T("%d"), pte_comp->_same_count + 1);
											m_StatisticsDataList.InsertItem(nListRowNum, pte_comp->_szresult);
											m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_stem[pte_comp->_pumsa], 0, 0, 0, NULL);
											m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
											m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
											nListRowNum += 1;
										}
									}
								}
							}
							pte_comp = pte_comp->_next;
						}
					}
				}
				else//����,����
				{
					if (pte_token->_pstem != NULL)
					{
						if (pte_token->_pstem->_szresult != NULL)
						{
							if (((pte_token->_pstem->_same_count) && (pte_token->_pstem->_same_deputy)) ||
								(pte_token->_pstem->_same_count == 0))
							{
								str_dis = "";
								str_dis += pte_token->_pstem->_szresult;
								if ((pte_token->_pstem->_pumsa == PUMSA_STEM_V) || (pte_token->_pstem->_pumsa == PUMSA_STEM_STA))
									str_dis += "-";
								str_dis += " \t";
								str_dis += pumsa_stem[pte_token->_pstem->_pumsa];
								str_dis += " \t";
								memset(tmp, 0x00, sizeof(tmp));
								sprintf(tmp, "%d", pte_token->_pstem->_same_count + 1);
								str_dis += tmp;

								if ((pte_token->_pstem->_szfeature != NULL) && (pte_token->_pstem->_szfeature[S_T121] == '1'))
								{
									str_dis += " \t";
									//f_user_pumsa_TO_STRING(pte_token->_pstem->_szfeature, "PUMSA", "-", tmp, sizeof(tmp), szpath_ini);
									str_dis += tmp;
								}
								else
								{
									str_dis += " \t-";
								}

								if (pte_token->_pstem->_pumsa == PUMSA_STEM_UNC)
								{
									//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
									strSameCount = "";
									strSameCount.Format(_T("%d"), pte_token->_pstem->_same_count + 1);
									m_StatisticsDataList.InsertItem(nListRowNum, pte_token->_pstem->_szresult);
									m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_stem[pte_token->_pstem->_pumsa], 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
									nListRowNum += 1;
								}
							}
						}
					}
				}
				pte_token = pte_token->_next;
			}
		}
		else if (view_out == _VIEW_OUT_ADV)//�λ縸 ���
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//���վ�
				{
					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//���վ� ����
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						while (pte_comp != m_pte_data->_pte_comp_tail)
						{
							if (pte_token->_token_offset == pte_comp->_token_offset)
							{
								if (pte_comp->_szresult != NULL)//���վ��
								{
									if (((pte_comp->_same_count) && (pte_comp->_same_deputy)) ||
										(pte_comp->_same_count == 0))
									{
										str_dis = "";
										str_dis += pte_comp->_szresult;
										if ((pte_comp->_pumsa == PUMSA_STEM_V) || (pte_comp->_pumsa == PUMSA_STEM_STA))
											str_dis += "-";
										str_dis += " \t";
										str_dis += pumsa_stem[pte_comp->_pumsa];
										str_dis += " \t";
										memset(tmp, 0x00, sizeof(tmp));
										sprintf(tmp, "%d", pte_comp->_same_count + 1);
										str_dis += tmp;

										if ((pte_comp->_szfeature != NULL) && (pte_comp->_szfeature[S_T121] == '1'))
										{
											str_dis += " \t";
											//f_user_pumsa_TO_STRING(pte_comp->_szfeature, "PUMSA", "-", tmp, sizeof(tmp), szpath_ini);
											str_dis += tmp;
										}
										else
										{
											str_dis += " \t-";
										}
										if (pte_comp->_pumsa == PUMSA_STEM_ADV)
										{
											//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
											strSameCount = "";
											strSameCount.Format(_T("%d"), pte_comp->_same_count + 1);
											m_StatisticsDataList.InsertItem(nListRowNum, pte_comp->_szresult);
											m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_stem[pte_comp->_pumsa], 0, 0, 0, NULL);
											m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
											m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
											nListRowNum += 1;
										}
									}
								}
							}
							pte_comp = pte_comp->_next;
						}
					}
				}
				else//����,����
				{
					if (pte_token->_pstem != NULL)
					{
						if (pte_token->_pstem->_szresult != NULL)
						{
							if (((pte_token->_pstem->_same_count) && (pte_token->_pstem->_same_deputy)) ||
								(pte_token->_pstem->_same_count == 0))
							{
								str_dis = "";
								str_dis += pte_token->_pstem->_szresult;
								if ((pte_token->_pstem->_pumsa == PUMSA_STEM_V) || (pte_token->_pstem->_pumsa == PUMSA_STEM_STA))
									str_dis += "-";
								str_dis += " \t";
								str_dis += pumsa_stem[pte_token->_pstem->_pumsa];
								str_dis += " \t";
								memset(tmp, 0x00, sizeof(tmp));
								sprintf(tmp, "%d", pte_token->_pstem->_same_count + 1);
								str_dis += tmp;

								if ((pte_token->_pstem->_szfeature != NULL) && (pte_token->_pstem->_szfeature[S_T121] == '1'))
								{
									str_dis += " \t";
									//f_user_pumsa_TO_STRING(pte_token->_pstem->_szfeature, "PUMSA", "-", tmp, sizeof(tmp), szpath_ini);
									str_dis += tmp;
								}
								else
								{
									str_dis += " \t-";
								}
								if (pte_token->_pstem->_pumsa == PUMSA_STEM_ADV)
								{
									//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
									strSameCount = "";
									strSameCount.Format(_T("%d"), pte_token->_pstem->_same_count + 1);
									m_StatisticsDataList.InsertItem(nListRowNum, pte_token->_pstem->_szresult);
									m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_stem[pte_token->_pstem->_pumsa], 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
									nListRowNum += 1;
								}
							}
						}
					}
				}
				pte_token = pte_token->_next;
			}
		}
		else if (view_out == _VIEW_OUT_IC)//��ź�縸 ���
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//���վ�
				{
					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//���վ� ����
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						while (pte_comp != m_pte_data->_pte_comp_tail)
						{
							if (pte_token->_token_offset == pte_comp->_token_offset)
							{
								if (pte_comp->_szresult != NULL)//���վ��
								{
									if (((pte_comp->_same_count) && (pte_comp->_same_deputy)) ||
										(pte_comp->_same_count == 0))
									{
										str_dis = "";
										str_dis += pte_comp->_szresult;
										if ((pte_comp->_pumsa == PUMSA_STEM_V) || (pte_comp->_pumsa == PUMSA_STEM_STA))
											str_dis += "-";
										str_dis += " \t";
										str_dis += pumsa_stem[pte_comp->_pumsa];
										str_dis += " \t";
										memset(tmp, 0x00, sizeof(tmp));
										sprintf(tmp, "%d", pte_comp->_same_count + 1);
										str_dis += tmp;

										if ((pte_comp->_szfeature != NULL) && (pte_comp->_szfeature[S_T121] == '1'))
										{
											str_dis += " \t";
											//f_user_pumsa_TO_STRING(pte_comp->_szfeature, "PUMSA", "-", tmp, sizeof(tmp), szpath_ini);
											str_dis += tmp;
										}
										else
										{
											str_dis += " \t-";
										}

										if (pte_comp->_pumsa == PUMSA_STEM_IC)
										{
											//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
											strSameCount = "";
											strSameCount.Format(_T("%d"), pte_comp->_same_count + 1);
											m_StatisticsDataList.InsertItem(nListRowNum, pte_comp->_szresult);
											m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_stem[pte_comp->_pumsa], 0, 0, 0, NULL);
											m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
											m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
											nListRowNum += 1;
										}
									}
								}
							}
							pte_comp = pte_comp->_next;
						}
					}
				}
				else//����,����
				{
					if (pte_token->_pstem != NULL)
					{
						if (pte_token->_pstem->_szresult != NULL)
						{
							if (((pte_token->_pstem->_same_count) && (pte_token->_pstem->_same_deputy)) ||
								(pte_token->_pstem->_same_count == 0))
							{
								str_dis = "";
								str_dis += pte_token->_pstem->_szresult;
								if ((pte_token->_pstem->_pumsa == PUMSA_STEM_V) || (pte_token->_pstem->_pumsa == PUMSA_STEM_STA))
									str_dis += "-";
								str_dis += " \t";
								str_dis += pumsa_stem[pte_token->_pstem->_pumsa];
								str_dis += " \t";
								memset(tmp, 0x00, sizeof(tmp));
								sprintf(tmp, "%d", pte_token->_pstem->_same_count + 1);
								str_dis += tmp;

								if ((pte_token->_pstem->_szfeature != NULL) && (pte_token->_pstem->_szfeature[S_T121] == '1'))
								{
									str_dis += " \t";
									//f_user_pumsa_TO_STRING(pte_token->_pstem->_szfeature, "PUMSA", "-", tmp, sizeof(tmp), szpath_ini);
									str_dis += tmp;
								}
								else
								{
									str_dis += " \t-";
								}
								if (pte_token->_pstem->_pumsa == PUMSA_STEM_IC)
								{
									//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
									strSameCount = "";
									strSameCount.Format(_T("%d"), pte_token->_pstem->_same_count + 1);
									m_StatisticsDataList.InsertItem(nListRowNum, pte_token->_pstem->_szresult);
									m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_stem[pte_token->_pstem->_pumsa], 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
									nListRowNum += 1;
								}
							}
						}
					}
				}
				pte_token = pte_token->_next;
			}
		}
		else if (view_out == _VIEW_OUT_SYMBOL)//��ȣ
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//���վ�
				{
					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//���վ� ����
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						while (pte_comp != m_pte_data->_pte_comp_tail)
						{
							if (pte_token->_token_offset == pte_comp->_token_offset)
							{
								if (pte_comp->_szresult != NULL)//���վ��
								{
									if (((pte_comp->_same_count) && (pte_comp->_same_deputy)) ||
										(pte_comp->_same_count == 0))
									{
										str_dis = "";
										str_dis += pte_comp->_szresult;
										if ((pte_comp->_pumsa == PUMSA_STEM_V) || (pte_comp->_pumsa == PUMSA_STEM_STA))
											str_dis += "-";
										str_dis += " \t";
										str_dis += pumsa_stem[pte_comp->_pumsa];
										str_dis += " \t";
										memset(tmp, 0x00, sizeof(tmp));
										sprintf(tmp, "%d", pte_comp->_same_count + 1);
										str_dis += tmp;
										if ((pte_comp->_szfeature != NULL) && (pte_comp->_szfeature[S_T121] == '1'))
										{
											str_dis += " \t";
											//f_user_pumsa_TO_STRING(pte_comp->_szfeature, "PUMSA", "-", tmp, sizeof(tmp), szpath_ini);
											str_dis += tmp;
										}
										else
										{
											str_dis += " \t-";
										}
										if (pte_comp->_pumsa == PUMSA_STEM_SYMBOL)
										{
											//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
											strSameCount = "";
											strSameCount.Format(_T("%d"), pte_comp->_same_count + 1);
											m_StatisticsDataList.InsertItem(nListRowNum, pte_comp->_szresult);
											m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_stem[pte_comp->_pumsa], 0, 0, 0, NULL);
											m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
											m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
											nListRowNum += 1;
										}
									}
								}
							}
							pte_comp = pte_comp->_next;
						}
					}
				}
				else//����,����
				{
					if (pte_token->_pstem != NULL)
					{
						if (pte_token->_pstem->_szresult != NULL)
						{
							if (((pte_token->_pstem->_same_count) && (pte_token->_pstem->_same_deputy)) ||
								(pte_token->_pstem->_same_count == 0))
							{
								str_dis = "";
								str_dis += pte_token->_pstem->_szresult;
								if ((pte_token->_pstem->_pumsa == PUMSA_STEM_V) || (pte_token->_pstem->_pumsa == PUMSA_STEM_STA))
									str_dis += "-";
								str_dis += " \t";
								str_dis += pumsa_stem[pte_token->_pstem->_pumsa];
								str_dis += " \t";
								memset(tmp, 0x00, sizeof(tmp));
								sprintf(tmp, "%d", pte_token->_pstem->_same_count + 1);
								str_dis += tmp;

								if ((pte_token->_pstem->_szfeature != NULL) && (pte_token->_pstem->_szfeature[S_T121] == '1'))
								{
									str_dis += " \t";
									//f_user_pumsa_TO_STRING(pte_token->_pstem->_szfeature, "PUMSA", "-", tmp, sizeof(tmp), szpath_ini);
									str_dis += tmp;
								}
								else
								{
									str_dis += " \t-";
								}
								if (pte_token->_pstem->_pumsa == PUMSA_STEM_SYMBOL)
								{
									//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
									strSameCount = "";
									strSameCount.Format(_T("%d"), pte_token->_pstem->_same_count + 1);
									m_StatisticsDataList.InsertItem(nListRowNum, pte_token->_pstem->_szresult);
									m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_stem[pte_token->_pstem->_pumsa], 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
									nListRowNum += 1;
								}
							}
						}
					}
				}
				pte_token = pte_token->_next;
			}
		}
		else if (view_out == _VIEW_OUT_NUMB)//����
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//���վ�
				{
					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//���վ� ����
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						while (pte_comp != m_pte_data->_pte_comp_tail)
						{
							if (pte_token->_token_offset == pte_comp->_token_offset)
							{
								if (pte_comp->_szresult != NULL)//���վ��
								{
									if (((pte_comp->_same_count) && (pte_comp->_same_deputy)) ||
										(pte_comp->_same_count == 0))
									{
										str_dis = "";
										str_dis += pte_comp->_szresult;
										if ((pte_comp->_pumsa == PUMSA_STEM_V) || (pte_comp->_pumsa == PUMSA_STEM_STA))
											str_dis += "-";
										str_dis += " \t";
										str_dis += pumsa_stem[pte_comp->_pumsa];
										str_dis += " \t";
										memset(tmp, 0x00, sizeof(tmp));
										sprintf(tmp, "%d", pte_comp->_same_count + 1);
										str_dis += tmp;

										if ((pte_comp->_szfeature != NULL) && (pte_comp->_szfeature[S_T121] == '1'))
										{
											str_dis += " \t";
											//f_user_pumsa_TO_STRING(pte_comp->_szfeature, "PUMSA", "-", tmp, sizeof(tmp), szpath_ini);
											str_dis += tmp;
										}
										else
										{
											str_dis += " \t-";
										}

										if (pte_comp->_pumsa == PUMSA_STEM_NUMB)
										{
											//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
											strSameCount = "";
											strSameCount.Format(_T("%d"), pte_comp->_same_count + 1);
											m_StatisticsDataList.InsertItem(nListRowNum, pte_comp->_szresult);
											m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_stem[pte_comp->_pumsa], 0, 0, 0, NULL);
											m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
											m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
											nListRowNum += 1;
										}
									}
								}
							}
							pte_comp = pte_comp->_next;
						}
					}
				}
				else//����,����
				{
					if (pte_token->_pstem != NULL)
					{
						if (pte_token->_pstem->_szresult != NULL)
						{
							if (((pte_token->_pstem->_same_count) && (pte_token->_pstem->_same_deputy)) ||
								(pte_token->_pstem->_same_count == 0))
							{
								str_dis = "";
								str_dis += pte_token->_pstem->_szresult;
								if ((pte_token->_pstem->_pumsa == PUMSA_STEM_V) || (pte_token->_pstem->_pumsa == PUMSA_STEM_STA))
									str_dis += "-";
								str_dis += " \t";
								str_dis += pumsa_stem[pte_token->_pstem->_pumsa];
								str_dis += " \t";
								memset(tmp, 0x00, sizeof(tmp));
								sprintf(tmp, "%d", pte_token->_pstem->_same_count + 1);
								str_dis += tmp;

								if ((pte_token->_pstem->_szfeature != NULL) && (pte_token->_pstem->_szfeature[S_T121] == '1'))
								{
									str_dis += " \t";
									//f_user_pumsa_TO_STRING(pte_token->_pstem->_szfeature, "PUMSA", "-", tmp, sizeof(tmp), szpath_ini);
									str_dis += tmp;
								}
								else
								{
									str_dis += " \t-";
								}
								if (pte_token->_pstem->_pumsa == PUMSA_STEM_NUMB)
								{
									//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
									strSameCount = "";
									strSameCount.Format(_T("%d"), pte_token->_pstem->_same_count + 1);
									m_StatisticsDataList.InsertItem(nListRowNum, pte_token->_pstem->_szresult);
									m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_stem[pte_token->_pstem->_pumsa], 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
									nListRowNum += 1;
								}
							}
						}
					}
				}
				pte_token = pte_token->_next;
			}
		}
		else if (view_out == _VIEW_OUT_FOREIGN)//�ܱ���
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//���վ�
				{
					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//���վ� ����
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						while (pte_comp != m_pte_data->_pte_comp_tail)
						{
							if (pte_token->_token_offset == pte_comp->_token_offset)
							{
								if (pte_comp->_szresult != NULL)//���վ��
								{
									if (((pte_comp->_same_count) && (pte_comp->_same_deputy)) ||
										(pte_comp->_same_count == 0))
									{
										str_dis = "";
										str_dis += pte_comp->_szresult;
										if ((pte_comp->_pumsa == PUMSA_STEM_V) || (pte_comp->_pumsa == PUMSA_STEM_STA))
											str_dis += "-";
										str_dis += " \t";
										str_dis += pumsa_stem[pte_comp->_pumsa];
										str_dis += " \t";
										memset(tmp, 0x00, sizeof(tmp));
										sprintf(tmp, "%d", pte_comp->_same_count + 1);
										str_dis += tmp;

										if ((pte_comp->_szfeature != NULL) && (pte_comp->_szfeature[S_T121] == '1'))
										{
											str_dis += " \t";
											//f_user_pumsa_TO_STRING(pte_comp->_szfeature, "PUMSA", "-", tmp, sizeof(tmp), szpath_ini);
											str_dis += tmp;
										}
										else
										{
											str_dis += " \t-";
										}
										if (pte_comp->_pumsa == PUMSA_STEM_FOREIGN)
										{
											//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
											strSameCount = "";
											strSameCount.Format(_T("%d"), pte_comp->_same_count + 1);
											m_StatisticsDataList.InsertItem(nListRowNum, pte_comp->_szresult);
											m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_stem[pte_comp->_pumsa], 0, 0, 0, NULL);
											m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
											m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
											nListRowNum += 1;
										}
									}
								}
							}
							pte_comp = pte_comp->_next;
						}
					}
				}
				else//����,����
				{
					if (pte_token->_pstem != NULL)
					{
						if (pte_token->_pstem->_szresult != NULL)
						{
							if (((pte_token->_pstem->_same_count) && (pte_token->_pstem->_same_deputy)) ||
								(pte_token->_pstem->_same_count == 0))
							{
								str_dis = "";
								str_dis += pte_token->_pstem->_szresult;
								if ((pte_token->_pstem->_pumsa == PUMSA_STEM_V) || (pte_token->_pstem->_pumsa == PUMSA_STEM_STA))
									str_dis += "-";
								str_dis += " \t";
								str_dis += pumsa_stem[pte_token->_pstem->_pumsa];
								str_dis += " \t";
								memset(tmp, 0x00, sizeof(tmp));
								sprintf(tmp, "%d", pte_token->_pstem->_same_count + 1);
								str_dis += tmp;

								if ((pte_token->_pstem->_szfeature != NULL) && (pte_token->_pstem->_szfeature[S_T121] == '1'))
								{
									str_dis += " \t";
									//f_user_pumsa_TO_STRING(pte_token->_pstem->_szfeature, "PUMSA", "-", tmp, sizeof(tmp), szpath_ini);
									str_dis += tmp;
								}
								else
								{
									str_dis += " \t-";
								}
								if (pte_token->_pstem->_pumsa == PUMSA_STEM_FOREIGN)
								{
									//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
									strSameCount = "";
									strSameCount.Format(_T("%d"), pte_token->_pstem->_same_count + 1);
									m_StatisticsDataList.InsertItem(nListRowNum, pte_token->_pstem->_szresult);
									m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_stem[pte_token->_pstem->_pumsa], 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
									nListRowNum += 1;
								}
							}
						}
					}
				}
				pte_token = pte_token->_next;
			}
		}
		else if (view_out == _VIEW_OUT_UNREGISTE)//�̵�Ͼ�
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//���վ�
				{
				}
				else//����,����
				{
					if (pte_token->_pstem != NULL)
					{
						if (pte_token->_pstem->_pumsa == PUMSA_STEM_UNREGISTE)
						{
							if (pte_token->_pstem->_szresult != NULL)
							{
								if (((pte_token->_pstem->_same_count) && (pte_token->_pstem->_same_deputy)) ||
									(pte_token->_pstem->_same_count == 0))
								{
									str_dis = "";
									str_dis += pte_token->_pstem->_szresult;
									if ((pte_token->_pstem->_pumsa == PUMSA_STEM_V) || (pte_token->_pstem->_pumsa == PUMSA_STEM_STA))
										str_dis += "-";
									str_dis += " \t";
									str_dis += pumsa_stem[pte_token->_pstem->_pumsa];
									str_dis += " \t";
									memset(tmp, 0x00, sizeof(tmp));
									sprintf(tmp, "%d", pte_token->_pstem->_same_count + 1);
									str_dis += tmp;

									if ((pte_token->_pstem->_szfeature != NULL) && (pte_token->_pstem->_szfeature[S_T121] == '1'))
									{
										str_dis += " \t";
										//f_user_pumsa_TO_STRING(pte_token->_pstem->_szfeature, "PUMSA", "-", tmp, sizeof(tmp), szpath_ini);
										str_dis += tmp;
									}
									else
									{
										str_dis += " \t-";
									}
									//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
									strSameCount = "";
									strSameCount.Format(_T("%d"), pte_token->_pstem->_same_count + 1);
									m_StatisticsDataList.InsertItem(nListRowNum, pte_token->_pstem->_szresult);
									m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_stem[pte_token->_pstem->_pumsa], 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
									nListRowNum += 1;
								}
							}
						}
					}
				}
				pte_token = pte_token->_next;
			}

			//=========
			//���վ�ۿ��� �̵�Ͼ� ã��
			if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//���վ� ����
			{
				pte_comp = m_pte_data->_pte_comp_head->_next;
				while (pte_comp != m_pte_data->_pte_comp_tail)
				{
					if (pte_comp->_pumsa == PUMSA_STEM_UNREGISTE)
					{
						if (pte_comp->_szresult != NULL)//���վ��
						{
							if (((pte_comp->_same_count) && (pte_comp->_same_deputy)) ||
								(pte_comp->_same_count == 0))
							{
								str_dis = "";
								str_dis += pte_comp->_szresult;
								if ((pte_comp->_pumsa == PUMSA_STEM_V) || (pte_comp->_pumsa == PUMSA_STEM_STA))
									str_dis += "-";
								str_dis += " \t";
								str_dis += pumsa_stem[pte_comp->_pumsa];
								str_dis += " \t";
								memset(tmp, 0x00, sizeof(tmp));
								sprintf(tmp, "%d", pte_comp->_same_count + 1);
								str_dis += tmp;

								if ((pte_comp->_szfeature != NULL) && (pte_comp->_szfeature[S_T121] == '1'))
								{
									str_dis += " \t";
									//f_user_pumsa_TO_STRING(pte_comp->_szfeature, "PUMSA", "-", tmp, sizeof(tmp), szpath_ini);
									str_dis += tmp;
								}
								else
								{
									str_dis += " \t-";
								}
								//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
								strSameCount = "";
								strSameCount.Format(_T("%d"), pte_comp->_same_count + 1);
								m_StatisticsDataList.InsertItem(nListRowNum, pte_comp->_szresult);
								m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_stem[pte_comp->_pumsa], 0, 0, 0, NULL);
								m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
								m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
								nListRowNum += 1;
							}
						}
					}
					pte_comp = pte_comp->_next;
				}
			}
			//=======
		}
		else if (view_out == _VIEW_OUT_NEND)//����
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//���վ�
				{
					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//���վ� ����
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						if (pte_token->_pend != NULL)
						{
							if (pte_token->_pend->_szresult != NULL)//���վ� ����
							{
								if (((pte_token->_pend->_same_count) && (pte_token->_pend->_same_deputy)) ||
									(pte_token->_pend->_same_count == 0))
								{
									str_dis = "";
									str_dis += pte_token->_pend->_szresult;
									str_dis += " \t";
									str_dis += pumsa_end[pte_token->_pend->_pumsa];
									str_dis += " \t";
									memset(tmp, 0x00, sizeof(tmp));
									sprintf(tmp, "%d", pte_token->_pend->_same_count + 1);
									str_dis += tmp;

									//����� ���� ����Ʈ �÷�
									str_dis += " \t-";
									if (pte_token->_pend->_pumsa == PUMSA_END_N)
									{
										//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
										strSameCount = "";
										strSameCount.Format(_T("%d"), pte_token->_pend->_same_count + 1);
										m_StatisticsDataList.InsertItem(nListRowNum, pte_token->_pend->_szresult);
										m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_end[pte_token->_pend->_pumsa], 0, 0, 0, NULL);
										m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
										m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
										nListRowNum += 1;
									}
								}
							}
						}
					}
				}
				else//����,����
				{
					if (pte_token->_pend != NULL)
					{
						if (pte_token->_pend->_szresult != NULL)
						{
							if (((pte_token->_pend->_same_count) && (pte_token->_pend->_same_deputy)) ||
								(pte_token->_pend->_same_count == 0))
							{

								str_dis = "";
								str_dis += pte_token->_pend->_szresult;
								str_dis += " \t";
								str_dis += pumsa_end[pte_token->_pend->_pumsa];
								str_dis += " \t";
								memset(tmp, 0x00, sizeof(tmp));
								sprintf(tmp, "%d", pte_token->_pend->_same_count + 1);
								str_dis += tmp;
								//����ڻ��� ����Ʈ �÷�
								str_dis += " \t-";
								if (pte_token->_pend->_pumsa == PUMSA_END_N)
								{
									//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
									strSameCount = "";
									strSameCount.Format(_T("%d"), pte_token->_pend->_same_count + 1);
									m_StatisticsDataList.InsertItem(nListRowNum, pte_token->_pend->_szresult);
									m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_end[pte_token->_pend->_pumsa], 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
									nListRowNum += 1;
								}

							}
						}
					}
				}
				pte_token = pte_token->_next;
			}
		}
		else if (view_out == _VIEW_OUT_VEND)//���
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//���վ�
				{
					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//���վ� ����
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						if (pte_token->_pend != NULL)
						{
							if (pte_token->_pend->_szresult != NULL)//���վ� ����
							{
								if (((pte_token->_pend->_same_count) && (pte_token->_pend->_same_deputy)) ||
									(pte_token->_pend->_same_count == 0))
								{
									str_dis = "";
									str_dis += pte_token->_pend->_szresult;
									str_dis += " \t";
									str_dis += pumsa_end[pte_token->_pend->_pumsa];
									str_dis += " \t";
									memset(tmp, 0x00, sizeof(tmp));
									sprintf(tmp, "%d", pte_token->_pend->_same_count + 1);
									str_dis += tmp;
									//����� ���� ����Ʈ �÷�
									str_dis += " \t-";
									if (pte_token->_pend->_pumsa == PUMSA_END_V)
									{
										//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
										strSameCount = "";
										strSameCount.Format(_T("%d"), pte_token->_pend->_same_count + 1);
										m_StatisticsDataList.InsertItem(nListRowNum, pte_token->_pend->_szresult);
										m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_end[pte_token->_pend->_pumsa], 0, 0, 0, NULL);
										m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
										m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
										nListRowNum += 1;
									}

								}
							}
						}
					}
				}
				else//����,����
				{
					if (pte_token->_pend != NULL)
					{
						if (pte_token->_pend->_szresult != NULL)
						{
							if (((pte_token->_pend->_same_count) && (pte_token->_pend->_same_deputy)) ||
								(pte_token->_pend->_same_count == 0))
							{
								str_dis = "";
								str_dis += pte_token->_pend->_szresult;
								str_dis += " \t";
								str_dis += pumsa_end[pte_token->_pend->_pumsa];
								str_dis += " \t";
								memset(tmp, 0x00, sizeof(tmp));
								sprintf(tmp, "%d", pte_token->_pend->_same_count + 1);
								str_dis += tmp;
								//����ڻ��� ����Ʈ �÷�
								str_dis += " \t-";
								if (pte_token->_pend->_pumsa == PUMSA_END_V)
								{
									//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
									strSameCount = "";
									strSameCount.Format(_T("%d"), pte_token->_pend->_same_count + 1);
									m_StatisticsDataList.InsertItem(nListRowNum, pte_token->_pend->_szresult);
									m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_end[pte_token->_pend->_pumsa], 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
									nListRowNum += 1;
								}
							}
						}
					}
				}
				pte_token = pte_token->_next;
			}
		}
		else if (view_out == _VIEW_OUT_CEND)//�����̺���ü
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//���վ�
				{
					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//���վ� ����
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						if (pte_token->_pend != NULL)
						{
							if (pte_token->_pend->_szresult != NULL)//���վ� ����
							{
								if (((pte_token->_pend->_same_count) && (pte_token->_pend->_same_deputy)) ||
									(pte_token->_pend->_same_count == 0))
								{
									str_dis = "";
									str_dis += pte_token->_pend->_szresult;
									str_dis += " \t";
									str_dis += pumsa_end[pte_token->_pend->_pumsa];
									str_dis += " \t";
									memset(tmp, 0x00, sizeof(tmp));
									sprintf(tmp, "%d", pte_token->_pend->_same_count + 1);
									str_dis += tmp;
									//����ڻ��� ����Ʈ �÷�
									str_dis += " \t-";
									if (pte_token->_pend->_pumsa == PUMSA_END_C)
									{
										//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
										strSameCount = "";
										strSameCount.Format(_T("%d"), pte_token->_pend->_same_count + 1);
										m_StatisticsDataList.InsertItem(nListRowNum, pte_token->_pend->_szresult);
										m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_end[pte_token->_pend->_pumsa], 0, 0, 0, NULL);
										m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
										m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
										nListRowNum += 1;
									}
								}
							}
						}
					}
				}
				else//����,����
				{
					if (pte_token->_pend != NULL)
					{
						if (pte_token->_pend->_szresult != NULL)
						{
							if (((pte_token->_pend->_same_count) && (pte_token->_pend->_same_deputy)) ||
								(pte_token->_pend->_same_count == 0))
							{

								str_dis = "";
								str_dis += pte_token->_pend->_szresult;
								str_dis += " \t";
								str_dis += pumsa_end[pte_token->_pend->_pumsa];
								str_dis += " \t";
								memset(tmp, 0x00, sizeof(tmp));
								sprintf(tmp, "%d", pte_token->_pend->_same_count + 1);
								str_dis += tmp;
								//����ڻ��� ����Ʈ �÷�
								str_dis += " \t-";
								if (pte_token->_pend->_pumsa == PUMSA_END_C)
								{
									//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
									strSameCount = "";
									strSameCount.Format(_T("%d"), pte_token->_pend->_same_count + 1);
									m_StatisticsDataList.InsertItem(nListRowNum, pte_token->_pend->_szresult);
									m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, pumsa_end[pte_token->_pend->_pumsa], 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
									m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
									nListRowNum += 1;
								}
							}
						}
					}
				}
				pte_token = pte_token->_next;
			}
		}
	}
}


void CTabFifthDialog::OnCbnSelchangeStatisticsCombo()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nSel = m_StatisticsCombo.GetCurSel();
	f_print_statistic(nSel);
}


void CTabFifthDialog::OnCbnSelchangeViewCombo()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nSel = m_ViewCombo.GetCurSel();
	f_print_out(nSel);
}
