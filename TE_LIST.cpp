#include "stdafx.h"
#include "TE_LIST.h"


TE_LIST::TE_LIST()
{
	_pte_data = NULL;
}


TE_LIST::~TE_LIST()
{
	f_del(true);
	_pte_data = NULL;
}

TE_DATA *TE_LIST::f_get_te_data()
{
	return (TE_DATA*)_pte_data;
}
bool TE_LIST::f_init()
{
	if (_pte_data != NULL)
	{
		f_del(true);
		_pte_data = NULL;
	}

	_pte_data = (TE_DATA*)malloc(sizeof(TE_DATA));
	if (_pte_data == NULL)
		return false;


	memset(_pte_data, 0x00, sizeof(TE_DATA));

	//TE_COMP_TOKEN
	_pte_data->_pte_comp_head = NULL;
	_pte_data->_pte_comp_tail = NULL;
	_pte_data->_pte_comp_head = (TE_COMP_TOKEN *)malloc(sizeof(TE_COMP_TOKEN));
	_pte_data->_pte_comp_tail = (TE_COMP_TOKEN *)malloc(sizeof(TE_COMP_TOKEN));
	if (_pte_data->_pte_comp_head == NULL || _pte_data->_pte_comp_tail == NULL)
		return false;

	memset(_pte_data->_pte_comp_head, 0x00, sizeof(TE_COMP_TOKEN));
	memset(_pte_data->_pte_comp_tail, 0x00, sizeof(TE_COMP_TOKEN));

	_pte_data->_pte_comp_head->_next = _pte_data->_pte_comp_tail;
	_pte_data->_pte_comp_head->_prev = _pte_data->_pte_comp_head;
	_pte_data->_pte_comp_tail->_next = _pte_data->_pte_comp_tail;
	_pte_data->_pte_comp_tail->_prev = _pte_data->_pte_comp_head;

	//TE_TOKEN
	_pte_data->_pte_token_head = NULL;
	_pte_data->_pte_token_tail = NULL;
	_pte_data->_pte_token_head = (TE_TOKEN *)malloc(sizeof(TE_TOKEN));
	_pte_data->_pte_token_tail = (TE_TOKEN *)malloc(sizeof(TE_TOKEN));
	if (_pte_data->_pte_token_head == NULL || _pte_data->_pte_token_tail == NULL)
		return false;

	memset(_pte_data->_pte_token_head, 0x00, sizeof(TE_TOKEN));
	memset(_pte_data->_pte_token_tail, 0x00, sizeof(TE_TOKEN));

	_pte_data->_pte_token_head->_next = _pte_data->_pte_token_tail;
	_pte_data->_pte_token_head->_prev = _pte_data->_pte_token_head;
	_pte_data->_pte_token_tail->_next = _pte_data->_pte_token_tail;
	_pte_data->_pte_token_tail->_prev = _pte_data->_pte_token_head;

	//TE_SENTENCE
	_pte_data->_pte_sentence_head = NULL;
	_pte_data->_pte_sentence_tail = NULL;
	_pte_data->_pte_sentence_head = (TE_SENTENCE *)malloc(sizeof(TE_SENTENCE));
	_pte_data->_pte_sentence_tail = (TE_SENTENCE *)malloc(sizeof(TE_SENTENCE));
	if (_pte_data->_pte_sentence_head == NULL || _pte_data->_pte_sentence_tail == NULL)
		return false;

	memset(_pte_data->_pte_sentence_head, 0x00, sizeof(TE_SENTENCE));
	memset(_pte_data->_pte_sentence_tail, 0x00, sizeof(TE_SENTENCE));

	_pte_data->_pte_sentence_head->_next = _pte_data->_pte_sentence_tail;
	_pte_data->_pte_sentence_head->_prev = _pte_data->_pte_sentence_head;
	_pte_data->_pte_sentence_tail->_next = _pte_data->_pte_sentence_tail;
	_pte_data->_pte_sentence_tail->_prev = _pte_data->_pte_sentence_head;

	return true;
}

bool TE_LIST::f_add(void *node, int add_type)
{
	TE_SENTENCE *pte_sentence = NULL;
	TE_TOKEN *pte_token = NULL;
	TE_COMP_TOKEN *pte_comp = NULL;

	switch (add_type)
	{
	case TYPE_TE_SENTENCE:
		pte_sentence = (TE_SENTENCE*)node;
		if ((_pte_data == NULL) || (pte_sentence == NULL))
		{
			return false;
		}
		_pte_data->_pte_sentence_tail->_prev->_next = pte_sentence;
		pte_sentence->_prev = _pte_data->_pte_sentence_tail->_prev;
		_pte_data->_pte_sentence_tail->_prev = pte_sentence;
		pte_sentence->_next = _pte_data->_pte_sentence_tail;
		break;
	case TYPE_TE_TOKEN:
		pte_token = (TE_TOKEN*)node;
		if ((_pte_data == NULL) || (pte_token == NULL))
		{
			return false;
		}
		_pte_data->_pte_token_tail->_prev->_next = pte_token;
		pte_token->_prev = _pte_data->_pte_token_tail->_prev;
		_pte_data->_pte_token_tail->_prev = pte_token;
		pte_token->_next = _pte_data->_pte_token_tail;
		break;
	case TYPE_TE_COMP:
		pte_comp = (TE_COMP_TOKEN*)node;
		if ((_pte_data == NULL) || (pte_comp == NULL))
		{
			return false;
		}
		_pte_data->_pte_comp_tail->_prev->_next = pte_comp;
		pte_comp->_prev = _pte_data->_pte_comp_tail->_prev;
		_pte_data->_pte_comp_tail->_prev = pte_comp;
		pte_comp->_next = _pte_data->_pte_comp_tail;
		break;
	default:
		return false;
	}

	return true;
}

