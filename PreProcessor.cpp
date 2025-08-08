#include "PreProcessor.h"



CPreProcessor::CPreProcessor()
{
	m_nAnalysisResultTime = 0;
}

CPreProcessor::CPreProcessor(int readType, char* strExePath)
{
	m_pStemEndDic = new CStemEndDic(readType, strExePath);
	m_pDocVector = new CNADocVector;
	/*
	m_wndNAHangulAnaly = new CNAHangulAnaly;
	m_wndNAHangulAnaly->m_pStemEndDic = m_pStemEndDic;
	m_wndNAHangulAnaly->m_LinkedList = m_pDocVector;
	*/
	m_wndMorphoLogicalAnalysis = new CMorphoLogicalAnalysis;
	m_wndMorphoLogicalAnalysis->m_pStemEndDic = m_pStemEndDic;
	m_wndMorphoLogicalAnalysis->m_LinkedList = m_pDocVector;

	m_nAnalysisResultTime = 0;
}


CPreProcessor::~CPreProcessor()
{
	/*
	if (m_wndNAHangulAnaly != NULL)
	{
		delete m_wndNAHangulAnaly;
		m_wndNAHangulAnaly = NULL;
	}
	*/
	if (m_wndMorphoLogicalAnalysis != NULL)
	{
		delete m_wndMorphoLogicalAnalysis;
		m_wndMorphoLogicalAnalysis = NULL;
	}
	if (m_pStemEndDic != NULL)
	{
		delete m_pStemEndDic;
		m_pStemEndDic = NULL;
	}
	if (m_pDocVector != NULL)
	{
		delete m_pDocVector;
		m_pDocVector = NULL;
	}
}


void CPreProcessor::RemoveDocVectorData()
{
	m_pDocVector->RemoveAllSentence();
}



void CPreProcessor::InsertLinkedList(char *pstrInsert, int nOffset)
{
	if (m_pDocVector != NULL)
		m_pDocVector->AddTail(pstrInsert, nOffset);
}

void CPreProcessor::InsertLinkedList(H_NODE* pLink)
{
	if (m_pDocVector != NULL)
		m_pDocVector->AddLinkTail(pLink);
}

bool CPreProcessor::InitHangulAnalyClass()
{
	bool bCreate = false;
	if (m_wndMorphoLogicalAnalysis == NULL)
	{
		m_wndMorphoLogicalAnalysis = new CMorphoLogicalAnalysis;
		bCreate = true;
	}
	else
	{
		bCreate = true;
	}
	/*
	if (m_wndNAHangulAnaly == NULL)
	{
		m_wndNAHangulAnaly = new CNAHangulAnaly;
		bCreate = true;
	}
	else
	{
		bCreate = true;
	}
	*/
	
	return bCreate;
}

