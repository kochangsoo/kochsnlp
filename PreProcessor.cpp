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
pAllString ������ ������ Space ������ PreToken
Space�� ������ �� ���� Char�� ���ö����� ��ġ�̵�
'\r', '\n'�� ������ nCase = 1�� �ؼ� PreToken�� ����
��ġ �̵������� ���ڿ��� PreToken�� ����
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

	int nTT = 0;	/*����ǥ*/
	int nSmalTT = -1;
	
	CNAPreLinkedList* pPreLinkedList = NULL;

	m_pAllString = pAllString;

	pPreLinkedList = new CNAPreLinkedList;
	pPreLinkedList->InitHeadTailNode();

	//Documnet Vector Init
	RemoveDocVectorData();
	

	nTotalCount = strlen(pAllString);
	memset(pSaveText, 0x00, 1024);

	//�м� ���۽ð� ����
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

			// �ѱ� + '.' + �ѱ� ���� �Ǵ�
			if (
				strlen(pSaveText) == 2 
				&& strcmp(pSaveText, "��") == 0 
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
						strstr("��,��,��,��,��,��,��,��,��,��,��,��,��,��,��", pOneChar) != 0 ||
						strstr("��,��,��,��,��,��,��,��,��,��,��,��,��,��,��", pOneChar) != 0 ||
						strstr("��,��,��,��,��,��,��,��,��,��,��,��,��,��,��", pOneChar) != 0 ||
						strstr("��,��,��,��,��,��,��,��,��,��,��,��,��,��,��", pOneChar) != 0 ||
						strstr("��,��,��,��,��,��,��,��,��,��,��,��,��,��,��", pOneChar) != 0 ||
						strstr("��,��,��,��,��,��,��,��,��,��,��,��,��,��,��", pOneChar) != 0 ||
						strstr("«,´,��,��,��,��,��,è,��,��,ħ,��,��,š,ŭ", pOneChar) != 0 ||
						strstr("Ŵ,Ž,��,��,Ʀ,ƴ,ƺ,��,��,��,��,�I,ǰ,��,��", pOneChar) != 0 ||
						strstr("��,��,��,��,��,��", pOneChar) != 0
					))
			{
				nCharCount++;
				nCase = PRENODE_CHAR_3;//'?', '!', '.'
				if (strlen(pSaveText) > 0)
					bAdd = true;
			}
			else if ((int)strlen(pSaveText) > 2 
					&& strcmp("��", pOneChar) == 0 
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
				// ','���� ���ڰ� ������ ���� �߰�
				if (strlen(pSaveText) > 0)
				{
					pPreLinkedList->AddPreTail(pSaveText, nSpaceOffset, PRENODE_CHAR_1, nFirstParent, nLastParent);//' '
					//Comma ���� ����
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
		else if (pAllString[nCharCount] == '-')//��� ������
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
				if (strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0)
				{
					pSaveText[nSaveText++] = '"';
					nTT = 1;
				}
				else if (strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0)
				{
				}
				else if (strcmp(pOneChar, "��") == 0)
					pSaveText[nSaveText++] = '~';
				else if (strcmp(pOneChar, "��") == 0)
					pSaveText[nSaveText++] = '$';
				else if (strcmp(pOneChar, "��") == 0)
					pSaveText[nSaveText++] = '\\';
				else if (
							strcmp(pOneChar, "��") == 0 
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

				if (nFirstParent == 0)	//���� ��ȣ
					nFirstParent = GetParentSymbol(pAllString[nCharCount]);
				if (nLastParent == 0)  //�ݴ� ��ȣ
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

					//���,�ݴ°�ȣ�� ���ÿ� ���ð�� �Ѵܾ�� ó�� ��) (�ѱ�)
					if (nFirstParent == nLastParent - 4)
					{
						ptmpOneChar[0] = pAllString[nCharCount];
						ptmpOneChar[1] = pAllString[nCharCount + 1];
						ptmpOneChar[2] = 0;
						if (pAllString[nCharCount + 2] == 0 
							&&(strcmp(ptmpOneChar, "��") == 0 || strcmp(ptmpOneChar, "��") == 0 
								|| strcmp(ptmpOneChar, "��") == 0 || strcmp(ptmpOneChar, "��") == 0 
								|| strcmp(ptmpOneChar, "��") == 0 || strcmp(ptmpOneChar, "��") == 0))
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
				// [Comman] ���� Setting
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
		// ����� ó��
		else if (nSaveText >= LONG_TOKEN - 1)//40
		{
			for (nLong1 = 0; nLong1<nSaveText; nLong1++)
			{
				memset(pLongChar, 0x00, 3);
				pLongChar[0] = pSaveText[nLong1];
				pLongChar[1] = pSaveText[nLong1 + 1];
				if (strstr("��,��,��,��,��,��,��,��,��,��,��,��,��,��,��,��", pLongChar) != NULL 
					||strstr("��,��,��,��,��,��,��,��,��,��,��,��,��,��,��,��,��", pLongChar) != NULL 
					||strstr("��,��,��,��,��,��,��,��,��,��,��,��,��,��,��,��,��", pLongChar) != NULL)
				{
					memset(pLongTmp, 0x00, 1024);
					nFirstParent = 0;
					nLastParent = 0;
					for (nLong2 = 0; nLong2<nLong1 + 2; nLong2++)
					{
						if (nFirstParent == 0)	//���� ��ȣ
							nFirstParent = GetParentSymbol(pAllString[nCharCount]);
						if (nLastParent == 0)  //�ݴ� ��ȣ
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
			//2017-08-16 ����и��� ���������� �̷�� ���� ���� ��쿡 ���� �����ϰ� ����� ���ѷ��������ϰ� �߰���.
			//���忹 : if (size != -1) printf ("Size = %ld\n", size); else printf("File Not Found\n");
			//nCharCount++;
		}
	}

	// Space ������ ������ ���� ���� ���� �� H_NODE ����
	PreLinkedNode(pPreLinkedList);
	


	// Pre Node ����
	if (pPreLinkedList != NULL)
	{
		pPreLinkedList->RemovePreAll();
		delete pPreLinkedList;
		pPreLinkedList = NULL;
	}

	
	//�м�����ð�����
	clockEndTimer = clock();
	m_nAnalysisResultTime = (double)(clockEndTimer - clockStartTimer) / CLOCKS_PER_SEC;
}



//-------------------------------------------------------------------------------
// m_pNode : ���� PreNode ��ġ
// pSetenceHeadNode : ������ ù ��ġ
// pSetenceTailNode : ������ ������ ��ġ
// m_pNode�� �������鼭 pSetenceHeadNode�� �����ϰ� pSetenceTailNode�� �����Ǹ�
// pSetenceHeadNode���� pSetenceTailNode���� '����'�� �߰��ϰ� H_NODE�� �߰�
//-------------------------------------------------------------------------------
void CPreProcessor::PreLinkedNode(CNAPreLinkedList* pPreLinkedList)
{
	char* pstrText = NULL;
	char* pUnitStr = NULL;
	int nLen = 0;
	int nOffset = 0;
	int nParent = 0;
	int nPos = -1; // Ư����ȣ ��ġ
	bool bHanja = true;

	int nLeftSymbol = -1;
	int nRightSymbol = -1;
	char pOneChar[3];
	int nOneLen = 0;

	PRE_NODE* m_pNode = NULL;
	PRE_NODE* pTmpNode = NULL;
	PRE_NODE* pCommaTmpNode = NULL;
	PRE_NODE* pParentNode = NULL;		//��ȣ ù��° ��ġ '('
	PRE_NODE* pSentenceFirst = NULL;
	PRE_NODE* pSentenceLast = NULL;

	m_pNode = pPreLinkedList->m_pHeadNode->pNext;
	pSentenceFirst = m_pNode;

	while (m_pNode != pPreLinkedList->m_pTailNode)
	{
		// '\n'�� ���ӵǾ� ������ ��� ���� ����
		if (m_pNode->pPreToken->nEndCheck == PRENODE_CHAR_6)//('\r', '\n' )*2
		{
			pSentenceLast = m_pNode;
			SentenceAddNode(pSentenceFirst, pSentenceLast);	//�������
			pSentenceFirst = m_pNode->pNext;//���� ù��° Link
			pSentenceLast = NULL;
		}
		// '.' ó��
		else if (m_pNode->pPreToken->nEndCheck == PRENODE_CHAR_3)//'?', '!', '.'
		{
			memset(pOneChar, 0x00, 3);
			pOneChar[0] = m_pNode->pPreToken->strOriginalSource[0];
			pOneChar[1] = m_pNode->pPreToken->strOriginalSource[1];

			// ��ħǥ : ��ū�� �ι��ڷ� �����Ǿ� �ְ�, ���ڰ� ��(��. ��. a.) �ƴѰ��)
			if (!(m_pNode->pPreToken->strOriginalSource[2] == '.' &&
				strstr("��,��,��,��,��,��,��,��,��,��,��,��,��,��,��,��,��,��,��,��,��,��,ī,Ÿ,��,��", pOneChar) != 0) &&
				!(m_pNode->pPreToken->strOriginalSource[1] == '.' &&
				((m_pNode->pPreToken->strOriginalSource[0] >= 'a' && m_pNode->pPreToken->strOriginalSource[0] <= 'z') ||
					(m_pNode->pPreToken->strOriginalSource[0] >= 'A' && m_pNode->pPreToken->strOriginalSource[0] <= 'Z') ||
					(m_pNode->pPreToken->strOriginalSource[0] >= '0' && m_pNode->pPreToken->strOriginalSource[0] <= '9'))))
			{
				pSentenceLast = m_pNode;
				NodeOneCharRemove(m_pNode, ".");
				SentenceAddNode(pSentenceFirst, pSentenceLast);	//�������
				pSentenceFirst = m_pNode->pNext;//���� ù��° Link
				pSentenceLast = NULL;
			}
		}
		// '.', '?', '!'
		else if (m_pNode->pPreToken->nEndCheck == PRENODE_CHAR_5)//('?', '!', '.') + ('\r', '\n')
		{
			memset(pOneChar, 0x00, 3);
			pOneChar[0] = m_pNode->pPreToken->strOriginalSource[0];
			pOneChar[1] = m_pNode->pPreToken->strOriginalSource[1];

			// ��ħǥ : ��ū�� �ι��ڷ� �����Ǿ� �ְ�, ���ڰ� ��(��. ��. a.) �ƴѰ��)
			if (!(m_pNode->pPreToken->strOriginalSource[2] == '.' &&
				strstr("��,��,��,��,��,��,��,��,��,��,��,��,��,��,��,��,��,��,��,��,��,��,ī,Ÿ,��,��", pOneChar) != 0) &&
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
				SentenceAddNode(pSentenceFirst, pSentenceLast);	//�������
				pSentenceFirst = m_pNode->pNext;//���� ù��° Link
				pSentenceLast = NULL;
			}
		}
		else if (m_pNode->pPreToken->nEndCheck == PRENODE_CHAR_2 && m_pNode->pNext != pPreLinkedList->m_pTailNode)//'\r','\n'
		{
			pSentenceLast = m_pNode;
			SentenceAddNode(pSentenceFirst, pSentenceLast);	//�������
			pSentenceFirst = m_pNode->pNext;//���� ù��° Link
			pSentenceLast = NULL;
		}
		// 4. ����ǥ�� ��ȣ ó��
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
				//��ȣ�� �ִ� ���ڴ� ��ó��
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
		case 3:   // / ������
				  // �ѱ��϶�
			if (IsSlachDot(m_pNode->pPreToken->strOriginalSource, nPos, TRUE_ASCII) != 1)
			{
				pPreLinkedList->PreNodeDivide(m_pNode, nPos, 1);

				// [ns], [ne] ���� Setting
				if (m_pNode->pPreToken->pFeature[SYMBOL_NE] == '1')//214
					m_pNode->pPreToken->pFeature[SYMBOL_NE] = '0';
				else
					m_pNode->pPreToken->pFeature[SYMBOL_NS] = '1';
				if (m_pNode->pNext != NULL && m_pNode->pNext->pPreToken != NULL)
					m_pNode->pNext->pPreToken->pFeature[SYMBOL_NE] = '1';
			}
			break;
		case 4:   // ��(2) �����
				  // �ѱ��϶�
			if (IsSlachDot(m_pNode->pPreToken->strOriginalSource, nPos, FALSE_ASCII) != 1)
			{
				pPreLinkedList->PreNodeDivide(m_pNode, nPos, 2);
				// [ns], [ne] ���� Setting
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
		case 6:   // ��(2) ���ĺ�ȣ
			if (IsOperation(m_pNode->pPreToken->strOriginalSource, nPos, FALSE_ASCII) == false)
			{
				NodeTwoCharRemove(m_pNode, "��");
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
		case 8:   // ��, ��
			if (IsCurrency(m_pNode->pPreToken->strOriginalSource, nPos, FALSE_ASCII) == false)
			{
				NodeTwoCharRemove(m_pNode, "��");
				NodeTwoCharRemove(m_pNode, "��");
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
		default:  // ������ Ư�� ��ȣ �����
			if (m_pNode->pPreToken->strOriginalSource[0] != ',')
			{
				CharSymbolDelete(m_pNode);
			}
		}
		//Next Node
		m_pNode = m_pNode->pNext;
	}

	// ������ ���� �ȳ������� ���� ����
	if (pSentenceFirst != pPreLinkedList->m_pTailNode)
	{
		pSentenceLast = m_pNode->pPrev;
		SentenceAddNode(pSentenceFirst, pSentenceLast);	//�������

	}

}

// PRE_NODE�� First�� Last�� �Է¹޾� ���� �߰�
void CPreProcessor::SentenceAddNode(PRE_NODE* pSentenceFirst, PRE_NODE* pSentenceLast)
{
	// 1. ���� �ڷᱸ�� ���� AddSentenceTail(char* pSentence, int nOffset)
	// 2. ���� �� ��ȣó��
	// 3. ������ ���¼� �ڷᱸ�� �߰�
	// 4. ������ String �����(4096)
	// 5. ������ String�� Replace
	// 6. ���¼� �м�
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
		// ���¼� �м�
		//m_wndNAHangulAnaly->StartHangulAnaly();
		m_wndMorphoLogicalAnalysis->StartMorphoLogicalAnalysis();
	}
	else
	{
		m_pDocVector->RemoveTailSentence();
	}



	//******************************************************************************
	//2017-06-21 malloc �޸� �����Ѵ�. 
	//******************************************************************************
	if(pSentence != NULL){
		free(pSentence);
		pSentence = NULL;
	}
}



int CPreProcessor::GetParentSymbol(char pChar, int nCase)
{
	int nReturn = 0;

	//��ȣ ����
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
	//��ȣ �ݱ�
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

// ���μ���04 : �������(Ư����ȣ Return)
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
			else if (strcmp(pOneChar, "��") == 0)
				nReturn = 4;
			else if (pOneChar[0] == '+' || pOneChar[0] == '-' ||
				pOneChar[0] == '*' || pOneChar[0] == '=' ||
				pOneChar[0] == ':' || pOneChar[0] == '~')
				nReturn = 5;
			else if (strcmp(pOneChar, "��") == 0)
				nReturn = 6;
			else if (pOneChar[0] == '$' || pOneChar[0] == '\\' || pOneChar[0] == '%')
				nReturn = 7;
			else if (strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0)
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

// Ư����ȣ ã��
// pSearch�� m_pNode->pPreToken->strOriginalSource���� ã��
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

// Ư����ȣ ����(�̻���)
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
					//���ˡ̧��ɧʧ˧̧ͧ��ܧݧާ������ԧէ֧קا���
					//������������������������������������������
					//��㧿�����§çħŧƧǧȧΧϧЧѧҧӧڧۢ�짦
					//���᧼�������ߢ����
					if (strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 ||
						strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 ||
						strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 ||
						strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 ||
						strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 ||
						strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 ||
						strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 ||
						strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 ||
						strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 ||
						strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 ||
						strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 ||
						strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 ||
						strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 ||
						strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 ||
						strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0)
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
		//2017-11-14 Temp�� ���� ���� ���� ���� ���
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

// Ư����ȣ (����->�ѱۺ�ȯ)
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
		//2017-11-14 Temp�� ���� �������� ���� ���
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

// ���� ���� Char�̵�
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
	//'<' ó��
	if (m_pNode->pPreToken->nFirstParent == SYMBOL_CHAR_1)
	{
		nReturn = SetParentNode1(pPreLinkedList, m_pNode, SYMBOL_CHAR_1, SYMBOL_CHAR_5);
		if (nReturn == -1)
		{
			NodeOneCharRemove(m_pNode, "<");
		}
	}
	//'(' ó��
	else if (m_pNode->pPreToken->nFirstParent == SYMBOL_CHAR_2)
	{
		nReturn = SetParentNode1(pPreLinkedList, m_pNode, SYMBOL_CHAR_2, SYMBOL_CHAR_6);
		if (nReturn == -1)
		{
			NodeOneCharRemove(m_pNode, "(");
		}
	}
	//'[' ó��
	else if (m_pNode->pPreToken->nFirstParent == SYMBOL_CHAR_3)
	{
		nReturn = SetParentNode1(pPreLinkedList, m_pNode, SYMBOL_CHAR_3, SYMBOL_CHAR_7);
		if (nReturn == -1)
		{
			NodeOneCharRemove(m_pNode, "[");
		}
	}
	//'{' ó��
	else if (m_pNode->pPreToken->nFirstParent == SYMBOL_CHAR_4)
	{
		nReturn = SetParentNode1(pPreLinkedList, m_pNode, SYMBOL_CHAR_4, SYMBOL_CHAR_8);
		if (nReturn == -1)
		{
			NodeOneCharRemove(m_pNode, "{");
		}
	}
	// '"' ó��
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
	// "'" ó��
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

// '(', '{', '[', '<' ó��
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

	// ���� ��ȣ�� �ݴ� ��ȣ�� ���� ���� �� ��) �̼���(���߷�), (�̼���)���߷�
	if (m_pNode->pPreToken->nFirstParent == m_pNode->pPreToken->nLastParent - 4)
	{
		nLen = strlen(m_pNode->pPreToken->strOriginalSource);
		// (���߷�)�� ���
		if (m_pNode->pPreToken->strOriginalSource[nLen - 1] == pDelChar2[0] &&
			m_pNode->pPreToken->strOriginalSource[0] == pDelChar1[0])
		{
			NodeOneCharRemove(m_pNode, pDelChar1);
			NodeOneCharRemove(m_pNode, pDelChar2);
			return 2;
		}
		// ')' �ڿ� �ִ°�?
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

	// ���� ��ȣ���̰� �� ������ ��ū�� ó���� �ƴѰ�?
	if (m_pNode->pPreToken->strOriginalSource[0] != pDelChar1[0])
	{
		n = 0;
		// ')', '}', ']'. '>' Position ��ġ ����
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

	// ')', '}', ']'. '>' ���ö����� ����
	while (m_pNode != pPreLinkedList->m_pTailNode)
	{
		if (m_pNode->pPreToken->nFirstParent == nCase1)
		{
			nParent++;
		}

		if (m_pNode->pPreToken->nLastParent == nCase2)
		{
			nParent--;
			if (nParent == 0) //��ȣ�� ���۰� ��
			{
				if (nParentPos > 0 && (int)strlen(pFirstNode->pPreToken->strOriginalSource) > nParentPos + 1)
				{
					// ')', '}', ']'. '>' Position ��ġ���� PreNode �߰�
					//------m_pPreLinkedList->PreNodeDivide(pFirstNode, nParentPos, 1);
					pPreLinkedList->PreNodeDivide(pFirstNode, nParentPos, 1);

					if (pFirstNode == m_pNode)
						m_pNode = m_pNode->pNext;

					nReturn = 2;
				}
				else
					NodeOneCharRemove(pFirstNode, pDelChar1);

				// pFirstNode�� [ns] ���� Setting
				pFirstNode->pPreToken->pFeature[SYMBOL_NS] = '1';

				// m_pNode�� [ne] ���� Setting
				m_pNode->pPreToken->pFeature[SYMBOL_NE] = '1';
				NodeOneCharRemove(m_pNode, pDelChar2);
				nLen = strlen(m_pNode->pPreToken->strOriginalSource);
				if (m_pNode->pPreToken->strOriginalSource[nLen - 1] == '.')
					m_pNode->pPreToken->strOriginalSource[nLen - 1] = 0;

				if (pCommaTmpNode != NULL)
				{
					pCommaTmpNode->pPreToken->nEndCheck = PRENODE_CHAR_4; //',' ����
					pCommaTmpNode->pPreToken->pFeature[SYMBOL_COMMA] = '1';
					pCommaTmpNode->pPreToken->pFeature[SYMBOL_CLAUSE] = '1';
					pCommaTmpNode = m_pNode;
				}

				if (nReturn == -1)
					nReturn = 1;
				break;
			}
		}

		// ',' ó���κ�
		if (m_pNode->pPreToken->nEndCheck == PRENODE_CHAR_3)
		{
			if (pCommaTmpNode != NULL)
			{
				pCommaTmpNode->pPreToken->nEndCheck = PRENODE_CHAR_4; //',' ����
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

// '"' ó��
bool CPreProcessor::SetParentNode2(CNAPreLinkedList* pPreLinkedList, PRE_NODE* m_pNode)
{
	bool bReturn = false;
	int nParent = 1;
	PRE_NODE* pCommaTmpNode = NULL;
	PRE_NODE* pFirstNode = m_pNode;
	char* pFeature = NULL;

	// '"' ���ö����� ����
	//m_pNode = m_pNode->pNext;
	// ���� ����ǥ ���� �ο�
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
					//�ݴ� ����ǥ ���� �ο�
					if (m_pNode != NULL && m_pNode->pPreToken != NULL)
					{
						//�ݴ� ����ǥ ���� �ο�
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
						pCommaTmpNode->pPreToken->nEndCheck = PRENODE_CHAR_4; //',' ����
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

		// ',' ó���κ�
		if (m_pNode->pPreToken->nEndCheck == PRENODE_CHAR_3)
		{
			if (pCommaTmpNode != NULL)
			{
				pCommaTmpNode->pPreToken->nEndCheck = PRENODE_CHAR_4; //',' ����
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
		// ù���ڰ� �񱳵Ǵ� ��������..
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

// 2 Byte Char ����
void CPreProcessor::NodeTwoCharRemove(PRE_NODE* m_pNode, char* pCmp) //pCmp ���ڸ� ����
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

//'/', '��' ó��
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
		// '/' ���(1 byte)
	case TRUE_ASCII:
		// �պκ�
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

		// �ںκ�
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

		// '��' (2 byte)
	case FALSE_ASCII:
		// �պκ�
		// �պκ�
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

		// �ںκ�
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
		nReturn = 1;		//�յ� ���� �ΰ��
	else if (nStart == 1 && nEnd == 2)
		nReturn = 2;		//��(����),��(�ѱ�)
	else if (nStart == 2 && nEnd == 1)
		nReturn = 3;		//��(�ѱ�),��(����)
	else if (nStart == 2 && nEnd == 2)
		nReturn = 4;		//�յ� �ѱ�

	return nReturn;
}
//�������ó�� 
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

//��ȭ����ó��
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

//���ͳ� ��ȣ @
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

//.�� ���� ó�� 
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
		|| high == 0x63 || high == 0x21)//single sentence symbol(��, ��,��,?,!)
	{
		return SINGLE_CHAR_WORD;
	}
	else if ((high == 0xa1 && low <= 0xbd) || high == 0x28
		|| high == 0x29 || high == 0x60 || high == 0x62 || high == 0x91 || high == 0x93)//twin sentence symbol(��,��,��,��,��,��,(,),<,>,[,]/
	{
		return TWIN_CHAR_WORD;
	}
	else if (high >= '0' && high <= '9'
		|| high == 0xa1 && low == 0xcd
		|| high == 0xa3 && low == 0xa4
		|| high == 0xa3 && low == 0xdc)//number ..�ܣ���
	{
		return NUMBER_CHAR_WORD;
	}
	else //�׿� Ư�����ڷ� ��� ó��
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