bool TE_LIST::f_del(bool del_self)
{
	if (_pte_data == NULL)
		return false;

	if (del_self)
	{
		f_del(TYPE_TE_COMP, true);
		f_del(TYPE_TE_TOKEN, true);
		f_del(TYPE_TE_SENTENCE, true);
		if (_pte_data != NULL)
		{
			free(_pte_data);
			_pte_data = NULL;
		}
	}
	else
	{	//statistic
		_pte_data->_cnt_sentence = 0;
		_pte_data->_cnt_token = 0;
		_pte_data->_cnt_stem = 0;
		_pte_data->_cnt_end = 0;
		_pte_data->_cnt_comp = 0;
		_pte_data->_cnt_comp_stem = 0;
		_pte_data->_cnt_comp_end = 0;

		_pte_data->_cnt_same_stem = 0;
		_pte_data->_cnt_same_end = 0;
		_pte_data->_cnt_same_comp = 0;

		//stem
		_pte_data->_cnt_noun = 0;
		_pte_data->_cnt_numeral = 0;
		_pte_data->_cnt_pronoun = 0;
		_pte_data->_cnt_actverb = 0;
		_pte_data->_cnt_stateverb = 0;
		_pte_data->_cnt_adv = 0;
		_pte_data->_cnt_unc = 0;
		_pte_data->_cnt_interjection = 0;
		_pte_data->_cnt_numb = 0;
		_pte_data->_cnt_foreign = 0;

		//ending
		_pte_data->_cnt_nend = 0;
		_pte_data->_cnt_vend = 0;
		_pte_data->_cnt_cend = 0;

		_pte_data->_self_id = 0;

		f_del(TYPE_TE_COMP, false);
		f_del(TYPE_TE_TOKEN, false);
		f_del(TYPE_TE_SENTENCE, false);
	}
	return true;
}
bool TE_LIST::f_del(int del_type, bool del_head_tail)
{
	if (_pte_data == NULL)
		return false;

	TE_SENTENCE *pte_sentence, *pte_sentence_del;
	TE_TOKEN *pte_token, *pte_token_del;
	TE_COMP_TOKEN *pte_comp, *pte_comp_del;
	pte_sentence = pte_sentence_del = NULL;
	pte_token = pte_token_del = NULL;
	pte_comp = pte_comp_del = NULL;

	switch (del_type)
	{
	case TYPE_TE_SENTENCE:
		if ((_pte_data->_pte_sentence_head == NULL) || (_pte_data->_pte_sentence_tail == NULL))
		{
			return false;
		}
		pte_sentence = _pte_data->_pte_sentence_head->_next;
		while (pte_sentence != _pte_data->_pte_sentence_tail)
		{
			pte_sentence_del = pte_sentence;
			pte_sentence = pte_sentence->_next;
			if (pte_sentence_del != NULL)
			{
				free(pte_sentence_del);
				pte_sentence_del = NULL;
			}
		}
		if (del_head_tail)
		{
			if (_pte_data->_pte_sentence_head != NULL)
			{
				free(_pte_data->_pte_sentence_head);
				_pte_data->_pte_sentence_head = NULL;
			}
			if (_pte_data->_pte_sentence_tail != NULL)
			{
				free(_pte_data->_pte_sentence_tail);
				_pte_data->_pte_sentence_tail = NULL;
			}
		}
		else
		{
			_pte_data->_pte_sentence_head->_next = _pte_data->_pte_sentence_tail;
			_pte_data->_pte_sentence_tail->_prev = _pte_data->_pte_sentence_head;
		}
		break;
	case TYPE_TE_TOKEN:
		if ((_pte_data->_pte_token_head == NULL) || (_pte_data->_pte_token_tail == NULL))
		{
			return false;
		}
		pte_token = _pte_data->_pte_token_head->_next;
		while (pte_token != _pte_data->_pte_token_tail)
		{
			pte_token_del = pte_token;
			pte_token = pte_token->_next;
			if (pte_token_del != NULL)
			{
				f_del(pte_token_del);
				//free(pte_token_del);
				//pte_token_del=NULL;
			}
		}
		if (del_head_tail)
		{
			if (_pte_data->_pte_token_head != NULL)
			{
				free(_pte_data->_pte_token_head);
				_pte_data->_pte_token_head = NULL;
			}
			if (_pte_data->_pte_token_tail != NULL)
			{
				free(_pte_data->_pte_token_tail);
				_pte_data->_pte_token_tail = NULL;
			}
		}
		else
		{
			_pte_data->_pte_token_head->_next = _pte_data->_pte_token_tail;
			_pte_data->_pte_token_tail->_prev = _pte_data->_pte_token_head;
		}
		break;
	case TYPE_TE_COMP:
		if ((_pte_data->_pte_comp_head == NULL) || (_pte_data->_pte_comp_tail == NULL))
		{
			return false;
		}
		pte_comp = _pte_data->_pte_comp_head->_next;
		while (pte_comp != _pte_data->_pte_comp_tail)
		{
			pte_comp_del = pte_comp;
			pte_comp = pte_comp->_next;
			if (pte_comp_del != NULL)
			{
				f_del(pte_comp_del);
				//free(pte_comp_del);
				//pte_comp_del=NULL;
			}
		}
		if (del_head_tail)
		{
			if (_pte_data->_pte_comp_head != NULL)
			{
				free(_pte_data->_pte_comp_head);
				_pte_data->_pte_comp_head = NULL;
			}
			if (_pte_data->_pte_comp_tail != NULL)
			{
				free(_pte_data->_pte_comp_tail);
				_pte_data->_pte_comp_tail = NULL;
			}
		}
		else
		{
			_pte_data->_pte_comp_head->_next = _pte_data->_pte_comp_tail;
			_pte_data->_pte_comp_tail->_prev = _pte_data->_pte_comp_head;
		}
		break;
	}

	return true;
}

bool TE_LIST::f_del(TE_TOKEN *pte_token)
{
	if (pte_token == NULL)
		return false;

	if (pte_token->_sztoken != NULL)//토큰삭제
	{
		free(pte_token->_sztoken);
		pte_token->_sztoken = NULL;
	}
	if (pte_token->_szcomp_token != NULL)//복합어(어미(조사)제외된복합어)삭제
	{
		free(pte_token->_szcomp_token);
		pte_token->_szcomp_token = NULL;
	}
	if (pte_token->_szambend != NULL)//중의성이발생한 엔딩문자열 삭제
	{
		free(pte_token->_szambend);
		pte_token->_szambend = NULL;
	}
	if (pte_token->_pstem != NULL)//스템삭제
	{
		if (pte_token->_pstem->_szresult != NULL)
		{
			free(pte_token->_pstem->_szresult);
			pte_token->_pstem->_szresult = NULL;
		}
		free(pte_token->_pstem);
		pte_token->_pstem = NULL;
	}
	if (pte_token->_pend != NULL)//엔딩삭제
	{
		if (pte_token->_pend->_szresult != NULL)
		{
			free(pte_token->_pend->_szresult);
			pte_token->_pend->_szresult = NULL;
		}
		free(pte_token->_pend);
		pte_token->_pend = NULL;
	}
	if (pte_token != NULL)//노드삭제
	{
		free(pte_token);
		pte_token = NULL;
	}
	return true;
}

bool TE_LIST::f_del(TE_COMP_TOKEN *pte_comp)
{
	if (pte_comp == NULL)
		return false;

	if (pte_comp->_szresult != NULL)//복합어 구성문자열(스템) 삭제
	{
		free(pte_comp->_szresult);
		pte_comp->_szresult = NULL;
	}
	if (pte_comp != NULL)//복합어 구성노드 삭제
	{
		free(pte_comp);
		pte_comp = NULL;
	}
	return true;
}