/*******************************************************************************************
pAllString 문장이 들어오면 Space 단위로 PreToken
Space가 나오면 그 다음 Char가 나올때까지 위치이동
'\r', '\n'이 나오면 nCase = 1로 해서 PreToken에 저장
위치 이동전까지 문자열을 PreToken에 저장
*******************************************************************************************/
void CPreProcessor::DivideDocSpace(char *pAllString)
{
	unsigned int nTotalCount = 0;
	unsigned int nCheckEnd = 0;
	int nFirstParent = 0, nLastParent = 0;
	int nCharCount = 0;
	int nSpaceOffset = 0;
	char pSaveText[1024];
	int nSaveText = 0;
	bool bAdd = false;
	int nCase = 0;
	char pLongChar[3];
	int nLong1;
	int nLong2;
	char pLongTmp[1024];
	char pOneChar[3];
	char ptmpOneChar[3];

	int nTT = 0;	/*따옴표*/
	int nSmalTT = -1;
	
	CNAPreLinkedList* pPreLinkedList = NULL;

	m_pAllString = pAllString;

	pPreLinkedList = new CNAPreLinkedList;
	pPreLinkedList->InitHeadTailNode();

	//Documnet Vector Init
	RemoveDocVectorData();
	

	nTotalCount = strlen(pAllString);
	memset(pSaveText, 0x00, 1024);

	//분석 시작시간 설정
	clock_t clockStartTimer, clockEndTimer;
	clockStartTimer = clock();

	while (nTotalCount > (unsigned)nCharCount)
	{
		if (pAllString[nCharCount] == ' ' || pAllString[nCharCount] == '\t')
		{
			if (NextCharPos(pAllString, nCharCount))
				nCase = PRENODE_CHAR_2;//'\r','\n'
			else
				nCase = PRENODE_CHAR_1;//' '

			if (strlen(pSaveText) > 0)
				bAdd = true;
		}
		else if (pAllString[nCharCount] == '\r' || pAllString[nCharCount] == '\n')
		{
			if (pAllString[nCharCount] == '\r' && pAllString[nCharCount + 1] == '\n') 
			{
				nCharCount++;
			}
			nCharCount++;
			if (NextCharPos(pAllString, nCharCount) == true)
				nCase = PRENODE_CHAR_6;//('\r', '\n' )*2
			else
				nCase = PRENODE_CHAR_2;//'\r','\n'
			if (strlen(pSaveText) > 0)
				bAdd = true;
		}
		else if ((pAllString[nCharCount] == '!' || pAllString[nCharCount] == '?' || pAllString[nCharCount] == '.') 
					&& (pAllString[nCharCount + 1] == 0x00 || pAllString[nCharCount + 1] == '\r' || pAllString[nCharCount + 1] == '\n'))
		{
			pSaveText[nSaveText++] = pAllString[nCharCount++];
			NextCharPos(pAllString, nCharCount);
			nCase = PRENODE_CHAR_5;//('?', '!', '.') + ('\r', '\n')
			if (strlen(pSaveText) > 0)
				bAdd = true;
		}
		else if ((pAllString[nCharCount] == '!' || pAllString[nCharCount] == '?' || pAllString[nCharCount] == '.') 
				&& pAllString[nCharCount + 1] == ' ')
		{
			if (pAllString[nCharCount] == '.')
				pSaveText[nSaveText++] = pAllString[nCharCount++];
			else
				nCharCount++;
			if (NextCharPos(pAllString, nCharCount) == true)
				nCase = PRENODE_CHAR_5;//('?', '!', '.') + ('\r', '\n')
			else
				nCase = PRENODE_CHAR_3;//'?', '!', '.'
			if (strlen(pSaveText) > 0)
				bAdd = true;
		}
		else if (pAllString[nCharCount] == '.')
		{
			memset(pOneChar, 0x00, 3);
			if (strlen(pSaveText) >= 2)
			{
				pOneChar[0] = pAllString[nCharCount - 2];
				if (pOneChar[0] & 0x80) // 2 Byte Char?
				{
					pOneChar[1] = pAllString[nCharCount - 1];
				}
			}

			// 한글 + '.' + 한글 인지 판단
			if (
				strlen(pSaveText) == 2 
				&& strcmp(pSaveText, "것") == 0 
				&& IsDot(pAllString, nCharCount, 3) == true
			)
			{
				nCharCount++;
				nCase = PRENODE_CHAR_3;//'?', '!', '.'
				if (strlen(pSaveText) > 0)
					bAdd = true;
			}
			else if (strlen(pOneChar) == 2 
					&& IsDot(pAllString, nCharCount, 3) == true 
					&&
					(
						strstr("감,갬,겜,굄,굼,귐,깜,깸,꼼,꾐,꿈,뀀,뀜,끔,낌", pOneChar) != 0 ||
						strstr("남,냄,넘,넴,뇜,눔,뉨,늼,님,다,댐,뎀,됨,둠,뒴", pOneChar) != 0 ||
						strstr("딤,땀,땜,떰,뗌,뙴,뜀,뜸,띔,띰,램,럼,렘,룀,룸", pOneChar) != 0 ||
						strstr("름,림,맴,멈,멤,몸,밈,뱀,벰,봄,뵘,빔,뺌,쁨,삠", pOneChar) != 0 ||
						strstr("샘,섬,셈,쇰,숨,쉼,심,쌈,쌤,썸,쏨,쐼,쑴,씀,씜", pOneChar) != 0 ||
						strstr("앰,얌,염,옴,욈,움,윔,음,읨,임,잠,죔,줌,쥠,짐", pOneChar) != 0 ||
						strstr("짬,쨈,쫌,쬠,쭘,찜,참,챔,춤,췸,침,켬,쿰,큄,큼", pOneChar) != 0 ||
						strstr("킴,탐,탬,툼,튐,틈,틤,팀,팜,팸,폄,폠,품,픔,핌", pOneChar) != 0 ||
						strstr("함,헴,휨,흼,힘,다", pOneChar) != 0
					))
			{
				nCharCount++;
				nCase = PRENODE_CHAR_3;//'?', '!', '.'
				if (strlen(pSaveText) > 0)
					bAdd = true;
			}
			else if ((int)strlen(pSaveText) > 2 
					&& strcmp("다", pOneChar) == 0 
					&&(pAllString[nCharCount + 1] >= 'A' && pAllString[nCharCount + 1] <= 'Z' || pAllString[nCharCount + 1] >= 'a' && pAllString[nCharCount + 1] <= 'z'))
			{
				nCharCount++;
				nCase = PRENODE_CHAR_3;//'?', '!', '.'
				if (strlen(pSaveText) > 0)
					bAdd = true;
			}
			else
			{
				pSaveText[nSaveText++] = pAllString[nCharCount++];
			}
		}
		else if (pAllString[nCharCount] == ',')
		{
			if (IsCurrency(pAllString, nCharCount, FALSE_ASCII))
			{
				nCharCount++;
			}
			else
			{
				// ','전에 문자가 있으면 먼저 추가
				if (strlen(pSaveText) > 0)
				{
					pPreLinkedList->AddPreTail(pSaveText, nSpaceOffset, PRENODE_CHAR_1, nFirstParent, nLastParent);//' '
					//Comma 자질 셋팅
					pPreLinkedList->m_pTailNode->pPrev->pPreToken->pFeature[SYMBOL_COMMA] = '1';//215

					memset(pSaveText, 0x00, 1024);
					nCase = 0;
					nSaveText = 0;
					nFirstParent = 0;
					nLastParent = 0;
				}
				else
				{
					nCharCount++;
				}
			}
		}
		else if (pAllString[nCharCount] == '-')//산술 연산자
		{
			if (!IsOperation(pAllString, nCharCount, TRUE_ASCII))
			{
				nCase = PRENODE_CHAR_1;//' '
				if (strlen(pSaveText) > 0)
					bAdd = true;
				nCharCount++;
			}
			else
				pSaveText[nSaveText++] = pAllString[nCharCount++];
		}
		else
		{
			// First Char Position
			if (nSaveText == 0)
				nSpaceOffset = nCharCount;

			memset(pOneChar, 0x00, 3);
			pOneChar[0] = pAllString[nCharCount];
			if (pOneChar[0] & 0x80) // 2 Byte Char?
			{
				pOneChar[1] = pAllString[nCharCount + 1];
				if (strcmp(pOneChar, "“") == 0 || strcmp(pOneChar, "”") == 0)
				{
					pSaveText[nSaveText++] = '"';
					nTT = 1;
				}
				else if (strcmp(pOneChar, "‘") == 0 || strcmp(pOneChar, "’") == 0)
				{
				}
				else if (strcmp(pOneChar, "∼") == 0)
					pSaveText[nSaveText++] = '~';
				else if (strcmp(pOneChar, "＄") == 0)
					pSaveText[nSaveText++] = '$';
				else if (strcmp(pOneChar, "￦") == 0)
					pSaveText[nSaveText++] = '\\';
				else if (
							strcmp(pOneChar, "·") == 0 
							&& IsSlachDot(pAllString, nCharCount, FALSE_ASCII) != 1
				)
				{
					nCase = PRENODE_CHAR_1;//' '
					if (strlen(pSaveText) > 0)
						bAdd = true;
				}
				else
				{
					pSaveText[nSaveText++] = pOneChar[0];
					pSaveText[nSaveText++] = pOneChar[1];
				}
				nCharCount += 2;
			}
			else
			{
				if (pAllString[nCharCount] == '"')
					nTT = 1;
				if (pAllString[nCharCount] == '`')
					pAllString[nCharCount] = '\'';

				if (nFirstParent == 0)	//여는 괄호
					nFirstParent = GetParentSymbol(pAllString[nCharCount]);
				if (nLastParent == 0)  //닫는 괄호
					nLastParent = GetParentSymbol(pAllString[nCharCount], 1);

				if (nFirstParent == SYMBOL_CHAR_2 && nTT == 1)//'('
				{
					nFirstParent = 0;
					nCase = PRENODE_CHAR_1;//' '
					if (strlen(pSaveText) > 0)
						bAdd = true;
				}
				else if (nFirstParent == SYMBOL_CHAR_2 && nLastParent == 0)//'('
				{
					pAllString[nCharCount] = ' ';
					nCharCount++;
					nCase = PRENODE_CHAR_1;//' '
					if (strlen(pSaveText) > 0)
						bAdd = true;
				}
				else
				{
					if (nLastParent != SYMBOL_CHAR_6)//')'
						pSaveText[nSaveText++] = pAllString[nCharCount];
					nCharCount++;

					//어는,닫는괄호가 동시에 나올경우 한단어로 처리 예) (한국)
					if (nFirstParent == nLastParent - 4)
					{
						ptmpOneChar[0] = pAllString[nCharCount];
						ptmpOneChar[1] = pAllString[nCharCount + 1];
						ptmpOneChar[2] = 0;
						if (pAllString[nCharCount + 2] == 0 
							&&(strcmp(ptmpOneChar, "이") == 0 || strcmp(ptmpOneChar, "가") == 0 
								|| strcmp(ptmpOneChar, "은") == 0 || strcmp(ptmpOneChar, "는") == 0 
								|| strcmp(ptmpOneChar, "을") == 0 || strcmp(ptmpOneChar, "를") == 0))
						{
							nCharCount++;
						}
						else
						{
							nCase = PRENODE_CHAR_1;//' '
							if (strlen(pSaveText) > 0)
								bAdd = true;
						}
					}
				}
			}
		}

		if (nTotalCount < (unsigned)nCharCount + 1 || bAdd == true)
		{
			if (pSaveText && strcmp(pSaveText, ".") != 0 && strcmp(pSaveText, "~") != 0)
			{
				pPreLinkedList->AddPreTail(pSaveText, nSpaceOffset, nCase, nFirstParent, nLastParent);
				// [Comman] 자질 Setting
				if (nCase == PRENODE_CHAR_4)//','
					(pPreLinkedList->GoPreNodeLast())->pPreToken->pFeature[SYMBOL_COMMA] = '1';//215
			}
			nTT = 0;
			nCase = 0;
			nSaveText = 0;
			bAdd = false;
			nFirstParent = 0;
			nLastParent = 0;
			memset(pSaveText, 0x00, 1024);
		}
		// 긴어절 처리
		else if (nSaveText >= LONG_TOKEN - 1)//40
		{
			for (nLong1 = 0; nLong1<nSaveText; nLong1++)
			{
				memset(pLongChar, 0x00, 3);
				pLongChar[0] = pSaveText[nLong1];
				pLongChar[1] = pSaveText[nLong1 + 1];
				if (strstr("가,게,고,과,까,나,냐,네,녕,뇨,는,니,다,대,던,데", pLongChar) != NULL 
					||strstr("도,든,래,려,렬,렴,로,록,를,리,만,며,면,및,서,쇼,슈", pLongChar) != NULL 
					||strstr("슨,아,야,에,오,와,요,운,을,의,이,자,죠,즉,지,차,테", pLongChar) != NULL)
				{
					memset(pLongTmp, 0x00, 1024);
					nFirstParent = 0;
					nLastParent = 0;
					for (nLong2 = 0; nLong2<nLong1 + 2; nLong2++)
					{
						if (nFirstParent == 0)	//여는 괄호
							nFirstParent = GetParentSymbol(pAllString[nCharCount]);
						if (nLastParent == 0)  //닫는 괄호
							nLastParent = GetParentSymbol(pAllString[nCharCount], 1);
						pLongTmp[nLong2] = pSaveText[nLong2];
					}
					pPreLinkedList->AddPreTail(pLongTmp, nSpaceOffset, 1, nFirstParent, nLastParent);
					nSpaceOffset = nSpaceOffset + nLong2;
					nCharCount = nSpaceOffset;

					nTT = 0;
					nCase = 0;
					nSaveText = 0;
					bAdd = false;
					nFirstParent = 0;
					nLastParent = 0;
					memset(pSaveText, 0x00, 1024);
				}
			}
		}
		else
		{
			//2017-08-16 문장분리가 정상적으로 이루어 지지 않을 경우에 값을 증가하게 만들어 무한루프방지하게 추가함.
			//문장예 : if (size != -1) printf ("Size = %ld\n", size); else printf("File Not Found\n");
			//nCharCount++;
		}
	}

	// Space 단위로 어절을 만든 다음 문장 및 H_NODE 구성
	PreLinkedNode(pPreLinkedList);
	


	// Pre Node 삭제
	if (pPreLinkedList != NULL)
	{
		pPreLinkedList->RemovePreAll();
		delete pPreLinkedList;
		pPreLinkedList = NULL;
	}

	
	//분석종료시간설정
	clockEndTimer = clock();
	m_nAnalysisResultTime = (double)(clockEndTimer - clockStartTimer) / CLOCKS_PER_SEC;
}



