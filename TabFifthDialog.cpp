// TabFifthDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EngineX.h"
#include "TabFifthDialog.h"
#include "afxdialogex.h"

#include "common.h" //공통사용 정보

// CTabFifthDialog 대화 상자입니다.

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


// CTabFifthDialog 메시지 처리기입니다.


void CTabFifthDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
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

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	
	//통계
	m_StatisticsCombo.AddString("전체");
	m_StatisticsCombo.AddString("기본");
	m_StatisticsCombo.AddString("품사");
	m_StatisticsCombo.AddString("중복어휘");
	m_StatisticsCombo.SetCurSel(0);
	
	//보기
	m_ViewCombo.AddString("전체");
	m_ViewCombo.AddString("어간형태");
	m_ViewCombo.AddString("어미형태");
	m_ViewCombo.AddString("명사");
	m_ViewCombo.AddString("수사");
	m_ViewCombo.AddString("대명사");
	m_ViewCombo.AddString("동사");
	m_ViewCombo.AddString("형용사");
	m_ViewCombo.AddString("관형사");
	m_ViewCombo.AddString("부사");
	m_ViewCombo.AddString("감탄사");
	m_ViewCombo.AddString("기호");
	m_ViewCombo.AddString("숫자");
	m_ViewCombo.AddString("외국문자");
	m_ViewCombo.AddString("미등록어");
	m_ViewCombo.AddString("조사");
	m_ViewCombo.AddString("어미");
	m_ViewCombo.AddString("조사어미복합체");
	m_ViewCombo.SetCurSel(0);


	CRect rect;
	m_StatisticsDataList.GetClientRect(&rect);
	m_StatisticsDataList.DeleteAllItems();
	m_StatisticsDataList.InsertColumn(0, "형태", LVCFMT_CENTER, 250, -1);
	m_StatisticsDataList.InsertColumn(1, "품사", LVCFMT_CENTER, 250, -1);
	m_StatisticsDataList.InsertColumn(2, "빈도수", LVCFMT_CENTER, 150, -1);
	m_StatisticsDataList.InsertColumn(3, "사용자정의", LVCFMT_CENTER, 300, -1);
	m_StatisticsDataList.SetExtendedStyle(m_StatisticsDataList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_StatisticsData.ShowScrollBar(SB_VERT, TRUE);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

//통계
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
		str += "====전체통계====\r\n";

		sprintf(ttt,
			"\r\n 문장: %d"
			"\r\n 어절: %d"
			"\r\n 어간형태: %d"
			"\r\n 어미형태: %d"

			"\r\n\r\n 복합어: %d"
			

			"\r\n\r\n 명사: %d"
			"\r\n 수사: %d"
			"\r\n 대명사: %d"
			"\r\n 동사: %d"
			"\r\n 형용사: %d"
			"\r\n 부사: %d"
			"\r\n 관형사: %d"
			"\r\n 감탄사: %d"
			"\r\n 숫자: %d"
			"\r\n 외국어: %d"
			"\r\n 미등록어: %d"

			"\r\n\r\n 조사: %d"
			"\r\n 어미: %d"
			"\r\n 조사어미 복합체: %d",

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

		str += "\r\n----중복-----\r\n";

		pte_token = m_pte_data->_pte_token_head->_next;
		while (pte_token != m_pte_data->_pte_token_tail)
		{
			if (pte_token->_same_count)
			{
				if (pte_token->_same_deputy)
				{
					memset(ttt, 0x00, 1024);
					sprintf(ttt, "\r\n 어절: %s: %d", pte_token->_sztoken, pte_token->_same_count + 1);
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
						sprintf(ttt, "\r\n 어간형태: %s: %d", pte_token->_pstem->_szresult, pte_token->_pstem->_same_count + 1);
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
						sprintf(ttt, "\r\n 어미형태: %s: %d", pte_token->_pend->_szresult, pte_token->_pend->_same_count + 1);
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
					sprintf(ttt, "\r\n 어간형태: %s: %d", pte_comp->_szresult, pte_comp->_same_count + 1);
					str += ttt;
				}
			}
			pte_comp = pte_comp->_next;
		}
	}
	else if (view_statistic == _VIEW_STATISTIC_NORMAL)
	{
		str += "====기본통계====";

		sprintf(ttt,
			"\r\n\r\n 문장: %d"
			"\r\n 어절: %d"
			"\r\n 어간형태: %d"
			"\r\n 어미형태: %d"

			"\r\n\r\n 복합어: %d",

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
		str += "====품사 통계====";

		sprintf(ttt,
			"\r\n\r\n 명사: %d"
			"\r\n 수사: %d"
			"\r\n 대명사: %d"
			"\r\n 동사: %d"
			"\r\n 형용사: %d"
			"\r\n 부사: %d"
			"\r\n 관형사: %d"
			"\r\n 감탄사: %d"
			"\r\n 숫자: %d"
			"\r\n 외국어: %d"
			"\r\n 미등록어: %d"

			"\r\n\r\n 조사: %d"
			"\r\n 어미: %d"
			"\r\n 조사어미 복합체: %d",

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
		str += "====중복어휘====\r\n";

		pte_token = m_pte_data->_pte_token_head->_next;
		while (pte_token != m_pte_data->_pte_token_tail)
		{
			if (pte_token->_same_count)
			{
				if (pte_token->_same_deputy)
				{
					memset(ttt, 0x00, 1024);
					sprintf(ttt, "\r\n 어절: %s: %d", pte_token->_sztoken, pte_token->_same_count + 1);
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
						sprintf(ttt, "\r\n 어간형태: %s: %d", pte_token->_pstem->_szresult, pte_token->_pstem->_same_count + 1);
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
						sprintf(ttt, "\r\n 어미형태: %s: %d", pte_token->_pend->_szresult, pte_token->_pend->_same_count + 1);
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
					sprintf(ttt, "\r\n 어간형태: %s: %d", pte_comp->_szresult, pte_comp->_same_count + 1);
					str += ttt;
				}
			}
			pte_comp = pte_comp->_next;
		}
	}

	m_StatisticsData.SetWindowText(str);
	
}
//보기
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
				if (pte_token->_iscomp)//복합어
				{
					if (pte_token->_szcomp_token != NULL)//복합어
					{
						if (((pte_token->_same_count) && (pte_token->_same_deputy)) || (pte_token->_same_count == 0))
						{
							str_dis = "";
							str_dis += pte_token->_szcomp_token;
							str_dis += " \t";
							str_dis += "명사\t";
							memset(tmp, 0x00, sizeof(tmp));
							sprintf(tmp, "%d", pte_token->_same_count + 1);
							str_dis += tmp;
							//사용자사전 디폴트 컬럼
							str_dis += " \t-";
							//lv_set_item(hwnd, (LPSTR)(LPCTSTR)str_dis.c_str(), '\t');
							
							strSameCount = "";
							strSameCount.Format(_T("%d"), pte_token->_same_count + 1);
							m_StatisticsDataList.InsertItem(nListRowNum, pte_token->_szcomp_token);
							m_StatisticsDataList.SetItem(nListRowNum, 1, LVIF_TEXT, "명사", 0, 0, 0, NULL);
							m_StatisticsDataList.SetItem(nListRowNum, 2, LVIF_TEXT, strSameCount, 0, 0, 0, NULL);
							m_StatisticsDataList.SetItem(nListRowNum, 3, LVIF_TEXT, "-", 0, 0, 0, NULL);
							nListRowNum += 1;
						}
					}

					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//복합어 스템
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						while (pte_comp != m_pte_data->_pte_comp_tail)
						{
							if (pte_token->_token_offset == pte_comp->_token_offset)
							{
								if (pte_comp->_szresult != NULL)//복합어스템
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
										//사용자사전에서 참조한 자질이있을경우
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
							if (pte_token->_pend->_szresult != NULL)//복합어 엔딩
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
									//사용자사전 필드용 디폴트
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
				else//스템,엔딩
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
								//사용자사전 디폴드 컬럼용
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
		else if (view_out == _VIEW_OUT_STEM)//스템만 출력
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//복합어
				{
					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//복합어 스템
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						while (pte_comp != m_pte_data->_pte_comp_tail)
						{
							if (pte_token->_token_offset == pte_comp->_token_offset)
							{
								if (pte_comp->_szresult != NULL)//복합어스템
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
				else//스템,엔딩
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
		else if (view_out == _VIEW_OUT_END)//엔딩만출력
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//복합어
				{
					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//복합어 스템
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						if (pte_token->_pend != NULL)
						{
							if (pte_token->_pend->_szresult != NULL)//복합어 엔딩
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
									//사용자사전 디폴트 컬럼용
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
				else//스템,엔딩
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
								//사용자사전 디폴트 컬럼용
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
		else if (view_out == _VIEW_OUT_N)//명사만 출력
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//복합어
				{
					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//복합어 스템
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						while (pte_comp != m_pte_data->_pte_comp_tail)
						{
							if (pte_token->_token_offset == pte_comp->_token_offset)
							{
								if (pte_comp->_szresult != NULL)//복합어스템
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
				else//스템,엔딩
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
		else if (view_out == _VIEW_OUT_NUM)//수사만 출력
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//복합어
				{
					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//복합어 스템
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						while (pte_comp != m_pte_data->_pte_comp_tail)
						{
							if (pte_token->_token_offset == pte_comp->_token_offset)
							{
								if (pte_comp->_szresult != NULL)//복합어스템
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
				else//스템,엔딩
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
		else if (view_out == _VIEW_OUT_PRO)//대명사만 출력
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//복합어
				{
					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//복합어 스템
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						while (pte_comp != m_pte_data->_pte_comp_tail)
						{
							if (pte_token->_token_offset == pte_comp->_token_offset)
							{
								if (pte_comp->_szresult != NULL)//복합어스템
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
				else//스템,엔딩
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
		else if (view_out == _VIEW_OUT_V)//동사만 출력
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//복합어
				{
					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//복합어 스템
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						while (pte_comp != m_pte_data->_pte_comp_tail)
						{
							if (pte_token->_token_offset == pte_comp->_token_offset)
							{
								if (pte_comp->_szresult != NULL)//복합어스템
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
				else//스템,엔딩
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
		else if (view_out == _VIEW_OUT_STA)//형용사만 출력
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//복합어
				{
					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//복합어 스템
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						while (pte_comp != m_pte_data->_pte_comp_tail)
						{
							if (pte_token->_token_offset == pte_comp->_token_offset)
							{
								if (pte_comp->_szresult != NULL)//복합어스템
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
				else//스템,엔딩
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
		else if (view_out == _VIEW_OUT_UNC)//관형사만 출력
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//복합어
				{
					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//복합어 스템
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						while (pte_comp != m_pte_data->_pte_comp_tail)
						{
							if (pte_token->_token_offset == pte_comp->_token_offset)
							{
								if (pte_comp->_szresult != NULL)//복합어스템
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
				else//스템,엔딩
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
		else if (view_out == _VIEW_OUT_ADV)//부사만 출력
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//복합어
				{
					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//복합어 스템
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						while (pte_comp != m_pte_data->_pte_comp_tail)
						{
							if (pte_token->_token_offset == pte_comp->_token_offset)
							{
								if (pte_comp->_szresult != NULL)//복합어스템
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
				else//스템,엔딩
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
		else if (view_out == _VIEW_OUT_IC)//감탄사만 출력
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//복합어
				{
					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//복합어 스템
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						while (pte_comp != m_pte_data->_pte_comp_tail)
						{
							if (pte_token->_token_offset == pte_comp->_token_offset)
							{
								if (pte_comp->_szresult != NULL)//복합어스템
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
				else//스템,엔딩
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
		else if (view_out == _VIEW_OUT_SYMBOL)//기호
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//복합어
				{
					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//복합어 스템
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						while (pte_comp != m_pte_data->_pte_comp_tail)
						{
							if (pte_token->_token_offset == pte_comp->_token_offset)
							{
								if (pte_comp->_szresult != NULL)//복합어스템
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
				else//스템,엔딩
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
		else if (view_out == _VIEW_OUT_NUMB)//숫자
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//복합어
				{
					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//복합어 스템
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						while (pte_comp != m_pte_data->_pte_comp_tail)
						{
							if (pte_token->_token_offset == pte_comp->_token_offset)
							{
								if (pte_comp->_szresult != NULL)//복합어스템
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
				else//스템,엔딩
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
		else if (view_out == _VIEW_OUT_FOREIGN)//외국어
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//복합어
				{
					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//복합어 스템
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						while (pte_comp != m_pte_data->_pte_comp_tail)
						{
							if (pte_token->_token_offset == pte_comp->_token_offset)
							{
								if (pte_comp->_szresult != NULL)//복합어스템
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
				else//스템,엔딩
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
		else if (view_out == _VIEW_OUT_UNREGISTE)//미등록어
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//복합어
				{
				}
				else//스템,엔딩
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
			//복합어스템에서 미등록어 찾음
			if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//복합어 스템
			{
				pte_comp = m_pte_data->_pte_comp_head->_next;
				while (pte_comp != m_pte_data->_pte_comp_tail)
				{
					if (pte_comp->_pumsa == PUMSA_STEM_UNREGISTE)
					{
						if (pte_comp->_szresult != NULL)//복합어스템
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
		else if (view_out == _VIEW_OUT_NEND)//조사
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//복합어
				{
					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//복합어 스템
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						if (pte_token->_pend != NULL)
						{
							if (pte_token->_pend->_szresult != NULL)//복합어 엔딩
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

									//사용자 사전 디폴트 컬럼
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
				else//스템,엔딩
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
								//사용자사전 디폴트 컬럼
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
		else if (view_out == _VIEW_OUT_VEND)//어미
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//복합어
				{
					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//복합어 스템
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						if (pte_token->_pend != NULL)
						{
							if (pte_token->_pend->_szresult != NULL)//복합어 엔딩
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
									//사용자 사전 디포트 컬럼
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
				else//스템,엔딩
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
								//사용자사전 디폴트 컬럼
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
		else if (view_out == _VIEW_OUT_CEND)//조사어미복합체
		{
			pte_token = m_pte_data->_pte_token_head->_next;
			while (pte_token != m_pte_data->_pte_token_tail)
			{
				if (pte_token->_iscomp)//복합어
				{
					if ((m_pte_data->_pte_comp_head != NULL) && (m_pte_data->_pte_comp_tail != NULL))//복합어 스템
					{
						pte_comp = m_pte_data->_pte_comp_head->_next;
						if (pte_token->_pend != NULL)
						{
							if (pte_token->_pend->_szresult != NULL)//복합어 엔딩
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
									//사용자사전 디폴트 컬럼
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
				else//스템,엔딩
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
								//사용자사전 디폴트 컬럼
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSel = m_StatisticsCombo.GetCurSel();
	f_print_statistic(nSel);
}


void CTabFifthDialog::OnCbnSelchangeViewCombo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSel = m_ViewCombo.GetCurSel();
	f_print_out(nSel);
}