//PreProcessor에있는데이타를 정리하여 te_data에 복사
void TE_LIST::f_migration(CPreProcessor *ppreprocessor)
{
	if (_pte_data != NULL)
	{
		f_del(true);
	}
	f_init();

	if ((_pte_data == NULL) || (ppreprocessor == NULL) || (ppreprocessor->m_pDocVector == NULL))
		return;

	SENTENCE* psentence = NULL;
	psentence = ppreprocessor->m_pDocVector->GoSentenceFirst();

	if (psentence != NULL)//문장리스트의 첫번째문장
	{
		while (psentence != ppreprocessor->m_pDocVector->m_pTailSentence)
		{
			f_migration(psentence);
			psentence = psentence->pNext;
		}
	}
	f_vindosu_statistic();
	//2018-07-18 Preprocessor 클래스 결과 탭 여러 윈도우에서 사용함으로 인해 삭제하지 않게 처리한다.
	//ppreprocessor->RemoveDocVectorData();
}

void TE_LIST::f_migration(SENTENCE *psentence)
{
	if ((_pte_data == NULL) || (psentence == NULL) ||
		(_pte_data->_pte_sentence_head == NULL) || (_pte_data->_pte_sentence_tail == NULL))
		return;

	int sentence_offset;
	int sentence_len;
	TE_SENTENCE *pte_sentence = NULL;

	sentence_offset = psentence->nOffset;
	sentence_len = strlen(psentence->szSentence);

	pte_sentence = (TE_SENTENCE*)malloc(sizeof(TE_SENTENCE));
	if (pte_sentence != NULL)
	{
		pte_sentence->_sentence_offset = sentence_offset;
		pte_sentence->_sentence_len = sentence_len;
		if (f_add((TE_SENTENCE*)pte_sentence, TYPE_TE_SENTENCE))
		{
			_pte_data->_cnt_sentence++;//문장갯수
		}
	}

	H_NODE* pnode = NULL;
	pnode = psentence->pHead->pNext;

	if (pnode != NULL)//토큰리스트첫번째원소
	{
		while (pnode != psentence->pTail)
		{
			if (pnode->nSkip == 0)
			{
				f_migration(pnode->pHToken, sentence_offset, sentence_len);
			}
			pnode = pnode->pNext;
		}
	}
}