//-------------------------------------------------------------------------------
// m_pNode : 현재 PreNode 위치
// pSetenceHeadNode : 문장의 첫 위치
// pSetenceTailNode : 문장의 마지막 위치
// m_pNode가 지나가면서 pSetenceHeadNode를 설정하고 pSetenceTailNode에 설정되면
// pSetenceHeadNode에서 pSetenceTailNode까지 '문장'을 추가하고 H_NODE를 추가
//-------------------------------------------------------------------------------
void CPreProcessor::PreLinkedNode(CNAPreLinkedList* pPreLinkedList)
{
	char* pstrText = NULL;
	char* pUnitStr = NULL;
	int nLen = 0;
	int nOffset = 0;
	int nParent = 0;
	int nPos = -1; // 특수부호 위치
	bool bHanja = true;

	int nLeftSymbol = -1;
	int nRightSymbol = -1;
	char pOneChar[3];
	int nOneLen = 0;

	PRE_NODE* m_pNode = NULL;
	PRE_NODE* pTmpNode = NULL;
	PRE_NODE* pCommaTmpNode = NULL;
	PRE_NODE* pParentNode = NULL;		//괄호 첫번째 위치 '('
	PRE_NODE* pSentenceFirst = NULL;
	PRE_NODE* pSentenceLast = NULL;

	m_pNode = pPreLinkedList->m_pHeadNode->pNext;
	pSentenceFirst = m_pNode;

	while (m_pNode != pPreLinkedList->m_pTailNode)
	{
		// '\n'이 연속되어 나오는 경우 문장 분할
		if (m_pNode->pPreToken->nEndCheck == PRENODE_CHAR_6)//('\r', '\n' )*2
		{
			pSentenceLast = m_pNode;
			SentenceAddNode(pSentenceFirst, pSentenceLast);	//문장분할
			pSentenceFirst = m_pNode->pNext;//문장 첫번째 Link
			pSentenceLast = NULL;
		}
		// '.' 처리
		else if (m_pNode->pPreToken->nEndCheck == PRENODE_CHAR_3)//'?', '!', '.'
		{
			memset(pOneChar, 0x00, 3);
			pOneChar[0] = m_pNode->pPreToken->strOriginalSource[0];
			pOneChar[1] = m_pNode->pPreToken->strOriginalSource[1];

			// 마침표 : 토큰이 두문자로 구성되어 있고, 문자가 점(가. 나. a.) 아닌경우)
			if (!(m_pNode->pPreToken->strOriginalSource[2] == '.' &&
				strstr("ㄱ,ㄴ,ㄷ,ㄹ,ㅁ,ㅂ,ㅅ,ㅇ,ㅈ,ㅋ,ㅌ,ㅍ,ㅎ,가,나,다,라,마,바,사,아,자,카,타,파,하", pOneChar) != 0) &&
				!(m_pNode->pPreToken->strOriginalSource[1] == '.' &&
				((m_pNode->pPreToken->strOriginalSource[0] >= 'a' && m_pNode->pPreToken->strOriginalSource[0] <= 'z') ||
					(m_pNode->pPreToken->strOriginalSource[0] >= 'A' && m_pNode->pPreToken->strOriginalSource[0] <= 'Z') ||
					(m_pNode->pPreToken->strOriginalSource[0] >= '0' && m_pNode->pPreToken->strOriginalSource[0] <= '9'))))
			{
				pSentenceLast = m_pNode;
				NodeOneCharRemove(m_pNode, ".");
				SentenceAddNode(pSentenceFirst, pSentenceLast);	//문장분할
				pSentenceFirst = m_pNode->pNext;//문장 첫번째 Link
				pSentenceLast = NULL;
			}
		}
		// '.', '?', '!'
		else if (m_pNode->pPreToken->nEndCheck == PRENODE_CHAR_5)//('?', '!', '.') + ('\r', '\n')
		{
			memset(pOneChar, 0x00, 3);
			pOneChar[0] = m_pNode->pPreToken->strOriginalSource[0];
			pOneChar[1] = m_pNode->pPreToken->strOriginalSource[1];

			// 마침표 : 토큰이 두문자로 구성되어 있고, 문자가 점(가. 나. a.) 아닌경우)
			if (!(m_pNode->pPreToken->strOriginalSource[2] == '.' &&
				strstr("ㄱ,ㄴ,ㄷ,ㄹ,ㅁ,ㅂ,ㅅ,ㅇ,ㅈ,ㅋ,ㅌ,ㅍ,ㅎ,가,나,다,라,마,바,사,아,자,카,타,파,하", pOneChar) != 0) &&
				!(m_pNode->pPreToken->strOriginalSource[1] == '.' &&
				((m_pNode->pPreToken->strOriginalSource[0] >= 'a' && m_pNode->pPreToken->strOriginalSource[0] <= 'z') ||
					(m_pNode->pPreToken->strOriginalSource[0] >= 'A' && m_pNode->pPreToken->strOriginalSource[0] <= 'Z') ||
					(m_pNode->pPreToken->strOriginalSource[0] >= '0' && m_pNode->pPreToken->strOriginalSource[0] <= '9'))))
			{
				pSentenceLast = m_pNode;
				NodeOneCharRemove(m_pNode, ".");
				NodeOneCharRemove(m_pNode, "!");
				NodeOneCharRemove(m_pNode, "?");
				NodeOneCharRemove(m_pNode, "\"");
				SentenceAddNode(pSentenceFirst, pSentenceLast);	//문장분할
				pSentenceFirst = m_pNode->pNext;//문장 첫번째 Link
				pSentenceLast = NULL;
			}
		}
		else if (m_pNode->pPreToken->nEndCheck == PRENODE_CHAR_2 && m_pNode->pNext != pPreLinkedList->m_pTailNode)//'\r','\n'
		{
			pSentenceLast = m_pNode;
			SentenceAddNode(pSentenceFirst, pSentenceLast);	//문장분할
			pSentenceFirst = m_pNode->pNext;//문장 첫번째 Link
			pSentenceLast = NULL;
		}
		// 4. 따옴표와 괄호 처리
		else
		{
			if ((m_pNode->pPreToken->nFirstParent == SYMBOL_CHAR_1 && m_pNode->pPreToken->nLastParent == SYMBOL_CHAR_5) //'<' , //'>'
				||(m_pNode->pPreToken->nFirstParent == SYMBOL_CHAR_2 && m_pNode->pPreToken->nLastParent == SYMBOL_CHAR_6) //'(' ,//')'
				||(m_pNode->pPreToken->nFirstParent == SYMBOL_CHAR_3 && m_pNode->pPreToken->nLastParent == SYMBOL_CHAR_7) //'[' ,//']'
				||(m_pNode->pPreToken->nFirstParent == SYMBOL_CHAR_4 &&m_pNode->pPreToken->nLastParent == SYMBOL_CHAR_8))//'{',//'}'
			{
				bHanja = false;
			}
			else if (bHanja == true)
			{
				//괄호에 있는 한자는 미처리
				Hanja2Hangul(m_pNode);
			}
			else
				bHanja = true;

			ParentNodeTotal(pPreLinkedList, m_pNode);
		}


		switch (CharFindSymbol(m_pNode, &nPos))
		{
		case 1:		// '.'
			if (IsDot(m_pNode->pPreToken->strOriginalSource, nPos, TRUE_ASCII) == false)
			{
				NodeOneCharRemove(m_pNode, ".");
				CharSymbolDelete(m_pNode);
				CharSymbolDelete(m_pNode);
			}
			break;
		case 2:   // ?, !
			NodeOneCharRemove(m_pNode, "?");
			NodeOneCharRemove(m_pNode, "!");
			CharSymbolDelete(m_pNode);
			break;
		case 3:   // / 슬래쉬
				  // 한글일때
			if (IsSlachDot(m_pNode->pPreToken->strOriginalSource, nPos, TRUE_ASCII) != 1)
			{
				pPreLinkedList->PreNodeDivide(m_pNode, nPos, 1);

				// [ns], [ne] 자잘 Setting
				if (m_pNode->pPreToken->pFeature[SYMBOL_NE] == '1')//214
					m_pNode->pPreToken->pFeature[SYMBOL_NE] = '0';
				else
					m_pNode->pPreToken->pFeature[SYMBOL_NS] = '1';
				if (m_pNode->pNext != NULL && m_pNode->pNext->pPreToken != NULL)
					m_pNode->pNext->pPreToken->pFeature[SYMBOL_NE] = '1';
			}
			break;
		case 4:   // ·(2) 가운데점
				  // 한글일때
			if (IsSlachDot(m_pNode->pPreToken->strOriginalSource, nPos, FALSE_ASCII) != 1)
			{
				pPreLinkedList->PreNodeDivide(m_pNode, nPos, 2);
				// [ns], [ne] 자질 Setting
				if (m_pNode->pPreToken->pFeature[SYMBOL_NE] == '1')
					m_pNode->pPreToken->pFeature[SYMBOL_NE] = '0';
				else
					m_pNode->pPreToken->pFeature[SYMBOL_NS] = '1';
				if (m_pNode->pNext != NULL && m_pNode->pNext->pPreToken != NULL)
					m_pNode->pNext->pPreToken->pFeature[SYMBOL_NE] = '1';
			}
			break;
		case 5:   // +, -, *, =, :
			if (IsOperation(m_pNode->pPreToken->strOriginalSource, nPos, TRUE_ASCII) == false)
			{
				NodeOneCharRemove(m_pNode, "+");
				NodeOneCharRemove(m_pNode, "-");
				NodeOneCharRemove(m_pNode, "=");
				NodeOneCharRemove(m_pNode, "*");
				NodeOneCharRemove(m_pNode, ":");
				NodeOneCharRemove(m_pNode, "~");
				CharSymbolDelete(m_pNode);
			}
			break;
		case 6:   // ℃(2) 수식부호
			if (IsOperation(m_pNode->pPreToken->strOriginalSource, nPos, FALSE_ASCII) == false)
			{
				NodeTwoCharRemove(m_pNode, "℃");
				CharSymbolDelete(m_pNode);
			}
			break;
		case 7:   // \, $, %
			if (IsCurrency(m_pNode->pPreToken->strOriginalSource, nPos, TRUE_ASCII) == false)
			{
				NodeOneCharRemove(m_pNode, "$");
				NodeOneCharRemove(m_pNode, "\\");
				NodeOneCharRemove(m_pNode, "%");
				CharSymbolDelete(m_pNode);
			}
			break;
		case 8:   // ￦, ＄
			if (IsCurrency(m_pNode->pPreToken->strOriginalSource, nPos, FALSE_ASCII) == false)
			{
				NodeTwoCharRemove(m_pNode, "￦");
				NodeTwoCharRemove(m_pNode, "＄");
				CharSymbolDelete(m_pNode);
			}
			break;
		case 9:   // @
			if (IsInternetChar(m_pNode->pPreToken->strOriginalSource, nPos, TRUE_ASCII) == false)
			{
				NodeTwoCharRemove(m_pNode, "@");
				CharSymbolDelete(m_pNode);
			}
			break;
		default:  // 나머지 특수 기호 지우기
			if (m_pNode->pPreToken->strOriginalSource[0] != ',')
			{
				CharSymbolDelete(m_pNode);
			}
		}
		//Next Node
		m_pNode = m_pNode->pNext;
	}

	// 문장의 끝이 안나왔을때 문장 분할
	if (pSentenceFirst != pPreLinkedList->m_pTailNode)
	{
		pSentenceLast = m_pNode->pPrev;
		SentenceAddNode(pSentenceFirst, pSentenceLast);	//문장분할

	}

}