void TE_LIST::f_migration(H_TOKEN *ph_token, int sentence_offset, int sentence_len)
{
	if ((_pte_data == NULL) || (ph_token == NULL) || (_pte_data->_pte_token_head == NULL) ||
		(_pte_data->_pte_token_head == NULL))
		return;

	int comp_self_id = 0;
	H_SUBNODE *ph_subnode = NULL;//복합명사리스트

	TE_COMP_TOKEN *pte_comp = NULL;
	TE_TOKEN *pte_token = NULL;

	pte_token = (TE_TOKEN*)malloc(sizeof(TE_TOKEN));
	if (pte_token != NULL)
	{
		memset(pte_token, 0x00, sizeof(TE_TOKEN));

		pte_token->_sentence_offset = sentence_offset;
		pte_token->_sentence_len = sentence_len;
		pte_token->_token_offset = ph_token->nOffset;
		

		if (ph_token->strOriginalSource != NULL)//토큰(단어)
		{
			pte_token->_sztoken = (char*)malloc(strlen(ph_token->strOriginalSource) + 1);
			if (pte_token->_sztoken != NULL)
			{
				memset(pte_token->_sztoken, 0x00, strlen(ph_token->strOriginalSource) + 1);
				strcpy(pte_token->_sztoken, ph_token->strOriginalSource);
			}
		}

		if (ph_token->strAmbEndingSource != NULL)//중의성이 발생한 엔딩문자열
		{
			if (strlen(ph_token->strAmbEndingSource))
			{
				pte_token->_szambend = (char*)malloc(strlen(ph_token->strAmbEndingSource) + 1);
				if (pte_token->_szambend != NULL)
				{
					memset(pte_token->_szambend, 0x00, strlen(ph_token->strAmbEndingSource) + 1);
					strcpy(pte_token->_szambend, ph_token->strAmbEndingSource);
					_pte_data->_cnt_ambend++;//중의성이발생한 엔딩문자열 갯수
				}
			}
		}

		pte_token->_iscomp = ph_token->bSubCompCreate;
		if (ph_token->bSubCompCreate)//복합어라면 pte_token->_szcomp_token의 값을 채운다.그리고 pte_token->_comp 리스트를 채운다
		{
			if (ph_token->strOriginalSource != NULL)
			{
				pte_token->_szcomp_token = (char*)malloc(strlen(ph_token->strOriginalSource) + 1);
				if (pte_token->_szcomp_token != NULL)
				{
					memset(pte_token->_szcomp_token, 0x00, strlen(ph_token->strOriginalSource) + 1);
					_pte_data->_cnt_comp++;//복합어갯수
					//_pte_data->_cnt_unregiste++;//복합어는 미등록어로 처리//하려했으나 명사로하라 그래서 주석처리
				}
			}

			if ((ph_token->pSubHeadNode != NULL) && (ph_token->pSubTailNode != NULL))
			{
				ph_subnode = ph_token->pSubHeadNode->pNext;
				if (ph_subnode != NULL)//복합어구성 스템리스트 포인터
				{
					while (ph_subnode != ph_token->pSubTailNode)//복합어구성 스템리스트 루프
					{
						if (ph_subnode->pCSubToken != NULL)
						{
							if (ph_subnode->pCSubToken->strResult != NULL)
							{
								if (pte_token->_szcomp_token != NULL)
								{
									if ((strlen(pte_token->_szcomp_token) + strlen(ph_subnode->pCSubToken->strResult)) <= strlen(ph_token->strOriginalSource))
										strcat(pte_token->_szcomp_token, ph_subnode->pCSubToken->strResult);
								}
							}
							if (ph_subnode->pCSubToken->strResult != NULL)
							{
								pte_comp = (TE_COMP_TOKEN*)malloc(sizeof(TE_COMP_TOKEN));
								if (pte_comp != NULL)//복합어구성 스템등록 
								{
									memset(pte_comp, 0x00, sizeof(TE_COMP_TOKEN));

									pte_comp->_sentence_offset = sentence_offset;
									pte_comp->_sentence_len = sentence_len;
									pte_comp->_token_offset = ph_token->nOffset;
									pte_comp->_self_id = comp_self_id++;

									//pte_comp->_same_count=0;
									pte_comp->_kind = ph_subnode->pCSubToken->nKindChar;
									f_kind_cnt(pte_comp->_kind);
									//if(ph_subnode->pCSubToken->strResult != NULL)
									//{
									pte_comp->_szresult = (char*)malloc(strlen(ph_subnode->pCSubToken->strResult) + 1);
									if (pte_comp->_szresult != NULL)
									{
										memset(pte_comp->_szresult, 0x00, strlen(ph_subnode->pCSubToken->strResult) + 1);
										strcpy(pte_comp->_szresult, ph_subnode->pCSubToken->strResult);
									}
									//}
									if (ph_subnode->pCSubToken->strFeature != NULL)
									{
										strcpy(pte_comp->_szfeature, ph_subnode->pCSubToken->strFeature);
										pte_comp->_pumsa = f_pumsa_cnt(IAM_STEM, pte_comp->_kind, pte_comp->_szfeature);
									}
									if (f_add((TE_COMP_TOKEN*)pte_comp, TYPE_TE_COMP))
									{
										_pte_data->_cnt_comp_stem++;//복합어 스템갯수;
										_pte_data->_cnt_stem++;//스템갯수
									}
									pte_comp = NULL;
								}
							}
						}
						ph_subnode = ph_subnode->pNext;
					}
					if (ph_token->subEnding != NULL)//복합어의 조사(어미)가 유효한지 체크
					{
						//어미(조사)가 있으면 등록
						//복합어가 어미(조사)를 가지고있는지 체크(두 문자열이 같지않으면 조사(어미)가 있다)
						if ((pte_token->_sztoken != NULL) && (pte_token->_szcomp_token != NULL))
						{
							if (strcmp(pte_token->_sztoken, pte_token->_szcomp_token) != 0)
							{
								if (ph_token->subEnding->strResult != NULL)
								{
									pte_token->_pend = (TE_END_TOKEN*)malloc(sizeof(TE_END_TOKEN));
									if (pte_token->_pend != NULL)
									{
										memset(pte_token->_pend, 0x00, sizeof(TE_END_TOKEN));

										pte_token->_pend->_sentence_offset = sentence_offset;
										pte_token->_pend->_sentence_len = sentence_len;
										pte_token->_pend->_token_offset = ph_token->nOffset;
										//pte_token->_pend->_same_count=0;

										pte_token->_pend->_iscomp = TRUE;
										pte_token->_pend->_kind = ph_token->subEnding->nKindChar;
										f_kind_cnt(pte_token->_pend->_kind);

										//if(ph_token->subEnding->strResult != NULL)
										//{
										pte_token->_pend->_szresult = (char*)malloc(strlen(ph_token->subEnding->strResult) + 1);
										if (pte_token->_pend->_szresult != NULL)
										{
											memset(pte_token->_pend->_szresult, 0x00, strlen(ph_token->subEnding->strResult) + 1);
											strcpy(pte_token->_pend->_szresult, ph_token->subEnding->strResult);
										}
										//}
										if (ph_token->subEnding->strFeature != NULL)
										{
											strcpy(pte_token->_pend->_szfeature, ph_token->subEnding->strFeature);
										}
										pte_token->_pend->_pumsa = f_pumsa_cnt(IAM_END, pte_token->_pend->_kind, pte_token->_pend->_szfeature);
										_pte_data->_cnt_comp_end++;//복합어 엔딩갯수
										_pte_data->_cnt_end++;//엔딩갯수
									}
								}
							}
						}
					}
				}
			}
		}
		else//복합어가 아니라면
		{
			if (ph_token->subStem != NULL)//스템 등록
			{
				if (ph_token->subStem->strResult != NULL)
				{
					pte_token->_pstem = (TE_STEM_TOKEN*)malloc(sizeof(TE_STEM_TOKEN));
					if (pte_token->_pstem != NULL)
					{
						memset(pte_token->_pstem, 0x00, sizeof(TE_STEM_TOKEN));

						pte_token->_pstem->_sentence_offset = sentence_offset;
						pte_token->_pstem->_sentence_len = sentence_len;
						pte_token->_pstem->_token_offset = ph_token->nOffset;
						//pte_token->_pstem->_same_count=0;

						pte_token->_pstem->_kind = ph_token->subStem->nKindChar;
						f_kind_cnt(pte_token->_pstem->_kind);
						//if(ph_token->subStem->strResult != NULL)
						//{
						pte_token->_pstem->_szresult = (char*)malloc(strlen(ph_token->subStem->strResult) + 1);
						if (pte_token->_pstem->_szresult != NULL)
						{
							memset(pte_token->_pstem->_szresult, 0x00, strlen(ph_token->subStem->strResult) + 1);
							strcpy(pte_token->_pstem->_szresult, ph_token->subStem->strResult);
						}
						//}
						if (ph_token->subStem->strFeature != NULL)
						{
							strcpy(pte_token->_pstem->_szfeature, ph_token->subStem->strFeature);
						}
						pte_token->_pstem->_pumsa = f_pumsa_cnt(IAM_STEM, pte_token->_pstem->_kind, pte_token->_pstem->_szfeature);
						_pte_data->_cnt_stem++;//스템갯수
					}
				}
			}
			if (ph_token->subEnding != NULL)//엔딩등록
			{
				if (ph_token->subEnding->strResult != NULL)
				{
					if (strlen(ph_token->subEnding->strResult) > 0)
					{
						pte_token->_pend = (TE_END_TOKEN*)malloc(sizeof(TE_END_TOKEN));
						if (pte_token->_pend != NULL)
						{
							memset(pte_token->_pend, 0x00, sizeof(TE_END_TOKEN));

							pte_token->_pend->_sentence_offset = sentence_offset;
							pte_token->_pend->_sentence_len = sentence_len;
							pte_token->_pend->_token_offset = ph_token->nOffset;
							//pte_token->_pend->_same_count=0;

							pte_token->_pend->_kind = ph_token->subEnding->nKindChar;
							f_kind_cnt(pte_token->_pend->_kind);
							//if(ph_token->subEnding->strResult != NULL)
							//{
							pte_token->_pend->_szresult = (char*)malloc(strlen(ph_token->subEnding->strResult) + 1);
							if (pte_token->_pend->_szresult != NULL)
							{
								memset(pte_token->_pend->_szresult, 0x00, strlen(ph_token->subEnding->strResult) + 1);
								strcpy(pte_token->_pend->_szresult, ph_token->subEnding->strResult);
							}
							//}
							if (ph_token->subEnding->strFeature != NULL)
							{
								strcpy(pte_token->_pend->_szfeature, ph_token->subEnding->strFeature);
							}
							pte_token->_pend->_pumsa = f_pumsa_cnt(IAM_END, pte_token->_pend->_kind, pte_token->_pend->_szfeature);
							_pte_data->_cnt_end++;//엔딩갯수
						}
					}
				}
			}
		}
		if (f_add((TE_TOKEN*)pte_token, TYPE_TE_TOKEN))
		{
			_pte_data->_cnt_token++;//토큰(단어)갯수
		}
	}
}
int TE_LIST::f_pumsa_cnt(int iam_stem_end, int kind, char *feature)//품사별 갯수 증가도우미
{
	if (feature == NULL)
		return 0;

	int pumsa = 0;

	switch (iam_stem_end)
	{
	case IAM_STEM:
		if ((kind == ENGLISH_CHAR) || (kind == JAPAN_CHAR))
		{
			pumsa = PUMSA_STEM_FOREIGN;
			_pte_data->_cnt_foreign++;
		}
		else
		{
			if (feature[S_Scmp] == '1')//미등록어
			{
				_pte_data->_cnt_unregiste++;
				pumsa = PUMSA_STEM_UNREGISTE;
			}
			else if ((feature[S_N] == '1') && (feature[S_Num] == '1'))//수사
			{
				_pte_data->_cnt_numeral++;
				pumsa = PUMSA_STEM_NUM;
			}
			else if ((feature[S_N] == '1') && (feature[S_Pro] == '1'))//대명사
			{
				_pte_data->_cnt_pronoun++;
				pumsa = PUMSA_STEM_PRO;
			}
			else if (feature[S_Numb] == '1')//숫자
			{
				_pte_data->_cnt_numb++;
				pumsa = PUMSA_STEM_NUMB;
			}
			else if (feature[S_N] == '1')//명사
			{
				_pte_data->_cnt_noun++;
				pumsa = PUMSA_STEM_N;
			}
			else if (((feature[S_V] == '1') && (feature[S_Sta] == '1')) || ((feature[S_Cmp] == '1') && (feature[S_Sta] == '1')))//형용사
			{
				_pte_data->_cnt_stateverb++;
				pumsa = PUMSA_STEM_STA;
			}
			else if ((feature[S_V] == '1') || (feature[S_Cmp] == '1'))//동사
			{
				_pte_data->_cnt_actverb++;
				pumsa = PUMSA_STEM_V;
			}
			else if (feature[S_Adv] == '1')//부사
			{
				_pte_data->_cnt_adv++;
				pumsa = PUMSA_STEM_ADV;
			}
			else if (feature[S_Unc] == '1')//관형사(수식언)
			{
				_pte_data->_cnt_unc++;
				pumsa = PUMSA_STEM_UNC;
			}
			else if (feature[S_IC] == '1')//감탄사
			{
				_pte_data->_cnt_interjection++;
				pumsa = PUMSA_STEM_IC;
			}
		}
		break;
	case IAM_END:
		if ((feature[E_N] == '1') && (feature[E_V] == '1'))//조사어미 복합체
		{
			_pte_data->_cnt_cend++;
			pumsa = PUMSA_END_C;
		}
		else if (feature[E_N] == '1')//조사
		{
			_pte_data->_cnt_nend++;
			pumsa = PUMSA_END_N;
		}
		else if (feature[E_V] == '1')//어미
		{
			_pte_data->_cnt_vend++;
			pumsa = PUMSA_END_V;
		}
		break;
	}
	return pumsa;
}
/*
#define KOREAN_CHAR 0
#define CHINESE_CHAR 1
#define JAPAN_CHAR 2
#define ENGLISH_CHAR 4
#define NUMBER_CHAR 5
#define ECT_CHAR 6
#define HANGUL_CHAR_WORD   1// 한글
#define CHINESE_CHAR_WORD  2// 한자
#define KATAGANA_CHAR_WORD 3// 가카가나
#define HIRAGANA_CHAR_WORD 4// 히라가나
#define ENGLISH_CHAR_WORD  5// 영어
#define NUMBER_CHAR_WORD   6// 숫자
#define SINGLE_CHAR_WORD   7// 단일문장부호
#define TWIN_CHAR_WORD     8// 결합쌍문장부호
#define ASCII_CHAR_WORD    9// ASCII 코드
#define SPECIAL_CHAR_WORD  10// 특수문자
*/
void TE_LIST::f_kind_cnt(int kind)
{
	switch (kind)
	{
	case KOREAN_CHAR:
		_pte_data->_cnt_corea_char++;
		break;
	case CHINESE_CHAR:
		_pte_data->_cnt_chinese_char++;
		break;
	case JAPAN_CHAR:
		_pte_data->_cnt_japan_char++;
		break;
	case ENGLISH_CHAR:
		_pte_data->_cnt_english_char++;
		break;
	case NUMBER_CHAR:
		_pte_data->_cnt_number_char++;
		break;
	case ECT_CHAR:
		_pte_data->_cnt_etc_char++;
		break;
		//default:
		//	_pte_data->_cnt_etc_char++;
		//	break;
	}
}