// PRE_NODE의 First와 Last를 입력받아 문장 추가
void CPreProcessor::SentenceAddNode(PRE_NODE* pSentenceFirst, PRE_NODE* pSentenceLast)
{
	// 1. 문장 자료구조 생성 AddSentenceTail(char* pSentence, int nOffset)
	// 2. 문장 내 기호처리
	// 3. 문장의 형태소 자료구조 추가
	// 4. 문장의 String 만들기(4096)
	// 5. 문장의 String의 Replace
	// 6. 형태소 분석
	int nLen = strlen(m_pAllString);
	char* pSentence = (char*)malloc(strlen(m_pAllString) + 1);
	memset(pSentence, 0x00, strlen(m_pAllString) + 1);
	
	bool bFirst = false;
	PRE_NODE* pTempNode = pSentenceFirst;
	int nPos = 0;
	int i, n = 0;

	nPos = pSentenceLast->pPreToken->nOffset + strlen(pSentenceLast->pPreToken->strOriginalSource) + 1;


	for (i = pSentenceFirst->pPreToken->nOffset; i<nPos; i++)
	{
		pSentence[n++] = m_pAllString[i];
	}
	
	if (nLen > i)
	{
		while (m_pAllString[i] != ' '
			&& m_pAllString[i] != '\r'
			&& m_pAllString[i] != '\n'
			&& m_pAllString[i] != '\0')
		{
			pSentence[n++] = m_pAllString[i++];
		}
	}
	// Insert Setence
	m_pDocVector->AddSentenceTail(pSentence, pSentenceFirst->pPreToken->nOffset);

	while (pTempNode->pPreToken && pTempNode != pSentenceLast)
	{
		if (strlen(pTempNode->pPreToken->strOriginalSource) > 0)
		{
			m_pDocVector->AddTail(pTempNode->pPreToken->strOriginalSource, pTempNode->pPreToken->pFeature, pTempNode->pPreToken->nOffset);
		}
		pTempNode = pTempNode->pNext;
	}

	if (pTempNode->pPreToken && strlen(pTempNode->pPreToken->strOriginalSource) > 0)
	{
		m_pDocVector->AddTail(pTempNode->pPreToken->strOriginalSource, pTempNode->pPreToken->pFeature, pTempNode->pPreToken->nOffset);
	}

	// Sentence Replace
	if (strlen(pSentence) > 0)
	{
		// 형태소 분석
		//m_wndNAHangulAnaly->StartHangulAnaly();
		m_wndMorphoLogicalAnalysis->StartMorphoLogicalAnalysis();
	}
	else
	{
		m_pDocVector->RemoveTailSentence();
	}



	//******************************************************************************
	//2017-06-21 malloc 메모리 해제한다. 
	//******************************************************************************
	if(pSentence != NULL){
		free(pSentence);
		pSentence = NULL;
	}
}



int CPreProcessor::GetParentSymbol(char pChar, int nCase)
{
	int nReturn = 0;

	//괄호 열기
	if (nCase == 0)
	{
		if (pChar == '<')
			nReturn = SYMBOL_CHAR_1;
		else if (pChar == '(')
			nReturn = SYMBOL_CHAR_2;
		else if (pChar == '[')
			nReturn = SYMBOL_CHAR_3;
		else if (pChar == '{')
			nReturn = SYMBOL_CHAR_4;
	}
	//괄호 닫기
	else
	{
		if (pChar == '>')
			nReturn = SYMBOL_CHAR_5;
		else if (pChar == ')')
			nReturn = SYMBOL_CHAR_6;
		else if (pChar == ']')
			nReturn = SYMBOL_CHAR_7;
		else if (pChar == '}')
			nReturn = SYMBOL_CHAR_8;
	}

	return nReturn;
}

// 프로세스04 : 문장부할(특수기호 Return)
int CPreProcessor::CharFindSymbol(PRE_NODE* m_pNode, int* nPos)
{
	int nReturn = -1;
	int n, nLen = 0;
	char pOneChar[3];

	if (m_pNode != NULL && m_pNode->pPreToken != NULL && m_pNode->pPreToken->strOriginalSource != NULL)
	{
		nLen = strlen(m_pNode->pPreToken->strOriginalSource);

		for (n = 0; n<nLen; n++)
		{
			pOneChar[0] = m_pNode->pPreToken->strOriginalSource[n];
			pOneChar[1] = m_pNode->pPreToken->strOriginalSource[n + 1];
			pOneChar[2] = 0;
			if (pOneChar[0] == '.')
				nReturn = 1;
			else if (pOneChar[0] == '?' || pOneChar[0] == '!')
				nReturn = 2;
			else if (pOneChar[0] == '/')
				nReturn = 3;
			else if (strcmp(pOneChar, "·") == 0)
				nReturn = 4;
			else if (pOneChar[0] == '+' || pOneChar[0] == '-' ||
				pOneChar[0] == '*' || pOneChar[0] == '=' ||
				pOneChar[0] == ':' || pOneChar[0] == '~')
				nReturn = 5;
			else if (strcmp(pOneChar, "℃") == 0)
				nReturn = 6;
			else if (pOneChar[0] == '$' || pOneChar[0] == '\\' || pOneChar[0] == '%')
				nReturn = 7;
			else if (strcmp(pOneChar, "￦") == 0 || strcmp(pOneChar, "＄") == 0)
				nReturn = 8;
			else if (pOneChar[0] == '@')
				nReturn = 9;

			if (nReturn != -1)
			{
				*nPos = n;
				break;
			}
		}
	}
	return nReturn;
}

// 특수기호 찾기
// pSearch를 m_pNode->pPreToken->strOriginalSource에서 찾음
int CPreProcessor::CharFindSymbol(PRE_NODE* m_pNode, char* pSearch)
{
	int nReturn = -1;
	int n, nLen = 0, nLen2 = 0;
	char pOneChar[3];
	int nCount = 0;

	if (m_pNode != NULL && m_pNode->pPreToken != NULL &&
		m_pNode->pPreToken->strOriginalSource != NULL && pSearch != NULL)
	{
		nLen = strlen(m_pNode->pPreToken->strOriginalSource);
		nLen2 = strlen(pSearch);
		if (nLen2 == 2)
		{
			for (n = 0; n<nLen; n++)
			{
				pOneChar[0] = m_pNode->pPreToken->strOriginalSource[n];
				pOneChar[1] = m_pNode->pPreToken->strOriginalSource[n + 1];
				pOneChar[2] = 0;
				if (strcmp(pOneChar, pSearch) == 0)
					nCount += 1;
			}
		}
		else if (nLen2 == 1)
		{
			for (n = 0; n<nLen; n++)
			{
				pOneChar[0] = m_pNode->pPreToken->strOriginalSource[n];
				pOneChar[1] = 0;
				if (pOneChar[0] == pSearch[0])
					nCount += 1;
			}
		}
	}
	if (nCount != 0)
		nReturn = nCount;
	return nReturn;
}

// 특수기호 삭제(미삭제)
int CPreProcessor::CharSymbolDelete(PRE_NODE* m_pNode)
{
	int nReturn = -1;
	int n, nLen = 0;
	char pOneChar[3];
	char* pOri = NULL;
	int nTmp = 0;
	char pTemp[1204];
	int nChar = -1;

	memset(pTemp, 0x00, 1024);

	if (m_pNode != NULL && m_pNode->pPreToken != NULL && m_pNode->pPreToken->strOriginalSource != NULL)
	{
		pOri = m_pNode->pPreToken->strOriginalSource;
		nLen = strlen(pOri);
		for (n = 0; n<nLen;)
		{
			memset(pOneChar, 0x00, 3);
			pOneChar[0] = pOri[n];
			if (pOneChar[0] & 0x80) // 2 Byte Char?
			{
				pOneChar[1] = pOri[n + 1];
				nChar = -1;
				nChar = GetCharCode(pOneChar);
				if (nChar == SPECIAL_CHAR_WORD)
				{
					//℉￠￡ℓ㎀㎁㎂㎃㎄㎈㎊㎋㎌㎍㎎㎏㎐㎑㎒㎓㎔㎕㎖
					//㎗㎘㎙㎚㎛㎜㎝㎞㎟㎠㎡㎢㎣㎤㎥㎦㎧㎨㎩㎪㎫㎬㎭
					//㎮㎯㎰㎱㎲㎳㎴㎵㎶㎷㎸㎹㎺㎻㎼㎽㎾㎿㏀㏁㏂㏃㏄
					//㏅㏆㏇㏈㏉㏊㏏㏐㏓㏖㏘㏛㏜㏝
					if (strcmp(pOneChar, "℉") == 0 || strcmp(pOneChar, "￠") == 0 || strcmp(pOneChar, "￡") == 0 || strcmp(pOneChar, "㎘") == 0 || strcmp(pOneChar, "㎙") == 0 || strcmp(pOneChar, "㎚") == 0 ||
						strcmp(pOneChar, "㎛") == 0 || strcmp(pOneChar, "㎜") == 0 || strcmp(pOneChar, "㎝") == 0 || strcmp(pOneChar, "㎞") == 0 || strcmp(pOneChar, "㎟") == 0 || strcmp(pOneChar, "㎠") == 0 ||
						strcmp(pOneChar, "㎡") == 0 || strcmp(pOneChar, "㎢") == 0 || strcmp(pOneChar, "㎣") == 0 || strcmp(pOneChar, "㎤") == 0 || strcmp(pOneChar, "㎥") == 0 || strcmp(pOneChar, "㎦") == 0 ||
						strcmp(pOneChar, "㎧") == 0 || strcmp(pOneChar, "㎨") == 0 || strcmp(pOneChar, "㎩") == 0 || strcmp(pOneChar, "㎪") == 0 || strcmp(pOneChar, "㎫") == 0 || strcmp(pOneChar, "㎬") == 0 ||
						strcmp(pOneChar, "㎭") == 0 || strcmp(pOneChar, "㎮") == 0 || strcmp(pOneChar, "㎯") == 0 || strcmp(pOneChar, "㎰") == 0 || strcmp(pOneChar, "㎱") == 0 || strcmp(pOneChar, "㎲") == 0 ||
						strcmp(pOneChar, "㎳") == 0 || strcmp(pOneChar, "㎴") == 0 || strcmp(pOneChar, "㎵") == 0 || strcmp(pOneChar, "㎶") == 0 || strcmp(pOneChar, "㎷") == 0 || strcmp(pOneChar, "㎸") == 0 ||
						strcmp(pOneChar, "㎹") == 0 || strcmp(pOneChar, "㎺") == 0 || strcmp(pOneChar, "㎻") == 0 || strcmp(pOneChar, "㎼") == 0 || strcmp(pOneChar, "㎽") == 0 || strcmp(pOneChar, "㎾") == 0 ||
						strcmp(pOneChar, "㎿") == 0 || strcmp(pOneChar, "㏀") == 0 || strcmp(pOneChar, "㏁") == 0 || strcmp(pOneChar, "㏂") == 0 || strcmp(pOneChar, "㏃") == 0 || strcmp(pOneChar, "㏄") == 0 ||
						strcmp(pOneChar, "㏅") == 0 || strcmp(pOneChar, "㏆") == 0 || strcmp(pOneChar, "㏇") == 0 || strcmp(pOneChar, "㏈") == 0 || strcmp(pOneChar, "㏉") == 0 || strcmp(pOneChar, "㏊") == 0 ||
						strcmp(pOneChar, "㏏") == 0 || strcmp(pOneChar, "㏐") == 0 || strcmp(pOneChar, "㏓") == 0 || strcmp(pOneChar, "㏖") == 0 || strcmp(pOneChar, "㏘") == 0 || strcmp(pOneChar, "㏛") == 0 ||
						strcmp(pOneChar, "㏜") == 0 || strcmp(pOneChar, "㏝") == 0 || strcmp(pOneChar, "ℓ") == 0 || strcmp(pOneChar, "㎀") == 0 || strcmp(pOneChar, "㎁") == 0 || strcmp(pOneChar, "㎂") == 0 ||
						strcmp(pOneChar, "㎃") == 0 || strcmp(pOneChar, "㎄") == 0 || strcmp(pOneChar, "㎈") == 0 || strcmp(pOneChar, "㎊") == 0 || strcmp(pOneChar, "㎋") == 0 || strcmp(pOneChar, "㎌") == 0 ||
						strcmp(pOneChar, "㎍") == 0 || strcmp(pOneChar, "㎎") == 0 || strcmp(pOneChar, "㎏") == 0 || strcmp(pOneChar, "㎐") == 0 || strcmp(pOneChar, "㎑") == 0 || strcmp(pOneChar, "㎒") == 0 ||
						strcmp(pOneChar, "㎓") == 0 || strcmp(pOneChar, "㎔") == 0 || strcmp(pOneChar, "㎕") == 0 || strcmp(pOneChar, "㎖") == 0 || strcmp(pOneChar, "㎗") == 0 || strcmp(pOneChar, "℃") == 0 ||
						strcmp(pOneChar, "°") == 0 || strcmp(pOneChar, "±") == 0 || strcmp(pOneChar, "˚") == 0)
					{
						pTemp[nTmp++] = pOri[n++];
						pTemp[nTmp++] = pOri[n++];
					}
					else
					{
						n = n + 2;
						nReturn = 1;
					}
				}
				else if (nChar == TWIN_CHAR_WORD)
				{
					n = n + 2;
					nReturn = 1;
				}
				else
				{
					pTemp[nTmp++] = pOri[n++];
					pTemp[nTmp++] = pOri[n++];
				}
			}
			else
			{
				if (GetCharCode(pOneChar) == SPECIAL_CHAR_WORD)
				{
					n = n + 1;
					nReturn = 1;
				}
				else
					pTemp[nTmp++] = pOri[n++];
			}
		}
	}
	if (nReturn != -1)
	{
		//2017-11-14 Temp에 값이 존재 하지 않을 경우
		if(pTemp != "")
		{
			delete[] m_pNode->pPreToken->strOriginalSource;
			m_pNode->pPreToken->strOriginalSource = new char[strlen(pTemp) + 1];
			memset(m_pNode->pPreToken->strOriginalSource, 0x00, strlen(pTemp) + 1);
			strcpy(m_pNode->pPreToken->strOriginalSource, pTemp);
		}
	}

	return nReturn;
}

// 특수기호 (한자->한글변환)
int CPreProcessor::Hanja2Hangul(PRE_NODE* m_pNode)
{
	int nReturn = -1;
	int n, nLen = 0;
	char pOneChar[3];
	char* pOri = NULL;
	int nTmp = 0;
	char pTemp[1204];
	int nChar = -1;
	unsigned int hangul;

	memset(pTemp, 0x00, 1024);

	if (m_pNode != NULL && m_pNode->pPreToken != NULL && m_pNode->pPreToken->strOriginalSource != NULL)
	{
		pOri = m_pNode->pPreToken->strOriginalSource;
		nLen = strlen(pOri);
		for (n = 0; n<nLen;)
		{
			memset(pOneChar, 0x00, 3);
			pOneChar[0] = pOri[n];
			if (pOneChar[0] & 0x80) // 2 Byte Char?
			{
				pOneChar[1] = pOri[n + 1];
				nChar = -1;
				nChar = GetCharCode(pOneChar);
				if (nChar == CHINESE_CHAR_WORD)
				{
					hangul = hanja_conv(pOneChar[0], pOneChar[1]);
					pTemp[nTmp++] = (hangul >> 8) & 0xff;
					pTemp[nTmp++] = hangul & 0xff;
					n = n + 2;
					nReturn = 1;
				}
				else
				{
					pTemp[nTmp++] = pOri[n++];
					pTemp[nTmp++] = pOri[n++];
				}
			}
			else
				pTemp[nTmp++] = pOri[n++];
		}
	}
	if (nReturn != -1)
	{
		//2017-11-14 Temp에 값이 존재하지 않을 경우
		if(pTemp != "")
		{
			delete[] m_pNode->pPreToken->strOriginalSource;
			m_pNode->pPreToken->strOriginalSource = new char[strlen(pTemp) + 1];
			memset(m_pNode->pPreToken->strOriginalSource, 0x00, strlen(pTemp) + 1);
			strcpy(m_pNode->pPreToken->strOriginalSource, pTemp);
		}
	}

	return nReturn;
}

// 다음 문자 Char이동
bool CPreProcessor::NextCharPos(char* pChar, int& nPos)
{
	bool bReturn = false;
	int nLen = strlen(pChar);
	while (pChar[nPos] == ' ' || pChar[nPos] == '\r' || pChar[nPos] == '\n' || pChar[nPos] == '\t')
	{
		if (pChar[nPos] == '\r' || pChar[nPos] == '\n')
			bReturn = true;
		nPos++;

		if (nPos>nLen)
			break;
	}
	return bReturn;
}

int CPreProcessor::ParentNodeTotal(CNAPreLinkedList* pPreLinkedList, PRE_NODE* m_pNode)
{
	int nReturn = -1;
	int nCount = 0;
	//'<' 처리
	if (m_pNode->pPreToken->nFirstParent == SYMBOL_CHAR_1)
	{
		nReturn = SetParentNode1(pPreLinkedList, m_pNode, SYMBOL_CHAR_1, SYMBOL_CHAR_5);
		if (nReturn == -1)
		{
			NodeOneCharRemove(m_pNode, "<");
		}
	}
	//'(' 처리
	else if (m_pNode->pPreToken->nFirstParent == SYMBOL_CHAR_2)
	{
		nReturn = SetParentNode1(pPreLinkedList, m_pNode, SYMBOL_CHAR_2, SYMBOL_CHAR_6);
		if (nReturn == -1)
		{
			NodeOneCharRemove(m_pNode, "(");
		}
	}
	//'[' 처리
	else if (m_pNode->pPreToken->nFirstParent == SYMBOL_CHAR_3)
	{
		nReturn = SetParentNode1(pPreLinkedList, m_pNode, SYMBOL_CHAR_3, SYMBOL_CHAR_7);
		if (nReturn == -1)
		{
			NodeOneCharRemove(m_pNode, "[");
		}
	}
	//'{' 처리
	else if (m_pNode->pPreToken->nFirstParent == SYMBOL_CHAR_4)
	{
		nReturn = SetParentNode1(pPreLinkedList, m_pNode, SYMBOL_CHAR_4, SYMBOL_CHAR_8);
		if (nReturn == -1)
		{
			NodeOneCharRemove(m_pNode, "{");
		}
	}
	// '"' 처리
	else if ((nCount = CharFindSymbol(m_pNode, "\"")) != -1)
	{
		if (nCount > 1)
		{
			NodeOneCharRemove(m_pNode, "\"");
		}
		else if (SetParentNode2(pPreLinkedList, m_pNode) == false)
		{
			NodeOneCharRemove(m_pNode, "\"");
		}
		else
			nReturn = 1;
	}
	else if (m_pNode->pPreToken->nFirstParent == SYMBOL_CHAR_6)
	{
		NodeOneCharRemove(m_pNode, ")");
	}
	// "'" 처리
	else
	{
		nReturn = CharFindSymbol(m_pNode, "'");
		if (nReturn != -1)
		{
			NodeOneCharRemove(m_pNode, "'");
			nReturn = 1;
		}
	}

	return nReturn;
}