void TE_LIST::f_vindosu_statistic()//빈도수 통계
{
	if (_pte_data == NULL)
		return;

	TE_COMP_TOKEN *pte_comp = NULL;
	TE_COMP_TOKEN *pte_comp_a, *pte_comp_b;

	TE_TOKEN *pte_token_a, *pte_token_b;

	pte_comp_a = pte_comp_b = NULL;
	pte_token_a = pte_token_b = NULL;

	if ((_pte_data->_pte_token_head != NULL) && (_pte_data->_pte_token_tail != NULL))
	{
		pte_token_a = _pte_data->_pte_token_head->_next;

		if (pte_token_a != NULL)
		{
			while (pte_token_a != _pte_data->_pte_token_tail)
			{
				pte_token_b = pte_token_a->_next;
				while (pte_token_b != _pte_data->_pte_token_tail)
				{
					//중복토큰 체크
					if ((pte_token_a->_szcomp_token != NULL) && (pte_token_b->_szcomp_token != NULL))
					{
						if (strcmp(pte_token_a->_szcomp_token, pte_token_b->_szcomp_token) == 0)
						{
							if (pte_token_a->_same_count == 0)
							{
								pte_token_a->_same_deputy = TRUE;
							}
							pte_token_a->_same_count++;
							if (pte_token_a->_same_count >= pte_token_b->_same_count)
							{
								pte_token_b->_same_count = pte_token_a->_same_count;
							}
							else
							{
								pte_token_a->_same_count = pte_token_b->_same_count;
							}
						}
					}

					//스템체크
					if ((pte_token_a->_pstem != NULL) && (pte_token_b->_pstem != NULL))
					{
						if ((pte_token_a->_pstem->_szresult != NULL) && (pte_token_b->_pstem->_szresult != NULL))
						{
							if (strcmp(pte_token_a->_pstem->_szresult, pte_token_b->_pstem->_szresult) == 0)
							{
								if ((pte_token_a->_pstem->_szfeature != NULL) && ((pte_token_b->_pstem->_szfeature != NULL)))
								{
									if (pte_token_a->_pstem->_pumsa == pte_token_b->_pstem->_pumsa)
										//if(strcmp(pte_token_a->_pstem->_szfeature,pte_token_b->_pstem->_szfeature)==0)
									{
										if (pte_token_a->_pstem->_same_count == 0)
										{
											pte_token_a->_pstem->_same_deputy = TRUE;
										}
										pte_token_a->_pstem->_same_count++;
										if (pte_token_a->_pstem->_same_count >= pte_token_b->_pstem->_same_count)
										{
											pte_token_b->_pstem->_same_count = pte_token_a->_pstem->_same_count;
										}
										else
										{
											pte_token_a->_pstem->_same_count = pte_token_b->_pstem->_same_count;
										}
									}
								}
							}
						}
					}


					//엔딩 체크
					if ((pte_token_a->_pend != NULL) && (pte_token_b->_pend != NULL))
					{
						if ((pte_token_a->_pend->_szresult != NULL) && (pte_token_b->_pend->_szresult != NULL))
						{
							if (strcmp(pte_token_a->_pend->_szresult, pte_token_b->_pend->_szresult) == 0)
							{
								if ((pte_token_a->_pend->_szfeature != NULL) && ((pte_token_b->_pend->_szfeature != NULL)))
								{
									if (pte_token_a->_pend->_pumsa == pte_token_b->_pend->_pumsa)
										//if(strcmp(pte_token_a->_pend->_szfeature,pte_token_b->_pend->_szfeature)==0)
									{
										if (pte_token_a->_pend->_same_count == 0)
										{
											pte_token_a->_pend->_same_deputy = TRUE;
										}
										pte_token_a->_pend->_same_count++;
										if (pte_token_a->_pend->_same_count >= pte_token_b->_pend->_same_count)
										{
											pte_token_b->_pend->_same_count = pte_token_a->_pend->_same_count;
										}
										else
										{
											pte_token_a->_pend->_same_count = pte_token_b->_pend->_same_count;
										}
									}
								}
							}
						}
					}
					pte_token_b = pte_token_b->_next;
				}

				//복합어스템 체크
				if ((_pte_data->_pte_comp_head != NULL) && (_pte_data->_pte_comp_tail != NULL))
				{
					pte_comp = _pte_data->_pte_comp_head->_next;
					if (pte_comp != NULL)
					{
						while (pte_comp != _pte_data->_pte_comp_tail)
						{
							if ((pte_token_a->_pstem != NULL) && (pte_comp != NULL))
							{
								if ((pte_token_a->_pstem->_szresult != NULL) && (pte_comp->_szresult != NULL))
								{
									if (strcmp(pte_token_a->_pstem->_szresult, pte_comp->_szresult) == 0)
									{
										if ((pte_token_a->_pstem->_szfeature != NULL) && ((pte_comp->_szfeature != NULL)))
										{
											if (pte_token_a->_pstem->_pumsa == pte_comp->_pumsa)
												//if(strcmp(pte_token_a->_pstem->_szfeature,pte_comp->_szfeature)==0)
											{
												if (pte_token_a->_pstem->_same_count == 0)
												{
													pte_token_a->_pstem->_same_deputy = TRUE;
												}
												pte_token_a->_pstem->_same_count++;
												if (pte_token_a->_pstem->_same_count >= pte_comp->_same_count)
												{
													pte_comp->_same_count = pte_token_a->_pstem->_same_count;
												}
												else
												{
													pte_token_a->_pstem->_same_count = pte_comp->_same_count;
												}
											}
										}
									}
								}
							}
							pte_comp = pte_comp->_next;
						}
					}
				}

				pte_token_a = pte_token_a->_next;
			}
		}
	}

	//복합어스템 단독 체크
	if ((_pte_data->_pte_comp_head != NULL) && (_pte_data->_pte_comp_tail != NULL))
	{
		pte_comp_a = _pte_data->_pte_comp_head->_next;
		while (pte_comp_a != _pte_data->_pte_comp_tail)
		{
			pte_comp_b = pte_comp_a->_next;
			while (pte_comp_b != _pte_data->_pte_comp_tail)
			{
				if ((pte_comp_a->_szresult != NULL) && (pte_comp_b->_szresult != NULL))
				{
					if (strcmp(pte_comp_a->_szresult, pte_comp_b->_szresult) == 0)
					{
						if ((pte_comp_a->_szfeature != NULL) && ((pte_comp_b->_szfeature != NULL)))
						{
							if (pte_comp_a->_pumsa == pte_comp_b->_pumsa)
								//if(strcmp(pte_comp_a->_szfeature,pte_comp_b->_szfeature)==0)
							{
								if (pte_comp_a->_same_count == 0)
								{
									pte_comp_a->_same_deputy = TRUE;
								}
								pte_comp_a->_same_count++;
								if (pte_comp_a->_same_count >= pte_comp_b->_same_count)
								{
									pte_comp_b->_same_count = pte_comp_a->_same_count;
								}
								else
								{
									pte_comp_a->_same_count = pte_comp_b->_same_count;
								}
							}
						}
					}
				}
				pte_comp_b = pte_comp_b->_next;
			}
			pte_comp_a = pte_comp_a->_next;
		}
	}
}