// '(', '{', '[', '<' 처리
int CPreProcessor::SetParentNode1(CNAPreLinkedList* pPreLinkedList, PRE_NODE* m_pNode, int nCase1, int nCase2)
{
	int nReturn = -1;
	int nParent = 0;
	int nLen, nParentPos = -1, n = -1;
	PRE_NODE* pFirstNode = m_pNode;
	PRE_NODE* pCommaTmpNode = NULL;
	char pDelChar1[2];
	char pDelChar2[2];

	memset(pDelChar1, 0x00, 2);
	memset(pDelChar2, 0x00, 2);

	if (nCase2 == SYMBOL_CHAR_5)
	{
		pDelChar1[0] = '<'; pDelChar2[0] = '>';
	}
	else if (nCase2 == SYMBOL_CHAR_6)
	{
		pDelChar1[0] = '('; pDelChar2[0] = ')';
	}
	else if (nCase2 == SYMBOL_CHAR_7)
	{
		pDelChar1[0] = '{'; pDelChar2[0] = '}';
	}
	else if (nCase2 == SYMBOL_CHAR_8)
	{
		pDelChar1[0] = '['; pDelChar2[0] = ']';
	}
	else
		return false;

	// 여는 괄호와 닫는 괄호가 같이 있을 때 예) 이성규(내추럴), (이성규)내추럴
	if (m_pNode->pPreToken->nFirstParent == m_pNode->pPreToken->nLastParent - 4)
	{
		nLen = strlen(m_pNode->pPreToken->strOriginalSource);
		// (내추럴)인 경우
		if (m_pNode->pPreToken->strOriginalSource[nLen - 1] == pDelChar2[0] &&
			m_pNode->pPreToken->strOriginalSource[0] == pDelChar1[0])
		{
			NodeOneCharRemove(m_pNode, pDelChar1);
			NodeOneCharRemove(m_pNode, pDelChar2);
			return 2;
		}
		// ')' 뒤에 있는가?
		else if (m_pNode->pPreToken->strOriginalSource[nLen - 1] == pDelChar2[0])
		{
			n = 0;
			while (m_pNode->pPreToken->strOriginalSource[n] != 0x00)
			{
				if (m_pNode->pPreToken->strOriginalSource[n] == pDelChar1[0])
				{
					nParentPos = n;
					break;
				}
				n++;
			}
			if (nParentPos > 0)
			{
				//------m_pPreLinkedList->PreNodeDivide(m_pNode, nParentPos, 1);
				pPreLinkedList->PreNodeDivide(m_pNode, nParentPos, 1);
				NodeOneCharRemove(m_pNode->pNext, pDelChar1);
				NodeOneCharRemove(m_pNode->pNext, pDelChar2);
			}
			return 2;
		}
		else if (m_pNode->pPreToken->strOriginalSource[0] == pDelChar1[0])
		{
			n = 0;
			while (m_pNode->pPreToken->strOriginalSource[n] != 0x00)
			{
				if (m_pNode->pPreToken->strOriginalSource[n] == pDelChar2[0])
				{
					nParentPos = n;
					break;
				}
				n++;
			}
			if (nParentPos > 0)
			{
				//------m_pPreLinkedList->PreNodeDivide(m_pNode, nParentPos, 1);
				pPreLinkedList->PreNodeDivide(m_pNode, nParentPos, 1);
				NodeOneCharRemove(m_pNode, pDelChar1);
				NodeOneCharRemove(m_pNode, pDelChar2);
			}
			return 2;
		}
	}

	// 여는 괄호루이고 그 위가가 토큰의 처음이 아닌가?
	if (m_pNode->pPreToken->strOriginalSource[0] != pDelChar1[0])
	{
		n = 0;
		// ')', '}', ']'. '>' Position 위치 저장
		while (m_pNode->pPreToken->strOriginalSource[n] != 0x00)
		{
			if (m_pNode->pPreToken->strOriginalSource[n] == pDelChar1[0])
			{
				nParentPos = n;
				break;
			}
			n++;
		}
	}

	// ')', '}', ']'. '>' 나올때까지 진행
	while (m_pNode != pPreLinkedList->m_pTailNode)
	{
		if (m_pNode->pPreToken->nFirstParent == nCase1)
		{
			nParent++;
		}

		if (m_pNode->pPreToken->nLastParent == nCase2)
		{
			nParent--;
			if (nParent == 0) //괄호의 시작과 끝
			{
				if (nParentPos > 0 && (int)strlen(pFirstNode->pPreToken->strOriginalSource) > nParentPos + 1)
				{
					// ')', '}', ']'. '>' Position 위치에서 PreNode 추가
					//------m_pPreLinkedList->PreNodeDivide(pFirstNode, nParentPos, 1);
					pPreLinkedList->PreNodeDivide(pFirstNode, nParentPos, 1);

					if (pFirstNode == m_pNode)
						m_pNode = m_pNode->pNext;

					nReturn = 2;
				}
				else
					NodeOneCharRemove(pFirstNode, pDelChar1);

				// pFirstNode에 [ns] 자질 Setting
				pFirstNode->pPreToken->pFeature[SYMBOL_NS] = '1';

				// m_pNode에 [ne] 자질 Setting
				m_pNode->pPreToken->pFeature[SYMBOL_NE] = '1';
				NodeOneCharRemove(m_pNode, pDelChar2);
				nLen = strlen(m_pNode->pPreToken->strOriginalSource);
				if (m_pNode->pPreToken->strOriginalSource[nLen - 1] == '.')
					m_pNode->pPreToken->strOriginalSource[nLen - 1] = 0;

				if (pCommaTmpNode != NULL)
				{
					pCommaTmpNode->pPreToken->nEndCheck = PRENODE_CHAR_4; //',' 인증
					pCommaTmpNode->pPreToken->pFeature[SYMBOL_COMMA] = '1';
					pCommaTmpNode->pPreToken->pFeature[SYMBOL_CLAUSE] = '1';
					pCommaTmpNode = m_pNode;
				}

				if (nReturn == -1)
					nReturn = 1;
				break;
			}
		}

		// ',' 처리부분
		if (m_pNode->pPreToken->nEndCheck == PRENODE_CHAR_3)
		{
			if (pCommaTmpNode != NULL)
			{
				pCommaTmpNode->pPreToken->nEndCheck = PRENODE_CHAR_4; //',' 인증
				pCommaTmpNode->pPreToken->pFeature[SYMBOL_COMMA] = '1';
				pCommaTmpNode->pPreToken->pFeature[SYMBOL_CLAUSE] = '1';
				pCommaTmpNode = m_pNode;
			}
			pCommaTmpNode = m_pNode;
		}

		m_pNode = m_pNode->pNext;
	}	// end while

	return nReturn;
}

// '"' 처리
bool CPreProcessor::SetParentNode2(CNAPreLinkedList* pPreLinkedList, PRE_NODE* m_pNode)
{
	bool bReturn = false;
	int nParent = 1;
	PRE_NODE* pCommaTmpNode = NULL;
	PRE_NODE* pFirstNode = m_pNode;
	char* pFeature = NULL;

	// '"' 나올때까지 진행
	//m_pNode = m_pNode->pNext;
	// 여는 따옴표 자질 부여
	if (pFirstNode != NULL && pFirstNode->pPreToken != NULL)
		pFeature = pFirstNode->pPreToken->pFeature;

	while (m_pNode != pPreLinkedList->m_pTailNode)
	{
		if (m_pNode != pFirstNode && CharFindSymbol(m_pNode, "\"") != -1)
		{
			if (m_pNode->pPreToken->strOriginalSource[0] != '\"')
			{
				nParent--;
				if (nParent == 0)
				{
					NodeOneCharRemove(pFirstNode, "\"");
					NodeOneCharRemove(m_pNode, "\"");
					NodeOneCharRemove(m_pNode, ".");
					//닫는 따옴표 자질 부여
					if (m_pNode != NULL && m_pNode->pPreToken != NULL)
					{
						//닫는 따옴표 자질 부여
						if (strlen(pFirstNode->pPreToken->strOriginalSource) == 0 &&
							pFirstNode->pNext != NULL && pFirstNode->pNext->pPreToken != NULL)
						{
							pFirstNode->pNext->pPreToken->pFeature[SYMBOL_TS] = '1';
							pFirstNode->pNext->pPreToken->nOffset = pFirstNode->pPreToken->nOffset;
						}
						else
							pFeature[SYMBOL_TS] = '1';
						m_pNode->pPreToken->pFeature[SYMBOL_TE] = '1';
					}

					bReturn = true;

					if (pCommaTmpNode != NULL)
					{
						pCommaTmpNode->pPreToken->nEndCheck = PRENODE_CHAR_4; //',' 인증
						pCommaTmpNode->pPreToken->pFeature[SYMBOL_COMMA] = '1';
						pCommaTmpNode->pPreToken->pFeature[SYMBOL_CLAUSE] = '1';
						pCommaTmpNode = m_pNode;
					}
					break;
				}
				else
					NodeOneCharRemove(m_pNode, "\"");
			}
			else
			{
				nParent++;
				NodeOneCharRemove(m_pNode, "\"");
			}
		}

		// ',' 처리부분
		if (m_pNode->pPreToken->nEndCheck == PRENODE_CHAR_3)
		{
			if (pCommaTmpNode != NULL)
			{
				pCommaTmpNode->pPreToken->nEndCheck = PRENODE_CHAR_4; //',' 인증
				pCommaTmpNode->pPreToken->pFeature[SYMBOL_COMMA] = '1';
				pCommaTmpNode->pPreToken->pFeature[SYMBOL_CLAUSE] = '1';
				pCommaTmpNode = m_pNode;
			}
			pCommaTmpNode = m_pNode;
		}

		m_pNode = m_pNode->pNext;
	}

	return bReturn;
}

bool CPreProcessor::NodeStrCmp(PRE_NODE* m_pNode, char* pCmp, int nCase)
{
	int nLen1 = 0;
	int nLen2 = 0;
	int i, j;

	if (m_pNode != NULL && m_pNode->pPreToken != NULL &&
		m_pNode->pPreToken->strOriginalSource != NULL && pCmp != NULL)
	{
		if (nCase == 0)
		{
			nLen1 = strlen(pCmp);
			nLen2 = strlen(m_pNode->pPreToken->strOriginalSource);
			for (i = 0; i<nLen2; i++)
			{
				for (j = 0; j<nLen1; j++)
				{
					if (m_pNode->pPreToken->strOriginalSource[i] == pCmp[j])
						return true;
				}
			}
		}
		// 첫글자가 비교되는 문자인지..
		else
		{
			nLen1 = strlen(pCmp);
			for (j = 0; j<nLen1; j++)
			{
				if (m_pNode->pPreToken->strOriginalSource[0] == pCmp[j])
					return true;
			}
		}
	}
	return false;
}

void CPreProcessor::NodeOneCharRemove(PRE_NODE* m_pNode, char* pCmp)
{
	int nLen = 0;
	int i;
	char pTmp[1024];
	int nTmp = 0;

	memset(pTmp, 0x00, 1024);

	if (m_pNode != NULL && m_pNode->pPreToken != NULL &&
		m_pNode->pPreToken->strOriginalSource != NULL && pCmp != NULL)
	{
		nLen = strlen(m_pNode->pPreToken->strOriginalSource);
		for (i = 0; i<nLen; i++)
		{
			if (m_pNode->pPreToken->strOriginalSource[i] != pCmp[0])
				pTmp[nTmp++] = m_pNode->pPreToken->strOriginalSource[i];
		}

		if (nTmp < nLen)
		{
			delete[] m_pNode->pPreToken->strOriginalSource;
			m_pNode->pPreToken->strOriginalSource = new char[strlen(pTmp) + 1];
			memset(m_pNode->pPreToken->strOriginalSource, 0x00, strlen(pTmp) + 1);
			strcpy(m_pNode->pPreToken->strOriginalSource, pTmp);
		}
	}
}

// 2 Byte Char 삭제
void CPreProcessor::NodeTwoCharRemove(PRE_NODE* m_pNode, char* pCmp) //pCmp 문자를 제거
{
	int i, nLen, nPos;
	char pTmp[1024];
	int nTmp = 0;
	char* pStr = NULL;

	memset(pTmp, 0x00, 1024);

	if (m_pNode != NULL && m_pNode->pPreToken != NULL &&
		m_pNode->pPreToken->strOriginalSource != NULL && pCmp != NULL)
	{
		nLen = strlen(m_pNode->pPreToken->strOriginalSource);
		pStr = strstr(m_pNode->pPreToken->strOriginalSource, pCmp);
		if (pStr != NULL)
		{
			nPos = pStr - m_pNode->pPreToken->strOriginalSource;
			for (i = 0; i<nLen; i++)
			{
				if (i == nPos)
					i++;
				else
					pTmp[nTmp++] = m_pNode->pPreToken->strOriginalSource[i];
			}
			delete[] m_pNode->pPreToken->strOriginalSource;
			m_pNode->pPreToken->strOriginalSource = new char[strlen(pTmp) + 1];
			memset(m_pNode->pPreToken->strOriginalSource, 0x00, strlen(pTmp) + 1);
			strcpy(m_pNode->pPreToken->strOriginalSource, pTmp);
		}
	}
}

//'/', '·' 처리
int CPreProcessor::IsSlachDot(char *pString, int nPos, int isAscii)
{
	int nReturn = -1;
	char pOneChar[3];
	int nStart = 0;
	int nEnd = 0;
	int nLen = strlen(pString);

	memset(pOneChar, 0x00, 3);

	switch (isAscii)
	{
		// '/' 경우(1 byte)
	case TRUE_ASCII:
		// 앞부분
		if (nPos - 1 >= 0)
		{
			pOneChar[0] = pString[nPos - 1];
			if (pOneChar[0] >= '0' && pOneChar[0] <= '9')
			{
				nStart = 1;
			}
			else if (nPos - 2>0)
			{
				memset(pOneChar, 0x00, 3);
				pOneChar[0] = pString[nPos - 2];
				pOneChar[1] = pString[nPos - 1];
				if (GetCharCode(pOneChar) == HANGUL_CHAR_WORD)
				{
					nStart = 2;
				}
			}
		}

		// 뒤부분
		if (nPos + 1 < nLen)
		{
			pOneChar[0] = pString[nPos + 1];
			if (pOneChar[0] >= '0' && pOneChar[0] <= '9')
			{
				nEnd = 1;
			}
			else if (nPos + 2 < nLen)
			{
				pOneChar[1] = pString[nPos + 2];
				if (GetCharCode(pOneChar) == HANGUL_CHAR_WORD)
				{
					nEnd = 2;
				}
			}
		}
		break;

		// '·' (2 byte)
	case FALSE_ASCII:
		// 앞부분
		// 앞부분
		if (nPos - 1 >= 0)
		{
			pOneChar[0] = pString[nPos - 1];
			if (pOneChar[0] >= '0' && pOneChar[0] <= '9')
			{
				nStart = 1;
			}
			else if (nPos - 2>0)
			{
				memset(pOneChar, 0x00, 3);
				pOneChar[0] = pString[nPos - 2];
				pOneChar[1] = pString[nPos - 1];
				if (GetCharCode(pOneChar) == HANGUL_CHAR_WORD)
				{
					nStart = 2;
				}
			}
		}

		// 뒤부분
		if (nPos + 1 < nLen)
		{
			pOneChar[0] = pString[nPos + 2];
			if (pOneChar[0] >= '0' && pOneChar[0] <= '9')
			{
				nEnd = 1;
			}
			else if (nPos + 2 < nLen)
			{
				pOneChar[1] = pString[nPos + 3];
				if (GetCharCode(pOneChar) == HANGUL_CHAR_WORD)
				{
					nEnd = 2;
				}
			}
		}
		break;
	}

	if (nStart == 1 && nEnd == 1)
		nReturn = 1;		//앞뒤 숫자 인경우
	else if (nStart == 1 && nEnd == 2)
		nReturn = 2;		//앞(숫자),뒷(한글)
	else if (nStart == 2 && nEnd == 1)
		nReturn = 3;		//앞(한글),뒤(숫자)
	else if (nStart == 2 && nEnd == 2)
		nReturn = 4;		//앞뒤 한글

	return nReturn;
}
//산술연산처리 
bool CPreProcessor::IsOperation(char *pString, int nPos, int isAscii)
{
	bool isbValue = false;
	int nTotalCount = 0;
	char pOneChar[3];

	nTotalCount = strlen(pString);
	memset(pOneChar, 0x00, 3);

	switch (isAscii)
	{
	case TRUE_ASCII:
		if (nPos - 1 < 0 || nPos == nTotalCount - 1)
			isbValue = false;
		else
		{
			if (nPos + 1 <= nTotalCount)
			{
				if ((pString[nPos - 1] >= '0' && pString[nPos - 1] <= '9' &&
					pString[nPos + 1] >= '0' && pString[nPos + 1] <= '9') ||

					(pString[nPos - 1] >= '0' && pString[nPos - 1] <= '9' &&
						pString[nPos + 1] >= 'a' && pString[nPos + 1] <= 'z') ||

						(pString[nPos - 1] >= '0' && pString[nPos - 1] <= '9' &&
							pString[nPos + 1] >= 'A' && pString[nPos + 1] <= 'Z') ||

							(pString[nPos - 1] >= 'a' && pString[nPos - 1] <= 'z' &&
								pString[nPos + 1] >= '0' && pString[nPos + 1] <= '9') ||

								(pString[nPos - 1] >= 'A' && pString[nPos - 1] <= 'Z' &&
									pString[nPos + 1] >= '0' && pString[nPos + 1] <= '9') ||

									(pString[nPos - 1] >= 'A' && pString[nPos - 1] <= 'Z' &&
										pString[nPos + 1] >= 'a' && pString[nPos + 1] <= 'z') ||

										(pString[nPos - 1] >= 'a' && pString[nPos - 1] <= 'z' &&
											pString[nPos + 1] >= 'A' && pString[nPos + 1] <= 'Z') ||

											(pString[nPos - 1] >= 'a' && pString[nPos - 1] <= 'z' &&
												pString[nPos + 1] >= 'a' && pString[nPos + 1] <= 'z') ||

												(pString[nPos - 1] >= 'A' && pString[nPos - 1] <= 'Z' &&
													pString[nPos + 1] >= 'A' && pString[nPos + 1] <= 'Z'))

					isbValue = true;
				else
					isbValue = false;
			}
			else
				isbValue = false;
		}
		break;
	case FALSE_ASCII:
		if (nPos - 1 >= 0 || nPos + 1 == nTotalCount - 1)
			isbValue = false;
		else
		{
			if (nPos + 2 <= nTotalCount)
			{
				if (pString[nPos - 1] >= '0' && pString[nPos - 1] <= '9' &&
					pString[nPos + 2] >= '0' && pString[nPos + 2] <= '9')
					isbValue = true;
				else
					isbValue = false;
			}
			else
				isbValue = false;
		}
		break;
	}
	return isbValue;
}