bool TE_LIST::f_save(char *path)
{
	if (_pte_data == NULL)
		return false;

	FILE *fp = NULL;

	fp = fopen(path, "wb");
	if (fp == NULL)
		return false;

	fwrite(_pte_data, TE_DATA_HEAD_SIZE, 1, fp);

	char save_load_separate = SAVE_LOAD_SEPARATE;

	TE_COMP_TOKEN *pte_comp = NULL;
	TE_TOKEN *pte_token = NULL;
	TE_SENTENCE *pte_sentence = NULL;

	if ((_pte_data->_pte_sentence_head != NULL) && (_pte_data->_pte_sentence_tail != NULL))
	{
		pte_sentence = _pte_data->_pte_sentence_head->_next;
		while (pte_sentence != _pte_data->_pte_sentence_tail)
		{
			fwrite(pte_sentence, TE_SENTENCE_HEAD_SIZE, 1, fp);
			pte_sentence = pte_sentence->_next;
		}
	}
	if ((_pte_data->_pte_token_head != NULL) && (_pte_data->_pte_token_tail != NULL))
	{
		//토큰저장
		pte_token = _pte_data->_pte_token_head->_next;
		while (pte_token != _pte_data->_pte_token_tail)
		{
			//일단헤더저장
			fwrite(pte_token, TE_TOKEN_HEAD_SIZE, 1, fp);
			//토큰(szresult,szambend,szcomp_token)저장=>순서대로 0x0a값0x0a값0x0a값0x0a
			fwrite(&save_load_separate, 1, 1, fp);//1
			if (pte_token->_sztoken != NULL)
			{
				fwrite(pte_token->_sztoken, strlen(pte_token->_sztoken), 1, fp);
			}
			fwrite(&save_load_separate, 1, 1, fp);//2
			if (pte_token->_szambend != NULL)
			{
				fwrite(pte_token->_szambend, strlen(pte_token->_szambend), 1, fp);
			}
			fwrite(&save_load_separate, 1, 1, fp);//3
			if (pte_token->_szcomp_token != NULL)
			{
				fwrite(pte_token->_szcomp_token, strlen(pte_token->_szcomp_token), 1, fp);
			}
			fwrite(&save_load_separate, 1, 1, fp);//4

			pte_token = pte_token->_next;
		}

		//토큰에 속한 스템저장
		pte_token = _pte_data->_pte_token_head->_next;
		while (pte_token != _pte_data->_pte_token_tail)
		{
			if (pte_token->_pstem != NULL)
			{
				//일단스템헤더저장
				fwrite(pte_token->_pstem, TE_STEM_HEAD_SIZE, 1, fp);
				//스템(szresult)저장=> 0x0a값0x0a
				fwrite(&save_load_separate, 1, 1, fp);//1
				if (pte_token->_pstem->_szresult != NULL)
				{
					fwrite(pte_token->_pstem->_szresult, strlen(pte_token->_pstem->_szresult), 1, fp);
				}
				fwrite(&save_load_separate, 1, 1, fp);//2
			}
			pte_token = pte_token->_next;
		}

		//토큰에 속한 엔딩저장
		pte_token = _pte_data->_pte_token_head->_next;
		while (pte_token != _pte_data->_pte_token_tail)
		{
			if (pte_token->_pend != NULL)
			{
				//일단엔딩헤더저장
				fwrite(pte_token->_pend, TE_END_HEAD_SIZE, 1, fp);
				//엔딩(szresult)저장=> 0x0a값0x0a
				fwrite(&save_load_separate, 1, 1, fp);//1
				if (pte_token->_pend->_szresult != NULL)
				{
					fwrite(pte_token->_pend->_szresult, strlen(pte_token->_pend->_szresult), 1, fp);
				}
				fwrite(&save_load_separate, 1, 1, fp);//2
			}
			pte_token = pte_token->_next;
		}
	}

	if ((_pte_data->_pte_comp_head != NULL) && (_pte_data->_pte_comp_tail != NULL))
	{
		//복합어에 속한 스템저장
		pte_comp = _pte_data->_pte_comp_head->_next;
		while (pte_comp != _pte_data->_pte_comp_tail)
		{
			//일단스템헤더저장
			fwrite(pte_comp, TE_COMP_HEAD_SIZE, 1, fp);
			//엔딩(szresult)저장=> 0x0a값0x0a
			fwrite(&save_load_separate, 1, 1, fp);//1
			if (pte_comp->_szresult != NULL)
			{
				fwrite(pte_comp->_szresult, strlen(pte_comp->_szresult), 1, fp);
			}
			fwrite(&save_load_separate, 1, 1, fp);//2

			pte_comp = pte_comp->_next;
		}
	}

	fclose(fp);
	return true;
}
bool TE_LIST::f_load(char *path)
{
	if (_pte_data != NULL)
	{
		f_del(true);
	}
	f_init();

	if ((_pte_data == NULL) ||
		(_pte_data->_pte_sentence_head == NULL) || (_pte_data->_pte_sentence_tail == NULL) ||
		(_pte_data->_pte_token_head == NULL) || (_pte_data->_pte_token_tail == NULL) ||
		(_pte_data->_pte_comp_head == NULL) || (_pte_data->_pte_comp_tail == NULL))
	{
		return false;
	}

	int i;
	int cnt_separate, cnt_sz;
	cnt_separate = cnt_sz = 0;
	int file_size = 0;
	char ch = 0x00;
	char *psz;
	char sz[1024];
	BOOL bret = FALSE;
	FILE *fp = NULL;

	fp = fopen(path, "rb");
	if (fp == NULL)
		return false;

	file_size = filelength(fileno(fp));

	if (file_size < TE_DATA_HEAD_SIZE)//최소한 헤더사이즈보다는 커야한다.
		false;

	TE_STEM_TOKEN *pte_stem = NULL;
	TE_END_TOKEN *pte_end = NULL;
	TE_COMP_TOKEN *pte_comp = NULL;
	TE_TOKEN *pte_token = NULL;
	TE_SENTENCE *pte_sentence = NULL;

	fread(_pte_data, TE_DATA_HEAD_SIZE, 1, fp);

	//문장정보를 읽어온다.
	if (_pte_data->_cnt_sentence > 0)
	{
		for (i = 0; i<_pte_data->_cnt_sentence; i++)
		{
			pte_sentence = (TE_SENTENCE*)malloc(sizeof(TE_SENTENCE));
			if (pte_sentence != NULL)
			{
				memset(pte_sentence, 0x00, sizeof(TE_SENTENCE));
				fread(pte_sentence, TE_SENTENCE_HEAD_SIZE, 1, fp);
				f_add(pte_sentence, TYPE_TE_SENTENCE);
				pte_sentence = NULL;
			}
		}
	}
	//토큰정보를 읽어온다.
	if (_pte_data->_cnt_token > 0)
	{
		for (i = 0; i<_pte_data->_cnt_token; i++)
		{
			memset(sz, 0x00, sizeof(sz));
			pte_token = (TE_TOKEN*)malloc(sizeof(TE_TOKEN));
			if (pte_token != NULL)
			{
				memset(pte_token, 0x00, sizeof(TE_TOKEN));
				fread(pte_token, TE_TOKEN_HEAD_SIZE, 1, fp);//토큰기본정보읽기
				cnt_separate = cnt_sz = 0;
				while (1)//토큰에포함된 스트링읽기
				{
					ch = 0x00;
					fread(&ch, 1, 1, fp);
					if (ch == SAVE_LOAD_SEPARATE)
					{
						cnt_separate++;

						if (cnt_separate == 2)//첫번째스트링(sztoken)
						{
							if (strlen(sz) > 0)
							{
								psz = (char*)malloc(strlen(sz) + 1);
								if (psz != NULL)
								{
									memset(psz, 0x00, strlen(sz) + 1);
									strcpy(psz, sz);
									pte_token->_sztoken = psz;
									memset(sz, 0x00, sizeof(sz));
									psz = NULL;
								}
							}
							cnt_sz = 0;
						}
						else if (cnt_separate == 3)//두번째스트링(szambend)
						{
							if (strlen(sz) > 0)
							{
								psz = (char*)malloc(strlen(sz) + 1);
								if (psz != NULL)
								{
									memset(psz, 0x00, strlen(sz) + 1);
									strcpy(psz, sz);
									pte_token->_szambend = psz;
									memset(sz, 0x00, sizeof(sz));
									psz = NULL;
								}
							}
							cnt_sz = 0;
						}
						else if (cnt_separate == 4)//세번째스트링(szcomp_token)
						{
							if (strlen(sz) > 0)
							{
								psz = (char*)malloc(strlen(sz) + 1);
								if (psz != NULL)
								{
									memset(psz, 0x00, strlen(sz) + 1);
									strcpy(psz, sz);
									pte_token->_szcomp_token = psz;
									memset(sz, 0x00, sizeof(sz));
									psz = NULL;
								}
							}
							cnt_sz = 0;
							break;
						}
						else if (cnt_separate > 4)//구분자가 5개이상일경우
						{
							if (psz != NULL)
							{
								free(psz);
								psz = NULL;
							}
							break;
						}
					}
					else//구분자가 아닐경우
					{

						sz[cnt_sz++] = ch;
					}
				}
				f_add(pte_token, TYPE_TE_TOKEN);
			}
		}
	}
	//토큰스템 읽기
	if ((_pte_data->_cnt_stem - _pte_data->_cnt_comp_stem) > 0)
	{
		for (i = 0; i<(_pte_data->_cnt_stem - _pte_data->_cnt_comp_stem); i++)
		{
			memset(sz, 0x00, sizeof(sz));
			psz = NULL;
			pte_stem = NULL;
			pte_stem = (TE_STEM_TOKEN*)malloc(sizeof(TE_STEM_TOKEN));
			if (pte_stem != NULL)
			{
				memset(pte_stem, 0x00, sizeof(TE_STEM_TOKEN));
				fread(pte_stem, TE_STEM_HEAD_SIZE, 1, fp);//스템기본정보읽기
				cnt_separate = cnt_sz = 0;
				while (1)//스템에포함된 스트링읽기
				{
					ch = 0x00;
					fread(&ch, 1, 1, fp);
					if (ch == SAVE_LOAD_SEPARATE)
					{
						cnt_separate++;
						if (cnt_separate == 2)//첫번째스트링(sztoken)
						{
							if (strlen(sz) > 0)
							{
								psz = (char*)malloc(strlen(sz) + 1);
								if (psz != NULL)
								{
									memset(psz, 0x00, strlen(sz) + 1);
									strcpy(psz, sz);
									pte_stem->_szresult = psz;
									memset(sz, 0x00, sizeof(sz));
									psz = NULL;
								}
							}
							cnt_sz = 0;
							break;
						}
						else if (cnt_separate > 2)//구분자가 3개이상일경우
						{
							if (psz != NULL)
							{
								free(psz);
								psz = NULL;
							}
							break;
						}
					}
					else//구분자가 아닐경우
					{
						sz[cnt_sz++] = ch;
					}
				}//end while
				bret = FALSE;
				pte_token = _pte_data->_pte_token_head->_next;
				while (pte_token != _pte_data->_pte_token_tail)
				{
					if ((pte_token->_token_offset == pte_stem->_token_offset) &&
						(pte_token->_sentence_offset == pte_stem->_sentence_offset))
					{
						pte_token->_pstem = pte_stem;
						bret = TRUE;
						break;
					}
					pte_token = pte_token->_next;
				}
				if (!bret)
				{
					if (pte_stem != NULL)
					{
						free(pte_stem);
						pte_stem = NULL;
					}
				}
			}
		}
	}
	//토큰엔딩 읽기
	if (_pte_data->_cnt_end > 0)
	{
		for (i = 0; i<_pte_data->_cnt_end; i++)
		{
			memset(sz, 0x00, sizeof(sz));
			pte_end = NULL;
			pte_end = (TE_END_TOKEN*)malloc(sizeof(TE_END_TOKEN));
			if (pte_end != NULL)
			{
				memset(pte_end, 0x00, sizeof(TE_END_TOKEN));
				fread(pte_end, TE_END_HEAD_SIZE, 1, fp);//스템기본정보읽기
				cnt_separate = cnt_sz = 0;
				while (1)//스템에포함된 스트링읽기
				{
					ch = 0x00;
					fread(&ch, 1, 1, fp);
					if (ch == SAVE_LOAD_SEPARATE)
					{
						cnt_separate++;
						if (cnt_separate == 2)//첫번째스트링(sztoken)
						{
							if (strlen(sz) > 0)
							{
								psz = (char*)malloc(strlen(sz) + 1);
								if (psz != NULL)
								{
									memset(psz, 0x00, strlen(sz) + 1);
									strcpy(psz, sz);
									pte_end->_szresult = psz;
									memset(sz, 0x00, sizeof(sz));
									psz = NULL;
								}
							}
							cnt_sz = 0;
							break;
						}
						else if (cnt_separate > 2)//구분자가 3개이상일경우
						{
							if (psz != NULL)
							{
								free(psz);
								psz = NULL;
							}
							break;
						}
					}
					else//구분자가 아닐경우
					{
						sz[cnt_sz++] = ch;
					}
				}//end while

				bret = FALSE;
				pte_token = _pte_data->_pte_token_head->_next;
				while (pte_token != _pte_data->_pte_token_tail)
				{
					if ((pte_token->_token_offset == pte_end->_token_offset) &&
						(pte_token->_sentence_offset == pte_end->_sentence_offset))
					{
						pte_token->_pend = pte_end;
						bret = TRUE;
						break;
					}
					pte_token = pte_token->_next;
				}
				if (!bret)
				{
					if (pte_end != NULL)
					{
						free(pte_end);
						pte_end = NULL;
					}
				}
			}
		}//for
	}
	//복합어스템 읽기
	if (_pte_data->_cnt_comp_stem > 0)
	{
		for (i = 0; i<_pte_data->_cnt_comp_stem; i++)
		{
			memset(sz, 0x00, sizeof(sz));
			pte_comp = (TE_COMP_TOKEN*)malloc(sizeof(TE_COMP_TOKEN));
			if (pte_comp != NULL)
			{
				memset(pte_comp, 0x00, sizeof(TE_COMP_TOKEN));
				fread(pte_comp, TE_COMP_HEAD_SIZE, 1, fp);//토큰기본정보읽기
				cnt_separate = cnt_sz = 0;
				while (1)//토큰에포함된 스트링읽기
				{
					ch = 0x00;
					fread(&ch, 1, 1, fp);
					if (ch == SAVE_LOAD_SEPARATE)
					{
						cnt_separate++;
						if (cnt_separate == 2)//첫번째스트링(sztoken)
						{
							if (strlen(sz) > 0)
							{
								psz = (char*)malloc(strlen(sz) + 1);
								if (psz != NULL)
								{
									memset(psz, 0x00, strlen(sz) + 1);
									strcpy(psz, sz);
									pte_comp->_szresult = psz;
									memset(sz, 0x00, sizeof(sz));
									psz = NULL;
								}
							}
							cnt_sz = 0;
							break;
						}
						else if (cnt_separate > 2)//구분자가 3개이상일경우
						{
							if (psz != NULL)
							{
								free(psz);
								psz = NULL;
							}
							break;
						}
					}
					else//구분자가 아닐경우
					{
						sz[cnt_sz++] = ch;
					}
				}
				f_add(pte_comp, TYPE_TE_COMP);
			}
		}
	}

	fclose(fp);
	return true;
}