//통화단위처리
bool CPreProcessor::IsCurrency(char *pString, int nPos, int isAscii)
{
	bool isbValue = false;
	int nTotalCount = 0;
	nTotalCount = strlen(pString);

	switch (isAscii)
	{
	case TRUE_ASCII:
		if (nPos - 1 < 0 || nPos == nTotalCount - 1)
		{
			if (nPos >= 0)
			{
				if (nPos + 1 <= nTotalCount)
				{
					if (pString[nPos + 1] >= '0' && pString[nPos + 1] <= '9')
						isbValue = true;
					else
						isbValue = false;
				}
				else
					isbValue = false;
			}
			if (nPos == nTotalCount - 1)
			{
				if (nPos - 1 >= 0)
				{
					if (pString[nPos - 1] >= '0' && pString[nPos - 1] <= '9')
						isbValue = true;
					else
						isbValue = false;
				}
				else
					isbValue = false;
			}
		}
		else
		{
			if (nPos + 1 <= nTotalCount)
			{
				if (pString[nPos - 1] >= '0' && pString[nPos - 1] <= '9' ||
					pString[nPos + 1] >= '0' && pString[nPos + 1] <= '9')
					isbValue = true;
				else
					isbValue = false;
			}
			else
				isbValue = false;
		}
		break;
	case FALSE_ASCII:
		if (nPos - 1 <= 0 || nPos + 1 == nTotalCount - 1)
		{
			if (nPos - 1 <= 0)
			{
				if (nPos + 2 <= nTotalCount)
				{
					if (pString[nPos + 2] >= '0' && pString[nPos + 2] <= '9')
						isbValue = true;
					else
						isbValue = false;
				}
				else
					isbValue = false;
			}
			if (nPos + 1 == nTotalCount - 1)
			{
				if (nPos - 1 >= 0)
				{
					if (pString[nPos - 1] >= '0' && pString[nPos - 1] <= '9')
						isbValue = true;
					else
						isbValue = false;
				}
				else
					isbValue = false;
			}
		}
		else
		{
			if (nPos + 2 <= nTotalCount)
			{
				if (pString[nPos - 1] >= '0' && pString[nPos - 1] <= '9' &&
					pString[nPos + 2] >= '0' && pString[nPos + 2] <= '9')
					isbValue = true;
				else
					isbValue = false;
			}
			else
				isbValue = false;
		}
		break;
	}
	return isbValue;
}

//인터넷 기호 @
bool CPreProcessor::IsInternetChar(char *pString, int nPos, int isAscii)
{
	bool isbValue = false;
	int nTotalCount = 0;
	int nPrevPos = 0;
	int nNextPos = 0;
	bool bPrevAllCharEng = false;
	bool bNextAllCharEng = false;
	nTotalCount = strlen(pString);

	switch (isAscii)
	{
	case TRUE_ASCII:
		if (nPos - 1 < 0 || nPos == nTotalCount - 1)
			isbValue = false;
		else
		{
			if (nPos + 1 <= nTotalCount)
			{
				nPrevPos = nPos - 1;
				while (nPrevPos > 0)
				{
					if (pString[nPrevPos] & 0x80)
					{
						bPrevAllCharEng = false;
						break;
					}
					else
					{
						if (!bPrevAllCharEng)
							bPrevAllCharEng = true;
					}
					nPrevPos -= 1;
				}
				nNextPos = nPos + 1;
				while (nNextPos < nTotalCount)
				{
					if (pString[nNextPos] & 0x80)
					{
						bNextAllCharEng = false;
						break;
					}
					else
					{
						if (!bNextAllCharEng)
							bNextAllCharEng = true;
					}
					nNextPos += 1;
				}
				if (bPrevAllCharEng == true && bNextAllCharEng == true)
					isbValue = true;
				else
					isbValue = false;
			}
			else
				isbValue = false;
		}
		break;
	case FALSE_ASCII:
		break;
	}
	return isbValue;
}

//.에 대한 처리 
bool CPreProcessor::IsDot(char *pString, int nPos, int isAscii)
{
	bool isbValue = false;
	int nTotalCount = 0;
	char pOneChar[3];
	nTotalCount = strlen(pString);

	switch (isAscii)
	{
	case TRUE_ASCII:
		if (nPos - 1 < 0)
			isbValue = false;
		else if (nPos == nTotalCount - 1)
		{
			if (pString[nPos - 1] >= '0' && pString[nPos - 1] <= '9' ||
				pString[nPos - 1] >= 'a' && pString[nPos - 1] <= 'z' ||
				pString[nPos - 1] >= 'A' && pString[nPos - 1] <= 'Z')
				isbValue = true;
		}
		else
		{
			if (nPos + 1 <= nTotalCount)
			{
				if ((pString[nPos - 1] >= '0' && pString[nPos - 1] <= '9' && pString[nPos + 1] >= '0' && pString[nPos + 1] <= '9') ||
					(pString[nPos - 1] >= 'a' && pString[nPos - 1] <= 'z' && pString[nPos + 1] >= 'a' && pString[nPos + 1] <= 'z') ||
					(pString[nPos - 1] >= 'A' && pString[nPos - 1] <= 'Z' && pString[nPos + 1] >= 'A' && pString[nPos + 1] <= 'Z') ||
					(pString[nPos - 1] >= '0' && pString[nPos - 1] <= '9' && pString[nPos + 1] >= 'a' && pString[nPos + 1] <= 'z') ||
					(pString[nPos - 1] >= '0' && pString[nPos - 1] <= '9' && pString[nPos + 1] >= 'A' && pString[nPos + 1] <= 'Z') ||
					(pString[nPos - 1] >= 'a' && pString[nPos - 1] <= 'z' && pString[nPos + 1] >= '0' && pString[nPos + 1] <= '9') ||
					(pString[nPos - 1] >= 'A' && pString[nPos - 1] <= 'Z' && pString[nPos + 1] >= '0' && pString[nPos + 1] <= '9'))
					isbValue = true;
				else
					isbValue = false;
			}
			else
				isbValue = false;
		}
		break;
	case FALSE_ASCII:
		break;
	case 3:
		memset(pOneChar, 0x00, 3);
		pOneChar[0] = pString[nPos - 2];
		pOneChar[1] = pString[nPos - 1];

		if (GetCharCode(pOneChar) == HANGUL_CHAR_WORD)
		{
			if (nTotalCount > nPos + 2)
			{
				memset(pOneChar, 0x00, 3);
				pOneChar[0] = pString[nPos + 1];
				pOneChar[1] = pString[nPos + 2];
				if (GetCharCode(pOneChar) == HANGUL_CHAR_WORD)
				{
					isbValue = true;
				}
			}
		}
		break;
	}
	return isbValue;
}

// Hanja API
int CPreProcessor::hanja_bsearch(unsigned int item)
{
	int mid;
	int min = 0, max = N_HANJA;

	while (min <= max) {
		mid = (max + min) / 2;
		if (kschanja[mid].hanja == item)
			return mid;
		else if (kschanja[mid].hanja > item)
			max = mid - 1;
		else if (kschanja[mid].hanja < item)
			min = mid + 1;

	}
	return -1;
}

unsigned int CPreProcessor::hanja_conv(char hbyte, char lbyte)
{
	int i;
	unsigned int hanja = ((hbyte << 8) & 0xff00) | (lbyte & 0xff);

	i = hanja_bsearch(hanja);
	return ((i < 0) ? hanja : kschanja[i].hangul);
}

int CPreProcessor::IsKindCode(int high, int low)
{
	if ((high >= 0xB0 && high <= 0xC8) &&
		(low >= 0xA1 && low <= 0xFE)) // hangul code
	{
		return HANGUL_CHAR_WORD;
	}
	else if ((high == 0xAA) &&
		(low >= 0xA1 && low <= 0xF3))// hiragana
	{
		return HIRAGANA_CHAR_WORD;
	}
	else if ((high == 0xAB) &&
		(low >= 0xA1 && low <= 0xF6))// katakana
	{
		return KATAGANA_CHAR_WORD;
	}
	else if ((high >= 0xCA && high <= 0xCF) && (low >= 0xA1 && low <= 0xFE) //hanja area 1
		|| (high >= 0xD0 && high <= 0xDF) && (low >= 0xA1 && low <= 0xFE) //hanja area 2
		|| (high >= 0xE0 && high <= 0xEF) && (low >= 0xA1 && low <= 0xFE) //hanja area 3 
		|| (high >= 0xF0 && high <= 0xFD) && (low >= 0xA1 && low <= 0xFE))//hanja area 4 
	{
		return CHINESE_CHAR_WORD;
	}
	else if (high >= 'A' && high <= 'Z' || high >= 'a' && high <= 'z')//english
	{
		return ENGLISH_CHAR_WORD;
	}
	else if (high == 0xA1 && (low >= 0xa2 && low <= 0xa4)
		|| high == 0x63 || high == 0x21)//single sentence symbol(。, 、,·,?,!)
	{
		return SINGLE_CHAR_WORD;
	}
	else if ((high == 0xa1 && low <= 0xbd) || high == 0x28
		|| high == 0x29 || high == 0x60 || high == 0x62 || high == 0x91 || high == 0x93)//twin sentence symbol(『,』,「,」,【,】,(,),<,>,[,]/
	{
		return TWIN_CHAR_WORD;
	}
	else if (high >= '0' && high <= '9'
		|| high == 0xa1 && low == 0xcd
		|| high == 0xa3 && low == 0xa4
		|| high == 0xa3 && low == 0xdc)//number ..￦＄￥
	{
		return NUMBER_CHAR_WORD;
	}
	else //그외 특수문자로 모두 처리
		return SPECIAL_CHAR_WORD;
}

int CPreProcessor::GetHighLow(int nKindCode, int *high, int *low)
{
	int ret = 0;
	*high = (nKindCode & 0xFF00) >> 8;
	*low = (nKindCode & 0x00FF);
	ret = IsKindCode(*high, *low);
	return ret;
}

int CPreProcessor::GetSylCode(char *pKindCode)
{
	int syl = 0;
	char *p = pKindCode;
	syl = (*p << 8) & 0x0000FF00;
	syl |= *(p + 1) & 0x000000FF;
	return syl;
}

int CPreProcessor::GetCharCode(char *pKindCode)
{
	int syl = 0;
	char *p = pKindCode;
	syl = (*p << 8) & 0x0000FF00;
	syl |= *(p + 1) & 0x000000FF;
	return GetHighLow(syl);
}

int CPreProcessor::GetHighLow(int nKindCode)
{
	int ret = 0;
	int high, low;
	high = (nKindCode & 0xFF00) >> 8;
	low = (nKindCode & 0x00FF);
	ret = IsKindCode(high, low);
	return ret;
}

double CPreProcessor::GetAnalysisResultTime()
{
	return m_nAnalysisResultTime;
}