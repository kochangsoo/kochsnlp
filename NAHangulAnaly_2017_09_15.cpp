#include "NAHangulAnaly.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNAHangulAnaly::CNAHangulAnaly()
{
	m_LinkedList = NULL;
	m_pConvert = new CConvert;
	m_pAmbiguity = new CNAAmbiguity;

	m_pPrevOldBuffer = NULL;
	m_pOldBuffer = NULL;
	m_pNewBuffer = NULL;
	m_pRealOriginalString = NULL;
	m_bIsFirstWord = true;
	m_bOneCharMask = false;

	m_nListPosition = 0;
	m_nListCount = 0;
	m_nInsertNodeCount = 0;
	m_nTableNumber = -1;
	m_nTableIndex = -1;
	m_nTableString = -1;

	memset(m_ArrayOldBuffer, 0x00, sizeof(m_ArrayOldBuffer));
	memset(m_ArrayNewBuffer, 0x00, sizeof(m_ArrayNewBuffer));

	m_pStemEndDic = NULL;

	m_pFormLenToNWord = NULL;//S(len~n)
	m_pFormNTo1Word = NULL;//S(n~1)
	m_pFormNPluseNumTo1Word = NULL;//S(n+1~1)

	m_pStemDicWord = NULL;//���ۻ��� �׼��� �ܾ�
	m_pStemDicDeleteJongWord = NULL;//���ۻ��� �׼��� �ܾ�(�������ŵ� ����)
	m_pEndingDicWord = NULL;//�������� �׼��� �ܾ�
	m_pEndingDicDeleteJongWord = NULL;//������ �׼��� �ܾ�(�������ŵ� ����)

	m_pOneNPluseNumChar = NULL;//n+1��°�� �ܾ�
	m_pOneNMinusNumChar = NULL;//n-1��°�� �ܾ�
	m_pNthOneChar = NULL;//N��° �ش� �ܾ�
	m_pJongChar = NULL;
	m_pChoJungChar = NULL;
	m_nPluseSyllable = 0;//nStart �������� +1(��->��) ��ġ�� �ش��ϴ� ������ ��
	m_nMinusSyllable = 0;//nStart �������� -1(��->��) ��ġ�� �ش��ϴ� ������ ��

	m_stemFeature01[STEM_DIC_ALL_FEATURE_NUMBER+1] = {0,};//��������
	m_stemFeature02[STEM_DIC_ALL_FEATURE_NUMBER + 1] = { 0, };//��������
	m_stemFeature03[STEM_DIC_ALL_FEATURE_NUMBER + 1] = { 0, };//��������
	m_endingFeature01[ENDING_DIC_BASIC_FEATURE_NUMBER + 1] = { 0, };//��������
	m_endingFeature02[ENDING_DIC_BASIC_FEATURE_NUMBER + 1] = { 0, };//��������
	m_endingFeature03[ENDING_DIC_BASIC_FEATURE_NUMBER + 1] = { 0, };//��������

	IS_CEHCK_FEATURE = '1';

	//���ڿ� ����
	m_p1thAddString = NULL;
	m_p2thAddString = NULL;
	m_p3thAddString = NULL;

	m_nRestorationJongChar = NULL;//N��° �������� ���� ���� ������
	m_nLen = 0;//���ڿ�����
	m_pLastOneChar = NULL;//S(1)�� ����
	m_pAddTwoString = NULL;
	m_pMinusTwoString = NULL;


}

CNAHangulAnaly::~CNAHangulAnaly()
{
	if (m_pConvert != NULL)
	{
		delete m_pConvert;
		m_pConvert = NULL;
	}
	if (m_pAmbiguity != NULL)
	{
		delete m_pAmbiguity;
		m_pAmbiguity = NULL;
	}

	CHAR_DELETE(m_pPrevOldBuffer);
	CHAR_DELETE(m_pOldBuffer);
	CHAR_DELETE(m_pNewBuffer);
	CHAR_DELETE(m_pRealOriginalString);
}


	
void CNAHangulAnaly::LoggingFunction(H_NODE *pNode,char* pCallFunc)
{
	#ifdef _DEBUG
		if(pNode != NULL){
			strcat(pNode->pHToken->strFlowChartDebug, pCallFunc);
		}
	#endif
}

//���¼Һм� �Լ��� ���Ǵ� ���� �ʱ�ȭ
void CNAHangulAnaly::IntitFunctionUsingValue()
{
	CHAR_DELETE(m_pFormLenToNWord);//S(len~n)
	CHAR_DELETE(m_pFormNTo1Word);//S(n~1)
	CHAR_DELETE(m_pFormNPluseNumTo1Word);//S(n+1~1)

	CHAR_DELETE(m_pStemDicWord);//���ۻ��� �׼��� �ܾ�
	CHAR_DELETE(m_pStemDicDeleteJongWord);//���ۻ��� �׼��� �ܾ�(�������ŵ� ����)
	CHAR_DELETE(m_pEndingDicWord);//�������� �׼��� �ܾ�
	CHAR_DELETE(m_pEndingDicDeleteJongWord);//�������� �׼��� �ܾ�(�������ŵ� ����)

	CHAR_DELETE(m_pOneNPluseNumChar);//n+1��°�� �ܾ�
	CHAR_DELETE(m_pOneNMinusNumChar);//n-1��°�� �ܾ�
	CHAR_DELETE(m_pNthOneChar);//N��° �ش� �ܾ�
	CHAR_DELETE(m_pJongChar);
	CHAR_DELETE(m_pChoJungChar);
	m_nPluseSyllable = 0;//nStart �������� +1(��->��) ��ġ�� �ش��ϴ� ������ ��
	m_nMinusSyllable = 0;//nStart �������� -1(��->��) ��ġ�� �ش��ϴ� ������ ��

	//��������/��������
	memset(m_stemFeature01, 0x00, sizeof(m_stemFeature01));
	memset(m_stemFeature02, 0x00, sizeof(m_stemFeature02));
	memset(m_stemFeature03, 0x00, sizeof(m_stemFeature03));

	memset(m_endingFeature01, 0x00, sizeof(m_endingFeature01));
	memset(m_endingFeature02, 0x00, sizeof(m_endingFeature02));
	memset(m_endingFeature03, 0x00, sizeof(m_endingFeature03));


	//���ڿ� ����
	CHAR_DELETE(m_p1thAddString);
	CHAR_DELETE(m_p2thAddString);
	CHAR_DELETE(m_p3thAddString);

	m_nRestorationJongChar =  -1;//N��° �������� ���� ���� ������
	m_nLen = 0;//���ڿ�����
	CHAR_DELETE(m_pLastOneChar);//S(1)�� ����
	CHAR_DELETE(m_pAddTwoString);
	CHAR_DELETE(m_pMinusTwoString);
	
}

int CNAHangulAnaly::GetWordFinalConsonant(char* pWord){
	int nHangulJa = 14;
	if(pWord != NULL)
	{
		if(strcmp(pWord,"��") == 0)  nHangulJa = REPLACE_HANGUL_JA00;
		else if (strcmp(pWord, "��") == 0) nHangulJa = REPLACE_HANGUL_JA01;
		else if (strcmp(pWord, "��") == 0) nHangulJa = REPLACE_HANGUL_JA02;
		else if (strcmp(pWord, "��") == 0) nHangulJa = REPLACE_HANGUL_JA03;
		else if (strcmp(pWord, "��") == 0) nHangulJa = REPLACE_HANGUL_JA04;
		else if (strcmp(pWord, "��") == 0) nHangulJa = REPLACE_HANGUL_JA05;
		else if (strcmp(pWord, "��") == 0) nHangulJa = REPLACE_HANGUL_JA06;
		else if (strcmp(pWord, "��") == 0) nHangulJa = REPLACE_HANGUL_JA07;
		else if (strcmp(pWord, "��") == 0) nHangulJa = REPLACE_HANGUL_JA08;
		else if (strcmp(pWord, "��") == 0) nHangulJa = REPLACE_HANGUL_JA09;
		else if (strcmp(pWord, "��") == 0) nHangulJa = REPLACE_HANGUL_JA10;
		else if (strcmp(pWord, "��") == 0) nHangulJa = REPLACE_HANGUL_JA11;
		else if (strcmp(pWord, "��") == 0) nHangulJa = REPLACE_HANGUL_JA12;
		else if (strcmp(pWord, "��") == 0) nHangulJa = REPLACE_HANGUL_JA13;
		else nHangulJa = REPLACE_HANGUL_JA14;
	}
	return nHangulJa;

}

char* CNAHangulAnaly::GetFromFirstToStartWord(char *pStr, int nMoveCount)
{
	char* pReturnStr = NULL;
	pReturnStr = new char[nMoveCount+1];
	memset(pReturnStr, 0x00, nMoveCount+1);
	for (int i = 0; i < nMoveCount; i++) {
		pReturnStr[i] = pStr[i];
	}
	return pReturnStr;
	/*
	char* pOriginalStr = NULL;
	char* pReturnStr = NULL;
	int nCount = 0;

	pReturnStr = new char[nMoveCount+1];
	memset(pReturnStr, 0x00, nMoveCount+1);

	pOriginalStr = pStr;

	while (nMoveCount)
	{
		pReturnStr[nCount] = *pOriginalStr;
		nCount += 1;
		pOriginalStr++;
		nMoveCount -= 1;
	}
	return pReturnStr;
	*/
}

char* CNAHangulAnaly::GetOneChar(char *pStr, int nPos)
{
	unsigned int IsKindCode = 0;
	int syl, high, low;
	char OneChar[3] = {0,};
	char* pReturnStr = NULL;
	
	if (pStr != NULL) {
		//*pStr+nPos
		if(nPos -1 > 0)
		{ 
			if(*(pStr+(nPos-1)) & 0x80)
			{
				OneChar[0] = pStr[nPos - 2];
				OneChar[1] = pStr[nPos - 1];
			}
			else
			{
				OneChar[0] = pStr[nPos - 1];
			}
		}
		else
		{	
			OneChar[0] = pStr[nPos-1];
		}
		
		syl = GetSylCode(OneChar);
		IsKindCode = GetHighLow(syl, &high, &low);

		switch (IsKindCode)
		{
			case HANGUL_CHAR_WORD:		// �ѱ�
			case CHINESE_CHAR_WORD:		// ����  
			case KATAGANA_CHAR_WORD:	// ��ī���� 
			case HIRAGANA_CHAR_WORD:	// ���󰡳�
			case SPECIAL_CHAR_WORD:		// Ư������
				pReturnStr = new char[3];
				memset(pReturnStr, 0x00, 3);
				pReturnStr[0] = pStr[nPos - 2];
				pReturnStr[1] = pStr[nPos - 1];
				break;
			default:
				pReturnStr = new char[2];
				memset(pReturnStr, 0x00, 2);
				pReturnStr[0] = pStr[nPos - 1];
				break;
		}
	}
	return pReturnStr;
}

char* CNAHangulAnaly::GetEndingPositionFromWord(char *pString, int nPos)
{
	char* pReturnCountChar = NULL;
	int nLen = 0;
	int nInxArr = 0;
	int nStartPos = 0;
	unsigned int IsKindCode = 0;
	int syl, high, low;
	char OneChar[3] = {0,};
	
	if (pString != NULL) {
		nLen = strlen(pString);
		pReturnCountChar = new char[nLen+1];
		memset(pReturnCountChar, 0x00, nLen+1);
		
		if (nPos - 1 > 0)
		{
			if (*(pString + (nPos - 1)) & 0x80)
			{
				OneChar[0] = pString[nPos - 2];
				OneChar[1] = pString[nPos - 1];
			}
			else
			{
				nStartPos = nPos - 1;//������ġ
				OneChar[0] = pString[nPos - 1];
			}
		}
		else
		{
			nStartPos = nPos - 1;//������ġ
			OneChar[0] = pString[nPos - 1];
		}
		
		syl = GetSylCode(OneChar);
		IsKindCode = GetHighLow(syl, &high, &low);
		switch (IsKindCode)
		{
			case HANGUL_CHAR_WORD:		// �ѱ�
			case CHINESE_CHAR_WORD:		// ����  
			case KATAGANA_CHAR_WORD:	// ��ī���� 
			case HIRAGANA_CHAR_WORD:	// ���󰡳�
			case SPECIAL_CHAR_WORD:		// Ư������
				nStartPos = nPos - 2;
				break;
			default:
				break;
		}
		for (int k = nStartPos; k < nLen; k++) {
			pReturnCountChar[nInxArr] = pString[k];
			nInxArr += 1;
		}
	}
	return pReturnCountChar;

	/*
	char* pReturnCountChar = NULL;
	char* pOriginalString = NULL;
	int nCount = 0;

	pOriginalString = pString;

	if (CalculateOneWordCount(pOriginalString, nPos))
		nPos -= 1;
	while (nPos > 1){
		pOriginalString++;
		nPos -= 1;
	}
	nLen = strlen(pOriginalString);
	pReturnCountChar = new char[nLen+1];
	memset(pReturnCountChar, 0x00, nLen+1);
	nCount = 0;
	while (nLen){
		pReturnCountChar[nCount] = pOriginalString[nCount];
		nCount += 1;
		nLen -= 1;
	}
	return pReturnCountChar;
	*/
	
}

char* CNAHangulAnaly::GetMiddleCountChar(char *pString, int nStartPos, int nEndPos, int nStartCase, int nEndCase)
{
	char* pOriginalString = NULL;
	char* pReturnString = NULL;

	bool isStart = false;
	bool isEnd = false;

	int nMiddleCount = 0;
	int nCount = 0;

	pOriginalString = pString;

	switch (nStartCase)
	{
	case CALCULATE_MINUS:
		isStart = CalculateWordCount(pOriginalString, nStartPos, 1, CALCULATE_MINUS);
		if (isStart)
		{
			if (IsThisWordHangulAscii(pOriginalString, nStartPos))
				nStartPos -= 3;
			else
				nStartPos -= 2;
		}
		else
		{
			if (IsThisWordHangulAscii(pOriginalString, nStartPos))
				nStartPos -= 3;
			else
				nStartPos -= 2;
		}
		break;
	case CALCULATE_PLUS:
		nStartPos += 1;
		break;
	case CALCULATE_STOP:
		isStart = CalculateOneWordCount(pOriginalString, nStartPos);
		if (isStart)
			nStartPos -= 1;
		break;
	}

	if (nEndPos > 0)
	{
		switch (nEndCase)
		{
		case CALCULATE_MINUS:
			isEnd = CalculateWordCount(pOriginalString, nEndPos, 1, CALCULATE_MINUS);
			if (isEnd)
			{
				if (IsThisWordHangulAscii(pOriginalString, nEndPos))
					nEndPos -= 2;
				else
					nEndPos -= 1;
			}
			else
			{
				if (IsThisWordHangulAscii(pOriginalString, nEndPos))
					nEndPos -= 2;
				else
					nEndPos -= 1;
			}
			break;
		case CALCULATE_PLUS:
			isStart = CalculateOneWordCount(pOriginalString, nStartPos);
			if (isStart)
			{
				if (IsThisWordHangulAscii(pOriginalString, nStartPos))
					nEndPos += 2;
				else
					nEndPos += 1;
			}
			else
			{
				if (IsThisWordHangulAscii(pOriginalString, nStartPos))
					nEndPos += 2;
				else
					nEndPos += 1;
			}
			break;
		case CALCULATE_STOP:
			break;
		}

		if (nEndPos > 0 && nStartPos > 0)
		{
			nMiddleCount = (nEndPos - nStartPos)+1;
			if (nMiddleCount == 1)
				nMiddleCount += 2;

			if (nMiddleCount > 0)
			{
				pReturnString = new char[nMiddleCount+1];
				memset(pReturnString, 0x00, nMiddleCount+1);

				while (nStartPos > 1)
				{
					pOriginalString++;
					nStartPos -= 1;
				}

				while (nMiddleCount)
				{
					pReturnString[nCount] = *pOriginalString;
					nMiddleCount -= 1;
					nCount += 1;
					pOriginalString++;
				}
			}
			else
			{
				pReturnString = new char[2];
				memset(pReturnString, 0x00, 2);
				strcpy(pReturnString, "");
			}
		}
		else
		{
			pReturnString = new char[2];
			memset(pReturnString, 0x00, 2);
			strcpy(pReturnString, "");
		}
	}
	else
	{
		int nTotalCount = 0;
		int nMove = 0;
		int nLen = 0;
		nTotalCount = strlen(pOriginalString);
		nMove = nTotalCount - nStartPos+1;
		nCount = 0;

		pReturnString = new char[nMove+1];
		memset(pReturnString, 0x00, nMove+1);
		while (nMove)
		{
			pReturnString[nCount] = pOriginalString[nStartPos - 1];
			nMove -= 1;
			nStartPos += 1;
			nCount += 1;
		}
	}
	return pReturnString;
}

char* CNAHangulAnaly::GetChojungJongsungChar(char *pOneChar, int nKind)
{
	if (pOneChar == NULL) return NULL;

	char* pOriginalChar = NULL;
	char* pReturnChar = NULL;
	int nTotalCount = 0;
	int nCount = 0;
	int nLen = 0;

	char OneChar[3];
	char chChoJung[3];
	char chJong[3];

	memset(chChoJung, 0x00, 3);
	memset(chJong, 0x00, 3);
	memset(OneChar, 0x00, 3);

	pOriginalChar = pOneChar;
	nTotalCount = strlen(pOriginalChar);

	while (nTotalCount)
	{
		OneChar[nCount] = *pOriginalChar;
		nTotalCount -= 1;
		nCount += 1;
		pOriginalChar++;
	}

	if (OneChar[0] & 0x80)
	{
		if (m_pConvert != NULL)
		{
			m_pConvert->EvalueLastChar(OneChar, chChoJung, chJong);
		}


		if (nKind == CHAR_CHOJUNGSUNG)
		{
			if (chChoJung != NULL && strlen(chChoJung) > 0)
			{
				nLen = strlen(chChoJung);
				nCount = 0;
				if (nLen > 0) {
					pReturnChar = new char[nLen+1];
					memset(pReturnChar, 0x00, nLen+1);
					while (nLen)
					{
						pReturnChar[nCount] = chChoJung[nCount];
						nLen -= 1;
						nCount += 1;
					}
				}
			}
		}

		if (nKind == CHAR_JONGSUNG)
		{
			nLen = strlen(chJong);
			nCount = 0;
			if (nLen > 0) {
				pReturnChar = new char[nLen+1];
				memset(pReturnChar, 0x00, nLen+1);
				while (nLen)
				{
					pReturnChar[nCount] = chJong[nCount];
					nLen -= 1;
					nCount += 1;
				}
			}
		}
	}
	else
	{
		if (nKind == CHAR_CHOJUNGSUNG)
		{
			pReturnChar = new char[2];
			memset(pReturnChar, 0x00, 2);
			strcpy(pReturnChar, "");
		}
		if (nKind == CHAR_JONGSUNG)
		{
			pReturnChar = new char[2];
			memset(pReturnChar, 0x00, 2);
			strcpy(pReturnChar, "");
		}
	}
	if (pReturnChar == NULL)
	{
		pReturnChar = new char[2];
		memset(pReturnChar, 0x00, 2);
		strcpy(pReturnChar, "");
	}
	return pReturnChar;
}

char* CNAHangulAnaly::MinusTwoStr(char *pOneString, char *pTwoString, int nPos)
{
	char* pRturnStirng = NULL;
	char* pChoJungChar = NULL;
	char* pJongChar = NULL;
	int nOneStringCount = 0;
	int nChoJungCount = 0;
	int nJongCount = 0;
	int nCount = 0;
	int nMove = 0;


	nOneStringCount = strlen(pOneString);

	if (pOneString[0] & 0x80)
	{
		if (strcmp(pOneString, pTwoString) != 0)
		{
			if (CalculateWordCount(pOneString, nPos, 1, CALCULATE_MINUS))
			{
				if (IsThisWordHangulAscii(pOneString, nPos))
					nOneStringCount -= 2;
				else
					nOneStringCount -= 1;
			}
			else
			{
				if (IsThisWordHangulAscii(pOneString, nPos))
					nOneStringCount -= 2;
				else
					nOneStringCount -= 1;
			}
			if (nOneStringCount > 0)
			{
				pChoJungChar = new char[3];
				memset(pChoJungChar, 0x00, 3);
				pJongChar = new char[3];
				memset(pJongChar, 0x00, 3);

				if (pTwoString[0] & 0x80)
				{
					if (m_pConvert != NULL)
						m_pConvert->EvalueLastChar(pTwoString, pChoJungChar, pJongChar);

					nChoJungCount = strlen(pChoJungChar);
					nJongCount = strlen(pJongChar);
				}

				pRturnStirng = new char[nOneStringCount+nChoJungCount+nJongCount+1];
				memset(pRturnStirng, 0x00, nOneStringCount+nChoJungCount+nJongCount+1);

				while (nOneStringCount)
				{
					pRturnStirng[nCount] = pOneString[nCount];
					nCount += 1;
					nOneStringCount -= 1;
				}
				while (nChoJungCount)
				{
					pRturnStirng[nCount] = pChoJungChar[nMove];
					nChoJungCount -= 1;
					nCount += 1;
					nMove += 1;
				}
			}
			else
			{
				pRturnStirng = new char[2];
				memset(pRturnStirng, 0x00, 2);
				strcpy(pRturnStirng, "");
			}
		}
		else
		{
			pChoJungChar = new char[3];
			memset(pChoJungChar, 0x00, 3);
			pJongChar = new char[3];
			memset(pJongChar, 0x00, 3);

			if (m_pConvert != NULL)
				m_pConvert->EvalueLastChar(pOneString, pChoJungChar, pJongChar);

			nChoJungCount = strlen(pChoJungChar);
			nCount = 0;

			pRturnStirng = new char[nChoJungCount+1];
			memset(pRturnStirng, 0x00, nChoJungCount+1);
			while (nChoJungCount)
			{
				pRturnStirng[nCount] = pChoJungChar[nCount];
				nChoJungCount -= 1;
				nCount += 1;
			}
		}
	}
	else
	{
		pRturnStirng = new char[2];
		memset(pRturnStirng, 0x00, 2);
		strcpy(pRturnStirng, "");
	}

	CHAR_DELETE(pChoJungChar);
	CHAR_DELETE(pJongChar);

	return pRturnStirng;
}

char* CNAHangulAnaly::AddTwoStr(char *pOneString, char *pTwoString)
{
	int nTotalLen = 0;
	int nCount = 0;
	int nOneCount = 0;
	int nTwoCount = 0;
	int nMove = 0;

	char* pAddTwoString = NULL;

	if (pOneString != NULL && pTwoString != NULL)
	{
		nTotalLen = strlen(pOneString);
		nTotalLen += strlen(pTwoString);
	}
	else if (pOneString != NULL && pTwoString == NULL)
	{
		nTotalLen = strlen(pOneString);
	}
	else if (pOneString == NULL && pTwoString != NULL)
	{
		nTotalLen = strlen(pTwoString);
	}
	else
	{
		nTotalLen = 0;
	}

	if (pOneString != NULL)
		nOneCount = strlen(pOneString);
	if (pTwoString != NULL)
		nTwoCount = strlen(pTwoString);

	pAddTwoString = new char[nTotalLen+1];
	memset(pAddTwoString, 0x00, nTotalLen+1);

	while (nOneCount > 0)
	{
		pAddTwoString[nCount] = pOneString[nCount];
		nOneCount -= 1;
		nCount += 1;
	}

	nMove = 0;
	if (pTwoString != NULL)
	{
		while (nTwoCount > 0)
		{
			pAddTwoString[nCount] = pTwoString[nMove];
			nTwoCount -= 1;
			nCount += 1;
			nMove += 1;
		}
	}
	return pAddTwoString;
}


char* CNAHangulAnaly::ReplaceString(char *pOneString, int nPos, int nCase)
{
	char pJongChar[3];
	char* pChoJungChar = NULL;
	char* pHapChar = NULL;


	memset(pJongChar, 0x00, 3);

	pChoJungChar = new char[3];
	memset(pChoJungChar, 0x00, 3);

	pHapChar = new char[3];
	memset(pHapChar, 0x00, 3);

	ReplaceHangulJA(pJongChar, nCase);

	pChoJungChar = GetChojungJongsungChar(pOneString, CHAR_CHOJUNGSUNG);


	if (pChoJungChar != NULL && pChoJungChar != "")
	{
		if (m_pConvert != NULL)
			m_pConvert->EvaluateHapChar(pHapChar, pChoJungChar, pJongChar);
	}
	else
		strcpy(pHapChar, "");

	CHAR_DELETE(pChoJungChar);

	return pHapChar;
}

bool CNAHangulAnaly::CalculateOneWordCount(char *pString, int nPos)
{
	bool isHangulCode = false;
	char* pMovingString = NULL;
	char OneChar[3];

	pMovingString = pString;

	while (nPos > 1)
	{
		nPos -= 1;
		pMovingString++;
	}
	memset(OneChar, 0x00, sizeof(OneChar));

	OneChar[0] = *pMovingString;

	if (OneChar[0] & 0x80)
		isHangulCode = true;
	else
		isHangulCode = false;

	return isHangulCode;
}

bool CNAHangulAnaly::CalculateWordCount(char *pString, int nCount, int nMove, int nCase)
{
	bool isOneCharCheck = false;
	bool isTwoCharCheck = false;
	bool isHangul = false;

	char* pMovingString = NULL;
	char OneChar[3];

	int nMovePos = 0;

	pMovingString = pString;

	while (nMove)
	{
		switch (nCase)
		{
		case CALCULATE_MINUS:
			nMovePos = nCount;
			while (nMovePos > 1)
			{
				nMovePos -= 1;
				pMovingString++;
			}
			memset(OneChar, 0x00, sizeof(OneChar));
			OneChar[0] = *pMovingString;
			if (OneChar[0] & 0x80)
				isOneCharCheck = true;
			else
				isOneCharCheck = false;

			if (isOneCharCheck == false)
			{
				nMovePos = nCount - 1;
				pMovingString = pString;
				while (nMovePos > 1)
				{
					nMovePos -= 1;
					pMovingString++;
				}
				memset(OneChar, 0x00, sizeof(OneChar));
				OneChar[0] = *pMovingString;
				if (OneChar[0] & 0x80)
					isTwoCharCheck = true;
				else
					isTwoCharCheck = false;

				if (isTwoCharCheck)
					isHangul = true;
				else
					isHangul = false;
			}
			else
			{
				nMovePos = nCount - 2;
				pMovingString = pString;
				while (nMovePos > 1)
				{
					nMovePos -= 1;
					pMovingString++;
				}
				memset(OneChar, 0x00, sizeof(OneChar));
				OneChar[0] = *pMovingString;
				if (OneChar[0] & 0x80)
					isTwoCharCheck = true;
				else
					isTwoCharCheck = false;

				if (isTwoCharCheck)
					isHangul = true;
				else
					isHangul = false;
			}
			break;
		case CALCULATE_PLUS:
			nMovePos = nCount+1;
			while (nMovePos > 1)
			{
				pMovingString++;
				nMovePos -= 1;
			}
			memset(OneChar, 0x00, sizeof(OneChar));
			OneChar[0] = *pMovingString;
			if (OneChar[0] & 0x80)
				isHangul = true;
			else
				isHangul = false;
			break;
		}
		nMove -= 1;
	}
	return isHangul;
}

bool CNAHangulAnaly::IsExistWildEndingDic(char *pString)
{
	bool isExist = false;
	isExist = m_pStemEndDic->EndFindWild(pString);
	return isExist;
}

bool CNAHangulAnaly::IsWildEndingDic(char *pEndingStr)
{
	bool isExsitDic = false;
	isExsitDic = m_pStemEndDic->EndFindWild(pEndingStr);
	return isExsitDic;
}

bool CNAHangulAnaly::IsExistEndingDic(char *pEnd)
{
	bool isExist = false;
	isExist = m_pStemEndDic->EndFindIdx(pEnd);
	return isExist;
}

bool CNAHangulAnaly::IsExistEndingDic(char *pEnd, char *pSaveBuffer)
{
	bool isExist = false;
	char Feature_End[ENDING_DIC_BASIC_FEATURE_NUMBER+1];
	char IS_CEHCK_FEATURE = '1';
	memset(Feature_End, 0x00, sizeof(Feature_End));
	isExist = m_pStemEndDic->EndGetRecord(pEnd, (char *)Feature_End);
	if (isExist == true)
	{
		memcpy((char*)pSaveBuffer, (char*)Feature_End, ENDING_DIC_BASIC_FEATURE_NUMBER);
	}
	return isExist;
}

bool CNAHangulAnaly::IsExistStemDic(char *pStem)
{
	bool isExist = false;
	isExist = m_pStemEndDic->StemFindIdx(pStem);
	return isExist;
}

bool CNAHangulAnaly::IsExistStemDic(char *pStem, char *pSaveBuffer)
{
	bool isExist = false;
	char Feature_Stem[STEM_DIC_ALL_FEATURE_NUMBER + 1];
	char IS_CEHCK_FEATURE = '1';
	memset(Feature_Stem, 0x00, sizeof(Feature_Stem));
	isExist = m_pStemEndDic->StemGetRecord(pStem, (char *)Feature_Stem);
	if (isExist == true)
	{
		for (int i = 0; i<STEM_DIC_ALL_FEATURE_NUMBER; i++)
		{
			if (Feature_Stem[i] == '1')
			{
				pSaveBuffer[i] = '1';
			}
		}
	}
	return isExist;
}

void CNAHangulAnaly::InsertEndingString(H_NODE *pNode, char *pEnding)
{
	int nLen = 0;
	int nCount = 0;
	if (pEnding != NULL && pEnding != "")
	{
		CHAR_DELETE(pNode->pHToken->subEnding->strResult);
		nLen = strlen(pEnding);
		pNode->pHToken->subEnding->strResult = new char[nLen+1];
		memset(pNode->pHToken->subEnding->strResult, 0x00, nLen+1);
		while (nLen)
		{
			pNode->pHToken->subEnding->strResult[nCount] = pEnding[nCount];
			nLen -= 1;
			nCount += 1;
		}
	}
	else
	{
		CHAR_DELETE(pNode->pHToken->subEnding->strResult);
	}
}

void CNAHangulAnaly::InsertAddSubTail(H_NODE *pNode, char *pString)
{
	H_SUBNODE* pInsertNode;
	pInsertNode = new H_SUBNODE;
	if (pInsertNode != NULL)
	{
		pInsertNode->pCSubToken = new COMP_SUB_TOKEN;
		if (pInsertNode->pCSubToken != NULL)
		{
			int nLen = strlen(pString);
			pInsertNode->pCSubToken->strResult = new char[nLen+1];
			memset(pInsertNode->pCSubToken->strResult, 0x00, nLen+1);
			strcpy(pInsertNode->pCSubToken->strResult, pString);

			ClassfyKindChar(pInsertNode, pString);

			pInsertNode->pCSubToken->strFeature = new char[STEM_DIC_ALL_FEATURE_NUMBER];
			memset(pInsertNode->pCSubToken->strFeature, 0x00, STEM_DIC_ALL_FEATURE_NUMBER);

			bool isExist = false;
			char Feature_Stem[STEM_DIC_ALL_FEATURE_NUMBER];
			char IS_CEHCK_FEATURE = '1';
			memset(Feature_Stem, 0x00, sizeof(Feature_Stem));

			if (m_pStemEndDic->StemFindIdx(pString) == true)
			{
				m_pStemEndDic->StemGetRecord(pString, (char *)Feature_Stem);
				for (int Feature = 0; Feature < STEM_DIC_ALL_FEATURE_NUMBER; Feature++)
				{
					if (Feature_Stem[Feature] == IS_CEHCK_FEATURE)
						pInsertNode->pCSubToken->strFeature[Feature] = '1';
					else
						pInsertNode->pCSubToken->strFeature[Feature] = '0';
				}
				if (pInsertNode->pCSubToken->strFeature[1] == '1')
				{
					pInsertNode->pCSubToken->strFeature[1] = '0';
					pInsertNode->pCSubToken->strFeature[2] = '1';
					pInsertNode->pCSubToken->strFeature[4] = '1';
				}
			}
			else
			{
				memset(pInsertNode->pCSubToken->strFeature, '0', STEM_DIC_ALL_FEATURE_NUMBER - 1);
			}

			pInsertNode->pPrev = pNode->pHToken->pSubTailNode->pPrev;
			pInsertNode->pNext = pNode->pHToken->pSubTailNode;
			pNode->pHToken->pSubTailNode->pPrev->pNext = pInsertNode;
			pNode->pHToken->pSubTailNode->pPrev = pInsertNode;
		}
	}
}

void CNAHangulAnaly::SubNodeUnity(H_NODE *pNode)
{
	H_SUBNODE* pDeleteNode = NULL;
	H_SUBNODE* pPosNode = NULL;
	char pCopy[1024];
	int nLen = 0;
	memset(pCopy, 0x00, 1024);
	if (pNode != NULL && pNode->pHToken != NULL && pNode->pHToken->pSubHeadNode != NULL)
	{
		pDeleteNode = pNode->pHToken->pSubHeadNode->pNext;
		while (pDeleteNode != pNode->pHToken->pSubTailNode)
		{
			pPosNode = pDeleteNode->pNext;
			if (pDeleteNode->pCSubToken->strResult != NULL)
			{
				strcat(pCopy, pDeleteNode->pCSubToken->strResult);
				delete[] pDeleteNode->pCSubToken->strResult;
			}
			if (pDeleteNode->pCSubToken->strFeature != NULL)
			{
				delete[] pDeleteNode->pCSubToken->strFeature;
			}
			if (pDeleteNode->pCSubToken != NULL)
			{
				delete pDeleteNode->pCSubToken;
			}
			if (pDeleteNode != NULL)
			{
				delete pDeleteNode;
			}
			pDeleteNode = pPosNode;
		}
		if (pNode->pHToken->pSubHeadNode != NULL)
		{
			delete pNode->pHToken->pSubHeadNode;
			pNode->pHToken->pSubHeadNode = NULL;
		}
		if (pNode->pHToken->pSubTailNode != NULL)
		{
			delete pNode->pHToken->pSubTailNode;
			pNode->pHToken->pSubTailNode = NULL;
		}

		pNode->pHToken->bSubCompCreate = false;

		nLen = strlen(pCopy)+1;
		if (nLen > 1)
		{
			CHAR_DELETE(pNode->pHToken->subStem->strResult);
			pNode->pHToken->subStem->strResult = new char[nLen];
			memset(pNode->pHToken->subStem->strResult, 0x00, nLen);
			strcpy(pNode->pHToken->subStem->strResult, pCopy);
			for (int i = 0; i<STEM_DIC_ALL_FEATURE_NUMBER; i++)
			{
				pNode->pHToken->subStem->strFeature[i] = '0';
			}

			ClassfyKindChar(pNode, pCopy);
		}
	}
}

H_SUBNODE* CNAHangulAnaly::SubNodeSectionUnity(H_NODE *pNode)
{
	H_SUBNODE* pRetureNode = NULL;
	H_SUBNODE* pDeleteNode = NULL;
	H_SUBNODE* pPosNode = NULL;
	char pCopy[1024];
	int nLen = 0;

	memset(pCopy, 0x00, 1024);

	if (pNode != NULL && pNode->pHToken != NULL && pNode->pHToken->pSubHeadNode != NULL)
	{
		pRetureNode = pNode->pHToken->pSubHeadNode->pNext;
		if (pRetureNode->pCSubToken->strResult != NULL)
		{
			strcat(pCopy, pRetureNode->pCSubToken->strResult);
		}
		pDeleteNode = pRetureNode->pNext;
		while (pDeleteNode != pNode->pHToken->pSubTailNode)
		{
			pPosNode = pDeleteNode->pNext;

			if (strcmp(pDeleteNode->pCSubToken->strResult, "��") == 0 || strcmp(pDeleteNode->pCSubToken->strResult, "��") == 0)
			{
				break;
			}
			if (pDeleteNode->pCSubToken->strResult != NULL)
			{
				strcat(pCopy, pDeleteNode->pCSubToken->strResult);
				delete[] pDeleteNode->pCSubToken->strResult;
				pDeleteNode->pCSubToken->strResult = NULL;
			}

			if (pDeleteNode->pCSubToken->strFeature != NULL)
			{
				delete[] pDeleteNode->pCSubToken->strFeature;
				pDeleteNode->pCSubToken->strFeature = NULL;
			}

			if (pDeleteNode->pCSubToken != NULL)
			{
				delete pDeleteNode->pCSubToken;
				pDeleteNode->pCSubToken = NULL;
			}
			//2017-08-15 Node ���� ���� �߰�
			if (pDeleteNode != NULL)
			{
				delete pDeleteNode;
				pDeleteNode = NULL;
			}
			pRetureNode->pNext = pPosNode;
			pPosNode->pPrev = pRetureNode;

			pDeleteNode = pPosNode;
		}

		nLen = strlen(pCopy)+1;
		if (nLen > 1)
		{
			if (pRetureNode->pCSubToken->strResult != NULL)
			{
				delete[] pRetureNode->pCSubToken->strResult;
				pRetureNode->pCSubToken->strResult = NULL;
			}
			pRetureNode->pCSubToken->strResult = new char[nLen];
			memset(pRetureNode->pCSubToken->strResult, 0x00, nLen);
			strcpy(pRetureNode->pCSubToken->strResult, pCopy);


			// ���� �ʱ�ȭ
			for (int i = 0; i<STEM_DIC_ALL_FEATURE_NUMBER; i++)
			{
				pRetureNode->pCSubToken->strFeature[i] = '0';
			}

			ClassfyKindChar(pRetureNode, pCopy);
		}
	}

	return pRetureNode;
}

void CNAHangulAnaly::InsertStemString(H_NODE *pNode, char *pStem)
{
	int nLen = 0;
	int nCount = 0;

	if (pStem != NULL && pStem != "")
	{
		CHAR_DELETE(pNode->pHToken->subStem->strResult);

		nLen = strlen(pStem);
		pNode->pHToken->subStem->strResult = new char[nLen+1];
		memset(pNode->pHToken->subStem->strResult, 0x00, nLen+1);

		while (nLen)
		{
			pNode->pHToken->subStem->strResult[nCount] = pStem[nCount];
			nLen -= 1;
			nCount += 1;
		}
		ClassfyKindChar(pNode, pStem);
	}
	else
	{
		CHAR_DELETE(pNode->pHToken->subStem->strResult);
	}
}

void CNAHangulAnaly::ReplaceHangulJongsungNull(H_NODE *pNode, int nPos)
{
	char* pOriginalString = NULL;
	char* pOneChar = NULL;
	char* pChoJungChar = NULL;
	char* pJongChar = NULL;

	int nCount = 0;
	int nLen = 0;
	int nMovePos = 0;

	nLen = strlen(pNode->pHToken->strSource);
	pOriginalString = new char[nLen+1];
	memset(pOriginalString, 0x00, nLen+1);

	pOneChar = GetOneChar(pNode->pHToken->strSource, nPos);

	if (CalculateWordCount(pNode->pHToken->strSource, nPos, 1, CALCULATE_MINUS))
	{

		if (IsThisWordHangulAscii(pNode->pHToken->strSource, nPos))
			nPos -= 1;

		nCount = 0;

		while (nLen)
		{
			pOriginalString[nCount] = pNode->pHToken->strSource[nCount];
			nLen -= 1;
			nCount += 1;
		}

		nCount = 0;
		nMovePos = 1;

		nLen = strlen(pNode->pHToken->strSource);

		nLen += 2;

		if (pNode->pHToken->strSource != NULL && strlen(pNode->pHToken->strSource) > 0)
		{
			delete[]pNode->pHToken->strSource;
			pNode->pHToken->strSource = NULL;
		}

		pNode->pHToken->strSource = new char[nLen+1];
		memset(pNode->pHToken->strSource, 0x00, nLen+1);


		pChoJungChar = GetChojungJongsungChar(pOneChar, CHAR_CHOJUNGSUNG);


		// ���߼��� ������
		if (pChoJungChar != NULL && strlen(pChoJungChar) > 0)
		{
			while (nLen)
			{
				if (nMovePos != nPos)
				{
					pNode->pHToken->strSource[nCount] = pOriginalString[nCount];
					nCount += 1;
					nLen -= 1;
					nMovePos += 1;
				}
				else
				{
					pNode->pHToken->strSource[nCount] = pChoJungChar[0];
					nCount += 1;
					nLen -= 1;
					nMovePos += 1;
					pNode->pHToken->strSource[nCount] = pChoJungChar[1];
					nCount += 1;
					nLen -= 1;
					nMovePos += 1;
				}
			}
		}
		else
		{
			nLen = strlen(pOriginalString);
			for (nCount = 0; nCount < nLen; nCount++)
			{
				pNode->pHToken->strSource[nCount] = pOriginalString[nCount];
			}
		}
	}
	else
	{
		strcpy(pOriginalString, pNode->pHToken->strSource);
		if (pNode->pHToken->strSource != NULL && strlen(pNode->pHToken->strSource) > 0)
		{
			delete[]pNode->pHToken->strSource;
			pNode->pHToken->strSource = NULL;
		}

		pNode->pHToken->strSource = new char[nLen+1];
		memset(pNode->pHToken->strSource, 0x00, nLen+1);

		pChoJungChar = GetChojungJongsungChar(pOneChar, CHAR_CHOJUNGSUNG);


		nMovePos = 1;
		nCount = 0;
		while (nLen >0)
		{
			if (nMovePos < nPos)
			{
				pNode->pHToken->strSource[nCount] = pOriginalString[nCount];
				nCount += 1;
				nLen -= 1;
				nMovePos += 1;
				if (nMovePos+1 == nPos)
					nMovePos += 1;
			}
			else
			{
				if (pChoJungChar != NULL && strlen(pChoJungChar) > 0)
				{
					pNode->pHToken->strSource[nCount] = pChoJungChar[0];
					nCount += 1;
					nLen -= 1;
					nMovePos += 1;
					pNode->pHToken->strSource[nCount] = pChoJungChar[1];
					nCount += 1;
					nLen -= 1;
					nMovePos += 1;
					CHAR_DELETE(pChoJungChar);
				}
				else
				{
					pNode->pHToken->strSource[nCount] = pOriginalString[nCount];
					nCount += 1;
					nLen -= 1;
					nMovePos += 1;
					pNode->pHToken->strSource[nCount] = pOriginalString[nCount];
					nCount += 1;
					nLen -= 1;
					nMovePos += 1;
				}
			}
		}
	}

	CHAR_DELETE(pOriginalString);
	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pChoJungChar);
	CHAR_DELETE(pJongChar);
}

bool CNAHangulAnaly::IsExistStrReplaceJaso(H_NODE *pNode, int nPos, int nWord, char* pStr)
{
	bool isExist = false;

	char* pOriginalString = NULL;
	char* pOneWord = NULL;
	char* pOneChar = NULL;
	char *pChoJungChar = NULL;

	char pJongChar[3];
	char pHapChar[3];

	int nSavePos = 0;
	int nCount = 0;
	int nLen = 0;
	int nMovePos = 0;

	nSavePos = nPos;

	memset(pJongChar, 0x00, 3);
	memset(pHapChar, 0x00, 3);

	nLen = strlen(pNode->pHToken->strSource);
	pOriginalString = new char[nLen+1];
	memset(pOriginalString, 0x00, nLen+1);
	nCount = 0;
	while (nLen)
	{
		pOriginalString[nCount] = pNode->pHToken->strSource[nCount];
		nLen -= 1;
		nCount += 1;
	}

	pOneWord = GetOneChar(pOriginalString, nPos);

	ReplaceHangulJA(pJongChar, nWord);

	pChoJungChar = GetChojungJongsungChar(pOneWord, CHAR_CHOJUNGSUNG);
	if (pChoJungChar != NULL && pChoJungChar != "")
	{
		if (m_pConvert != NULL)
			m_pConvert->EvaluateHapChar(pHapChar, pChoJungChar, pJongChar);

		nPos = GetNextPrevPositionWord(pOriginalString, nPos, 1, CALCULATE_MINUS);

		nLen = strlen(pOriginalString);
		nCount = 0;
		if (pNode->pHToken->strSource != NULL && strlen(pNode->pHToken->strSource) > 0)
		{
			delete[]pNode->pHToken->strSource;
			pNode->pHToken->strSource = NULL;
		}

		pNode->pHToken->strSource = new char[nLen+1];
		memset(pNode->pHToken->strSource, 0x00, nLen+1);

		nMovePos = 0;

		while (nLen)
		{
			if (nMovePos != nPos)
			{
				pNode->pHToken->strSource[nCount] = pOriginalString[nCount];
				nLen -= 1;
				nCount += 1;
				nMovePos += 1;
			}
			else
			{
				pNode->pHToken->strSource[nCount] = pHapChar[0];
				nCount += 1;
				pNode->pHToken->strSource[nCount] = pHapChar[1];
				nCount += 1;
				nMovePos += 2;
				nLen -= 2;
			}
		}
	}


	CHAR_DELETE(pOriginalString);
	pOriginalString = pNode->pHToken->strSource;

	pOneChar = GetOneChar(pOriginalString, nSavePos);

	CHAR_DELETE(pChoJungChar);
	pChoJungChar = GetChojungJongsungChar(pOneChar, CHAR_CHOJUNGSUNG);

	if (strstr(pStr, pChoJungChar) != NULL)
	{
		isExist = true;
	}
	else
	{
		isExist = false;
	}

	CHAR_DELETE(pOneWord);
	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pChoJungChar);

	return isExist;
}


bool CNAHangulAnaly::IsExisteTableReplaceJaso(H_NODE *pNode, int nPos, int nCase, int nWord)
{
	bool isExist = false;

	char* pOriginalString = NULL;
	char* pOneWord = NULL;
	char* pOneChar = NULL;
	char *pChoJungChar = NULL;

	char pJongChar[3];
	char pHapChar[3];

	int nSavePos = 0;
	int nCount = 0;
	int nLen = 0;
	int nMovePos = 0;

	nSavePos = nPos;

	memset(pJongChar, 0x00, 3);
	memset(pHapChar, 0x00, 3);

	nLen = strlen(pNode->pHToken->strSource);
	pOriginalString = new char[nLen+1];
	memset(pOriginalString, 0x00, nLen+1);
	nCount = 0;
	while (nLen)
	{
		pOriginalString[nCount] = pNode->pHToken->strSource[nCount];
		nLen -= 1;
		nCount += 1;
	}

	pOneWord = GetOneChar(pOriginalString, nPos);

	ReplaceHangulJA(pJongChar, nWord);

	switch (nCase)
	{
	case FORCE_REPLACE_ONEWORD:
		break;
	case FORCE_REPLACE_ENDWORD:
		pChoJungChar = GetChojungJongsungChar(pOneWord, CHAR_CHOJUNGSUNG);
		if (pChoJungChar != NULL && pChoJungChar != "")
		{
			if (m_pConvert != NULL)
				m_pConvert->EvaluateHapChar(pHapChar, pChoJungChar, pJongChar);

			nPos = GetNextPrevPositionWord(pOriginalString, nPos, 1, CALCULATE_MINUS);

			nLen = strlen(pOriginalString);
			nCount = 0;
			if (pNode->pHToken->strSource != NULL && strlen(pNode->pHToken->strSource) > 0)
			{
				delete[]pNode->pHToken->strSource;
				pNode->pHToken->strSource = NULL;
			}

			pNode->pHToken->strSource = new char[nLen+1];
			memset(pNode->pHToken->strSource, 0x00, nLen+1);

			nMovePos = 0;

			while (nLen)
			{
				if (nMovePos != nPos)
				{
					pNode->pHToken->strSource[nCount] = pOriginalString[nCount];
					nLen -= 1;
					nCount += 1;
					nMovePos += 1;
				}
				else
				{
					pNode->pHToken->strSource[nCount] = pHapChar[0];
					nCount += 1;
					pNode->pHToken->strSource[nCount] = pHapChar[1];
					nCount += 1;
					nMovePos += 2;
					nLen -= 2;
				}
			}
		}
		break;
	}

	CHAR_DELETE(pOriginalString);

	pOriginalString = pNode->pHToken->strSource;

	pOneChar = GetOneChar(pOriginalString, nSavePos);

	CHAR_DELETE(pChoJungChar);

	pChoJungChar = GetChojungJongsungChar(pOneChar, CHAR_CHOJUNGSUNG);

	if (CompareVariousTable01(pChoJungChar, 0, false) == true ||
		CompareVariousTable02(pChoJungChar, 1, false) == true ||
		CompareVariousTable02(pChoJungChar, 2, false) == true ||
		CompareVariousTable03(pChoJungChar, 0, false) == true ||
		CompareVariousTable03(pChoJungChar, 1, false) == true ||
		CompareVariousTable04(pChoJungChar, 1, false) == true ||
		CompareVariousTable04(pChoJungChar, 2, false) == true ||
		CompareVariousTable04(pChoJungChar, 3, false) == true ||
		CompareVariousTable05(pChoJungChar, ALL_TABLE_05, true) == true ||
		CompareVariousTable06(pChoJungChar, ALL_TABLE_06, true) == true ||
		CompareVariousTable07(pChoJungChar, ALL_TABLE_07, true) == true)
	{
		isExist = true;
	}
	else
	{
		isExist = false;
	}

	CHAR_DELETE(pOneWord);
	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pChoJungChar);

	return isExist;
}

bool CNAHangulAnaly::ReplaceHgngulTokenSource(H_NODE *pNode, int nPos, int nCase, int nWord)
{
	bool isExist = false;

	char* pOriginalString = NULL;
	char* pCountChar = NULL;
	char* pOneChar = NULL;
	char* pChoJungChar = NULL;

	char pJongChar[3];
	char pHapChar[3];

	int nSavePos = 0;
	int nCount = 0;
	int nLen = 0;
	int nMovePos = 0;

	nSavePos = nPos;

	memset(pJongChar, 0x00, 3);
	memset(pHapChar, 0x00, 3);

	nLen = strlen(pNode->pHToken->strSource);
	pOriginalString = new char[nLen+1];
	memset(pOriginalString, 0x00, nLen+1);

	nCount = 0;
	while (nLen)
	{
		pOriginalString[nCount] = pNode->pHToken->strSource[nCount];
		nLen -= 1;
		nCount += 1;
	}

	pOneChar = GetOneChar(pOriginalString, nPos);

	ReplaceHangulJA(pJongChar, nWord);

	switch (nCase)
	{
	case FORCE_REPLACE_ONEWORD:
		break;
	case FORCE_REPLACE_ENDWORD:
		pChoJungChar = GetChojungJongsungChar(pOneChar, CHAR_CHOJUNGSUNG);
		if (pChoJungChar != NULL && pChoJungChar != "" && strlen(pChoJungChar) > 0)
		{
			if (m_pConvert != NULL)
				m_pConvert->EvaluateHapChar(pHapChar, pChoJungChar, pJongChar);
			if (pHapChar != NULL && strlen(pHapChar) > 0)
			{
				if (CalculateWordCount(pOriginalString, nPos, 1, CALCULATE_MINUS))
				{
					if (IsThisWordHangulAscii(pOriginalString, nPos))
						nPos -= 1;
				}
				else
				{
					if (IsThisWordHangulAscii(pOriginalString, nPos))
						nPos -= 1;
				}

				nLen = strlen(pOriginalString);
				nCount = 0;

				CHAR_DELETE(pNode->pHToken->strSource);

				pNode->pHToken->strSource = new char[nLen+1];
				memset(pNode->pHToken->strSource, 0x00, nLen+1);

				nMovePos = 1;

				while (nLen)
				{
					if (nMovePos != nPos)
					{
						pNode->pHToken->strSource[nCount] = pOriginalString[nCount];
						nLen -= 1;
						nCount += 1;
						nMovePos += 1;
					}
					else
					{
						pNode->pHToken->strSource[nCount] = pHapChar[0];
						nCount += 1;
						pNode->pHToken->strSource[nCount] = pHapChar[1];
						nCount += 1;
						nMovePos += 2;
						nLen -= 2;
					}
				}
			}
		}
		break;
	}

	CHAR_DELETE(pOriginalString);

	pOriginalString = pNode->pHToken->strSource;

	pCountChar = GetFromFirstToStartWord(pOriginalString, nSavePos);

	if (IsExistStemDic(pCountChar))
		isExist = true;
	else
		isExist = false;

	CHAR_DELETE(pCountChar);
	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pChoJungChar);

	return isExist;
}

bool CNAHangulAnaly::DeleteHgngulTokenSource(H_NODE *pNode, int nPos, int nCase)
{
	bool isExist = false;
	char* pOriginalString = NULL;
	char* pOneChar = NULL;
	char* pChoJungChar = NULL;
	char* pStartCountChar = NULL;

	int nSavePos = 0;
	int nCount = 0;
	int nLen = 0;
	int nMovePos = 0;

	nSavePos = nPos;

	nLen = strlen(pNode->pHToken->strSource);

	pOriginalString = new char[nLen+1];
	memset(pOriginalString, 0x00, nLen+1);
	nCount = 0;
	while (nLen)
	{
		pOriginalString[nCount] = pNode->pHToken->strSource[nCount];
		nLen -= 1;
		nCount += 1;
	}

	pOneChar = GetOneChar(pOriginalString, nPos);

	switch (nCase)
	{
	case FORCE_REPLACE_ONEWORD:
		break;
	case FORCE_REPLACE_ENDWORD:
		pChoJungChar = GetChojungJongsungChar(pOneChar, CHAR_CHOJUNGSUNG);
		if (pChoJungChar != NULL && pChoJungChar != "")
		{
			if (CalculateWordCount(pOriginalString, nPos, 1, CALCULATE_MINUS))
			{
				if (IsThisWordHangulAscii(pOriginalString, nPos))
					nPos -= 1;
			}
			else
			{
				if (IsThisWordHangulAscii(pOriginalString, nPos))
					nPos -= 1;
			}

			nLen = strlen(pOriginalString);
			nCount = 0;
			if (pNode->pHToken->strSource != NULL && strlen(pNode->pHToken->strSource) > 0)
			{
				delete[]pNode->pHToken->strSource;
				pNode->pHToken->strSource = NULL;
			}

			pNode->pHToken->strSource = new char[nLen+1];
			memset(pNode->pHToken->strSource, 0x00, nLen+1);

			nMovePos = 1;

			while (nLen)
			{
				if (nMovePos != nPos)
				{
					pNode->pHToken->strSource[nCount] = pOriginalString[nCount];
					nLen -= 1;
					nCount += 1;
					nMovePos += 1;
				}
				else
				{
					pNode->pHToken->strSource[nCount] = pChoJungChar[0];
					nCount += 1;
					pNode->pHToken->strSource[nCount] = pChoJungChar[1];
					nCount += 1;
					nMovePos += 2;
					nLen -= 2;
				}
			}
		}
		break;
	}

	CHAR_DELETE(pOriginalString);
	pOriginalString = pNode->pHToken->strSource;
	pStartCountChar = GetFromFirstToStartWord(pOriginalString, nSavePos);

	if (IsExistStemDic(pStartCountChar))
		isExist = true;
	else
		isExist = false;

	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pChoJungChar);
	CHAR_DELETE(pStartCountChar);

	return isExist;
}

void CNAHangulAnaly::AddHTokenEnding(H_NODE *pEndingNode, int nCase)
{
	if (pEndingNode->pHToken->subEnding->strFeature != NULL)
		pEndingNode->pHToken->subEnding->strFeature[nCase] = '1';
}

void CNAHangulAnaly::DeleteHTokenEnding(H_NODE *pEndingNode, int nCase)
{
	if (pEndingNode->pHToken->subEnding->strFeature != NULL)
		pEndingNode->pHToken->subEnding->strFeature[nCase] = '0';
}

void CNAHangulAnaly::SaveHTokenEnding(H_NODE *pEndingNode)
{
	char* pEnding = NULL;
	pEnding = pEndingNode->pHToken->subEnding->strResult;
	CHAR_DELETE(pEndingNode->pHToken->subEnding->strFeature);
	pEndingNode->pHToken->subEnding->strFeature = new char[ENDING_DIC_BASIC_FEATURE_NUMBER];
	memset(pEndingNode->pHToken->subEnding->strFeature, 0x00, ENDING_DIC_BASIC_FEATURE_NUMBER);
	for (int Feature = 0; Feature < ENDING_DIC_BASIC_FEATURE_NUMBER; Feature++)
	{
		pEndingNode->pHToken->subEnding->strFeature[Feature] = '0';
	}
	IsExistEndingDic(pEnding, pEndingNode->pHToken->subEnding->strFeature);
}

void CNAHangulAnaly::AddHTokenStem(H_NODE *pStemNode, int nCase)
{
	if (pStemNode->pHToken->subStem->strFeature != NULL)
		pStemNode->pHToken->subStem->strFeature[nCase] = '1';
}

void CNAHangulAnaly::DeleteHTokenStem(H_NODE *pStemNode, int nCase)
{
	if (pStemNode->pHToken->subStem->strFeature != NULL)
		pStemNode->pHToken->subStem->strFeature[nCase] = '0';
}

void CNAHangulAnaly::SaveHTokenStem(H_NODE *pStemNode)
{
	char* pStem = NULL;
	pStem = pStemNode->pHToken->subStem->strResult;
	if (pStemNode != NULL) {
		CHAR_DELETE(pStemNode->pHToken->subStem->strFeature);
		pStemNode->pHToken->subStem->strFeature = new char[STEM_DIC_ALL_FEATURE_NUMBER];
		memset(pStemNode->pHToken->subStem->strFeature, 0x00, STEM_DIC_ALL_FEATURE_NUMBER);
		for (int Feature = 0; Feature < STEM_DIC_ALL_FEATURE_NUMBER; Feature++)
		{
			pStemNode->pHToken->subStem->strFeature[Feature] = '0';
		}
		IsExistStemDic(pStem, pStemNode->pHToken->subStem->strFeature);
	}
}

void CNAHangulAnaly::SaveHTokenStemFeature(H_NODE *pStemNode, char *pStem)
{
	if (pStemNode != NULL && IsExistStemDic(pStem) == true)
	{
		if (pStemNode->pHToken->subStem->strFeature == NULL)
		{
			pStemNode->pHToken->subStem->strFeature = new char[STEM_DIC_ALL_FEATURE_NUMBER];
			memset(pStemNode->pHToken->subStem->strFeature, 0x00, STEM_DIC_ALL_FEATURE_NUMBER);
			for (int Feature = 0; Feature < STEM_DIC_ALL_FEATURE_NUMBER; Feature++)
			{
				pStemNode->pHToken->subStem->strFeature[Feature] = '0';
			}
		}
		if (IsExistStemDic(pStem, pStemNode->pHToken->subStem->strFeature) == false)
		{
		}
	}
}

bool CNAHangulAnaly::IsExistFeature(char *pString, int nKindDic, int nFeature)
{
	bool isExist = false;
	char Feature_Stem[STEM_DIC_ALL_FEATURE_NUMBER];
	char Feature_End[ENDING_DIC_BASIC_FEATURE_NUMBER];
	int FindIndex = -1;
	char IS_CHECK_FEATURE = '1';

	switch (nKindDic)
	{
	case STEM_DIC:
		memset(Feature_Stem, 0x00, sizeof(Feature_Stem));
		if (IsExistStemDic(pString, (char *)Feature_Stem) == true)
		{
			if (Feature_Stem[nFeature] == IS_CHECK_FEATURE)
				isExist = true;
		}
		break;
	case END_DIC:
		memset(Feature_End, 0x00, sizeof(Feature_End));
		if (IsExistEndingDic(pString, (char *)Feature_End) == true)
		{
			if (Feature_End[nFeature] == IS_CHECK_FEATURE)
				isExist = true;
		}
		break;
	}
	return isExist;
}

int CNAHangulAnaly::ReturnNumberMovingLeftPositionWord(char *pString, int &nCount)
{
	int nReturnNumber = 0;
	char OneChar[3] =  {0,};
	if (nCount - 1 > 0)
	{
		if (*(pString + (nCount - 1)) & 0x80)
		{
			nCount -= 2;
		}
		else
		{
			nCount -= 1;
		}
	}
	else
	{
		nCount -= 1;
	}
	nReturnNumber = nCount;
	InitOneWordMarsk();
	return nReturnNumber;
	/*
	int nLeftMovePos = 0;
	nLeftMovePos = nCount;
	while (nLeftMovePos > 1)
	{
		pString++;
		nLeftMovePos -= 1;
	}
	memset(OneChar, 0x00, sizeof(OneChar));
	OneChar[0] = *pString;
	if (OneChar[0] & 0x80)
		nCount -= 2;
	else
		nCount -= 1;
	nReturnNumber = nCount;
	InitOneWordMarsk();
	return nReturnNumber;
	*/
}

int CNAHangulAnaly::Restoration(char *pJongChar)
{
	int nReturnValue = 0;

	if (strcmp(pJongChar, "��") == 0)
		nReturnValue = REPLACE_HANGUL_JA00;
	if (strcmp(pJongChar, "��") == 0)
		nReturnValue = REPLACE_HANGUL_JA01;
	if (strcmp(pJongChar, "��") == 0)
		nReturnValue = REPLACE_HANGUL_JA02;
	if (strcmp(pJongChar, "��") == 0)
		nReturnValue = REPLACE_HANGUL_JA03;
	if (strcmp(pJongChar, "��") == 0)
		nReturnValue = REPLACE_HANGUL_JA04;
	if (strcmp(pJongChar, "��") == 0)
		nReturnValue = REPLACE_HANGUL_JA05;
	if (strcmp(pJongChar, "��") == 0)
		nReturnValue = REPLACE_HANGUL_JA06;
	if (strcmp(pJongChar, "��") == 0)
		nReturnValue = REPLACE_HANGUL_JA07;
	if (strcmp(pJongChar, "��") == 0)
		nReturnValue = REPLACE_HANGUL_JA08;
	if (strcmp(pJongChar, "��") == 0)
		nReturnValue = REPLACE_HANGUL_JA09;
	if (strcmp(pJongChar, "��") == 0)
		nReturnValue = REPLACE_HANGUL_JA10;
	if (strcmp(pJongChar, "��") == 0)
		nReturnValue = REPLACE_HANGUL_JA11;
	if (strcmp(pJongChar, "��") == 0)
		nReturnValue = REPLACE_HANGUL_JA12;
	if (strcmp(pJongChar, "��") == 0)
		nReturnValue = REPLACE_HANGUL_JA13;
	if (strcmp(pJongChar, "") == 0)
		nReturnValue = REPLACE_HANGUL_JA14;

	return nReturnValue;
}

bool CNAHangulAnaly::IsSameWordPos(H_NODE *pNode, int lStart)
{
	char* pOriginalString = NULL;
	char* pBufferOneChar = NULL;
	char* pOneChar = NULL;
	bool isSameWord = false;
	int nCount = 0;
	int nLen = 0;

	pOriginalString = pNode->pHToken->strSource;

	pOneChar = GetOneChar(pOriginalString, lStart);

	if (m_pNewBuffer[0] & 0x80)
		nLen = 2;
	else
		nLen = 1;

	pBufferOneChar = new char[nLen+1];
	memset(pBufferOneChar, 0x00, nLen+1);
	while (nLen)
	{
		pBufferOneChar[nCount] = m_pNewBuffer[nCount];
		nLen -= 1;
		nCount += 1;
	}

	if (strcmp(pOneChar, pBufferOneChar) == 0)
		isSameWord = true;
	else
		isSameWord = false;

	CHAR_DELETE(pBufferOneChar);
	CHAR_DELETE(pOneChar);

	return isSameWord;
}

void CNAHangulAnaly::ChangePosAnyStartPosition(H_NODE *pNode, int &nStart, int &mStart, int &lStart, int nCase)
{
	int nTotalLen = 0;
	if (m_pOldBuffer != NULL)
	{
		if (m_pOldBuffer[0] & 0x80)
			nTotalLen = nStart+2;
		else
			nTotalLen = nStart+1;

		switch (nCase)
		{
		case CHANGE_FIRST_START:
			nStart = nTotalLen;
			break;
		case CHANGE_SECOND_START:
			mStart = nTotalLen;
			break;
		case CHANGE_THIRD_START:
			lStart = nTotalLen;
			break;
		}
	}
}

bool CNAHangulAnaly::IsThisWordHangulAscii(char *pThisChar, int nPos)
{
	char OneChar[3];
	char* pOriginalStr = NULL;
	char* pIsHangulCode = NULL;
	bool bIsWordState = false;

	int nPosition = 0;

	pOriginalStr = pThisChar;
	pIsHangulCode = pThisChar;
	nPosition = nPos;

	while (nPosition > 1)
	{
		pIsHangulCode++;
		nPosition -= 1;
	}

	memset(OneChar, 0x00, sizeof(OneChar));
	OneChar[0] = *pIsHangulCode;
	if (OneChar[0] & 0x80)
	{
		nPos -= 1;
		nPosition = nPos;
	}
	else
	{
		nPosition = nPos;
	}

	while (nPosition > 1)
	{
		pOriginalStr++;
		nPosition -= 1;
	}

	memset(OneChar, 0x00, sizeof(OneChar));
	OneChar[0] = *pOriginalStr;

	if (OneChar[0] & 0x80)
		bIsWordState = true;
	else
		bIsWordState = false;

	return bIsWordState;
}

void CNAHangulAnaly::ReplaceHangulJA(char* pStr, int nWord)
{
	switch (nWord)
	{
	case REPLACE_HANGUL_JA00:
		strcpy(pStr, "��");
		break;
	case REPLACE_HANGUL_JA01:
		strcpy(pStr, "��");
		break;
	case REPLACE_HANGUL_JA02:
		strcpy(pStr, "��");
		break;
	case REPLACE_HANGUL_JA03:
		strcpy(pStr, "��");
		break;
	case REPLACE_HANGUL_JA04:
		strcpy(pStr, "��");
		break;
	case REPLACE_HANGUL_JA05:
		strcpy(pStr, "��");
		break;
	case REPLACE_HANGUL_JA06:
		strcpy(pStr, "��");
		break;
	case REPLACE_HANGUL_JA07:
		strcpy(pStr, "��");
		break;
	case REPLACE_HANGUL_JA08:
		strcpy(pStr, "��");
		break;
	case REPLACE_HANGUL_JA09:
		strcpy(pStr, "��");
		break;
	case REPLACE_HANGUL_JA10:
		strcpy(pStr, "��");
		break;
	case REPLACE_HANGUL_JA11:
		strcpy(pStr, "��");
		break;
	case REPLACE_HANGUL_JA12:
		strcpy(pStr, "��");
		break;
	case REPLACE_HANGUL_JA13:
		strcpy(pStr, "��");
		break;
	default://��������
		strcpy(pStr, "");
		break;
	}
}

void CNAHangulAnaly::JasoRestoration(H_NODE *pNode, int nPos, int nCase, int nWord)
{
	bool isExist = false;

	char* pOriginalString = NULL;
	char* pOneChar = NULL;
	char* pChoJungChar = NULL;

	char pJongChar[3];
	char pHapChar[3];

	int nSavePos = 0;
	int nCount = 0;
	int nLen = 0;
	int nMovePos = 0;

	nSavePos = nPos;

	memset(pJongChar, 0x00, 3);
	memset(pHapChar, 0x00, 3);

	nLen = strlen(pNode->pHToken->strSource);
	pOriginalString = new char[nLen+1];
	memset(pOriginalString, 0x00, nLen+1);

	nCount = 0;

	while (nLen)
	{
		pOriginalString[nCount] = pNode->pHToken->strSource[nCount];
		nLen -= 1;
		nCount += 1;
	}

	pOneChar = GetOneChar(pOriginalString, nPos);

	ReplaceHangulJA(pJongChar, nWord);

	switch (nCase)
	{
	case FORCE_REPLACE_ONEWORD:
		break;
	case FORCE_REPLACE_ENDWORD:
		pChoJungChar = GetChojungJongsungChar(pOneChar, CHAR_CHOJUNGSUNG);
		if (pChoJungChar != NULL && pChoJungChar != "")
		{
			if (m_pConvert != NULL)
				m_pConvert->EvaluateHapChar(pHapChar, pChoJungChar, pJongChar);

			if (CalculateWordCount(pOriginalString, nPos, 1, CALCULATE_MINUS))
				nPos -= 1;

			nLen = strlen(pOriginalString);
			nCount = 0;
			if (pNode->pHToken->strSource != NULL && strlen(pNode->pHToken->strSource) > 0)
			{
				delete[]pNode->pHToken->strSource;
				pNode->pHToken->strSource = NULL;
			}

			pNode->pHToken->strSource = new char[nLen+1];
			memset(pNode->pHToken->strSource, 0x00, nLen+1);

			nMovePos = 1;

			while (nLen > 0)
			{
				if (nMovePos != nPos)
				{
					pNode->pHToken->strSource[nCount] = pOriginalString[nCount];
					nLen -= 1;
					nCount += 1;
					nMovePos += 1;
				}
				else
				{
					if (strlen(pJongChar) > 0)
					{
						pNode->pHToken->strSource[nCount] = pHapChar[0];
						nCount += 1;
						nLen -= 1;
						pNode->pHToken->strSource[nCount] = pHapChar[1];
						nCount += 1;
						nLen -= 1;
						nMovePos += 2;
					}
					else
					{
						pNode->pHToken->strSource[nCount] = pChoJungChar[0];
						nCount += 1;
						nLen -= 1;
						pNode->pHToken->strSource[nCount] = pChoJungChar[1];
						nCount += 1;
						nLen -= 1;
						nMovePos += 2;
					}
				}
			}
		}
		break;
	}

	CHAR_DELETE(pOriginalString);
	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pChoJungChar);
}

int CNAHangulAnaly::GetNextPrevPositionWord(char *pOriginalStr, int nStartPos, int nCount, int nCase)
{
	unsigned int IsKindCode = 0;
	int nLen = 0;
	int syl, high, low;
	char OneChar[3] = { 0, };
	int nReturnValue = 0;

	if (pOriginalStr != NULL) {
		nLen = strlen(pOriginalStr);
		
		if (nStartPos - 1 > 0)
		{
			if (*(pOriginalStr + (nStartPos - 1)) & 0x80)
			{
				OneChar[0] = pOriginalStr[nStartPos - 2];
				OneChar[1] = pOriginalStr[nStartPos - 1];
			}
			else
			{
				OneChar[0] = pOriginalStr[nStartPos - 1];
			}
		}
		else
		{
			OneChar[0] = pOriginalStr[nStartPos - 1];
		}
		/*
		OneChar[0] = pOriginalStr[nStartPos - 1];
		if (OneChar[0] & 0x80)
		{
			OneChar[1] = pOriginalStr[nStartPos];
		}
		*/
		syl = GetSylCode(OneChar);
		IsKindCode = GetHighLow(syl, &high, &low);
		switch (IsKindCode)
		{
			case HANGUL_CHAR_WORD:		// �ѱ�
			case CHINESE_CHAR_WORD:		// ����  
			case KATAGANA_CHAR_WORD:	// ��ī���� 
			case HIRAGANA_CHAR_WORD:	// ���󰡳�
			case SPECIAL_CHAR_WORD:		// Ư������
				switch (nCase)
				{
					case CALCULATE_MINUS:
							nReturnValue = nStartPos - 2;
						break;
					case CALCULATE_PLUS:
							nReturnValue = nStartPos+1;
						break;
				}
				break;
		default:
			switch (nCase)
			{
				case CALCULATE_MINUS:
					nReturnValue = nStartPos - 1;
					break;
				case CALCULATE_PLUS:
						nReturnValue = nStartPos+1;
					break;
			}
			break;
		}
		
	}
	
	//���ܻ�Ȳö��
	if (nReturnValue < 0)
		nReturnValue = 0;
	else if (strlen(pOriginalStr) < (unsigned)nReturnValue)
		nReturnValue = strlen(pOriginalStr);

	return nReturnValue;
	
	/*
	switch (nCase)
	{
	case CALCULATE_MINUS:
		switch (nCount)
		{
		case 1:
			if (CalculateWordCount(pOriginalStr, nStartPos, nCount, CALCULATE_MINUS))
			{
				if (IsThisWordHangulAscii(pOriginalStr, nStartPos))
					nReturnValue = nStartPos - 2;
				else
					nReturnValue = nStartPos - 1;
				if (nReturnValue < 0)
					nReturnValue = 0;
			}
			else
			{
				if (IsThisWordHangulAscii(pOriginalStr, nStartPos))
					nReturnValue = nStartPos - 2;
				else
					nReturnValue = nStartPos - 1;
				if (nReturnValue < 0)
					nReturnValue = 0;
			}
			break;
		case 2:
			if (CalculateWordCount(pOriginalStr, nStartPos, nCount, CALCULATE_MINUS))
			{
				if (IsThisWordHangulAscii(pOriginalStr, nStartPos))
				{
					if (CalculateWordCount(pOriginalStr, nStartPos, 1, CALCULATE_MINUS))
					{
						nReturnValue = nStartPos - 4;
						if (nReturnValue < 0)
							nReturnValue = 0;
					}
					else
					{
						nReturnValue = nStartPos - 3;
						if (nReturnValue < 0)
							nReturnValue = 0;
					}
				}
				else
				{
					if (CalculateWordCount(pOriginalStr, nStartPos, 1, CALCULATE_MINUS))
					{
						nReturnValue = nStartPos - 3;
						if (nReturnValue < 0)
							nReturnValue = 0;
					}
					else
					{
						nReturnValue = nStartPos - 2;
						if (nReturnValue < 0)
							nReturnValue = 0;
					}
				}
			}
			else
			{
				if (IsThisWordHangulAscii(pOriginalStr, nStartPos))
				{
					if (CalculateWordCount(pOriginalStr, nStartPos, 1, CALCULATE_MINUS))
					{
						nReturnValue = nStartPos - 4;
						if (nReturnValue < 0)
							nReturnValue = 0;
					}
					else
					{
						nReturnValue = nStartPos - 3;
						if (nReturnValue < 0)
							nReturnValue = 0;
					}
				}
				else
				{
					if (CalculateWordCount(pOriginalStr, nStartPos, 1, CALCULATE_MINUS))
					{
						nReturnValue = nStartPos - 3;
						if (nReturnValue < 0)
							nReturnValue = 0;
					}
					else
					{
						nReturnValue = nStartPos - 2;
						if (nReturnValue < 0)
							nReturnValue = 0;
					}
				}
			}
			break;
		}
		break;
	case CALCULATE_PLUS:
		if (CalculateWordCount(pOriginalStr, nStartPos, nCount, CALCULATE_PLUS))
		{
			nReturnValue = nStartPos+2;
			if (strlen(pOriginalStr) < (unsigned)nReturnValue)
				nReturnValue = strlen(pOriginalStr);
		}
		else
		{
			nReturnValue = nStartPos+1;
			if (strlen(pOriginalStr) < (unsigned)nReturnValue)
				nReturnValue = strlen(pOriginalStr);
		}
		break;
	}

	return nReturnValue;
	*/
	
}

bool CNAHangulAnaly::IsExistNFeature(char *pString, int nStart, int nFeature)
{
	bool isExist = false;
	char Feature_Stem[STEM_DIC_ALL_FEATURE_NUMBER];
	int FindIndex = -1;
	char IS_CHECK_FEATURE = '1';
	char* pStartCountChar = NULL;
	pStartCountChar = GetFromFirstToStartWord(pString, nStart);

	if (IsExistStemDic(pStartCountChar) == true)
	{
		memset(Feature_Stem, 0x00, sizeof(Feature_Stem));
		IsExistStemDic(pStartCountChar, (char *)Feature_Stem);
		if (Feature_Stem[nFeature] == IS_CHECK_FEATURE)
			isExist = true;
		else
			isExist = false;
	}

	CHAR_DELETE(pStartCountChar);

	return isExist;
}


void CNAHangulAnaly::InitOldNewBuffer()
{
	memset(m_ArrayOldBuffer, 0x00, sizeof(m_ArrayOldBuffer));
	memset(m_ArrayNewBuffer, 0x00, sizeof(m_ArrayNewBuffer));
}


void CNAHangulAnaly::SaveHTokenEndingOldFeature(H_NODE *pNode)
{
	int nFeature = 0;
	for (nFeature = 0; nFeature < ENDING_DIC_BASIC_FEATURE_NUMBER; nFeature++)
	{
		if (m_ArrayOldBuffer[nFeature] == '1')
		{
			pNode->pHToken->subEnding->strFeature[nFeature] = m_ArrayOldBuffer[nFeature];
		}
		else
		{
			if (pNode->pHToken->subEnding->strFeature[nFeature] == '1')
				pNode->pHToken->subEnding->strFeature[nFeature] = '1';
			else
				pNode->pHToken->subEnding->strFeature[nFeature] = '0';
		}
	}
}

void CNAHangulAnaly::SaveHTokenEndingNewFeature(H_NODE *pNode)
{
	int nFeature = 0;
	for (nFeature = 0; nFeature < ENDING_DIC_BASIC_FEATURE_NUMBER; nFeature++)
	{
		if (m_ArrayNewBuffer[nFeature] == 1)
		{
			pNode->pHToken->subEnding->strFeature[nFeature] = m_ArrayNewBuffer[nFeature];
		}
		else
		{
			if (pNode->pHToken->subEnding->strFeature[nFeature] == '1')
				pNode->pHToken->subEnding->strFeature[nFeature] = '1';
			else
				pNode->pHToken->subEnding->strFeature[nFeature] = '0';
		}
	}
}

void CNAHangulAnaly::AddEndingFeature(H_NODE *pEndingNode, char *pString)
{
	char Feature_End[ENDING_DIC_BASIC_FEATURE_NUMBER];
	char IS_CEHCK_FEATURE = '1';
	memset(Feature_End, 0x00, sizeof(Feature_End));

	if (pString != NULL && strlen(pString) > 0)
	{
		if (IsExistEndingDic(pString) == true)
		{
			IsExistEndingDic(pString, (char *)Feature_End);
			if (pEndingNode->pHToken->subEnding->strFeature == "")
			{
				for (int Feature = 0; Feature < ENDING_DIC_BASIC_FEATURE_NUMBER; Feature++)
				{
					if (Feature_End[Feature] == IS_CEHCK_FEATURE)
						pEndingNode->pHToken->subEnding->strFeature[Feature] = '1';
					else
						pEndingNode->pHToken->subEnding->strFeature[Feature] = '0';
				}
			}
			else
			{
				if (pEndingNode->pHToken->subEnding->strFeature == NULL)
				{
					pEndingNode->pHToken->subEnding->strFeature = new char[ENDING_DIC_BASIC_FEATURE_NUMBER];
					memset(pEndingNode->pHToken->subEnding->strFeature, 0x00, ENDING_DIC_BASIC_FEATURE_NUMBER);
				}
				for (int Feature = 0; Feature < ENDING_DIC_BASIC_FEATURE_NUMBER; Feature++)
				{
					if (Feature_End[Feature] == IS_CEHCK_FEATURE ||
						pEndingNode->pHToken->subEnding->strFeature[Feature] == IS_CEHCK_FEATURE)
						pEndingNode->pHToken->subEnding->strFeature[Feature] = '1';
					else
						pEndingNode->pHToken->subEnding->strFeature[Feature] = '0';
				}
			}
		}
	}
}

void CNAHangulAnaly::MakeNewEndingFeature(H_NODE *pEndingNode)
{
	if (pEndingNode->pHToken->subEnding->strFeature != NULL)
	{
		delete[]pEndingNode->pHToken->subEnding->strFeature;
		pEndingNode->pHToken->subEnding->strFeature = NULL;
		pEndingNode->pHToken->subEnding->strFeature = new char[ENDING_DIC_BASIC_FEATURE_NUMBER];
		memset(pEndingNode->pHToken->subEnding->strFeature, 0x00, ENDING_DIC_BASIC_FEATURE_NUMBER);
	}
	else
	{
		pEndingNode->pHToken->subEnding->strFeature = new char[ENDING_DIC_BASIC_FEATURE_NUMBER];
		memset(pEndingNode->pHToken->subEnding->strFeature, 0x00, ENDING_DIC_BASIC_FEATURE_NUMBER);
	}
}


void CNAHangulAnaly::InsertArrayFeatureNew(char *pString)
{
	char Feature_End[ENDING_DIC_BASIC_FEATURE_NUMBER];
	char IS_CEHCK_FEATURE = '1';
	int Feature = 0;
	int FindIndex = -1;

	memset(Feature_End, 0x00, sizeof(Feature_End));

	if (pString != NULL && strlen(pString) > 0)
	{
		if (IsExistEndingDic(pString) == true)
		{
			if (m_ArrayNewBuffer != "")
				memset(m_ArrayNewBuffer, 0x00, sizeof(m_ArrayNewBuffer));
			IsExistEndingDic(pString, (char *)Feature_End);
			for (Feature = 0; Feature < ENDING_DIC_BASIC_FEATURE_NUMBER; Feature++)
			{
				if (Feature_End[Feature] == IS_CEHCK_FEATURE)
				{
					m_ArrayNewBuffer[Feature] = '1';
				}
				else
				{
					if (m_ArrayNewBuffer[Feature] == '1')
						m_ArrayNewBuffer[Feature] = '1';
					else
						m_ArrayNewBuffer[Feature] = '0';
				}
			}
		}
	}
}

void CNAHangulAnaly::InsertArrayFeatureOld(char *pString)
{
	char Feature_End[ENDING_DIC_BASIC_FEATURE_NUMBER];
	char IS_CEHCK_FEATURE = '1';
	int Feature = 0;
	int FindIndex = -1;
	int nLen = 0;

	memset(Feature_End, 0x00, sizeof(Feature_End));

	//������ -> �� -> ���� -> ��+���� 
	//���� -> ��+��+�� 
	if (pString != NULL && strlen(pString) > 0)
	{
		if (IsExistEndingDic(pString) == true)
		{
			IsExistEndingDic(pString, (char *)Feature_End);
			if (m_pPrevOldBuffer != NULL)
			{
				int nCount = 0;
				int nTotalLen = 0;
				nLen = strlen(m_pPrevOldBuffer);
				char* pBuffer = NULL;
				pBuffer = new char[nLen+1];
				memset(pBuffer, 0x00, nLen+1);
				nTotalLen = strlen(pString);
				nTotalLen -= nLen;
				while (nLen > 0)
				{
					pBuffer[nCount] = pString[nTotalLen];
					nCount += 1;
					nTotalLen += 1;
					nLen -= 1;
				}
				if (strcmp(m_pPrevOldBuffer, pBuffer) == 0)
				{
					if (m_ArrayOldBuffer != "")
						memset(m_ArrayOldBuffer, 0x00, sizeof(m_ArrayOldBuffer));
				}
				CHAR_DELETE(pBuffer);

				//������ -> �� -> ���� -> ��+���� 
				//���� -> ��+��+�� 
			}
			else
			{
				if (m_ArrayOldBuffer != "")
					memset(m_ArrayOldBuffer, 0x00, sizeof(m_ArrayOldBuffer));
			}

			for (Feature = 0; Feature < ENDING_DIC_BASIC_FEATURE_NUMBER; Feature++)
			{
				if (Feature_End[Feature] == IS_CEHCK_FEATURE)
				{
					m_ArrayOldBuffer[Feature] = '1';
				}
				else
				{
					if (m_ArrayOldBuffer[Feature] == '1')
						m_ArrayOldBuffer[Feature] = '1';
					else
						m_ArrayOldBuffer[Feature] = '0';
				}
			}
		}
	}

	if (pString != NULL && strlen(pString) > 0)
	{
		nLen = strlen(pString);

		CHAR_DELETE(m_pPrevOldBuffer);

		m_pPrevOldBuffer = new char[nLen+1];
		memset(m_pPrevOldBuffer, 0x00, nLen+1);
		strcpy(m_pPrevOldBuffer, pString);
	}
}

void CNAHangulAnaly::AddOldNewBuffer(H_NODE *pNode)
{
	char* pTempBuffer = NULL;
	int nLen = 0;
	int nCount = 0;
	int nMove = 0;

	char* pAddTwoString = NULL;
	char* pAddTwoString2 = NULL;


	for (nLen = 0; nLen < ENDING_DIC_BASIC_FEATURE_NUMBER; nLen++)
	{
		if (m_ArrayOldBuffer[nLen] != m_ArrayNewBuffer[nLen])
		{
			if (m_ArrayOldBuffer[nLen] == '0' && m_ArrayNewBuffer[nLen] == '1')
				m_ArrayOldBuffer[nLen] = m_ArrayNewBuffer[nLen];
		}
	}

	if (m_pOldBuffer != NULL && m_pNewBuffer != NULL)
	{
		if (m_ArrayOldBuffer[0] == '1' && m_ArrayNewBuffer[0] == '1')
		{
			pAddTwoString = AddTwoStr(m_pNewBuffer, "%");
			pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
			if (pNode->pHToken->strAmbEndingSource != NULL)
			{
				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr(pNode->pHToken->strAmbEndingSource, pAddTwoString2);
				delete[]pNode->pHToken->strAmbEndingSource;
				pNode->pHToken->strAmbEndingSource = NULL;
				nLen = strlen(pAddTwoString);
				nCount = 0;
				pNode->pHToken->strAmbEndingSource = new char[nLen+1];
				memset(pNode->pHToken->strAmbEndingSource, 0x00, nLen+1);
				strcpy(pNode->pHToken->strAmbEndingSource, pAddTwoString);
			}
			else
			{
				nLen = strlen(pAddTwoString2);
				nCount = 0;
				pNode->pHToken->strAmbEndingSource = new char[nLen+1];
				memset(pNode->pHToken->strAmbEndingSource, 0x00, nLen+1);
				strcpy(pNode->pHToken->strAmbEndingSource, pAddTwoString2);
			}
		}
		else if (m_ArrayOldBuffer[0] == '1' && m_ArrayNewBuffer[0] == '0')
		{
			CHAR_DELETE(pAddTwoString);

			pAddTwoString = AddTwoStr(m_pOldBuffer, "%");

			if (pNode->pHToken->strAmbEndingSource != NULL)
			{
				pAddTwoString2 = AddTwoStr(pNode->pHToken->strAmbEndingSource, pAddTwoString);

				CHAR_DELETE(pNode->pHToken->strAmbEndingSource);

				nLen = strlen(pAddTwoString2);
				nCount = 0;
				pNode->pHToken->strAmbEndingSource = new char[nLen+1];
				memset(pNode->pHToken->strAmbEndingSource, 0x00, nLen+1);
				strcpy(pNode->pHToken->strAmbEndingSource, pAddTwoString2);
			}
			else
			{
				nLen = strlen(pAddTwoString);
				nCount = 0;
				pNode->pHToken->strAmbEndingSource = new char[nLen+1];
				memset(pNode->pHToken->strAmbEndingSource, 0x00, nLen+1);
				strcpy(pNode->pHToken->strAmbEndingSource, pAddTwoString);
			}
		}
		else if (m_ArrayOldBuffer[0] == '0' && m_ArrayNewBuffer[0] == '1')
		{
			CHAR_DELETE(pAddTwoString);

			pAddTwoString = AddTwoStr(m_pNewBuffer, "%");
			if (pNode->pHToken->strAmbEndingSource != NULL)
			{
				pAddTwoString2 = AddTwoStr(pNode->pHToken->strAmbEndingSource, pAddTwoString);

				CHAR_DELETE(pNode->pHToken->strAmbEndingSource);

				nLen = strlen(pAddTwoString2);
				nCount = 0;
				pNode->pHToken->strAmbEndingSource = new char[nLen+1];
				memset(pNode->pHToken->strAmbEndingSource, 0x00, nLen+1);
				strcpy(pNode->pHToken->strAmbEndingSource, pAddTwoString2);
			}
			else
			{
				nLen = strlen(pAddTwoString);
				nCount = 0;
				pNode->pHToken->strAmbEndingSource = new char[nLen+1];
				memset(pNode->pHToken->strAmbEndingSource, 0x00, nLen+1);
				strcpy(pNode->pHToken->strAmbEndingSource, pAddTwoString);
			}
		}
	}
	else if (m_pOldBuffer != NULL && m_pNewBuffer == NULL)
	{
		if (m_ArrayOldBuffer[0] == '1')
		{
			CHAR_DELETE(pAddTwoString);

			pAddTwoString = AddTwoStr(m_pOldBuffer, "%");

			if (pNode->pHToken->strAmbEndingSource != NULL)
			{
				pAddTwoString2 = AddTwoStr(pNode->pHToken->strAmbEndingSource, pAddTwoString);

				CHAR_DELETE(pNode->pHToken->strAmbEndingSource);

				nLen = strlen(pAddTwoString2);
				nCount = 0;
				pNode->pHToken->strAmbEndingSource = new char[nLen+1];
				memset(pNode->pHToken->strAmbEndingSource, 0x00, nLen+1);
				strcpy(pNode->pHToken->strAmbEndingSource, pAddTwoString2);
			}
			else
			{
				nLen = strlen(pAddTwoString);
				nCount = 0;
				pNode->pHToken->strAmbEndingSource = new char[nLen+1];
				memset(pNode->pHToken->strAmbEndingSource, 0x00, nLen+1);
				strcpy(pNode->pHToken->strAmbEndingSource, pAddTwoString);
			}
		}
	}
	else if (m_pOldBuffer == NULL && m_pNewBuffer != NULL)
	{
		if (m_ArrayNewBuffer[0] == '1')
		{
			CHAR_DELETE(pAddTwoString);

			pAddTwoString = AddTwoStr(m_pNewBuffer, "%");
			if (pNode->pHToken->strAmbEndingSource != NULL)
			{
				pAddTwoString2 = AddTwoStr(pNode->pHToken->strAmbEndingSource, pAddTwoString);

				CHAR_DELETE(pNode->pHToken->strAmbEndingSource);

				nLen = strlen(pAddTwoString2);
				nCount = 0;
				pNode->pHToken->strAmbEndingSource = new char[nLen+1];
				memset(pNode->pHToken->strAmbEndingSource, 0x00, nLen+1);
				strcpy(pNode->pHToken->strAmbEndingSource, pAddTwoString2);
			}
			else
			{
				nLen = strlen(pAddTwoString);
				nCount = 0;
				pNode->pHToken->strAmbEndingSource = new char[nLen+1];
				memset(pNode->pHToken->strAmbEndingSource, 0x00, nLen+1);
				strcpy(pNode->pHToken->strAmbEndingSource, pAddTwoString);
			}
		}
	}
	if (m_pNewBuffer != NULL)
		nLen = strlen(m_pNewBuffer);

	if (m_pOldBuffer != NULL)
		nLen += strlen(m_pOldBuffer);

	pTempBuffer = new char[nLen+1];
	memset(pTempBuffer, 0x00, nLen+1);

	if (m_pNewBuffer != NULL)
	{
		nLen = strlen(m_pNewBuffer);
		while (nLen)
		{
			pTempBuffer[nCount] = m_pNewBuffer[nCount];
			nLen -= 1;
			nCount += 1;
		}
	}
	if (m_pOldBuffer != NULL)
	{
		nLen = strlen(m_pOldBuffer);
		while (nLen)
		{
			pTempBuffer[nCount] = m_pOldBuffer[nMove];
			nLen -= 1;
			nCount += 1;
			nMove += 1;
		}
	}

	CHAR_DELETE(m_pOldBuffer);

	nLen = strlen(pTempBuffer);
	m_pOldBuffer = new char[nLen+1];
	memset(m_pOldBuffer, 0x00, nLen+1);
	nCount = 0;
	while (nLen)
	{
		m_pOldBuffer[nCount] = pTempBuffer[nCount];
		nLen -= 1;
		nCount += 1;
	}

	CHAR_DELETE(pTempBuffer);
	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pAddTwoString2);
}


void CNAHangulAnaly::MovingLeftPositionWord(char *pString, int &nCount)
{
	unsigned int IsKindCode = 0;
	int syl, high, low;
	char OneChar[3] = {0,};
	char* pReturnStr = NULL;

	if (pString != NULL) {
		if (nCount - 1 > 0)
		{
			if (*(pString + (nCount - 1)) & 0x80)
			{
				OneChar[0] = pString[nCount - 2];
				OneChar[1] = pString[nCount - 1];
			}
			else
			{
				OneChar[0] = pString[nCount - 1];
			}
		}
		else
		{
			OneChar[0] = pString[nCount - 1];
		}
		
		syl = GetSylCode(OneChar);
		IsKindCode = GetHighLow(syl, &high, &low);

		switch (IsKindCode)
		{
			case HANGUL_CHAR_WORD:		// �ѱ�
			case CHINESE_CHAR_WORD:		// ����  
			case KATAGANA_CHAR_WORD:	// ��ī���� 
			case HIRAGANA_CHAR_WORD:	// ���󰡳�
			case SPECIAL_CHAR_WORD:		// Ư������
				nCount -= 2;
			break;
		default:
			nCount -= 1;
			break;
		}
	}
	else
	{
		nCount = 0;
	}
}

void CNAHangulAnaly::SaveAmbstrEnding(H_NODE *pNode, char *pEndStr)
{
	bool bFindAmbFeature = false;
	char Feature_End[ENDING_DIC_BASIC_FEATURE_NUMBER];
	char IS_CEHCK_FEATURE = '1';
	memset(Feature_End, 0x00, sizeof(Feature_End));
	int nLen = 0;
	char* pAddTwoString = NULL;
	char* pAddTwoString2 = NULL;

	if (pEndStr != NULL)
	{
		if (IsExistEndingDic(pEndStr) == true)
		{
			IsExistEndingDic(pEndStr, (char *)Feature_End);
			if (Feature_End[0] == IS_CEHCK_FEATURE)
			{
				if (pNode->pHToken->strAmbEndingSource == NULL)
				{
					nLen = strlen(pEndStr);
					pNode->pHToken->strAmbEndingSource = new char[nLen+1];
					memset(pNode->pHToken->strAmbEndingSource, 0x00, nLen+1);
					strcpy(pNode->pHToken->strAmbEndingSource, pEndStr);
				}
				else
				{
					pAddTwoString = AddTwoStr(pNode->pHToken->strAmbEndingSource, "%");
					pAddTwoString2 = AddTwoStr(pAddTwoString, pEndStr);

					nLen = strlen(pAddTwoString2);

					CHAR_DELETE(pNode->pHToken->strAmbEndingSource);

					pNode->pHToken->strAmbEndingSource = new char[nLen+1];
					memset(pNode->pHToken->strAmbEndingSource, 0x00, nLen+1);
					strcpy(pNode->pHToken->strAmbEndingSource, pAddTwoString2);

				}
			}
		}
	}
	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pAddTwoString2);
}

//Is Find Table Index ...
bool CNAHangulAnaly::CompareVariousTable01(char *pOneChar, int nIndex, bool isAll)
{
	bool isExistTable = false;
	int nStart = 0;

	if (isAll == false)
	{
		switch (nIndex)
		{
		case 0:
			for (nStart = 0; nStart < ALL_TABLE_01; nStart++)
			{
				if (strcmp(pOneChar, Table01[nStart].strOneWord) == 0)
				{
					m_nTableNumber = 1;
					m_nTableIndex = nStart;
					m_nTableString = 1;
					isExistTable = true;
					break;
				}
			}
			break;
		case 1:
			for (nStart = 0; nStart < ALL_TABLE_01; nStart++)
			{
				if (strcmp(pOneChar, Table01[nStart].strTwoWord) == 0)
				{
					m_nTableNumber = 1;
					m_nTableIndex = nStart;
					m_nTableString = 2;
					isExistTable = true;
					break;
				}
			}
			break;
		case 2:
			for (nStart = 0; nStart < ALL_TABLE_01; nStart++)
			{
				if (strcmp(pOneChar, Table01[nStart].strThreeWord) == 0)
				{
					m_nTableNumber = 1;
					m_nTableIndex = nStart;
					m_nTableString = 3;
					isExistTable = true;
					break;
				}
			}
			break;
		}
	}
	else
	{
		for (nStart = 0; nStart < ALL_TABLE_01; nStart++)
		{
			if (strcmp(pOneChar, Table01[nStart].strOneWord) == 0 ||
				strcmp(pOneChar, Table01[nStart].strTwoWord) == 0 ||
				strcmp(pOneChar, Table01[nStart].strThreeWord) == 0)
			{
				if (strcmp(pOneChar, Table01[nStart].strOneWord) == 0)
				{
					m_nTableNumber = 1;
					m_nTableIndex = nStart;
					m_nTableString = 1;
				}
				if (strcmp(pOneChar, Table01[nStart].strTwoWord) == 0)
				{
					m_nTableNumber = 1;
					m_nTableIndex = nStart;
					m_nTableString = 2;
				}
				if (strcmp(pOneChar, Table01[nStart].strThreeWord) == 0)
				{
					m_nTableNumber = 1;
					m_nTableIndex = nStart;
					m_nTableString = 3;
				}
				isExistTable = true;
				break;
			}
		}
	}
	return isExistTable;
}

bool CNAHangulAnaly::CompareVariousTable02(char *pOneChar, int nIndex, bool isAll)
{
	bool isExistTable = false;
	int nStart = 0;
	if (isAll == false)
	{
		switch (nIndex)
		{
		case 0:
			for (nStart = 0; nStart < ALL_TABLE_02; nStart++)
			{
				if (strcmp(pOneChar, Table02[nStart].strOneWord) == 0)
				{
					m_nTableNumber = 2;
					m_nTableIndex = nStart;
					m_nTableString = 1;
					isExistTable = true;
					break;
				}
			}
			break;
		case 1:
			for (nStart = 0; nStart < ALL_TABLE_02; nStart++)
			{
				if (strcmp(pOneChar, Table02[nStart].strTwoWord) == 0)
				{
					m_nTableNumber = 2;
					m_nTableIndex = nStart;
					m_nTableString = 2;
					isExistTable = true;
					break;
				}
			}
			break;
		case 2:
			for (nStart = 0; nStart < ALL_TABLE_02; nStart++)
			{
				if (strcmp(pOneChar, Table02[nStart].strThreeWord) == 0)
				{
					m_nTableNumber = 2;
					m_nTableIndex = nStart;
					m_nTableString = 3;
					isExistTable = true;
					break;
				}
			}
			break;
		}
	}
	else
	{
		for (nStart = 0; nStart < ALL_TABLE_02; nStart++)
		{
			if (strcmp(pOneChar, Table02[nStart].strOneWord) == 0 ||
				strcmp(pOneChar, Table02[nStart].strTwoWord) == 0 ||
				strcmp(pOneChar, Table02[nStart].strThreeWord) == 0)
			{
				if (strcmp(pOneChar, Table02[nStart].strOneWord) == 0)
				{
					m_nTableNumber = 2;
					m_nTableIndex = nStart;
					m_nTableString = 1;
				}
				if (strcmp(pOneChar, Table02[nStart].strTwoWord) == 0)
				{
					m_nTableNumber = 2;
					m_nTableIndex = nStart;
					m_nTableString = 2;
				}
				if (strcmp(pOneChar, Table02[nStart].strThreeWord) == 0)
				{
					m_nTableNumber = 2;
					m_nTableIndex = nStart;
					m_nTableString = 3;
				}
				isExistTable = true;
				break;
			}
		}
	}
	return isExistTable;
}

bool CNAHangulAnaly::CompareVariousTable03(char *pOneChar, int nIndex, bool isAll)
{
	bool isExistTable = false;
	int nStart = 0;
	if (isAll == false)
	{
		switch (nIndex)
		{
		case 0:
			for (nStart = 0; nStart < ALL_TABLE_03; nStart++)
			{
				if (strcmp(pOneChar, Table03[nStart].strOneWord) == 0)
				{
					m_nTableNumber = 3;
					m_nTableIndex = nStart;
					m_nTableString = 1;
					isExistTable = true;
					break;
				}
			}
			break;
		case 1:
			for (nStart = 0; nStart < ALL_TABLE_03; nStart++)
			{
				if (strcmp(pOneChar, Table03[nStart].strTwoWord) == 0)
				{
					m_nTableNumber = 3;
					m_nTableIndex = nStart;
					m_nTableString = 2;
					isExistTable = true;
					break;
				}
			}
			break;
		}
	}
	else
	{
		for (nStart = 0; nStart < ALL_TABLE_03; nStart++)
		{
			if (strcmp(pOneChar, Table03[nStart].strOneWord) == 0 ||
				strcmp(pOneChar, Table03[nStart].strTwoWord) == 0)
			{
				if (strcmp(pOneChar, Table03[nStart].strOneWord) == 0)
				{
					m_nTableNumber = 3;
					m_nTableIndex = nStart;
					m_nTableString = 1;
				}
				if (strcmp(pOneChar, Table03[nStart].strTwoWord) == 0)
				{
					m_nTableNumber = 3;
					m_nTableIndex = nStart;
					m_nTableString = 2;
				}
				isExistTable = true;
				break;
			}
		}
	}
	return isExistTable;
}

bool CNAHangulAnaly::CompareVariousTable04(char *pOneChar, int nIndex, bool isAll)
{
	bool isExistTable = false;
	int nStart = 0;
	if (isAll == false)
	{
		switch (nIndex)
		{
		case 0:
			for (nStart = 0; nStart < ALL_TABLE_04; nStart++)
			{
				if (strcmp(pOneChar, Table04[nStart].strOneWord) == 0)
				{
					m_nTableNumber = 4;
					m_nTableIndex = nStart;
					m_nTableString = 1;
					isExistTable = true;
					break;
				}
			}
			break;
		case 1:
			for (nStart = 0; nStart < ALL_TABLE_04; nStart++)
			{
				if (strcmp(pOneChar, Table04[nStart].strTwoWord) == 0)
				{
					m_nTableNumber = 4;
					m_nTableIndex = nStart;
					m_nTableString = 2;
					isExistTable = true;
					break;
				}
			}
			break;
		case 2:
			for (nStart = 0; nStart < ALL_TABLE_04; nStart++)
			{
				if (strcmp(pOneChar, Table04[nStart].strThreeWord) == 0)
				{
					m_nTableNumber = 4;
					m_nTableIndex = nStart;
					m_nTableString = 3;
					isExistTable = true;
					break;
				}
			}
			break;
		case 3:
			for (nStart = 0; nStart < ALL_TABLE_04; nStart++)
			{
				if (strcmp(pOneChar, Table04[nStart].strFourWord) == 0)
				{
					m_nTableNumber = 4;
					m_nTableIndex = nStart;
					m_nTableString = 4;
					isExistTable = true;
					break;
				}
			}
			break;
		}
	}
	else
	{
		for (nStart = 0; nStart < ALL_TABLE_04; nStart++)
		{
			if (strcmp(pOneChar, Table04[nStart].strOneWord) == 0 ||
				strcmp(pOneChar, Table04[nStart].strTwoWord) == 0 ||
				strcmp(pOneChar, Table04[nStart].strThreeWord) == 0 ||
				strcmp(pOneChar, Table04[nStart].strFourWord) == 0)
			{
				if (strcmp(pOneChar, Table04[nStart].strOneWord) == 0)
				{
					m_nTableNumber = 4;
					m_nTableIndex = nStart;
					m_nTableString = 1;
				}
				if (strcmp(pOneChar, Table04[nStart].strTwoWord) == 0)
				{
					m_nTableNumber = 4;
					m_nTableIndex = nStart;
					m_nTableString = 2;
				}
				if (strcmp(pOneChar, Table04[nStart].strThreeWord) == 0)
				{
					m_nTableNumber = 4;
					m_nTableIndex = nStart;
					m_nTableString = 3;
				}
				if (strcmp(pOneChar, Table04[nStart].strFourWord) == 0)
				{
					m_nTableNumber = 4;
					m_nTableIndex = nStart;
					m_nTableString = 4;
				}
				isExistTable = true;
				break;
			}
		}
	}
	return isExistTable;
}

bool CNAHangulAnaly::CompareVariousTable05(char *pOneChar, int nIndex, bool isAll)
{
	bool isExistTable = false;
	int nStart = 0;
	if (isAll == false)
	{
		switch (nIndex)
		{
		case 0:
			for (nStart = 0; nStart < ALL_TABLE_05; nStart++)
			{
				if (strcmp(pOneChar, Table05[nStart].strOneWord) == 0)
				{
					m_nTableNumber = 5;
					m_nTableIndex = nStart;
					m_nTableString = 1;
					isExistTable = true;
					break;
				}
			}
			break;
		}
	}
	else
	{
		for (nStart = 0; nStart < ALL_TABLE_05; nStart++)
		{
			if (strcmp(pOneChar, Table05[nStart].strOneWord) == 0)
			{
				m_nTableNumber = 5;
				m_nTableIndex = nStart;
				m_nTableString = 1;
				isExistTable = true;
				break;
			}
		}
	}
	return isExistTable;
}

bool CNAHangulAnaly::CompareVariousTable06(char *pOneChar, int nIndex, bool isAll)
{
	bool isExistTable = false;
	int nStart = 0;
	if (isAll == false)
	{
		switch (nIndex)
		{
		case 0:
			for (nStart = 0; nStart < ALL_TABLE_06; nStart++)
			{
				if (strcmp(pOneChar, Table06[nStart].strOneWord) == 0)
				{
					m_nTableNumber = 6;
					m_nTableIndex = nStart;
					m_nTableString = 1;
					isExistTable = true;
					break;
				}
			}
			break;
		}
	}
	else
	{
		for (nStart = 0; nStart < ALL_TABLE_06; nStart++)
		{
			if (strcmp(pOneChar, Table06[nStart].strOneWord) == 0)
			{
				m_nTableNumber = 6;
				m_nTableIndex = nStart;
				m_nTableString = 1;
				isExistTable = true;
				break;
			}
		}
	}
	return isExistTable;
}

bool CNAHangulAnaly::CompareVariousTable07(char *pOneChar, int nIndex, bool isAll)
{
	bool isExistTable = false;
	int nStart = 0;
	if (isAll == false)
	{
		switch (nIndex)
		{
		case 0:
			for (nStart = 0; nStart < ALL_TABLE_07; nStart++)
			{
				if (strcmp(pOneChar, Table07[nStart].strOneWord) == 0)
				{
					m_nTableNumber = 7;
					m_nTableIndex = nStart;
					m_nTableString = 1;
					isExistTable = true;
					break;
				}
			}
			break;
		}
	}
	else
	{
		for (nStart = 0; nStart < ALL_TABLE_07; nStart++)
		{
			if (strcmp(pOneChar, Table07[nStart].strOneWord) == 0)
			{
				m_nTableNumber = 7;
				m_nTableIndex = nStart;
				m_nTableString = 1;
				isExistTable = true;
				break;
			}
		}
	}
	return isExistTable;
}

bool CNAHangulAnaly::CompareVariousTable08(char *pOneChar, int nIndex, bool isAll)
{
	bool isExistTable = false;
	int nStart = 0;
	if (isAll == false)
	{
		switch (nIndex)
		{
		case 0:
			for (nStart = 0; nStart < ALL_TABLE_08; nStart++)
			{
				if (strcmp(pOneChar, Table08[nStart].strOneWord) == 0)
				{
					m_nTableNumber = 8;
					m_nTableIndex = nStart;
					m_nTableString = 1;
					isExistTable = true;
					break;
				}
			}
			break;
		case 1:
			for (nStart = 0; nStart < ALL_TABLE_08; nStart++)
			{
				if (strcmp(pOneChar, Table08[nStart].strTwoWord) == 0)
				{
					m_nTableNumber = 8;
					m_nTableIndex = nStart;
					m_nTableString = 2;
					isExistTable = true;
					break;
				}
			}
			break;
		}
	}
	else
	{
		for (nStart = 0; nStart < ALL_TABLE_08; nStart++)
		{
			if (strcmp(pOneChar, Table08[nStart].strOneWord) == 0 ||
				strcmp(pOneChar, Table08[nStart].strTwoWord) == 0)
			{
				if (strcmp(pOneChar, Table08[nStart].strOneWord) == 0)
				{
					m_nTableNumber = 8;
					m_nTableIndex = nStart;
					m_nTableString = 1;
				}
				if (strcmp(pOneChar, Table08[nStart].strTwoWord) == 0)
				{
					m_nTableNumber = 8;
					m_nTableIndex = nStart;
					m_nTableString = 1;
				}
				isExistTable = true;
				break;
			}
		}
	}
	return isExistTable;
}

bool CNAHangulAnaly::CompareVariousTable09(char *pOneChar, int nIndex, bool isAll)
{
	bool isExistTable = false;
	int nStart = 0;
	if (isAll == false)
	{
		switch (nIndex)
		{
		case 0:
			for (nStart = 0; nStart < ALL_TABLE_09; nStart++)
			{
				if (strcmp(pOneChar, Table09[nStart].strOneWord) == 0)
				{
					m_nTableNumber = 9;
					m_nTableIndex = nStart;
					m_nTableString = 1;
					isExistTable = true;
					break;
				}
			}
			break;
		case 1:
			for (nStart = 0; nStart < ALL_TABLE_09; nStart++)
			{
				if (strcmp(pOneChar, Table09[nStart].strTwoWord) == 0)
				{
					m_nTableNumber = 9;
					m_nTableIndex = nStart;
					m_nTableString = 2;
					isExistTable = true;
					break;
				}
			}
			break;
		}
	}
	else
	{
		for (nStart = 0; nStart < ALL_TABLE_09; nStart++)
		{
			if (strcmp(pOneChar, Table09[nStart].strOneWord) == 0 ||
				strcmp(pOneChar, Table09[nStart].strTwoWord) == 0)
			{
				if (strcmp(pOneChar, Table09[nStart].strOneWord) == 0)
				{
					m_nTableNumber = 9;
					m_nTableIndex = nStart;
					m_nTableString = 1;
				}
				if (strcmp(pOneChar, Table09[nStart].strTwoWord) == 0)
				{
					m_nTableNumber = 9;
					m_nTableIndex = nStart;
					m_nTableString = 2;
				}
				isExistTable = true;
				break;
			}
		}
	}
	return isExistTable;
}

bool CNAHangulAnaly::CompareVariousTable10(char *pOneChar, int nIndex, bool isAll)
{
	bool isExistTable = false;
	int nStart = 0;
	if (isAll == false)
	{
		switch (nIndex)
		{
		case 0:
			for (nStart = 0; nStart < ALL_TABLE_10; nStart++)
			{
				if (strcmp(pOneChar, Table10[nStart].strOneWord) == 0)
				{
					m_nTableNumber = 10;
					m_nTableIndex = nStart;
					m_nTableString = 1;
					isExistTable = true;
					break;
				}
			}
			break;
		case 1:
			for (nStart = 0; nStart < ALL_TABLE_10; nStart++)
			{
				if (strcmp(pOneChar, Table10[nStart].strTwoWord) == 0)
				{
					m_nTableNumber = 10;
					m_nTableIndex = nStart;
					m_nTableString = 2;
					isExistTable = true;
					break;
				}
			}
			break;
		}
	}
	else
	{
		for (nStart = 0; nStart < ALL_TABLE_10; nStart++)
		{
			if (strcmp(pOneChar, Table10[nStart].strOneWord) == 0 ||
				strcmp(pOneChar, Table10[nStart].strTwoWord) == 0)
			{
				if (strcmp(pOneChar, Table10[nStart].strOneWord) == 0)
				{
					m_nTableNumber = 10;
					m_nTableIndex = nStart;
					m_nTableString = 1;
				}
				if (strcmp(pOneChar, Table10[nStart].strTwoWord) == 0)
				{
					m_nTableNumber = 10;
					m_nTableIndex = nStart;
					m_nTableString = 2;
				}
				isExistTable = true;
				break;
			}
		}
	}
	return isExistTable;
}

char* CNAHangulAnaly::GetAnyTableValue()
{
	char* pReturnChar = NULL;

	switch (m_nTableNumber)
	{
	case 1:
		switch (m_nTableString)
		{
		case 1:
			pReturnChar = Table01[m_nTableIndex].strOneWord;
			break;
		case 2:
			pReturnChar = Table01[m_nTableIndex].strTwoWord;
			break;
		case 3:
			pReturnChar = Table01[m_nTableIndex].strThreeWord;
			break;
		}
		break;
	case 2:
		switch (m_nTableString)
		{
		case 1:
			pReturnChar = Table02[m_nTableIndex].strOneWord;
			break;
		case 2:
			pReturnChar = Table02[m_nTableIndex].strTwoWord;
			break;
		case 3:
			pReturnChar = Table02[m_nTableIndex].strThreeWord;
			break;
		}
		break;
	case 3:
		switch (m_nTableString)
		{
		case 1:
			pReturnChar = Table03[m_nTableIndex].strOneWord;
			break;
		case 2:
			pReturnChar = Table03[m_nTableIndex].strTwoWord;
			break;
		}
		break;
	case 4:
		switch (m_nTableString)
		{
		case 1:
			pReturnChar = Table04[m_nTableIndex].strOneWord;
			break;
		case 2:
			pReturnChar = Table04[m_nTableIndex].strTwoWord;
			break;
		case 3:
			pReturnChar = Table04[m_nTableIndex].strThreeWord;
			break;
		}
		break;
	case 5:
		switch (m_nTableString)
		{
		case 1:
			pReturnChar = Table05[m_nTableIndex].strOneWord;
			break;
		}
		break;
	case 6:
		switch (m_nTableString)
		{
		case 1:
			pReturnChar = Table06[m_nTableIndex].strOneWord;
			break;
		}
		break;
	case 7:
		switch (m_nTableString)
		{
		case 1:
			pReturnChar = Table07[m_nTableIndex].strOneWord;
			break;
		}
		break;
	case 8:
		switch (m_nTableString)
		{
		case 1:
			pReturnChar = Table08[m_nTableIndex].strOneWord;
			break;
		case 2:
			pReturnChar = Table08[m_nTableIndex].strTwoWord;
			break;
		}
		break;
	case 9:
		switch (m_nTableString)
		{
		case 1:
			pReturnChar = Table09[m_nTableIndex].strOneWord;
			break;
		case 2:
			pReturnChar = Table09[m_nTableIndex].strTwoWord;
			break;
		}
		break;
	case 10:
		switch (m_nTableString)
		{
		case 1:
			pReturnChar = Table10[m_nTableIndex].strOneWord;
			break;
		case 2:
			pReturnChar = Table10[m_nTableIndex].strTwoWord;
			break;
		}
		break;
	}

	return pReturnChar;
}

void CNAHangulAnaly::CheckFirstWord()
{
	m_bIsFirstWord = true;
}

void CNAHangulAnaly::SetOneWordMarsk()
{
	m_bOneCharMask = true;
}

void CNAHangulAnaly::GoErrorFunction(H_NODE *pNode)
{
	int nLen = 0;
	int nCount = 0;;

	CHAR_DELETE(pNode->pHToken->strSource);

	nLen = strlen(m_pRealOriginalString);
	pNode->pHToken->strSource = new char[nLen+1];
	memset(pNode->pHToken->strSource, 0x00, nLen+1);
	nCount = 0;
	strcpy(pNode->pHToken->strSource, m_pRealOriginalString);


	CHAR_DELETE(pNode->pHToken->subStem->strResult);
	//CHAR_DELETE(pNode->pHToken->subStem->strFeature);
	CHAR_DELETE(pNode->pHToken->subEnding->strResult);
	CHAR_DELETE(pNode->pHToken->subEnding->strFeature);

	IntitFunctionUsingValue();//���¼� �м��� ���Ǵ� ���� �ʱ�ȭ
	//�̵�Ͼ� ó�� �Լ�
	UnRegisterWord(pNode);
}

void CNAHangulAnaly::AllInitMemberVariable()
{
	m_bIsFirstWord = true;
	m_bOneCharMask = false;

	m_nTableNumber = -1;
	m_nTableIndex = -1;
	m_nTableString = -1;

	CHAR_DELETE(m_pPrevOldBuffer);
	CHAR_DELETE(m_pOldBuffer);
	CHAR_DELETE(m_pNewBuffer);

	memset(m_ArrayOldBuffer, 0x00, sizeof(m_ArrayOldBuffer));
	memset(m_ArrayNewBuffer, 0x00, sizeof(m_ArrayNewBuffer));
}

void CNAHangulAnaly::InintMemberVariable()
{
}


void CNAHangulAnaly::InitTableNumber()
{
	m_nTableNumber = -1;
	m_nTableIndex = -1;
	m_nTableString = -1;
}

void CNAHangulAnaly::InitBuffer()
{
	CHAR_DELETE(m_pPrevOldBuffer);
	CHAR_DELETE(m_pOldBuffer);
	CHAR_DELETE(m_pNewBuffer);
}

void CNAHangulAnaly::InitOneWordMarsk()
{
	m_bOneCharMask = false;
}

void CNAHangulAnaly::AllAnalyString()
{
}

void CNAHangulAnaly::SaveRealOriginalStr(char *pSaveStr)
{
	int nLen = 0;
	if (pSaveStr != NULL && strlen(pSaveStr) > 0)
	{
		nLen = strlen(pSaveStr);
		CHAR_DELETE(m_pRealOriginalString);
		m_pRealOriginalString = new char[nLen+1];
		memset(m_pRealOriginalString, 0x00, nLen+1);
		strcpy(m_pRealOriginalString, pSaveStr);
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//
//	MprphAnaly Function ==> MAR1 Sector
//
////////////////////////////////////////////////////////////////////////////////////////

void CNAHangulAnaly::GoMAR1(H_NODE *pNode)
{
	if (pNode == NULL || pNode->pHToken == NULL || pNode->pHToken->strSource == NULL) {
		return;
	}
	int nStart = 0;
	int mStart = 0;
	int lStart = 0;
	IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
	AllInitMemberVariable();
	nStart = strlen(pNode->pHToken->strSource);
	LoggingFunction(pNode, "MAR1_00000+ ");
	if (IsExistStemDic(pNode->pHToken->strSource, m_stemFeature01))
	{//S(len~1)�� stem ������ �ִ°�? /*��ü ������ ������ ���� ���, ���� ó�� �״�� MAR1_00000
		LoggingFunction(pNode, "MAR1_00001+ ");
		if (m_stemFeature01[STEM_DIC_V] == IS_CEHCK_FEATURE)
		{//1y : �� stem��[v]�ΰ� ? MAR1_00001
			LoggingFunction(pNode, "MAR1_00002+ ");
			m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));
			if (m_pNthOneChar != NULL && strcmp(m_pNthOneChar, "��") == 0)
			{//2y : S(1)�� ���̡��ΰ� ? MAR1_00002
				LoggingFunction(pNode, "MAR1_00003+ ");

				//3y : ��� MAR1_00003
				GoMAR1_1(pNode, nStart, mStart, lStart);
			}
			else 
			{
				LoggingFunction(pNode, "MAR1_00004+ ");
				m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);
				if (strcmp(m_pJongChar, "��") == 0)
				{//3n : C(1)�� �������ΰ� ? MAR1_00004
					LoggingFunction(pNode, "MAR1_00005+ ");
					//4y : stem = S(len~1), ending = ������+[v][aff][unc][fut]  MAR1_00005
					m_pFormNTo1Word = GetFromFirstToStartWord(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));
					InsertStemString(pNode, m_pFormNTo1Word);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, "��");
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, ENDING_DIC_V);
					AddHTokenEnding(pNode, ENDING_DIC_AFF);
					AddHTokenEnding(pNode, ENDING_DIC_UNC);
					AddHTokenEnding(pNode, ENDING_DIC_FUT);
				}
				else
				{
					LoggingFunction(pNode, "MAR1_00006+ ");
					//4n : stem = S(len~1), ending = ���+[v][aff][con]   MAR1_00006
					m_pFormNTo1Word = GetFromFirstToStartWord(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));
					InsertStemString(pNode, m_pFormNTo1Word);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, "��");
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, ENDING_DIC_V);
					AddHTokenEnding(pNode, ENDING_DIC_AFF);
					AddHTokenEnding(pNode, ENDING_DIC_CON);
				}
			}
		}
		else 
		{
			LoggingFunction(pNode, "MAR1_00007+ ");
			//2n : stem = S(len~1), ending = null MAR1_00007
			m_pFormNTo1Word = GetFromFirstToStartWord(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));
			InsertStemString(pNode, m_pFormNTo1Word);
			SaveHTokenStem(pNode);
		}
	}
	else
	{
		//GO MAR1_1
		GoMAR1_1(pNode, nStart, mStart, lStart);
	}

	IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
	AllInitMemberVariable();
	InitRealOriginalString();

	
}

void CNAHangulAnaly::GoMAR1_1(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ

	//1n : �� n++ �� n > len ? MAR1_1_00001
	if (m_bIsFirstWord == false) 
	{
		LoggingFunction(pNode, "MAR1_1_00001+ ");
		MovingLeftPositionWord(pNode->pHToken->strSource, nStart);
		InitOneWordMarsk();
	}
	m_bIsFirstWord = false;

	if (nStart <= 0) 
	{
		LoggingFunction(pNode, "MAR1_1_00002+ ");
		GoErrorFunction(pNode);//1'y : Error MAR1_1_00002
	}
	else 
	{
		m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)
		m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG); //C(n)
		
		LoggingFunction(pNode, "MAR1_1_00003+ ");
		if((m_bOneCharMask == false || strcmp(m_pNthOneChar, "��") != 0)//1'n : S(n)�� [mask]||�������� �ƴϰ�, {S(n) == txx  ||���⡱||���䡱||������|| C(n) == ������||������||������}�ΰ�? MAR1_1_00003
			&&(CompareVariousTable01(m_pNthOneChar, 0, false) == true
				|| CompareVariousTable02(m_pNthOneChar, 1, false) == true || CompareVariousTable02(m_pNthOneChar, 2, false) == true
				|| CompareVariousTable03(m_pNthOneChar, 0, false) == true || CompareVariousTable03(m_pNthOneChar, 1, false) == true
				|| CompareVariousTable04(m_pNthOneChar, 1, false) == true || CompareVariousTable04(m_pNthOneChar, 2, false) == true || CompareVariousTable04(m_pNthOneChar, 3, false) == true
				|| CompareVariousTable05(m_pNthOneChar, ALL_TABLE_05, true) == true
				|| CompareVariousTable06(m_pNthOneChar, ALL_TABLE_06, true) == true
				|| CompareVariousTable07(m_pNthOneChar, ALL_TABLE_07, true) == true
				|| strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0
				|| strcmp(m_pJongChar, "��") == 0 || strcmp(m_pJongChar, "��") == 0 || strcmp(m_pJongChar, "��") == 0))
		{
			LoggingFunction(pNode, "MAR1_1_00004+ ");
			if (strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0
				|| strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "ŭ") == 0)
			{//2y : S(n) == ���١� || ���ߡ� || ������ || ���̡� || ������ || ���͡� || ������ || ������ || ��ŭ���ΰ� ? MAR1_1_00004
				LoggingFunction(pNode, "MAR1_1_00005+ ");

				GoMAR3(pNode, nStart, mStart, lStart);
			}
			else
			{
				LoggingFunction(pNode, "MAR1_1_00006+ ");

				GoIMR1(pNode, nStart, mStart, lStart);
			}
		}
		else 
		{
			GoMAR1_2(pNode, nStart, mStart, lStart);
		}
	}
	
	IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
}

void CNAHangulAnaly::GoMAR1_2(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ

	LoggingFunction(pNode, "MAR1_2_00001+ ");
	m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
	if (IsExistWildEndingDic(m_pFormNTo1Word) == true)
	{//2n : �� *S(n~1)�� ending ������ �ִ°� ? MAR1_2_00001
		LoggingFunction(pNode, "MAR1_2_00002+ ");
		mStart = nStart;
		if (IsExistEndingDic(m_pFormNTo1Word, m_endingFeature01))
		{//3y : m = n �� S(n~1)�� ending ������ �ִ°� ? MAR1_2_00002
			LoggingFunction(pNode, "MAR1_2_00003+ ");
			m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
			m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
			if (IsExistStemDic(m_pStemDicWord, m_stemFeature01))
			{//4y : S(len~n+1)�� stem ������ �ִ°� ? MAR1_2_00003
				LoggingFunction(pNode, "MAR1_2_00004+ ");
				if (strcmp(m_pStemDicWord, "�ڽ�") == 0 || strcmp(m_pStemDicWord, "��") == 0
					|| strcmp(m_pStemDicWord, "��") == 0 || strcmp(m_pStemDicWord, "Ʋ") == 0
					|| strcmp(m_pStemDicWord, "��") == 0 || strcmp(m_pStemDicWord, "Ǯ") == 0
					|| strcmp(m_pStemDicWord, "��") == 0 || strcmp(m_pStemDicWord, "��") == 0
					|| strcmp(m_pStemDicWord, "��") == 0 || strcmp(m_pStemDicWord, "��") == 0
					|| strcmp(m_pStemDicWord, "��") == 0 || strcmp(m_pStemDicWord, "��") == 0
					|| strcmp(m_pStemDicWord, "��") == 0 || strcmp(m_pStemDicWord, "��") == 0
					|| strcmp(m_pStemDicWord, "��") == 0 || strcmp(m_pStemDicWord, "��") == 0
					|| strcmp(m_pStemDicWord, "�㹰") == 0 || strcmp(m_pStemDicWord, "��") == 0)
				{//5y : {S(len~n+1) == ���ڽš� || ���桱 || ���ȡ� || ��Ʋ�� || ������ | ��Ǯ�� || ������ || ������ || ���� || ���ޡ� || ���ҡ� || ���족 || ������ || ���ˡ� || ��� || ������ || ���㹰�� || ������ || ���ȡ�}�ΰ� ? MAR1_2_00004
					LoggingFunction(pNode, "MAR1_2_00005+ ");
					//6y : stem = S(len~n+1), ending = S(n~1) MAR1_2_00005
					InsertStemString(pNode, m_pStemDicWord);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_pFormNTo1Word);
					SaveHTokenEnding(pNode);
				}
				else
				{
					m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//n+1�� ����
					m_pJongChar = GetChojungJongsungChar(m_pOneNPluseNumChar, CHAR_JONGSUNG);//C(n+1)�� ����
					m_pEndingDicWord = AddTwoStr(m_pJongChar, m_pFormNTo1Word);//C(n+1)+S(n~1)
					LoggingFunction(pNode, "MAR1_2_00006+ ");
					if(IsExistEndingDic(m_pEndingDicWord, m_endingFeature02))
					{//6n : C(n+1)+S(n ~ 1)�� ending ������ �ִ°� ? MAR1_2_00006
						LoggingFunction(pNode, "MAR1_2_00007+ ");
						m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pOneNPluseNumChar, m_nPluseSyllable);
						if (IsExistStemDic(m_pStemDicDeleteJongWord, m_stemFeature02))
						{//7y : (S(len~n+1)-C(n+1))�� stem ������ �ִ°� ? MAR1_2_00007
							LoggingFunction(pNode, "MAR1_2_00008+ ");
							if (strcmp(m_pStemDicDeleteJongWord, "�ڽ�") != 0
								&&((m_stemFeature02[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature02[ENDING_DIC_N] == IS_CEHCK_FEATURE)
								|| (m_stemFeature02[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature02[ENDING_DIC_N] != IS_CEHCK_FEATURE)
								|| (m_stemFeature02[STEM_DIC_AMB] == IS_CEHCK_FEATURE && m_endingFeature02[ENDING_DIC_AMB] == IS_CEHCK_FEATURE)
							)) 
							{//8y : {(S(len~n+1)-C(n+1)) != :�ڽ�: || : ���� : }�̰�, { (S(len~n+1)-C(n+1)) == [N] && C(n+1)+S(n~1) == [N] }�̰ų�, { (S(len~n+1)-C(n+1)) <>[N] && C(n+1)+S(n~1) <>[N] }�̰ų�, { (S(len~n+1)-C(n+1)) == [amb] || C(n+1)+S(n~1) == [amb] }�ΰ� ? MAR1_2_00008
								LoggingFunction(pNode, "MAR1_2_00009+ ");
								m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//n�� ����
								m_p1thAddString = AddTwoStr(m_pJongChar, m_pNthOneChar);//C(n+1)+S(n)
								m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);//p1thAddString+OldBuf()
								//9y : stem = S(len~n+1)-C(n+1), ending = C(n+1)+S(n)+OldBuf() MAR1_2_00009
								InsertStemString(pNode, m_pStemDicDeleteJongWord);
								SaveHTokenStem(pNode);
								InsertEndingString(pNode, m_p2thAddString);
								SaveHTokenEnding(pNode);
							}
							else
							{
								LoggingFunction(pNode, "MAR1_2_00010+ ");
								if ((m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] == IS_CEHCK_FEATURE)
									|| (m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] != IS_CEHCK_FEATURE)
									|| (m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_AMB] == IS_CEHCK_FEATURE))
								{//9n : {S(len~n+1) == [N] && S(n~1) == [N]}�̰ų�, { S(len~n+1) <>[N] && S(n~1) <>[N] }�̰ų�, { S(len~n+1) == [amb] || S(n~1) == [amb] }�ΰ� ? MAR1_2_00010
									LoggingFunction(pNode, "MAR1_2_00011+ ");
									//10y : stem = S(len~n+1), ending = S(n~1)  MAR1_2_00011
									InsertStemString(pNode, m_pStemDicWord);
									SaveHTokenStem(pNode);
									InsertEndingString(pNode, m_pFormNTo1Word);
									SaveHTokenEnding(pNode);
								}
								else
								{
									LoggingFunction(pNode, "MAR1_2_00012+ ");

									//10n : error  MAR1_2_00012
									GoErrorFunction(pNode);
								}
							}
						}
						else
						{
							LoggingFunction(pNode, "MAR1_2_00013+ ");
							//8n : stem = S(len~n+1), ending = S(n~1) MAR1_2_00013
							InsertStemString(pNode, m_pStemDicWord);
							SaveHTokenStem(pNode);
							InsertEndingString(pNode, m_pFormNTo1Word);
							SaveHTokenEnding(pNode);
						}
					}
					else
					{
						IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
						m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
						m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)�� ����
						m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
						m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
						m_pEndingDicWord = AddTwoStr(m_pJongChar, m_pFormNTo1Word);//C(n)+S(n-1~1)
						LoggingFunction(pNode, "MAR1_2_00014+ ");
						if (IsExistEndingDic(m_pEndingDicWord, m_endingFeature01))
						{//7n : C(n)+S(n-1~1)�� ending ������ �ִ°�? MAR1_2_00014
							LoggingFunction(pNode, "MAR1_2_00015+ ");
							m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
							m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
							if (IsExistStemDic(m_pStemDicDeleteJongWord, m_stemFeature01))
							{//8y : S(len~n)- #C(n) �� stem ������ �ִ°�? MAR1_2_00015
								LoggingFunction(pNode, "MAR1_2_00016+ ");
								if ((m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] == IS_CEHCK_FEATURE)
									|| (m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] != IS_CEHCK_FEATURE)
									|| (m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_AMB] == IS_CEHCK_FEATURE))
								{//9y : { S(len~n)- #C(n) == [N] && C(n)+S(n-1~1) == [N]}�̰ų�, { S(len~n)- #C(n) <> [N] && C(n)+S(n-1~1) <> [N]}�̰ų�, { S(len~n)- #C(n) == [amb] || C(n)+S(n-1~1) == [amb]} MAR1_2_00016
									LoggingFunction(pNode, "MAR1_2_00017+ ");
									//10y : stem = S(len~n)-C(n), ending = C(n)+S(n-1~1) MAR1_2_00017
									InsertStemString(pNode, m_pStemDicDeleteJongWord);
									SaveHTokenStem(pNode);
									InsertEndingString(pNode, m_pEndingDicWord);
									SaveHTokenEnding(pNode);
								}
								else
								{
									LoggingFunction(pNode, "MAR1_2_00018+ ");
									IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
									m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
									m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
									m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
									//10n : stem = S(len~n+1), ending = S(n~1) MAR1_2_00018
									InsertStemString(pNode, m_pStemDicWord);
									SaveHTokenStem(pNode);
									InsertEndingString(pNode, m_pFormNTo1Word);
									SaveHTokenEnding(pNode);
								}
							}
							else
							{
								LoggingFunction(pNode, "MAR1_2_00019+ ");
								if (strcmp(m_pStemDicWord, "����") == 0)
								{//9n : {S(len~n) == :����:}�ΰ�? MAR1_2_00019
									LoggingFunction(pNode, "MAR1_2_00020+ ");
									//10y : stem = S(len~n), ending = OldBuf() MAR1_2_00020
									InsertStemString(pNode, m_pStemDicWord);
									SaveHTokenStem(pNode);
									InsertEndingString(pNode, m_pOldBuffer);
									SaveHTokenEnding(pNode);
								}
								else
								{
									IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
									m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
									m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)�� ����
									m_nRestorationJongChar = Restoration(m_pJongChar);//�����ؾ� �� ������
									ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);//C(n) = ������
									m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
									bool bFind = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
									LoggingFunction(pNode, "MAR1_2_00021+ ");
									if (bFind == true && m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE)
									{//10n : C(n) = ������ �� S(len~n)�� stem ������ �ְ�, S(len~n) != [n]�ΰ�? MAR1_2_00021
										LoggingFunction(pNode, "MAR1_2_00022+ ");
										//11y : stem = S(len~n), ending = ������+OldBuf() MAR1_2_00022
										InsertStemString(pNode, m_pStemDicWord);
										SaveHTokenStem(pNode);
										m_p1thAddString = AddTwoStr("��", m_pOldBuffer);
										InsertEndingString(pNode, m_p1thAddString);
										SaveHTokenStem(pNode);
									}
									else
									{
										//������ ���������� ����
										JasoRestoration(pNode, nStart, FORCE_REPLACE_ENDWORD, m_nRestorationJongChar);// C(n) = #C(n)
										IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
										LoggingFunction(pNode, "MAR1_2_00023+ ");
										m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
										m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
										m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
										bool bFindStemWord = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
										bool bFindEndingWord = IsExistEndingDic(m_pFormNTo1Word, m_endingFeature01);
										if (bFindStemWord == true && bFindEndingWord == true && m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_PPO] == IS_CEHCK_FEATURE)
										{//11n : C(n) = #C(n) �� {S(len~n+1) == [N] && S(n~1) == [Ppo]}�ΰ�? MAR1_2_00023
											LoggingFunction(pNode, "MAR1_2_00024+ ");
											m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
											m_p1thAddString = AddTwoStr("��", m_pNthOneChar);
											m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
											//12y : stem = S(len~n+1), ending = ���̡�[Pol]+S(n)+OldBuf() MAR1_2_00024
											InsertStemString(pNode, m_pStemDicWord);
											SaveHTokenStem(pNode);
											InsertEndingString(pNode, m_p2thAddString);
											SaveHTokenEnding(pNode);
											AddHTokenEnding(pNode, ENDING_DIC_POL);//[Pol]�����ο�
										}
										else
										{
											LoggingFunction(pNode, "MAR1_2_00025+ ");
											if ((m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] == IS_CEHCK_FEATURE)
												|| (m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] != IS_CEHCK_FEATURE)
												|| (m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_AMB] == IS_CEHCK_FEATURE))
											{//12n : C(n) = #C(n) �� {S(len~n+1) == [N] && S(n~1) == [N]}�̰ų�, {S(len~n+1) <> [N] && S(n~1) <> [N]}�̰ų� {S(len~n+1) == [amb] || S(n~1) == [amb]}�ΰ�? MAR1_2_00025
												LoggingFunction(pNode, "MAR1_2_00026+ ");
												m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
												m_p1thAddString = AddTwoStr(m_pNthOneChar, m_pOldBuffer);
												//13y : stem = S(len~n+1), ending = S(n)+OldBuf() MAR1_2_00026
												InsertStemString(pNode, m_pStemDicWord);
												SaveHTokenStem(pNode);
												InsertEndingString(pNode, m_p1thAddString);
												SaveHTokenEnding(pNode);
											}
											else
											{
												LoggingFunction(pNode, "MAR1_2_00027+ ");
												CHAR_DELETE(m_pOldBuffer);
												//13n : OldBuf() = S(n~1) �� ��� MAR1_2_00027
												int nLen = strlen(m_pFormNTo1Word);
												m_pOldBuffer = new char[nLen+1];
												memset(m_pOldBuffer, 0x00, nLen+1);
												strcpy(m_pOldBuffer, m_pFormNTo1Word);
												InsertArrayFeatureOld(m_pFormNTo1Word);

												GoMAR1_1(pNode, nStart, mStart, lStart);
											}
										}
									}
								}
							}
						}
						else
						{
							IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
							m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
							m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
							m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
							m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)�� ����
							bool bFindStemWord = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
							bool bFindEndingWord = IsExistEndingDic(m_pNthOneChar, m_endingFeature01);
							LoggingFunction(pNode, "MAR1_2_00028+ ");
							if ((bFindStemWord == true && bFindEndingWord == true) 
								&&((m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] == IS_CEHCK_FEATURE) || (m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] != IS_CEHCK_FEATURE) || (m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_AMB] == IS_CEHCK_FEATURE)))
							{//8n : {S(len~n+1) == [N] && S(n) == [N]}�̰ų�, {S(len~n+1) <> [N] && S(n) <> [N]}�̰ų�, {S(len~n+1) == [amb] || S(n) == [amb]}�ΰ�? MAR1_2_00028
								m_pLastOneChar = GetOneChar(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));//S(1)
								m_pFormLenToNWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
								LoggingFunction(pNode, "MAR1_2_00029+ ");
								if (strcmp(m_pNthOneChar, m_pLastOneChar) == 0
									&& (strcmp(m_pFormLenToNWord, "�־���") == 0 || strcmp(m_pFormLenToNWord, "����") == 0 || strcmp(m_pFormLenToNWord, "�ڱ�") == 0
									|| strcmp(m_pFormLenToNWord, "�ű�") == 0 || strcmp(m_pFormLenToNWord, "�ű�") == 0 || strcmp(m_pFormLenToNWord, "�鸦") == 0
									|| strcmp(m_pFormLenToNWord, "�̱�") == 0 || strcmp(m_pFormLenToNWord, "����") == 0 || strcmp(m_pFormLenToNWord, "�ñ�") == 0
									|| strcmp(m_pFormLenToNWord, "����") == 0 || strcmp(m_pFormLenToNWord, "����") == 0 || strcmp(m_pFormLenToNWord, "��") == 0
									|| strcmp(m_pFormLenToNWord, "�θ�") == 0 || strcmp(m_pFormLenToNWord, "���") == 0 || strcmp(m_pFormLenToNWord, "������") == 0
									|| strcmp(m_pFormLenToNWord, "����") == 0 || strcmp(m_pFormLenToNWord, "����") == 0 || strcmp(m_pFormLenToNWord, "�̾���") == 0
									|| strcmp(m_pFormLenToNWord, "���") == 0 || strcmp(m_pFormLenToNWord, "�ñ�") == 0 || strcmp(m_pFormLenToNWord, "����") == 0
									|| strcmp(m_pFormLenToNWord, "����") == 0 || strcmp(m_pFormLenToNWord, "�˷���") == 0
									)) 
								{//9y : S(n) == S(1)�̰�, S(len~n) == ���־�����|| ��������|| ���ڱ桱|| ���ű桱|| ���ű䡱|| ���鸦��|| ���̱桱|| ��������|| ���ñ䡱|| ��������|| ��������|| ���𸦡�|| ���θ��� || ����桱|| ����������|| ��������|| ��������|| ���̾�����|| ����䡱|| ���ñ桱|| ��������|| �����䡱|| ���˷������ΰ�? MAR1_2_00029
									LoggingFunction(pNode, "MAR1_2_00030+ ");
									m_pStemDicDeleteJongWord = MinusTwoStr(m_pFormLenToNWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
									//10y : stem = S(len~n)-C(n), ending = C(n)+OldBuf() MAR1_2_00030
									InsertStemString(pNode, m_pStemDicDeleteJongWord);
									SaveHTokenStem(pNode);
									m_p1thAddString = AddTwoStr(m_pJongChar, m_pOldBuffer);
									InsertEndingString(pNode, m_p1thAddString);
									SaveHTokenEnding(pNode);
								}
								else
								{
									LoggingFunction(pNode, "MAR1_2_00031+ ");
									if (strcmp(m_pNthOneChar, m_pLastOneChar) == 0
										&& (strcmp(m_pFormLenToNWord, "����") == 0 || strcmp(m_pFormLenToNWord, "���") == 0
										|| strcmp(m_pFormLenToNWord, "����") == 0 || strcmp(m_pFormLenToNWord, "����") == 0))
									{//10n : S(n) == S(1)�̰�, S(len~n) == �����硱||����硱||�����硱||�����硱 �ΰ�? MAR1_2_00031
										LoggingFunction(pNode, "MAR1_2_00032+ ");
										IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
										//11y : stem = S(len~n)-C(n)+������, ending = ����" MAR1_2_00032
										ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);//S(len~n)-C(n)+������
										m_pFormLenToNWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
										InsertStemString(pNode, m_pFormLenToNWord);
										SaveHTokenStem(pNode);
										InsertEndingString(pNode, "��");
										SaveHTokenEnding(pNode);
									}
									else
									{
										LoggingFunction(pNode, "MAR1_2_00033+ ");
										IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
										m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
										m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
										m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
										bool bFindStemWord = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
										bool bFindEndingWord = IsExistEndingDic(m_pFormNTo1Word, m_endingFeature01);
										if (bFindStemWord == true && bFindEndingWord == true && m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_PPO] == IS_CEHCK_FEATURE)
										{//11n : S(len~n+1) == [N] && S(n~1) == [Ppo]�ΰ�? MAR1_2_00033
											LoggingFunction(pNode, "MAR1_2_00034+ ");
											//12y : stem = S(len~n+1), ending = S(n~1) MAR1_2_00034
											InsertStemString(pNode, m_pStemDicWord);
											SaveHTokenStem(pNode);
											InsertEndingString(pNode, m_pFormNTo1Word);
											SaveHTokenEnding(pNode);
										}
										else
										{
											LoggingFunction(pNode, "MAR1_2_00035+ ");
											//12n : stem = S(len~n+1), ending = S(n)+OldBuf() MAR1_2_00035
											InsertStemString(pNode, m_pStemDicWord);
											SaveHTokenStem(pNode);
											m_p2thAddString = AddTwoStr(m_pNthOneChar, m_pOldBuffer);
											InsertEndingString(pNode, m_p2thAddString);
											SaveHTokenEnding(pNode);
										}
									}
								}
							}
							else 
							{
								IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
								m_pLastOneChar = GetOneChar(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));//S(1)
								m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
								LoggingFunction(pNode, "MAR1_2_00036+ ");
								if(strcmp(m_pNthOneChar, m_pLastOneChar) == 0)
								{//9n : S(n) == S(1)�ΰ�? MAR1_2_00036
									m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)�� ����
									LoggingFunction(pNode, "MAR1_2_00037+ ");
									if(strcmp(m_pJongChar, "��") == 0 || strcmp(m_pJongChar, "��") == 0)
									{ //10y :  C(n) == ������ || �������ΰ� ? MAR1_2_00037 
										m_pFormLenToNWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
										m_pStemDicDeleteJongWord = MinusTwoStr(m_pFormLenToNWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
										LoggingFunction(pNode, "MAR1_2_00038+ ");
										if (IsExistStemDic(m_pStemDicDeleteJongWord, m_stemFeature01))
										{//11y : S(len~n)-C(n) �� stem ������ �ִ°�? MAR1_2_00038
											LoggingFunction(pNode, "MAR1_2_00039+ ");
											//12y: stem = S(len ~ n)-C(n), ending = ������ || ������ MAR1_2_00039
											InsertStemString(pNode, m_pStemDicDeleteJongWord);
											SaveHTokenStem(pNode);
											InsertEndingString(pNode, m_pJongChar);
											SaveHTokenEnding(pNode);
										}
										else
										{
											LoggingFunction(pNode, "MAR1_2_00040+ ");
											if (strcmp(m_pJongChar, "��") == 0)
											{//12n : C(n) == �������ΰ�? MAR1_2_00040
												IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
												ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);//C(n) = ������->S(len~n)
												m_pFormLenToNWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
												LoggingFunction(pNode, "MAR1_2_00041+ ");
												if (IsExistStemDic(m_pFormLenToNWord, m_stemFeature01))
												{//13y : C(n) = ������->S(len~n) �� stem ������ �ִ°�? MAR1_2_00041
													LoggingFunction(pNode, "MAR1_2_00042+ ");
													if (strcmp(m_pFormLenToNWord, "������") == 0)
													{//14y : S(len~n)�� �������ϡ��ΰ�? MAR1_2_00042
														LoggingFunction(pNode, "MAR1_2_00043+ ");
														//15y : stem = S(len ~ n+1), ending=����"+[Pol]+������ MAR1_2_00043
														m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
														m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
														m_p1thAddString = AddTwoStr("��", "��");
														InsertStemString(pNode, m_pStemDicWord);
														SaveHTokenStem(pNode);
														InsertEndingString(pNode, m_p1thAddString);
														SaveHTokenEnding(pNode);
														AddHTokenEnding(pNode, ENDING_DIC_POL);
													}
													else
													{
														LoggingFunction(pNode, "MAR1_2_00044+ ");
														//15n : stem = S(len ~ n), ending = ������ MAR1_2_00044
														InsertStemString(pNode, m_pFormLenToNWord);
														SaveHTokenStem(pNode);
														InsertEndingString(pNode, "��");
														SaveHTokenEnding(pNode);
													}
												}
												else
												{
													IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
													JasoRestoration(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA01);//C(n) = ������
													m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
													m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
													m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
													bool bFindStemWord = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
													bool bFindEndingWord = IsExistEndingDic(m_pFormNTo1Word, m_endingFeature01);
													LoggingFunction(pNode, "MAR1_2_00045+ ");
													if (bFindStemWord == true && bFindEndingWord == true && m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_PPO] == IS_CEHCK_FEATURE)
													{//14n : C(n) = ������ �� {S(len~n+1) == [N] && S(n~1) == [Ppo]}�ΰ�? MAR1_2_00045
														LoggingFunction(pNode, "MAR1_2_00046+ ");
														m_pLastOneChar = GetOneChar(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));//S(1)
														m_p1thAddString = AddTwoStr("��", m_pLastOneChar);
														m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
														//15y : stem = S(len~n+1), ending= ���̡�[Pol]+S(1)+OldBuf() MAR1_2_00046
														InsertStemString(pNode, m_pStemDicWord);
														SaveHTokenStem(pNode);
														InsertEndingString(pNode, m_p2thAddString);
														SaveHTokenEnding(pNode);
														AddHTokenEnding(pNode, ENDING_DIC_POL);//[Pol]�����ο�
													}
													else
													{
														LoggingFunction(pNode, "MAR1_2_00047+ ");
														if ((bFindStemWord == true && bFindEndingWord == true)
															&& ((m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] == IS_CEHCK_FEATURE)
																|| (m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] != IS_CEHCK_FEATURE)
																|| (m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_AMB] == IS_CEHCK_FEATURE)))
														{//15n : C(n) = ������ �� {S(len~n+1) == [N] && S(n~1) == [N]}�̰ų�, {S(len~n+1) <> [N] && S(n~1) <> [N]}�̰ų� {S(len~n+1) == [amb] || S(n~1) == [amb]}�ΰ�? MAR1_2_00047
															LoggingFunction(pNode, "MAR1_2_00048+ ");
															m_pLastOneChar = GetOneChar(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));//S(1)
															m_p1thAddString = AddTwoStr(m_pLastOneChar, m_pOldBuffer);
															//16y : stem = S(len~n+1), ending=S(1)+OldBuf() MAR1_2_00048
															InsertStemString(pNode, m_pStemDicWord);
															SaveHTokenStem(pNode);
															InsertEndingString(pNode, m_p1thAddString);
															SaveHTokenEnding(pNode);
														}
														else
														{
															LoggingFunction(pNode, "MAR1_2_00049+ ");
															CHAR_DELETE(m_pOldBuffer);
															//16n : OldBuf() = S(n~1) �� ��� MAR1_2_00049
															int nLen = strlen(m_pFormNTo1Word);
															m_pOldBuffer = new char[nLen+1];
															memset(m_pOldBuffer, 0x00, nLen+1);
															strcpy(m_pOldBuffer, m_pFormNTo1Word);
															InsertArrayFeatureOld(m_pFormNTo1Word);

															GoMAR1_1(pNode, nStart, mStart, lStart);
														}
													}
												}
											}
											else
											{
												LoggingFunction(pNode, "MAR1_2_00050+ ");
												IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
												m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
												m_pFormLenToNWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
												m_pLastOneChar = GetOneChar(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));//S(1)
												m_p1thAddString = AddTwoStr(m_pLastOneChar, m_pOldBuffer);
												//13n : Stem = S(len~n+1), ending = S(1)+OldBuf() MAR1_2_00050
												InsertStemString(pNode, m_pFormLenToNWord);
												SaveHTokenStem(pNode);
												InsertEndingString(pNode, m_p1thAddString);
												SaveHTokenEnding(pNode);
											}
										}
									}
									else
									{
										LoggingFunction(pNode, "MAR1_2_00051+ ");
										IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
										m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
										m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
										m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
										bool bFindStemWord = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
										bool bFindEndingWord = IsExistEndingDic(m_pFormNTo1Word, m_endingFeature01);
										if ((bFindStemWord == true && bFindEndingWord == true) && (m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_PPO] == IS_CEHCK_FEATURE))
										{//11n : {S(len~n+1) == [N] && S(n~1) == [Ppo]}�ΰ�? MAR1_2_00051
											
											LoggingFunction(pNode, "MAR1_2_00052+ ");
											CHAR_DELETE(m_pLastOneChar);
											m_pLastOneChar = GetOneChar(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));//S(1)
											m_p1thAddString = AddTwoStr("��", m_pLastOneChar);
											m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
											//12y : stem = S(len~n+1), ending =����"[Pol]+S(1)+OldBuf() MAR1_2_00052
											InsertStemString(pNode, m_pStemDicWord);
											SaveHTokenStem(pNode);
											InsertEndingString(pNode, m_p2thAddString);
											SaveHTokenEnding(pNode);
											AddHTokenEnding(pNode, ENDING_DIC_POL);
										}
										else
										{
											LoggingFunction(pNode, "MAR1_2_00054+ ");
											if ((bFindStemWord == true && bFindEndingWord == true)
												&& ((m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] == IS_CEHCK_FEATURE)
													|| (m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] != IS_CEHCK_FEATURE)
													|| (m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_AMB] == IS_CEHCK_FEATURE)))
											{//12n : {S(len~n+1) == [N] && S(n~1) == [N]}�̰ų�, {S(len~n+1) <> [N] && S(n~1) <> [N]}�̰ų� {S(len~n+1) == [amb] || S(n~1) == [amb]}�ΰ�? MAR1_2_00053
												LoggingFunction(pNode, "MAR1_2_00054+ ");
												m_pLastOneChar = GetOneChar(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));//S(1)
												m_p1thAddString = AddTwoStr(m_pLastOneChar, m_pOldBuffer);
												//13y : stem = S(len~n+1), ending = S(1)+OldBuf() MAR1_2_00054
												InsertStemString(pNode, m_pStemDicWord);
												SaveHTokenStem(pNode);
												InsertEndingString(pNode, m_p1thAddString);
												SaveHTokenEnding(pNode);
											}
											else
											{
												LoggingFunction(pNode, "MAR1_2_00055+ ");
												CHAR_DELETE(m_pOldBuffer);
												//13n : OldBuf() = S(n~1) �� ��� MAR1_2_00055
												int nLen = strlen(m_pFormNTo1Word);
												m_pOldBuffer = new char[nLen+1];
												memset(m_pOldBuffer, 0x00, nLen+1);
												strcpy(m_pOldBuffer, m_pFormNTo1Word);
												InsertArrayFeatureOld(m_pFormNTo1Word);

												GoMAR1_1(pNode, nStart, mStart, lStart);
											}
										}
									}
								}
								else
								{
									IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
									m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
									m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
									m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
									bool bFindStemWord = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
									bool bFindEndingWord = IsExistEndingDic(m_pFormNTo1Word, m_endingFeature01);
									LoggingFunction(pNode, "MAR1_2_00056+ ");
									if ((bFindStemWord == true && bFindEndingWord == true) && (m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_PPO] == IS_CEHCK_FEATURE))
									{//10n : {S(len~n+1) == [N] && S(n~1) == [Ppo]}�ΰ�? MAR1_2_00056
										LoggingFunction(pNode, "MAR1_2_00057+ ");
										m_p1thAddString = AddTwoStr("��", m_pFormNTo1Word);
										//11y : stem = S(len~n+1), ending = ����"[Pol]+S(n~1) MAR1_2_00057
										InsertStemString(pNode, m_pStemDicWord);
										SaveHTokenStem(pNode);
										InsertEndingString(pNode, m_p1thAddString);
										SaveHTokenEnding(pNode);
									}
									else
									{
										LoggingFunction(pNode, "MAR1_2_00058+ ");
										if((bFindStemWord == true && bFindEndingWord == true)
											&& ((m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] == IS_CEHCK_FEATURE)
												|| (m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] != IS_CEHCK_FEATURE)
												|| (m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_AMB] == IS_CEHCK_FEATURE)))
										{//11n : {S(len~n+1) == [N] && S(n~1) == [N]}�̰ų�, {S(len~n+1) <> [N] && S(n~1) <> [N]}�̰ų� {S(len~n+1) == [amb] || S(n~1) == [amb]}�ΰ�? MAR1_2_00058
											LoggingFunction(pNode, "MAR1_2_00059+ ");
											//12y : stem = S(len~n+1), ending = S(n~1) MAR1_2_00059
											InsertStemString(pNode, m_pStemDicWord);
											SaveHTokenStem(pNode);
											InsertEndingString(pNode, m_pFormNTo1Word);
											SaveHTokenEnding(pNode);
										}
										else
										{
											LoggingFunction(pNode, "MAR1_2_00060+ ");

											//12n : Error(20030128 ������ ���� ������������) MAR1_2_00060
											GoErrorFunction(pNode);
										}
									}
								}
							}
						}
					}
				}
			}
			else
			{
				IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
				m_pLastOneChar = GetOneChar(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));//S(1)
				m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
				LoggingFunction(pNode, "MAR1_2_00061+ ");
				if (strcmp(m_pNthOneChar, m_pLastOneChar) == 0)
				{//5n : S(n) == S(1)�ΰ�? MAR1_2_00061
					m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)�� ����
					LoggingFunction(pNode, "MAR1_2_00062+ ");
					if (strcmp(m_pJongChar, "��") == 0 || strcmp(m_pJongChar, "��") == 0)
					{//6y :  C(n) == ������ || �������ΰ�? MAR1_2_00062
						m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
						m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
						LoggingFunction(pNode, "MAR1_2_00063+ ");
						if (IsExistStemDic(m_pStemDicDeleteJongWord))
						{//7y : S(len~n)-C(n) �� stem ������ �ִ°�? MAR1_2_00063
							LoggingFunction(pNode, "MAR1_2_00064+ ");
							//8y: stem = S(len ~ n)-C(n), ending = ������ || ������  MAR1_2_00064
							InsertStemString(pNode, m_pStemDicDeleteJongWord);
							SaveHTokenStem(pNode);
							InsertEndingString(pNode, m_pJongChar);
							SaveHTokenEnding(pNode);
						}
						else
						{
							LoggingFunction(pNode, "MAR1_2_00065+ ");
							if (strcmp(m_pJongChar, "��") == 0)
							{//8n : C(n) == �������ΰ�? MAR1_2_00065
								CHAR_DELETE(m_pStemDicWord);
								m_nRestorationJongChar = Restoration(m_pJongChar);//�����ؾ� �� ������
								ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);//C(n) = ������
								m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
								LoggingFunction(pNode, "MAR1_2_00066+ ");
								if (IsExistStemDic(m_pStemDicWord))
								{//9y : C(n) = ������ ->  S(len~n) �� stem ������ �ִ°�? MAR1_2_00066
									LoggingFunction(pNode, "MAR1_2_00067+ ");
									//10y : stem = S(len ~ n), ending = ������  MAR1_2_00067
									InsertStemString(pNode, m_pStemDicWord);
									SaveHTokenStem(pNode);
									InsertEndingString(pNode, "��");
									SaveHTokenEnding(pNode);
								}
								else
								{
									LoggingFunction(pNode, "MAR1_2_00068+ ");
									//10n : C(n) = ������ -> OldBuf() = S(n~1) �� ��� MAR1_2_00068
									ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA01);//C(n) = ������
									m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
									CHAR_DELETE(m_pOldBuffer);
									int nLen = strlen(m_pFormNTo1Word);
									m_pOldBuffer = new char[nLen+1];
									memset(m_pOldBuffer, 0x00, nLen+1);
									strcpy(m_pOldBuffer, m_pFormNTo1Word);
									InsertArrayFeatureOld(m_pFormNTo1Word);

									GoMAR1_1(pNode, nStart, mStart, lStart);
								}
							}
							else
							{
								LoggingFunction(pNode, "MAR1_2_00069+ ");
								m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
								//9n : OldBuf() = S(n-1) �� ��� MAR1_2_00069
								CHAR_DELETE(m_pOldBuffer);
								int nLen = strlen(m_pFormNTo1Word);
								m_pOldBuffer = new char[nLen+1];
								memset(m_pOldBuffer, 0x00, nLen+1);
								strcpy(m_pOldBuffer, m_pFormNTo1Word);
								InsertArrayFeatureOld(m_pFormNTo1Word);

								GoMAR1_1(pNode, nStart, mStart, lStart);
							}
						}
					}
					else
					{
						IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
						m_pLastOneChar = GetOneChar(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));//S(1)
						m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
						LoggingFunction(pNode, "MAR1_2_00070+ ");
						if (m_pOldBuffer != NULL && strcmp(m_pOldBuffer, m_pLastOneChar) != 0)
						{//7n : OldBuf()�� S(1)�� ���� ������?(OldBuf() != S(1)) MAR1_2_00070
							LoggingFunction(pNode, "MAR1_2_00071+ ");
							//8y : OldBuf() = S(1)+OldBuf()�� ��� MAR1_2_00071
							m_p1thAddString = AddTwoStr(m_pLastOneChar, m_pOldBuffer);
							InsertArrayFeatureOld(m_pLastOneChar);
							CHAR_DELETE(m_pOldBuffer);
							int nLen = strlen(m_p1thAddString);
							m_pOldBuffer = new char[nLen+1];
							memset(m_pOldBuffer, 0x00, nLen+1);
							strcpy(m_pOldBuffer, m_p1thAddString);

							GoMAR1_1(pNode, nStart, mStart, lStart);
						}
						else
						{
							LoggingFunction(pNode, "MAR1_2_00072+ ");
							//8n : OldBuf() = S(n~1) �� ��� MAR1_2_00072
							CHAR_DELETE(m_pOldBuffer);
							int nLen = strlen(m_pFormNTo1Word);
							m_pOldBuffer = new char[nLen+1];
							memset(m_pOldBuffer, 0x00, nLen+1);
							strcpy(m_pOldBuffer, m_pFormNTo1Word);
							InsertArrayFeatureOld(m_pFormNTo1Word);

							GoMAR1_1(pNode, nStart, mStart, lStart);
						}
					}
				}
				else
				{
					IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
					m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
					m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)�� ����
					m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
					m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
					m_pEndingDicWord = AddTwoStr(m_pJongChar, m_pFormNTo1Word);//C(n)+S(n-1~1)
					LoggingFunction(pNode, "MAR1_2_00073+ ");
					if (IsExistEndingDic(m_pEndingDicWord, m_endingFeature01))
					{//6n : C(n)+S(n-1~1)�� ending������ �ִ°�? MAR1_2_00073
						m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
						m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
						LoggingFunction(pNode, "MAR1_2_00074+ ");
						if (IsExistStemDic(m_pStemDicDeleteJongWord, m_stemFeature01))
						{//7y : S(len~n)-C(n)�� stem������ �ִ°�? MAR1_2_00074
							LoggingFunction(pNode, "MAR1_2_00075+ ");
							if ((m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] == IS_CEHCK_FEATURE)
								|| (m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] != IS_CEHCK_FEATURE)
								|| (m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_AMB] == IS_CEHCK_FEATURE))
							{//8y : {(S(len~n)-C(n)) == [N] && C(n) +S(n-1~1) == [N]}�̰ų�, {(S(len~n)-C(n)) <> [N] && C(n) +S(n-1~1) <> [N]}�̰ų�  {(S(len~n)-C(n)) == [amb] || C(n) +S(n-1~1) == [amb]}�ΰ�? MAR1_2_00075
								LoggingFunction(pNode, "MAR1_2_00076+ ");
								//9y : Stem = S(len~n)-C(n), ending = C(n) +S(n-1~1) MAR1_2_00076
								InsertStemString(pNode, m_pStemDicWord);
								SaveHTokenStem(pNode);
								InsertEndingString(pNode, m_pEndingDicWord);
								SaveHTokenEnding(pNode);
							}
							else
							{
								LoggingFunction(pNode, "MAR1_2_00077+ ");
								CHAR_DELETE(m_pFormNTo1Word);
								//9n : OldBuf = S(n~1)  ��� MAR1_2_00077
								m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
								CHAR_DELETE(m_pOldBuffer);
								int nLen = strlen(m_pFormNTo1Word);
								m_pOldBuffer = new char[nLen+1];
								memset(m_pOldBuffer, 0x00, nLen+1);
								strcpy(m_pOldBuffer, m_pFormNTo1Word);
								InsertArrayFeatureOld(m_pFormNTo1Word);

								GoMAR1_1(pNode, nStart, mStart, lStart);
							}
						}
						else
						{
							LoggingFunction(pNode, "MAR1_2_00078+ ");
							CHAR_DELETE(m_pFormNTo1Word);
							//8n : OldBuf = S(n~1)  ��� MAR1_2_00078
							m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
							CHAR_DELETE(m_pOldBuffer);
							int nLen = strlen(m_pFormNTo1Word);
							m_pOldBuffer = new char[nLen+1];
							memset(m_pOldBuffer, 0x00, nLen+1);
							strcpy(m_pOldBuffer, m_pFormNTo1Word);
							InsertArrayFeatureOld(m_pFormNTo1Word);

							GoMAR1_1(pNode, nStart, mStart, lStart);
						}
					}
					else
					{
						IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
						m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
						bool bFindEndingWord = IsExistEndingDic(m_pFormNTo1Word, m_endingFeature01);
						LoggingFunction(pNode, "MAR1_2_00079+ ");
						if (bFindEndingWord == true && m_endingFeature01[ENDING_DIC_PRS] == IS_CEHCK_FEATURE)
						{//7n : S(n~1)�� [Prs]�ΰ�? MAR1_2_00079
							m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
							m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)
							LoggingFunction(pNode, "MAR1_2_00080+ ");
							if (strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0)
							{//8y : S(n+1) == ������ || ���á� || ���̡� || ���ϡ� || ���ǡ� || ������ || ���족�ΰ�? MAR1_2_00080
								LoggingFunction(pNode, "MAR1_2_00081+ ");
								//9y : OldBuf = S(n~1)  ��� MAR1_2_00081
								CHAR_DELETE(m_pOldBuffer);
								int nLen = strlen(m_pFormNTo1Word);
								m_pOldBuffer = new char[nLen+1];
								memset(m_pOldBuffer, 0x00, nLen+1);
								strcpy(m_pOldBuffer, m_pFormNTo1Word);
								InsertArrayFeatureOld(m_pFormNTo1Word);

								GoMAR1_1(pNode, nStart, mStart, lStart);
							}
							else
							{
								LoggingFunction(pNode, "MAR1_2_00082+ ");

								//9n : Error(��������, �Ǻΰ�����) MAR1_2_00082
								GoErrorFunction(pNode);
							}
						}
						else
						{
							LoggingFunction(pNode, "MAR1_2_00083+ ");
							//8n : OldBuf = S(n~1)  ��� / *������� = �����+�� MAR1_2_00083
							CHAR_DELETE(m_pOldBuffer);
							int nLen = strlen(m_pFormNTo1Word);
							m_pOldBuffer = new char[nLen+1];
							memset(m_pOldBuffer, 0x00, nLen+1);
							strcpy(m_pOldBuffer, m_pFormNTo1Word);
							InsertArrayFeatureOld(m_pFormNTo1Word);

							GoMAR1_1(pNode, nStart, mStart, lStart);
						}
					}
				}
			}
		}
		else
		{
			IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
			m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
			m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1~1)
			LoggingFunction(pNode, "MAR1_2_00084+ ");
			if (IsExistEndingDic(m_pFormNTo1Word, m_endingFeature01))
			{//4n : S(n+1~1)�� ending ������ �ִ°�? MAR1_2_00084
				LoggingFunction(pNode, "MAR1_2_00085+ ");
				m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)
				m_p1thAddString = AddTwoStr(m_pNthOneChar, m_pOldBuffer);
				//5y : OldBuf() = S(n)+OldBuf() �� ��� MAR1_2_00085
				CHAR_DELETE(m_pOldBuffer);
				int nLen = strlen(m_p1thAddString);
				m_pOldBuffer = new char[nLen+1];
				memset(m_pOldBuffer, 0x00, nLen+1);
				strcpy(m_pOldBuffer, m_p1thAddString);
				InsertArrayFeatureOld(m_p1thAddString);

				GoMAR1_1(pNode, nStart, mStart, lStart);
			}
			else
			{
				IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
				m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
				m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
				m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//n+1�� ����
				m_pJongChar = GetChojungJongsungChar(m_pOneNPluseNumChar, CHAR_JONGSUNG);//C(n+1)�� ����
				m_pEndingDicWord = AddTwoStr(m_pJongChar, m_pFormNTo1Word);//C(n+1)+S(n~1)
				LoggingFunction(pNode, "MAR1_2_00086+ ");
				if (IsExistEndingDic(m_pEndingDicWord, m_endingFeature01))
				{//5n : C(n+1)+S(n~1)�� ending ������ �ִ°�? MAR1_2_00086
					LoggingFunction(pNode, "MAR1_2_00087+ ");
					m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)
					m_p1thAddString = AddTwoStr(m_pNthOneChar, m_pOldBuffer);
					//6y : OldBuf() = S(n)+OldBuf() �� ��� MAR1_2_00087
					CHAR_DELETE(m_pOldBuffer);
					int nLen = strlen(m_p1thAddString);
					m_pOldBuffer = new char[nLen+1];
					memset(m_pOldBuffer, 0x00, nLen+1);
					strcpy(m_pOldBuffer, m_p1thAddString);
					InsertArrayFeatureOld(m_p1thAddString);

					GoMAR1_1(pNode, nStart, mStart, lStart);
				}
				else
				{
					IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
					m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
					m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1~1)
					m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
					m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//n+2�� ����
					m_pJongChar = GetChojungJongsungChar(m_pOneNPluseNumChar, CHAR_JONGSUNG);//C(n+2)�� ����
					m_pEndingDicWord = AddTwoStr(m_pJongChar, m_pFormNTo1Word);//C(n+2)+S(n+1~1)
					LoggingFunction(pNode, "MAR1_2_00088+ ");
					if (IsExistEndingDic(m_pEndingDicWord, m_endingFeature01))
					{//6n : C(n+2)+S(n+1~1)�� ending ������ �ִ°�? MAR1_2_00088
						LoggingFunction(pNode, "MAR1_2_00089+ ");
						m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)
						m_p1thAddString = AddTwoStr(m_pNthOneChar, m_pOldBuffer);
						//7y : OldBuf() = S(n)+OldBuf() �� ���  MAR1_2_00089
						CHAR_DELETE(m_pOldBuffer);
						int nLen = strlen(m_p1thAddString);
						m_pOldBuffer = new char[nLen+1];
						memset(m_pOldBuffer, 0x00, nLen+1);
						strcpy(m_pOldBuffer, m_p1thAddString);
						InsertArrayFeatureOld(m_p1thAddString);

						GoMAR1_1(pNode, nStart, mStart, lStart);
					}
					else
					{
						IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
						m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
						m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)�� ����
						m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
						m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
						m_pEndingDicWord = AddTwoStr(m_pJongChar, m_pFormNTo1Word);//C(n)+S(n-1~1)
						LoggingFunction(pNode, "MAR1_2_00090+ ");
						if (IsExistEndingDic(m_pEndingDicWord, m_endingFeature01))
						{//7n : C(n)+S(n-1~1)�� ending ������ �ִ°�? MAR1_2_00090
							m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
							m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
							LoggingFunction(pNode, "MAR1_2_00091+ ");
							if (IsExistStemDic(m_pStemDicDeleteJongWord, m_stemFeature01))
							{//8y : S(len~n)-C(n)�� stem ������ �ִ°�? MAR1_2_00091
								LoggingFunction(pNode, "MAR1_2_00092+ ");
								m_p1thAddString = AddTwoStr(m_pJongChar, m_pOldBuffer);
								//9y : Stem = S(len~n)-C(n), ending = C(n)+OldBuf() MAR1_2_00092
								InsertStemString(pNode, m_pStemDicDeleteJongWord);
								SaveHTokenStem(pNode);
								InsertEndingString(pNode, m_p1thAddString);
								SaveHTokenEnding(pNode);
							}
							else
							{
								LoggingFunction(pNode, "MAR1_2_00093+ ");
								//9n : OldBuf() = C(n)+S(n-1~1), S(n) = S(n)-C(n) , MAR2�� MAR1_2_00093
								CHAR_DELETE(m_pOldBuffer);
								int nLen = strlen(m_pEndingDicWord);
								m_pOldBuffer = new char[nLen+1];
								memset(m_pOldBuffer, 0x00, nLen+1);
								strcpy(m_pOldBuffer, m_pEndingDicWord);
								InsertArrayFeatureOld(m_pEndingDicWord);
								ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA14);//S(n)-C(n) ������ ���ֶ�� �ǹ��� :REPLACE_HANGUL_JA14 NULL�ǹ�
								
								GoMAR2(pNode, nStart, mStart, lStart);
							}
						}
						else
						{
							IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
							m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
							m_pLastOneChar = GetOneChar(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));//S(1)
							m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)�� ����
							LoggingFunction(pNode, "MAR1_2_00094+ ");
							if (strcmp(m_pNthOneChar, m_pLastOneChar) == 0)
							{//8n : S(n) == S(1)�ΰ�? MAR1_2_00094
								LoggingFunction(pNode, "MAR1_2_00095+ ");
								if (strcmp(m_pJongChar, "��") == 0)
								{//9y : C(n) == ������ �ΰ�? MAR1_2_00095
									m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
									m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
									LoggingFunction(pNode, "MAR1_2_00096+ ");
									if (IsExistStemDic(m_pStemDicDeleteJongWord, m_stemFeature01))
									{//10y : S(len~n)-C(n) �� stem ������ �ִ°�? MAR1_2_00096
										LoggingFunction(pNode, "MAR1_2_00097+ ");
										//11y : stem = S(len ~ n)-C(n), ending = ������  MAR1_2_00097
										InsertStemString(pNode, m_pStemDicDeleteJongWord);
										SaveHTokenStem(pNode);
										InsertEndingString(pNode, "��");
										SaveHTokenEnding(pNode);
									}
									else
									{
										m_pChoJungChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_CHOJUNGSUNG);//S(n)-C(n)
										LoggingFunction(pNode, "MAR1_2_00098+ ");
										if (strcmp(m_pChoJungChar, "��") == 0 || strcmp(m_pChoJungChar, "��") == 0)
										{//11n : S(n)-C(n) == ���ϡ�||���ǡ��ΰ�? MAR1_2_00098
											LoggingFunction(pNode, "MAR1_2_00099+ ");
											//12y : OldBuf() = ������ -> IRM����  MAR1_2_00099
											CHAR_DELETE(m_pOldBuffer);
											int nLen = strlen("��");
											m_pOldBuffer = new char[nLen+1];
											memset(m_pOldBuffer, 0x00, nLen+1);
											strcpy(m_pOldBuffer, "��");
											InsertArrayFeatureOld("��");
											
											GoIMR1(pNode, nStart, mStart,lStart);
										}
										else
										{
											m_nRestorationJongChar = Restoration(m_pJongChar);//�����ؾ� �� ������
											ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);//C(n) = ������
											m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
											LoggingFunction(pNode, "MAR1_2_00100+ ");
											if (IsExistStemDic(m_pStemDicWord, m_stemFeature01))
											{//12n : C(n) = ������ ->  S(len~n) �� stem ������ �ִ°�? MAR1_2_00100
												LoggingFunction(pNode, "MAR1_2_00101+ ");
												//13y : stem = S(len ~ n), ending = ������ MAR1_2_00101
												InsertStemString(pNode, m_pStemDicWord);
												SaveHTokenStem(pNode);
												InsertEndingString(pNode, "��");
												SaveHTokenEnding(pNode);
											}
											else
											{
												LoggingFunction(pNode, "MAR1_2_00102+ ");
												//13n : OldBuf() = ������ , C(n) = Null, ��� MAR1_2_00102
												CHAR_DELETE(m_pOldBuffer);
												int nLen = strlen("��");
												m_pOldBuffer = new char[nLen+1];
												memset(m_pOldBuffer, 0x00, nLen+1);
												strcpy(m_pOldBuffer, "��");
												InsertArrayFeatureOld("��");
												ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA14);//S(n)-C(n) ������ ���ֶ�� �ǹ��� :REPLACE_HANGUL_JA14 NULL�ǹ�
												
												GoMAR1_2(pNode, nStart, mStart, lStart);
											}
										}
									}
								}
								else
								{
									IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
									m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
									m_pLastOneChar = GetOneChar(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));//S(1)
									m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)�� ����
									LoggingFunction(pNode, "MAR1_2_00103+ ");
									if (strcmp(m_pJongChar, "��") == 0)
									{//10n : C(n) == ������ �ΰ�? MAR1_2_00103
										m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
										m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
										LoggingFunction(pNode, "MAR1_2_00104+ ");
										if (IsExistStemDic(m_pStemDicDeleteJongWord, m_stemFeature01))
										{//11y : S(len~n)-C(n) �� stem ������ �ִ°�? MAR1_2_00104
											LoggingFunction(pNode, "MAR1_2_00105+ ");
											//12y : stem = S(len ~ n)-C(n), ending = ������ MAR1_2_00105
											InsertStemString(pNode, m_pStemDicDeleteJongWord);
											SaveHTokenStem(pNode);
											InsertEndingString(pNode, "��");
											SaveHTokenEnding(pNode);
										}
										else
										{
											LoggingFunction(pNode, "MAR1_2_00106+ ");
											//12n : OldBuf() = S(n)+OldBuf(), ��� MAR1_2_00106
											m_p1thAddString = AddTwoStr(m_pNthOneChar, m_pOldBuffer);
											CHAR_DELETE(m_pOldBuffer);
											int nLen = strlen(m_p1thAddString);
											m_pOldBuffer = new char[nLen+1];
											memset(m_pOldBuffer, 0x00, nLen+1);
											strcpy(m_pOldBuffer, m_p1thAddString);
											InsertArrayFeatureOld(m_p1thAddString);

											GoMAR1_1(pNode, nStart, mStart, lStart);
										}
									}
									else
									{
										LoggingFunction(pNode, "MAR1_2_00107+ ");
										//11n : OldBuf() = S(n)+OldBuf(), ��� MAR1_2_00107
											m_p1thAddString = AddTwoStr(m_pNthOneChar, m_pOldBuffer);
											CHAR_DELETE(m_pOldBuffer);
											int nLen = strlen(m_p1thAddString);
											m_pOldBuffer = new char[nLen+1];
											memset(m_pOldBuffer, 0x00, nLen+1);
											strcpy(m_pOldBuffer, m_p1thAddString);
											InsertArrayFeatureOld(m_p1thAddString);

											GoMAR1_1(pNode, nStart, mStart, lStart);
									}
								}
							}
							else
							{
								LoggingFunction(pNode, "MAR1_2_00108+ ");
								//9n : m = m - 1 �� MAR2�� MAR1_2_00108
								mStart = mStart - 1;
								
								GoMAR1_1(pNode, nStart, mStart, lStart);
							}
						}
					}
				}
			}
		}
	}
	else
	{
		//����������������������������������������������������������������������������������������������MAR1_3
		//3n : n == 1�ΰ� ? MAR1_3_00001 
		GoMAR1_3(pNode, nStart, mStart, lStart);
	}
}

	
void CNAHangulAnaly::GoMAR1_3(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
	m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)�� ����
	LoggingFunction(pNode, "MAR1_3_00001+ ");
	if (strlen(pNode->pHToken->strOriginalSource) == nStart)
	{//3n : n == 1�ΰ� ? MAR1_3_00001
		LoggingFunction(pNode, "MAR1_3_00002+ ");
		if (strcmp(m_pJongChar, "��") == 0)
		{//4y : C(n) == �������ΰ� ? MAR1_3_00002
			m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
			m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
			LoggingFunction(pNode, "MAR1_3_00003+ ");
			if (IsExistStemDic(m_pStemDicDeleteJongWord))
			{//5y : S(len~n)-C(n)�� stem ������ �ִ°� ? MAR1_3_00003
				LoggingFunction(pNode, "MAR1_3_00004+ ");
				//6y : stem = S(len~n)-C(n), ending = C(n) MAR1_3_00004
				InsertStemString(pNode, m_pStemDicDeleteJongWord);
				SaveHTokenStem(pNode);
				InsertEndingString(pNode, m_pJongChar);
				SaveHTokenEnding(pNode);
			}
			else
			{
				CHAR_DELETE(m_pStemDicWord);
				m_nRestorationJongChar = Restoration(m_pJongChar);//�����ؾ� �� ������
				ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);//C(n) = ������
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
				LoggingFunction(pNode, "MAR1_3_00005+ ");
				if (IsExistStemDic(m_pStemDicWord))
				{//6n : C(n) = ������ �� S(len~n)�� stem ������ �ִ°� ? MAR1_3_00005
					LoggingFunction(pNode, "MAR1_3_00006+ ");
					if (strcmp(m_pStemDicWord, "¦����") == 0 || strcmp(m_pStemDicWord, "Ȧ����") == 0)
					{//7y : S(len~n) == ��¦���ϡ� || ��Ȧ���ϡ��ΰ� ? MAR1_3_00006
						LoggingFunction(pNode, "MAR1_3_00007+ ");
						m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
						m_pFormLenToNWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
						m_p1thAddString = AddTwoStr("��", "��");
						//8y : stem = S(len~n+1), ending = ���̡�+[pol]+����" MAR1_3_00007
						InsertStemString(pNode, m_pFormLenToNWord);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, m_p1thAddString);
						SaveHTokenEnding(pNode);
						AddHTokenEnding(pNode, ENDING_DIC_POL);
					}
					else
					{
						LoggingFunction(pNode, "MAR1_3_00008+ ");
						//8n : stem = S(len~n), ending = ������ MAR1_3_00008
						InsertStemString(pNode, m_pStemDicWord);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, "��");
						SaveHTokenEnding(pNode);
					}
				}
				else
				{
					CHAR_DELETE(m_pNthOneChar);
					ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA01);//C(n) = ������
					m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
					m_pChoJungChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_CHOJUNGSUNG);//S(n)-C(n)
					LoggingFunction(pNode, "MAR1_3_00009+ ");
					if (strcmp(m_pChoJungChar, "��") == 0 || strcmp(m_pChoJungChar, "��") == 0 || strcmp(m_pChoJungChar, "��") == 0
						|| strcmp(m_pChoJungChar, "��") == 0 || strcmp(m_pChoJungChar, "��") == 0 || strcmp(m_pChoJungChar, "��") == 0
						|| strcmp(m_pChoJungChar, "��") == 0 || strcmp(m_pChoJungChar, "Ű") == 0 || strcmp(m_pChoJungChar, "��") == 0)
					{//7n : C(n) = ������ �� S(n)-C(n) ==  ������|| ���á�|| ���̡�|| ���ϡ�|| ���ǡ�|| ������|| ���족|| ��Ű" || ���⡱�ΰ�? (20030128,������ ����, ����Ƭ�̿¡�, �����ڻ�����)) MAR1_3_00009
						LoggingFunction(pNode, "MAR1_3_00010+ ");
						//8y : OldBuf = ������, S(n) = S(n)-C(n) MAR1_3_00010
						//�� S(n)-C(n) == ���̡�||���ϡ�||���ǡ�||������||��Ű��||���족�̸�, IRM���� MAR1_3_00011
						//�� S(n)-C(n) == ���á�||������ MAR1�� ��� MAR1_3_00012
						CHAR_DELETE(m_pOldBuffer);
						int nLen = strlen("��");
						m_pOldBuffer = new char[nLen+1];
						memset(m_pOldBuffer, 0x00, nLen+1);
						strcpy(m_pOldBuffer, "��");
						InsertArrayFeatureOld("��");
						ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA14);//S(n)-C(n) ������ ���ֶ�� �ǹ��� :REPLACE_HANGUL_JA14 NULL�ǹ�

						if (strcmp(m_pChoJungChar, "��") == 0 || strcmp(m_pChoJungChar, "��") == 0 || strcmp(m_pChoJungChar, "��") == 0
							|| strcmp(m_pChoJungChar, "��") == 0 || strcmp(m_pChoJungChar, "Ű") == 0 || strcmp(m_pChoJungChar, "��") == 0)
						{
							LoggingFunction(pNode, "MAR1_3_00011+ ");

							GoIMR1(pNode, nStart, mStart,lStart);
						}
						else if (strcmp(m_pChoJungChar, "��") == 0 || strcmp(m_pChoJungChar, "��") == 0)
						{
							LoggingFunction(pNode, "MAR1_3_00012+ ");

							GoMAR1_2(pNode, nStart, mStart, lStart);
						}
					}
					else
					{
						LoggingFunction(pNode, "MAR1_3_00013+ ");

						//8n : Error(���� �˰��� �Ϻ� ����) MAR1_3_00013
						GoErrorFunction(pNode);
					}
				}
			}
		}
		else
		{
			LoggingFunction(pNode, "MAR1_3_00014+ ");
			if (strcmp(m_pNthOneChar, "��") != 0 && strcmp(m_pNthOneChar, "��") != 0 && strcmp(m_pJongChar, "��") == 0)
			{//5n : S(n) != ������&&���ߡ�  && C(n) == �������ΰ�? MAR1_3_00014
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
				m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
				LoggingFunction(pNode, "MAR1_3_00015+ ");
				if (IsExistStemDic(m_pStemDicDeleteJongWord))
				{//6y : S(len~n)-C(n)�� stem ������ �ִ°�? MAR1_3_00015
					LoggingFunction(pNode, "MAR1_3_00016+ ");
					//7y : stem = S(len~n)-C(n), ending = C(n) MAR1_3_00016
					InsertStemString(pNode, m_pStemDicDeleteJongWord);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_pJongChar);
					SaveHTokenEnding(pNode);
				}
				else
				{
					LoggingFunction(pNode, "MAR1_3_00017+ ");
					if (IsExistStemDic(m_pStemDicWord))
					{//7n : S(len~n)�� stem ������ �ִ°�? MAR1_3_00017
						LoggingFunction(pNode, "MAR1_3_00018+ ");
						//8y : stem = S(len~n), ending = �� MAR1_3_00018
						InsertStemString(pNode, m_pStemDicWord);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, "��");
						SaveHTokenEnding(pNode);
					}
					else
					{
						m_pChoJungChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_CHOJUNGSUNG);//S(n)-C(n)
						LoggingFunction(pNode, "MAR1_3_00019+ ");
						//S(len~n)-C(n)�� Stem������ ���� ..6y���� �̹� Ȯ�ε� ���� ������ ���Խ�Ű�� ���� else���� �̹� false������ ���Ե�
						if (strcmp(m_pChoJungChar, "��") == 0 || strcmp(m_pChoJungChar, "��") == 0 || strcmp(m_pChoJungChar, "��") == 0
							|| strcmp(m_pChoJungChar, "��") == 0 || strcmp(m_pChoJungChar, "��") == 0 || strcmp(m_pChoJungChar, "Ű") == 0)
						{//8n : S(len~n)-C(n)�� Stem������ ����, S(n)-C(n) != ���̡�|| ������|| ���á�|| ���ϡ�|| ���ǡ�|| ��Ű������ Ȯ���϶�. MAR1_3_00019
							LoggingFunction(pNode, "MAR1_3_00020+ ");

							//9y : Error(���̽�������, �ٵ���� 20030321 ������ ����) MAR1_3_00020
							GoErrorFunction(pNode);
						}
						else
						{
							LoggingFunction(pNode, "MAR1_3_00021+ ");
							if (CompareVariousTable02(m_pChoJungChar, 1, false) == true || CompareVariousTable02(m_pChoJungChar, 2, false) == true
								|| CompareVariousTable03(m_pChoJungChar, 0, false) == true || CompareVariousTable03(m_pChoJungChar, 1, false) == true
								|| CompareVariousTable04(m_pChoJungChar, 1, false) == true || CompareVariousTable04(m_pChoJungChar, 2, false) == true || CompareVariousTable04(m_pChoJungChar, 3, false) == true
								|| CompareVariousTable05(m_pChoJungChar, ALL_TABLE_05, true) == true
								|| CompareVariousTable06(m_pChoJungChar, ALL_TABLE_06, true) == true
								|| CompareVariousTable07(m_pChoJungChar, ALL_TABLE_07, true) == true
								|| strcmp(m_pChoJungChar, "��") == 0)
							{//9n : S(n)-C(n) == txx ||���⡱�ΰ�? MAR1_3_00021
								LoggingFunction(pNode, "MAR1_3_00022+ ");
								//10y : OldBuf = ������ , S(n) = S(n)-C(n) &&IRM���� MAR1_3_00022
								CHAR_DELETE(m_pOldBuffer);
								int nLen = strlen("��");
								m_pOldBuffer = new char[nLen+1];
								memset(m_pOldBuffer, 0x00, nLen+1);
								strcpy(m_pOldBuffer, "��");
								InsertArrayFeatureOld("��");
								ReplaceHangulJongsungNull(pNode, nStart);
								
								GoIMR1(pNode, nStart, mStart, lStart);
							}
							else
							{
								LoggingFunction(pNode, "MAR1_3_00023+ ");
								//10n : OldBuf = C(n), S(n) = S(n)-C(n), MAR1_2�� MAR1_3_00023
								CHAR_DELETE(m_pOldBuffer);
								int nLen = strlen(m_pJongChar);
								m_pOldBuffer = new char[nLen+1];
								memset(m_pOldBuffer, 0x00, nLen+1);
								strcpy(m_pOldBuffer, m_pJongChar);
								InsertArrayFeatureOld(m_pJongChar);
								ReplaceHangulJongsungNull(pNode, nStart);

								GoMAR1_2(pNode, nStart, mStart, lStart);
							}
						}
					}
				}
			}
			else
			{
				LoggingFunction(pNode, "MAR1_3_00024+ ");
				if (strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0)
				{//6n : S(n) = ������||���ߡ��ΰ�? MAR1_3_00024
					LoggingFunction(pNode, "MAR1_3_00025+ ");

					//7y : Error MAR1_3_00025
					GoErrorFunction(pNode);
				}
				else
				{
					LoggingFunction(pNode, "MAR1_3_00026+ ");
					if (IsExistEndingDic(m_pJongChar))
					{//7n : C(n)�� ending ������ �ִ°�? MAR1_3_00026
						m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
						m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
						LoggingFunction(pNode, "MAR1_3_00027+ ");
						if (IsExistStemDic(m_pStemDicDeleteJongWord))
						{//8y : S(len~n)-C(n)�� stem ������ �ִ°�? MAR1_3_00027
							LoggingFunction(pNode, "MAR1_3_00028+ ");
							//9y : stem = (len~n)-C(n), ending = C(n) MAR1_3_00028
							InsertStemString(pNode, m_pStemDicDeleteJongWord);
							SaveHTokenStem(pNode);
							InsertEndingString(pNode, m_pJongChar);
							SaveHTokenEnding(pNode);
						}
						else
						{
							m_pChoJungChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_CHOJUNGSUNG);//S(n)-C(n)
							LoggingFunction(pNode, "MAR1_3_00029+ ");
							if (CompareVariousTable02(m_pChoJungChar, 1, false) == true || CompareVariousTable02(m_pChoJungChar, 2, false) == true
								|| CompareVariousTable03(m_pChoJungChar, 0, false) == true || CompareVariousTable03(m_pChoJungChar, 1, false) == true
								|| CompareVariousTable04(m_pChoJungChar, 1, false) == true || CompareVariousTable04(m_pChoJungChar, 2, false) == true || CompareVariousTable04(m_pChoJungChar, 3, false) == true
								|| CompareVariousTable05(m_pChoJungChar, ALL_TABLE_05, true) == true
								|| CompareVariousTable06(m_pChoJungChar, ALL_TABLE_06, true) == true
								|| CompareVariousTable07(m_pChoJungChar, ALL_TABLE_07, true) == true)
							{//9n : S(n)-C(n) == txx�ΰ�? MAR1_3_00030
								LoggingFunction(pNode, "MAR1_3_00029+ ");
								ReplaceHangulJongsungNull(pNode, nStart);
								//10y : S(n) = S(n)-C(n) && IRM���� MAR1_3_00030
								
								GoIMR1(pNode, nStart, mStart, lStart);
							}
							else
							{
								LoggingFunction(pNode, "MAR1_3_00031+ ");
								CHAR_DELETE(m_pOldBuffer);
								int nLen = strlen(m_pJongChar);
								m_pOldBuffer = new char[nLen+1];
								memset(m_pOldBuffer, 0x00, nLen+1);
								strcpy(m_pOldBuffer, m_pJongChar);
								//10n : OldBuf = C(n), S(n) = S(n)-C(n), MAR1_2�� MAR1_3_00031
								InsertArrayFeatureOld(m_pJongChar);
								ReplaceHangulJongsungNull(pNode, nStart);

								GoMAR1_2(pNode, nStart, mStart, lStart);
							}
						}
					}
					else
					{
						LoggingFunction(pNode, "MAR1_3_00032+ ");

						//8n : Error MAR1_3_00032
						GoErrorFunction(pNode);
					}
				}
			}
		}
	}
	else
	{
		//4n : C(n) == Null�ΰ� ? MAR1_4_00001
		GoMAR1_4(pNode, nStart, mStart, lStart);
	}
}


void CNAHangulAnaly::GoMAR1_4(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
	m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)�� ����
	LoggingFunction(pNode, "MAR1_4_00001+ ");
	if(m_pJongChar == NULL || strcmp(m_pJongChar,"")== 0)
	{//4n : C(n) == Null�ΰ� ? MAR1_4_00001
		ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);//C(n) = ������
		m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
		LoggingFunction(pNode, "MAR1_4_00002+ ");
		if (IsExistStemDic(m_pStemDicWord, m_stemFeature01))
		{//5y : C(n) = ������ �� S(len~n)�� stem ������ �ִ°� ? MAR1_4_00002
			m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
			m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
			bool bFind = IsExistEndingDic(m_pFormNTo1Word, m_endingFeature01);
			LoggingFunction(pNode, "MAR1_4_00003+ ");
			if(bFind == true && m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] != IS_CEHCK_FEATURE)
			{//6y : {S(len~n) <>[N] && S(n-1~1) <>[N]}�ΰ� ? MAR1_4_00003
				LoggingFunction(pNode, "MAR1_4_00004+ ");
				if(m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_AMB] == IS_CEHCK_FEATURE)
				{//7y : {S(len~n) == [amb] || S(n-1~1) == [amb]}�ΰ� ? MAR1_4_00004
					LoggingFunction(pNode, "MAR1_4_00005+ ");
					if(m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_PPO] != IS_CEHCK_FEATURE)
					{//8y : {S(len~n) == [N] && S(n-1~1) != [Ppo]}�ΰ� ? MAR1_4_00005
						LoggingFunction(pNode, "MAR1_4_00006+ ");
						//9y : Error MAR1_4_00006
						GoErrorFunction(pNode);
					}
					else
					{
						LoggingFunction(pNode, "MAR1_4_00007+ ");
						//9n : Stem = S(len~n), ending = OldBuf() MAR1_4_00007
						InsertStemString(pNode, m_pStemDicWord);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, m_pOldBuffer);
						SaveHTokenEnding(pNode);
					}
				}
				else
				{ 
					LoggingFunction(pNode, "MAR1_4_00008+ ");
					//8n : Stem = S(len~n), ending = OldBuf() MAR1_4_00008
					InsertStemString(pNode, m_pStemDicWord);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_pOldBuffer);
					SaveHTokenEnding(pNode);
				}
			}
			else
			{
				LoggingFunction(pNode, "MAR1_4_00009+ ");
				//7n : C(n) = Null �� MAR2�� MAR1_4_00009
				ReplaceHangulJongsungNull(pNode, nStart);

				GoMAR2(pNode, nStart, mStart, lStart);
			}
		}
		else
		{
			LoggingFunction(pNode, "MAR1_4_00010+ ");
			//6n : C(n) = Null �� MAR2�� MAR1_4_00010
			ReplaceHangulJongsungNull(pNode, nStart);

			GoMAR2(pNode, nStart, mStart, lStart);

		}
	}
	else
	{
		//5n : #C(n)+S(n-1 ~ 1)�� ending ������ �ִ°� ? MAR1_5_00001
		GoMAR1_5(pNode, nStart, mStart, lStart);
	}
}

void CNAHangulAnaly::GoMAR1_5(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
	m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)�� ����
	m_nRestorationJongChar = Restoration(m_pJongChar);//�����ؾ� �� ������ #C(n)
	m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
	m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
	m_pEndingDicWord = AddTwoStr(m_pJongChar, m_pFormNTo1Word);//C(n)+S(n-1~1)
	LoggingFunction(pNode, "MAR1_5_00001+ ");
	if(IsExistEndingDic(m_pEndingDicWord, m_endingFeature01))
	{//5n : #C(n)+S(n-1~1)�� ending ������ �ִ°� ? MAR1_5_00001
		m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
		m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
		LoggingFunction(pNode, "MAR1_5_00002+ ");
		if (IsExistStemDic(m_pStemDicDeleteJongWord, m_stemFeature01))
		{//6y : (S(len~n)-C(n))�� stem ������ �ִ°� ? MAR1_5_00002
			if(m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_PPO] == IS_CEHCK_FEATURE)
			{//7y : {(S(len~n)-C(n)) == [N] && C(n)+S(n-1~1) == [Ppo]}�ΰ� ? MAR1_5_00003
				LoggingFunction(pNode, "MAR1_5_00004+ ");
				m_p1thAddString = AddTwoStr("��", m_pEndingDicWord);
				//8y : stem = S(len~n)-C(n), ending = ���̡�[Pol]+C(n)+S(n-1~1) MAR1_5_00004
				InsertStemString(pNode, m_pStemDicDeleteJongWord);
				SaveHTokenStem(pNode);
				InsertEndingString(pNode, m_p1thAddString);
				SaveHTokenEnding(pNode);
				AddHTokenEnding(pNode, ENDING_DIC_POL);//[Pol]�����ο�
			}
			else
			{
				LoggingFunction(pNode, "MAR1_5_00005+ ");
				if((m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] == IS_CEHCK_FEATURE)
					|| (m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] != IS_CEHCK_FEATURE)
					|| (m_stemFeature01[STEM_DIC_AMB] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_AMB] != IS_CEHCK_FEATURE))
				{//8n : {(S(len~n)-C(n)) == [N] && C(n) +S(n-1~1) == [N]}�̰ų�, {(S(len~n)-C(n)) <> [N] && C(n) +S(n-1~1) <> [N]}�̰ų�  {(S(len~n)-C(n)) == [amb] || C(n) +S(n-1~1) == [amb]}�ΰ�? MAR1_5_00005
					LoggingFunction(pNode, "MAR1_5_00006+ ");
					//9y : stem = S(len~n)-C(n), ending = C(n)+S(n-1~1) MAR1_5_00006
					InsertStemString(pNode, m_pStemDicDeleteJongWord);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_pEndingDicWord);
					SaveHTokenEnding(pNode);
				}
				else
				{ 
					CHAR_DELETE(m_pFormNTo1Word);
					ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);//C(n) = ������
					m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n-1~1)
					LoggingFunction(pNode, "MAR1_5_00007+ ");
					if (IsExistStemDic(m_pFormNTo1Word, m_stemFeature02))
					{//9n : C(n) = ������ , S(len~n)�� stem ������ �ִ°�? MAR1_5_00007
						LoggingFunction(pNode, "MAR1_5_00008+ ");
						if((m_stemFeature02[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] == IS_CEHCK_FEATURE)
							|| (m_stemFeature02[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] != IS_CEHCK_FEATURE)
							|| (m_stemFeature02[STEM_DIC_AMB] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_AMB] != IS_CEHCK_FEATURE))
						{//10y :  {S(len~n) == [N] && C(n) +S(n-1~1) == [N]}�̰ų�, {S(len~n)  <> [N] && C(n) +S(n-1~1) <> [N]}�̰ų�  {S(len~n) == [amb] || C(n) +S(n-1~1) == [amb]}�ΰ�? MAR1_5_00008
							LoggingFunction(pNode, "MAR1_5_00009+ ");
							//11y : Stem = S(len~n), Ending = C(n)+S(n-1~1) MAR1_5_00009
							InsertStemString(pNode, m_pFormNTo1Word);
							SaveHTokenStem(pNode);
							InsertEndingString(pNode, m_pEndingDicWord);
							SaveHTokenEnding(pNode);
						}
						else
						{
							LoggingFunction(pNode, "MAR1_5_00010+ ");
							CHAR_DELETE(m_pFormNTo1Word);
							//������ ���������� ����
							JasoRestoration(pNode, nStart, FORCE_REPLACE_ENDWORD, m_nRestorationJongChar);// C(n) = #C(n)
							//11n : C(n) = #C(n), stem = S(len~n+1), ending = S(n~1) MAR1_5_00010
							m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
							m_pFormLenToNWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
							m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
							InsertStemString(pNode, m_pFormLenToNWord);
							SaveHTokenStem(pNode);
							InsertEndingString(pNode, m_pFormNTo1Word);
							SaveHTokenEnding(pNode);
						}
					}
					else
					{ 
						LoggingFunction(pNode, "MAR1_5_00011+ ");
						//10n : C(n) = #C(n), stem = S(len~n+1), ending = S(n~1) MAR1_5_00011
						m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
						m_pFormLenToNWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
						m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
						InsertStemString(pNode, m_pFormLenToNWord);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, m_pFormNTo1Word);
						SaveHTokenEnding(pNode);
					}
				}
			}
		}
		else
		{
			CHAR_DELETE(m_pStemDicWord);
			ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);//C(n) = ������
			m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
			LoggingFunction(pNode, "MAR1_5_00012+ ");
			if (IsExistStemDic(m_pStemDicDeleteJongWord, m_stemFeature01))
			{//7n : {S(len~n)-#C(n) } && C(n) = ��������S(len~n)�� stem ������ �ִ°�? MAR1_5_00012
				LoggingFunction(pNode, "MAR1_5_00013+ ");
				if((m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] == IS_CEHCK_FEATURE)
					|| (m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] != IS_CEHCK_FEATURE)
					|| (m_stemFeature01[STEM_DIC_AMB] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_AMB] != IS_CEHCK_FEATURE))
				{//8y : {S(len~n) == [N] && #C(n)+S(n-1~1) == [N]}�̰ų�, {S(len~n) <> [N] && #C(n)+S(n-1~1) <> [N]}�̰ų�  {S(len~n) == [amb] || #C(n)+S(n-1~1) == [amb]}�ΰ�? MAR1_5_00013
					LoggingFunction(pNode, "MAR1_5_00014+ ");
					//9y : Stem = S(len~n), ending = #C(n)+OldBuf() MAR1_5_00014
					m_p1thAddString = AddTwoStr(m_pJongChar, m_pOldBuffer);
					InsertStemString(pNode, m_pStemDicWord);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_p1thAddString);
					SaveHTokenEnding(pNode);
				}
				else
				{
					LoggingFunction(pNode, "MAR1_5_00015+ ");
					m_p1thAddString = AddTwoStr(m_pJongChar, m_pOldBuffer);
					//9n : C(n) = #C(n) , OldBuf() = C(n)+OldBuf(), S(n) = S(n)-C(n)��MAR2�� MAR1_5_00015
					CHAR_DELETE(m_pOldBuffer);
					int nLen = strlen(m_p1thAddString);
					m_pOldBuffer = new char[nLen + 1];
					memset(m_pOldBuffer, 0x00, nLen + 1);
					strcpy(m_pOldBuffer, m_p1thAddString);
					InsertArrayFeatureOld(m_p1thAddString);
					//����������
					ReplaceHangulJongsungNull(pNode, nStart);

					GoMAR2(pNode, nStart, mStart, lStart);
				}
			}
			else
			{
				//������ ���������� ����
				JasoRestoration(pNode, nStart, FORCE_REPLACE_ENDWORD, m_nRestorationJongChar);//C(n)=#C(n)
				IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
				m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
				m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)�� ����
				m_pJongChar = GetChojungJongsungChar(m_pOneNPluseNumChar, CHAR_JONGSUNG);//C(n+1)�� ����
				m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
				m_pEndingDicWord = AddTwoStr(m_pJongChar, m_pFormNTo1Word);//C(n+1)+S(n~1)
				LoggingFunction(pNode, "MAR1_5_00016+ ");
				if (IsExistEndingDic(m_pEndingDicWord, m_endingFeature01))
				{//8n : C(n) = #C(n) �� C(n+1)+S(n~1)�� ending������ �ִ°�? MAR1_5_00016
					m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
					m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pOneNPluseNumChar, m_nPluseSyllable);//S(len~n+1)-C(n+1)
					LoggingFunction(pNode, "MAR1_5_00017+ ");
					if (IsExistStemDic(m_pStemDicDeleteJongWord, m_stemFeature01))
					{//9y : S(len~n+1)-C(n+1)�� Stem ������ �ִ°�? MAR1_5_00017
						LoggingFunction(pNode, "MAR1_5_00018+ ");
						if((m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] == IS_CEHCK_FEATURE)
							|| (m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] != IS_CEHCK_FEATURE)
							|| (m_stemFeature01[STEM_DIC_AMB] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_AMB] != IS_CEHCK_FEATURE))
						{//10y : {(S(len~n+1)-C(n+1)) == [N] && C(n+1) +S(n~1) == [N]}�̰ų�, {(S(len~n+1)-C(n+1)) <> [N] && C(n+1) +S(n~1) <> [N]}�̰ų�  {(S(len~n+1)-C(n+1)) == [amb] || C(n+1) +S(n~1) == [amb]}�ΰ�? MAR1_5_00018
							LoggingFunction(pNode, "MAR1_5_00019+ ");
							//11y : stem = S(len~n+1)-C(n+1), ending = C(n+1)+S(n~1) MAR1_5_00019
							InsertStemString(pNode, m_pStemDicWord);
							SaveHTokenStem(pNode);
							InsertEndingString(pNode, m_pEndingDicWord);
							SaveHTokenEnding(pNode);
						}
						else
						{
							m_pChoJungChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_CHOJUNGSUNG);//S(n)-C(n)
							LoggingFunction(pNode, "MAR1_5_00020+ ");
							if (CompareVariousTable02(m_pChoJungChar, 1, false) == true || CompareVariousTable02(m_pChoJungChar, 2, false) == true
								|| CompareVariousTable03(m_pChoJungChar, 0, false) == true || CompareVariousTable03(m_pChoJungChar, 1, false) == true
								|| CompareVariousTable04(m_pChoJungChar, 1, false) == true || CompareVariousTable04(m_pChoJungChar, 2, false) == true || CompareVariousTable04(m_pChoJungChar, 3, false) == true
								|| CompareVariousTable05(m_pChoJungChar, ALL_TABLE_05, true) == true
								|| CompareVariousTable06(m_pChoJungChar, ALL_TABLE_06, true) == true
								|| CompareVariousTable07(m_pChoJungChar, ALL_TABLE_07, true) == true)
							{//11n : S(n)-C(n) == txx�ΰ�? MAR1_5_00020
								LoggingFunction(pNode, "MAR1_5_00021_01+ ");
								IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
								m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
								m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)�� ����
								m_nRestorationJongChar = Restoration(m_pJongChar);//�����ؾ� �� ������ #C(n)
								m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
								m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
								m_pEndingDicWord = AddTwoStr(m_pJongChar, m_pFormNTo1Word);//C(n)+S(n-1~1)

								CHAR_DELETE(m_pOldBuffer);
								int nLen = strlen(m_pJongChar);
								m_pOldBuffer = new char[nLen + 1];
								memset(m_pOldBuffer, 0x00, nLen + 1);
								strcpy(m_pOldBuffer, m_pEndingDicWord);
								//12y : OldBuf = C(n)+S(n-1~1), S(n) = S(n)-C(n) && IRM���� MAR1_5_00021_01
								InsertArrayFeatureOld(m_pEndingDicWord);
								ReplaceHangulJongsungNull(pNode, nStart);

								GoIMR1(pNode, nStart, mStart, lStart);
							}
							else
							{
								LoggingFunction(pNode, "MAR1_5_00021_02+ ");
								
								IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
								m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
								m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)�� ����
								m_nRestorationJongChar = Restoration(m_pJongChar);//�����ؾ� �� ������ #C(n)
								m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
								m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
								m_pEndingDicWord = AddTwoStr(m_pJongChar, m_pFormNTo1Word);//C(n)+S(n-1~1)
								CHAR_DELETE(m_pOldBuffer);
								int nLen = strlen(m_pJongChar);
								m_pOldBuffer = new char[nLen + 1];
								memset(m_pOldBuffer, 0x00, nLen + 1);
								strcpy(m_pOldBuffer, m_pEndingDicWord);
								//12n : OldBuf = C(n)+S(n-1~1), S(n) = S(n)-C(n) && MAR2�� ? MAR1_5_00021_02
								InsertArrayFeatureOld(m_pEndingDicWord);
								ReplaceHangulJongsungNull(pNode, nStart);

								GoMAR2(pNode, nStart, mStart, lStart);
							}
						}
					}
					else
					{ 
						m_pChoJungChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_CHOJUNGSUNG);//S(n)-C(n)
						LoggingFunction(pNode, "MAR1_5_00022+ ");
						if (CompareVariousTable02(m_pChoJungChar, 1, false) == true || CompareVariousTable02(m_pChoJungChar, 2, false) == true
							|| CompareVariousTable03(m_pChoJungChar, 0, false) == true || CompareVariousTable03(m_pChoJungChar, 1, false) == true
							|| CompareVariousTable04(m_pChoJungChar, 1, false) == true || CompareVariousTable04(m_pChoJungChar, 2, false) == true || CompareVariousTable04(m_pChoJungChar, 3, false) == true
							|| CompareVariousTable05(m_pChoJungChar, ALL_TABLE_05, true) == true
							|| CompareVariousTable06(m_pChoJungChar, ALL_TABLE_06, true) == true
							|| CompareVariousTable07(m_pChoJungChar, ALL_TABLE_07, true) == true)
						{//10n : S(n)-C(n) == txx�ΰ�? MAR1_5_00022
							LoggingFunction(pNode, "MAR1_5_00023+ ");
							
							IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
							m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
							m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)�� ����
							m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
							m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
							m_pEndingDicWord = AddTwoStr(m_pJongChar, m_pFormNTo1Word);//C(n)+S(n-1~1)
							CHAR_DELETE(m_pOldBuffer);
							int nLen = strlen(m_pEndingDicWord);
							m_pOldBuffer = new char[nLen + 1];
							memset(m_pOldBuffer, 0x00, nLen + 1);
							strcpy(m_pOldBuffer, m_pEndingDicWord);
							//11y : OldBuf = C(n)+S(n-1~1), S(n) = S(n)-C(n) && IRM���� MAR1_5_00023
							InsertArrayFeatureOld(m_pEndingDicWord);

							ReplaceHangulJongsungNull(pNode, nStart);

							GoIMR2(pNode, nStart, mStart, lStart);
						}
						else
						{
							LoggingFunction(pNode, "MAR1_5_00024+ ");
							
							IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
							m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
							m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)�� ����
							m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
							m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
							m_pEndingDicWord = AddTwoStr(m_pJongChar, m_pFormNTo1Word);//C(n)+S(n-1~1)
							CHAR_DELETE(m_pOldBuffer);
							int nLen = strlen(m_pEndingDicWord);
							m_pOldBuffer = new char[nLen + 1];
							memset(m_pOldBuffer, 0x00, nLen + 1);
							strcpy(m_pOldBuffer, m_pEndingDicWord);
							//11n : OldBuf = C(n)+S(n-1~1), S(n) = S(n)-C(n) && MAR2 MAR1_5_00024
							InsertArrayFeatureOld(m_pEndingDicWord);

							ReplaceHangulJongsungNull(pNode, nStart);

							GoMAR2(pNode, nStart, mStart, lStart);
						}
					}
				}
				else
				{	
					IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ

					m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
					m_pChoJungChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_CHOJUNGSUNG);//S(n)-C(n)
					m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)�� ����
					m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
					m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
					bool bFindEnding = IsExistEndingDic(m_pJongChar, m_endingFeature01);
					bool bFindStem = IsExistStemDic(m_pStemDicDeleteJongWord, m_stemFeature01);
					LoggingFunction(pNode, "MAR1_5_00025+ ");
					if((bFindEnding == true && m_endingFeature01[ENDING_DIC_PRS] == IS_CEHCK_FEATURE) && bFindStem == false
						&&(strcmp(m_pChoJungChar,"��") == 0 || strcmp(m_pChoJungChar, "��") == 0 || strcmp(m_pChoJungChar, "��") == 0 || strcmp(m_pChoJungChar, "��") == 0 || strcmp(m_pChoJungChar, "��") == 0))
					{//9n : C(n) == [Prs] && S(len~n)-C(n)�� Stem������ ����, S(n)-C(n) != ������||���á�||���ϡ�||���ǡ�||���̡��� Ȯ���϶�. MAR1_5_00025
						LoggingFunction(pNode, "MAR1_5_00026+ ");

						//10y : Error(20030123 ������, �������ϱ�, �Ż��ƿϱ���, �����̽������С�, ���ٵ����) MAR1_5_00026
						GoErrorFunction(pNode);
					}
					else
					{
						LoggingFunction(pNode, "MAR1_5_00027+ ");
						if (CompareVariousTable02(m_pChoJungChar, 1, false) == true || CompareVariousTable02(m_pChoJungChar, 2, false) == true
							|| CompareVariousTable03(m_pChoJungChar, 0, false) == true || CompareVariousTable03(m_pChoJungChar, 1, false) == true
							|| CompareVariousTable04(m_pChoJungChar, 1, false) == true || CompareVariousTable04(m_pChoJungChar, 2, false) == true || CompareVariousTable04(m_pChoJungChar, 3, false) == true
							|| CompareVariousTable05(m_pChoJungChar, ALL_TABLE_05, true) == true
							|| CompareVariousTable06(m_pChoJungChar, ALL_TABLE_06, true) == true
							|| CompareVariousTable07(m_pChoJungChar, ALL_TABLE_07, true) == true)
						{//10n : S(n)-C(n) == txx�ΰ�? MAR1_5_00027
							LoggingFunction(pNode, "MAR1_5_00028+ ");

							IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
							m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
							m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)�� ����
							m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
							m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
							m_pEndingDicWord = AddTwoStr(m_pJongChar, m_pFormNTo1Word);//C(n)+S(n-1~1)
							CHAR_DELETE(m_pOldBuffer);
							int nLen = strlen(m_pEndingDicWord);
							m_pOldBuffer = new char[nLen + 1];
							memset(m_pOldBuffer, 0x00, nLen + 1);
							strcpy(m_pOldBuffer, m_pEndingDicWord);
							//11y : OldBuf = C(n)+S(n-1~1), S(n) = S(n)-C(n) && IRM���� MAR1_5_00028
							InsertArrayFeatureOld(m_pEndingDicWord);
							ReplaceHangulJongsungNull(pNode, nStart);

							GoIMR2(pNode, nStart, mStart, lStart);
						}
						else
						{
							LoggingFunction(pNode, "MAR1_5_00029+ ");

							IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
							m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
							m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)�� ����
							m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
							m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
							m_pEndingDicWord = AddTwoStr(m_pJongChar, m_pFormNTo1Word);//C(n)+S(n-1~1)
							CHAR_DELETE(m_pOldBuffer);
							int nLen = strlen(m_pEndingDicWord);
							m_pOldBuffer = new char[nLen + 1];
							memset(m_pOldBuffer, 0x00, nLen + 1);
							strcpy(m_pOldBuffer, m_pEndingDicWord);
							//11n : OldBuf = C(n)+S(n-1~1), S(n) = S(n)-C(n) && MAR2 MAR1_5_00029
							InsertArrayFeatureOld(m_pEndingDicWord);
							ReplaceHangulJongsungNull(pNode, nStart);

							GoMAR2(pNode, nStart, mStart, lStart);
						}
					}
				}
			}
		}
	}
	else
	{
		//6n : MAR2��(�ٽ� ������) MAR1_5_00030
		LoggingFunction(pNode, "MAR1_5_00030+ ");
		GoMAR2(pNode, nStart, mStart, lStart);
	}
}


/////////////////////////////////////////////////////////
//MAR2
/////////////////////////////////////////////////////////
void CNAHangulAnaly::GoMAR2(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
	LoggingFunction(pNode, "MAR2_00001+ ");
	lStart = 0;//�ʱ�ȭ
	GoMAR2_1(pNode, nStart, mStart, lStart);
}

void CNAHangulAnaly::GoMAR2_1(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
	m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)�� ����
	LoggingFunction(pNode, "MAR2_1_00001+ ");
	if (m_bOneCharMask == false && strcmp(m_pNthOneChar, "��") == 0)
	{//��	 S(n) = [mask]�� �ƴ� �������ΰ� ? MAR2_1_00001
		LoggingFunction(pNode, "MAR2_1_00002+ ");

		//0'y : MAR3�� MAR2_1_00002
		GoMAR3(pNode, nStart, mStart, lStart);
	}
	else
	{
		LoggingFunction(pNode, "MAR2_1_00003+ ");
		if((m_bOneCharMask == false || strcmp(m_pNthOneChar, "��") != 0)
			&& ((CompareVariousTable01(m_pNthOneChar, 0, false) == true
				|| CompareVariousTable02(m_pNthOneChar, 1, false) == true || CompareVariousTable02(m_pNthOneChar, 2, false) == true
				|| CompareVariousTable03(m_pNthOneChar, 0, false) == true || CompareVariousTable03(m_pNthOneChar, 1, false) == true
				|| CompareVariousTable04(m_pNthOneChar, 1, false) == true || CompareVariousTable04(m_pNthOneChar, 2, false) == true || CompareVariousTable04(m_pNthOneChar, 3, false) == true
				|| CompareVariousTable05(m_pNthOneChar, ALL_TABLE_05, true) == true
				|| CompareVariousTable06(m_pNthOneChar, ALL_TABLE_06, true) == true
				|| CompareVariousTable07(m_pNthOneChar, ALL_TABLE_07, true) == true
				|| strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0)
				|| (strcmp(m_pJongChar, "��") == 0 || strcmp(m_pJongChar, "��") == 0 || strcmp(m_pJongChar, "��") == 0)))
		{//0'n : S(n)�� [mask]||�������� �ƴϰ�, {S(n) = txx || ���⡱ || ���족 } || { C(n) == �� || �� || �� } �ΰ�? MAR2_1_00003
			LoggingFunction(pNode, "MAR2_1_00004+ ");

			//0y : IRM���� MAR2_1_00004
			GoIMR1(pNode, nStart, mStart, lStart);
		}
		else
		{
			//0n : m = n�̸� m = n - 1, *S(n~m+1)�� ending ������ �ִ°� ? MAR2_2_00001
			GoMAR2_2(pNode, nStart, mStart, lStart);
		}
	}
}

void CNAHangulAnaly::GoMAR2_2(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
	LoggingFunction(pNode, "MAR2_2_00001+ ");
	if (nStart == mStart)
	{
		mStart = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
	}
	m_pEndingDicWord = GetMiddleCountChar(pNode->pHToken->strSource, nStart, mStart, CALCULATE_STOP, CALCULATE_MINUS);//S(n~m+1)
	if (IsExistEndingDic(m_pEndingDicWord))
	{//0n : m = n�̸� m = n - 1, *S(n~m+1)�� ending ������ �ִ°� ? MAR2_2_00001
		GoMAR2_3(pNode, nStart, mStart, lStart);
	}
	else
	{
		//1n : C(n+1)+S(n~m+1)�� ending ������ �����ϴ°� ? MAR2_4_00001
		GoMAR2_4(pNode, nStart, mStart, lStart);
	}
}

void CNAHangulAnaly::GoMAR2_3(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ

	lStart = nStart;

	m_pEndingDicWord = GetMiddleCountChar(pNode->pHToken->strSource, nStart, mStart, CALCULATE_STOP, CALCULATE_MINUS);//S(n~m+1)
	LoggingFunction(pNode, "MAR2_3_00001+ ");
	if (IsExistEndingDic(m_pEndingDicWord))
	{//1y: 1 = n �� �� S(n~m+1)�� ending ������ �ִ°� ? MAR2_3_00001
		CHAR_DELETE(m_pNewBuffer);
		int nLen = strlen(m_pEndingDicWord);
		m_pNewBuffer = new char[nLen + 1];//NewBuf() = S(n~m+1)
		memset(m_pNewBuffer, 0x00, nLen + 1);
		strcpy(m_pNewBuffer, m_pEndingDicWord);
		InsertArrayFeatureNew(m_pEndingDicWord);
		m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
		m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)�� ����
		m_pJongChar = GetChojungJongsungChar(m_pOneNPluseNumChar, CHAR_JONGSUNG);//C(n+1)�� ����
		m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
		LoggingFunction(pNode, "MAR2_3_00002+ ");
		if (IsExistStemDic(m_pStemDicWord, m_stemFeature01))
		{//2y : NewBuf() = S(n~m+1) �� S(len~n+1)�� stem ������ �����ϴ°� ? MAR2_3_00002
			CHAR_DELETE(m_pEndingDicWord);
			m_pEndingDicWord = AddTwoStr(m_pJongChar, m_pEndingDicWord);//C(n+1)+S(n~m+1)
			LoggingFunction(pNode, "MAR2_3_00003+ ");
			if (IsExistEndingDic(m_pEndingDicWord, m_endingFeature01))
			{//3y : C(n+1)+S(n~m+1)�� ending ������ �����ϴ°� ? MAR2_3_00003
				m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pOneNPluseNumChar, m_nPluseSyllable);//S(len~n+1)-C(n+1)
				LoggingFunction(pNode, "MAR2_3_00004+ ");
				if (IsExistStemDic(m_pStemDicDeleteJongWord, m_stemFeature01))
				{//4y : S(len~n+1)-C(n+1)�� stem ������ �����ϴ°� ? MAR2_3_00004
					LoggingFunction(pNode, "MAR2_3_00005+ ");
					if((m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] == IS_CEHCK_FEATURE)
						|| (m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] != IS_CEHCK_FEATURE)
						|| (m_stemFeature01[STEM_DIC_AMB] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_AMB] != IS_CEHCK_FEATURE))
					{//5y : {S(len~n+1)-C(n+1) == [N] && C(n+1)+S(n~m+1) == [N]} ��{ S(len~n+1)-C(n+1) <>[N] && C(n+1)+S(n~m+1) <>[N] } || {S(len~n+1)-C(n+1) == [amb] || C(n+1)+S(n~m+1) == [amb]}�ΰ� ? MAR2_3_00005
						LoggingFunction(pNode, "MAR2_3_00006+ ");
						if((m_endingFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_pOldBuffer[ENDING_DIC_N] == IS_CEHCK_FEATURE)
							|| (m_endingFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_pOldBuffer[ENDING_DIC_N] != IS_CEHCK_FEATURE)
							|| (m_endingFeature01[STEM_DIC_AMB] != IS_CEHCK_FEATURE && m_pOldBuffer[ENDING_DIC_AMB] != IS_CEHCK_FEATURE))
						{//6y : {C(n+1)+S(n~m+1) == [n] && Oldbuf == [n]} || {C(n+1)+S(n~m+1) <>[n] && Oldbuf <>[n]} || {C(n+1)+S(n~m+1) == [amb] || Oldbuf == [amb]}�ΰ� ? MAR2_3_00006
							CHAR_DELETE(m_pStemDicWord);
							m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
							LoggingFunction(pNode, "MAR2_3_00007+ ");
							bool bFindEndingFirst = IsExistEndingDic(m_pNewBuffer, m_endingFeature02);
							bool bFindEndingSecond = IsExistEndingDic(m_pOldBuffer, m_endingFeature03);
							if((strcmp(m_pStemDicWord, "�ڽ�") == 0 || strcmp(m_pStemDicWord, "��") == 0)
								&& bFindEndingFirst == true && bFindEndingSecond == true  && (m_endingFeature02[ENDING_DIC_N] == IS_CEHCK_FEATURE || m_endingFeature02[ENDING_DIC_AMB] == IS_CEHCK_FEATURE || m_endingFeature02[ENDING_DIC_N] == IS_CEHCK_FEATURE || m_endingFeature02[ENDING_DIC_AMB] == IS_CEHCK_FEATURE))
							{//7y : {S(len~n+1) == ���ڽš� || ���С�} && {(NewBuf() == [N] || [amb]) || (OldBuf() == [N] || [amb])}�ΰ� ? MAR2_3_00007
								LoggingFunction(pNode, "MAR2_3_00008+ ");
								//8y : stem = S(len~n+1), ending = NewBuf()+OldBuf() MAR2_3_00008
								InsertStemString(pNode, m_pStemDicWord);
								SaveHTokenStem(pNode);
								m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
								InsertEndingString(pNode, m_p1thAddString);
								SaveHTokenEnding(pNode);
							}
							else
							{
								LoggingFunction(pNode, "MAR2_3_00009+ ");
								//8n : stem = S(len~n+1)-C(n+1), ending = C(n+1)+S(n~m+1)+OldBuf() MAR2_3_00009
								InsertStemString(pNode, m_pStemDicDeleteJongWord);
								SaveHTokenStem(pNode);
								m_p1thAddString = AddTwoStr(m_pEndingDicWord, m_pOldBuffer);
								InsertEndingString(pNode, m_p1thAddString);
								SaveHTokenEnding(pNode);
							}
						}
						else
						{
							m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
							CHAR_DELETE(m_pOldBuffer);
							CHAR_DELETE(m_pNewBuffer);
							int nLen = strlen(m_p1thAddString);
							m_pOldBuffer = new char[nLen + 1];
							memset(m_pOldBuffer, 0x00, nLen + 1);
							strcpy(m_pOldBuffer, m_p1thAddString);
							InsertArrayFeatureOld(m_p1thAddString);

							CHAR_DELETE(m_pNewBuffer);
							
							m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
							m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
							m_pFormLenToNWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
							
							bool bFindFirst = IsExistStemDic(m_pFormNTo1Word, m_stemFeature01);
							bool bFindSecond = IsExistStemDic(m_pFormLenToNWord, m_stemFeature02);

							LoggingFunction(pNode, "MAR2_3_00010+ ");
							if(bFindFirst == true && bFindSecond == true  
								&& (m_stemFeature01[STEM_DIC_V] != IS_CEHCK_FEATURE && m_stemFeature01[STEM_DIC_AMB] != IS_CEHCK_FEATURE) && strcmp(m_pFormNTo1Word,"����") != 0)
							{//7n : OldBuf() = NewBuf+OldBuf(), NewBuf()=Null �� stem = S(len~n+1), S(n~1)�� stem ������ �ְ�[v] || [amb] || ���������� �ƴѰ� ? MAR2_3_00010
								LoggingFunction(pNode, "MAR2_3_00011+ ");
								//8y : ���ո�� stem = S(n~1), ->end MAR2_3_00011 ���ո��� ������ ���, ��� �ΰ� ����
								InsertStemString(pNode, m_pFormNTo1Word);
								SaveHTokenStem(pNode);
								m_LinkedList->CreateSubNode(pNode);
								InsertAddSubTail(pNode, m_pFormLenToNWord);//S(len~n+1)
								InsertAddSubTail(pNode, m_pFormNTo1Word);//S(n~1)
							}
							else
							{
								LoggingFunction(pNode, "MAR2_3_00012+ ");
								//8n : ending = OldBuf() MAR2_3_00012
								InsertStemString(pNode, m_pFormNTo1Word);
								SaveHTokenStem(pNode);
								AddEndingFeature(pNode, m_pOldBuffer);
								SaveHTokenEnding(pNode);
							}
						}
					}
					else
					{
						bool bFindEndingFirst = IsExistEndingDic(m_pNewBuffer, m_endingFeature02);
						bool bFindEndingSecond = IsExistEndingDic(m_pOldBuffer, m_endingFeature03);
						LoggingFunction(pNode, "MAR2_3_00013+ ");
						if(bFindEndingFirst == true && bFindEndingSecond == true && m_endingFeature02[ENDING_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature03[ENDING_DIC_V] == IS_CEHCK_FEATURE)
						{//6n : NewBuf()�� �� == [N] && OldBuf()�� �� == [V]�ΰ� ? MAR2_3_00013
							LoggingFunction(pNode, "MAR2_3_00014+ ");

							//7y : Error(20030128 ������ ����, ����ȥ������) MAR2_3_00014
							GoErrorFunction(pNode);
						}
						else
						{
							m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
							CHAR_DELETE(m_pOldBuffer);
							CHAR_DELETE(m_pNewBuffer);
							int nLen = strlen(m_p1thAddString);
							m_pOldBuffer = new char[nLen + 1];
							memset(m_pOldBuffer, 0x00, nLen + 1);
							strcpy(m_pOldBuffer, m_p1thAddString);
							InsertArrayFeatureOld(m_p1thAddString);

							CHAR_DELETE(m_pNewBuffer);

							m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
							m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
							m_pFormLenToNWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)

							bool bFindFirst = IsExistStemDic(m_pFormNTo1Word, m_stemFeature01);
							bool bFindSecond = IsExistStemDic(m_pFormLenToNWord, m_stemFeature02);

							LoggingFunction(pNode, "MAR2_3_00015+ ");
							if (bFindFirst == true && bFindSecond == true
								&& (m_stemFeature01[STEM_DIC_V] != IS_CEHCK_FEATURE && m_stemFeature01[STEM_DIC_AMB] != IS_CEHCK_FEATURE) && strcmp(m_pFormNTo1Word, "����") != 0)
							{//7n : OldBuf() = NewBuf+OldBuf(), NewBuf()=Null �� stem = S(len~n+1), S(n~1)�� stem ������ �ְ�[v] || [amb] || ���������� �ƴѰ� ? MAR2_3_00010
								LoggingFunction(pNode, "MAR2_3_00016+ ");
								//8y : ���ո�� stem = S(n~1), ->end MAR2_3_00011 ���ո��� ������ ���, ��� �ΰ� ����
								InsertStemString(pNode, m_pFormNTo1Word);
								SaveHTokenStem(pNode);
								m_LinkedList->CreateSubNode(pNode);
								InsertAddSubTail(pNode, m_pFormLenToNWord);//S(len~n+1)
								InsertAddSubTail(pNode, m_pFormNTo1Word);//S(n~1)
							}
							else
							{
								LoggingFunction(pNode, "MAR2_3_00017+ ");
								//8n : ending = OldBuf() MAR2_3_00012
								InsertStemString(pNode, m_pFormNTo1Word);
								SaveHTokenStem(pNode);
								AddEndingFeature(pNode, m_pOldBuffer);
								SaveHTokenEnding(pNode);
							}
						}
					}
				}
				else
				{
					m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
					CHAR_DELETE(m_pOldBuffer);
					CHAR_DELETE(m_pNewBuffer);
					int nLen = strlen(m_p1thAddString);
					m_pOldBuffer = new char[nLen + 1];
					memset(m_pOldBuffer, 0x00, nLen + 1);
					strcpy(m_pOldBuffer, m_p1thAddString);
					InsertArrayFeatureOld(m_p1thAddString);

					CHAR_DELETE(m_pNewBuffer);

					m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
					m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
					m_pFormLenToNWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)

					bool bFindFirst = IsExistStemDic(m_pFormNTo1Word, m_stemFeature01);
					bool bFindSecond = IsExistStemDic(m_pFormLenToNWord, m_stemFeature02);

					LoggingFunction(pNode, "MAR2_3_00018+ ");
					if (bFindFirst == true && bFindSecond == true
						&& (m_stemFeature01[STEM_DIC_V] != IS_CEHCK_FEATURE && m_stemFeature01[STEM_DIC_AMB] != IS_CEHCK_FEATURE) && strcmp(m_pFormNTo1Word, "����") != 0)
					{//5n : OldBuf() = NewBuf+OldBuf() �� stem = S(len~n+1), S(n~1)�� stem ������ �ְ�[v] || [amb] || ���������� �ƴѰ� ? MAR2_3_00018
						LoggingFunction(pNode, "MAR2_3_00019+ ");
						//6y : ���ո�� stem = S(n~1), ->end MAR2_3_00019 ���ո��� ������ ���, ��� �ΰ� ����
						InsertStemString(pNode, m_pFormNTo1Word);
						SaveHTokenStem(pNode);
						m_LinkedList->CreateSubNode(pNode);
						InsertAddSubTail(pNode, m_pFormLenToNWord);//S(len~n+1)
						InsertAddSubTail(pNode, m_pFormNTo1Word);//S(n~1)
					}
					else
					{
						LoggingFunction(pNode, "MAR2_3_00020+ ");
						//6n : ending = OldBuf() MAR2_3_00020
						InsertStemString(pNode, m_pFormNTo1Word);
						SaveHTokenStem(pNode);
						AddEndingFeature(pNode, m_pOldBuffer);
						SaveHTokenEnding(pNode);
					}
				}
			}
			else
			{
				IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
				m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
				m_pFormLenToNWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
				bool bFindSecond = IsExistStemDic(m_pFormLenToNWord, m_stemFeature01);
				bool bFindNewBuffer = IsExistStemDic(m_pNewBuffer, m_stemFeature02);
				bool bFindOldBuffer = IsExistStemDic(m_pOldBuffer, m_stemFeature03);
				LoggingFunction(pNode, "MAR2_3_00021+ ");
				if((m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_stemFeature02[ENDING_DIC_N] == IS_CEHCK_FEATURE)
					|| (m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_stemFeature02[ENDING_DIC_N] != IS_CEHCK_FEATURE)
					|| (m_stemFeature01[STEM_DIC_AMB] != IS_CEHCK_FEATURE && m_stemFeature02[ENDING_DIC_AMB] != IS_CEHCK_FEATURE))
				{//4n : {S(len~n+1) == [N] && NewBuf() == [N]} || {S(len~n+1)<>[N] && NewBuf()<>[N]} || { S(len~n+1) == [Amb]} || { NewBuf() == [Amb]}�ΰ� ? MAR2_3_00021
					LoggingFunction(pNode, "MAR2_3_00022+ ");
					if((m_stemFeature02[ENDING_DIC_N] == IS_CEHCK_FEATURE && m_stemFeature03[ENDING_DIC_V] == IS_CEHCK_FEATURE)
						|| m_stemFeature02[ENDING_DIC_PPR] == IS_CEHCK_FEATURE
						|| (strcmp(m_pNewBuffer, "��") == 0 && (m_pOldBuffer != NULL && strcmp(m_pOldBuffer, "��") == 0))
						|| (strcmp(m_pNewBuffer, "��") && (m_pOldBuffer != NULL && strcmp(m_pOldBuffer, "��") == 0)))
					{//5y : {NewBuf()�� �� == [N] && OldBuf()�� �� == [V]} || {NewBuf()�� �� == [Pps]} || {NewBuf()�� �� == ������ && OldBuf() == ������} || {NewBuf()�� �� == ������ && OldBuf() == ������} || {NewBuf()�� �� == ������ && OldBuf() == ������}�ΰ� ? MAR2_3_00022
						LoggingFunction(pNode, "MAR2_3_00023+ ");

						//6y : Error(20030128 ������ ����, �������̻硱 ���ؿ��̻硱, �������Ͱ��ԡ�, ������������, ��������������) MAR2_3_00023
						GoErrorFunction(pNode);
					}
					else
					{
						m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
						CHAR_DELETE(m_pOldBuffer);
						CHAR_DELETE(m_pNewBuffer);
						int nLen = strlen(m_p1thAddString);
						m_pOldBuffer = new char[nLen + 1];
						memset(m_pOldBuffer, 0x00, nLen + 1);
						strcpy(m_pOldBuffer, m_p1thAddString);
						InsertArrayFeatureOld(m_p1thAddString);

						CHAR_DELETE(m_pNewBuffer);

						m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
						m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
						m_pFormLenToNWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)

						bool bFindFirst = IsExistStemDic(m_pFormNTo1Word, m_stemFeature01);
						bool bFindSecond = IsExistStemDic(m_pFormLenToNWord, m_stemFeature02);

						LoggingFunction(pNode, "MAR2_3_00024+ ");
						if (bFindFirst == true && bFindSecond == true
							&& m_stemFeature02[STEM_DIC_N] == IS_CEHCK_FEATURE
							&&(m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_ADV] == IS_CEHCK_FEATURE)
							&& strcmp(m_pFormNTo1Word, "����") != 0 && strcmp(m_pFormNTo1Word, "����") != 0 && strcmp(m_pFormNTo1Word, "����") != 0)
						{//6n : OldBuf() = NewBuf+OldBuf(), NewBuf()=Null �� stem = S(len~n+1), S(n~1)�� stem ������ �ְ�, { S(len~n+1) == [N] } && {S(n~1) == [N] || [Adv]} && {S(n~1) != �����Ρ�} && {S(n~1) != ��������} && {S(n~1) != �����̡�}�ΰ� ? MAR2_3_00024
							LoggingFunction(pNode, "MAR2_3_00025+ ");
							//7y : ���ո�� stem = S(n~1), ->end MAR2_3_00025 ���ո��� ������ ���, ��� �ΰ� ����
							InsertStemString(pNode, m_pFormNTo1Word);
							SaveHTokenStem(pNode);
							m_LinkedList->CreateSubNode(pNode);
							InsertAddSubTail(pNode, m_pFormLenToNWord);//S(len~n+1)
							InsertAddSubTail(pNode, m_pFormNTo1Word);//S(n~1)
						}
						else
						{
							LoggingFunction(pNode, "MAR2_3_00026+ ");
							//7n : ending = OldBuf() MAR2_3_00026
							InsertStemString(pNode, m_pFormNTo1Word);
							SaveHTokenStem(pNode);
							AddEndingFeature(pNode, m_pOldBuffer);
							SaveHTokenEnding(pNode);
						}
					}
				}
				else
				{
					LoggingFunction(pNode, "MAR2_3_00027+ ");

					//5n : Error  MAR2_3_00027
					GoErrorFunction(pNode);
				}
			}
		}
		else
		{
			IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
			m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
			m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)�� ����
			LoggingFunction(pNode, "MAR2_3_00028+ ");
			IsExistEndingDic(m_pNewBuffer, m_endingFeature01);//m_pNewBuffer�� �����׼���
			if((strcmp(m_pOneNPluseNumChar, "��") != 0 && strcmp(m_pOneNPluseNumChar, "��") != 0) && (m_pNewBuffer != NULL && strcmp(m_pNewBuffer, "��")) && m_endingFeature01[ENDING_DIC_PRS] == IS_CEHCK_FEATURE || m_endingFeature01[ENDING_DIC_PSS] == IS_CEHCK_FEATURE)
			{//3n: NOT{ S(n+1) == ���ϡ�|| ������ && NewBuf() == ���á�} && NewBuf() == [Prs]||[Pss]�ΰ�? MAR2_3_00028
				LoggingFunction(pNode, "MAR2_3_00029+ ");

				//4y: Error  MAR2_3_00029
				GoErrorFunction(pNode);
			}
			else
			{
				LoggingFunction(pNode, "MAR2_3_00030(n++��n>len)+ ");
				int nMovingLeftPosition = ReturnNumberMovingLeftPositionWord(pNode->pHToken->strSource,nStart);
				if(nMovingLeftPosition < 0)
				{//4n: n++��n>len? MAR2_3_00030
					LoggingFunction(pNode, "MAR2_3_00031+ ");

					//5y: Error MAR2_3_00031
					GoErrorFunction(pNode);
				}
				else
				{
					LoggingFunction(pNode, "MAR2_3_00032+ ");
					//5n: <����> -> ��� ����. MAR2_3_00032 ���������϶�� �ǹ���(?) ������ �ǹ� �ľ� �ȵ�
					ReplaceHangulJongsungNull(pNode, nStart);

					GoMAR2_1(pNode, nStart, mStart, lStart);
				}
			}
		}
	}
	else
	{ 
		IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
		int nMovingLeftPosition = ReturnNumberMovingLeftPositionWord(pNode->pHToken->strSource, nStart);
		LoggingFunction(pNode, "MAR2_3_00033(n++->n>len)+ ");
		if (nMovingLeftPosition <= 0)
		{//2n: n++ �� n > len�ΰ�? MAR2_3_00033
			LoggingFunction(pNode, "MAR2_3_00034+ ");
			//3y: Error MAR2_3_00034
			GoErrorFunction(pNode);
		}
		else
		{
			m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
			m_pFormLenToNWord = GetMiddleCountChar(pNode->pHToken->strSource, nStart, m_nMinusSyllable, CALCULATE_STOP, CALCULATE_STOP);
			LoggingFunction(pNode, "MAR2_3_00035+ ");
			if(IsExistEndingDic(m_pFormLenToNWord))
			{//3n:  S(n~n-1)�� ending ������ �ִ°�? MAR2_3_00035
				LoggingFunction(pNode, "MAR2_3_00036+ ");
				m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1)�� ����
				//4y : NewBuf() = S(n-1)+NewBuf() �� ��� ����. MAR2_3_00036
				m_p1thAddString = AddTwoStr(m_pNthOneChar, m_pNewBuffer);
				CHAR_DELETE(m_pNewBuffer);
				int nLen = strlen(m_p1thAddString);
				m_pNewBuffer = new char[nLen + 1];
				memset(m_pNewBuffer, 0x00, nLen + 1);
				strcpy(m_pNewBuffer, m_p1thAddString);
				InsertArrayFeatureNew(m_p1thAddString);

				GoMAR2_1(pNode,nStart,mStart,lStart);
			}
			else
			{
				CHAR_DELETE(m_pFormLenToNWord);
				m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
				m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
				m_pFormLenToNWord = GetMiddleCountChar(pNode->pHToken->strSource, m_nPluseSyllable, m_nMinusSyllable, CALCULATE_STOP, CALCULATE_STOP);//S(n+1~n-1)
				//(n + 1 > len�̸�, Error)
				if(m_nPluseSyllable == nStart)
				{
					LoggingFunction(pNode, "MAR2_3_00037 n+1 > len�̸�, Error + ");
					GoErrorFunction(pNode);
				}
				else
				{
					LoggingFunction(pNode, "MAR2_3_00037+ ");
					if (IsExistStemDic(m_pFormLenToNWord))
					{//4n : S(n+1~n-1)�� Ending ������ �ִ°�?(n+1 > len�̸�, Error) MAR2_3_00037
						LoggingFunction(pNode, "MAR2_3_00038+ ");
						//5y : NewBuf() = S(n-1)+NewBuf() �� ��� ����. MAR2_3_00038
						m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1)�� ����
						m_p1thAddString = AddTwoStr(m_pNthOneChar, m_pNewBuffer);
						CHAR_DELETE(m_pNewBuffer);
						int nLen = strlen(m_p1thAddString);
						m_pNewBuffer = new char[nLen + 1];
						memset(m_pNewBuffer, 0x00, nLen + 1);
						strcpy(m_pNewBuffer, m_p1thAddString);
						InsertArrayFeatureNew(m_p1thAddString);

						GoMAR2_1(pNode, nStart, mStart, lStart);
					}
					else
					{
						CHAR_DELETE(m_pFormLenToNWord);
						m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_PLUS);//n-2
						m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
						m_pFormLenToNWord = GetMiddleCountChar(pNode->pHToken->strSource, m_nPluseSyllable, m_nMinusSyllable, CALCULATE_STOP, CALCULATE_STOP);//S(n+2~n-2)
						//(n + 1 > len�̸�, Error)
						if (m_nPluseSyllable == nStart)
						{
							LoggingFunction(pNode, "MAR2_3_00039 n+1 > len�̸�, Error + ");
							GoErrorFunction(pNode);
						}
						else
						{
							LoggingFunction(pNode, "MAR2_3_00039+ ");
							if (IsExistStemDic(m_pFormLenToNWord))
							{ //5n : S(n+2~n-2)�� ending ������ �ִ°�? (n+1 > len�̸�, Error) MAR2_3_00039
								LoggingFunction(pNode, "MAR2_3_00040+ ");
								//6y : NewBuf() = S(n-1)+NewBuf() �� ��� ����. MAR2_3_00040
								m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
								m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1)�� ����
								m_p1thAddString = AddTwoStr(m_pNthOneChar, m_pNewBuffer);
								CHAR_DELETE(m_pNewBuffer);
								int nLen = strlen(m_p1thAddString);
								m_pNewBuffer = new char[nLen + 1];
								memset(m_pNewBuffer, 0x00, nLen + 1);
								strcpy(m_pNewBuffer, m_p1thAddString);
								InsertArrayFeatureNew(m_p1thAddString);

								GoMAR2_1(pNode, nStart, mStart, lStart);
							}
							else
							{
								LoggingFunction(pNode, "MAR2_3_00041+ ");

								//6n : Errorr  ������ ���� ���������硱, ����ǰ����, ��������ġ��⡱ MAR2_3_00041
								GoErrorFunction(pNode);
							}
						}
					}
				}
			}
		}
	}
}

void CNAHangulAnaly::GoMAR2_4(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
	m_pEndingDicWord = GetMiddleCountChar(pNode->pHToken->strSource, nStart, mStart, CALCULATE_STOP, CALCULATE_MINUS);//S(n~m+1)
	m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
	m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)�� ����
	m_pJongChar = GetChojungJongsungChar(m_pOneNPluseNumChar, CHAR_JONGSUNG);//C(n+1)�� ����
	m_pFormLenToNWord = AddTwoStr(m_pJongChar, m_pEndingDicWord);//C(n+1)+S(n~m+1)
	LoggingFunction(pNode, "MAR2_4_00001+ ");
	if (IsExistEndingDic(m_pFormLenToNWord))
	{//1n : C(n+1)+S(n~m+1)�� ending ������ �����ϴ°� ? MAR2_4_00001
		m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
		m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pOneNPluseNumChar, m_nPluseSyllable);//S(len~n+1)-C(n+1)
		LoggingFunction(pNode, "MAR2_4_00002+ ");
		if (IsExistEndingDic(m_pStemDicDeleteJongWord))
		{//2y : S(len~n+1)-C(n+1)�� stem ������ �����ϴ°� ? MAR2_4_00002
			LoggingFunction(pNode, "MAR2_4_00003+ ");
			//3y : stem = S(len~n+1)-C(n+1), ending = C(n+1)+S(n~m+1)+OldBuf()��end MAR2_4_00003
			InsertStemString(pNode, m_pStemDicDeleteJongWord);
			SaveHTokenStem(pNode);
			InsertEndingString(pNode, m_pFormLenToNWord);
			SaveHTokenEnding(pNode);
		}
		else
		{
			IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
			m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
			m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)�� ����
			m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
			m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, mStart, 1, CALCULATE_MINUS);//m+1
			m_pFormLenToNWord = GetMiddleCountChar(pNode->pHToken->strSource, m_nPluseSyllable, m_nMinusSyllable, CALCULATE_STOP, CALCULATE_STOP);//S(n-1~m+1)
			m_pEndingDicWord = AddTwoStr(m_pJongChar, m_pFormLenToNWord);//C(n)+S(n-1~m+1)
			LoggingFunction(pNode, "MAR2_4_00004+ ");
			if (IsExistEndingDic(m_pFormLenToNWord))
			{//3n : C(n)+S(n-1~m+1)�� ending ������ �����ϴ°� ? MAR2_4_00004
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
				m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
				LoggingFunction(pNode, "MAR2_4_00005+ ");
				if(IsExistStemDic(m_pStemDicDeleteJongWord))
				{//4y : S(len~n)-C(n)�� stem ������ MAR2_4_00006 ? MAR2_4_00005
					LoggingFunction(pNode, "MAR2_4_00005+ ");
					m_p1thAddString = AddTwoStr(m_pJongChar, m_pFormLenToNWord);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					//5y : stem = S(len~n)-C(n), ending = C(n)+S(n-1~m+1)+OldBuf() MAR2_4_00006
					InsertStemString(pNode, m_pStemDicDeleteJongWord);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_p2thAddString);
					SaveHTokenEnding(pNode);
				}
				else
				{
					LoggingFunction(pNode, "MAR2_4_00007(m=m+1)+ ");
					//5n : OldBuf = C(n)+S(n-1~m+1)+OldBuf(), S(n) = S(n)-C(n), m=m+1 �� MA_02() ��� MAR2_4_00007
					m_p1thAddString = AddTwoStr(m_pJongChar, m_pFormLenToNWord);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					CHAR_DELETE(m_pOldBuffer);
					int nLen = strlen(m_p2thAddString);
					m_pOldBuffer = new char[nLen + 1];
					memset(m_pOldBuffer, 0x00, nLen + 1);
					strcpy(m_pOldBuffer, m_p2thAddString);
					InsertArrayFeatureOld(m_p2thAddString);

					ReplaceHangulJongsungNull(pNode, nStart);//S(n)=S(n)-C(n) ��������
					ReturnNumberMovingLeftPositionWord(pNode->pHToken->strSource, mStart);//m=m+1
					
					GoMAR2_1(pNode,nStart,mStart,lStart);
				}
			}
			else
			{
				IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
				LoggingFunction(pNode, "MAR2_4_00008+ ");
				m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
				m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)�� ����
				m_pJongChar = GetChojungJongsungChar(m_pOneNPluseNumChar, CHAR_JONGSUNG);//C(n+1)�� ����
				m_pFormLenToNWord = AddTwoStr(m_pJongChar, m_pEndingDicWord);//C(n+1)+S(n~m+1)
				//4n : ending = C(n+1)+S(n~m+1)+OldBuf() , tmp_stem = S(len~n+1)-C(n+1) �� MA_02() ��� MAR2_4_00008
				m_p1thAddString = AddTwoStr(m_pFormLenToNWord, m_pOldBuffer);
				InsertEndingString(pNode, m_p1thAddString);
				SaveHTokenEnding(pNode);

				InitOneWordMarsk();//tmp_stem = S(len~n+1)-C(n+1)

				GoMAR2_1(pNode, nStart, mStart, lStart);
			}
		}
	}
	else
	{ 
		IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
		m_pEndingDicWord = GetMiddleCountChar(pNode->pHToken->strSource, nStart, mStart, CALCULATE_PLUS, CALCULATE_MINUS);//S(n-1~m+1)
		m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
		m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)�� ����
		m_pFormLenToNWord = AddTwoStr(m_pJongChar, m_pEndingDicWord);//C(n)+S(n-1~m+1)
		LoggingFunction(pNode, "MAR2_4_00009+ ");
		if (IsExistEndingDic(m_pFormLenToNWord, m_endingFeature01))
		{//2n : C(n)+S(n-1~m+1)�� ending ������ �����ϴ°�? MAR2_4_00009
			LoggingFunction(pNode, "MAR2_4_00010+ ");
			if(m_endingFeature01[ENDING_DIC_PSS] == IS_CEHCK_FEATURE)
			{//3y : C(n)+S(n-1~m+1) == [Pss]�ΰ�?(20030128, ������ ����) MAR2_4_00010
				LoggingFunction(pNode, "MAR2_4_00011+ ");

				//4y : Error(���Ǻΰ�����) MAR2_4_00011
				GoErrorFunction(pNode);
			}
			else
			{
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
				m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
				LoggingFunction(pNode, "MAR2_4_00012+ ");
				if (IsExistStemDic(m_pStemDicDeleteJongWord))
				{//4n : S(len~n)-C(n)�� stem ������ �����ϴ°�? MAR2_4_00012
					LoggingFunction(pNode, "MAR2_4_00013+ ");
					m_p1thAddString = AddTwoStr(m_pJongChar, m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					//5y : stem = S(len~n)-C(n), ending = C(n)+NewBuf()+oldBuf() MAR2_4_00013
					InsertStemString(pNode, m_pStemDicDeleteJongWord);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_p2thAddString);
					SaveHTokenEnding(pNode);
				}
				else
				{	
					LoggingFunction(pNode, "MAR2_4_00014( m = m+1 ->��)+ ");
					m_p1thAddString = AddTwoStr(m_pJongChar, m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p2thAddString, m_pOldBuffer);
					//5n : OldBuf = C(n)+NewBuf()+OldBuf(), S(n) = S(n)-C(n), NewBuf()=Null, m = m+1 �� MA_02() ��� MAR2_4_00014
					CHAR_DELETE(m_pOldBuffer);
					CHAR_DELETE(m_pNewBuffer);
					int nLen = strlen(m_p2thAddString);
					m_pOldBuffer = new char[nLen + 1];
					memset(m_pOldBuffer, 0x00, nLen + 1);
					strcpy(m_pOldBuffer, m_p2thAddString);
					InsertArrayFeatureOld(m_p2thAddString);

					ReplaceHangulJongsungNull(pNode, nStart);//S(n)=S(n)-C(n) ��������

					ReturnNumberMovingLeftPositionWord(pNode->pHToken->strSource, mStart);//m=m+1
					
					GoMAR2_1(pNode, nStart, mStart, lStart);
				}
			}
		}
		else
		{
			LoggingFunction(pNode, "MAR2_4_00015+ ");
			if (IsExistEndingDic(m_pJongChar))
			{//3n : C(n)�� ending ������ �ִ°�?(�� ����, ���δٴµ�) MAR2_4_00015
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
				m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
				LoggingFunction(pNode, "MAR2_4_00016+ ");
				if (IsExistStemDic(m_pStemDicDeleteJongWord, m_stemFeature01))
				{//4y : S(len~n)-C(n)�� stem ������ �ִ°�? MAR2_4_00016
					LoggingFunction(pNode, "MAR2_4_00017+ ");
					if ((strcmp(m_pJongChar, "��") == 0 || strcmp(m_pJongChar, "��") == 0) && m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE)
					{//5y : {C(n) == ������||������} && {S(len~n-C(n) == [N]}�ΰ�? MAR2_4_00017
						LoggingFunction(pNode, "MAR2_4_00018+ ");

						//6y : Error MAR2_4_00018
						GoErrorFunction(pNode);
					}
					else
					{
						LoggingFunction(pNode, "MAR2_4_00019+ ");
						m_p1thAddString = AddTwoStr(m_pJongChar, m_pNewBuffer);
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
						//6n : stem = S(len~n)-C(n), ending = C(n)+NewBuf()+OldBuf() MAR2_4_00019
						InsertStemString(pNode, m_pStemDicDeleteJongWord);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, m_p2thAddString);
						SaveHTokenEnding(pNode);
					}
				}
				else
				{
					IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
					LoggingFunction(pNode, "MAR2_4_00020+ ");
					m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
					if (IsExistEndingDic(m_pNthOneChar, m_endingFeature01))
					{//5n : S(n)�� ending ������ �ִ°�? MAR2_4_00020
						m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
						m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)�� ����
						m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
						LoggingFunction(pNode, "MAR2_4_00021+ ");
						if (IsExistStemDic(m_pStemDicDeleteJongWord, m_stemFeature01))
						{//6y : S(len~n+1)�� stem ������ �ִ°�? MAR2_4_00021
							LoggingFunction(pNode, "MAR2_4_00022+ ");
							if((m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] == IS_CEHCK_FEATURE) || (m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] != IS_CEHCK_FEATURE))
							{//7y : {S_S(len~n+1) == [N] && E_S(n) == [N]} || {S_S(len~n+1) != [N] && E_S(n) != [N]}�ΰ�? MAR2_4_00022
								LoggingFunction(pNode, "MAR2_4_00023+ ");
								m_p1thAddString = AddTwoStr(m_pNthOneChar, m_pNewBuffer);
								m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
								//8y : stem = S(len~n+1), ending = S(n)+NewBuf()+OldBuf() MAR2_4_00023
								InsertStemString(pNode, m_pStemDicWord);//S(len~n+1)
								SaveHTokenStem(pNode);
								InsertEndingString(pNode, m_p2thAddString);
								SaveHTokenEnding(pNode);
							}
							else
							{
								LoggingFunction(pNode, "MAR2_4_00024+ ");

								//8n : Error MAR2_4_00024
								GoErrorFunction(pNode);
							}
						}
						else
						{
							LoggingFunction(pNode, "MAR2_4_00025+ ");

							//7n : Error MAR2_4_00025
							GoErrorFunction(pNode);
						}
					}
					else
					{
						LoggingFunction(pNode, "MAR2_4_00026+ ");

						//6n : Error MAR2_4_00026
						GoErrorFunction(pNode);
					}
				}
			}
			else
			{ 
				IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
				m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
				m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
				m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)�� ����
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
				LoggingFunction(pNode, "MAR2_4_00027+ ");
				if (IsExistStemDic(m_pStemDicWord, m_stemFeature01))
				{//4n : S(len~n+1)�� stem ������ �ִ°�? MAR2_4_00027
					m_p1thAddString = AddTwoStr(m_pNthOneChar,m_pNewBuffer);
					LoggingFunction(pNode, "MAR2_4_00028+ ");
					if (IsExistStemDic(m_p1thAddString))
					{//5y : S(n)+NewBuf()�� stem ������ �ִ°�? MAR2_4_00028
						LoggingFunction(pNode, "MAR2_4_00029+ ");

						//6y : Error MAR2_4_00029
						GoErrorFunction(pNode);
					}
					else
					{
						m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
						bool bFind = IsExistStemDic(m_pFormNTo1Word, m_stemFeature02);
						LoggingFunction(pNode, "MAR2_4_00030+ ");
						if(strcmp(m_pFormNTo1Word,"����") != 0 && bFind == true && (m_stemFeature02[STEM_DIC_V] != IS_CEHCK_FEATURE && m_stemFeature02[STEM_DIC_AMB] != IS_CEHCK_FEATURE))
						{//6n : Stem = S(len~n+1), S(n~1) != �������� && S(n~1)�� stem ������ �ְ� [v] || [amb]�� �ƴѰ�? MAR2_4_00030
							LoggingFunction(pNode, "MAR2_4_00031+ ");
							if(m_stemFeature01[STEM_DIC_V] == IS_CEHCK_FEATURE)
							{//7y : S(len~n+1) == [V]�ΰ�? MAR2_4_00031
								LoggingFunction(pNode, "MAR2_4_00032+ ");
								m_p1thAddString = AddTwoStr(m_pNthOneChar, m_pNewBuffer);
								m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
								//8y : ending = S(n)+NewBuf()+OldBuf() MAR2_4_00032
								InsertStemString(pNode, m_pStemDicWord);//S(len~n+1)
								SaveHTokenStem(pNode);
								InsertEndingString(pNode, m_p2thAddString);
								SaveHTokenEnding(pNode);
							}
							else
							{
								LoggingFunction(pNode, "MAR2_4_00033+ ");
								//8n : ���ո�� stem = S(n~1),-> end MAR2_4_00033 ���ո��� ������ ���, ��� �ΰ� ����
								InsertStemString(pNode, m_pFormNTo1Word);
								SaveHTokenStem(pNode);
								m_LinkedList->CreateSubNode(pNode);
								InsertAddSubTail(pNode, m_pStemDicWord);//S(len~n+1)
								InsertAddSubTail(pNode, m_pFormNTo1Word);//S(n~1)
							}
						}
						else
						{
							LoggingFunction(pNode, "MAR2_4_00034+ ");
							if (IsExistEndingDic(m_pNthOneChar, m_endingFeature01))
							{//7n : S(n)�� ending������ �ִ°�? MAR2_4_00034
								IsExistEndingDic(m_pNewBuffer, m_endingFeature02);//m_pNewBuffer�� �����׼���
								LoggingFunction(pNode, "MAR2_4_00035+ ");
								if((m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature02[STEM_DIC_N] == IS_CEHCK_FEATURE)
									|| (m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature02[STEM_DIC_N] != IS_CEHCK_FEATURE))
								{//8y : {S(len~n+1) == [N] && NewBuf() == [N]} || {S(len~n+1) <> [N] && NewBuf() <> [N]}�ΰ�? MAR2_4_00035
									LoggingFunction(pNode, "MAR2_4_00036+ ");
									m_p1thAddString = AddTwoStr(m_pNthOneChar, m_pNewBuffer);
									m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
									//9y : S(len~n+1), ending = S(n)+NewBuf()+OldBuf() MAR2_4_00036
									InsertStemString(pNode, m_pStemDicWord);//S(len~n+1)
									SaveHTokenStem(pNode);
									InsertEndingString(pNode, m_p2thAddString);
									SaveHTokenEnding(pNode);
								}
								else
								{
									LoggingFunction(pNode, "MAR2_4_00037+ ");

									//9n : error MAR2_4_00037
									GoErrorFunction(pNode);
								}
							}
							else
							{
								LoggingFunction(pNode, "MAR2_4_00038+ ");

								//8n : error MAR2_4_00038
								GoErrorFunction(pNode);
							}
						}
					}
				}
				else
				{
					LoggingFunction(pNode, "MAR2_4_00039+ ");
					if(m_pNewBuffer == NULL || strcmp(m_pNewBuffer,"") == 0)
					{//5n : �� NewBuf() ==Null? MAR2_4_00039
						LoggingFunction(pNode, "MAR2_4_00040+ ");

						//6y : MAR3 MAR2_4_00040
						GoMAR3(pNode, nStart, mStart, lStart);
					}
					else
					{
						bool bSameWrod = IsSameWordPos(pNode, lStart);
						LoggingFunction(pNode, "MAR2_4_00041+ ");
						if(bSameWrod == true)
						{//6n : l == last(of NewBuf)�ΰ�? MAR2_4_00041
							m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
							CHAR_DELETE(m_pOldBuffer);
							CHAR_DELETE(m_pNewBuffer);
							int nLen = strlen(m_p1thAddString);
							m_pOldBuffer = new char[nLen + 1];
							memset(m_pOldBuffer, 0x00, nLen + 1);
							strcpy(m_pOldBuffer, m_p1thAddString);
							InsertArrayFeatureOld(m_p1thAddString);

							ChangePosAnyStartPosition(pNode, nStart, mStart, lStart, CHANGE_SECOND_START);//m=last(of Old Buf)
							m_pEndingDicWord = GetMiddleCountChar(pNode->pHToken->strSource, nStart, mStart, CALCULATE_STOP, CALCULATE_MINUS);//S(n~m+1)
							LoggingFunction(pNode, "MAR2_4_00042+ ");
							if (IsExistEndingDic(m_pEndingDicWord))
							{//7y : OldBuf() = NewBuf()+OldBuf() , NewBuf()=Null �� m = last(of Old Buf) �� *S(n~m+1)�� ending������ �ִ°�? MAR2_4_00042
								LoggingFunction(pNode, "MAR2_4_00043(l=n �� ��)+ ");
								//8y : l =n �� ��� ���� MAR2_4_00043
								lStart = nStart;

								GoMAR2_3(pNode,nStart,mStart,lStart);
							}
							else
							{
								CHAR_DELETE(m_pEndingDicWord);
								m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
								LoggingFunction(pNode, "MAR2_4_00044+ ");
								if (IsExistStemDic(m_pStemDicWord))
								{//8n : S(len~n)�� stem ������ �ִ°� MAR2_4_00044
									LoggingFunction(pNode, "MAR2_4_00045+ ");
									//9y : ending = OldBuf(), stem = S(len~n) MAR2_4_00045
									InsertStemString(pNode, m_pStemDicWord);//S(len~n+1)
									SaveHTokenStem(pNode);
									InsertEndingString(pNode, m_pOldBuffer);
									SaveHTokenEnding(pNode);
								}
								else
								{
									LoggingFunction(pNode, "MAR2_4_00046+ ");

									//9n : MAR3���� MAR2_4_00046
									GoMAR3(pNode, nStart, mStart, lStart);
								}
							}
						}
						else
						{
							LoggingFunction(pNode, "MAR2_4_00047+ ");
							//7n : OldBuf() = NewBuf()+OldBuf(), NewBuf()=Null MAR2_4_00047
							m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
							CHAR_DELETE(m_pOldBuffer);
							CHAR_DELETE(m_pNewBuffer);
							int nLen = strlen(m_p1thAddString);
							m_pOldBuffer = new char[nLen + 1];
							memset(m_pOldBuffer, 0x00, nLen + 1);
							strcpy(m_pOldBuffer, m_p1thAddString);
							InsertArrayFeatureOld(m_p1thAddString);

							LoggingFunction(pNode, "MAR2_4_00048+ ");
							ChangePosAnyStartPosition(pNode, nStart, mStart, lStart, CHANGE_FIRST_START);//n = last(of OldBuf)+1
							nStart -= 2;
							ChangePosAnyStartPosition(pNode, nStart, mStart, lStart, CHANGE_SECOND_START);//m = last (of OldBuf)
							//�� n = last(of OldBuf)+1, m = last (of OldBuf) MAR2_4_00048
							m_pEndingDicWord = GetMiddleCountChar(pNode->pHToken->strSource, nStart, mStart, CALCULATE_STOP, CALCULATE_MINUS);//S(n~m+1)
							if (IsExistEndingDic(m_pEndingDicWord))
							{//�� *S(n~m+1)�� ending ������ �����ϴ°�? MAR2_4_00049
								LoggingFunction(pNode, "MAR2_4_00050( l =n �� ��)+ ");
								//8y : l =n �� ��� ���� MAR2_4_00050
								lStart = nStart;

								GoMAR2_3(pNode, nStart, mStart, lStart);
							}
							else
							{
								LoggingFunction(pNode, "MAR2_4_00051+ ");
								//8n : ending = OldBuf(), MAR3���� MAR2_4_00051
								InsertEndingString(pNode, m_pOldBuffer);
								SaveHTokenEnding(pNode);

								GoMAR3(pNode, nStart, mStart, lStart);
							}
						}
					}
				}
			}
		}
	}
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//MAR3
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CNAHangulAnaly::GoMAR3(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
	LoggingFunction(pNode, "MAR3_00001+ ");
	if(strcmp(m_pNthOneChar, "��") == 0)
	{//S(n) == �������ΰ� ? MAR3_00001
		LoggingFunction(pNode, "MAR3_00002+ ");
		//0y : S(n) = [mask], ->MAR1��  ��� MAR3_00002
		SetOneWordMarsk();
		GoMAR1_2(pNode, nStart, mStart, lStart);
	}
	else
	{
		LoggingFunction(pNode, "MAR3_00003+ ");
		if (strcmp(m_pNthOneChar, "��") == 0)
		{//0n : S(n) == �������ΰ� ? MAR3_00003
			m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
			m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)�� ����
			m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
			m_p1thAddString = AddTwoStr(m_pStemDicWord,"��");
			bool bFind = IsExistStemDic(m_p1thAddString, m_stemFeature01);
			LoggingFunction(pNode, "MAR3_00004+ ");
			if(bFind == true && (m_stemFeature01[STEM_DIC_V] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE))
			{//1y : S(len~n+1) + ���̡� �� stem ������ �ְ�, S(len~n+1) + ���̡� == [V] || [amb]�ΰ� ? MAR3_00004
				LoggingFunction(pNode, "MAR3_00005+ ");
				m_p2thAddString = AddTwoStr("��",m_pOldBuffer);
				//2y : stem = (len~n+1) + ���̡�, ending = ��� + OldBuf()  MAR3_00005
				InsertStemString(pNode, m_p1thAddString);//stem = (len~n+1) + ���̡�
				SaveHTokenStem(pNode);
				InsertEndingString(pNode, m_p2thAddString);//ending = ��� + OldBuf()
				SaveHTokenEnding(pNode);
			}
			else
			{
				LoggingFunction(pNode, "MAR3_00006+ ");
				if(IsExistStemDic(m_pStemDicWord, m_stemFeature02))
				{//2n : S(len~n+1)�� stem ������ �ִ°�? MAR3_00006
					LoggingFunction(pNode, "MAR3_00007+ ");
					if(strcmp(m_pOneNPluseNumChar, "��") == 0 && m_stemFeature02[STEM_DIC_V] == IS_CEHCK_FEATURE)
					{//3y : S(n+1) == ���ϡ� && S(len~n+1) == [V]�ΰ�? MAR3_00007
						LoggingFunction(pNode, "MAR3_00008+ ");
						//4y : stem = S(len~n+1), ending = S(n)+NewBuf()+Oldbuf()  MAR3_00008
						InsertStemString(pNode, m_pStemDicWord);
						SaveHTokenStem(pNode);
						m_p1thAddString = AddTwoStr(m_pNthOneChar, m_pNewBuffer);
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
						InsertEndingString(pNode, m_p2thAddString);
						SaveHTokenEnding(pNode);
					}
					else
					{
						m_pLastOneChar = GetOneChar(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));//S(1)
						LoggingFunction(pNode, "MAR3_00009+ ");
						if(strcmp(m_pNthOneChar, m_pLastOneChar) == 0)
						{//4n : S(n) == S(1)�ΰ�? MAR3_00009
							LoggingFunction(pNode, "MAR3_00010+ ");
							//5y : stem = S(len~n+1), ending = S(n)+NewBuf()+Oldbuf()  MAR3_00010
							m_p1thAddString = AddTwoStr(m_pNthOneChar, m_pNewBuffer);
							m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
							InsertStemString(pNode, m_pStemDicWord);
							SaveHTokenStem(pNode);
							InsertEndingString(pNode, m_p2thAddString);
							SaveHTokenEnding(pNode);
						}
						else
						{
							LoggingFunction(pNode, "MAR3_00011+ ");

							//5n : MAR1�� ��� MAR3_00011
							GoMAR1_2(pNode, nStart, mStart, lStart);
						}
					}
				}
				else
				{
					LoggingFunction(pNode, "MAR3_00012+ ");

					//3n : MAR1�� ��� MAR3_00012
					GoMAR1_2(pNode, nStart, mStart, lStart);
				}
			}
		}
		else
		{
			GoMAR3_1(pNode, nStart, mStart, lStart);
		}
	}
}

void CNAHangulAnaly::GoMAR3_1(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
	LoggingFunction(pNode, "MAR3_1_00001+ ");
	if(strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0)
	{//1n : S(n) == ���١� || ���ߡ� || �������ΰ� ? MAR3_1_00001
		m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
		m_pEndingDicWord = GetMiddleCountChar(pNode->pHToken->strSource, nStart, m_nMinusSyllable, CALCULATE_STOP, CALCULATE_STOP);//S(n~n-1)
		m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
		m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)�� ����
		m_pJongChar = GetChojungJongsungChar(m_pOneNPluseNumChar, CHAR_JONGSUNG);//C(n+1)�� ����
		m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
		bool bFindEndingDic = IsExistEndingDic(m_pEndingDicWord);
		bool bFindStemDic = IsExistStemDic(m_pStemDicWord);
		LoggingFunction(pNode, "MAR3_1_00002+ ");
		if(strcmp(m_pNthOneChar, "��") == 0 && bFindStemDic == true && bFindStemDic == true)
		{//2y : S(n) == ���١��̰�, S(n~n-1)�� ending ������ �ְ�, S(len~n+1)�� stem ������ �ִ°� ? MAR3_1_00002
			m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
			m_pFormLenToNWord = GetMiddleCountChar(pNode->pHToken->strSource, nStart, m_nMinusSyllable, CALCULATE_STOP, CALCULATE_STOP);//S(n~n-1)
			m_p1thAddString = AddTwoStr(m_pJongChar,m_pEndingDicWord);//C(n+1)+S(n~n-1)
			m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pOneNPluseNumChar, m_nPluseSyllable);//S(len~n+1)-C(n+1)
			bool bFindEndingDic = IsExistEndingDic(m_p1thAddString);
			bool bFindStemDic = IsExistEndingDic(m_pStemDicDeleteJongWord);
			LoggingFunction(pNode, "MAR3_1_00003+ ");
			if(bFindEndingDic == true && bFindStemDic == true)
			{//3y : C(n+1)+S(n~n-1)�� ending������ �ְ�, S(len~n+1)-C(n+1)�� stem������ �ִ°� ? MAR3_1_00003
				LoggingFunction(pNode, "MAR3_1_00004+ ");
				//4y : S(len~n+1) - C(n+1), ending = C(n+1) + ���١� + NewBuf()+OldBuf() MAR3_1_00004
				CHAR_DELETE(m_p1thAddString);
				m_p1thAddString = AddTwoStr(m_pJongChar, "��");
				m_p2thAddString = AddTwoStr(m_p1thAddString, m_pNewBuffer);
				m_p3thAddString = AddTwoStr(m_p2thAddString, m_pOldBuffer);
				InsertStemString(pNode, m_pStemDicDeleteJongWord);
				SaveHTokenStem(pNode);
				InsertEndingString(pNode, m_p3thAddString);
				SaveHTokenEnding(pNode);
			}
			else
			{
				LoggingFunction(pNode, "MAR3_1_00005+ ");
				//4n : Stem = S(len~n+1), ending = ���١�+NewBuf()+OldBuf( MAR3_1_00005
				CHAR_DELETE(m_p1thAddString);
				m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
				m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
				InsertStemString(pNode, m_pStemDicWord);
				SaveHTokenStem(pNode);
				InsertEndingString(pNode, m_p2thAddString);
				SaveHTokenEnding(pNode);
			}
		}
		else
		{
			ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA13);//C(n) = ������
			m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
			m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)�� ����
			m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
			m_p1thAddString = AddTwoStr(m_pStemDicWord, m_pJongChar);
			LoggingFunction(pNode, "MAR3_1_00006+ ");
			if(IsExistStemDic(m_p1thAddString))
			{//3n : C(n) = �� �� S(len~n)+C(n) �� stem ������ �ִ°�? MAR3_1_00006
				LoggingFunction(pNode, "MAR3_1_00007+ ");
				//4y : stem = S(len~n), ending = NewBuf()+OldBuf() MAR3_1_00007
				CHAR_DELETE(m_p1thAddString);
				m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
				InsertStemString(pNode, m_pStemDicWord);
				SaveHTokenStem(pNode);
				InsertEndingString(pNode, m_p1thAddString);
				SaveHTokenEnding(pNode);
			}
			else
			{
				LoggingFunction(pNode, "MAR3_1_00008+ ");
				//4n : C(n) = NULL �� MAR1�� ��� MAR3_1_00008
				ReplaceHangulJongsungNull(pNode, nStart);//S(n)=S(n)-C(n) ��������

				GoMAR1_2(pNode, nStart, mStart, lStart);
			}
		}
	}
	else
	{
		LoggingFunction(pNode, "MAR3_1_00009+ ");
		if (strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0)
		{//2n : S(n) == ���̡� || ������ �ΰ� MAR3_1_00009
			m_pLastOneChar = GetOneChar(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));//S(1)
			if(strcmp(m_pNthOneChar, m_pLastOneChar) == 0)
			{//3y : S(n) == S(1)�ΰ�? MAR3_1_00010
				LoggingFunction(pNode, "MAR3_1_00011+ ");
				
				//4y : MAR1�� ��� MAR3_1_00011
				GoMAR1_2(pNode, nStart, mStart, lStart);
			}
			else
			{ 
				LoggingFunction(pNode, "MAR3_1_00012+ ");

				//4n : IRM���� MAR3_1_00012
				GoIMR1(pNode, nStart, mStart, lStart);
			}
		}
		else
		{
			LoggingFunction(pNode, "MAR3_1_00013+ ");
			if (strcmp(m_pNthOneChar, "��") == 0)
			{//3n : S(n) == ���͡��ΰ� MAR3_1_00013
				m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
				m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)�� ����
				LoggingFunction(pNode, "MAR3_1_00014+ ");
				if(strcmp(m_pOneNPluseNumChar, "��") == 0)
				{//4y : S(n+1) == ���Ρ��ΰ�? MAR3_1_00014
					m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
					m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+2)
					LoggingFunction(pNode, "MAR3_1_00015+ ");
					if (IsExistStemDic(m_pStemDicWord))
					{//5y : S(len~n+2)�� Stem ������ �ִ°�? MAR3_1_00015
						LoggingFunction(pNode, "MAR3_1_00016+ ");
						//6y : Stem = S(len~n+2), ending = S(n+1~n)+NewBuf()+OldBuf() MAR3_1_00016
						m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
						m_pFormLenToNWord = GetMiddleCountChar(pNode->pHToken->strSource, m_nPluseSyllable, nStart, CALCULATE_STOP, CALCULATE_STOP);//S(n+1~n)
						m_p1thAddString = AddTwoStr(m_pFormLenToNWord, m_pNewBuffer);
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);

						InsertStemString(pNode, m_pStemDicWord);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, m_p2thAddString);
						SaveHTokenEnding(pNode);
					}
					else
					{
						LoggingFunction(pNode, "MAR3_1_00017(n++ �� m=n �� n++ �� MAR2)+ ");
						//6n : OldBuf() = �����͡�+OldBuf() �� n++ �� m=n �� n++ �� MAR2�� MAR3_1_00017
						m_p1thAddString = AddTwoStr("����", m_pOldBuffer);
						CHAR_DELETE(m_pOldBuffer);
						int nLen = strlen(m_p1thAddString);
						m_pOldBuffer = new char[nLen + 1];
						memset(m_pOldBuffer, 0x00, nLen + 1);
						strcpy(m_pOldBuffer, m_p1thAddString);
						InsertArrayFeatureOld(m_p1thAddString);

						ReturnNumberMovingLeftPositionWord(pNode->pHToken->strSource, nStart);//n++
						mStart = nStart;
						ReturnNumberMovingLeftPositionWord(pNode->pHToken->strSource, nStart);//n++
						InitOneWordMarsk();
						if (nStart < 0)
						{
							GoErrorFunction(pNode);
						}
						else
						{
							GoMAR2(pNode, nStart, mStart, lStart);
						}
					}
				}
				else
				{
					LoggingFunction(pNode, "MAR3_1_00018+ ");

					//5n : IRM���� MAR3_1_00018
					GoIMR1(pNode, nStart, mStart, lStart);
				}
			}
			else
			{	
				GoMAR3_2(pNode, nStart, mStart, lStart);
			}
		}
	}
}

void CNAHangulAnaly::GoMAR3_2(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
	m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)�� ����
	m_nRestorationJongChar = Restoration(m_pJongChar);//�����ؾ� �� ������
	LoggingFunction(pNode, "MAR3_2_00001+ ");
	if (strcmp(m_pJongChar, "��") == 0)
	{//4n : C(n) == �������ΰ� ? MAR3_2_00001
		ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);//C(n) = ������
		m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
		LoggingFunction(pNode, "MAR3_2_00002+ ");
		if (IsExistStemDic(m_pStemDicWord, m_stemFeature01))
		{//5y : C(n) = �� �� S(len~n)�� stem ������ �ִ°� ? MAR3_2_00002
			LoggingFunction(pNode, "MAR3_2_00003+ ");
			if(m_stemFeature01[STEM_DIC_LIR] == IS_CEHCK_FEATURE)
			{//6y : S(len~n)�� Lir�ΰ� ? MAR3_2_00003
				LoggingFunction(pNode, "MAR3_2_00004+ ");
				m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
				m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
				//7y : stem = S(len~n), ending = �� + NewBuf()+OldBuf() MAR3_2_00004
				InsertStemString(pNode, m_pStemDicWord);
				SaveHTokenStem(pNode);
				InsertEndingString(pNode, m_p2thAddString);
				SaveHTokenEnding(pNode);
			}
			else
			{
				LoggingFunction(pNode, "MAR3_2_00005+ ");
				//7n : C(n) = �� �� IRM���� MAR3_2_00005
				//������ ���������� ����
				JasoRestoration(pNode, nStart, FORCE_REPLACE_ENDWORD, m_nRestorationJongChar);// C(n) = #C(n)

				GoIMR1(pNode, nStart, mStart, lStart);
			}
		}
		else
		{
			LoggingFunction(pNode, "MAR3_2_00006+ ");
			//6n : C(n) = �� �� IRM���� MAR3_2_00006
			//������ ���������� ����
			JasoRestoration(pNode, nStart, FORCE_REPLACE_ENDWORD, m_nRestorationJongChar);// C(n) = #C(n)

			GoIMR1(pNode, nStart, mStart, lStart);
		}
	}
	else
	{ 
		LoggingFunction(pNode, "MAR3_2_00007+ ");
		if (strcmp(m_pJongChar, "��") == 0)
		{//5n : C(n) == �������ΰ� ? MAR3_2_00007
			ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);//C(n) = ������
			m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
			LoggingFunction(pNode, "MAR3_2_00008+ ");
			if (IsExistStemDic(m_pStemDicWord, m_stemFeature01))
			{//6y : C(n) = �� �� S(len~n)�� stem ������ �ִ°� ? MAR3_2_00008
				LoggingFunction(pNode, "MAR3_2_00009+ ");
				if (m_stemFeature01[STEM_DIC_LIR] == IS_CEHCK_FEATURE)
				{//7y : S(len~n)�� Lir�ΰ� ? MAR3_2_00009
					LoggingFunction(pNode, "MAR3_2_00010+ ");
					m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					//8y : stem = S(len~n), ending = �� + NewBuf()+OldBuf() MAR3_2_00010
					InsertStemString(pNode, m_pStemDicWord);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_p2thAddString);
					SaveHTokenEnding(pNode);

				}
				else
				{
					LoggingFunction(pNode, "MAR3_2_00011+ ");
					//8n : C(n) = �� �� Error MAR3_2_00011
					//������ ���������� ����
					JasoRestoration(pNode, nStart, FORCE_REPLACE_ENDWORD, m_nRestorationJongChar);// C(n) = #C(n)

					GoErrorFunction(pNode);
				}
			}
			else
			{
				LoggingFunction(pNode, "MAR3_2_00012+ ");
				//7n : C(n) = �� �� Error MAR3_2_00012
				//������ ���������� ����
				JasoRestoration(pNode, nStart, FORCE_REPLACE_ENDWORD, m_nRestorationJongChar);// C(n) = #C(n)

				GoErrorFunction(pNode);
			}
		}
		else
		{
			LoggingFunction(pNode, "MAR3_2_00013+ ");
			if (strcmp(m_pNthOneChar, "��") == 0)
			{//6n : S(n)�� �������ΰ� ? MAR3_2_00013
				CHAR_DELETE(m_pJongChar);
				m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
				m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)�� ����
				m_pJongChar = GetChojungJongsungChar(m_pOneNPluseNumChar, CHAR_JONGSUNG);//C(n+1)�� ����
				LoggingFunction(pNode, "MAR3_2_00014+ ");
				if(strcmp(m_pOneNPluseNumChar, "��") != 0 &&
					(strcmp(m_pJongChar, "��") == 0 || strcmp(m_pJongChar, "��") == 0
					|| strcmp(m_pOneNPluseNumChar, "��") == 0 || strcmp(m_pOneNPluseNumChar, "��") == 0 || strcmp(m_pOneNPluseNumChar, "��") == 0
					|| strcmp(m_pOneNPluseNumChar, "��") == 0 || strcmp(m_pOneNPluseNumChar, "��") == 0 || strcmp(m_pOneNPluseNumChar, "��") == 0
					|| strcmp(m_pOneNPluseNumChar, "��") == 0 || strcmp(m_pOneNPluseNumChar, "��") == 0 || strcmp(m_pOneNPluseNumChar, "��") == 0
					|| strcmp(m_pOneNPluseNumChar, "��") == 0 || strcmp(m_pOneNPluseNumChar, "��") == 0 || strcmp(m_pOneNPluseNumChar, "��") == 0
					|| strcmp(m_pOneNPluseNumChar, "��") == 0 || strcmp(m_pOneNPluseNumChar, "�Ρ�") == 0))
				{//7y : S(n+1) != ���ˡ� && C(n+1) == ������ || ������ || S(n+1) == ���١� || ��� || ���ס� || ������ || ������ || ���ߡ� || ���塱 || ���š� || ���١� || ���ѡ� || ������ || ������ || ������ || ���Ρ� �ΰ� ? MAR3_2_00014
					m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
					m_pAddTwoString = AddTwoStr(m_pStemDicWord,"��");
					LoggingFunction(pNode, "MAR3_2_00015+ ");
					if (IsExistStemDic(m_pAddTwoString))
					{//8y : S(len~n+1)+�������� stem ������ �ִ°� MAR3_2_00015
						LoggingFunction(pNode, "MAR3_2_00016+ ");
						m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
						//9y : stem = S(len~n+1)+������, ending = ��� + NewBuf()+OldBuf()  MAR3_2_00016
						InsertStemString(pNode, m_pAddTwoString);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, m_p2thAddString);
						SaveHTokenEnding(pNode);
					}
					else
					{
						LoggingFunction(pNode, "MAR3_2_00017+ ");
						//9n : S(n) = [mask], MAR2�� MAR3_2_00017
						SetOneWordMarsk();

						GoMAR2(pNode, nStart, mStart, lStart);
					}
				}
				else
				{
					m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
					LoggingFunction(pNode, "MAR3_2_00018+ ");
					if (IsExistStemDic(m_pStemDicWord, m_stemFeature01))
					{//8n :  S(len~n+1)�� stem ������ �ִ°�? MAR3_2_00018
						LoggingFunction(pNode, "MAR3_2_00019+ ");
						if(m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE)
						{//9y : S(len~n+1) <> [N]�ΰ�? MAR3_2_00019
							LoggingFunction(pNode, "MAR3_2_00020+ ");
							m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
							m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
							//10y : stem = S(len~n+1),  ending = ������+NewBuf() +OldBuf() MAR3_2_00020
							InsertStemString(pNode, m_pStemDicWord);
							SaveHTokenStem(pNode);
							InsertEndingString(pNode, m_p2thAddString);
							SaveHTokenEnding(pNode);
						}
						else
						{
							LoggingFunction(pNode, "MAR3_2_00021+ ");
							//10n : S(n) = [mask], MAR1_2�� MAR3_2_00021
							SetOneWordMarsk();

							GoMAR1_2(pNode, nStart, mStart, lStart);
						}
					}
					else
					{
						LoggingFunction(pNode, "MAR3_2_0002+ ");
						//9n : S(n) = [mask], MAR1_2�� MAR3_2_0002
						SetOneWordMarsk();

						GoMAR1_2(pNode, nStart, mStart, lStart);
					}
				}
			}
			else
			{	
				GoMAR3_3(pNode, nStart, mStart, lStart);
			}
		}
	}

}
void CNAHangulAnaly::GoMAR3_3(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
	m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)�� ����
	LoggingFunction(pNode, "MAR3_3_00001+ ");
	if (strcmp(m_pNthOneChar, "ŭ") == 0)
	{//7n : S(n) == ��ŭ���ΰ� ? MAR3_3_00001
		m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
		m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)�� ����
		m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
		m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+2)
		LoggingFunction(pNode, "MAR3_3_00002+ ");
		if(strcmp(m_pOneNPluseNumChar, "��") == 0 && IsExistStemDic(m_pStemDicWord))
		{//8y : S(n+1)�� �������̰�, S(len~n+2)�� stem ������ �ִ°� ? MAR3_3_00002
			LoggingFunction(pNode, "MAR3_3_00003+ ");
			m_p1thAddString = AddTwoStr("��ŭ", m_pNewBuffer);
			m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
			//9y : Stem = S(len~n+2), ending = ����ŭ�� + NewBuf()+OldBuf() MAR3_3_00003
			InsertStemString(pNode, m_pStemDicWord);
			SaveHTokenStem(pNode);
			InsertEndingString(pNode, m_p2thAddString);
			SaveHTokenEnding(pNode);
		}
		else
		{
			LoggingFunction(pNode, "MAR3_3_00004+ ");
			//9n : S(n) = [mask], MAR2�� MAR3_3_00004
			SetOneWordMarsk();

			GoMAR2(pNode, nStart, mStart, lStart);
		}
	}
	else
	{
		LoggingFunction(pNode, "MAR3_3_00005+ ");
		if(m_pJongChar == NULL || strcmp(m_pJongChar,"") == 0)
		{//8n : C(n) == Null�ΰ� ? MAR3_3_00005
			CHAR_DELETE(m_pNthOneChar);
			m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
			m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1)�� ����
			LoggingFunction(pNode, "MAR3_3_00006+ ");
			if(strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0
				|| strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0)
			{//9y : S(n-1)�� ���¡� || ���á� || ���š� || ���̡� || ������ || ���¡� �ΰ� ? MAR3_3_00006
				ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);//C(n) = ������
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
				LoggingFunction(pNode, "MAR3_3_00007+ ");
				if (IsExistStemDic(m_pStemDicWord, m_stemFeature01))
				{//10y : C(n)="��" �� S(len~n)�� stem ������ �ִ°� ? MAR3_3_00007
					LoggingFunction(pNode, "MAR3_3_00008+ ");
					if (m_stemFeature01[STEM_DIC_LIR] == IS_CEHCK_FEATURE)
					{//11y : S(len~n)�� Lir�ΰ� ? MAR3_3_00008
						LoggingFunction(pNode, "MAR3_3_00009+ ");
						m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
						//12y : stem = S(len~n), ending = NewBuf()+OldBuf() MAR3_3_00009
						InsertStemString(pNode, m_pStemDicWord);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, m_p1thAddString);
						SaveHTokenEnding(pNode);
					}
					else
					{
						LoggingFunction(pNode, "MAR3_3_00010+ ");
						//12n : C(n) = Null �� Error MAR3_3_00010
						ReplaceHangulJongsungNull(pNode, nStart);

						GoErrorFunction(pNode);
					}
				}
				else
				{
					LoggingFunction(pNode, "MAR3_3_00011+ ");
					//11n : C(n) = Null �� Error MAR3_3_00011
					ReplaceHangulJongsungNull(pNode, nStart);

					GoErrorFunction(pNode);
				}
			}
			else
			{
				LoggingFunction(pNode, "MAR3_3_00012+ ");
				//10n : C(n) = Null �� Error MAR3_3_00012
				ReplaceHangulJongsungNull(pNode, nStart);

				GoErrorFunction(pNode);
			}
		}
		else
		{
			LoggingFunction(pNode, "MAR3_3_00013+ ");
			//9n : stem = S(len~n), end = NewBuf()+OldBuf() -> Error MAR3_3_00013
			m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
			m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
			InsertStemString(pNode, m_pStemDicWord);
			SaveHTokenStem(pNode);
			InsertEndingString(pNode, m_p1thAddString);
			SaveHTokenEnding(pNode);
		}
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//IMR1
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CNAHangulAnaly::GoIMR1(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
	LoggingFunction(pNode, "IMR1_00001+ ");
	if (strcmp(m_pNthOneChar, "��") == 0)
	{//S(n) == ���ɡ� �ΰ� ? IMR1_00001
		m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
		m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
		m_pAddTwoString = AddTwoStr(m_pStemDicWord, "��");//S(len~n+1)+���ϡ�
		LoggingFunction(pNode, "IMR1_00002+ ");
		if (IsExistStemDic(m_pAddTwoString))
		{//1y : (S(len~n+1)+���ϡ�)�� stem ������ �ִ°� ? IMR1_00003
			LoggingFunction(pNode, "IMR1_00002+ ");
			if (m_pOldBuffer == NULL || strcmp(m_pOldBuffer,"") == 0)
			{//2y : stem = S(len~n+1)+���ϡ� �� OldBuf()�� Null�ΰ� ? IMR1_00003
				LoggingFunction(pNode, "IMR1_00004+ ");
				m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
				//3y : ending = ���ԡ� + [V][Aff] + NewBuf() IMR1_00004
				InsertStemString(pNode, m_pAddTwoString);
				SaveHTokenStem(pNode);
				InsertEndingString(pNode, m_p1thAddString);
				SaveHTokenEnding(pNode);
				AddHTokenEnding(pNode, ENDING_DIC_V);
				AddHTokenEnding(pNode, ENDING_DIC_AFF);
			}
			else
			{
				LoggingFunction(pNode, "IMR1_00005+ ");
				//3n : ending = ���ԡ� + [V][Aff] + NewBuf()+OldBuf() IMR1_00005
				m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
				m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
				InsertStemString(pNode, m_pAddTwoString);
				SaveHTokenStem(pNode);
				InsertEndingString(pNode, m_p2thAddString);
				SaveHTokenEnding(pNode);
				AddHTokenEnding(pNode, ENDING_DIC_V);
				AddHTokenEnding(pNode, ENDING_DIC_AFF);
			}
		}
		else
		{
			bool bFind = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
			LoggingFunction(pNode, "IMR1_00006+ ");
			if(bFind == true && m_stemFeature01[STEM_DIC_CONC] != IS_CEHCK_FEATURE)
			{//2n : S(len~n+1)�� stem ������ �ְ�, S(len~n+1) != [Conc]�ΰ� ? (20030128 ������ ����) IMR1_00006
				LoggingFunction(pNode, "IMR1_00007+ ");
				if (m_pOldBuffer == NULL || strcmp(m_pOldBuffer, "") == 0)
				{//3y : stem = S(len~n+1)+���ϡ� + [V][Lex] - [N] �� OldBuf()�� Null�ΰ� ? IMR1_00007
					LoggingFunction(pNode, "IMR1_00008+ ");
					m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
					//4y : ending = ���ԡ� + [V][Aff] + NewBuf() IMR1_00008
					InsertStemString(pNode, m_pAddTwoString);
					SaveHTokenStem(pNode);
					AddHTokenStem(pNode, STEM_DIC_V);
					AddHTokenStem(pNode, STEM_DIC_LEX);
					DeleteHTokenStem(pNode, STEM_DIC_N);
					InsertEndingString(pNode, m_p1thAddString);
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, ENDING_DIC_V);
					AddHTokenEnding(pNode, ENDING_DIC_AFF);
				}
				else
				{
					LoggingFunction(pNode, "IMR1_00009+ ");
					m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					//4n : ending = ���ԡ� + [V][Aff] + NewBuf()+OldBuf() IMR1_00009
					InsertStemString(pNode, m_pAddTwoString);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_p2thAddString);
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, ENDING_DIC_V);
					AddHTokenEnding(pNode, ENDING_DIC_AFF);
				}
			}
			else
			{
				LoggingFunction(pNode, "IMR1_00010+ ");

				//3n : Error IMR1_00010
				GoErrorFunction(pNode);
			}
		}
	}
	else
	{
		GoIMR1_1(pNode, nStart, mStart, lStart);
	}

}

void CNAHangulAnaly::GoIMR1_1(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
	LoggingFunction(pNode, "IMR1_1_00001+ ");
	if (strcmp(m_pNthOneChar, "ġ") == 0)
	{ //1n : S(n)�� ��ġ���ΰ� ? IMR1_1_00001
		CHAR_DELETE(m_pNthOneChar);
		m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
		m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1)�� ����
		m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
		m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
		bool bFind = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
		LoggingFunction(pNode, "IMR1_1_00002+ ");
		if(strcmp(m_pNthOneChar, "��") == 0 && (bFind == true && (m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE)))
		{//2y : {S(n-1) == ����} && {S(len~n+1) == [N] || [Amb]} �ΰ� ? IMR1_1_00002
			LoggingFunction(pNode, "IMR1_1_00003+ ");
			//3y : S(n) = [mask] �� MAR1�� ��� IMR1_1_00003
			SetOneWordMarsk();

			GoMAR1_2(pNode, nStart, mStart, lStart);
		}
		else
		{
			CHAR_DELETE(m_pNthOneChar);
			m_pAddTwoString = AddTwoStr(m_pStemDicWord, "��");//S(len~n+1)+���ϡ�
			m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
			m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+2)�� ����
			LoggingFunction(pNode, "IMR1_1_00004+ ");
			if(IsExistStemDic(m_pAddTwoString) && (m_pNthOneChar == NULL || strcmp(m_pNthOneChar, "") == 0 || strcmp(m_pNthOneChar, "��") == 0))
			{//3n : (S(len~n+1)+���ϡ�)�� stem ������ �ְ�, S(n+2) == ������ || NULL�ΰ� ? IMR1_1_00004
				if (m_pOldBuffer == NULL || strcmp(m_pOldBuffer, "") == 0)
				{//4y : stem = S(len~n+1)+���ϡ�[V][Lex] - [N] �� OldBuf()�� Null�ΰ� ? IMR1_1_00005
					LoggingFunction(pNode, "IMR1_1_00006+ ");
					m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
					//5y : ending = ������ + [V][Aff] + NewBuf() IMR1_1_00006
					InsertStemString(pNode, m_pAddTwoString);
					SaveHTokenStem(pNode);
					AddHTokenStem(pNode, STEM_DIC_V);
					AddHTokenStem(pNode, STEM_DIC_LEX);
					DeleteHTokenStem(pNode, STEM_DIC_N);
					InsertEndingString(pNode, m_p1thAddString);
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, ENDING_DIC_V);
					AddHTokenEnding(pNode, ENDING_DIC_AFF);
				}
				else
				{
					LoggingFunction(pNode, "IMR1_1_00007+ ");
					m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					//5n : ending = ������ + [V][Aff] + NewBuf()+OldBuf() IMR1_1_00007
					InsertStemString(pNode, m_pAddTwoString);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_p2thAddString);
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, ENDING_DIC_V);
					AddHTokenEnding(pNode, ENDING_DIC_AFF);
				}
			}
			else
			{
				LoggingFunction(pNode, "IMR1_1_00008+ ");
				if ((bFind == true && m_stemFeature01[STEM_DIC_CONC] != IS_CEHCK_FEATURE) && (m_pNthOneChar == NULL || strcmp(m_pNthOneChar, "") == 0 || strcmp(m_pNthOneChar, "��") == 0))
				{//4n : S(len~n+1)�� stem ������ �ְ�, S(len~n+1) != [Conc]�̰�, S(n+2) == ������ || NULL�ΰ� ? IMR1_1_00008
					if (m_pOldBuffer == NULL || strcmp(m_pOldBuffer, "") == 0)
					{//5y : stem = S(len~n+1)+���ϡ� + [V][Lex] - [N] �� OldBuf()�� Null�ΰ� ? IMR1_1_00009
						m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
						//6y : ending = ������ + [V][Aff] + NewBuf() IMR1_1_00010
						InsertStemString(pNode, m_pAddTwoString);
						SaveHTokenStem(pNode);
						AddHTokenStem(pNode, STEM_DIC_V);
						AddHTokenStem(pNode, STEM_DIC_LEX);
						DeleteHTokenStem(pNode, STEM_DIC_N);
						InsertEndingString(pNode, m_p1thAddString);
						SaveHTokenEnding(pNode);
						AddHTokenEnding(pNode, ENDING_DIC_V);
						AddHTokenEnding(pNode, ENDING_DIC_AFF);
					}
					else
					{
						LoggingFunction(pNode, "IMR1_1_00011+ ");
						m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
						//6n : ending = ������ + [V][Aff] + NewBuf()+OldBuf() IMR1_1_00011
						InsertStemString(pNode, m_pAddTwoString);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, m_p2thAddString);
						SaveHTokenEnding(pNode);
						AddHTokenEnding(pNode, ENDING_DIC_V);
						AddHTokenEnding(pNode, ENDING_DIC_AFF);
					}
				}
				else
				{
					LoggingFunction(pNode, "IMR1_1_00012+ ");

					//5n : Error IMR1_1_00012
					GoErrorFunction(pNode);
				}
			}
		}
	}
	else
	{
		GoIMR1_2(pNode, nStart, mStart, lStart);
	}

}

void CNAHangulAnaly::GoIMR1_2(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
	LoggingFunction(pNode, "IMR1_2_00001+ ");
	if (strcmp(m_pNthOneChar, "��") == 0)
	{//2n : S(n)�� ���ϡ��ΰ� ? IMR1_2_00001
		CHAR_DELETE(m_pNthOneChar);
		m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
		m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)�� ����
		LoggingFunction(pNode, "IMR1_2_00002+ ");
		if (strcmp(m_pNthOneChar, "��") == 0)
		{//3y : S(n+1)�� ���䡱�ΰ� ? IMR1_2_00002
			m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
			m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+2)
			m_pAddTwoString = AddTwoStr(m_pStemDicWord, "��");//S(len~n+2)+���ϡ�
			LoggingFunction(pNode, "IMR1_2_00003+ ");
			if (IsExistStemDic(m_pAddTwoString))
			{//4y : (S(len~n+2)+���ϡ�)�� stem ������ �ִ°� ? IMR1_2_00003
				LoggingFunction(pNode, "IMR1_2_00004+ ");
				if (m_pOldBuffer == NULL || strcmp(m_pOldBuffer, "") == 0)
				{//5y : stem = S(len~n+2)+���ϡ�[V][Lex] - [N] �� OldBuf()�� Null�ΰ� ? IMR1_2_00004
					LoggingFunction(pNode, "IMR1_2_00005+ ");
					m_p1thAddString = AddTwoStr("����", m_pNewBuffer);
					//6y : ending = �����ϡ� + [V][Aff][Con][Rsu] + NewBuf() IMR1_2_00005
					InsertStemString(pNode, m_pAddTwoString);
					SaveHTokenStem(pNode);
					AddHTokenStem(pNode, STEM_DIC_V);
					AddHTokenStem(pNode, STEM_DIC_LEX);
					DeleteHTokenStem(pNode, STEM_DIC_N);
					InsertEndingString(pNode, m_p1thAddString);
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, ENDING_DIC_V);
					AddHTokenEnding(pNode, ENDING_DIC_AFF);
					AddHTokenEnding(pNode, ENDING_DIC_CON);
					AddHTokenEnding(pNode, ENDING_DIC_RSU);
				}
				else
				{
					LoggingFunction(pNode, "IMR1_2_00006+ ");
					m_p1thAddString = AddTwoStr("����", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					//6n : �����ϡ� + [V][Aff][Con][Rsu] + NewBuf()+OldBuf() IMR1_2_00006
					InsertStemString(pNode, m_pAddTwoString);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_p2thAddString);
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, ENDING_DIC_V);
					AddHTokenEnding(pNode, ENDING_DIC_AFF);
					AddHTokenEnding(pNode, ENDING_DIC_CON);
					AddHTokenEnding(pNode, ENDING_DIC_RSU);
				}
			}
			else
			{
				bool bFind = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
				LoggingFunction(pNode, "IMR1_2_00007+ ");
				if(bFind == true && m_stemFeature01[STEM_DIC_CONC] != IS_CEHCK_FEATURE)
				{//5n : S(len~n+2)�� stem ������ �ְ�, S(len~n+2) != [Conc]�ΰ� ? (20030128������ ����) IMR1_2_00007
					LoggingFunction(pNode, "IMR1_2_00008+ ");
					if (m_pOldBuffer == NULL || strcmp(m_pOldBuffer, "") == 0)
					{//6y : stem = S(len~n+2)+���ϡ� || ���ǡ� + [V][Lex] - [N] �� OldBuf()�� Null�ΰ� ? IMR1_2_00008 : "��"�� �⺻���� ���ϰ� ��
						LoggingFunction(pNode, "IMR1_2_00009+ ");
						m_p1thAddString = AddTwoStr("����", m_pNewBuffer);
						//7y : ending = �����ϡ� + [V][Aff][Con][Rsu] + NewBuf() IMR1_2_00009
						InsertStemString(pNode, m_pAddTwoString);
						SaveHTokenStem(pNode);
						AddHTokenStem(pNode, STEM_DIC_V);
						AddHTokenStem(pNode, STEM_DIC_LEX);
						DeleteHTokenStem(pNode, STEM_DIC_N);
						InsertEndingString(pNode, m_p1thAddString);
						SaveHTokenEnding(pNode);
						AddHTokenEnding(pNode, ENDING_DIC_V);
						AddHTokenEnding(pNode, ENDING_DIC_AFF);
						AddHTokenEnding(pNode, ENDING_DIC_CON);
						AddHTokenEnding(pNode, ENDING_DIC_RSU);
					}
					else
					{
						LoggingFunction(pNode, "IMR1_2_00010+ ");
						
						m_p1thAddString = AddTwoStr("����", m_pNewBuffer);
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
						//7n : ending = �����ϡ� + [V][Aff][Con][Rsu] + NewBuf()+OldBuf() IMR1_2_00010
						InsertStemString(pNode, m_pAddTwoString);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, m_p2thAddString);
						SaveHTokenEnding(pNode);
						AddHTokenEnding(pNode, ENDING_DIC_V);
						AddHTokenEnding(pNode, ENDING_DIC_AFF);
						AddHTokenEnding(pNode, ENDING_DIC_CON);
						AddHTokenEnding(pNode, ENDING_DIC_RSU);
					}
				}
				else
				{
					LoggingFunction(pNode, "IMR1_2_00011+ ");
					
					//6n : Error IMR1_2_00011
					GoErrorFunction(pNode);
				}
			}
		}
		else
		{
			LoggingFunction(pNode, "IMR1_2_00012+ ");
			if (strcmp(m_pNthOneChar, "��") == 0)
			{//4n : S(n+1)�� �������ΰ� ? IMR1_2_00012
				m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+2)
				LoggingFunction(pNode, "IMR1_2_00013+ ");
				if(IsExistStemDic(m_pStemDicWord))
				{//5y : S(len~n+2)�� stem ������ �ִ°� ? IMR1_2_00013
					LoggingFunction(pNode, "IMR1_2_00014+ ");
					m_p1thAddString = AddTwoStr("����", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					//6y : stem = S(len~n+2), ending = �����ϡ� + [V][Aff][Con][Rsu] + NewBuf()+OldBuf() IMR1_2_00014
					InsertStemString(pNode, m_pStemDicWord);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_p2thAddString);
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, ENDING_DIC_V);
					AddHTokenEnding(pNode, ENDING_DIC_AFF);
					AddHTokenEnding(pNode, ENDING_DIC_CON);
					AddHTokenEnding(pNode, ENDING_DIC_RSU);
				}
				else
				{
					CHAR_DELETE(m_pNthOneChar);
					m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
					m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+2)�� ����
					LoggingFunction(pNode, "IMR1_2_00015+ ");
					if(strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0)
					{//6n : S(n+2)�� ���ϡ�||���ǡ��ΰ�? IMR1_2_00015
						CHAR_DELETE(m_pStemDicWord);
						m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 3, CALCULATE_MINUS);//n+3
						m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+3)
						LoggingFunction(pNode, "IMR1_2_00016+ ");
						if (IsExistStemDic(m_pStemDicWord))
						{//7y : S(len~n+3)�� stem ������ �ִ°�? IMR1_2_00016
							LoggingFunction(pNode, "IMR1_2_00017+ ");
							if (m_pOldBuffer == NULL || strcmp(m_pOldBuffer, "") == 0)
							{//8y : stem = S(len~n+3)+���ϡ�||���ǡ�+[V][Lex]-[N] �� OldBuf( )�� Null�ΰ�? IMR1_2_00017
								LoggingFunction(pNode, "IMR1_2_00018+ ");
								m_p1thAddString = AddTwoStr("����", m_pNewBuffer);
								m_pAddTwoString = AddTwoStr(m_pStemDicWord, m_pNthOneChar);
								//9y : ending = �����ϡ�+[V][Aff][Con][Rsu]+NewBuf()  IMR1_2_00018
								InsertStemString(pNode, m_pAddTwoString);
								SaveHTokenStem(pNode);
								AddHTokenStem(pNode, STEM_DIC_V);
								AddHTokenStem(pNode, STEM_DIC_LEX);
								DeleteHTokenStem(pNode, STEM_DIC_N);
								InsertEndingString(pNode, m_p1thAddString);
								SaveHTokenEnding(pNode);
								AddHTokenEnding(pNode, ENDING_DIC_V);
								AddHTokenEnding(pNode, ENDING_DIC_AFF);
								AddHTokenEnding(pNode, ENDING_DIC_CON);
								AddHTokenEnding(pNode, ENDING_DIC_RSU);
							}
							else
							{
								LoggingFunction(pNode, "IMR1_2_00019+ ");
								m_pAddTwoString = AddTwoStr(m_pStemDicWord, m_pNthOneChar);
								m_p1thAddString = AddTwoStr("����", m_pNewBuffer);
								m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
								//9n : ending = �����ϡ�+[V][Aff][Con][Rsu]+NewBuf()+OldBuf( ) IMR1_2_00019
								InsertStemString(pNode, m_pAddTwoString);
								SaveHTokenStem(pNode);
								AddHTokenStem(pNode, STEM_DIC_V);
								AddHTokenStem(pNode, STEM_DIC_LEX);
								DeleteHTokenStem(pNode, STEM_DIC_N);
								InsertEndingString(pNode, m_p2thAddString);
								SaveHTokenEnding(pNode);
								AddHTokenEnding(pNode, ENDING_DIC_V);
								AddHTokenEnding(pNode, ENDING_DIC_AFF);
								AddHTokenEnding(pNode, ENDING_DIC_CON);
								AddHTokenEnding(pNode, ENDING_DIC_RSU);
							}
						}
						else
						{
							LoggingFunction(pNode, "IMR1_2_00020+ ");
							CHAR_DELETE(m_pStemDicWord);
							m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
							m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+2)
							m_p1thAddString = AddTwoStr("����", m_pNewBuffer);
							m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
							//8n : ending =�����ϡ�+[V][Aff][Con][Rsu]+NewBuf()+OldBuf( )
							//stem = S(len~n+2) �� Error IMR1_2_00020 stem�� �����ϰ� �����Լ��� ȣ���ϴ��� �ǹ̰� �Ҹ�Ȯ
							InsertStemString(pNode, m_pStemDicWord);
							SaveHTokenStem(pNode);

							InsertEndingString(pNode, m_p2thAddString);
							SaveHTokenEnding(pNode);
							AddHTokenEnding(pNode, ENDING_DIC_V);
							AddHTokenEnding(pNode, ENDING_DIC_AFF);
							AddHTokenEnding(pNode, ENDING_DIC_CON);
							AddHTokenEnding(pNode, ENDING_DIC_RSU);

							GoErrorFunction(pNode);
						}
					}
					else
					{
						LoggingFunction(pNode, "IMR1_2_00021+ ");

						//7n : S(n) = [mask] -> MAR1�� ���(�߰���) IMR1_2_00021
						SetOneWordMarsk();
						GoMAR1_2(pNode, nStart, mStart, lStart);
					}
				}
			}
			else
			{
				LoggingFunction(pNode, "IMR1_2_00022+ ");

				//5n :  S(n) = [mask] -> MAR1�� ��� IMR1_2_00022
				SetOneWordMarsk();
				GoMAR1_2(pNode, nStart, mStart, lStart);
			}
		}
	}
	else
	{
		GoIMR1_3(pNode, nStart, mStart, lStart);
	}

}

void CNAHangulAnaly::GoIMR1_3(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
	m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)�� ����
	LoggingFunction(pNode, "IMR1_3_00001+ ");
	if(strcmp(m_pNthOneChar, "��") == 0)
	{//3n : S(n)�� ���֡��ΰ� ? IMR1_3_00001
		m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
		m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
		LoggingFunction(pNode, "IMR1_3_00002+ ");
		if(IsExistStemDic(m_pStemDicWord))
		{//4y : S(len~n+1)�� stem ������ �ִ°� ? IMR1_3_00002
			LoggingFunction(pNode, "IMR1_3_00003+ ");
			//5y : stem = S(len~n+1) + ���֡� + [V][Sta] - [N], ending = NewBuf()+OldBuf() IMR1_3_00003
			m_pAddTwoString = AddTwoStr(m_pStemDicWord,"��");
			m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
			InsertStemString(pNode, m_pAddTwoString);//stem = S(len~n+1) + ���֡�
			AddHTokenStem(pNode, STEM_DIC_V);
			AddHTokenStem(pNode, STEM_DIC_STA);
			DeleteHTokenStem(pNode, STEM_DIC_N);
			SaveHTokenStem(pNode);
			InsertEndingString(pNode, m_p1thAddString);
			SaveHTokenEnding(pNode);
		}
		else
		{
			LoggingFunction(pNode, "IMR1_3_00004+ ");

			//5n : Error IMR1_3_00004
			GoErrorFunction(pNode);
		}
	}
	else
	{ 
		LoggingFunction(pNode, "IMR1_3_00005+ ");
		if (strcmp(m_pNthOneChar, "��") == 0)
		{//4n : S(n)�� �������ΰ� ? IMR1_3_00005
			m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
			m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
			LoggingFunction(pNode, "IMR1_3_00006+ ");
			if (IsExistStemDic(m_pStemDicWord))
			{//5y : S(len~n+1)�� stem ������ �ִ°� ? IMR1_3_00006
				LoggingFunction(pNode, "IMR1_3_00007+ ");
				//6y : stem = S(len~n) + ������ + [V][Sta][neg] - [N], ending = NewBuf()+OldBuf() IMR1_3_00007
				CHAR_DELETE(m_pStemDicWord);
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
				m_pAddTwoString = AddTwoStr(m_pStemDicWord, "��");
				m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
				InsertStemString(pNode, m_pAddTwoString);//stem = S(len~n+1) + ������
				AddHTokenStem(pNode, STEM_DIC_V);
				AddHTokenStem(pNode, STEM_DIC_STA);
				AddHTokenStem(pNode, STEM_DIC_NEG);
				DeleteHTokenStem(pNode, STEM_DIC_N);
				SaveHTokenStem(pNode);
				InsertEndingString(pNode, m_p1thAddString);
				SaveHTokenEnding(pNode);
			}
			else
			{
				LoggingFunction(pNode, "IMR1_3_00008+ ");

				//6n : Error IMR1_3_00008
				GoErrorFunction(pNode);
			}
		}
		else
		{
			LoggingFunction(pNode, "IMR1_3_00009+ ");
			if (strcmp(m_pNthOneChar, "��") == 0)
			{//5n : S(n) �� ���š��ΰ� ? IMR1_3_00009
				CHAR_DELETE(m_pNthOneChar);
				m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
				m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n+1)�� ����
				LoggingFunction(pNode, "IMR1_3_00010+ ");
				if(strcmp(m_pNthOneChar,"��") ==0)
				{//6y : S(n+1)�� �������ΰ� ? IMR1_3_00010
					CHAR_DELETE(m_pStemDicWord);
					m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
					m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+2)
					LoggingFunction(pNode, "IMR1_3_00011+ ");
					if (IsExistStemDic(m_pStemDicWord))
					{//7y : S(len~n+2)�� stem ������ �ִ°� ? IMR1_3_00011
						LoggingFunction(pNode, "IMR1_3_00012+ ");
						m_p1thAddString = AddTwoStr("���þ�", m_pNewBuffer);
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
						//8y : stem = S(len~n+2), ending = �����þ + NewBuf()+OldBuf() IMR1_3_00012
						InsertStemString(pNode, m_pStemDicWord);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, m_p2thAddString);
						SaveHTokenEnding(pNode);
					}
					else
					{
						LoggingFunction(pNode, "IMR1_3_00013+ ");
						//8n : ending = �����þ + NewBuf()+OldBuf(), n=n+2->MAR2�� IMR1_3_00013
						m_p1thAddString = AddTwoStr("���þ�", m_pNewBuffer);
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
						InsertEndingString(pNode, m_p2thAddString);
						SaveHTokenEnding(pNode);
						//n=n+2
						MovingLeftPositionWord(pNode->pHToken->strSource, nStart); //n=n+1
						MovingLeftPositionWord(pNode->pHToken->strSource, nStart); //n=n+1
						InitOneWordMarsk();

						GoMAR2(pNode, nStart, mStart, lStart);
					}
				}
				else
				{
					CHAR_DELETE(m_pStemDicWord);
					m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
					m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
					m_pAddTwoString = AddTwoStr(m_pStemDicWord, "��");//S(len~n+1)+���á�
					LoggingFunction(pNode, "IMR1_3_00014+ ");
					if (IsExistStemDic(m_pAddTwoString))
					{//7n : S(len~n+1) + ���á��� stem ������ �����ϴ°� ? IMR1_3_00014
						LoggingFunction(pNode, "IMR1_3_00015+ ");
						m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
						//8y : stem = S(len~n+1) + ���á�, ending = ��� + NewBuf()+OldBuf() IMR1_3_00015
						InsertStemString(pNode, m_pAddTwoString);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, m_p2thAddString);
						SaveHTokenEnding(pNode);
					}
					else
					{
						LoggingFunction(pNode, "IMR1_3_00016+ ");
						if (IsExistStemDic(m_pStemDicWord))
						{//8n : S(len~n+1)�� stem ������ �����ϴ°�? IMR1_3_00016
							m_p1thAddString = AddTwoStr("�þ�", m_pNewBuffer);
							m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
							//9y : stem = S(len~n+1), ending = ���þ+ NewBuf()+OldBuf() IMR1_3_00017
							InsertStemString(pNode, m_pStemDicWord);
							SaveHTokenStem(pNode);
							InsertEndingString(pNode, m_p2thAddString);
							SaveHTokenEnding(pNode);
						}
						else
						{
							CHAR_DELETE(m_pStemDicWord);
							m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
							m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+2)

							MovingLeftPositionWord(pNode->pHToken->strSource, nStart);//n++ �� n > len
							InitOneWordMarsk();

							m_p1thAddString = AddTwoStr("�þ�", m_pNewBuffer);
							m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
							
							InsertEndingString(pNode, m_p2thAddString);
							SaveHTokenEnding(pNode);
							
							LoggingFunction(pNode, "IMR1_3_00018+ ");
							if (nStart <= 0)
							{//9n : ending = ���þ+NewBuf()+OldBuf(), n++ �� n > len? IMR1_3_00018
								LoggingFunction(pNode, "IMR1_3_00019+ ");

								//10y : Error IMR1_3_00019
								GoErrorFunction(pNode);//1'y : Error MAR1_1_00002
							}
							else
							{	
								LoggingFunction(pNode, "IMR1_3_00020+ ");
								if((strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0) && IsExistStemDic(m_pStemDicWord))
								{//10n : S(n+1) == ���ϡ� || ���ǡ� || ���ޡ� && S(len~n+2)�� Stem ������ �ִ°�? IMR1_3_00020
									LoggingFunction(pNode, "IMR1_3_00021+ ");
									m_p1thAddString = AddTwoStr("�þ�", m_pNewBuffer);
									m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
									//11y : Stem = S(len~n+1), Ending =���þ+ NewBuf()+OldBuf()  IMR1_3_00021
									InsertStemString(pNode, m_pStemDicWord);
									SaveHTokenStem(pNode);
									InsertEndingString(pNode, m_p2thAddString);
									SaveHTokenEnding(pNode);
								}
								else
								{
									//11n : S(n) = [mask]  �� MAR2�� IMR1_3_00022
									SetOneWordMarsk();

									GoMAR2(pNode, nStart, mStart, lStart);
								}
							}
						}
					}
				}
			}
			else
			{
				LoggingFunction(pNode, "IMR1_3_00023+ ");
				if (strcmp(m_pNthOneChar, "Ű") == 0)
				{//6n : S(n) �� ��Ű���ΰ�? IMR1_3_00023
					m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
					m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
					m_pAddTwoString = AddTwoStr(m_pStemDicWord, "��");//S(len~n+1)+���ϡ�
					bool bFind = IsExistStemDic(m_pAddTwoString, m_stemFeature01);
					LoggingFunction(pNode, "IMR1_3_00024+ ");
					if (bFind == true)
					{//7y : {S(len~n+1)+���ϡ�}�� stem������ �ִ°�? IMR1_3_00024
						LoggingFunction(pNode, "IMR1_3_00025+ ");
						if (m_pOldBuffer == NULL || strcmp(m_pOldBuffer, "") == 0)
						{//8y : stem = S(len~n+1)+���ϡ�[VN][Lex]-[N] �� OldBuf()�� Null�ΰ�? IMR1_3_00025
							LoggingFunction(pNode, "IMR1_3_00026+ ");
							m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
							//9y : ending = ���⡱+[V][Aff]+NewBuf() IMR1_3_00026
							InsertStemString(pNode, m_pAddTwoString);
							SaveHTokenStem(pNode);
							AddHTokenStem(pNode, STEM_DIC_V);
							AddHTokenStem(pNode, STEM_DIC_CMP);//VN->CMP�� ������
							AddHTokenStem(pNode, STEM_DIC_LEX);
							DeleteHTokenStem(pNode, STEM_DIC_N);

							InsertEndingString(pNode, m_p1thAddString);
							SaveHTokenEnding(pNode);
							AddHTokenEnding(pNode, ENDING_DIC_V);
							AddHTokenEnding(pNode, ENDING_DIC_AFF);
						}
						else
						{
							LoggingFunction(pNode, "IMR1_3_00027+ ");
							m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
							m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
							//9n : ending = ���⡱+[V][Aff]+NewBuf()+OldBuf() IMR1_3_00027
							InsertStemString(pNode, m_pAddTwoString);
							SaveHTokenStem(pNode);
							AddHTokenStem(pNode, STEM_DIC_V);
							AddHTokenStem(pNode, STEM_DIC_CMP);//VN->CMP�� ������
							AddHTokenStem(pNode, STEM_DIC_LEX);
							DeleteHTokenStem(pNode, STEM_DIC_N);
							InsertEndingString(pNode, m_p2thAddString);
							SaveHTokenEnding(pNode);
							AddHTokenEnding(pNode, ENDING_DIC_V);
							AddHTokenEnding(pNode, ENDING_DIC_AFF);
						}
					}
					else
					{
						LoggingFunction(pNode, "IMR1_3_00028+ ");
						if (bFind == true && m_stemFeature01[STEM_DIC_CONC] != IS_CEHCK_FEATURE)
						{//8n : S(len~n+1)�� stem ������ �ְ�, S(len~n+1) != [Conc]�ΰ�? IMR1_3_00028
							LoggingFunction(pNode, "IMR1_3_00029+ ");
							if (m_pOldBuffer == NULL || strcmp(m_pOldBuffer, "") == 0)
							{//9y : stem = S(len~n+1)+���ϡ�[VN][Lex]-[N] �� OldBuf()�� Null�ΰ�? IMR1_3_00029
								LoggingFunction(pNode, "IMR1_3_00030+ ");
								m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
								//10y : ending = ���⡱+[V][Aff]+NewBuf() IMR1_3_00030
								InsertStemString(pNode, m_pAddTwoString);
								SaveHTokenStem(pNode);
								AddHTokenStem(pNode, STEM_DIC_V);
								AddHTokenStem(pNode, STEM_DIC_CMP);//VN->CMP�� ������
								AddHTokenStem(pNode, STEM_DIC_LEX);
								DeleteHTokenStem(pNode, STEM_DIC_N);
								InsertEndingString(pNode, m_p1thAddString);
								SaveHTokenEnding(pNode);
								AddHTokenEnding(pNode, ENDING_DIC_V);
								AddHTokenEnding(pNode, ENDING_DIC_AFF);
							}
							else
							{
								LoggingFunction(pNode, "IMR1_3_00031+ ");
								m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
								m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
								//10n : ending = ���⡱+[V][Aff]+NewBuf()+OldBuf( ) IMR1_3_00031
								InsertStemString(pNode, m_pAddTwoString);
								SaveHTokenStem(pNode);
								AddHTokenStem(pNode, STEM_DIC_V);
								AddHTokenStem(pNode, STEM_DIC_CMP);//VN->CMP�� ������
								AddHTokenStem(pNode, STEM_DIC_LEX);
								DeleteHTokenStem(pNode, STEM_DIC_N);
								InsertEndingString(pNode, m_p2thAddString);
								SaveHTokenEnding(pNode);
								AddHTokenEnding(pNode, ENDING_DIC_V);
								AddHTokenEnding(pNode, ENDING_DIC_AFF);
							}
						}
						else
						{
							CHAR_DELETE(m_pNthOneChar);
							m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
							m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)�� ����
							LoggingFunction(pNode, "IMR1_3_00032+ ");
							if(strcmp(m_pNthOneChar, "��") == 0)
							{//9n : S(n+1)�� ���á��ΰ�? IMR1_3_00032
								CHAR_DELETE(m_pStemDicWord);
								m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
								m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+2)
								LoggingFunction(pNode, "IMR1_3_00033+ ");
								if(IsExistStemDic(m_pStemDicWord))
								{//10y : S(len~n+2)�� stem ������ �ִ°�? IMR1_3_00033
									LoggingFunction(pNode, "IMR1_3_00034+ ");
									m_pAddTwoString = AddTwoStr(m_pStemDicWord, "��Ű");
									m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
									//11y : stem = S(len~n+2) +����Ű��+[V][Act]-[N], ending = NewBuf()+OldBuf() IMR1_3_00034
									InsertStemString(pNode, m_pAddTwoString);
									SaveHTokenStem(pNode);
									AddHTokenStem(pNode, STEM_DIC_V);
									AddHTokenStem(pNode, STEM_DIC_ACT);
									DeleteHTokenStem(pNode, STEM_DIC_N);
									InsertEndingString(pNode, m_p1thAddString);
									SaveHTokenEnding(pNode);
								}
								else
								{
									LoggingFunction(pNode, "IMR1_3_00035+ ");
									//11n : S(n) = [mask]  �� MAR1�� ��� IMR1_3_00035
									SetOneWordMarsk();

									GoMAR1_2(pNode, nStart, mStart, lStart);
								}
							}
							else
							{
								LoggingFunction(pNode, "IMR1_3_00036+ ");
								//10n : S(n) = [mask]  �� MAR1�� ��� IMR1_3_00036
								SetOneWordMarsk();

								GoMAR1_2(pNode, nStart, mStart, lStart);
							}
						}
					}
				}
				else
				{
					LoggingFunction(pNode, "IMR1_3_00037+ ");
					if (strcmp(m_pNthOneChar, "��") == 0)
					{//7n : S(n) �� ���ϡ��ΰ�? IMR1_3_00037
						CHAR_DELETE(m_pNthOneChar);
						m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
						m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)�� ����	
						if(strcmp(m_pNthOneChar,"��") ==0)
						{//8y : S(n+1)�� ���硱�ΰ�? IMR1_3_00038
							CHAR_DELETE(m_pStemDicWord);
							m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
							m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+2)
							LoggingFunction(pNode, "IMR1_3_00039+ ");
							if (IsExistStemDic(m_pStemDicWord))
							{//9y : S(len~n+2)�� stem ������ �ִ°�? IMR1_3_00039
								LoggingFunction(pNode, "IMR1_3_00040+ ");
								m_pAddTwoString = AddTwoStr(m_pStemDicWord, "����");
								m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
								//10y : stem = S(len~n+2)+�����ϡ� +[V][Act]-[N], ending = NewBuf()+OldBuf() IMR1_3_00040
								InsertStemString(pNode, m_pAddTwoString);
								SaveHTokenStem(pNode);
								AddHTokenStem(pNode, STEM_DIC_V);
								AddHTokenStem(pNode, STEM_DIC_ACT);
								DeleteHTokenStem(pNode, STEM_DIC_N);
								InsertEndingString(pNode, m_p1thAddString);
								SaveHTokenEnding(pNode);
							}
							else
							{
								LoggingFunction(pNode, "IMR1_3_00041+ ");
								//10n : S(n) = [mask]  �� MAR1�� ��� IMR1_3_00041
								SetOneWordMarsk();

								GoMAR1_2(pNode, nStart, mStart, lStart);
							}
						}
						else
						{
							LoggingFunction(pNode, "IMR1_3_00042+ ");
							//9n : S(n) = [mask]  �� IRM4 �� IMR1_3_00042
							SetOneWordMarsk();

							GoIMR4(pNode, nStart, mStart, lStart);
						}
					}
					else
					{
						LoggingFunction(pNode, "IMR1_3_00043+ ");
						if (strcmp(m_pNthOneChar, "��") == 0)
						{//8n : S(n) �� �������ΰ�? IMR1_3_00043
							CHAR_DELETE(m_pStemDicWord);
							m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
							m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
							LoggingFunction(pNode, "IMR1_3_00044+ ");
							if(strcmp(m_pStemDicWord, "��") == 0 || strcmp(m_pStemDicWord, "�ֹ�") == 0 || strcmp(m_pStemDicWord, "���") == 0)
							{//9y : S(len~n+1)�� ���衱|| ���ֹ���|| ������� �ΰ�? IMR1_3_00044
								LoggingFunction(pNode, "IMR1_3_00045+ ");
								m_pAddTwoString = AddTwoStr(m_pStemDicWord, "��");
								m_p1thAddString = AddTwoStr("��", m_pOldBuffer);
								//10y : stem = S(len~n+1) +���á�, ending = ���+ OldBuf() IMR1_3_00045
								InsertStemString(pNode, m_pStemDicWord);//S(len~n+1)�� ������ ������ ���������� ���� ��, "��"�� ������ �߰��Ѵ�.
								SaveHTokenStem(pNode);
								InsertStemString(pNode, m_pAddTwoString);
								InsertEndingString(pNode, m_p1thAddString);
								SaveHTokenEnding(pNode);
							}
							else
							{
								//10n : S(n) = [mask] -> MAR1�� ��� IMR1_3_00046
								SetOneWordMarsk();

								GoMAR1_2(pNode, nStart, mStart, lStart);
							}
						}
						else
						{
							LoggingFunction(pNode, "IMR1_3_00047+ ");
							if (strcmp(m_pJongChar, "��") == 0)
							{//9n : C(n)�� �������ΰ� IMR1_3_00047
								//10y : IRM2�� IMR1_3_00048
								LoggingFunction(pNode, "IMR1_3_00048+ ");
								GoIMR2(pNode, nStart, mStart, lStart);
							}
							else
							{
								LoggingFunction(pNode, "IMR1_3_00049+ ");
								if ((strcmp(m_pJongChar, "��") == 0 || strcmp(m_pJongChar, "��") == 0) || strcmp(m_pNthOneChar, "��") == 0)
								{//10n : {C(n) == �� || ��} || {S(n) == ���⡱}�ΰ�? IMR1_3_00049
									LoggingFunction(pNode, "IMR1_3_00050+ ");
									//11y : IRM3����   IMR1_3_00050

									GoIMR3(pNode, nStart, mStart, lStart);
								}
								else
								{
									LoggingFunction(pNode, "IMR1_3_00051+ ");
									if(CompareVariousTable01(m_pNthOneChar, 0, false) == true ||
										CompareVariousTable02(m_pNthOneChar, 1, false) == true ||
										CompareVariousTable02(m_pNthOneChar, 2, false) == true ||
										CompareVariousTable03(m_pNthOneChar, 0, false) == true ||
										CompareVariousTable03(m_pNthOneChar, 1, false) == true ||
										CompareVariousTable04(m_pNthOneChar, 1, false) == true ||
										CompareVariousTable04(m_pNthOneChar, 2, false) == true ||
										CompareVariousTable04(m_pNthOneChar, 3, false) == true ||
										CompareVariousTable05(m_pNthOneChar, ALL_TABLE_05, true) == true ||
										CompareVariousTable06(m_pNthOneChar, ALL_TABLE_06, true) == true ||
										CompareVariousTable07(m_pNthOneChar, ALL_TABLE_07, true) == true ||
										strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0)
									{//11n : S(n)�� txx || ���䡱||������||���족 �ΰ�? IMR1_3_00051
										LoggingFunction(pNode, "IMR1_3_00052+ ");
										//12y : IRM4�� IMR1_3_00052

										GoIMR4(pNode, nStart, mStart, lStart);
									}
									else
									{
										LoggingFunction(pNode, "IMR1_3_00053+ ");

										//12n : Error IMR1_3_00053
										GoErrorFunction(pNode);
									}
								}
							}
						}
					}
				}
			}
		}
	}

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//IMR2
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CNAHangulAnaly::GoIMR2(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
	m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)�� ����
	LoggingFunction(pNode, "IMR2_00001+ ");
	if (strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0)
	{//S(n) == ������ || ���ҡ� || ���ڡ��ΰ� IMR2_00001
		m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
		m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
		LoggingFunction(pNode, "IMR2_00002+ ");
		if (IsExistStemDic(m_pStemDicWord))
		{//1y : S(len~n+1)�� stem ������ �ִ°� IMR2_00002
			LoggingFunction(pNode, "IMR2_00002+ ");
			m_p1thAddString = AddTwoStr(m_pNthOneChar,m_pNewBuffer);
			m_p2thAddString = AddTwoStr(m_p2thAddString, m_pOldBuffer);
			//2y : stem = S(len~n+1), ending = ������ || ���ҡ� || ���ڡ� + NewBuf()+OldBuf() IMR2_00003
			InsertStemString(pNode, m_pStemDicWord);
			SaveHTokenStem(pNode);
			InsertStemString(pNode, m_pAddTwoString);
			InsertEndingString(pNode, m_p2thAddString);
			SaveHTokenEnding(pNode);
		}
		else
		{	
			//2n : OldBuf = ������ || ���ҡ� || ���ڡ� + NewBuf()+OldBuf(), NewBuf()=Null IMR2_00004
			m_p1thAddString = AddTwoStr(m_pNthOneChar, m_pNewBuffer);
			m_p2thAddString = AddTwoStr(m_p1thAddString, m_pNewBuffer);
			CHAR_DELETE(m_pOldBuffer);
			int nLen = strlen(m_p2thAddString);
			m_pOldBuffer = new char[nLen + 1];
			memset(m_pOldBuffer, 0x00, nLen + 1);
			strcpy(m_pOldBuffer, m_p2thAddString);
			InsertArrayFeatureOld(m_p2thAddString);

			LoggingFunction(pNode, "IMR2_00005(m = n ��  n++->n>le)+ ");
			mStart = nStart;

			int nMovingLeftPosition = ReturnNumberMovingLeftPositionWord(pNode->pHToken->strSource, nStart);

			if (nMovingLeftPosition <= 0)
			{
				LoggingFunction(pNode, "IMR2_00006+ ");
				GoErrorFunction(pNode);
			}
			else
			{
				LoggingFunction(pNode, "IMR2_00007+ ");
				//3n : MAR2�� IMR2_00007
				
				GoMAR2(pNode, nStart, mStart, lStart);
			}
		}
	}
	else
	{
		GoIMR2_1(pNode, nStart, mStart, lStart);
	}

}

void CNAHangulAnaly::GoIMR2_1(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
	m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)�� ����
	LoggingFunction(pNode, "IMR2_1_00001+ ");
	if (strcmp(m_pNthOneChar, "��") == 0)
	{//1n : S(n)�� �������ΰ� ? IMR2_1_00001
		CHAR_DELETE(m_pNthOneChar);
		m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
		m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)�� ����	
		m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
		if(strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0)
		{//2y : S(n+1)�� ���ϡ� || ���ǡ��ΰ� ? IMR2_1_00002
			if (IsExistStemDic(m_pStemDicWord))
			{//3y : S(len~n+1)�� stem ������ �����ϴ°� ? IMR2_1_00003
				LoggingFunction(pNode, "IMR2_1_00004+ ");
				m_pAddTwoString = AddTwoStr("��", m_pNewBuffer);
				m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
				//4y : stem = S(len~n+1) - [N], ending = ������ + NewBuf()+OldBuf() IMR2_1_00004
				InsertStemString(pNode, m_pStemDicWord);
				SaveHTokenStem(pNode);
				AddHTokenStem(pNode, STEM_DIC_V);
				AddHTokenStem(pNode, STEM_DIC_ACT);
				DeleteHTokenStem(pNode, STEM_DIC_N);
				InsertEndingString(pNode, m_p1thAddString);
				SaveHTokenEnding(pNode);
			}
			else
			{
				CHAR_DELETE(m_pStemDicWord);
				m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+2)
				if (IsExistStemDic(m_pStemDicWord))
				{//4n : S(len~n+2)�� stem ������ �����ϴ°� ? IMR2_1_00005
					LoggingFunction(pNode, "IMR2_1_00006+ ");
					m_pAddTwoString = AddTwoStr(m_pStemDicWord, m_pNthOneChar);
					m_p1thAddString = AddTwoStr("��", m_pOldBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pNewBuffer);

					//5y : stem = S(len~n+2) + ���ϡ� || ���ǡ� + [V] - [N], ending = ������ + NewBuf()+OldBuf() IMR2_1_00006
					InsertStemString(pNode, m_pStemDicWord);//S(len~n+1)�� ������ ������ ���������� ���� ��, "��"�� ������ �߰��Ѵ�.
					SaveHTokenStem(pNode);
					InsertStemString(pNode, m_pAddTwoString);
					AddHTokenStem(pNode, STEM_DIC_V);
					DeleteHTokenStem(pNode, STEM_DIC_N);
					InsertEndingString(pNode, m_p2thAddString);
					SaveHTokenEnding(pNode);
				}
				else
				{
					LoggingFunction(pNode, "IMR2_1_00007+ ");
					//5n : Error IMR2_1_00007

					GoErrorFunction(pNode);
				}
			}
		}
		else
		{
			m_pAddTwoString = AddTwoStr(m_pStemDicWord, "��");
			LoggingFunction(pNode, "IMR2_1_00008+ ");
			if (IsExistStemDic(m_pAddTwoString))
			{//3n : S(len~n+1) + ���̡��� stem ������ �����ϴ°� ? IMR2_1_00008
				LoggingFunction(pNode, "IMR2_1_00009+ ");
				m_p1thAddString = AddTwoStr("��", m_pOldBuffer);
				m_p2thAddString = AddTwoStr(m_p1thAddString, m_pNewBuffer);
				//4y : stem = S(len~n+1) + ���̡�, ending = ������ + NewBuf()+OldBuf() IMR2_1_00009
				InsertStemString(pNode, m_pStemDicWord);//S(len~n+1)
				SaveHTokenStem(pNode);//S(len~n+1)�� ������������
				InsertStemString(pNode, m_pAddTwoString);//S(len~n+1) + ���̡�
				InsertEndingString(pNode, m_p2thAddString);
				SaveHTokenEnding(pNode);
			}
			else
			{
				LoggingFunction(pNode, "IMR2_1_00010+ ");
				if (IsExistStemDic(m_pStemDicWord))
				{//4n : stem = S(len~n+1)�� stem ������ �����ϴ°� ? IMR2_1_00010
					LoggingFunction(pNode, "IMR2_1_00011+ ");
					m_p1thAddString = AddTwoStr("��", "��");
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pNewBuffer);
					m_p3thAddString = AddTwoStr(m_p2thAddString, m_pOldBuffer);
					//5y : stem = S(len~n+1), ending = ���̡�[Pol] - [N] + ������ + NewBuf()+OldBuf() IMR2_1_00011
					InsertStemString(pNode, m_pStemDicWord);//S(len~n+1)
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_p3thAddString);
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, ENDING_DIC_POL);//[Pol]�����ο�
				}
				else
				{
					//5n : OldBuf() = ������ + NewBuf()+OldBuf(), NewBuf()=Null, m = n, n++ �� n > len ? IMR2_1_00012
					m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pNewBuffer);
					CHAR_DELETE(m_pOldBuffer);
					CHAR_DELETE(m_pNewBuffer);
					int nLen = strlen(m_p2thAddString);
					m_pOldBuffer = new char[nLen + 1];
					memset(m_pOldBuffer, 0x00, nLen + 1);
					strcpy(m_pOldBuffer, m_p2thAddString);
					InsertArrayFeatureOld(m_p2thAddString);
					
					LoggingFunction(pNode, "IMR2_1_00012(m = n ��  n++->n>le)+ ");
					mStart = nStart;

					int nMovingLeftPosition = ReturnNumberMovingLeftPositionWord(pNode->pHToken->strSource, nStart);

					if (nMovingLeftPosition <= 0)
					{//6y : Error IMR2_1_00013
						LoggingFunction(pNode, "IMR2_1_00013+ ");
						GoErrorFunction(pNode);
					}
					else
					{
						LoggingFunction(pNode, "IMR2_1_00014+ ");
						//6n : MAR2�� IMR2_1_00014

						GoMAR2(pNode, nStart, mStart, lStart);
					}

				}
			}
		}
	}
	else
	{
		LoggingFunction(pNode, "IMR2_1_00015+ ");
		if (strcmp(m_pNthOneChar, "��") == 0)
		{//2n : S(n)�� ���̡��ΰ� ? IMR2_1_00015
			CHAR_DELETE(m_pNthOneChar);
			m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
			m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)�� ����	
			LoggingFunction(pNode, "IMR2_1_00016+ ");
			if(strcmp(m_pNthOneChar, "��") == 0)
			{//3y : S(n+1)�� �������ΰ� ? IMR2_1_00016
				m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+2)
				LoggingFunction(pNode, "IMR2_1_00017+ ");
				if (IsExistStemDic(m_pStemDicWord))
				{//4y : S(len~n+2)�� stem ������ �ִ°� ? IMR2_1_00017
					LoggingFunction(pNode, "IMR2_1_00018+ ");
					m_p1thAddString = AddTwoStr("����", "��");
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pNewBuffer);
					m_p3thAddString = AddTwoStr(m_p2thAddString, m_pOldBuffer);
					//5y : stem = S(len~n+2), ending = �����á� + ������ + NewBuf()+OldBuf() IMR2_1_00018
					InsertStemString(pNode, m_pStemDicWord);//S(len~n+2)
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_p3thAddString);
					SaveHTokenEnding(pNode);
				}
				else
				{
					LoggingFunction(pNode, "IMR2_1_00019(n=n+2)+ ");
					//5n : ending = �����á� + ������ + NewBuf()+OldBuf(), n = n + 2->MAR2�� IMR2_1_00019
					m_p1thAddString = AddTwoStr("����", "��");
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pNewBuffer);
					m_p3thAddString = AddTwoStr(m_p2thAddString, m_pOldBuffer);
					InsertEndingString(pNode, m_p3thAddString);
					SaveHTokenEnding(pNode);

					MovingLeftPositionWord(pNode->pHToken->strSource, nStart);//n+1
					MovingLeftPositionWord(pNode->pHToken->strSource, nStart);//n+1
					InitOneWordMarsk();
					
					GoMAR2(pNode, nStart, mStart, lStart);

				}
			}
			else
			{
				m_pAddTwoString = AddTwoStr(m_pStemDicWord,"��");
				LoggingFunction(pNode, "IMR2_1_00020+ ");
				if (IsExistStemDic(m_pAddTwoString))
				{//4n : S(len~n+1) + ���á��� stem ������ �����ϴ°� ? IMR2_1_00020
					m_p1thAddString = AddTwoStr("��", m_pOldBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pNewBuffer);
					//5y : stem = S(len~n+1) + ���á�, ending = ������ + NewBuf()+OldBuf() IMR2_1_00021
					InsertStemString(pNode, m_pStemDicWord);//S(len~n+1)
					SaveHTokenStem(pNode);
					InsertStemString(pNode, m_pAddTwoString);
					InsertEndingString(pNode, m_p2thAddString);
					SaveHTokenEnding(pNode);
				
				}
				else
				{
					if (IsExistStemDic(m_pStemDicWord))
					{//5n : S(len~n+1)�� stem ������ �����ϴ°� ? IMR2_1_00022
						m_p1thAddString = AddTwoStr("�þ�", m_pOldBuffer);
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pNewBuffer);
						//6y : stem = S(len~n+1), ending = ���á� + ������ + NewBuf()+OldBuf() IMR2_1_00023
						InsertStemString(pNode, m_pStemDicWord);//S(len~n+1)
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, m_p2thAddString);
						SaveHTokenEnding(pNode);
					}
					else
					{
						//6n: OldBuf() = ���á� + ������ + NewBuf() + OldBuf(), NewBuf() = Null, n++ �� n>len ? IMR2_1_00024
						m_p1thAddString = AddTwoStr("�þ�", m_pNewBuffer);
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
						CHAR_DELETE(m_pOldBuffer);
						CHAR_DELETE(m_pNewBuffer);
						int nLen = strlen(m_p2thAddString);
						m_pOldBuffer = new char[nLen + 1];
						memset(m_pOldBuffer, 0x00, nLen + 1);
						strcpy(m_pOldBuffer, m_p2thAddString);
						InsertArrayFeatureOld(m_p2thAddString);


						LoggingFunction(pNode, "IMR2_1_00024(m = n ��  n++->n>le)+ ");
						
						int nMovingLeftPosition = ReturnNumberMovingLeftPositionWord(pNode->pHToken->strSource, nStart);

						if (nMovingLeftPosition <= 0)
						{//7y : Error IMR2_1_00025
							LoggingFunction(pNode, "IMR2_1_00025+ ");

							GoErrorFunction(pNode);
						}
						else
						{
							m_nRestorationJongChar = Restoration(m_pJongChar);//�����ؾ� �� ������
							ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);//C(n) = ������
							CHAR_DELETE(m_pStemDicWord);
							m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
							bool bFind = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
							LoggingFunction(pNode, "IMR2_1_00026+ ");
							if(bFind == true && m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE)
							{//7n : C(n)=������ �� S(len~n)�� stem ������ �ְ�, S(len ~n) != [N]�ΰ� ? IMR2_1_00026
								LoggingFunction(pNode, "IMR2_1_00027+ ");
								//8y : stem = S(len~n), ending = OldBuf() IMR2_1_00027
								InsertStemString(pNode, m_pStemDicWord);
								SaveHTokenStem(pNode);
								InsertEndingString(pNode, m_pOldBuffer);
								SaveHTokenEnding(pNode);
							}
							else
							{
								LoggingFunction(pNode, "IMR2_1_00028+ ");
								//8n : C(n) = #C(n), �� MAR2��  IMR2_1_00028
								//������ ���������� ����
								JasoRestoration(pNode, nStart, FORCE_REPLACE_ENDWORD, m_nRestorationJongChar);// C(n) = #C(n)

								GoMAR2(pNode, nStart, mStart, lStart);
							}
						}
					}
				}
			}
		}
		else
		{
			LoggingFunction(pNode, "IMR2_1_00029+ ");
			if (strcmp(m_pNthOneChar, "��") == 0)
			{//3n : S(n)�� �������ΰ�? IMR2_1_00029
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
				m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
				LoggingFunction(pNode, "IMR2_1_00030+ ");
				if (IsExistStemDic(m_pStemDicDeleteJongWord))
				{//4y : S(len~n)-C(n)�� stem ������ �ִ°�? IMR2_1_00030
					LoggingFunction(pNode, "IMR2_1_00031+ ");
					m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					//5y : stem = S(len~n)- C(n), ending = ������+NewBuf()+OldBuf() IMR2_1_00031
					InsertStemString(pNode, m_pStemDicDeleteJongWord);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_p2thAddString);
					SaveHTokenEnding(pNode);
				}
				else
				{
					LoggingFunction(pNode, "IMR2_1_00032+ ");
					//5n : Error  IMR2_1_00032

					GoErrorFunction(pNode);
				}
			}
			else
			{
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
				m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
				LoggingFunction(pNode, "IMR2_1_00033+ ");
				if (IsExistStemDic(m_pStemDicDeleteJongWord))
				{//4n : S(len~n)-C(n)�� stem ������ �ִ°�?(������ϴ�) IMR2_1_00033
					LoggingFunction(pNode, "IMR2_1_00034+ ");
					m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					//5y : Stem = S(len~n)-C(n), ending = ������+NewBuf()+OldBuf() IMR2_1_00034
					InsertStemString(pNode, m_pStemDicDeleteJongWord);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_p2thAddString);
					SaveHTokenEnding(pNode);
				}
				else
				{
					LoggingFunction(pNode, "IMR2_1_00035+ ");
					m_pChoJungChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_CHOJUNGSUNG);//S(n)-C(n)
					if (CompareVariousTable02(m_pChoJungChar, 1, false) == true || CompareVariousTable02(m_pChoJungChar, 2, false) == true
						|| CompareVariousTable03(m_pChoJungChar, 0, false) == true || CompareVariousTable03(m_pChoJungChar, 1, false) == true
						|| CompareVariousTable04(m_pChoJungChar, 1, false) == true || CompareVariousTable04(m_pChoJungChar, 2, false) == true || CompareVariousTable04(m_pChoJungChar, 3, false) == true
						|| CompareVariousTable05(m_pChoJungChar, ALL_TABLE_05, true) == true
						|| CompareVariousTable06(m_pChoJungChar, ALL_TABLE_06, true) == true
						|| CompareVariousTable07(m_pChoJungChar, ALL_TABLE_07, true) == true)
					{//5n : S(n)-C(n)�� txx�ΰ� IMR2_1_00035
						LoggingFunction(pNode, "IMR2_1_00036+ ");
						//6y : OldBuf() = ������+NewBuf()+OldBuf(), NewBuf()=Null, S(n) = S(n)-C(n) �� IRM4�� IMR2_1_00036
						m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
						CHAR_DELETE(m_pNewBuffer);
						CHAR_DELETE(m_pOldBuffer);
						int nLen = strlen(m_p2thAddString);
						m_pOldBuffer = new char[nLen + 1];
						memset(m_pOldBuffer, 0x00, nLen + 1);
						strcpy(m_pOldBuffer, m_p2thAddString);
						InsertArrayFeatureOld(m_p2thAddString);
						
						ReplaceHangulJongsungNull(pNode, nStart);//S(n)=S(n)-C(n) ��������

						GoIMR4(pNode, nStart, mStart, lStart);

					}
					else
					{
						LoggingFunction(pNode, "IMR2_1_00037+ ");
						m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
						//6n : ending = ������+NewBuf()+OldBuf(), stem = S(len~n)-C(n) -> Error IMR2_1_00037
						InsertStemString(pNode, m_pStemDicDeleteJongWord);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, m_p2thAddString);
						SaveHTokenEnding(pNode);

						GoErrorFunction(pNode);
					}
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//IMR3
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CNAHangulAnaly::GoIMR3(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
	m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)�� ����
	LoggingFunction(pNode, "IMR3_00001+ ");
	if (strcmp(m_pJongChar, "��") == 0)
	{//C(n)�� ����'�ΰ�? IMR3_00001
		m_nRestorationJongChar = Restoration(m_pJongChar);//�����ؾ� �� ������
		ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);//C(n) = ������
		m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
		LoggingFunction(pNode, "IMR3_00002+ ");
		if (IsExistStemDic(m_pStemDicWord))
		{//1y : C(n)=����" �� S(len~n)�� stem ������ �ִ°�? IMR3_00002
			LoggingFunction(pNode, "IMR3_00003+ ");
			m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
			m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
			//2y : stem = S(len~n) + [VN] - [V], ending = ������ + NewBuf() + OldBuf() IMR3_00003
			InsertStemString(pNode, m_pStemDicWord);
			SaveHTokenStem(pNode);
			AddHTokenStem(pNode, STEM_DIC_V);
			AddHTokenStem(pNode, STEM_DIC_CMP);//VN->CMP�� ������
			InsertEndingString(pNode, m_p2thAddString);
			SaveHTokenEnding(pNode);
		}
		else
		{
			LoggingFunction(pNode, "IMR3_00004+ ");

			//2n : Error IMR3_00004
			GoErrorFunction(pNode);
		}
	}
	else
	{
		LoggingFunction(pNode, "IMR3_00005+ ");
		if (strcmp(m_pNthOneChar, "��") == 0)
		{//1n : S(n)�� ���ӡ��ΰ� ? IMR3_00005
			m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
			LoggingFunction(pNode, "IMR3_00006+ ");
			if (IsExistStemDic(m_pStemDicWord))
			{//2y : S(len~n)�� stem ������ �ִ°� ? IMR3_00006
				LoggingFunction(pNode, "IMR3_00007+ ");
				//3y : Error(���ո��м� * / ������, ������) IMR3_00007

				GoErrorFunction(pNode);
			}
			else
			{
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
				m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
				LoggingFunction(pNode, "IMR3_00008+ ");
				if (IsExistStemDic(m_pStemDicDeleteJongWord))
				{//3n : S(len~n) - C(n)�� stem ������ �ִ°� ? IMR3_00008
					LoggingFunction(pNode, "IMR3_00009+ ");
					m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					//4y : stem = S(len~n) - C(n) + [VN] - [N], ending = ������ + NewBuf() + OldBuf() IMR3_00009
					InsertStemString(pNode, m_pStemDicDeleteJongWord);
					SaveHTokenStem(pNode);
					AddHTokenStem(pNode, STEM_DIC_V);
					AddHTokenStem(pNode, STEM_DIC_CMP);//VN->CMP�� ������
					DeleteHTokenStem(pNode, STEM_DIC_N);
					InsertEndingString(pNode, m_p2thAddString);
					SaveHTokenEnding(pNode);
				}
				else
				{
					CHAR_DELETE(m_pStemDicWord);
					m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
					m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
					LoggingFunction(pNode, "IMR3_00010+ ");
					if (IsExistStemDic(m_pStemDicWord))
					{//4n : S(len~n+1)�� stem ������ �ִ°� ? IMR3_00010
						LoggingFunction(pNode, "IMR3_00011+ ");
						//5y : stem = S(len~n+1), ending = ���̡�[Pol] + ������ + NewBuf() + OldBuf() - ���� IMR3_00011
						m_p1thAddString = AddTwoStr("��", "��");
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pNewBuffer);
						m_p3thAddString = AddTwoStr(m_p2thAddString, m_pOldBuffer);
						InsertStemString(pNode, m_pStemDicWord);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, m_p3thAddString);
						SaveHTokenEnding(pNode);
						AddHTokenEnding(pNode, ENDING_DIC_POL);
					}
					else
					{
						m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
						InsertEndingString(pNode, m_p2thAddString);
						SaveHTokenEnding(pNode);

						LoggingFunction(pNode, "IMR3_00012(n++->n>len)+ ");
						int nMovingLeftPosition = ReturnNumberMovingLeftPositionWord(pNode->pHToken->strSource, nStart);

						if (nMovingLeftPosition <= 0)
						{
							LoggingFunction(pNode, "IMR3_00013+ ");

							GoErrorFunction(pNode);
						}
						else
						{
							LoggingFunction(pNode, "IMR3_00014+ ");
							//6n : MAR2�� IMR3_00014

							GoMAR2(pNode, nStart, mStart, lStart);
						}
					}
				}
			}
		}
		else
		{
			LoggingFunction(pNode, "IMR3_00015+ ");
			if (strcmp(m_pNthOneChar, "��") == 0)
			{//2n : S(n)�� �������ΰ� ? IMR3_00015
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
				m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
				LoggingFunction(pNode, "IMR3_00016+ ");
				if (IsExistStemDic(m_pStemDicDeleteJongWord))
				{//3y : (S(len~n)-������)�� stem ������ �ִ°� ? IMR3_00016
					LoggingFunction(pNode, "IMR3_00017+ ");
					m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					//4y : stem = S(len~n)-������ + [VN] - [V], ending = ������ + NewBuf() + OldBuf() IMR3_00017
					InsertStemString(pNode, m_pStemDicDeleteJongWord);
					SaveHTokenStem(pNode);
					AddHTokenStem(pNode, STEM_DIC_CMP);//VN->CMP�� ������
					DeleteHTokenStem(pNode, STEM_DIC_V);
					InsertEndingString(pNode, m_p2thAddString);
					SaveHTokenEnding(pNode);
				}
				else
				{
					CHAR_DELETE(m_pStemDicWord);
					m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
					m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
					LoggingFunction(pNode, "IMR3_00018+ ");
					if (IsExistStemDic(m_pStemDicWord, m_stemFeature01))
					{//4n : S(len~n+1)�� stem ������ �ִ°� ? IMR3_00018
						m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)�� ����
						m_pJongChar = GetChojungJongsungChar(m_pOneNPluseNumChar, CHAR_JONGSUNG);//C(n+1)�� ����
						LoggingFunction(pNode, "IMR3_00018+ ");
						if(m_pJongChar != NULL && strcmp(m_pJongChar, "") != 0 && m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE)
						{//5y : {C(n+1) != Null} && {S(len~n+1) != [N]}�ΰ� ? IMR3_00019
							LoggingFunction(pNode, "IMR3_00020+ ");
							m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
							m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
							//6y : stem = S(len~n+1) + [VN] - [V], ending = ������ + NewBuf() + OldBuf() IMR3_00020
							InsertStemString(pNode, m_pStemDicWord);
							SaveHTokenStem(pNode);
							AddHTokenStem(pNode, STEM_DIC_CMP);//VN->CMP�� ������
							DeleteHTokenStem(pNode, STEM_DIC_V);
							InsertEndingString(pNode, m_p2thAddString);
							SaveHTokenEnding(pNode);
						}
						else
						{
							LoggingFunction(pNode, "IMR3_00021+ ");
							//6n : Error(20030129 ������ ���� ���Ʊ����ᡱ) IMR3_00021

							GoErrorFunction(pNode);
						}
					}
					else
					{
						m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
						InsertEndingString(pNode, m_p2thAddString);
						SaveHTokenEnding(pNode);

						LoggingFunction(pNode, "IMR3_00022(m = n �� n++ �� n>len)+ ");
						mStart = nStart;

						int nMovingLeftPosition = ReturnNumberMovingLeftPositionWord(pNode->pHToken->strSource, nStart);

						if (nMovingLeftPosition <= 0)
						{//5n : tmp_Stem = S(len~n+1) + [VN - [V], ending = ���⡱ + NewBuf() + OldBuf(), m = n �� n++ �� n>len ? IMR3_00022
							LoggingFunction(pNode, "IMR3_00023+ ");

							GoErrorFunction(pNode);
						}
						else
						{
							LoggingFunction(pNode, "IMR3_00024+ ");

							//6n : MAR2 IMR3_00024
							GoMAR2(pNode, nStart, mStart, lStart);
						}
					}
				}
			}
			else
			{
				GoIMR3_1(pNode, nStart, mStart, lStart);
			}
		}
	}
}

void CNAHangulAnaly::GoIMR3_1(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
	m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)�� ����
	LoggingFunction(pNode, "IMR3_1_00001+ ");
	if (strcmp(m_pNthOneChar, "��") == 0)
	{//3n : S(n)�� ���⡱�ΰ� ? (20030129 ���� ���� ������ ����) IMR3_1_00001
		m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
		m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
		LoggingFunction(pNode, "IMR3_1_00002+ ");
		if (IsExistStemDic(m_pStemDicWord, m_stemFeature01))
		{//3'y : S(len~n+1)�� stem ������ �ִ°�? IMR3_1_00002
			LoggingFunction(pNode, "IMR3_1_00003+ ");
			if(m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE)
			{//4y : {S(len~n+1) != [N]}�ΰ� ? IMR3_1_00003
				LoggingFunction(pNode, "IMR3_1_00004+ ");
				m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
				m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
				//5y : stem = S(len~n+1) + [Cmp] - [V], ending = ���⡱ + NewBuf() + OldBuf() IMR3_1_00004
				InsertStemString(pNode, m_pStemDicWord);
				SaveHTokenStem(pNode);
				AddHTokenStem(pNode, STEM_DIC_CMP);//VN->CMP�� ������
				DeleteHTokenStem(pNode, STEM_DIC_V);
				InsertEndingString(pNode, m_p2thAddString);
				SaveHTokenEnding(pNode);
			}
			else
			{
				LoggingFunction(pNode, "IMR3_1_00005+ ");
				//5n : Error IMR3_1_00005

				GoErrorFunction(pNode);
			}
		}
		else
		{
			m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
			m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
			InsertEndingString(pNode, m_p2thAddString);
			SaveHTokenEnding(pNode);

			LoggingFunction(pNode, "IMR3_1_00006(m = n �� n++ �� n>len)+ ");
			mStart = nStart;

			int nMovingLeftPosition = ReturnNumberMovingLeftPositionWord(pNode->pHToken->strSource, nStart);

			if (nMovingLeftPosition <= 0)
			{//4n : tmp_Stem = S(len~n+1) + [Cmp] - [V], ending = ���⡱ + NewBuf() + OldBuf(), m = n �� n++ �� n>len ? IMR3_1_00006
				 //5y : Error IMR3_1_00007
				LoggingFunction(pNode, "IMR3_1_00007+ ");

				GoErrorFunction(pNode);
			}
			else
			{
				LoggingFunction(pNode, "IMR3_1_00008+ ");

				//5n : MAR2 IMR3_1_00008
				GoMAR2(pNode, nStart, mStart, lStart);
			}
		}
	}
	else
	{
		m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
		m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
		LoggingFunction(pNode, "IMR3_1_00009+ ");
		if (IsExistStemDic(m_pStemDicWord, m_stemFeature01))
		{//3'n : S(len~n)-C(n)�� stem ������ �ִ°�? IMR3_1_00009
			LoggingFunction(pNode, "IMR3_1_00010+ ");
			m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
			m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
			//4y : stem = S(len~n)-C(n) + [VN] - [V], ending = ������ + NewBuf() + OldBuf() IMR3_1_00010
			InsertStemString(pNode, m_pStemDicWord);
			SaveHTokenStem(pNode);
			AddHTokenStem(pNode, STEM_DIC_CMP);//VN->CMP�� ������
			DeleteHTokenStem(pNode, STEM_DIC_V);
			InsertEndingString(pNode, m_p2thAddString);
			SaveHTokenEnding(pNode);
		}
		else
		{ 
			LoggingFunction(pNode, "IMR3_1_00011+ ");
			if(strcmp(m_pNthOneChar, "��") == 0)
			{//4n : S(n)�� ���ԡ��ΰ� ? IMR3_1_00011
				CHAR_DELETE(m_pStemDicWord);
				m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
				LoggingFunction(pNode, "IMR3_1_00012+ ");
				if (IsExistStemDic(m_pStemDicWord, m_stemFeature01))
				{//5y : S(len~n+1)�� stem ������ �ִ°� ? IMR3_1_00012
					LoggingFunction(pNode, "IMR3_1_00013+ ");
					m_pAddTwoString = AddTwoStr(m_pStemDicWord, "��");
					m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					//6y : stem = S(len~n+1) + ���ϡ� + [VN] - [N], ending = ������ + NewBuf() + OldBuf() IMR3_1_00013
					InsertStemString(pNode, m_pStemDicWord);
					SaveHTokenStem(pNode);
					InsertStemString(pNode, m_pAddTwoString);
					AddHTokenStem(pNode, STEM_DIC_CMP);//VN->CMP�� ������
					DeleteHTokenStem(pNode, STEM_DIC_N);
					InsertEndingString(pNode, m_p2thAddString);
					SaveHTokenEnding(pNode);
				}
				else
				{
					LoggingFunction(pNode, "IMR3_1_00014+ ");
					//6n : S(n) = [mask]->MAR2�� IMR3_1_00014
					SetOneWordMarsk();

					GoMAR2(pNode, nStart, mStart, lStart);
				}
			}
			else
			{
				LoggingFunction(pNode, "IMR3_1_00015+ ");
				if (strcmp(m_pNthOneChar, "��") == 0)
				{//5n : S(n)�� ���ʡ��ΰ� ? IMR3_1_00015
					CHAR_DELETE(m_pStemDicWord);
					m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
					m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
					LoggingFunction(pNode, "IMR3_1_00016+ ");
					if (IsExistStemDic(m_pStemDicWord, m_stemFeature01))
					{//6y : S(len~n+1)�� stem ������ �ִ°� ? IMR3_1_00016
						LoggingFunction(pNode, "IMR3_1_00017+ ");
						m_pAddTwoString = AddTwoStr(m_pStemDicWord, "��");
						m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
						//7y : stem = S(len~n+1) + ���ǡ� + [VN] - [V], ending = ������ + NewBuf() + OldBuf() IMR3_1_00017
						InsertStemString(pNode, m_pStemDicWord);
						SaveHTokenStem(pNode);
						InsertStemString(pNode, m_pAddTwoString);
						AddHTokenStem(pNode, STEM_DIC_CMP);//VN->CMP�� ������
						DeleteHTokenStem(pNode, STEM_DIC_V);
						InsertEndingString(pNode, m_p2thAddString);
						SaveHTokenEnding(pNode);
					}
					else
					{
						LoggingFunction(pNode, "IMR3_1_00018+ ");
						//7n : S(n) = [mask]->MAR2�� IMR3_1_00018
						SetOneWordMarsk();

						GoMAR2(pNode, nStart, mStart, lStart);
					}
				}
				else
				{
					LoggingFunction(pNode, "IMR3_1_00019+ ");
					if (strcmp(m_pNthOneChar, "��") == 0)
					{//6n : S(n)�� ���ɡ��ΰ� ? IMR3_1_00019
						CHAR_DELETE(m_pStemDicWord);
						m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
						m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
						LoggingFunction(pNode, "IMR3_1_00020+ ");
						if (IsExistStemDic(m_pStemDicWord, m_stemFeature01))
						{//7y : S(len~n+1)�� stem ������ �ִ°� ? IMR3_1_00020
							LoggingFunction(pNode, "IMR3_1_00021+ ");
							m_p1thAddString = AddTwoStr("��", "��");
							m_p2thAddString = AddTwoStr(m_p1thAddString, m_pNewBuffer);
							m_p3thAddString = AddTwoStr(m_p2thAddString, m_pOldBuffer);
							//8y : stem = (len~n + 1), ending = ���á� + ������ + NewBuf() + OldBuf() IMR3_1_00021
							InsertStemString(pNode, m_pStemDicWord);
							SaveHTokenStem(pNode);
							InsertEndingString(pNode, m_p3thAddString);
							SaveHTokenEnding(pNode);
						}
						else
						{
							
							LoggingFunction(pNode, "IMR3_1_00022+ ");
							//8n : S(n) = [mask] -> MAR1�� ��� IMR3_1_00022
							SetOneWordMarsk();

							GoMAR1_2(pNode, nStart, mStart, lStart);
						}
					}
					else
					{
						LoggingFunction(pNode, "IMR3_1_00023+ ");
						//8n : S(n) = [mask] �� MAR1-2�� IMR3_1_00023
						SetOneWordMarsk();

						GoMAR1_2(pNode, nStart, mStart, lStart);
					}
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//IMR4
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CNAHangulAnaly::GoIMR4(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
	m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)�� ����
	LoggingFunction(pNode, "IMR4_00001+ ");
	if (CompareVariousTable05(m_pNthOneChar, ALL_TABLE_05, true) == true)
	{//S(n)�� Table05�ΰ� ? IMR4_00001
		m_nRestorationJongChar = Restoration(m_pJongChar);//�����ؾ� �� ������
		ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA02);//C(n) = ������
		m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
		m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
		m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
		bool bFindStem = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
		bool bFindEnding = IsExistEndingDic(m_pFormNTo1Word, m_endingFeature01);
		LoggingFunction(pNode, "IMR4_00002+ ");
		if(bFindStem == true)
		{//1y : C(n)=������ �� S(len~n)�� stem ������ �ִ°�? IMR4_00002
			LoggingFunction(pNode, "IMR4_00003+ ");
			if(m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && bFindEnding == true && m_endingFeature01[ENDING_DIC_N] != IS_CEHCK_FEATURE)
			{//2y : S(len~n) != [n] && S(N-1~1) != [n]�ΰ�?  IMR4_00003
				//3y : stem = S(len~n), ending = S(n-1~1)[=NewBuf()+OldBuf()] IMR4_00004
				InsertStemString(pNode, m_pStemDicWord);
				SaveHTokenStem(pNode);
				InsertEndingString(pNode, m_pFormNTo1Word);
				SaveHTokenEnding(pNode);
			}
			else
			{
				ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);//C(n) = ������
				CHAR_DELETE(m_pStemDicWord);
				CHAR_DELETE(m_pFormNTo1Word);
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
				m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
				m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
				LoggingFunction(pNode, "IMR4_00005+ ");
				if(IsExistStemDic(m_pStemDicWord))
				{//3n : C(n)=������ �� S(len~n)�� stem ������ �ִ°�? IMR4_00005
					LoggingFunction(pNode, "IMR4_00006+ ");
					//4y : stem = S(len~n), ending = S(n-1~1)[=NewBuf()+OldBuf()] IMR4_00006
					InsertStemString(pNode, m_pStemDicWord);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_pFormNTo1Word);
					SaveHTokenEnding(pNode);
				}
				else
				{
					LoggingFunction(pNode, "IMR4_00007+ ");

					//4n : S(n) = [mask] -> MAR1�� ��� IMR4_00007
					SetOneWordMarsk();

					GoMAR1_2(pNode, nStart, mStart, lStart);
				}
			}
		}
		else
		{
			CHAR_DELETE(m_pStemDicWord);
			CHAR_DELETE(m_pFormNTo1Word);
			ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);//C(n) = ������
			m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
			m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
			m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
			LoggingFunction(pNode, "IMR4_00008+ ");
			if (IsExistStemDic(m_pStemDicWord, m_stemFeature01))
			{//2n : C(n)=������ �� S(len~n)�� stem ������ �ִ°�? IMR4_00008
				LoggingFunction(pNode, "IMR4_00009+ ");
				if(m_stemFeature01[STEM_DIC_LIR] == IS_CEHCK_FEATURE)
				{//3y : S(len~n)�� [Lir]�ΰ�? IMR4_00009
					LoggingFunction(pNode, "IMR4_00010+ ");
					m_p1thAddString = AddTwoStr("��", m_pFormNTo1Word);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pNewBuffer);
					m_p3thAddString = AddTwoStr(m_p2thAddString, m_pOldBuffer);
					//4y : stem = S(len~n), ending = ������+ S(n-1~1)+NewBuf()+OldBuf()] IMR4_00010
					InsertStemString(pNode, m_pStemDicWord);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_pFormNTo1Word);
					SaveHTokenEnding(pNode);
				}
				else
				{
					LoggingFunction(pNode, "IMR4_00011+ ");
					m_p1thAddString = AddTwoStr(m_pFormNTo1Word, m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					//4n : stem = S(len~n), ending = S(n-1~1)+NewBuf()+OldBuf() IMR4_00011
					InsertStemString(pNode, m_pStemDicWord);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_p2thAddString);
					SaveHTokenEnding(pNode);
				}
			}
			else
			{
				LoggingFunction(pNode, "IMR4_00012+ ");

				//3n :  S(n) = [mask] -> MAR1�� ��� IMR4_00012
				SetOneWordMarsk();

				GoMAR1_2(pNode, nStart, mStart, lStart);
			}
		}
	}
	else
	{
		LoggingFunction(pNode, "IMR4_00013+ ");
		if (CompareVariousTable06(m_pNthOneChar, ALL_TABLE_06, true) == true)
		{//1n : S(n)�� Table06�ΰ�?   IMR4_00013
			m_nRestorationJongChar = Restoration(m_pJongChar);//�����ؾ� �� ������
			ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA06);//C(n) = ������
			m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
			m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
			m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
			bool bFindStem = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
			bool bFindEnding = IsExistEndingDic(m_pFormNTo1Word, m_endingFeature01);
			LoggingFunction(pNode, "IMR4_00014+ ");
			if(bFindStem == true)
			{//2y : C(n)=������ �� S(len~n)�� stem ������ �ִ°�? IMR4_00014
				LoggingFunction(pNode, "IMR4_00015+ ");
				if (m_stemFeature01[STEM_DIC_V] == IS_CEHCK_FEATURE && bFindEnding == true && m_endingFeature01[ENDING_DIC_V] == IS_CEHCK_FEATURE)
				{//3y : S(len~n) == [v] && S(n-1~1) == [v]�ΰ�?  IMR4_00015
					LoggingFunction(pNode, "IMR4_00016+ ");
					m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
					//4y : stem = S(len~n), ending = NewBuf()+OldBuf() IMR4_00016
					InsertStemString(pNode, m_pStemDicWord);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_p1thAddString);
					SaveHTokenEnding(pNode);
				}
				else
				{
					CHAR_DELETE(m_pStemDicWord);
					CHAR_DELETE(m_pFormNTo1Word);
					ReplaceHangulJongsungNull(pNode, nStart);//delete ������
					m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
					m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
					m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
					LoggingFunction(pNode, "IMR4_00017+ ");
					if(IsExistStemDic(m_pStemDicWord))
					{//4n : delete ������ �� S(len~n)�� stem ������ �ִ°�? IMR4_00017
						LoggingFunction(pNode, "IMR4_00018+ ");
						//5y : stem = S(len~n), ending = S(n-1~1)[=NewBuf()+OldBuf()] IMR4_00018
						InsertStemString(pNode, m_pStemDicWord);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, m_pFormNTo1Word);
						SaveHTokenEnding(pNode);
					}
					else
					{
						CHAR_DELETE(m_pStemDicWord);
						CHAR_DELETE(m_pNthOneChar);
						m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
						m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
						m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
						LoggingFunction(pNode, "IMR4_00019+ ");
						if (IsExistStemDic(m_pStemDicWord, m_stemFeature01))
						{//5n : S(len~n+1)�� stem ������ �ִ°�? IMR4_00019
							LoggingFunction(pNode, "IMR4_00020+ ");
							if(strcmp(m_pNthOneChar, "��") == 0 && (m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_CMP] == IS_CEHCK_FEATURE))
							{//6y : S(n)�� ���̡��̰�, S(len~n+1) == [n]||[amb]||[Cmp]�ΰ�? IMR4_00020
								LoggingFunction(pNode, "IMR4_00021+ ");
								m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
								m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
								//7y : stem = S(len~n+1), ending = ���̡�[Pol]-[N]+NewBuf()+OldBuf() IMR4_00021
								InsertStemString(pNode, m_pStemDicWord);
								SaveHTokenStem(pNode);
								InsertEndingString(pNode, m_p2thAddString);
								SaveHTokenEnding(pNode);
								AddHTokenEnding(pNode, ENDING_DIC_POL);//[Pol]�����ο�
								DeleteHTokenEnding(pNode, ENDING_DIC_N);//[N]��������
							}
							else
							{
								LoggingFunction(pNode, "IMR4_00022+ ");

								//7n : S(n) = [mask] -> MAR1�� ��� IMR4_00022
								SetOneWordMarsk();

								GoMAR1_2(pNode, nStart, mStart, lStart);
							}
						}
						else
						{
							LoggingFunction(pNode, "IMR4_00023+ ");

							//6n : S(n) = [mask] -> MAR1�� ��� IMR4_00023
							SetOneWordMarsk();

							GoMAR1_2(pNode, nStart, mStart, lStart);
						}
					}
				}
			}
			else
			{
				CHAR_DELETE(m_pStemDicWord);
				CHAR_DELETE(m_pFormNTo1Word);
				ReplaceHangulJongsungNull(pNode, nStart);//delete ������
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
				m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
				m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
				bool bFindStem = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
				bool bFindEnding = IsExistEndingDic(m_pFormNTo1Word, m_endingFeature01);
				LoggingFunction(pNode, "IMR4_00024+ ");
				if(bFindStem == true)
				{//3n : delete ������ �� S(len~n)�� stem ������ �ִ°�? IMR4_00024
					LoggingFunction(pNode, "IMR4_00025+ ");
					if((m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] == IS_CEHCK_FEATURE) || (m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] != IS_CEHCK_FEATURE) || (m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_AMB] == IS_CEHCK_FEATURE))
					{//4y : {S(len~n) == [N] && S(n-1~1) == [N]}�̰ų�, {S(len~n) <> [N] && S(n-1~1) <> [N]}�̰ų�, {S(len~n) == [amb] || S(n-1) == [amb]}�ΰ�? IMR4_00025
						LoggingFunction(pNode, "IMR4_00026+ ");
						m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
						//5y : stem = S(len~n), ending = NewBuf()+OldBuf() IMR4_00026
						InsertStemString(pNode, m_pStemDicWord);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, m_p1thAddString);
						SaveHTokenEnding(pNode);
					}
					else
					{
						LoggingFunction(pNode, "IMR4_00027+ ");

						//5n : S(n) = [mask] �� MAR1�� ��� IMR4_00027
						SetOneWordMarsk();

						GoMAR1_2(pNode, nStart, mStart, lStart);
					}
				}
				else
				{
					CHAR_DELETE(m_pNthOneChar);
					CHAR_DELETE(m_pStemDicWord);
					CHAR_DELETE(m_pFormNTo1Word);
					m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
					m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
					m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
					bool bFindStem = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
					LoggingFunction(pNode, "IMR4_00028+ ");
					if(strcmp(m_pNthOneChar, "��") == 0 && (m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_CMP] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_ADV] == IS_CEHCK_FEATURE))
					{//4n : S(n) == ���̡� && S(len~n+1) == [n]||[amb]||[Cmp]||[Adv]�ΰ�? IMR4_00028
						m_pLastOneChar = GetOneChar(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));//S(1)
						m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
						bool bFindFirst = IsExistEndingDic(m_pFormNTo1Word, m_endingFeature01);
						bool bFindSecond = IsExistEndingDic(m_pNewBuffer, m_endingFeature02);
						bool bFindThird = IsExistEndingDic(m_pOldBuffer, m_endingFeature03);
						LoggingFunction(pNode, "IMR4_00029+ ");
						if(strcmp(m_pNthOneChar, m_pLastOneChar) != 0  && (bFindFirst == true || (bFindSecond == true && m_endingFeature02[ENDING_DIC_V] == IS_CEHCK_FEATURE && bFindThird == true)))
						{//5y : S(n) != S(1) && {IsExistEndDic_S(n~1) || (NewBuffer == E_V && IsExistEndDic_OldBuffer)}�ΰ�? IMR4_00029
							//6y : stem = S(len~n+1), ending=���̡�[Pol]-[N]+NewBuf()+OldBuf() IMR4_00030
							LoggingFunction(pNode, "IMR4_00030+ ");
							m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
							m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
							//6y : stem = S(len~n+1), ending=���̡�[Pol]-[N]+NewBuf()+OldBuf() IMR4_00030
							InsertStemString(pNode, m_pStemDicWord);
							SaveHTokenStem(pNode);
							InsertEndingString(pNode, m_p2thAddString);
							SaveHTokenEnding(pNode);
							AddHTokenEnding(pNode, ENDING_DIC_POL);//[Pol]�����ο�
							DeleteHTokenEnding(pNode, ENDING_DIC_N);//[N]��������
						}
						else
						{
							CHAR_DELETE(m_pFormNTo1Word);
							m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
							m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
							bool bFindEnding = IsExistEndingDic(m_pFormNTo1Word, m_endingFeature01);
							LoggingFunction(pNode, "IMR4_00031+ ");
							if(m_endingFeature01[ENDING_DIC_PRS] == IS_CEHCK_FEATURE || m_endingFeature02[ENDING_DIC_N] == IS_CEHCK_FEATURE 
								|| (strcmp(m_pNewBuffer,"��") == 0 && m_pOldBuffer != NULL && strcmp(m_pOldBuffer, "") != 0)
								|| (strcmp(m_pNewBuffer, "��") == 0 && m_pOldBuffer != NULL && strcmp(m_pOldBuffer, "") != 0)
								|| (strcmp(m_pNewBuffer, "��") == 0 && m_pOldBuffer != NULL && strcmp(m_pOldBuffer, "") != 0))
							{//6n : S(n-1~1) == [prs] || NewBuf() == [N] || (NewBuffer == ���ߡ� && OldBuffer != NULL) || (NewBuffer == ���ڡ� && OldBuffer != NULL) || (NewBuffer == ������ && OldBuffer != NULL)�ΰ�? IMR4_00031
								LoggingFunction(pNode, "IMR4_00032+ ");
								//7y : Error IMR4_00032

								GoErrorFunction(pNode);
							}
							else
							{
								CHAR_DELETE(m_pFormNTo1Word);
								CHAR_DELETE(m_pStemDicWord);
								m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
								m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
								m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
								bool bFind = IsExistEndingDic(m_pFormNTo1Word, m_endingFeature01);
								LoggingFunction(pNode, "IMR4_00033+ ");
								if((bFind == true && m_endingFeature01[ENDING_DIC_N] == IS_CEHCK_FEATURE) || strcmp(m_pFormNTo1Word, "��") == 0 || strcmp(m_pFormNTo1Word, "��") == 0)
								{//7n :  {S(n~1) == [N]||:��:||:��}�ΰ�? IMR4_00033
									LoggingFunction(pNode, "IMR4_00034+ ");
									m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
									m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
									//8y : stem = S(len~n+1), ending = ���̡�-[Agt][Pss]+NewBuf()+OldBuf() IMR4_00034
									InsertStemString(pNode, m_pStemDicWord);
									SaveHTokenStem(pNode);
									InsertEndingString(pNode, m_p2thAddString);
									SaveHTokenEnding(pNode);
									DeleteHTokenEnding(pNode, ENDING_DIC_AGT);//[AGT]��������
									DeleteHTokenEnding(pNode, ENDING_DIC_PSS);//[PSS]��������
								}
								else
								{
									LoggingFunction(pNode, "IMR4_00035+ ");
									m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
									m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
									//8n : stem = S(len~n+1), ending = ���̡�[Pol]-[N]+NewBuf()+OldBuf() IMR4_00035
									InsertStemString(pNode, m_pStemDicWord);
									SaveHTokenStem(pNode);
									InsertEndingString(pNode, m_p2thAddString);
									SaveHTokenEnding(pNode);
									AddHTokenEnding(pNode, ENDING_DIC_POL);//[POL]�����߰�
									DeleteHTokenEnding(pNode, ENDING_DIC_N);//[N]��������
								}
							}
						}
					}
					else
					{
						CHAR_DELETE(m_pStemDicWord);
						m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
						m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n+1)�� ����
						m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
						m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+2)
						bool bFindEnding = IsExistEndingDic(m_pOneNPluseNumChar, m_endingFeature01);
						bool bFindStem = IsExistEndingDic(m_pStemDicWord, m_stemFeature01);
						LoggingFunction(pNode, "IMR4_00036+ ");
						if(strcmp(m_pNthOneChar, "��") == 0
							&& (m_endingFeature01[ENDING_DIC_N] == IS_CEHCK_FEATURE || m_endingFeature01[ENDING_DIC_AFF] == IS_CEHCK_FEATURE || m_endingFeature01[ENDING_DIC_CAU] == IS_CEHCK_FEATURE)
							&& (m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE))
						{//5n : S(n) == ���̡��̰�, S(n+1)�� [n, aff, cau]�̰�, S(len~n+2) == [n] || [amb] || [VN]�ΰ�? IMR4_00036
							LoggingFunction(pNode, "IMR4_00037+ ");
							m_p1thAddString = AddTwoStr(m_pOneNPluseNumChar, m_pNthOneChar);
							m_p2thAddString = AddTwoStr(m_p1thAddString, m_pNewBuffer);
							m_p3thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
							//6y : Stem = S(len~n+2), Ending = S(n+1)+S(n)+NewBuf()+OldBuf() IMR4_00037
							InsertStemString(pNode, m_pStemDicWord);
							SaveHTokenStem(pNode);
							InsertEndingString(pNode, m_p3thAddString);
							SaveHTokenEnding(pNode);
						}
						else
						{
							LoggingFunction(pNode, "IMR4_00038+ ");

							//6n : S(n) = [mask] -> MAR1�� ��� IMR4_00038
							SetOneWordMarsk();

							GoMAR1_2(pNode, nStart, mStart, lStart);
						}
					}
				}
			}
		}
		else
		{ 
			GoIMR4_1(pNode, nStart, mStart, lStart);
		}
		
	}


}

void CNAHangulAnaly::GoIMR4_1(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
	m_pFormLenToNWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
	LoggingFunction(pNode, "IMR4_1_00001+ ");
	if(strcmp(m_pFormLenToNWord,"��") == 0)
	{//2n : S(len~n) == ���ۡ��ΰ� ? IMR4_1_00001
		InsertStemString(pNode, "Ǫ");
		SaveHTokenStem(pNode);
		LoggingFunction(pNode, "IMR4_1_00002+ ");
		if (m_pNewBuffer == NULL)
		{//3y : stem = ��Ǫ�� �� tmp_end == null�ΰ�? IMR4_1_00002
			LoggingFunction(pNode, "IMR4_1_00003+ ");
			//4y : ending = ���+[V][AFF][Con]+NewBuf()+OldBuf() IMR4_1_00003
			m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
			m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
			InsertEndingString(pNode, m_p2thAddString);
			SaveHTokenEnding(pNode);
			AddHTokenEnding(pNode, ENDING_DIC_V);//[V]�����߰�
			AddHTokenEnding(pNode, ENDING_DIC_AFF);//[AFF]�����߰�
			AddHTokenEnding(pNode, ENDING_DIC_CON);//[CON]�����߰�
		}
		else
		{
			LoggingFunction(pNode, "IMR4_1_00004+ ");
			//4n : ending = NewBuf()+OldBuf() IMR4_1_00004
			m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
			InsertEndingString(pNode, m_p1thAddString);
			SaveHTokenEnding(pNode);
		}
	}
	else
	{
		IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
		m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
		m_pFormLenToNWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
		LoggingFunction(pNode, "IMR4_1_00005+ ");
		if(CompareVariousTable01(m_pNthOneChar, 0, false) == true || CompareVariousTable02(m_pNthOneChar, 1, false) == true || CompareVariousTable02(m_pNthOneChar, 2, false) == true)
		{//3n : S(n)�� Table01[0] || Table02[1] || Table02[2] �ΰ�? IMR4_1_00005
			char* pTableValue;
			if (m_nTableNumber == 1)
			{
				pTableValue = Table01[m_nTableIndex].strTwoWord;
			}
			else if (m_nTableNumber == 2)
			{
				pTableValue = Table02[m_nTableIndex].strOneWord;
			}
			m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
			m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
			m_pAddTwoString = AddTwoStr(m_pStemDicWord, pTableValue);//S(len~n+1)+table01[1] or S(len~n+1)+Table02[0]
			bool bFind = IsExistStemDic(m_pAddTwoString, m_stemFeature01);
			LoggingFunction(pNode, "IMR4_1_00006 OR IMR4_1_00007+ ");
			if(bFind == true)
			{//4y : Table01[0]�� ��� :S(len~n+1)+table01[1] �� stem ������ �ִ°�?  IMR4_1_00006
				//Table02[1]||Table02[2]�� ��� : S(len~n+1)+table02[0]�� stem ������ �ִ°�? IMR4_1_00007
				LoggingFunction(pNode, "IMR4_1_00008+ ");
				if(m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_ADV] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_IC] == IS_CEHCK_FEATURE)
				{//5y : [{S(len~n+1)+table01[1]}||{S(len~n+1)+Table02[0]}] == [n] || [{S(len~n+1)+table01[1]}||{S(len~n+1)+Table02[0]}] == [ADV] || [{S(len~n+1)+table01[1]}||{S(len~n+1)+Table02[0]}] == [IC]�ΰ�? IMR4_1_00008
					LoggingFunction(pNode, "IMR4_1_00009+ ");
					//6y : S(n) = [mask] -> MAR1�� ��� IMR4_1_00009
					SetOneWordMarsk();

					GoMAR1_2(pNode, nStart, mStart, lStart);
				}
				else
				{
					LoggingFunction(pNode, "IMR4_1_00010+ ");
					m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);

					//6n : stem = S(len~n+1)+table01[1]||table02[0] , ending = ���+[V][Aff][Con]+NewBuf()+OldBuf() IMR4_1_00010
					InsertStemString(pNode, m_pAddTwoString);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_p2thAddString);
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, ENDING_DIC_V);//[V]�����߰�
					AddHTokenEnding(pNode, ENDING_DIC_AFF);//[AFF]�����߰�
					AddHTokenEnding(pNode, ENDING_DIC_CON);//[CON]�����߰�
				}
			}
			else
			{
				LoggingFunction(pNode, "IMR4_1_00011+ ");
				if(strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0)
				{//5n : S(n) == ������ || ���š� || �������ΰ�? IMR4_1_00011
					CHAR_DELETE(m_pAddTwoString);
					m_pAddTwoString = AddTwoStr(m_pFormLenToNWord,"��");
					LoggingFunction(pNode, "IMR4_1_00012+ ");
					if(IsExistStemDic(m_pAddTwoString))
					{//6y : S(len~n)+�� �� stem ������ �ִ°�? IMR4_1_00012
						m_p1thAddString = AddTwoStr( m_pNewBuffer, m_pOldBuffer);
						LoggingFunction(pNode, "IMR4_1_00013+ ");
						//7y : stem = S(len~n)+������, ending = NewBuf()+OldBuf() IMR4_1_00013
						InsertStemString(pNode, m_pAddTwoString);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, m_p1thAddString);
						SaveHTokenEnding(pNode);
					}
					else
					{
						//IMR4_1_00014~IMR4_1_00017 �ּ��� �߸� ǥ����.
						LoggingFunction(pNode, "IMR4_1_00018+ ");

						//7n : S(n) = [mask] -> MAR1�� IMR4_1_00018
						SetOneWordMarsk();
						GoMAR1_2(pNode, nStart, mStart, lStart);
					}
				}
				else
				{
					m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
					m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)�� ����
					m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
					m_pFormLenToNWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+2)
					bool bFind = IsExistStemDic(m_pFormLenToNWord, m_stemFeature01);
					LoggingFunction(pNode, "IMR4_1_00019+ ");
					if(strcmp(m_pNthOneChar, "��") == 0 && strcmp(m_pOneNPluseNumChar, "��") == 0 && (bFind == true && (m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE)))
					{//6n : S(n) == ���ѡ� && S(n+1) == ���á� && S(len~n+2) == [N]||[Amb]�ΰ�? IMR4_1_00019
						LoggingFunction(pNode, "IMR4_1_00020+ ");
						m_pAddTwoString = AddTwoStr(m_pFormLenToNWord, "��Ű");
						m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
						//7y : stem = S(len~n+2)+��Ű+[V][Act]-[N], ending = ���+OldBuf()+NewBuf()IMR4_1_00020
						InsertStemString(pNode, m_pFormLenToNWord);
						SaveHTokenStem(pNode);
						InsertStemString(pNode, m_pAddTwoString);
						AddHTokenStem(pNode, STEM_DIC_V);//[V]�����߰�
						AddHTokenStem(pNode, STEM_DIC_ACT);//[ACT]�����߰�
						DeleteHTokenStem(pNode, STEM_DIC_N);//[N]��������
						InsertEndingString(pNode, m_p2thAddString);
						SaveHTokenEnding(pNode);
					}
					else
					{
						LoggingFunction(pNode, "IMR4_1_00021+ ");
						if (strcmp(m_pNthOneChar, "��") == 0 && strcmp(m_pOneNPluseNumChar, "��") == 0 && (bFind == true && (m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE)))
						{//7n : S(n) == ������ && S(n+1) == ���١� && S(len~n+2) == [N]||[Amb]�ΰ�? IMR4_1_00021
							LoggingFunction(pNode, "IMR4_1_00022+ ");
							m_pAddTwoString = AddTwoStr(m_pFormLenToNWord, "��");
							m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
							m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
							//8y : stem = S(len~n+2)+��+[V][Sta]-[N], ending = ���+OldBuf()+NewBuf() IMR4_1_00022
							InsertStemString(pNode, m_pFormLenToNWord);
							SaveHTokenStem(pNode);
							InsertStemString(pNode, m_pAddTwoString);
							AddHTokenStem(pNode, STEM_DIC_V);//[V]�����߰�
							AddHTokenStem(pNode, STEM_DIC_STA);//[STA]�����߰�
							DeleteHTokenStem(pNode, STEM_DIC_N);//[N]��������
							InsertEndingString(pNode, m_p2thAddString);
							SaveHTokenEnding(pNode);
						}
						else
						{
							CHAR_DELETE(m_pFormLenToNWord);
							m_pFormNTo1Word = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+2)�� ����
							m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+3
							m_pFormLenToNWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+3)
							bool bFind = IsExistStemDic(m_pFormLenToNWord, m_stemFeature01);
							LoggingFunction(pNode, "IMR4_1_00023+ ");
							if((strcmp(m_pNthOneChar, "��") == 0 && strcmp(m_pOneNPluseNumChar, "��") == 0 && strcmp(m_pFormNTo1Word, "��") == 0) && (bFind == true && (m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE)))
							{//8n : S(n) == ������ && S(n+1) == ������ && S(n+2 ) == ������ && S(len~n+3) == [N]||[Amb]�ΰ�? IMR4_1_00023
								LoggingFunction(pNode, "IMR4_1_00024+ ");
								m_pAddTwoString = AddTwoStr(m_pFormLenToNWord, "����");
								m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
								m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
								//9y : stem = S(len~n+3)+����+[V][Sta]-[N], ending = ���+OldBuf()+NewBuf() IMR4_1_00024
								InsertStemString(pNode, m_pFormLenToNWord);
								SaveHTokenStem(pNode);
								InsertStemString(pNode, m_pAddTwoString);
								AddHTokenStem(pNode, STEM_DIC_V);//[V]�����߰�
								AddHTokenStem(pNode, STEM_DIC_STA);//[STA]�����߰�
								DeleteHTokenStem(pNode, STEM_DIC_N);//[N]��������
								InsertEndingString(pNode, m_p2thAddString);
								SaveHTokenEnding(pNode);
							}
							else
							{
								LoggingFunction(pNode, "IMR4_1_00025+ ");
								//9n : S(n) = [mask] -> MAR1�� ��� IMR4_1_00025
								SetOneWordMarsk();

								GoMAR1_2(pNode, nStart, mStart, lStart);
							}
						}
					}
				}
			}
		}
		else
		{
			IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
			m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
			m_pFormLenToNWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)

			LoggingFunction(pNode, "IMR4_1_00026+ ");
			if (CompareVariousTable03(m_pNthOneChar, 0, false) == true)
			{//4n : S(n)�� Table03[0]���� Ȯ���϶�.IMR4_1_00026
				LoggingFunction(pNode, "IMR4_1_00026+ ");
				if(IsExistStemDic(m_pFormLenToNWord))
				{//5y : S(len~n)�� stem ������ �ִ°�? IMR4_1_00027
					LoggingFunction(pNode, "IMR4_1_00028+ ");
					m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
					//6y : stem = S(len~n), ending = NewBuf()+OldBuf() IMR4_1_00028
					InsertStemString(pNode, m_pFormLenToNWord);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_p1thAddString);
					SaveHTokenEnding(pNode);
				}
				else
				{	
					m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
					m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
					bool bFind = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
					LoggingFunction(pNode, "IMR4_1_00029+ ");
					if (bFind == true)
					{//6n : S(len~n+1)�� stem ������ �ִ°�? IMR4_1_00029
						m_pLastOneChar = GetOneChar(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));//S(1)
						LoggingFunction(pNode, "IMR4_1_00030+ ");
						if((m_pOldBuffer == NULL || strcmp(m_pOldBuffer, "") == 0) && strcmp(m_pNthOneChar, m_pLastOneChar) == 0 && strcmp(m_pNthOneChar, "��") == 0)
						{//7y : {OldBuf() == NULL} && {S(n) == S(1)} && {S(n) == "��"} �ΰ�? IMR4_1_00030
							LoggingFunction(pNode, "IMR4_1_00031+ ");
							//8y : stem = S(len~n+1), ending = S(n) IMR4_1_00031
							InsertStemString(pNode, m_pStemDicWord);
							SaveHTokenStem(pNode);
							InsertEndingString(pNode, m_pNthOneChar);
							SaveHTokenEnding(pNode);
						}
						else
						{
							m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
							m_pOneNMinusNumChar = GetOneChar(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1)�� ����
							LoggingFunction(pNode, "IMR4_1_00032+ ");
							if(strcmp(m_pNthOneChar, "��") == 0 && (strcmp(m_pOneNMinusNumChar, "��") == 0 || strcmp(m_pOneNMinusNumChar, "��") == 0 || strcmp(m_pOneNMinusNumChar, "��") == 0) && (bFind == true && m_stemFeature01[STEM_DIC_CONC] == IS_CEHCK_FEATURE))
							{//8n : {S(n) == "��"} && {S(n-1)==������ || ((S(n-1) == "��" || "��")&& S(len~n+1) == [Conc]) }�ΰ�? IMR4_1_00032
								LoggingFunction(pNode, "IMR4_1_00033+ ");
								m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
								//9y : Stem =  S(len~n+1), Ending = S(n~n-1)+[N][Cor]-[V]+ S(n-2~1) IMR4_1_00033
								InsertStemString(pNode, m_pStemDicWord);
								SaveHTokenStem(pNode);
								InsertEndingString(pNode, m_pFormNTo1Word);
								SaveHTokenEnding(pNode);
								AddHTokenEnding(pNode, ENDING_DIC_N);//[N]�����߰�
								AddHTokenEnding(pNode, ENDING_DIC_COR);//[COR]�����߰�
								DeleteHTokenEnding(pNode, ENDING_DIC_V);//[V]��������
							}
							else
							{
								LoggingFunction(pNode, "IMR4_1_00034+ ");
								if (strcmp(m_pNthOneChar, "��") == 0)
								{//9n : S(n) == ���ǡ��ΰ�? IMR4_1_00034
									LoggingFunction(pNode, "IMR4_1_00035+ ");
									m_pFormNTo1Word = AddTwoStr(m_pStemDicWord, Table03[m_nTableIndex].strOneWord);//S(len~n+1)+table03[0]
									m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
									//10y : stem = S(len~n+1)+table03[0]+[V] [Fnc] [T078]-[N] [Act] [T104], ending = NewBuf()+OldBuf() IMR4_1_00035
									InsertStemString(pNode, m_pStemDicWord);//S(len~n+1)�� ���� ������ ���������� �����Ѵ�.
									SaveHTokenStem(pNode);
									InsertStemString(pNode, m_pFormNTo1Word);//S(len~n+1)+table03[0]
									AddHTokenStem(pNode, STEM_DIC_V);//[V]�����߰�
									AddHTokenStem(pNode, STEM_DIC_FNC);//[FNC]�����߰�
									AddHTokenStem(pNode, S_T078);//[T078]�����߰�
									DeleteHTokenStem(pNode, STEM_DIC_N);//[N]��������
									DeleteHTokenStem(pNode, STEM_DIC_ACT);//[N]��������
									DeleteHTokenStem(pNode, S_T104);//[T104]��������
									InsertEndingString(pNode, m_p1thAddString);
									SaveHTokenEnding(pNode);
								}
								else
								{
									LoggingFunction(pNode, "IMR4_1_00036+ ");
									m_pFormNTo1Word = AddTwoStr(m_pStemDicWord, Table03[m_nTableIndex].strOneWord);//S(len~n+1)+table03[0]
									m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
									//10n : stem = S(len~n+1)+table03[0]+[V][T077]-[N], ending = NewBuf()+OldBuf() IMR4_1_00036
									InsertStemString(pNode, m_pFormNTo1Word);
									SaveHTokenStem(pNode);
									AddHTokenStem(pNode, STEM_DIC_V);//[V]�����߰�
									AddHTokenStem(pNode, S_T077);//[T077]�����߰�
									DeleteHTokenStem(pNode, STEM_DIC_N);//[N]��������
									InsertEndingString(pNode, m_p1thAddString);
									SaveHTokenEnding(pNode);
								}
							}
						}
					}
					else
					{
						LoggingFunction(pNode, "IMR4_1_00037+ ");

						//7n :S(n) = [mask] -> MAR1�� ��� IMR4_1_00037
						SetOneWordMarsk();

						GoMAR1_2(pNode, nStart, mStart, lStart);
					}
				}
			}
			else
			{
				IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
				m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
				m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
				LoggingFunction(pNode, "IMR4_1_00038+ ");
				if (CompareVariousTable03(m_pNthOneChar, 1, false) == true)
				{//5n : S(n)�� Table03[1]���� Ȯ���϶�. IMR4_1_00038
					LoggingFunction(pNode, "IMR4_1_00037+ ");
					m_pFormNTo1Word = AddTwoStr(m_pStemDicWord, Table03[m_nTableIndex].strOneWord);//S(len~n+1)+table03[0]
					m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					LoggingFunction(pNode, "IMR4_1_00039+ ");
					if(IsExistStemDic(m_pFormNTo1Word))
					{//6y : S(len~n+1)+Table03[0]�� stem ������ �ִ°�? IMR4_1_00039
						LoggingFunction(pNode, "IMR4_1_00040+ ");
						//7y : stem = S(len~n+1)+Table03[0], ending=���+NewBuf()+Oldbuf() IMR4_1_00040
						InsertStemString(pNode, m_pFormNTo1Word);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, m_p2thAddString);
						SaveHTokenEnding(pNode);
					}
					else
					{
						LoggingFunction(pNode, "IMR4_1_00041+ ");
						bool bFind = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
						if(bFind == true)
						{//7n : S(len~n+1)�� stem ������ �ִ°�? IMR4_1_00041
							LoggingFunction(pNode, "IMR4_1_00042+ ");
							if(strcmp(m_pNthOneChar, "��") == 0 || strcmp(m_pNthOneChar, "��") == 0)
							{//8y : S(n) == ���ǡ� || ���š��ΰ�? IMR4_1_00042
								LoggingFunction(pNode, "IMR4_1_00043+ ");
								m_pFormNTo1Word = AddTwoStr(m_pStemDicWord, Table03[m_nTableIndex].strOneWord);//S(len~n+1)+table03[0]
								m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
								m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
								//9y : stem = S(len~n+1)+table03[0]+[V][Fnc][T078]-[N][Act][T104], ending = ���+NewBuf()+OldBuf() IMR4_1_00043
								InsertStemString(pNode, m_pStemDicWord);//S(len~n+1)�� �������� ����
								SaveHTokenStem(pNode);
								InsertStemString(pNode, m_pFormNTo1Word);//S(len~n+1)+table03[0]
								AddHTokenStem(pNode, STEM_DIC_V);//[V]�����߰�
								AddHTokenStem(pNode, STEM_DIC_FNC);//[FNC]�����߰�
								AddHTokenStem(pNode, S_T078);//[T078]�����߰�
								DeleteHTokenStem(pNode, STEM_DIC_N);//[N]��������
								DeleteHTokenStem(pNode, STEM_DIC_ACT);//[N]��������
								DeleteHTokenStem(pNode, S_T104);//[T104]��������
								InsertEndingString(pNode, m_p2thAddString);
								SaveHTokenEnding(pNode);
							}
							else
							{
								CHAR_DELETE(m_pFormNTo1Word);
								m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
								m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
								bool bFindEnding = IsExistEndingDic(m_pFormNTo1Word, m_endingFeature01);
								LoggingFunction(pNode, "IMR4_1_00044+ ");
								if(m_stemFeature01[STEM_DIC_CONC] != IS_CEHCK_FEATURE && m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && (bFindEnding == true && m_endingFeature01[ENDING_DIC_N] != IS_CEHCK_FEATURE))
								{//9n : S(len~n+1) != [Conc] && S(len~n+1) != [Tm] && ending ������S(n-1~1)!=[N]�ΰ�? IMR4_1_00044
									LoggingFunction(pNode, "IMR4_1_00045+ ");
									m_pFormNTo1Word = AddTwoStr(m_pStemDicWord, Table03[m_nTableIndex].strOneWord);//S(len~n+1)+table03[0]
									m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
									m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
									//10y : stem = S(len~n+1)+table03[0] +[V][T077]-[N], ending = ���+NewBuf()+OldBuf() IMR4_1_00045
									InsertStemString(pNode, m_pStemDicWord);//S(len~n+1)�� �������� ����
									SaveHTokenStem(pNode);
									InsertStemString(pNode, m_pFormNTo1Word);//S(len~n+1)+table03[0]
									AddHTokenStem(pNode, STEM_DIC_V);//[V]�����߰�
									AddHTokenStem(pNode, S_T077);//[T077]�����߰�
									DeleteHTokenStem(pNode, STEM_DIC_N);//[N]��������
									InsertEndingString(pNode, m_p2thAddString);
									SaveHTokenEnding(pNode);
								}
								else
								{
									LoggingFunction(pNode, "IMR4_1_00046+ ");
									//10n : Error(20030212 ������ ���� ������ء�)  IMR4_1_00046

									GoErrorFunction(pNode);
								}
							}
						}
						else
						{
							LoggingFunction(pNode, "IMR4_1_00047+ ");
							//8n : S(n) = [mask] -> MAR1�� ��� IMR4_1_00047
							SetOneWordMarsk();

							GoMAR1_2(pNode, nStart, mStart, lStart);
						}
					}
				}
				else
				{
					LoggingFunction(pNode, "IMR4_1_00048+ ");
					//6n : �������� IMR4_1_00048


					GoIMR4_2(pNode, nStart, mStart, lStart);
				}
			}
		}
	}

}

void CNAHangulAnaly::GoIMR4_2(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//���¼� �м��� �ʿ��� ���� �ʱ�ȭ
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)�� ����
	LoggingFunction(pNode, "IMR4_2_00001+ ");
	if(strcmp(m_pNthOneChar,"��") == 0 || strcmp(m_pNthOneChar, "��") == 0)
	{//(n) == ���� || �������ΰ�?IMR4_2_00001
		m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//n+1�� ����
		m_pJongChar = GetChojungJongsungChar(m_pOneNPluseNumChar, CHAR_JONGSUNG);//C(n+1)�� ����
		LoggingFunction(pNode, "IMR4_2_00002+ ");
		if(strcmp(m_pJongChar, "��") == 0)
		{//1y : C(n+1)�� �������ΰ�? IMR4_2_00002
			CHAR_DELETE(m_pNthOneChar);
			CHAR_DELETE(m_pJongChar);
			m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)�� ����
			m_nRestorationJongChar = Restoration(m_pJongChar);//�����ؾ� �� ������
			m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
			m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)�� ����
			m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
			m_pMinusTwoString = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, m_nPluseSyllable);//S(len~n+1)-C(n+1)
			m_pAddTwoString = AddTwoStr(m_pMinusTwoString,"��");
			bool bFindStem = IsExistStemDic(m_pAddTwoString, m_stemFeature01);
			LoggingFunction(pNode, "IMR4_2_00003+ ");
			if(bFindStem == true)
			{//2y : (S(len~n+1)-C(n+1)+������)�� stem ������ �ִ°�? IMR4_2_00003
				LoggingFunction(pNode, "IMR4_2_00004+ ");
				if(m_stemFeature01[STEM_DIC_RIR] == IS_CEHCK_FEATURE)
				{//3y : (S(len~n+1)-C(n+1)+������)�� [Rir]�ΰ�? IMR4_2_00004
					LoggingFunction(pNode, "IMR4_2_00005+ ");
					m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					//4y : stem = S(len~n+1)-C(n+1)+������, ending=���+[V][Aff][Con]+NewBuf()+OldBuf() IMR4_2_00005
					InsertStemString(pNode, m_pAddTwoString);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_p2thAddString);
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, ENDING_DIC_V);//[V]�����߰�
					AddHTokenEnding(pNode, ENDING_DIC_AFF);//[AFF]�����߰�
					AddHTokenEnding(pNode, ENDING_DIC_CON);//[CON]�����߰�
				}
				else
				{
					CHAR_DELETE(m_pAddTwoString);
					m_pAddTwoString = AddTwoStr(m_pStemDicWord, "��");
					bFindStem = IsExistStemDic(m_pAddTwoString, m_stemFeature01);
					LoggingFunction(pNode, "IMR4_2_00006+ ");
					if(bFindStem == true)
					{//4n : (S(len~n+1)+������)�� stem ������ �ִ°�? IMR4_2_00006
						LoggingFunction(pNode, "IMR4_2_00007+ ");
						if (m_stemFeature01[STEM_DIC_UIR] == IS_CEHCK_FEATURE)
						{//5y : (S(len~n+1)+������)�� [Uir]�ΰ�? IMR4_2_00007
							LoggingFunction(pNode, "IMR4_2_00008+ ");
							m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
							m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
							//6y : stem = S(len~n+1)+������, ending=���+[V][Aff][Con]+NewBuf()+OldBuf() IMR4_2_00008
							InsertStemString(pNode, m_pAddTwoString);
							SaveHTokenStem(pNode);
							InsertEndingString(pNode, m_p2thAddString);
							SaveHTokenEnding(pNode);
							AddHTokenEnding(pNode, ENDING_DIC_V);//[V]�����߰�
							AddHTokenEnding(pNode, ENDING_DIC_AFF);//[AFF]�����߰�
							AddHTokenEnding(pNode, ENDING_DIC_CON);//[CON]�����߰�
						}
						else
						{
							LoggingFunction(pNode, "IMR4_2_00009+ ");
							//6n : S(n) = [mask] -> MAR1�� ��� IMR4_2_00009
							SetOneWordMarsk();

							GoMAR1_2(pNode, nStart, mStart, lStart);
						}
					}
					else
					{
						LoggingFunction(pNode, "IMR4_2_00010+ ");
						//5n : S(n) = [mask] -> MAR1�� ��� IMR4_2_00010
						SetOneWordMarsk();

						GoMAR1_2(pNode, nStart, mStart, lStart);
					}
				}
			}
			else
			{
				LoggingFunction(pNode, "IMR4_2_00011+ ");
				//3n : S(n) = [mask] -> MAR1�� ��� IMR4_2_00011
				SetOneWordMarsk();

				GoMAR1_2(pNode, nStart, mStart, lStart);
			}
		}
		else
		{
			m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
			m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
			m_pAddTwoString = AddTwoStr(m_pMinusTwoString, "��");
			bool bFindStem = IsExistStemDic(m_pAddTwoString, m_stemFeature01);
			LoggingFunction(pNode, "IMR4_2_00012+ ");
			if(bFindStem == true)
			{//2n : (S(len~n+1)+������)�� stem ������ �ִ°�? IMR4_2_00012
				LoggingFunction(pNode, "IMR4_2_00013+ ");
				if (m_stemFeature01[STEM_DIC_UIR] == IS_CEHCK_FEATURE)
				{//3y : (S(len~n+1)+������)�� [Uir]�ΰ�? IMR4_2_00013
					LoggingFunction(pNode, "IMR4_2_00014+ ");
					m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					//4y : stem = S(len~2)+������, ending=��� +[V][Aff][Con]+NewBuf()+OldBuf() IMR4_2_00014
					InsertStemString(pNode, m_pAddTwoString);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_p2thAddString);
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, ENDING_DIC_V);//[V]�����߰�
					AddHTokenEnding(pNode, ENDING_DIC_AFF);//[AFF]�����߰�
					AddHTokenEnding(pNode, ENDING_DIC_CON);//[CON]�����߰�
				}
				else
				{
					LoggingFunction(pNode, "IMR4_2_00015+ ");
					//4n : S(n) = [mask] -> MAR1�� ��� IMR4_2_00015
					SetOneWordMarsk();

					GoMAR1_2(pNode, nStart, mStart, lStart);
				}
			}
			else
			{
				bFindStem = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
				LoggingFunction(pNode, "IMR4_2_00016+ ");
				if(bFindStem == true)
				{//3n : S(len~n+1)�� stem ������ �ִ°�? IMR4_2_00016
					LoggingFunction(pNode, "IMR4_2_00017+ ");
					if (m_stemFeature01[STEM_DIC_UIR] == IS_CEHCK_FEATURE)
					{//4y : S(len~n+1)�� [Ruir]�ΰ�? IMR4_2_00017
						LoggingFunction(pNode, "IMR4_2_00018+ ");
						m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
						//5y : stem = S(len~n+1), ending=���+[V][Aff][Con]+NewBuf()+OldBuf() IMR4_2_00018
						InsertStemString(pNode, m_pAddTwoString);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, m_p2thAddString);
						SaveHTokenEnding(pNode);
						AddHTokenEnding(pNode, ENDING_DIC_V);//[V]�����߰�
						AddHTokenEnding(pNode, ENDING_DIC_AFF);//[AFF]�����߰�
						AddHTokenEnding(pNode, ENDING_DIC_CON);//[CON]�����߰�
					}
					else
					{	 
						LoggingFunction(pNode, "IMR4_2_00019+ ");
						//5n : S(n) = [mask] -> MAR1�� ��� IMR4_2_00019
						SetOneWordMarsk();

						GoMAR1_2(pNode, nStart, mStart, lStart);
					}
				}
				else
				{
					CHAR_DELETE(m_pStemDicWord);
					ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA13);//C(n) = ������
					m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
					LoggingFunction(pNode, "IMR4_2_00020+ ");
					if(IsExistStemDic(m_pStemDicWord))
					{//4n : C(n) = �� �� S(len~n)�� stem ������ �ִ°�? IMR4_2_00020
						LoggingFunction(pNode, "IMR4_2_00021+ ");
						m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
						//5y : stem = S(len~n), ending = NewBuf()+OldBuf() IMR4_2_00021
						InsertStemString(pNode, m_pStemDicWord);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, m_p1thAddString);
						SaveHTokenEnding(pNode);
					}
					else
					{
						CHAR_DELETE(m_pStemDicWord);
						ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA05);//C(n) = ������
						m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
						LoggingFunction(pNode, "IMR4_2_00022+ ");
						if (IsExistStemDic(m_pStemDicWord))
						{//5n : C(n) = ������ �� S(len~n)�� stem ������ �ִ°�? IMR4_2_00022
							LoggingFunction(pNode, "IMR4_2_00023+ ");
							m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
							//6y : stem = S(len~n), ending = NewBuf()+OldBuf() IMR4_2_00023
							InsertStemString(pNode, m_pStemDicWord);
							SaveHTokenStem(pNode);
							InsertEndingString(pNode, m_p1thAddString);
							SaveHTokenEnding(pNode);
						}
						else
						{
							CHAR_DELETE(m_pNthOneChar);
							CHAR_DELETE(m_pStemDicWord);
							CHAR_DELETE(m_pAddTwoString);
							m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
							m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)�� ����
							m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
							m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+2)
							bFindStem = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
							LoggingFunction(pNode, "IMR4_2_00025+ ");
							if(bFindStem == true && (m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE) && strcmp(m_pNthOneChar,"��") == 0)
							{//6n : S(len~n+2) == [N]||[Amb] && S(n+1) == �������ΰ�? IMR4_2_00025
								LoggingFunction(pNode, "IMR4_2_00025+ ");
								m_pAddTwoString = AddTwoStr(m_pStemDicWord,"����");
								m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
								//7y : stem = S(len~n+2)+����+[V][Sta]-[N], ending = NewBuf()+OldBuf() IMR4_2_00025
								InsertStemString(pNode, m_pAddTwoString);
								SaveHTokenStem(pNode);
								AddHTokenStem(pNode, STEM_DIC_V);//[V]�����߰�
								AddHTokenStem(pNode, STEM_DIC_STA);//[STA]�����߰�
								DeleteHTokenStem(pNode, STEM_DIC_N);//[N]��������
								InsertEndingString(pNode, m_p1thAddString);
								SaveHTokenEnding(pNode);
							}
							else
							{
								LoggingFunction(pNode, "IMR4_2_00026+ ");
								//7n : C(n) = Null �� S(n) = [mask] -> MAR1�� ��� IMR4_2_00026
								ReplaceHangulJongsungNull(pNode, nStart);//��������
								SetOneWordMarsk();

								GoMAR1_2(pNode, nStart, mStart, lStart);
							}
						}
					}
				}
			}
		}
	}
	else
	{      
		LoggingFunction(pNode, "IMR4_2_00027+ ");
		if(CompareVariousTable04(m_pNthOneChar, 3, false) == true)
		{//1n : S(n)�� Table04[3]�ΰ�? IMR4_2_00027
			m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
			m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
			m_pAddTwoString = AddTwoStr(m_pStemDicWord, Table04[m_nTableIndex].strOneWord);//S(len~n+1)+Table04[0]
			LoggingFunction(pNode, "IMR4_2_00028+ ");
			if (IsExistStemDic(m_pAddTwoString))
			{//2y : S(len~n+1)+Table04[0]�� stem ������ �ִ°�? IMR4_2_00028	
				LoggingFunction(pNode, "IMR4_2_00029+ ");
				if(m_pOldBuffer == NULL || strcmp(m_pOldBuffer,"") == 0)
 				{//3y : stem = S(len~n+1)+Table03[0]�� OldBuf() == Null�ΰ�? IMR4_2_00029
					LoggingFunction(pNode, "IMR4_2_00030+ ");
					m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					//4y : ending = ���+[V][Aff][Con] +NewBuf() IMR4_2_00030
					InsertStemString(pNode, m_pAddTwoString);
					SaveHTokenStem(pNode);
					CHAR_DELETE(m_pAddTwoString);
					m_pAddTwoString = AddTwoStr(m_pStemDicWord, Table03[m_nTableIndex].strOneWord);//S(len~n+1)+Table03[0]
					InsertStemString(pNode, m_pAddTwoString);
					InsertEndingString(pNode, m_p2thAddString);
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, ENDING_DIC_V);//[V]�����߰�
					AddHTokenEnding(pNode, ENDING_DIC_AFF);//[AFF]�����߰�
					AddHTokenEnding(pNode, ENDING_DIC_CON);//[CON]�����߰�
					
				}
				else
				{
					LoggingFunction(pNode, "IMR4_2_00031+ ");
					m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					//4n : stem = S(len~n+1)+Table04[0], ending =���+[V][Aff][Con]+NewBuf()+OldBuf() IMR4_2_00031
					InsertStemString(pNode, m_pAddTwoString);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_p2thAddString);
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, ENDING_DIC_V);//[V]�����߰�
					AddHTokenEnding(pNode, ENDING_DIC_AFF);//[AFF]�����߰�
					AddHTokenEnding(pNode, ENDING_DIC_CON);//[CON]�����߰�
				}
			}
			else
			{
				CHAR_DELETE(m_pStemDicWord);
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
				bool bFindStem = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
				LoggingFunction(pNode, "IMR4_2_00032+ ");
				if(bFindStem == true)
				{//3n : S(len~n)�� stem�� �ִ°�? IMR4_2_00032
					LoggingFunction(pNode, "IMR4_2_00033+ ");
					if(m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_ADV] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_IC] == IS_CEHCK_FEATURE)
					{//4y : S(len~n) == [N] || [ADV] || [IC]�ΰ�? IMR4_2_00033
						LoggingFunction(pNode, "IMR4_2_00034+ ");
						//5y : S(n) = [mask]  -> MAR1�� ��� IMR4_2_00034
						SetOneWordMarsk();

						GoMAR1_2(pNode, nStart, mStart, lStart);
					}
					else
					{
						LoggingFunction(pNode, "IMR4_2_00035+ ");
						m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
						//5n : Stem = S(len~n), Ending = NewBuf()+OldBuf() IMR4_2_00035
						InsertStemString(pNode, m_pStemDicWord);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, m_p1thAddString);
						SaveHTokenEnding(pNode);
					}
				}
				else
				{
					LoggingFunction(pNode, "IMR4_2_00036+ ");
					//4n : S(n) = [mask]  -> MAR1�� ��� IMR4_2_00036
					SetOneWordMarsk();

					GoMAR1_2(pNode, nStart, mStart, lStart);
				}
			}
		}
		else
		{	 
			LoggingFunction(pNode, "IMR4_2_00037+ ");
			if(CompareVariousTable04(m_pNthOneChar, 1, false) == true || CompareVariousTable04(m_pNthOneChar, 2, false) == true)
			{//2n : S(n)�� Table04[1] || Table04[2] �ΰ�? IMR4_2_00037
				m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
				m_pAddTwoString = AddTwoStr(m_pStemDicWord, Table04[m_nTableIndex].strOneWord);//S(len~n+1)+Table04[0]
				LoggingFunction(pNode, "IMR4_2_00038+ ");
				if(IsExistStemDic(m_pAddTwoString))
				{//3y : S(len~n+1)+Table04[0]�� stem ������ �ִ°�? IMR4_2_00038
					LoggingFunction(pNode, "IMR4_2_00039+ ");
					//4y : stem = S(len~n+1)+Table04[0]
					//�� Table04[1] : ending = NewBuf()+OldBuf()
					//�� Table04[2] : ending = ��+NewBuf()+OldBuf() IMR4_2_00039
					InsertStemString(pNode, m_pAddTwoString);
					SaveHTokenStem(pNode);
					if (m_nTableString == 2)//Table04[1] : ending
					{
						m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
						InsertEndingString(pNode, m_p1thAddString);
						SaveHTokenEnding(pNode);
					}
					else if (m_nTableString == 3)//Table04[2] : ending
					{
						m_p1thAddString = AddTwoStr("��", m_pNewBuffer);
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
						InsertEndingString(pNode, m_p2thAddString);
						SaveHTokenEnding(pNode);
					}
				}
				else
				{
					CHAR_DELETE(m_pStemDicWord);
					m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
					LoggingFunction(pNode, "IMR4_2_00040+ ");
					if(IsExistStemDic(m_pStemDicWord))
					{//4n : S(len~n)�� stem ������ �ִ°�? IMR4_2_00040
						LoggingFunction(pNode, "IMR4_2_00041+ ");
						m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
						//5y : stem = S(len~n), Ending = NewBuf()+OldBuf() IMR4_2_00041
						InsertStemString(pNode, m_pStemDicWord);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, m_p1thAddString);
						SaveHTokenEnding(pNode);
					}
					else
					{
						LoggingFunction(pNode, "IMR4_2_00042+ ");
						//5n : S(n) = [mask] -> MAR1�� ��� IMR4_2_00042
						SetOneWordMarsk();

						GoMAR1_2(pNode, nStart, mStart, lStart);
					}
				}
			}
			else
			{	
				m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
				bool bFindStem = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
				LoggingFunction(pNode, "IMR4_2_00043+ ");
				if(strcmp(m_pNthOneChar, "��") == 0 && bFindStem == true && (m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE))
				{//3n : S(n) == ���䡱 && S(len~n+1) == [N]||[Amb]�ΰ�? IMR4_2_00043
					LoggingFunction(pNode, "IMR4_2_00044+ ");
					m_pAddTwoString = AddTwoStr(m_pStemDicWord,"��");
					m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
					//4y : stem = S(len~n+1)+��+[V][Sta]-[N], Ending = NewBuf()+OldBuf() IMR4_2_00044
					InsertStemString(pNode, m_pStemDicWord);
					SaveHTokenStem(pNode);
					InsertStemString(pNode, m_pAddTwoString);
					AddHTokenStem(pNode, STEM_DIC_V);//[V]�����߰�
					AddHTokenStem(pNode, STEM_DIC_STA);//[STA]�����߰�
					DeleteHTokenStem(pNode, STEM_DIC_N);//[N]��������
					InsertEndingString(pNode, m_p1thAddString);
					SaveHTokenEnding(pNode);
				}
				else
				{
					CHAR_DELETE(m_pStemDicWord);
					m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
					m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)�� ����
					m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
					m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+2)
					bool bFindStem = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
					LoggingFunction(pNode, "IMR4_2_00045+ ");
					if(strcmp(m_pNthOneChar, "��") == 0 && strcmp(m_pOneNPluseNumChar, "��") == 0 && bFindStem == true && (m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE))
					{//4n : S(n) == ������ && S(n+1) == ������ && S(len~n+2) == [N]||[Amb]�ΰ�? IMR4_2_00045
						LoggingFunction(pNode, "IMR4_2_00046+ ");
						m_pAddTwoString = AddTwoStr(m_pStemDicWord, "����");
						m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
						//5y : stem = S(len~n+2)+����+[V][Sta]-[N], Ending = NewBuf()+OldBuf() IMR4_2_00046
						InsertStemString(pNode, m_pStemDicWord);
						SaveHTokenStem(pNode);
						InsertStemString(pNode, m_pAddTwoString);
						AddHTokenStem(pNode, STEM_DIC_V);//[V]�����߰�
						AddHTokenStem(pNode, STEM_DIC_STA);//[STA]�����߰�
						DeleteHTokenStem(pNode, STEM_DIC_N);//[N]��������
						InsertEndingString(pNode, m_p1thAddString);
						SaveHTokenEnding(pNode);
					}
					else
					{
						LoggingFunction(pNode, "IMR4_2_00047+ ");
						if (strcmp(m_pNthOneChar, "��") == 0 && strcmp(m_pOneNPluseNumChar, "��") == 0 && bFindStem == true && (m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE))
						{//5n : S(n) == ���족 && S(n+1) == ���١� && S(len~n+2) == [N]||[Amb]�ΰ�? IMR4_2_00047
							LoggingFunction(pNode, "IMR4_2_00048+ "); 
							m_pAddTwoString = AddTwoStr(m_pStemDicWord, "��");
							m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
							//6y : stem = S(len~n+2)+��+[V][Sta]-[N], Ending = NewBuf()+OldBuf() IMR4_2_00048
							InsertStemString(pNode, m_pStemDicWord);
							SaveHTokenStem(pNode);
							InsertStemString(pNode, m_pAddTwoString);
							AddHTokenStem(pNode, STEM_DIC_V);//[V]�����߰�
							AddHTokenStem(pNode, STEM_DIC_STA);//[STA]�����߰�
							DeleteHTokenStem(pNode, STEM_DIC_N);//[N]��������
							InsertEndingString(pNode, m_p1thAddString);
							SaveHTokenEnding(pNode);
						}
						else
						{
							CHAR_DELETE(m_pStemDicWord);
							m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
							m_pFormLenToNWord = GetUnRegisterMiddleCountChar(pNode->pHToken->strSource, m_nPluseSyllable, m_nPluseSyllable + 1, CALCULATE_STOP, CALCULATE_STOP);//S(n+1~n+2)
							m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 3, CALCULATE_MINUS);//n+3
							m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+3)
							bool bFindStem = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
							LoggingFunction(pNode, "IMR4_2_00049+ ");
							if (strcmp(m_pNthOneChar, "��") == 0 && strcmp(m_pFormLenToNWord, "����") == 0 && bFindStem == true && (m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE))
							{//6n : S(n) == ���족 && S(n+1~n+2) == �������� && S(len~n+3) == [N]||[Amb]�ΰ�? IMR4_2_00049
								LoggingFunction(pNode, "IMR4_2_00050+ ");
								m_pAddTwoString = AddTwoStr(m_pStemDicWord, "����");
								m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
								//7y : stem = S(len~n+3)+����+[V][Sta]-[N], Ending = NewBuf()+OldBuf() IMR4_2_00050
								InsertStemString(pNode, m_pStemDicWord);
								SaveHTokenStem(pNode);
								InsertStemString(pNode, m_pAddTwoString);
								AddHTokenStem(pNode, STEM_DIC_V);//[V]�����߰�
								AddHTokenStem(pNode, STEM_DIC_STA);//[STA]�����߰�
								DeleteHTokenStem(pNode, STEM_DIC_N);//[N]��������
								InsertEndingString(pNode, m_p1thAddString);
								SaveHTokenEnding(pNode);
							}
							else
							{
								LoggingFunction(pNode, "IMR4_2_00051+ ");
								//7n : S(n) = [mask]->MAR1�� ��� IMR4_2_00051

								GoMAR1_2(pNode, nStart, mStart, lStart);
							}
						}
					}
				}
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////////////////
// �̵�Ͼ� �м� �˰���
// 2002.6.9
////////////////////////////////////////////////////////////////////////////////////// 
void CNAHangulAnaly::UnRegisterWord(H_NODE *pNode)
{
	AllInitMemberVariable();
	InitBuffer();
	GoFNM1(pNode);
}

char* CNAHangulAnaly::GetUnRegisterOneChar(char *pString, int nCount)
{
	char* pReturnOneChar = NULL;
	int nThisPos = 0;
	nThisPos = nCount;
	int nTotalCount = 0;

	nTotalCount = strlen(pString);

	if (nThisPos+1 < nTotalCount)
	{
		if (pString[nThisPos] & 0x80)
		{
			if (pString[nThisPos+1] & 0x80)
			{
				pReturnOneChar = new char[3];
				memset(pReturnOneChar, 0x00, 3);
				pReturnOneChar[0] = pString[nThisPos];
				pReturnOneChar[1] = pString[nThisPos+1];
			}
			else
			{
				pReturnOneChar = new char[3];
				memset(pReturnOneChar, 0x00, 3);
				strcpy(pReturnOneChar, "");
			}
		}
		else
		{
			pReturnOneChar = new char[2];
			memset(pReturnOneChar, 0x00, 2);
			pReturnOneChar[0] = pString[nThisPos];
		}
	}
	else
	{
		pReturnOneChar = new char[2];
		memset(pReturnOneChar, 0x00, 2);
		pReturnOneChar[0] = pString[nThisPos];
	}
	return pReturnOneChar;
}

void CNAHangulAnaly::UnRegisteMoveRight(char *pString, int &nCount)
{
	int nTotalCount = 0;
	nTotalCount = strlen(pString);
	if (pString[nCount] & 0x80)
	{
		if (nCount+1 < nTotalCount)
		{
			if (pString[nCount+1] & 0x80)
				nCount += 2;
			else
				nCount += 1;
		}
		else if (nCount+1 == (nTotalCount - 1))
			nCount = nTotalCount;
		else
			nCount += 1;
	}
	else
	{
		if (nCount+1 < nTotalCount)
			nCount += 1;
		else if (nCount+1 == (nTotalCount - 1))
			nCount = nTotalCount;
		else
			nCount += 1;
	}
}

int CNAHangulAnaly::GetUnRegisterMoveRightCount(char *pString, int nCount)
{
	int nReturnNumber = 0;
	int nTotalCount = 0;
	nTotalCount = strlen(pString);
	if (pString[nCount] & 0x80)
	{
		if (nCount+1 < nTotalCount)
		{
			if (pString[nCount+1] & 0x80)
			{
				nReturnNumber = nCount+2;
			}
			else
			{
				nReturnNumber = nCount+1;
			}
		}
	}
	else
	{
		if (nCount+1 < nTotalCount)
			nReturnNumber = nCount+1;
	}
	return nReturnNumber;
}

char* CNAHangulAnaly::GetUnRegisterMiddleCountChar(char *pString, int nStartPos, int nEndPos, int nStartCase, int nEndCase)
{
	char* pReturnMiddleString = NULL;
	int nLen = 0;
	int nCount = 0;
	int nTotalCount = 0;
	nTotalCount = strlen(pString);
	switch (nStartCase)
	{
	case CALCULATE_PLUS:
		if (pString[nStartPos] & 0x80)
		{
			if (nStartPos+1 < nTotalCount)
			{
				if (pString[nStartPos+1] & 0x80)
					nStartPos += 2;
				else
					nStartPos += 1;
			}
		}
		break;
	case CALCULATE_MINUS:
		if (pString[nStartPos] & 0x80)
		{
			if (pString[nStartPos - 1] & 0x80)
			{
				if (pString[nStartPos - 2] & 0x80)
					nStartPos -= 2;
				else
					nStartPos -= 1;
			}
			else
				nStartPos -= 1;
		}
		else
		{
			if (pString[nStartPos - 1] & 0x80)
				nStartPos -= 2;
			else
				nStartPos -= 1;
		}
		break;
	case CALCULATE_STOP:
		break;
	}
	switch (nEndCase)
	{
	case CALCULATE_PLUS:
		nEndPos += 1;
		break;
	case CALCULATE_MINUS:
		if (pString[nEndPos] & 0x80)
		{
			if (pString[nEndPos - 1] & 0x80)
				nEndPos -= 2;
			else
				nEndPos -= 1;
		}
		else
			nEndPos -= 1;
		break;
	case CALCULATE_STOP:
		if (pString[nEndPos] & 0x80)
		{
		}
		else
			nEndPos -= 1;
		break;
	}

	if (nStartPos == nEndPos)
	{
		if (pString[nStartPos] & 0x80)
		{
			if (pString[nStartPos+1] & 0x80)
			{
				pReturnMiddleString = new char[3];
				memset(pReturnMiddleString, 0x00, 3);
				pReturnMiddleString[0] = pString[nStartPos];
				pReturnMiddleString[1] = pString[nStartPos+1];
			}
			else
			{
				pReturnMiddleString = new char[2];
				memset(pReturnMiddleString, 0x00, 2);
				pReturnMiddleString[0] = pString[nStartPos];
			}
		}
		else
		{
			pReturnMiddleString = new char[2];
			memset(pReturnMiddleString, 0x00, 2);
			pReturnMiddleString[0] = pString[nStartPos];
		}
	}
	else
	{
		nLen = nEndPos - nStartPos+1;
		if (nLen > 0)
		{
			nCount = 0;
			pReturnMiddleString = new char[nLen+1];
			memset(pReturnMiddleString, 0x00, nLen+1);
			for (nStartPos; nStartPos <= nEndPos; nStartPos++)
			{
				pReturnMiddleString[nCount] = pString[nStartPos];
				nCount += 1;
			}
		}
		else
		{
			if ((unsigned)nEndPos+1 <= strlen(pString))
			{
				pReturnMiddleString = new char[2];
				memset(pReturnMiddleString, 0x00, 2);
				pReturnMiddleString[0] = pString[nEndPos+1];
			}
			else
			{
				pReturnMiddleString = new char[2];
				memset(pReturnMiddleString, 0x00, 2);
				strcpy(pReturnMiddleString, "");
			}
		}
	}
	return pReturnMiddleString;
}

void CNAHangulAnaly::UniAddEndingFeature(H_NODE *pEndingNode, char *pString)
{
	char Feature_End[ENDING_DIC_BASIC_FEATURE_NUMBER];
	char IS_CEHCK_FEATURE = '1';
	memset(Feature_End, 0x00, sizeof(Feature_End));

	if (pString != NULL && strlen(pString) > 0)
	{
		if (IsExistEndingDic(pString) == true)
		{
			IsExistEndingDic(pString, (char *)Feature_End);
			for (int Feature = 0; Feature < ENDING_DIC_BASIC_FEATURE_NUMBER; Feature++)
			{
				if (Feature_End[Feature] == IS_CEHCK_FEATURE)
					pEndingNode->pHToken->subEnding->strFeature[Feature] = '1';
				else
					pEndingNode->pHToken->subEnding->strFeature[Feature] = '0';
			}
		}
	}
}

void CNAHangulAnaly::UniMakeNewEndingFeature(H_NODE *pEndingNode)
{
	CHAR_DELETE(pEndingNode->pHToken->subEnding->strFeature);

	pEndingNode->pHToken->subEnding->strFeature = new char[ENDING_DIC_BASIC_FEATURE_NUMBER];
	memset(pEndingNode->pHToken->subEnding->strFeature, 0x00, ENDING_DIC_BASIC_FEATURE_NUMBER);
}

void CNAHangulAnaly::GoFNM1(H_NODE *pNode)
{
	int nStart = -1;
	char pTm[128];
	char* pTmOri = NULL;
	int nLen = 0;
	int nPos = 0;
	int n = 0;

	memset(pTm, 0x00, 128);

	GoFNM1_1(pNode, nStart);

	if (pNode != NULL && pNode->pHToken != NULL && pNode->pHToken->subStem != NULL)
	{
		if (pNode->pHToken->subStem->strResult != NULL)
		{
			pTmOri = pNode->pHToken->subStem->strResult;
			nLen = strlen(pTmOri);
			for (nPos = nLen-1; nPos >= 0; nPos--)
			{
				if (pTmOri[nPos] >= '0' && pTmOri[nPos] <= '9')
					break;
			}
			if (nPos >= 0)
			{
				for (nPos = nPos+1; nPos<nLen; nPos++)
				{
					pTm[n++] = pTmOri[nPos];
				}
				if (strlen(pTm) >= 2)
				{
					if (strcmp(pTm, "��") == 0 || strcmp(pTm, "��") == 0 ||
						strcmp(pTm, "��") == 0 || strcmp(pTm, "��") == 0 ||
						strcmp(pTm, "��") == 0 || strcmp(pTm, "��") == 0 ||
						strcmp(pTm, "�Ⱓ") == 0 || strcmp(pTm, "�⵵") == 0 ||
						strcmp(pTm, "���") == 0 || strcmp(pTm, "�ð�") == 0 ||
						strcmp(pTm, "����") == 0 || strcmp(pTm, "��") == 0 ||
						strcmp(pTm, "�г�") == 0 || strcmp(pTm, "�ֳ�") == 0 ||
						strcmp(pTm, "�б�") == 0 || strcmp(pTm, "�ְ�") == 0 ||
						strcmp(pTm, "����") == 0 || strcmp(pTm, "����") == 0 ||
						strcmp(pTm, "�а�") == 0 || strcmp(pTm, "��") == 0 ||
						strcmp(pTm, "���") == 0 || strcmp(pTm, "�ϰ�") == 0 ||
						strcmp(pTm, "����") == 0 || strcmp(pTm, "�ϰ�") == 0 ||
						strcmp(pTm, "�а�") == 0 || strcmp(pTm, "�ʰ�") == 0 ||
						strcmp(pTm, "�ð�") == 0 || strcmp(pTm, "�а�") == 0 ||
						strcmp(pTm, "����") == 0 || strcmp(pTm, "����") == 0 ||
						strcmp(pTm, "����") == 0 || strcmp(pTm, "�⸻") == 0 ||
						strcmp(pTm, "����") == 0 || strcmp(pTm, "����") == 0 ||
						strcmp(pTm, "�ָ�") == 0 || strcmp(pTm, "�ù�") == 0 ||
						strcmp(pTm, "�ùݰ�") == 0)
						AddHTokenStem(pNode, S_Tm);
				}
			}
		}
	}
}

void CNAHangulAnaly::GoFNM1_1(H_NODE *pNode, int &nStart)
{
	char* pOriginalString = NULL;
	int nTotalLen = 0;

	pOriginalString = pNode->pHToken->strSource;
	nTotalLen = strlen(pOriginalString);

	if (nStart == -1)
	{
		CHAR_DELETE(m_pOldBuffer);
		nStart = 0;
	}
	else
	{
		UnRegisteMoveRight(pOriginalString, nStart);
	}

	if (nStart >= nTotalLen)
	{
		if (m_pOldBuffer == NULL)
		{
			GoSVA(pNode);
		}
		else
		{
			InsertStemString(pNode, m_pOldBuffer);
			SaveHTokenStem(pNode);
			ClassfyKindChar(pNode, m_pOldBuffer);
			if (pNode->pHToken->subStem->nKindChar == ENGLISH_CHAR)
			{
				AddHTokenStem(pNode, 2);
				AddHTokenStem(pNode, 4);
			}
			if (pNode->pHToken->subStem->nKindChar == NUMBER_CHAR)
			{
				AddHTokenStem(pNode, 2);
				AddHTokenStem(pNode, 4);
				AddHTokenStem(pNode, 12);
			}
		}
	}
	else
	{
		GoFNM1_2(pNode, nStart);
	}
}

void CNAHangulAnaly::GoFNM1_2(H_NODE *pNode, int &nStart)
{
	char* pOriginalString = NULL;

	char* pOneChar = NULL;
	char* pAddTwoString = NULL;
	char* pMiddleString = NULL;

	int nLen = 0;
	int nCount = 0;
	unsigned int nThisPos = 0;
	int nTotalCount = 0;
	int nPos = 0;
	unsigned int nNextPos = 0;
	bool bIsFind = false;


	pOriginalString = pNode->pHToken->strSource;
	nTotalCount = strlen(pOriginalString);

	pOneChar = GetUnRegisterOneChar(pOriginalString, nStart);
	if ((strcmp(pOneChar, "0") >= 0 && strcmp(pOneChar, "9") <= 0) ||
		(strcmp(pOneChar, "a") >= 0 && strcmp(pOneChar, "z") <= 0) ||
		(strcmp(pOneChar, "A") >= 0 && strcmp(pOneChar, "Z") <= 0) ||
		strcmp(pOneChar, "+") == 0 || strcmp(pOneChar, "-") == 0 ||
		strcmp(pOneChar, "*") == 0 || strcmp(pOneChar, "/") == 0 ||
		strcmp(pOneChar, "_") == 0 || strcmp(pOneChar, ",") == 0 ||
		strcmp(pOneChar, ".") == 0 || strcmp(pOneChar, ":") == 0 ||
		strcmp(pOneChar, "<") == 0 || strcmp(pOneChar, ">") == 0 ||
		strcmp(pOneChar, "=") == 0 || strcmp(pOneChar, "%") == 0 ||
		strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "~") == 0)
	{
		pAddTwoString = AddTwoStr(m_pOldBuffer, pOneChar);
		nLen = strlen(pAddTwoString);
		nCount = 0;
		if (nLen > 0)
		{
			CHAR_DELETE(m_pOldBuffer);

			m_pOldBuffer = new char[nLen+1];
			memset(m_pOldBuffer, 0x00, nLen+1);
			strcpy(m_pOldBuffer, pAddTwoString);
		}
		//S(n+1)
		nThisPos = GetUnRegisterMoveRightCount(pOriginalString, nStart);

		if ((int)nThisPos < nStart)
		{
			if (m_pOldBuffer == NULL)
			{
				GoSVA(pNode);
			}
			else
			{
				InsertStemString(pNode, m_pOldBuffer);
				SaveHTokenStem(pNode);
				ClassfyKindChar(pNode, m_pOldBuffer);
				if (pNode->pHToken->subStem->nKindChar == ENGLISH_CHAR)
				{
					AddHTokenStem(pNode, 2);
					AddHTokenStem(pNode, 4);
				}
				if (pNode->pHToken->subStem->nKindChar == NUMBER_CHAR)
				{
					AddHTokenStem(pNode, 2);
					AddHTokenStem(pNode, 4);
					AddHTokenStem(pNode, 12);
				}
			}
		}
		else
		{
			CHAR_DELETE(pOneChar);

			//S(n+1)
			pOneChar = GetUnRegisterOneChar(pOriginalString, nThisPos);
			if (strcmp(pOneChar, "0") >= 0 && strcmp(pOneChar, "9") <= 0 ||
				(strcmp(pOneChar, "a") >= 0 && strcmp(pOneChar, "z") <= 0) ||
				(strcmp(pOneChar, "A") >= 0 && strcmp(pOneChar, "Z") <= 0) ||
				strcmp(pOneChar, "+") == 0 || strcmp(pOneChar, "-") == 0 ||
				strcmp(pOneChar, "*") == 0 || strcmp(pOneChar, "/") == 0 ||
				strcmp(pOneChar, "_") == 0 || strcmp(pOneChar, ",") == 0 ||
				strcmp(pOneChar, ".") == 0 || strcmp(pOneChar, ":") == 0 ||
				strcmp(pOneChar, "<") == 0 || strcmp(pOneChar, ">") == 0 ||
				strcmp(pOneChar, "=") == 0 || strcmp(pOneChar, "%") == 0 ||
				strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "~") == 0)
			{
				/*
				CHAR_DELETE(pOneChar);
				CHAR_DELETE(pAddTwoString);
				CHAR_DELETE(pMiddleString);
				*/
				GoFNM1_1(pNode, nStart);
			}
			else
			{
				GoFNM1_5(pNode, nStart);
			}
		}
	}
	else
	{
		/*
		CHAR_DELETE(pOneChar);
		CHAR_DELETE(pAddTwoString);
		CHAR_DELETE(pMiddleString);
		*/

		GoFNM1_3(pNode, nStart);
	}

	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pMiddleString);
}

void CNAHangulAnaly::GoFNM1_3(H_NODE *pNode, int &nStart)
{
	char* pOriginalString = NULL;

	char* pOneChar = NULL;
	char* pNextOneChar = NULL;
	char* pMiddleString = NULL;

	bool bIsFind = false;
	unsigned int nPos = 0;
	unsigned int nLen = 0;
	unsigned int nCount = 0;
	unsigned int nTotalCount = 0;

	pOriginalString = pNode->pHToken->strSource;
	nTotalCount = strlen(pOriginalString);

	pOneChar = GetUnRegisterOneChar(pOriginalString, nStart);

	//S(n+1)
	nCount = GetUnRegisterMoveRightCount(pOriginalString, nStart);

	//S(n+1)
	pNextOneChar = GetUnRegisterOneChar(pOriginalString, nCount);
	if ((strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 ||
		strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0) &&
		(strcmp(pNextOneChar, "0") >= 0 && strcmp(pNextOneChar, "9") <= 0 ||
		(strcmp(pNextOneChar, "a") >= 0 && strcmp(pNextOneChar, "z") <= 0) ||
			(strcmp(pNextOneChar, "A") >= 0 && strcmp(pNextOneChar, "Z") <= 0)))
	{
		CHAR_DELETE(m_pOldBuffer);

		m_pOldBuffer = new char[3];
		memset(m_pOldBuffer, 0x00, 3);
		strcpy(m_pOldBuffer, pOneChar);

		CHAR_DELETE(pMiddleString);

		GoFNM1_1(pNode, nStart);
	}
	else
	{
		for (nPos = 0; nPos < ALL_TABLE_002; nPos++)
		{
			if (strcmp(pOneChar, Table002[nPos].strOneWord) == 0)
			{
				bIsFind = true;
				break;
			}
		}

		if (bIsFind == false)
		{
			for (nPos = 0; nPos < ALL_TABLE_003; nPos++)
			{
				if (strcmp(pOneChar, Table003[nPos].strOneWord) == 0)
				{
					bIsFind = true;
					break;
				}
			}
		}

		if (bIsFind == true)
		{
			CHAR_DELETE(m_pOldBuffer);

			nLen = strlen(pOneChar);
			nCount = 0;
			m_pOldBuffer = new char[nLen+1];
			memset(m_pOldBuffer, 0x00, nLen+1);
			strcpy(m_pOldBuffer, pOneChar);

			pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, nStart, nTotalCount - 1, CALCULATE_PLUS, CALCULATE_STOP);
			if (IsExistEndingDic(pMiddleString))
			{
				InsertStemString(pNode, m_pOldBuffer);
				SaveHTokenStem(pNode);
				AddHTokenStem(pNode, 4);
				AddHTokenStem(pNode, 2);
				ClassfyKindChar(pNode, m_pOldBuffer);
				InsertEndingString(pNode, pMiddleString);
				SaveHTokenEnding(pNode);
			}
			else
			{
				CHAR_DELETE(pNode->pHToken->strSource);

				nLen = strlen(m_pRealOriginalString);
				pNode->pHToken->strSource = new char[nLen+1];
				memset(pNode->pHToken->strSource, 0x00, nLen+1);
				nCount = 0;
				strcpy(pNode->pHToken->strSource, m_pRealOriginalString);


				CHAR_DELETE(pNode->pHToken->subStem->strResult);
				CHAR_DELETE(pNode->pHToken->subStem->strFeature);
				CHAR_DELETE(pNode->pHToken->subEnding->strResult);
				CHAR_DELETE(pNode->pHToken->subEnding->strFeature);
				CHAR_DELETE(m_pOldBuffer);
				CHAR_DELETE(pOneChar);
				CHAR_DELETE(pMiddleString);

				GoSVA(pNode);
			}
		}
		else
		{
			CHAR_DELETE(pOneChar);
			CHAR_DELETE(pMiddleString);

			GoFNM1_4(pNode, nStart);
		}
	}

	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pNextOneChar);
	CHAR_DELETE(pMiddleString);
}

void CNAHangulAnaly::GoFNM1_4(H_NODE *pNode, int &nStart)
{
	char* pOriginalString = NULL;

	char* pOneChar = NULL;
	char* pMiddleString = NULL;

	unsigned int nCount = 0;
	unsigned int nLen = 0;
	unsigned int nTotalCount = 0;
	unsigned int nPos = 0;
	unsigned int nThisPos = 0;
	bool bIsFind = false;



	pOriginalString = pNode->pHToken->strSource;
	nTotalCount = strlen(pOriginalString);

	pOneChar = GetUnRegisterOneChar(pOriginalString, nStart);

	for (nPos = 0; nPos < ALL_TABLE_004; nPos++)
	{
		if (strcmp(pOneChar, Table004[nPos].strOneWord) == 0)
		{
			bIsFind = true;
			break;
		}
	}

	if (bIsFind == true)
	{
		CHAR_DELETE(m_pOldBuffer);

		nLen = strlen(pOneChar);
		nCount = 0;
		m_pOldBuffer = new char[nLen+1];
		memset(m_pOldBuffer, 0x00, nLen+1);
		strcpy(m_pOldBuffer, pOneChar);
		pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, nStart, nTotalCount - 1, CALCULATE_PLUS, CALCULATE_STOP);
		if (IsExistEndingDic(pMiddleString))
		{
			InsertStemString(pNode, m_pOldBuffer);
			SaveHTokenStem(pNode);
			AddHTokenStem(pNode, 4);
			AddHTokenStem(pNode, 2);
			ClassfyKindChar(pNode, m_pOldBuffer);
			InsertEndingString(pNode, pMiddleString);
			SaveHTokenEnding(pNode);
		}
		else if (IsAalphabet(pOneChar) ||
			(strcmp(pOneChar, "0") >= 0 && strcmp(pOneChar, "9") <= 0))
		{
			CHAR_DELETE(m_pOldBuffer);

			m_pOldBuffer = new char[3];
			memset(m_pOldBuffer, 0x00, 3);
			strcpy(m_pOldBuffer, pOneChar);

			GoFNM1_1(pNode, nStart);
		}
		else
		{
			CHAR_DELETE(pNode->pHToken->strSource);

			nLen = strlen(m_pRealOriginalString);
			pNode->pHToken->strSource = new char[nLen+1];
			memset(pNode->pHToken->strSource, 0x00, nLen+1);
			nCount = 0;
			strcpy(pNode->pHToken->strSource, m_pRealOriginalString);

			CHAR_DELETE(pNode->pHToken->subStem->strResult);
			CHAR_DELETE(pNode->pHToken->subStem->strFeature);
			CHAR_DELETE(pNode->pHToken->subEnding->strResult);
			CHAR_DELETE(pNode->pHToken->subEnding->strFeature);
			CHAR_DELETE(m_pOldBuffer);
			CHAR_DELETE(pOneChar);
			CHAR_DELETE(pMiddleString);

			GoUWR(pNode);
		}
	}
	else
	{
		//S(n+1)
		nThisPos = GetUnRegisterMoveRightCount(pOriginalString, nStart);

		CHAR_DELETE(pOneChar);

		//S(n+1)
		pOneChar = GetUnRegisterOneChar(pOriginalString, nThisPos);
		if (strcmp(pOneChar, "0") >= 0 && strcmp(pOneChar, "9") <= 0 ||
			(strcmp(pOneChar, "a") >= 0 && strcmp(pOneChar, "z") <= 0) ||
			(strcmp(pOneChar, "A") >= 0 && strcmp(pOneChar, "Z") <= 0) ||
			strcmp(pOneChar, "+") == 0 || strcmp(pOneChar, "-") == 0 ||
			strcmp(pOneChar, "*") == 0 || strcmp(pOneChar, "/") == 0 ||
			strcmp(pOneChar, "_") == 0 || strcmp(pOneChar, ",") == 0 ||
			strcmp(pOneChar, ".") == 0 || strcmp(pOneChar, ":") == 0 ||
			strcmp(pOneChar, "<") == 0 || strcmp(pOneChar, ">") == 0 ||
			strcmp(pOneChar, "=") == 0 || strcmp(pOneChar, "��") == 0 ||
			strcmp(pOneChar, "~") == 0 || strcmp(pOneChar, "%") == 0)
		{
			CHAR_DELETE(pOneChar);

			pOneChar = GetUnRegisterOneChar(pOriginalString, nStart);

			CHAR_DELETE(m_pOldBuffer);

			m_pOldBuffer = new char[3];
			memset(m_pOldBuffer, 0x00, 3);
			strcpy(m_pOldBuffer, pOneChar);

			CHAR_DELETE(pMiddleString);

			GoFNM1_1(pNode, nStart);
		}
		else
		{
			CHAR_DELETE(pOneChar);
			CHAR_DELETE(pMiddleString);

			GoSVA(pNode);
		}
	}

	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pMiddleString);
}

void CNAHangulAnaly::GoFNM1_5(H_NODE *pNode, int &nStart)
{

	char* pOriginalString = NULL;

	char* pMiddleString = NULL;
	char* pMiddleString1 = NULL;
	char* pNextMiddleString1 = NULL;
	char* pNextMiddleString2 = NULL;
	char* pNextMiddleString3 = NULL;
	char* pNextMiddleString4 = NULL;
	char* pNextMiddleString5 = NULL;
	char* pThisOneChar = NULL;
	char* pNextOneChar = NULL;
	char* pNextOneChar2 = NULL;
	char* pNextOneChar3 = NULL;
	char* pNextOneChar4 = NULL;
	char* pLastOneChar = NULL;
	char* pChoJungChar = NULL;
	char* pChoJungChar2 = NULL;
	char* pChoJungChar3 = NULL;
	char* pJongChar = NULL;
	char* pJongChar2 = NULL;
	char* pJongChar3 = NULL;
	char* pPrevOneChar = NULL;
	char* pAddTwoString = NULL;
	char* pAddTwoString2 = NULL;
	char* pAddTwoString3 = NULL;
	char* pMiddleString12 = NULL;
	char* pMiddleString23 = NULL;
	char* pBuffer = NULL;
	char* pEndTemp = NULL;
	char* pTempStr = NULL;

	bool isbFind005 = false;
	bool isbFind006 = false;
	bool isbFind007 = false;
	bool isbFind008 = false;
	bool isbFind009 = false;



	int nTotalCount = 0;
	int nLen = 0;
	int nMinusLen = 0;

	int nPos = 0;
	int nPos1 = 0;
	int nNextPos1 = 0;
	int nNextPos2 = 0;
	int nNextPos3 = 0;
	int nNextPos4 = 0;
	int nNextPos5 = 0;
	int nThisPos = 0;

	bool bEPol = false;

	pOriginalString = pNode->pHToken->strSource;
	nTotalCount = strlen(pOriginalString);

	//S(n+1)
	nNextPos1 = GetUnRegisterMoveRightCount(pOriginalString, nStart);
	//S(n+2)
	nNextPos2 = GetUnRegisterMoveRightCount(pOriginalString, nNextPos1);
	//S(n+3)
	nNextPos3 = GetUnRegisterMoveRightCount(pOriginalString, nNextPos2);
	//S(n+4)
	nNextPos4 = GetUnRegisterMoveRightCount(pOriginalString, nNextPos3);
	//S(n+5)
	nNextPos5 = GetUnRegisterMoveRightCount(pOriginalString, nNextPos4);


	//S(n+1~len)
	pNextMiddleString1 = GetUnRegisterMiddleCountChar(pOriginalString, nNextPos1, nTotalCount, CALCULATE_STOP, CALCULATE_STOP);

	if (pOriginalString[nTotalCount - 1] & 0x80)
	{
		nThisPos = nTotalCount - 2;
	}
	else
	{
		nThisPos = nTotalCount - 1;
	}
	//S(len)
	pLastOneChar = GetUnRegisterOneChar(pOriginalString, nThisPos);

	pNextOneChar = GetUnRegisterOneChar(pOriginalString, nNextPos1);
	pNextOneChar2 = GetUnRegisterOneChar(pOriginalString, nNextPos2);
	pChoJungChar = GetChojungJongsungChar(pNextOneChar, CHAR_CHOJUNGSUNG);
	pJongChar = GetChojungJongsungChar(pNextOneChar, CHAR_JONGSUNG);

	pThisOneChar = GetUnRegisterOneChar(pOriginalString, nStart);
	if (nStart > 0)
	{
		if (pOriginalString[nStart - 1] & 0x80)
		{
			pPrevOneChar = GetUnRegisterOneChar(pOriginalString, nStart - 2);
		}
		else
		{
			pPrevOneChar = GetUnRegisterOneChar(pOriginalString, nStart - 1);
		}
	}
	else
	{
		if (pOriginalString[nTotalCount - 2] & 0x80)
		{
			pPrevOneChar = GetUnRegisterOneChar(pOriginalString, nTotalCount - 2);
		}
		else
		{
			pPrevOneChar = GetUnRegisterOneChar(pOriginalString, nStart - 1);
		}
	}

	if ((unsigned)nNextPos2 == strlen(pOriginalString))
	{
		nNextPos2 -= 1;
	}
	if (nNextPos2 >= nNextPos1)
	{
		pNextMiddleString2 = GetUnRegisterMiddleCountChar(pOriginalString, nNextPos1, nNextPos2+1, CALCULATE_STOP, CALCULATE_STOP);
	}

	if ((unsigned)nNextPos3 == strlen(pOriginalString))
	{
		nNextPos3 -= 1;
	}
	if (nNextPos3 >= nNextPos1)
	{
		pNextMiddleString3 = GetUnRegisterMiddleCountChar(pOriginalString, nNextPos1, nNextPos3+1, CALCULATE_STOP, CALCULATE_STOP);
	}

	if ((unsigned)nNextPos4 == strlen(pOriginalString))
	{
		nNextPos4 -= 1;
	}

	if (nNextPos4 >= nNextPos1)
	{
		pNextMiddleString4 = GetUnRegisterMiddleCountChar(pOriginalString, nNextPos1, nNextPos4+1, CALCULATE_STOP, CALCULATE_STOP);
	}

	if ((unsigned)nNextPos5 == strlen(pOriginalString))
	{
		nNextPos5 -= 1;
	}
	if (nNextPos5 >= nNextPos1)
	{
		pNextMiddleString5 = GetUnRegisterMiddleCountChar(pOriginalString, nNextPos1, nNextPos5+1, CALCULATE_STOP, CALCULATE_STOP);
	}

	//S(n+2~len)
	nLen = nNextPos1;
	nLen += 1;
	if (pOriginalString[nLen] & 0x80)
	{
		nLen += 1;
	}
	pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, nLen/*NextPos1*/, nTotalCount - 1, CALCULATE_STOP, CALCULATE_STOP);
	pAddTwoString = AddTwoStr(pJongChar, pMiddleString);

	//S(n+1~n+2)
	pMiddleString12 = GetUnRegisterMiddleCountChar(pOriginalString, nNextPos1, nNextPos2+1, CALCULATE_STOP, CALCULATE_STOP);
	//S(n+2~n+3)
	pMiddleString23 = GetUnRegisterMiddleCountChar(pOriginalString, nNextPos2, nNextPos3+1, CALCULATE_STOP, CALCULATE_STOP);

	//7.24����
	//3n
	if (IsExistEndingDic(pNextMiddleString1)) // S(n+1~len)�� Ending ������ �ִ°�?
	{
		//4y'
		if (IsExistFeature(pNextMiddleString1, END_DIC, E_Ppo) == true &&
			strcmp(pNextOneChar, "��") != 0)
		{
			InsertStemString(pNode, m_pOldBuffer);
			SaveHTokenStem(pNode);
			AddHTokenStem(pNode, 4);
			AddHTokenStem(pNode, 2);
			ClassfyKindChar(pNode, m_pOldBuffer);

			if (pNode->pHToken->subStem->nKindChar == NUMBER_CHAR_WORD)
			{
				AddHTokenStem(pNode, S_Numb);
			}

			InsertEndingString(pNode, pNextMiddleString1);
			SaveHTokenEnding(pNode);
			AddHTokenEnding(pNode, 26);
		}
		//4n'
		else if (strcmp(pThisOneChar, "0") >= 0 && strcmp(pThisOneChar, "9") <= 0)
		{
			//5y
			if (strcmp(pNextOneChar, "��") == 0 || strcmp(pNextOneChar, "��") == 0 ||
				strcmp(pNextOneChar, "��") == 0 || strcmp(pNextOneChar, "��") == 0 ||
				strcmp(pNextOneChar, "��") == 0 || strcmp(pNextOneChar, "��") == 0 ||
				strcmp(pNextOneChar, "��") == 0 || strcmp(pNextOneChar, "��") == 0 ||
				strcmp(pNextOneChar, "��") == 0 || strcmp(pNextOneChar, "��") == 0 ||
				strcmp(pNextOneChar, "��") == 0 || strcmp(pNextOneChar, "��") == 0 ||
				strcmp(pNextOneChar, "��") == 0 || strcmp(pNextOneChar, "��") == 0)
			{
				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr(m_pOldBuffer, pNextOneChar);
				InsertStemString(pNode, pAddTwoString);
				SaveHTokenStem(pNode);
				AddHTokenStem(pNode, 4);
				AddHTokenStem(pNode, 2);
				AddHTokenStem(pNode, 12);
				AddHTokenStem(pNode, 25);
				// Added by KKH 2003-12-16
				if (pNode->pHToken->subStem->strFeature[S_Amb] == '1')
				{
					pNode->pHToken->subStem->strFeature[S_Amb] = '0';
				}
				// Added by KKH 2003-12-16
				ClassfyKindChar(pNode, pAddTwoString);

				if (strcmp(pNextOneChar, pMiddleString) != 0)
				{
					InsertEndingString(pNode, pMiddleString);
					SaveHTokenEnding(pNode);
				}
			}
			else if ((strcmp(pThisOneChar, "0") == 0 || strcmp(pThisOneChar, "1") == 0 ||
				strcmp(pThisOneChar, "3") == 0 || strcmp(pThisOneChar, "6") == 0 ||
				strcmp(pThisOneChar, "7") == 0 || strcmp(pThisOneChar, "8") == 0) &&
				strcmp(pNextOneChar, "��") == 0)
			{
				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr(m_pOldBuffer, pNextOneChar);
				InsertStemString(pNode, pAddTwoString);
				SaveHTokenStem(pNode);
				AddHTokenStem(pNode, 4);
				AddHTokenStem(pNode, 2);
				AddHTokenStem(pNode, 12);
				AddHTokenStem(pNode, 25);
				ClassfyKindChar(pNode, pAddTwoString);

				if (strcmp(pNextOneChar, pMiddleString) != 0)
				{
					InsertEndingString(pNode, pMiddleString);
					SaveHTokenEnding(pNode);
				}
			}
			else
			{
				InsertStemString(pNode, m_pOldBuffer);
				SaveHTokenStem(pNode);
				AddHTokenStem(pNode, 4);
				AddHTokenStem(pNode, 2);
				AddHTokenStem(pNode, 12);
				ClassfyKindChar(pNode, m_pOldBuffer);

				InsertEndingString(pNode, pNextMiddleString1);
				SaveHTokenEnding(pNode);
			}
		}
		//5n
		else if ((strcmp(pThisOneChar, "%") == 0 || strcmp(pThisOneChar, "$") == 0 ||
			strcmp(pThisOneChar, "\\") == 0) &&
			(strcmp(pPrevOneChar, "0") >= 0 && strcmp(pPrevOneChar, "9") <= 0))
		{
			//6y
			InsertStemString(pNode, m_pOldBuffer);
			SaveHTokenStem(pNode);
			AddHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_Lex);
			AddHTokenStem(pNode, S_Numb);
			ClassfyKindChar(pNode, m_pOldBuffer);
			InsertEndingString(pNode, pNextMiddleString1);
			SaveHTokenEnding(pNode);
		}
		//6n
		else if ((strcmp(pLastOneChar, "0") >= 0 && strcmp(pLastOneChar, "9") <= 0)
			&& IsAalphabet(pThisOneChar))
		{
			//7y
			InsertStemString(pNode, m_pOldBuffer);
			SaveHTokenStem(pNode);
			AddHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_Lex);
			AddHTokenStem(pNode, S_Numb);
			ClassfyKindChar(pNode, m_pOldBuffer);
			InsertEndingString(pNode, pNextMiddleString1);
			SaveHTokenEnding(pNode);
		}
		//7n
		else
		{
			InsertStemString(pNode, m_pOldBuffer);
			SaveHTokenStem(pNode);
			AddHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_Lex);
			ClassfyKindChar(pNode, m_pOldBuffer);

			InsertEndingString(pNode, pNextMiddleString1);
			SaveHTokenEnding(pNode);
		}
	}
	//4n
	else if ((strcmp(pThisOneChar, "0") >= 0 && strcmp(pThisOneChar, "9") <= 0) &&
		(strcmp(pNextMiddleString1, "��") == 0 || strcmp(pNextMiddleString1, "��") == 0))
	{
		CHAR_DELETE(pAddTwoString);

		pAddTwoString = AddTwoStr(m_pOldBuffer, pNextMiddleString1);
		InsertStemString(pNode, pAddTwoString);
		SaveHTokenStem(pNode);
		AddHTokenStem(pNode, 4);
		AddHTokenStem(pNode, 2);
		AddHTokenStem(pNode, 12);
		AddHTokenStem(pNode, 25);
		ClassfyKindChar(pNode, pAddTwoString);
	}
	//5n
	else if ((strcmp(pThisOneChar, "0") >= 0 && strcmp(pThisOneChar, "9") <= 0) &&
		strcmp(pNextOneChar, "��") == 0 && IsExistEndingDic(pMiddleString))
	{
		CHAR_DELETE(pAddTwoString);

		pAddTwoString = AddTwoStr(m_pOldBuffer, pNextOneChar);
		InsertStemString(pNode, pAddTwoString);
		SaveHTokenStem(pNode);
		AddHTokenStem(pNode, S_N);
		AddHTokenStem(pNode, S_Lex);
		AddHTokenStem(pNode, S_Numb);
		AddHTokenStem(pNode, S_Unt);
		ClassfyKindChar(pNode, pAddTwoString);

		InsertEndingString(pNode, pMiddleString);
		SaveHTokenEnding(pNode);
	}
	//5'n
	else if (strcmp(pNextOneChar, "��") == 0 ||
		strcmp(pMiddleString12, "����") == 0 ||
		strcmp(pMiddleString12, "����") == 0 ||
		strcmp(pMiddleString12, "����") == 0)
	{
		InsertStemString(pNode, m_pOldBuffer);
		SaveHTokenStem(pNode);
		AddHTokenStem(pNode, 4);
		AddHTokenStem(pNode, 2);
		ClassfyKindChar(pNode, m_pOldBuffer);
		InsertEndingString(pNode, pNextMiddleString1);

		if (IsExistEndingDic(pNextMiddleString1))
		{
			SaveHTokenEnding(pNode);
		}
		else
		{
			MakeNewEndingFeature(pNode);
			AddEndingFeature(pNode, pNextOneChar);
			AddEndingFeature(pNode, pMiddleString);
			AddEndingFeature(pNode, pMiddleString12);
		}
	}
	//6n
	else if ((isbFind005 = (strcmp(pNextOneChar, "��") == 0 && IsExistEndingDic(pMiddleString))) ||
		(isbFind006 = (strcmp(pChoJungChar, "��") == 0 && IsExistEndingDic(pAddTwoString))) ||
		(isbFind007 = (strcmp(pChoJungChar, "��") == 0 && IsExistEndingDic(pJongChar) &&
			nTotalCount <= nLen && IsExistFeature(pNextOneChar2, STEM_DIC, S_N) &&
			IsExistFeature(pNextOneChar2, STEM_DIC, S_Aff))))
	{
		if (isbFind005 == true)
		{
			pEndTemp = pMiddleString;
		}
		else if (isbFind006 == true)
		{
			pEndTemp = pAddTwoString;
		}
		else
		{
			pEndTemp = pJongChar;
		}

		//7y
		if ((strcmp(pThisOneChar, "0") >= 0 && strcmp(pThisOneChar, "9") <= 0) ||
			((strcmp(pThisOneChar, "%") == 0 || strcmp(pThisOneChar, "$") == 0 ||
				strcmp(pThisOneChar, "\\") == 0)) &&
				(strcmp(pPrevOneChar, "0") >= 0 && strcmp(pPrevOneChar, "9") <= 0))
		{
			InsertStemString(pNode, m_pOldBuffer);
			SaveHTokenStem(pNode);
			AddHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_Lex);
			AddHTokenStem(pNode, S_Numb);
			ClassfyKindChar(pNode, m_pOldBuffer);
		}
		//8n
		else
		{
			InsertStemString(pNode, m_pOldBuffer);
			SaveHTokenStem(pNode);
			AddHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_Lex);
			ClassfyKindChar(pNode, m_pOldBuffer);
		}

		//Ending �߰�
		CHAR_DELETE(pAddTwoString2);
		pAddTwoString2 = AddTwoStr("��", pEndTemp);
		InsertEndingString(pNode, pAddTwoString2);
		SaveHTokenEnding(pNode);
		AddHTokenEnding(pNode, E_Pol);
		AddEndingFeature(pNode, pEndTemp);
	}
	//7n
	else if (strcmp(pNextOneChar, "��") == 0 && IsExistTabel008(pMiddleString23))
	{
		CHAR_DELETE(pAddTwoString);
		CHAR_DELETE(pAddTwoString2);
		CHAR_DELETE(pMiddleString);

		pAddTwoString = AddTwoStr(m_pOldBuffer, pNextOneChar);
		pAddTwoString2 = AddTwoStr(pAddTwoString, pMiddleString23);

		InsertStemString(pNode, pAddTwoString2);
		SaveHTokenStem(pNode);
		AddHTokenStem(pNode, S_N);
		AddHTokenStem(pNode, S_Lex);
		AddHTokenStem(pNode, S_Numb);
		ClassfyKindChar(pNode, pAddTwoString2);

		if (nNextPos4 > 0 && nNextPos4 < nTotalCount)
		{
			pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, nNextPos4, nTotalCount - 1, CALCULATE_STOP, CALCULATE_STOP);

			MakeNewEndingFeature(pNode);
			if (pMiddleString != NULL)
			{
				InsertEndingString(pNode, pMiddleString);
				SaveHTokenEnding(pNode);
			}
		}
	}
	//7'n
	else if (strcmp(pNextOneChar, "��") == 0 && IsExistTabel009(pNextOneChar2))
	{
		CHAR_DELETE(pAddTwoString);
		CHAR_DELETE(pAddTwoString2);
		CHAR_DELETE(pMiddleString);

		pAddTwoString = AddTwoStr(m_pOldBuffer, pNextOneChar);
		pAddTwoString2 = AddTwoStr(pAddTwoString, pNextOneChar2);

		InsertStemString(pNode, pAddTwoString2);
		SaveHTokenStem(pNode);
		AddHTokenStem(pNode, S_N);
		AddHTokenStem(pNode, S_Lex);
		AddHTokenStem(pNode, S_Numb);
		ClassfyKindChar(pNode, pAddTwoString2);

		if (nNextPos3 > 0 && nNextPos3 < nTotalCount)
		{
			pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, nNextPos3, nTotalCount - 1, CALCULATE_STOP, CALCULATE_STOP);

			MakeNewEndingFeature(pNode);
			if (pMiddleString != NULL)
			{
				InsertEndingString(pNode, pMiddleString);
				SaveHTokenEnding(pNode);
			}
		}
	}
	//8'n
	else if ((strcmp(pNextOneChar, "��") == 0 || strcmp(pNextOneChar, "��") == 0) &&
		IsExistEndingDic(pMiddleString))
	{
		//8y
		if ((strcmp(pThisOneChar, "0") >= 0 && strcmp(pThisOneChar, "9") <= 0) ||
			((strcmp(pThisOneChar, "%") == 0 || strcmp(pThisOneChar, "$") == 0 ||
				strcmp(pThisOneChar, "\\") == 0)) &&
				(strcmp(pPrevOneChar, "0") >= 0 && strcmp(pPrevOneChar, "9") <= 0))
		{
			InsertStemString(pNode, m_pOldBuffer);
			SaveHTokenStem(pNode);
			AddHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_Lex);
			AddHTokenStem(pNode, S_Numb);
			ClassfyKindChar(pNode, m_pOldBuffer);
		}
		else
		{
			InsertStemString(pNode, m_pOldBuffer);
			SaveHTokenStem(pNode);
			AddHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_Lex);
			ClassfyKindChar(pNode, m_pOldBuffer);
		}

		//Ending �߰�
		CHAR_DELETE(pAddTwoString2);
		pAddTwoString2 = AddTwoStr("�̾�", pAddTwoString);
		InsertEndingString(pNode, pAddTwoString2);
		SaveHTokenEnding(pNode);
		AddHTokenEnding(pNode, E_Pol);
		AddEndingFeature(pNode, "�̾�");
		AddEndingFeature(pNode, pJongChar);
		AddEndingFeature(pNode, pMiddleString);
	}
	//8n
	else if (IsAalphabet(pThisOneChar))
	{
		//9y
		if (IsExistStemDic(pNextMiddleString1))
		{
			if ((strcmp(pNextOneChar, "��") == 0 || strcmp(pNextOneChar, "��") == 0 || strcmp(pNextOneChar, "��") == 0) &&
				nTotalCount <= nLen)
			{
				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr(m_pOldBuffer, "��");
				InsertStemString(pNode, pAddTwoString);
				SaveHTokenStem(pNode);
				SaveHTokenStemFeature(pNode, m_pOldBuffer);
				DeleteHTokenStem(pNode, 4);//n
				AddHTokenStem(pNode, 7);//v
				ClassfyKindChar(pNode, pAddTwoString);

				InsertEndingString(pNode, pJongChar);
				SaveHTokenEnding(pNode);
			}
			else
			{
				InsertStemString(pNode, m_pOldBuffer);
				SaveHTokenStem(pNode);
				SaveHTokenStemFeature(pNode, m_pOldBuffer);

				m_LinkedList->CreateSubNode(pNode);
				InsertAddSubTail(pNode, m_pOldBuffer);

				AddSubNodeTailFeature(pNode, 4);
				AddSubNodeTailFeature(pNode, 2);

				nLen = strlen(m_pOldBuffer);
				nMinusLen = nTotalCount - nLen;
				CHAR_DELETE(pBuffer);
				pBuffer = new char[nMinusLen+1];
				memset(pBuffer, 0x00, nMinusLen+1);
				nPos = 0;
				for (nLen; nLen<nTotalCount; nLen++)
				{
					pBuffer[nPos] = pOriginalString[nLen];
					nPos += 1;
				}

				CHAR_DELETE(pNode->pHToken->strSource);

				nLen = strlen(pBuffer);
				nPos = 0;
				pNode->pHToken->strSource = new char[nLen+1];
				memset(pNode->pHToken->strSource, 0x00, nLen+1);
				strcpy(pNode->pHToken->strSource, pBuffer);

				CHAR_DELETE(pBuffer);
				CHAR_DELETE(m_pOldBuffer);

				GoUWR(pNode);
			}
		}
		else if (strcmp(pNextOneChar, "��") == 0 || strcmp(pNextOneChar, "��") == 0)
		{
			CHAR_DELETE(pAddTwoString);

			pAddTwoString = AddTwoStr(m_pOldBuffer, pNextOneChar);
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStem(pNode);
			SaveHTokenStemFeature(pNode, m_pOldBuffer);
			DeleteHTokenStem(pNode, 4);//n
			AddHTokenStem(pNode, 7);//v
			ClassfyKindChar(pNode, pAddTwoString);

			InsertEndingString(pNode, pMiddleString);
			SaveHTokenEnding(pNode);
		}
		else if (strcmp(pChoJungChar, "��") == 0 || strcmp(pChoJungChar, "��") == 0)
		{
			CHAR_DELETE(pAddTwoString);

			pAddTwoString = AddTwoStr(m_pOldBuffer, pChoJungChar);
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStem(pNode);
			SaveHTokenStemFeature(pNode, m_pOldBuffer);
			DeleteHTokenStem(pNode, 4);//n
			AddHTokenStem(pNode, 7);//v
			ClassfyKindChar(pNode, pAddTwoString);

			CHAR_DELETE(pAddTwoString);

			pAddTwoString = AddTwoStr(pJongChar, pMiddleString);
			InsertEndingString(pNode, pAddTwoString);
			SaveHTokenEnding(pNode);
			AddEndingFeature(pNode, pJongChar);
			AddEndingFeature(pNode, pMiddleString);
		}
		else
		{
			InsertStemString(pNode, m_pOldBuffer);
			SaveHTokenStem(pNode);
			SaveHTokenStemFeature(pNode, m_pOldBuffer);

			m_LinkedList->CreateSubNode(pNode);
			InsertAddSubTail(pNode, m_pOldBuffer);

			AddSubNodeTailFeature(pNode, 4);
			AddSubNodeTailFeature(pNode, 2);

			nLen = strlen(m_pOldBuffer);
			nMinusLen = nTotalCount - nLen;
			CHAR_DELETE(pBuffer);
			pBuffer = new char[nMinusLen+1];
			memset(pBuffer, 0x00, nMinusLen+1);
			nPos = 0;
			for (nLen; nLen<nTotalCount; nLen++)
			{
				pBuffer[nPos] = pOriginalString[nLen];
				nPos += 1;
			}

			CHAR_DELETE(pNode->pHToken->strSource);

			nLen = strlen(pBuffer);
			nPos = 0;
			pNode->pHToken->strSource = new char[nLen+1];
			memset(pNode->pHToken->strSource, 0x00, nLen+1);
			strcpy(pNode->pHToken->strSource, pBuffer);

			CHAR_DELETE(pBuffer);
			CHAR_DELETE(m_pOldBuffer);


			GoUWR(pNode);
		}
	}
	//9n
	else if (IsExistStemDic(pNextMiddleString1))
	{
		isbFind005 = IsExistTabel005(pNextMiddleString1);
		isbFind006 = IsExistTabel006(pNextMiddleString1);
		isbFind007 = IsExistTabel007(pNextMiddleString1);
		isbFind008 = IsExistTabel008(pNextMiddleString1);
		isbFind009 = IsExistTabel009(pNextMiddleString1);
		if (isbFind005 == true || isbFind006 == true || isbFind007 == true ||
			isbFind008 == true || isbFind009 == true)
		{
			CHAR_DELETE(pAddTwoString);

			pAddTwoString = AddTwoStr(m_pOldBuffer, pNextMiddleString1);
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStem(pNode);
			SaveHTokenStemFeature(pNode, m_pOldBuffer);
			AddHTokenStem(pNode, 4);
			AddHTokenStem(pNode, 2);
			AddHTokenStem(pNode, 12);
			AddHTokenStem(pNode, 25);
			ClassfyKindChar(pNode, pAddTwoString);
		}
		else if (strcmp(pNextOneChar, "��") == 0 || strcmp(pNextOneChar, "��") == 0 ||
			strcmp(pNextOneChar, "��") == 0)
		{
			CHAR_DELETE(pAddTwoString);

			pAddTwoString = AddTwoStr(m_pOldBuffer, "��");
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStem(pNode);
			SaveHTokenStemFeature(pNode, m_pOldBuffer);
			DeleteHTokenStem(pNode, 4);//n
			AddHTokenStem(pNode, 7);//v
			ClassfyKindChar(pNode, pAddTwoString);

			InsertEndingString(pNode, pJongChar);
			SaveHTokenEnding(pNode);
		}
		else if (IsExistTabel009(pNextOneChar) && IsExistEndingDic(pMiddleString) &&
			strcmp(pNextMiddleString1, "����") != 0)
		{
			CHAR_DELETE(pAddTwoString);

			pAddTwoString = AddTwoStr(m_pOldBuffer, pNextOneChar);
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStem(pNode);
			AddHTokenStem(pNode, 4);
			AddHTokenStem(pNode, 2);
			AddHTokenStem(pNode, 12);
			AddHTokenStem(pNode, 25);
			ClassfyKindChar(pNode, pAddTwoString);

			if (strcmp(pNextOneChar, pMiddleString) != 0)
			{
				InsertEndingString(pNode, pMiddleString);
				SaveHTokenEnding(pNode);
			}
		}
		else if (IsExistTabel009(pNextOneChar) && IsExistTabel009(pMiddleString))
		{
			CHAR_DELETE(pAddTwoString);

			pAddTwoString = AddTwoStr(m_pOldBuffer, pNextMiddleString1);
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStem(pNode);
			SaveHTokenStemFeature(pNode, m_pOldBuffer);
			AddHTokenStem(pNode, 4);
			AddHTokenStem(pNode, 2);
			AddHTokenStem(pNode, 12);
			AddHTokenStem(pNode, 25);
			ClassfyKindChar(pNode, pAddTwoString);
		}
		//13n
		else
		{
			CHAR_DELETE(pMiddleString1);
			CHAR_DELETE(pAddTwoString);
			CHAR_DELETE(pAddTwoString2);
			CHAR_DELETE(pJongChar);
			//S(n+3~len)
			pMiddleString1 = GetUnRegisterMiddleCountChar(pOriginalString, nNextPos3, nTotalCount - 1, CALCULATE_STOP, CALCULATE_STOP);
			//C(n+2)
			pJongChar = GetChojungJongsungChar(pNextOneChar2, CHAR_JONGSUNG);
			//C(n+2)+S(n+2~len)
			pAddTwoString = AddTwoStr(pJongChar, pMiddleString1);
			//S(n+2)-C(n+2) : ���߼�
			pAddTwoString2 = GetChojungJongsungChar(pNextOneChar2, CHAR_CHOJUNGSUNG);
			if ((IsExistTabel009(pNextOneChar) && strcmp(pNextOneChar, "��") == 0 && IsExistEndingDic(pMiddleString1)) ||
				(strcmp(pAddTwoString2, "��") == 0 && IsExistEndingDic(pAddTwoString)) ||
				(strcmp(pAddTwoString2, "��") == 0 && IsExistEndingDic(pJongChar) && nTotalCount <= nNextPos2) ||
				((strcmp(pNextOneChar2, "��") == 0 || strcmp(pNextOneChar2, "��") == 0) && IsExistEndingDic(pMiddleString1))
				)
			{
				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr(m_pOldBuffer, pNextOneChar);
				InsertStemString(pNode, pAddTwoString);
				SaveHTokenStem(pNode);
				AddHTokenStem(pNode, S_N);
				AddHTokenStem(pNode, S_Lex);
				AddHTokenStem(pNode, S_Numb);
				ClassfyKindChar(pNode, pAddTwoString);
				InsertEndingString(pNode, pMiddleString);
				SaveHTokenEnding(pNode);
				AddHTokenEnding(pNode, E_Pol);
			}
			else
			{
				CHAR_DELETE(pAddTwoString);
				pAddTwoString = AddTwoStr(m_pOldBuffer, pNextMiddleString1);
				InsertStemString(pNode, pAddTwoString);
				SaveHTokenStem(pNode);
				SaveHTokenStemFeature(pNode, pNextMiddleString1);
				AddHTokenStem(pNode, S_N);
				AddHTokenStem(pNode, S_Lex);
				AddHTokenStem(pNode, S_Numb);
				DeleteHTokenStem(pNode, S_Amb);
			}
		}
	}
	else if (IsAalphabet(pPrevOneChar))
	{
		InsertStemString(pNode, m_pOldBuffer);
		SaveHTokenStem(pNode);
		SaveHTokenStemFeature(pNode, m_pOldBuffer);

		m_LinkedList->CreateSubNode(pNode);
		InsertAddSubTail(pNode, m_pOldBuffer);

		AddSubNodeTailFeature(pNode, 4);
		AddSubNodeTailFeature(pNode, 2);

		nLen = strlen(m_pOldBuffer);
		nMinusLen = nTotalCount - nLen;
		CHAR_DELETE(pBuffer);
		pBuffer = new char[nMinusLen+1];
		memset(pBuffer, 0x00, nMinusLen+1);
		nPos = 0;
		for (nLen; nLen<nTotalCount; nLen++)
		{
			pBuffer[nPos] = pOriginalString[nLen];
			nPos += 1;
		}

		CHAR_DELETE(pNode->pHToken->strSource);

		nLen = strlen(pBuffer);
		nPos = 0;
		pNode->pHToken->strSource = new char[nLen+1];
		memset(pNode->pHToken->strSource, 0x00, nLen+1);
		strcpy(pNode->pHToken->strSource, pBuffer);

		CHAR_DELETE(pBuffer);
		CHAR_DELETE(m_pOldBuffer);

		GoUWR(pNode);
	}
	// 11n, 12n, 13n, 14n
	else if ((isbFind005 = IsExistTabel005(pNextMiddleString5)) ||
		(isbFind006 = IsExistTabel006(pNextMiddleString4)) ||
		(isbFind007 = IsExistTabel007(pNextMiddleString3)) ||
		(isbFind008 = IsExistTabel008(pNextMiddleString2)))
	{
		CHAR_DELETE(pMiddleString);
		CHAR_DELETE(pMiddleString1);
		CHAR_DELETE(pMiddleString12);
		CHAR_DELETE(pThisOneChar);
		CHAR_DELETE(pNextOneChar);
		CHAR_DELETE(pChoJungChar);
		CHAR_DELETE(pJongChar);
		CHAR_DELETE(pAddTwoString);

		if (isbFind005 == true)
		{
			nPos1 = nNextPos5;
			pTempStr = pNextMiddleString5;
		}
		else if (isbFind006 == true)
		{
			nPos1 = nNextPos4;
			pTempStr = pNextMiddleString4;
		}
		else if (isbFind007 == true)
		{
			nPos1 = nNextPos3;
			pTempStr = pNextMiddleString3;
		}
		else if (isbFind008 == true)
		{
			nPos1 = nNextPos2;
			pTempStr = pNextMiddleString2;
		}

		if (nTotalCount > nPos1+1)
		{
			nPos = nPos1+1;
			if (pOriginalString[nPos1] & 0x80)
			{
				nPos += 1;
			}

			pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, nPos, nTotalCount - 1, CALCULATE_STOP, CALCULATE_STOP);
			pThisOneChar = GetUnRegisterOneChar(pOriginalString, nPos);
			pChoJungChar = GetChojungJongsungChar(pThisOneChar, CHAR_CHOJUNGSUNG);
			pJongChar = GetChojungJongsungChar(pThisOneChar, CHAR_JONGSUNG);
			if (nTotalCount > nPos+1)
			{
				nPos1 = nPos;
				nPos1 += 1;
				if (pOriginalString[nPos] & 0x80)
				{
					nPos1 += 1;
				}

				//S(n+7~len)
				pMiddleString1 = GetUnRegisterMiddleCountChar(pOriginalString, nPos1, nTotalCount - 1, CALCULATE_STOP, CALCULATE_STOP);
				//S(n+7)
				pNextOneChar = GetUnRegisterOneChar(pOriginalString, nPos1);
			}
			pAddTwoString = AddTwoStr(pJongChar, pMiddleString1);

			if (nTotalCount > nPos1+1)
			{
				nPos1 += 1;
				if (pOriginalString[nPos1 - 1] & 0x80)
				{
					nPos1 += 1;
				}

				//S(n+8~len)
				pMiddleString12 = GetUnRegisterMiddleCountChar(pOriginalString, nPos1, nTotalCount - 1, CALCULATE_STOP, CALCULATE_STOP);
			}
		}

		if (IsExistEndingDic(pMiddleString) ||
			(pMiddleString == NULL ||
			(pMiddleString != NULL && strlen(pMiddleString) == 0)))
		{
			CHAR_DELETE(pAddTwoString);

			if (isbFind005 == true)
			{
				pAddTwoString = AddTwoStr(m_pOldBuffer, pNextMiddleString5);
			}
			else if (isbFind006 == true)
			{
				pAddTwoString = AddTwoStr(m_pOldBuffer, pNextMiddleString4);
			}
			else if (isbFind007 == true)
			{
				pAddTwoString = AddTwoStr(m_pOldBuffer, pNextMiddleString3);
			}
			else if (isbFind008 == true)
			{
				pAddTwoString = AddTwoStr(m_pOldBuffer, pNextMiddleString2);
			}

			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStem(pNode);
			SaveHTokenStemFeature(pNode, m_pOldBuffer);

			AddHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_Lex);
			AddHTokenStem(pNode, S_Numb);
			AddHTokenStem(pNode, S_Unt);

			ClassfyKindChar(pNode, pAddTwoString);

			if (pMiddleString != NULL)
			{
				InsertEndingString(pNode, pMiddleString);
			}
			SaveHTokenEnding(pNode);

		}
		else if ((strcmp(pThisOneChar, "��") == 0 && ((strcmp(pNextOneChar, "��") == 0 ||
			strcmp(pNextOneChar, "��") == 0))) ||
			(strcmp(pThisOneChar, "��") == 0 && pMiddleString1 != NULL && IsExistEndingDic(pMiddleString1)) ||
			(strcmp(pChoJungChar, "��") == 0 && pAddTwoString != NULL && IsExistEndingDic(pAddTwoString)) ||
			(strcmp(pChoJungChar, "��") == 0 && pJongChar != NULL && IsExistEndingDic(pJongChar) &&
				strcmp(pThisOneChar, pLastOneChar) == 0 && pNextOneChar != NULL && IsExistFeature(pNextOneChar, STEM_DIC, S_N) &&
				IsExistFeature(pNextOneChar, STEM_DIC, S_Aff)))
		{
			CHAR_DELETE(pAddTwoString);

			pAddTwoString = AddTwoStr(m_pOldBuffer, pTempStr);
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStem(pNode);
			SaveHTokenStemFeature(pNode, m_pOldBuffer);
			AddHTokenStem(pNode, 4);
			AddHTokenStem(pNode, 2);
			AddHTokenStem(pNode, 25);
			AddHTokenStem(pNode, 12);
			ClassfyKindChar(pNode, pAddTwoString);

			CHAR_DELETE(pAddTwoString);

			pAddTwoString = AddTwoStr("��", pJongChar);
			pAddTwoString2 = AddTwoStr(pAddTwoString, pMiddleString1);

			InsertEndingString(pNode, pAddTwoString2);
			MakeNewEndingFeature(pNode);
			AddEndingFeature(pNode, pJongChar);
			AddEndingFeature(pNode, pMiddleString1);
			AddHTokenEnding(pNode, 26);
		}
		//14n
		else if (pNextOneChar != NULL && (strcmp(pNextOneChar, "��") == 0 || strcmp(pNextOneChar, "��") == 0) &&
			pMiddleString1 != NULL && IsExistEndingDic(pMiddleString1))
		{
			CHAR_DELETE(pAddTwoString);

			pAddTwoString = AddTwoStr(m_pOldBuffer, pTempStr);
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStem(pNode);
			SaveHTokenStemFeature(pNode, m_pOldBuffer);
			AddHTokenStem(pNode, 4);
			AddHTokenStem(pNode, 2);
			AddHTokenStem(pNode, 25);
			AddHTokenStem(pNode, 12);
			ClassfyKindChar(pNode, pAddTwoString);

			CHAR_DELETE(pAddTwoString);
			CHAR_DELETE(pAddTwoString2);

			pAddTwoString = AddTwoStr("�̾�", pJongChar);
			pAddTwoString2 = AddTwoStr(pAddTwoString, pMiddleString1);

			InsertEndingString(pNode, pAddTwoString2);
			MakeNewEndingFeature(pNode);
			AddHTokenEnding(pNode, E_Pol);
			AddEndingFeature(pNode, pJongChar);
			AddEndingFeature(pNode, pMiddleString1);
			AddHTokenEnding(pNode, 26);
		}
		else if (strcmp(pThisOneChar, "��") == 0 && IsExistEndingDic(pMiddleString1))
		{
			CHAR_DELETE(pAddTwoString);

			pAddTwoString = AddTwoStr(m_pOldBuffer, pTempStr);
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStem(pNode);
			AddHTokenStem(pNode, 4);
			AddHTokenStem(pNode, 2);
			AddHTokenStem(pNode, 12);
			AddHTokenStem(pNode, 25);
			ClassfyKindChar(pNode, pAddTwoString);

			CHAR_DELETE(pAddTwoString);

			pAddTwoString = AddTwoStr(pThisOneChar, pMiddleString1);
			InsertEndingString(pNode, pAddTwoString);
			SaveHTokenEnding(pNode);
			AddEndingFeature(pNode, pThisOneChar);
			AddEndingFeature(pNode, pMiddleString1);
		}
		else if (pNextOneChar != NULL &&
			((strcmp(pThisOneChar, "��") == 0 && strcmp(pNextOneChar, "ġ") == 0) ||
			(strcmp(pThisOneChar, "��") == 0 && strcmp(pNextOneChar, "��") == 0) || strcmp(pThisOneChar, "") == 0 && strcmp(pNextOneChar, "ġ") == 0 &&
				(strcmp(pThisOneChar, "��") == 0 && strcmp(pNextOneChar, "��") == 0) ||
				(strcmp(pThisOneChar, "��") == 0 && strcmp(pNextOneChar, "��") == 0)) &&
				(pMiddleString12 == NULL || (pMiddleString12 != NULL && IsExistEndingDic(pMiddleString12))))
		{
			CHAR_DELETE(pAddTwoString);
			CHAR_DELETE(pAddTwoString2);

			pAddTwoString = AddTwoStr(m_pOldBuffer, pTempStr);
			pAddTwoString2 = AddTwoStr(pAddTwoString, pThisOneChar);
			CHAR_DELETE(pAddTwoString);
			pAddTwoString = AddTwoStr(pAddTwoString2, pNextOneChar);

			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStem(pNode);
			AddHTokenStem(pNode, 4);
			AddHTokenStem(pNode, 2);
			AddHTokenStem(pNode, 12);
			AddHTokenStem(pNode, 25);
			ClassfyKindChar(pNode, pAddTwoString);

			if (pMiddleString12 != NULL)
			{
				InsertEndingString(pNode, pMiddleString12);
				SaveHTokenEnding(pNode);
			}
		}
		else if (strcmp(pThisOneChar, "0") >= 0 && strcmp(pThisOneChar, "9") <= 0)
		{
			CHAR_DELETE(pAddTwoString);

			pAddTwoString = AddTwoStr(m_pOldBuffer, pTempStr);

			CHAR_DELETE(m_pOldBuffer);

			m_pOldBuffer = new char[strlen(pAddTwoString)+1];
			memset(m_pOldBuffer, 0x00, strlen(pAddTwoString)+1);
			strcpy(m_pOldBuffer, pAddTwoString);

			nStart = nNextPos1+1;
			if (pOriginalString[nNextPos1+1] & 0x80)
			{
				nStart += 1;
			}

			GoFNM1_1(pNode, nStart);
		}
		else
		{
			InsertStemString(pNode, m_pOldBuffer);
			SaveHTokenStem(pNode);
			SaveHTokenStemFeature(pNode, m_pOldBuffer);
			ClassfyKindChar(pNode, m_pOldBuffer);

			m_LinkedList->CreateSubNode(pNode);
			InsertAddSubTail(pNode, m_pOldBuffer);

			AddSubNodeTailFeature(pNode, 4);
			AddSubNodeTailFeature(pNode, 2);
			AddSubNodeTailFeature(pNode, 12);

			nLen = strlen(m_pOldBuffer);
			nMinusLen = nTotalCount - nLen;
			CHAR_DELETE(pBuffer);
			pBuffer = new char[nMinusLen+1];
			memset(pBuffer, 0x00, nMinusLen+1);
			nPos = 0;
			for (nLen; nLen<nTotalCount; nLen++)
			{
				pBuffer[nPos] = pOriginalString[nLen];
				nPos += 1;
			}

			CHAR_DELETE(pNode->pHToken->strSource);

			nLen = strlen(pBuffer);
			nPos = 0;
			pNode->pHToken->strSource = new char[nLen+1];
			memset(pNode->pHToken->strSource, 0x00, nLen+1);
			strcpy(pNode->pHToken->strSource, pBuffer);

			CHAR_DELETE(pBuffer);
			CHAR_DELETE(m_pOldBuffer);

			GoUWR(pNode);
		}
	}
	else if (IsExistTabel009(pNextOneChar))
	{
		if (nTotalCount > nNextPos1+1)
		{
			nPos = nNextPos1+1;
			if (pOriginalString[nNextPos1] & 0x80)
			{
				nPos += 1;
			}

			CHAR_DELETE(pMiddleString);
			CHAR_DELETE(pMiddleString1);
			CHAR_DELETE(pMiddleString12);
			CHAR_DELETE(pMiddleString23);
			CHAR_DELETE(pThisOneChar);
			CHAR_DELETE(pNextOneChar2);
			CHAR_DELETE(pNextOneChar3);
			CHAR_DELETE(pNextOneChar4);
			CHAR_DELETE(pChoJungChar);
			CHAR_DELETE(pChoJungChar2);
			CHAR_DELETE(pChoJungChar3);
			CHAR_DELETE(pJongChar);
			CHAR_DELETE(pJongChar2);
			CHAR_DELETE(pJongChar3);
			CHAR_DELETE(pAddTwoString);
			CHAR_DELETE(pAddTwoString2);
			CHAR_DELETE(pAddTwoString3);


			pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, nPos, nTotalCount - 1, CALCULATE_STOP, CALCULATE_STOP);
			pThisOneChar = GetUnRegisterOneChar(pOriginalString, nPos);
			pChoJungChar = GetChojungJongsungChar(pThisOneChar, CHAR_CHOJUNGSUNG);
			pJongChar = GetChojungJongsungChar(pThisOneChar, CHAR_JONGSUNG);
			if (nTotalCount > nPos+1)
			{
				nPos += 1;
				if (pOriginalString[nPos - 1] & 0x80)
				{
					nPos += 1;
				}

				nPos1 = nPos;
				pMiddleString1 = GetUnRegisterMiddleCountChar(pOriginalString, nPos1, nTotalCount, CALCULATE_STOP, CALCULATE_STOP);
				//S(n+3)
				pNextOneChar2 = GetUnRegisterOneChar(pOriginalString, nPos1);
				//S(n+3)-C(n+3)
				pChoJungChar2 = GetChojungJongsungChar(pNextOneChar2, CHAR_CHOJUNGSUNG);
				//C(n+3)
				pJongChar2 = GetChojungJongsungChar(pNextOneChar2, CHAR_JONGSUNG);
			}
			//C(n+2)+S(n+3~len)
			pAddTwoString = AddTwoStr(pJongChar, pMiddleString1);

			if (nTotalCount > nPos+1)
			{
				nPos += 1;
				if (pOriginalString[nPos - 1] & 0x80)
				{
					nPos += 1;
				}

				nPos1 = nPos;
				//S(n+4~len)
				pMiddleString12 = GetUnRegisterMiddleCountChar(pOriginalString, nPos1, nTotalCount, CALCULATE_STOP, CALCULATE_STOP);
				//S(n+4)
				pNextOneChar3 = GetUnRegisterOneChar(pOriginalString, nPos1);
				//S(n+4)-C(n+4)
				pChoJungChar3 = GetChojungJongsungChar(pNextOneChar3, CHAR_CHOJUNGSUNG);
				//C(n+4)
				pJongChar3 = GetChojungJongsungChar(pChoJungChar3, CHAR_JONGSUNG);
			}
			//C(n+3)+S(n+4~len)
			pAddTwoString2 = AddTwoStr(pJongChar2, pMiddleString12);

			if (nTotalCount > nPos+1)
			{
				nPos += 1;
				if (pOriginalString[nPos - 1] & 0x80)
				{
					nPos += 1;
				}

				nPos1 = nPos;
				//S(n+5~len)
				pMiddleString23 = GetUnRegisterMiddleCountChar(pOriginalString, nPos1, nTotalCount, CALCULATE_STOP, CALCULATE_STOP);
				//S(n+5)
				pNextOneChar4 = GetUnRegisterOneChar(pOriginalString, nPos1);
			}
			//C(n+4)+S(n+5~len)
			pAddTwoString3 = AddTwoStr(pJongChar3, pMiddleString23);
			if (IsExistEndingDic(pMiddleString))
			{
				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr(m_pOldBuffer, pNextOneChar);
				InsertStemString(pNode, pAddTwoString);
				SaveHTokenStem(pNode);
				SaveHTokenStemFeature(pNode, m_pOldBuffer);
				AddHTokenStem(pNode, S_N);
				AddHTokenStem(pNode, S_Lex);
				AddHTokenStem(pNode, S_Numb);
				AddHTokenStem(pNode, S_Unt);
				ClassfyKindChar(pNode, pAddTwoString);

				InsertEndingString(pNode, pMiddleString);
				SaveHTokenEnding(pNode);

			}
			else if ((strcmp(pThisOneChar, "��") == 0 && ((strcmp(pNextOneChar, "��") == 0 ||
				strcmp(pNextOneChar, "��") == 0))) ||
				(strcmp(pThisOneChar, "��") == 0 && IsExistEndingDic(pMiddleString1)) ||
				(strcmp(pChoJungChar, "��") == 0 && IsExistEndingDic(pAddTwoString)) ||
				(strcmp(pChoJungChar, "��") == 0 && IsExistEndingDic(pJongChar) &&
					strcmp(pThisOneChar, pLastOneChar) == 0 && IsExistFeature(pNextOneChar, STEM_DIC, S_N) &&
					IsExistFeature(pNextOneChar, STEM_DIC, S_Aff)))
			{
				CHAR_DELETE(pAddTwoString);
				//2017-08-16 pAddTwoString2 DELETE �߰�
				CHAR_DELETE(pAddTwoString2);

				pAddTwoString = AddTwoStr(m_pOldBuffer, pNextOneChar);
				InsertStemString(pNode, pAddTwoString);
				SaveHTokenStem(pNode);
				SaveHTokenStemFeature(pNode, m_pOldBuffer);
				AddHTokenStem(pNode, S_N);
				AddHTokenStem(pNode, S_Lex);
				AddHTokenStem(pNode, S_Numb);
				AddHTokenStem(pNode, S_Unt);
				ClassfyKindChar(pNode, pAddTwoString);

				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr("��", pJongChar);
				pAddTwoString2 = AddTwoStr(pAddTwoString, pMiddleString1);

				InsertEndingString(pNode, pAddTwoString2);
				MakeNewEndingFeature(pNode);
				AddHTokenEnding(pNode, 26);
				AddEndingFeature(pNode, pJongChar);
				AddEndingFeature(pNode, pMiddleString1);


			}
			else if ((strcmp(pThisOneChar, "��") == 0 || strcmp(pThisOneChar, "��") == 0) &&
				(IsExistEndingDic(pMiddleString1) || pMiddleString1 == NULL))
			{
				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr(m_pOldBuffer, pNextOneChar);
				InsertStemString(pNode, pAddTwoString);
				SaveHTokenStem(pNode);
				SaveHTokenStemFeature(pNode, m_pOldBuffer);
				AddHTokenStem(pNode, 4);
				AddHTokenStem(pNode, 2);
				AddHTokenStem(pNode, 25);
				AddHTokenStem(pNode, 12);
				ClassfyKindChar(pNode, pAddTwoString);

				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr(pThisOneChar, pMiddleString1);
				InsertEndingString(pNode, pAddTwoString);
				MakeNewEndingFeature(pNode);
				AddEndingFeature(pNode, pThisOneChar);
				AddEndingFeature(pNode, pMiddleString1);
				DeleteHTokenEnding(pNode, 5);
			}
			else if (strcmp(pThisOneChar, "��") == 0 && strcmp(pNextOneChar2, "��") == 0 &&
				(pMiddleString12 != NULL && IsExistEndingDic(pMiddleString12) || pMiddleString12 == NULL || strlen(pMiddleString12) == 0))
			{
				CHAR_DELETE(pAddTwoString);
				CHAR_DELETE(pAddTwoString2);

				pAddTwoString = AddTwoStr(m_pOldBuffer, pNextOneChar);
				pAddTwoString2 = AddTwoStr(pAddTwoString, pThisOneChar);

				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr(pAddTwoString2, pNextOneChar2);

				InsertStemString(pNode, pAddTwoString);
				SaveHTokenStem(pNode);
				SaveHTokenStemFeature(pNode, m_pOldBuffer);
				AddHTokenStem(pNode, 4);
				AddHTokenStem(pNode, 2);
				AddHTokenStem(pNode, 25);
				AddHTokenStem(pNode, 12);
				ClassfyKindChar(pNode, pAddTwoString);

				if (pMiddleString12 != NULL)
				{
					InsertEndingString(pNode, pMiddleString12);
					AddEndingFeature(pNode, pMiddleString12);
				}
			}
			else if (pMiddleString23 != NULL && (strcmp(pMiddleString23, "����") == 0 ||
				strcmp(pMiddleString23, "����") == 0 ||
				strcmp(pMiddleString23, "����") == 0))
			{
				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr(m_pOldBuffer, pNextOneChar);
				InsertStemString(pNode, pAddTwoString);
				SaveHTokenStem(pNode);
				SaveHTokenStemFeature(pNode, m_pOldBuffer);
				AddHTokenStem(pNode, 4);
				AddHTokenStem(pNode, 2);
				AddHTokenStem(pNode, 25);
				AddHTokenStem(pNode, 12);
				ClassfyKindChar(pNode, pAddTwoString);

				CHAR_DELETE(pAddTwoString);
				CHAR_DELETE(pMiddleString12);

				//S(n+2~len)
				pMiddleString12 = GetUnRegisterMiddleCountChar(pOriginalString, nNextPos2, nTotalCount - 1, CALCULATE_STOP, CALCULATE_STOP);

				InsertEndingString(pNode, pMiddleString12);
				MakeNewEndingFeature(pNode);
				AddEndingFeature(pNode, pMiddleString12);
			}
			//20n S(n+2) �����ΰ�?
			else if (strcmp(pThisOneChar, "0") >= 0 && strcmp(pThisOneChar, "9") <= 0)
			{
				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr(m_pOldBuffer, pNextOneChar);

				CHAR_DELETE(m_pOldBuffer);
				CHAR_DELETE(pNextOneChar);

				m_pOldBuffer = new char[strlen(pAddTwoString)+1];
				memset(m_pOldBuffer, 0x00, strlen(pAddTwoString)+1);
				strcpy(m_pOldBuffer, pAddTwoString);

				nStart = nNextPos1+1;
				if (pOriginalString[nPos] & 0x80)
				{
					nPos += 1;
				}

				GoFNM1_1(pNode, nStart);
			}
			//21n
			else if ((bEPol = IsExistTabel008(pMiddleString23))
				&&
				(pMiddleString12 == NULL ||
				(pMiddleString12 != NULL &&IsExistEndingDic(pMiddleString12)) ||
					(pNextOneChar3 != NULL && pNextOneChar4 != NULL && strcmp(pNextOneChar3, "��") == 0 && (strcmp(pNextOneChar4, "��") == 0 || strcmp(pNextOneChar4, "��") == 0)) ||
					(pNextOneChar3 != NULL && pMiddleString23 != NULL && strcmp(pNextOneChar3, "��") == 0 &&
						IsExistStemDic(pMiddleString23) &&
						IsExistFeature(pMiddleString23, STEM_DIC, S_N) == false &&
						IsExistFeature(pMiddleString23, STEM_DIC, S_Aff) == false) ||
						(pChoJungChar3 != NULL && pAddTwoString3 != NULL &&
							strcmp(pChoJungChar3, "��") == 0 && IsExistEndingDic(pAddTwoString3)) ||
							(pChoJungChar3 != NULL && pJongChar3 != NULL && pNextOneChar3 != NULL &&
								strcmp(pChoJungChar3, "��") == 0 && IsExistEndingDic(pJongChar3) &&
								strcmp(pNextOneChar3, pLastOneChar) == 0)))
			{
				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr(m_pOldBuffer, pMiddleString23);
				InsertStemString(pNode, pAddTwoString);
				SaveHTokenStem(pNode);
				SaveHTokenStemFeature(pNode, m_pOldBuffer);
				AddHTokenStem(pNode, S_N);
				AddHTokenStem(pNode, S_Lex);
				AddHTokenStem(pNode, S_Numb);
				AddHTokenStem(pNode, S_Unt);
				ClassfyKindChar(pNode, pAddTwoString);

				if (bEPol == true && pMiddleString12 != NULL)
				{
					InsertEndingString(pNode, pMiddleString12);
					SaveHTokenEnding(pNode);
				}
				else
				{
					CHAR_DELETE(pAddTwoString);
					if (pAddTwoString3 != NULL)
					{
						pAddTwoString = AddTwoStr("��", pAddTwoString3);
						InsertEndingString(pNode, pAddTwoString);
						SaveHTokenEnding(pNode);
						AddHTokenEnding(pNode, E_Pol);
						AddEndingFeature(pNode, pAddTwoString3);
					}
					else
					{
						InsertEndingString(pNode, "��");
						SaveHTokenEnding(pNode);
						AddHTokenEnding(pNode, E_Pol);
					}
				}
			}
			//
			else if ((bEPol = (IsExistTabel009(pThisOneChar)) || strcmp(pThisOneChar, "t") == 0 || strcmp(pThisOneChar, "T") == 0)
				&&
				(pMiddleString1 == NULL || strlen(pMiddleString1) == 0 ||
				(pMiddleString1 != NULL && IsExistEndingDic(pMiddleString1)) ||
					(pNextOneChar2 != NULL && pNextOneChar3 != NULL &&
						strcmp(pNextOneChar2, "��") == 0 && (strcmp(pNextOneChar3, "��") == 0 || strcmp(pNextOneChar3, "��") == 0)) ||
						(pNextOneChar2 != NULL && pMiddleString12 != NULL &&
							strcmp(pNextOneChar2, "��") == 0 &&
							IsExistStemDic(pMiddleString12) &&
							IsExistFeature(pMiddleString12, STEM_DIC, S_N) == false &&
							IsExistFeature(pMiddleString12, STEM_DIC, S_Aff) == false) ||
							(pChoJungChar2 != NULL && pAddTwoString2 != NULL &&
								strcmp(pChoJungChar2, "��") == 0 && IsExistEndingDic(pAddTwoString2)) ||
								(pChoJungChar2 != NULL && pJongChar2 != NULL && pNextOneChar2 != NULL &&
									strcmp(pChoJungChar2, "��") == 0 && IsExistEndingDic(pJongChar2) &&
									strcmp(pNextOneChar2, pLastOneChar) == 0)))
			{
				CHAR_DELETE(pAddTwoString);
				CHAR_DELETE(pAddTwoString3);

				pAddTwoString = AddTwoStr(m_pOldBuffer, pNextOneChar);
				pAddTwoString3 = AddTwoStr(pAddTwoString, pThisOneChar);
				InsertStemString(pNode, pAddTwoString3);
				SaveHTokenStem(pNode);
				AddHTokenStem(pNode, S_N);
				AddHTokenStem(pNode, S_Lex);
				AddHTokenStem(pNode, S_Numb);
				AddHTokenStem(pNode, S_Unt);
				ClassfyKindChar(pNode, pAddTwoString3);

				if (bEPol == true && pMiddleString1 != NULL)
				{
					InsertEndingString(pNode, pMiddleString1);
					SaveHTokenEnding(pNode);
				}
				else if (bEPol != true)
				{
					CHAR_DELETE(pAddTwoString);
					if (pAddTwoString2 != NULL)
					{
						pAddTwoString = AddTwoStr("��", pAddTwoString2);
						InsertEndingString(pNode, pAddTwoString);
						SaveHTokenEnding(pNode);
						AddHTokenEnding(pNode, E_Pol);
						AddEndingFeature(pNode, pAddTwoString2);
					}
					else
					{
						InsertEndingString(pNode, "��");
						SaveHTokenEnding(pNode);
						AddHTokenEnding(pNode, E_Pol);
					}
				}
			}
			else if (strcmp(pThisOneChar, "��") == 0 && pNextOneChar2 != NULL && IsExistTabel009(pNextOneChar2)
				&& (pMiddleString12 == NULL || strlen(pMiddleString12) == 0 || pMiddleString12 != NULL && IsExistEndingDic(pMiddleString12)))
			{
				CHAR_DELETE(pAddTwoString);
				CHAR_DELETE(pAddTwoString3);

				pAddTwoString = AddTwoStr(m_pOldBuffer, pNextOneChar);
				pAddTwoString3 = AddTwoStr(pAddTwoString, pThisOneChar);
				CHAR_DELETE(pAddTwoString);
				pAddTwoString = AddTwoStr(pAddTwoString3, pNextOneChar2);
				InsertStemString(pNode, pAddTwoString);
				SaveHTokenStem(pNode);
				AddHTokenStem(pNode, S_N);
				AddHTokenStem(pNode, S_Lex);
				AddHTokenStem(pNode, S_Numb);
				AddHTokenStem(pNode, S_Unt);
				ClassfyKindChar(pNode, pAddTwoString);

				InsertEndingString(pNode, pMiddleString12);
				SaveHTokenEnding(pNode);
			}
			else
			{
				InsertStemString(pNode, m_pOldBuffer);
				SaveHTokenStem(pNode);
				SaveHTokenStemFeature(pNode, m_pOldBuffer);

				ClassfyKindChar(pNode, m_pOldBuffer);

				m_LinkedList->CreateSubNode(pNode);
				InsertAddSubTail(pNode, m_pOldBuffer);

				AddSubNodeTailFeature(pNode, 4);
				AddSubNodeTailFeature(pNode, 2);
				AddSubNodeTailFeature(pNode, 12);

				nLen = strlen(m_pOldBuffer);
				nMinusLen = nTotalCount - nLen;
				CHAR_DELETE(pBuffer);
				pBuffer = new char[nMinusLen+1];
				memset(pBuffer, 0x00, nMinusLen+1);
				nPos = 0;
				for (nLen; nLen<nTotalCount; nLen++)
				{
					pBuffer[nPos] = pOriginalString[nLen];
					nPos += 1;
				}

				CHAR_DELETE(pNode->pHToken->strSource);

				nLen = strlen(pBuffer);
				nPos = 0;
				pNode->pHToken->strSource = new char[nLen+1];
				memset(pNode->pHToken->strSource, 0x00, nLen+1);
				strcpy(pNode->pHToken->strSource, pBuffer);

				CHAR_DELETE(pBuffer);
				CHAR_DELETE(m_pOldBuffer);

				GoUWR(pNode);
			}
		}
	}
	else if (strcmp(pNextOneChar, "��") == 0 || strcmp(pNextOneChar, "��") == 0)
	{
		CHAR_DELETE(pAddTwoString);

		pAddTwoString = AddTwoStr(m_pOldBuffer, pNextOneChar);
		InsertStemString(pNode, pAddTwoString);
		SaveHTokenStem(pNode);
		SaveHTokenStemFeature(pNode, m_pOldBuffer);
		DeleteHTokenStem(pNode, 4);//n
		AddHTokenStem(pNode, 7);//v
		ClassfyKindChar(pNode, pAddTwoString);

		CHAR_DELETE(pAddTwoString);

		pAddTwoString = AddTwoStr(pJongChar, pMiddleString);
		InsertEndingString(pNode, pAddTwoString);
		SaveHTokenEnding(pNode);
	}
	else if (strcmp(pChoJungChar, "��") == 0 || strcmp(pChoJungChar, "��") == 0)
	{
		CHAR_DELETE(pAddTwoString);

		pAddTwoString = AddTwoStr(m_pOldBuffer, pNextOneChar);
		InsertStemString(pNode, pAddTwoString);
		SaveHTokenStem(pNode);
		SaveHTokenStemFeature(pNode, m_pOldBuffer);
		DeleteHTokenStem(pNode, 4);//n
		AddHTokenStem(pNode, 7);//v
		ClassfyKindChar(pNode, pAddTwoString);

		CHAR_DELETE(pAddTwoString);

		pAddTwoString = AddTwoStr(pJongChar, pMiddleString);
		InsertEndingString(pNode, pAddTwoString);
		SaveHTokenEnding(pNode);
	}
	else
	{
		CHAR_DELETE(pMiddleString12);

		//S(n+2~len)
		pMiddleString12 = GetUnRegisterMiddleCountChar(pOriginalString, nNextPos2, nTotalCount - 1, CALCULATE_STOP, CALCULATE_STOP);
		if ((strcmp(pThisOneChar, "0") >= 0 && strcmp(pThisOneChar, "9") <= 0) &&
			IsNextOneCharSpecial(pNextOneChar) == true &&
			IsExistEndingDic(pMiddleString12) == true)//S(n)==���� && S(n+1)==Ư����ȣ && S(n+2~len)�� ���� ������ �ִ°�?
		{
			CHAR_DELETE(pAddTwoString);

			pAddTwoString = AddTwoStr(m_pOldBuffer, pNextOneChar);
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStem(pNode);
			SaveHTokenStemFeature(pNode, m_pOldBuffer);
			AddHTokenStem(pNode, 12);//Numb
			ClassfyKindChar(pNode, pAddTwoString);

			InsertEndingString(pNode, pMiddleString12);
			SaveHTokenEnding(pNode);
		}
		else
		{
			InsertStemString(pNode, m_pOldBuffer);
			SaveHTokenStem(pNode);
			SaveHTokenStemFeature(pNode, m_pOldBuffer);

			m_LinkedList->CreateSubNode(pNode);
			InsertAddSubTail(pNode, m_pOldBuffer);

			AddSubNodeTailFeature(pNode, 4);
			AddSubNodeTailFeature(pNode, 2);

			nLen = strlen(m_pOldBuffer);
			nMinusLen = nTotalCount - nLen;
			CHAR_DELETE(pBuffer);
			pBuffer = new char[nMinusLen+1];
			memset(pBuffer, 0x00, nMinusLen+1);
			nPos = 0;
			for (nLen; nLen<nTotalCount; nLen++)
			{
				pBuffer[nPos] = pOriginalString[nLen];
				nPos += 1;
			}

			CHAR_DELETE(pNode->pHToken->strSource);

			nLen = strlen(pBuffer);
			nPos = 0;
			pNode->pHToken->strSource = new char[nLen+1];
			memset(pNode->pHToken->strSource, 0x00, nLen+1);
			strcpy(pNode->pHToken->strSource, pBuffer);

			CHAR_DELETE(pBuffer);
			CHAR_DELETE(m_pOldBuffer);

			GoUWR(pNode);
		}
	}
	/*
	CHAR_DELETE(pMiddleString);
	CHAR_DELETE(pMiddleString1);
	CHAR_DELETE(pNextMiddleString1);
	CHAR_DELETE(pNextMiddleString2);
	CHAR_DELETE(pNextMiddleString3);
	CHAR_DELETE(pNextMiddleString4);
	CHAR_DELETE(pNextMiddleString5);
	CHAR_DELETE(pThisOneChar);
	CHAR_DELETE(pLastOneChar);
	CHAR_DELETE(pNextOneChar);
	CHAR_DELETE(pNextOneChar2);
	CHAR_DELETE(pNextOneChar3);
	CHAR_DELETE(pNextOneChar4);
	CHAR_DELETE(pChoJungChar);
	CHAR_DELETE(pChoJungChar2);
	CHAR_DELETE(pChoJungChar3);
	CHAR_DELETE(pJongChar);
	CHAR_DELETE(pJongChar2);
	CHAR_DELETE(pJongChar3);
	CHAR_DELETE(pPrevOneChar);
	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pAddTwoString2);
	CHAR_DELETE(pAddTwoString3);
	CHAR_DELETE(pMiddleString12);
	CHAR_DELETE(pMiddleString23);
	*/
	CHAR_DELETE(pMiddleString);
	CHAR_DELETE(pMiddleString1);
	CHAR_DELETE(pNextMiddleString1);
	CHAR_DELETE(pNextMiddleString2);
	CHAR_DELETE(pNextMiddleString3);
	CHAR_DELETE(pNextMiddleString4);
	CHAR_DELETE(pNextMiddleString5);
	CHAR_DELETE(pThisOneChar);
	CHAR_DELETE(pNextOneChar);
	CHAR_DELETE(pNextOneChar2);
	CHAR_DELETE(pNextOneChar3);
	CHAR_DELETE(pNextOneChar4);
	CHAR_DELETE(pLastOneChar);
	CHAR_DELETE(pChoJungChar);
	CHAR_DELETE(pChoJungChar2);
	CHAR_DELETE(pChoJungChar3);
	CHAR_DELETE(pJongChar);
	CHAR_DELETE(pJongChar2);
	CHAR_DELETE(pJongChar3);
	CHAR_DELETE(pPrevOneChar);
	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pAddTwoString2);
	CHAR_DELETE(pAddTwoString3);
	CHAR_DELETE(pMiddleString12);
	CHAR_DELETE(pMiddleString23);
	CHAR_DELETE(pBuffer);

}

void CNAHangulAnaly::GoUWR(H_NODE *pNode)
{
	char* pOriginalString = NULL;
	char* pThisOneChar = NULL;

	int nStart = -1;
	int mStart = 0;
	int nLen = 0;
	int nCount = 0;

	//��ü���ν�NER 01
	if (GoNER01(pNode) == false)
	{
		pOriginalString = pNode->pHToken->strSource;
		pThisOneChar = GetUnRegisterOneChar(pOriginalString, nCount);
		if (strcmp(pThisOneChar, "��") == 0 || strcmp(pThisOneChar, "��") == 0 || strcmp(pThisOneChar, "��") == 0)
		{
			CHAR_DELETE(pThisOneChar);
			GoPFM(pNode, nStart, mStart);
		}
		else
		{
			CHAR_DELETE(pThisOneChar);

			if (m_nUWRFlag == 2)
			{
				GoUWR1_1(pNode, nStart, mStart);
				//��ü���ν�NER 02
				GoNER02(pNode);
			}
			else
				m_nUWRFlag = 1;
		}
	}

	CHAR_DELETE(pThisOneChar);
}


void CNAHangulAnaly::GoUWR1_1(H_NODE *pNode, int &nStart, int &mStart)
{
	char* pOriginalString = NULL;

	char* pMiddleString = NULL;
	char* pAddTwoString = NULL;
	char* pAddTwoString2 = NULL;
	char* pOneChar = NULL;
	char* pJongChar = NULL;

	int nTotalLen = 0;
	int nLen = 0;
	int nCount = 0;


	pOriginalString = pNode->pHToken->strSource;

	nTotalLen = strlen(pOriginalString);

	if (nStart == -1)
	{
		nStart = 0;
	}
	else
	{
		UnRegisteMoveRight(pOriginalString, nStart);
	}

	pOneChar = GetUnRegisterOneChar(pOriginalString, nStart);

	if (nStart >= nTotalLen)
	{
		if (m_pOldBuffer != NULL && strcmp(m_pOldBuffer, pOriginalString) == 0)
		{
			InsertStemString(pNode, m_pOldBuffer);
			SaveHTokenStem(pNode);
			ClassfyKindChar(pNode, m_pOldBuffer);

			m_LinkedList->CreateSubNode(pNode);
			InsertAddSubTail(pNode, m_pOldBuffer);
		}
		else
		{
			CHAR_DELETE(pNode->pHToken->strSource);

			nLen = strlen(m_pRealOriginalString);
			pNode->pHToken->strSource = new char[nLen+1];
			memset(pNode->pHToken->strSource, 0x00, nLen+1);
			strcpy(pNode->pHToken->strSource, m_pRealOriginalString);

			CHAR_DELETE(pOneChar);
			CHAR_DELETE(pJongChar);
			CHAR_DELETE(pMiddleString);
			CHAR_DELETE(pAddTwoString);
			CHAR_DELETE(pAddTwoString2);

			GoEND(pNode);
		}
	}
	else
	{
		if (pOriginalString[nStart] & 0x80)
		{
			nCount = nStart+2;
			if (pOriginalString[nCount] & 0x80)
			{
				nCount = nCount+1;
			}
		}
		else
		{
			nCount = nStart+1;
			if (pOriginalString[nCount] & 0x80)
			{
				nCount = nCount+2;
			}
		}
		pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, mStart, nStart+1, CALCULATE_STOP, CALCULATE_STOP);
		if (IsExistStemDic(pMiddleString))
		{
			CHAR_DELETE(m_pOldBuffer);

			nLen = strlen(pMiddleString);
			m_pOldBuffer = new char[nLen+1];
			memset(m_pOldBuffer, 0x00, nLen+1);
			strcpy(m_pOldBuffer, pMiddleString);
			if (nStart+1 == nTotalLen-1 || nStart == nTotalLen-1)
			{
				if (nStart == mStart &&
					(strcmp(m_pOldBuffer, "��") == 0 || strcmp(m_pOldBuffer, "��") == 0 ||
						strcmp(m_pOldBuffer, "��") == 0 ||
						strcmp(m_pOldBuffer, "��") == 0))
				{
					CHAR_DELETE(pNode->pHToken->subEnding->strResult);

					nLen = strlen(m_pOldBuffer);
					nLen += 1;
					pNode->pHToken->subEnding->strResult = new char[nLen];
					memset(pNode->pHToken->subEnding->strResult, 0x00, nLen);
					strcpy(pNode->pHToken->subEnding->strResult, m_pOldBuffer);
					if (pNode->pHToken->subEnding->strFeature != NULL)
					{
						memset(pNode->pHToken->subEnding->strFeature, '0', ENDING_DIC_BASIC_FEATURE_NUMBER - 1);
					}
					SaveHTokenEnding(pNode);
				}
				else if (nStart == mStart &&
					(strcmp(m_pOldBuffer, "��") == 0 || strcmp(m_pOldBuffer, "��") == 0 ||
						strcmp(m_pOldBuffer, "��") == 0))
				{
					pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);

					CHAR_DELETE(pNode->pHToken->subStem->strResult);

					if (pNode->pHToken->subStem->strFeature != NULL)
					{
						memset(pNode->pHToken->subStem->strFeature, '0', STEM_DIC_ALL_FEATURE_NUMBER - 1);
					}

					pNode->pHToken->subStem->strResult = new char[3];
					memset(pNode->pHToken->subStem->strResult, 0x00, 3);
					InsertStemString(pNode, "��");
					SaveHTokenStem(pNode);
					ClassfyKindChar(pNode, "��");

					CHAR_DELETE(pNode->pHToken->subEnding->strResult);

					nLen = strlen(pJongChar);
					nLen += 1;
					pNode->pHToken->subEnding->strResult = new char[nLen];
					memset(pNode->pHToken->subEnding->strResult, 0x00, nLen);
					strcpy(pNode->pHToken->subEnding->strResult, pJongChar);
					if (pNode->pHToken->subEnding->strFeature != NULL)
					{
						memset(pNode->pHToken->subEnding->strFeature, '0', ENDING_DIC_BASIC_FEATURE_NUMBER - 1);
					}
					SaveHTokenEnding(pNode);
				}
				else if (nStart == mStart &&
					(strcmp(m_pOldBuffer, "��") == 0 || strcmp(m_pOldBuffer, "��") == 0 ||
						strcmp(m_pOldBuffer, "ȭ") == 0) && pNode->pHToken->subStem->strResult != NULL)
				{
					CHAR_DELETE(pAddTwoString);

					pAddTwoString = AddTwoStr(pNode->pHToken->subStem->strResult, m_pOldBuffer);

					CHAR_DELETE(pNode->pHToken->subStem->strResult);

					nLen = strlen(pAddTwoString);
					nLen += 1;
					pNode->pHToken->subStem->strResult = new char[nLen];
					memset(pNode->pHToken->subStem->strResult, 0x000, nLen);
					strcpy(pNode->pHToken->subStem->strResult, pAddTwoString);
					SaveHTokenStemFeature(pNode, m_pOldBuffer);
				}
				else
				{
					if (m_pOldBuffer != NULL)
					{
						CHAR_DELETE(pNode->pHToken->subStem->strResult);

						InsertStemString(pNode, m_pOldBuffer);
						SaveHTokenStem(pNode);
						ClassfyKindChar(pNode, m_pOldBuffer);

						m_LinkedList->CreateSubNode(pNode);
						InsertAddSubTail(pNode, m_pOldBuffer);
					}
					else
					{
						m_LinkedList->CreateSubNode(pNode);
						InsertAddSubTail(pNode, m_pOldBuffer);
					}
				}
			}
			else
			{
				CHAR_DELETE(pOneChar);
				CHAR_DELETE(pJongChar);
				CHAR_DELETE(pMiddleString);
				CHAR_DELETE(pAddTwoString);
				CHAR_DELETE(pAddTwoString2);

				GoUWR1_1(pNode, nStart, mStart);
			}
		}
		else
		{
			if (m_pOldBuffer == NULL)//m_pOldBuffer�� ���ΰ�?
			{
				CHAR_DELETE(pOneChar);
				CHAR_DELETE(pJongChar);
				CHAR_DELETE(pMiddleString);
				CHAR_DELETE(pAddTwoString);
				CHAR_DELETE(pAddTwoString2);

				GoUWR1_1(pNode, nStart, mStart);
			}
			else
			{
				CHAR_DELETE(pOneChar);
				CHAR_DELETE(pJongChar);
				CHAR_DELETE(pMiddleString);
				CHAR_DELETE(pAddTwoString);
				CHAR_DELETE(pAddTwoString2);

				GoUWR1_2(pNode, nStart, mStart);
			}
		}
	}
	/*
	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pMiddleString);
	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pAddTwoString2);
	*/
	CHAR_DELETE(pMiddleString);
	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pAddTwoString2);
	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pJongChar);

}

void CNAHangulAnaly::GoUWR1_2(H_NODE *pNode, int &nStart, int &mStart)
{
	char* pOriginalString = NULL;

	char* pMiddleString = NULL;
	char* pMiddleString2 = NULL;
	char* pAddTwoString = NULL;
	char* pAddTwoString2 = NULL;
	char* pLastOneChar = NULL;
	char* pOneChar = NULL;
	char* ptmpFeature = NULL; // Added by KKH 2004-01-12
	char* ptmpFeature2 = NULL; // Added by KKH 2004-01-12

	int nLen = 0;
	int nCount = 0;
	int nTotalLen = 0;
	int nThisPos = 0;

	ptmpFeature = new char[256]; // Added by KKH 2004-01-12
	memset(ptmpFeature, 0x00, 256); // Added by KKH 2004-01-12
	ptmpFeature2 = new char[256]; // Added by KKH 2004-01-12
	memset(ptmpFeature2, 0x00, 256); // Added by KKH 2004-01-12
	pOriginalString = pNode->pHToken->strSource;
	nTotalLen = strlen(pOriginalString);

	if (pOriginalString[nStart] & 0x80)
	{
		nCount = nStart+2;
	}
	else
	{
		nCount = nStart+1;
	}
	//S(m~n+1)
	pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, mStart, nCount - 1, CALCULATE_STOP, CALCULATE_STOP);
	if ((unsigned)nCount+1 < strlen(pOriginalString))
	{
		if (pOriginalString[nCount] & 0x80)
		{
			nCount += 2;
		}
		else
		{
			nCount += 1;
		}
	}
	//S(m~n+2)
	pMiddleString2 = GetUnRegisterMiddleCountChar(pOriginalString, mStart, nCount - 1, CALCULATE_STOP, CALCULATE_STOP);

	if (IsExistStemDic(pMiddleString, ptmpFeature))
	{
		CHAR_DELETE(m_pOldBuffer);

		nLen = strlen(pMiddleString);
		nLen += 1;
		m_pOldBuffer = new char[nLen];
		memset(m_pOldBuffer, 0x00, nLen);
		strcpy(m_pOldBuffer, pMiddleString);
		//n++
		UnRegisteMoveRight(pOriginalString, nStart);

		CHAR_DELETE(pMiddleString);
		CHAR_DELETE(pMiddleString2);
		CHAR_DELETE(pAddTwoString);
		CHAR_DELETE(pAddTwoString2);
		CHAR_DELETE(pLastOneChar);
		CHAR_DELETE(pOneChar);

		GoUWR1_1(pNode, nStart, mStart);
	}
	else if (IsExistStemDic(pMiddleString2, ptmpFeature2))
	{
		CHAR_DELETE(pMiddleString);

		if (pOriginalString[nTotalLen-1] & 0x80)
		{
			nThisPos = nTotalLen - 2;
		}
		else
		{
			nThisPos = nTotalLen-1;
		}
		//S(len)
		pLastOneChar = GetUnRegisterOneChar(pOriginalString, nThisPos);
		//S(n+2)
		pOneChar = GetUnRegisterOneChar(pOriginalString, nCount);
		if ((unsigned)nCount+1 < strlen(pOriginalString))
		{
			if (pOriginalString[nCount] & 0x80)
			{
				nCount += 2;
			}
			else
			{
				nCount += 1;
			}
			//S(m~n+3)
			pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, mStart, nCount - 1, CALCULATE_STOP, CALCULATE_STOP);
		}
		if (pMiddleString != NULL)
		{
			if (strcmp(pLastOneChar, pOneChar) != 0 && IsExistEndingDic(pMiddleString))
			{
				CHAR_DELETE(m_pOldBuffer);

				nLen = strlen(pMiddleString2);
				nLen += 1;
				m_pOldBuffer = new char[nLen];
				memset(m_pOldBuffer, 0x00, nLen);
				strcpy(m_pOldBuffer, pMiddleString2);

				CHAR_DELETE(pNode->pHToken->subEnding->strResult);

				nLen = strlen(pMiddleString);
				nLen += 1;
				pNode->pHToken->subEnding->strResult = new char[nLen];
				memset(pNode->pHToken->subEnding->strResult, 0x00, nLen);
				strcpy(pNode->pHToken->subEnding->strResult, pMiddleString);
				if (pNode->pHToken->subEnding->strFeature != NULL)
				{
					memset(pNode->pHToken->subEnding->strFeature, '0', ENDING_DIC_BASIC_FEATURE_NUMBER - 1);
				}
				SaveHTokenEnding(pNode);

				if (m_pOldBuffer != NULL)
				{
					CHAR_DELETE(pNode->pHToken->subStem->strResult);

					InsertStemString(pNode, m_pOldBuffer);
					SaveHTokenStem(pNode);
					ClassfyKindChar(pNode, m_pOldBuffer);

					m_LinkedList->CreateSubNode(pNode);
					InsertAddSubTail(pNode, m_pOldBuffer);
				}
				else
				{
					m_LinkedList->CreateSubNode(pNode);
					InsertAddSubTail(pNode, m_pOldBuffer);
				}
			}
			else
			{
				CHAR_DELETE(m_pOldBuffer);

				nLen = strlen(pMiddleString2);
				nLen += 1;
				m_pOldBuffer = new char[nLen];
				memset(m_pOldBuffer, 0x00, nLen);
				strcpy(m_pOldBuffer, pMiddleString2);

				CHAR_DELETE(pMiddleString);
				CHAR_DELETE(pMiddleString2);
				CHAR_DELETE(pAddTwoString);
				CHAR_DELETE(pAddTwoString2);
				CHAR_DELETE(pLastOneChar);
				CHAR_DELETE(pOneChar);

				UnRegisteMoveRight(pOriginalString, nStart);
				GoUWR1_1(pNode, nStart, mStart);
			}
		}
		else
		{
			CHAR_DELETE(m_pOldBuffer);

			nLen = strlen(pMiddleString2);
			nLen += 1;
			m_pOldBuffer = new char[nLen];
			memset(m_pOldBuffer, 0x00, nLen);
			strcpy(m_pOldBuffer, pMiddleString2);

			CHAR_DELETE(pMiddleString);
			CHAR_DELETE(pMiddleString2);
			CHAR_DELETE(pAddTwoString);
			CHAR_DELETE(pAddTwoString2);
			CHAR_DELETE(pOneChar);
			CHAR_DELETE(pLastOneChar);

			UnRegisteMoveRight(pOriginalString, nStart);
			GoUWR1_1(pNode, nStart, mStart);
		}
	}
	else
	{
		CHAR_DELETE(pAddTwoString);
		CHAR_DELETE(pAddTwoString2);
		CHAR_DELETE(pMiddleString);
		CHAR_DELETE(pMiddleString2);
		CHAR_DELETE(pOneChar);
		CHAR_DELETE(pLastOneChar);

		GoUWR1_3(pNode, nStart, mStart);
	}

	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pLastOneChar);
	CHAR_DELETE(pMiddleString);
	CHAR_DELETE(pMiddleString2);
	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pAddTwoString2);
	CHAR_DELETE(ptmpFeature);
	CHAR_DELETE(ptmpFeature2);
}

void CNAHangulAnaly::GoUWR1_3(H_NODE *pNode, int &nStart, int &mStart)
{
	char* pOriginalString = NULL;

	char* pMiddleString = NULL;
	char* pMiddleString2 = NULL;
	char* pAddTwoString = NULL;
	char* pAddTwoString2 = NULL;
	char* pBuffer = 0;
	char* pJongChar = NULL;
	char* pChoJungChar = NULL;
	char* pOneChar = NULL;

	int nLen = 0;
	int nCount = 0;
	int nTotalLen = 0;
	int nThisPos = 0;
	int nMinusLen = 0;
	int nNextPos = 0;
	int nSubNodeCount = 0;

	pOriginalString = pNode->pHToken->strSource;

	nTotalLen = strlen(pOriginalString);

	if (m_pOldBuffer != NULL)
	{
		if ((strcmp(m_pOldBuffer, "��") == 0 ||
			strcmp(m_pOldBuffer, "��") == 0 ||
			strcmp(m_pOldBuffer, "ȭ") == 0) &&
			pNode->pHToken->subStem->strResult != NULL)
		{
			CHAR_DELETE(pAddTwoString);

			pAddTwoString = AddTwoStr(pNode->pHToken->subStem->strResult, m_pOldBuffer);

			CHAR_DELETE(pNode->pHToken->subStem->strResult);

			nLen = strlen(pAddTwoString);
			nLen += 1;
			pNode->pHToken->subStem->strResult = new char[nLen];
			memset(pNode->pHToken->subStem->strResult, 0x000, nLen);
			strcpy(pNode->pHToken->subStem->strResult, pAddTwoString);


			///////////���ո�� �߰� �κ�
			nSubNodeCount = SubNodeCount(pNode);

			//"��"�� �ٴ� ��翡�� [T151]���� �߰�
			if (strcmp(m_pOldBuffer, "��") == 0)
			{
				AddSubNodeTailFeature(pNode, S_T151);
			}

			if (m_pOldBuffer != NULL)
			{
				m_LinkedList->CreateSubNode(pNode);
				InsertAddSubTail(pNode, m_pOldBuffer);
			}
			else
			{
				m_LinkedList->CreateSubNode(pNode);
				InsertAddSubTail(pNode, m_pOldBuffer);
			}

			if (pOriginalString[nStart] & 0x80)
			{
				nCount = nStart+2;
			}
			else
			{
				nCount = nStart+1;
			}

			//S(n+1~len)
			pMiddleString2 = GetUnRegisterMiddleCountChar(pOriginalString, nCount, nTotalLen-1, CALCULATE_STOP, CALCULATE_STOP);
			if ((unsigned)nCount+1 < strlen(pOriginalString))
			{
				if (pOriginalString[nCount] & 0x80)
				{
					nCount += 2;
				}
				else
				{
					nCount += 1;
				}
			}
			CHAR_DELETE(pAddTwoString);

			//S(n)
			pOneChar = GetUnRegisterOneChar(pOriginalString, nStart);
			pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);
			pChoJungChar = GetChojungJongsungChar(pOneChar, CHAR_CHOJUNGSUNG);
			pAddTwoString = AddTwoStr(pJongChar, pMiddleString2);


			//S(n~len)
			pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, nStart, nTotalLen-1, CALCULATE_STOP, CALCULATE_STOP);
			if (IsExistEndingDic(pMiddleString))
			{
				CHAR_DELETE(pNode->pHToken->subEnding->strResult);

				nLen = strlen(pMiddleString);
				nLen += 1;
				pNode->pHToken->subEnding->strResult = new char[nLen];
				memset(pNode->pHToken->subEnding->strResult, 0x00, nLen);
				strcpy(pNode->pHToken->subEnding->strResult, pMiddleString);
				if (pNode->pHToken->subEnding->strFeature != NULL)
				{
					memset(pNode->pHToken->subEnding->strFeature, '0', ENDING_DIC_BASIC_FEATURE_NUMBER - 1);
				}
				SaveHTokenEnding(pNode);
			}
			else if ((strcmp(pOneChar, "��") == 0 && IsExistEndingDic(pMiddleString2)) ||
				(strcmp(pChoJungChar, "��") == 0 && IsExistEndingDic(pAddTwoString)))
			{
				CHAR_DELETE(pNode->pHToken->subEnding->strResult);

				nLen = strlen(pMiddleString);
				nLen += 1;
				pNode->pHToken->subEnding->strResult = new char[nLen];
				memset(pNode->pHToken->subEnding->strResult, 0x00, nLen);
				strcpy(pNode->pHToken->subEnding->strResult, pMiddleString);

				if (pNode->pHToken->subEnding->strFeature != NULL)
				{
					memset(pNode->pHToken->subEnding->strFeature, '0', ENDING_DIC_BASIC_FEATURE_NUMBER - 1);
				}

				if (IsExistEndingDic(pAddTwoString))
				{
					SaveHTokenEnding(pNode);
					AddEndingFeature(pNode, "��");
					AddHTokenEnding(pNode, 26);
					AddEndingFeature(pNode, pAddTwoString);
					DeleteHTokenEnding(pNode, 4);
					DeleteHTokenEnding(pNode, 2);
					DeleteHTokenEnding(pNode, 25);
				}
				else
				{
					SaveHTokenEnding(pNode);
					AddEndingFeature(pNode, "��");
					AddHTokenEnding(pNode, 26);
					AddEndingFeature(pNode, pJongChar);
					AddEndingFeature(pNode, pMiddleString2);
					DeleteHTokenEnding(pNode, 4);
					DeleteHTokenEnding(pNode, 2);
					DeleteHTokenEnding(pNode, 25);
				}
			}
			else if ((strcmp(pChoJungChar, "��") == 0 || strcmp(pChoJungChar, "��") == 0) &&
				strcmp(pJongChar, "��") == 0 &&
				(pMiddleString2 == NULL || (pMiddleString2 != NULL &&
				(strlen(pMiddleString2) == 0 || IsExistEndingDic(pMiddleString2) == true))))
			{
				CHAR_DELETE(pAddTwoString);
				//���ո�� ���翩�ο� ���� ó�� ���
				if (nSubNodeCount <= 1)
				{
					DeleteAllSubNode(pNode);
					pAddTwoString = AddTwoStr(pNode->pHToken->subStem->strResult, pChoJungChar);
					InsertStemString(pNode, pAddTwoString);
					SaveHTokenStem(pNode);
					SaveHTokenStemFeature(pNode, pNode->pHToken->subStem->strResult);
					SaveHTokenStemFeature(pNode, pChoJungChar);
					AddHTokenStem(pNode, S_Cmp);
					DeleteHTokenStem(pNode, S_V);
					DeleteHTokenStem(pNode, S_N);
					ClassfyKindChar(pNode, pAddTwoString);
				}
				else
				{
					pAddTwoString = AddTwoStr(pNode->pHToken->subStem->strResult, pChoJungChar);
					DeleteSubNodeTail(pNode);
					ReplaceSubNodeTailString(pNode, pAddTwoString);
					AddSubNodeTailFeature(pNode, pNode->pHToken->subStem->strResult);
					AddSubNodeTailFeature(pNode, pChoJungChar);
				}

				if (pMiddleString2 == NULL)
				{
					InsertEndingString(pNode, pJongChar);
					AddEndingFeature(pNode, pMiddleString2);
				}
				else
				{
					CHAR_DELETE(pAddTwoString);
					pAddTwoString = AddTwoStr(pJongChar, pMiddleString2);
					InsertEndingString(pNode, pAddTwoString);
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, pJongChar);
					AddEndingFeature(pNode, pMiddleString2);
				}
			}
			//8n
			else if ((strcmp(pChoJungChar, "��") == 0 || strcmp(pChoJungChar, "��") == 0) &&
				(strcmp(pJongChar, "��") == 0 || strcmp(pJongChar, "��") == 0) &&
				(pMiddleString2 == NULL || (pMiddleString2 != NULL &&
				(strlen(pMiddleString2) == 0 || IsExistEndingDic(pMiddleString2) == true))))
			{
				CHAR_DELETE(pAddTwoString);
				//���ո�� ���翩�ο� ���� ó�� ���
				if (nSubNodeCount <= 1)
				{
					DeleteAllSubNode(pNode);
					pAddTwoString = AddTwoStr(pNode->pHToken->subStem->strResult, pChoJungChar);
					InsertStemString(pNode, pAddTwoString);
					SaveHTokenStem(pNode);
					SaveHTokenStemFeature(pNode, pNode->pHToken->subStem->strResult);
					SaveHTokenStemFeature(pNode, pChoJungChar);
					// Added by KKH 2003-12-16
					if (pNode->pHToken->subStem->strFeature[S_Amb] == '1')
						pNode->pHToken->subStem->strFeature[S_Amb] = '0';
					// Added by KKH 2003-12-16
					if (strcmp(pChoJungChar, "��") == 0)
					{
						AddHTokenStem(pNode, S_Act);
						AddHTokenStem(pNode, S_V);
						DeleteHTokenStem(pNode, S_N);
					}
					else if (strcmp(pChoJungChar, "��") == 0)
					{
						AddHTokenStem(pNode, S_Fnc);
						AddHTokenStem(pNode, S_V);
						DeleteHTokenStem(pNode, S_Act);
						DeleteHTokenStem(pNode, S_N);
					}
					ClassfyKindChar(pNode, pAddTwoString);
				}
				else
				{
					pAddTwoString = AddTwoStr(pNode->pHToken->subStem->strResult, pChoJungChar);
					DeleteSubNodeTail(pNode);
					ReplaceSubNodeTailString(pNode, pAddTwoString);
					AddSubNodeTailFeature(pNode, pNode->pHToken->subStem->strResult);
					AddSubNodeTailFeature(pNode, pChoJungChar);
					if (strcmp(pChoJungChar, "��") == 0)
					{
						AddSubNodeTailFeature(pNode, S_Act);
						AddSubNodeTailFeature(pNode, S_V);
						DeleteSubNodeTailFeature(pNode, S_N);
					}
					else if (strcmp(pChoJungChar, "��") == 0)
					{
						AddSubNodeTailFeature(pNode, S_Fnc);
						AddSubNodeTailFeature(pNode, S_V);
						DeleteSubNodeTailFeature(pNode, S_N);
						DeleteSubNodeTailFeature(pNode, S_Act);
					}
				}

				if (pMiddleString2 == NULL)
				{
					InsertEndingString(pNode, pJongChar);
				}
				else
				{
					pAddTwoString = AddTwoStr(pJongChar, pMiddleString2);
					InsertEndingString(pNode, pAddTwoString);
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, pJongChar);
					AddEndingFeature(pNode, pMiddleString2);
				}
			}
			else if (IsExistStemDic(pMiddleString))
			{
				CHAR_DELETE(pNode->pHToken->subStem->strResult);

				nLen = strlen(pMiddleString);
				nLen += 1;
				pNode->pHToken->subStem->strResult = new char[nLen];
				memset(pNode->pHToken->subStem->strResult, 0x00, nLen);
				strcpy(pNode->pHToken->subStem->strResult, pMiddleString);
				if (pNode->pHToken->subStem->strFeature != NULL)
				{
					memset(pNode->pHToken->subStem->strFeature, '0', STEM_DIC_ALL_FEATURE_NUMBER - 1);
				}
				SaveHTokenStem(pNode);


				if (pMiddleString != NULL)
				{
					m_LinkedList->CreateSubNode(pNode);
					InsertAddSubTail(pNode, pMiddleString);
				}
				else
				{
					m_LinkedList->CreateSubNode(pNode);
					InsertAddSubTail(pNode, pMiddleString);
				}
			}
			else
			{
				nLen = strlen(m_pOldBuffer);
				nTotalLen = strlen(pOriginalString);
				nMinusLen = nTotalLen - nLen;
				pBuffer = new char[nMinusLen+1];
				memset(pBuffer, 0x00, nMinusLen+1);
				nCount = 0;
				for (nLen; nLen < nTotalLen; nLen++)
				{
					pBuffer[nCount] = pOriginalString[nLen];
					nCount += 1;
				}

				CHAR_DELETE(pNode->pHToken->strSource);

				nLen = strlen(pBuffer);
				nCount = 0;
				pNode->pHToken->strSource = new char[nLen+1];
				memset(pNode->pHToken->strSource, 0x00, nLen+1);
				strcpy(pNode->pHToken->strSource, pBuffer);

				CHAR_DELETE(pChoJungChar);

				CHAR_DELETE(pJongChar);
				CHAR_DELETE(pOneChar);
				CHAR_DELETE(pBuffer);

				CHAR_DELETE(pMiddleString);
				CHAR_DELETE(pAddTwoString);
				CHAR_DELETE(pAddTwoString2);
				CHAR_DELETE(m_pOldBuffer);
				CHAR_DELETE(pMiddleString2);

				nStart = -1;
				mStart = 0;
				GoUWR1_1(pNode, nStart, mStart);
			}

			CHAR_DELETE(m_pOldBuffer);
		}
		else if (strcmp(m_pOldBuffer, "��") == 0)
		{
			//S(n~len)
			pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, nStart, nTotalLen-1, CALCULATE_STOP, CALCULATE_STOP);

			nCount = nStart - 1;
			//S(n-1)
			pOneChar = GetOneChar(pOriginalString, nCount);
			pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);
			pAddTwoString2 = AddTwoStr(pJongChar, pMiddleString);

			if (IsExistEndingDic(pAddTwoString2))
			{

				CHAR_DELETE(pNode->pHToken->subStem->strResult);

				if (pNode->pHToken->subStem->strFeature != NULL)
				{
					memset(pNode->pHToken->subStem->strFeature, '0', STEM_DIC_ALL_FEATURE_NUMBER - 1);
				}

				pNode->pHToken->subStem->strResult = new char[3];
				memset(pNode->pHToken->subStem->strResult, 0x00, 3);
				InsertStemString(pNode, "��");
				SaveHTokenStem(pNode);
				ClassfyKindChar(pNode, "��");

				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr(pJongChar, pMiddleString);

				CHAR_DELETE(pNode->pHToken->subEnding->strResult);

				nLen = strlen(pAddTwoString);
				nLen += 1;
				pNode->pHToken->subEnding->strResult = new char[nLen];
				memset(pNode->pHToken->subEnding->strResult, 0x00, nLen);
				strcpy(pNode->pHToken->subEnding->strResult, pAddTwoString);
				if (pNode->pHToken->subEnding->strFeature != NULL)
				{
					memset(pNode->pHToken->subEnding->strFeature, '0', ENDING_DIC_BASIC_FEATURE_NUMBER - 1);
				}
				SaveHTokenEnding(pNode);
			}
			else
			{
				if (m_pOldBuffer != NULL)
				{
					CHAR_DELETE(pNode->pHToken->subStem->strResult);

					InsertStemString(pNode, m_pOldBuffer);
					SaveHTokenStem(pNode);
					ClassfyKindChar(pNode, m_pOldBuffer);

					m_LinkedList->CreateSubNode(pNode);
					InsertAddSubTail(pNode, m_pOldBuffer);
				}
				else
				{
					m_LinkedList->CreateSubNode(pNode);
					InsertAddSubTail(pNode, m_pOldBuffer);
				}

				if (IsExistEndingDic(pMiddleString))
				{
					CHAR_DELETE(pNode->pHToken->subEnding->strResult);

					nLen = strlen(pMiddleString);
					nLen += 1;
					pNode->pHToken->subEnding->strResult = new char[nLen];
					memset(pNode->pHToken->subEnding->strResult, 0x00, nLen);
					strcpy(pNode->pHToken->subEnding->strResult, pMiddleString);
					if (pNode->pHToken->subEnding->strFeature != NULL)
					{
						memset(pNode->pHToken->subEnding->strFeature, '0', ENDING_DIC_BASIC_FEATURE_NUMBER - 1);
					}
					SaveHTokenEnding(pNode);
				}
				else if (IsExistStemDic(pMiddleString))
				{
					//���ո�� �߰� 
					CHAR_DELETE(pNode->pHToken->subStem->strResult);

					nLen = strlen(pMiddleString);
					nLen += 1;
					pNode->pHToken->subStem->strResult = new char[nLen];
					memset(pNode->pHToken->subStem->strResult, 0x00, nLen);
					strcpy(pNode->pHToken->subStem->strResult, pMiddleString);
					if (pNode->pHToken->subStem->strFeature != NULL)
					{
						memset(pNode->pHToken->subStem->strFeature, '0', STEM_DIC_ALL_FEATURE_NUMBER - 1);
					}
					SaveHTokenStem(pNode);
					ClassfyKindChar(pNode, pMiddleString);

					if (m_pOldBuffer != NULL)
					{
						m_LinkedList->CreateSubNode(pNode);
						InsertAddSubTail(pNode, pMiddleString);
					}
					else
					{
						m_LinkedList->CreateSubNode(pNode);
						InsertAddSubTail(pNode, pMiddleString);
					}

					CHAR_DELETE(m_pOldBuffer);
				}
				else
				{
					nLen = strlen(m_pOldBuffer);
					nTotalLen = strlen(pOriginalString);
					nMinusLen = nTotalLen - nLen;
					pBuffer = new char[nMinusLen+1];
					memset(pBuffer, 0x00, nMinusLen+1);
					nCount = 0;
					for (nLen; nLen < nTotalLen; nLen++)
					{
						pBuffer[nCount] = pOriginalString[nLen];
						nCount += 1;
					}

					CHAR_DELETE(pNode->pHToken->strSource);

					nLen = strlen(pBuffer);
					nCount = 0;
					pNode->pHToken->strSource = new char[nLen+1];
					memset(pNode->pHToken->strSource, 0x00, nLen+1);
					strcpy(pNode->pHToken->strSource, pBuffer);

					CHAR_DELETE(m_pOldBuffer);
					CHAR_DELETE(pJongChar);
					CHAR_DELETE(pOneChar);
					CHAR_DELETE(pBuffer);
					CHAR_DELETE(pMiddleString);
					CHAR_DELETE(pAddTwoString);
					CHAR_DELETE(pAddTwoString2);
					CHAR_DELETE(pMiddleString2);
					CHAR_DELETE(pChoJungChar);

					nStart = -1;
					mStart = 0;
					GoUWR1_1(pNode, nStart, mStart);
				}
			}
		}
		else
		{
			pOneChar = GetUnRegisterOneChar(pOriginalString, nStart);
			pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);
			pChoJungChar = GetChojungJongsungChar(pOneChar, CHAR_CHOJUNGSUNG);

			if (pOriginalString[nStart] & 0x80)
			{
				nNextPos = nStart+2;
			}
			else
			{
				nNextPos = nStart+1;
			}

			//S(n+1~len)
			pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, nNextPos, nTotalLen-1, CALCULATE_STOP, CALCULATE_STOP);
			pAddTwoString2 = AddTwoStr(pJongChar, pMiddleString);

			//7n
			if ((strcmp(pChoJungChar, "��") == 0 || strcmp(pChoJungChar, "��") == 0) &&
				strcmp(pJongChar, "��") == 0 &&
				(pMiddleString == NULL || (pMiddleString != NULL &&
				(strlen(pMiddleString) == 0 || IsExistEndingDic(pMiddleString) == true))))
			{
				CHAR_DELETE(pAddTwoString);
				if (m_pOldBuffer != NULL)
				{
					pAddTwoString = AddTwoStr(m_pOldBuffer, pChoJungChar);
				}

				CHAR_DELETE(pNode->pHToken->subStem->strResult);

				nLen = strlen(pAddTwoString);
				nLen += 1;
				pNode->pHToken->subStem->strResult = new char[nLen];
				memset(pNode->pHToken->subStem->strResult, 0x000, nLen);
				strcpy(pNode->pHToken->subStem->strResult, pAddTwoString);

				InsertStemString(pNode, pAddTwoString);
				SaveHTokenStem(pNode);
				SaveHTokenStemFeature(pNode, pNode->pHToken->subStem->strResult);
				SaveHTokenStemFeature(pNode, pChoJungChar);
				AddHTokenStem(pNode, S_Cmp);
				DeleteHTokenStem(pNode, S_V);
				DeleteHTokenStem(pNode, S_N);
				DeleteHTokenStem(pNode, S_Amb);
				ClassfyKindChar(pNode, pAddTwoString);

				if (pAddTwoString != NULL)
				{
					m_LinkedList->CreateSubNode(pNode);
					InsertAddSubTail(pNode, pAddTwoString);
					AddSubNodeTailFeature(pNode, m_pOldBuffer);
					AddSubNodeTailFeature(pNode, S_Cmp);
					DeleteSubNodeTailFeature(pNode, S_V);
					DeleteSubNodeTailFeature(pNode, S_N);
					DeleteSubNodeTailFeature(pNode, S_Amb);
				}

				if (pMiddleString == NULL || strlen(pMiddleString) == 0)
				{
					InsertEndingString(pNode, pJongChar);
					AddEndingFeature(pNode, pJongChar);
				}
				else
				{
					CHAR_DELETE(pAddTwoString);
					pAddTwoString = AddTwoStr(pJongChar, pMiddleString);
					InsertEndingString(pNode, pAddTwoString);
					AddEndingFeature(pNode, pMiddleString);
				}
			}
			//8n
			else if ((strcmp(pChoJungChar, "��") == 0 || strcmp(pChoJungChar, "��") == 0) &&
				(strcmp(pJongChar, "��") == 0 || strcmp(pJongChar, "��") == 0) &&
				(pMiddleString == NULL || (pAddTwoString2 != NULL &&
				(strlen(pAddTwoString2) == 0 || IsExistEndingDic(pAddTwoString2) == true))))
			{
				CHAR_DELETE(pAddTwoString);
				if (m_pOldBuffer != NULL)
				{
					pAddTwoString = AddTwoStr(m_pOldBuffer, pChoJungChar);
				}

				CHAR_DELETE(pNode->pHToken->subStem->strResult);

				nLen = strlen(pAddTwoString);
				nLen += 1;
				pNode->pHToken->subStem->strResult = new char[nLen];
				memset(pNode->pHToken->subStem->strResult, 0x000, nLen);
				strcpy(pNode->pHToken->subStem->strResult, pAddTwoString);

				InsertStemString(pNode, pAddTwoString);
				SaveHTokenStem(pNode);
				SaveHTokenStemFeature(pNode, pNode->pHToken->subStem->strResult);
				SaveHTokenStemFeature(pNode, pChoJungChar);
				if (strcmp(pChoJungChar, "��") == 0)
				{
					AddHTokenStem(pNode, S_Act);
					AddHTokenStem(pNode, S_V);
					DeleteHTokenStem(pNode, S_N);
					DeleteHTokenStem(pNode, S_Amb);
					if (pAddTwoString != NULL)
					{
						m_LinkedList->CreateSubNode(pNode);
						InsertAddSubTail(pNode, pAddTwoString);
						AddSubNodeTailFeature(pNode, m_pOldBuffer);
						AddSubNodeTailFeature(pNode, S_Act);
						AddSubNodeTailFeature(pNode, S_V);
						DeleteSubNodeTailFeature(pNode, S_N);
						DeleteSubNodeTailFeature(pNode, S_Amb);
					}
				}
				else if (strcmp(pChoJungChar, "��") == 0)
				{
					AddHTokenStem(pNode, S_Fnc);
					AddHTokenStem(pNode, S_V);
					DeleteHTokenStem(pNode, S_Act);
					DeleteHTokenStem(pNode, S_N);
					DeleteHTokenStem(pNode, S_Amb);
					if (pAddTwoString != NULL)
					{
						m_LinkedList->CreateSubNode(pNode);
						InsertAddSubTail(pNode, pAddTwoString);
						AddSubNodeTailFeature(pNode, m_pOldBuffer);
						AddSubNodeTailFeature(pNode, S_Fnc);
						AddSubNodeTailFeature(pNode, S_V);
						DeleteSubNodeTailFeature(pNode, S_Act);
						DeleteSubNodeTailFeature(pNode, S_N);
						DeleteSubNodeTailFeature(pNode, S_Amb);
					}
				}

				ClassfyKindChar(pNode, pAddTwoString);

				if (pMiddleString == NULL || strlen(pMiddleString) == 0)
				{
					InsertEndingString(pNode, pJongChar);
					AddEndingFeature(pNode, pJongChar);
				}
				else
				{
					pAddTwoString = AddTwoStr(pJongChar, pMiddleString);
					InsertEndingString(pNode, pAddTwoString);
					AddEndingFeature(pNode, pMiddleString);
				}
			}
			else
			{
				CHAR_DELETE(pChoJungChar);
				CHAR_DELETE(pJongChar);
				CHAR_DELETE(pOneChar);
				CHAR_DELETE(pMiddleString);

				GoUWR1_4(pNode, nStart, mStart);
			}
		}
	}
	else
	{
		GoUWR1_4(pNode, nStart, mStart);
	}

	CHAR_DELETE(pChoJungChar);
	CHAR_DELETE(pJongChar);
	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pBuffer);
	CHAR_DELETE(pMiddleString);
	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pAddTwoString2);
	CHAR_DELETE(pMiddleString2);
}

void CNAHangulAnaly::GoUWR1_4(H_NODE *pNode, int &nStart, int &mStart)
{
	H_SUBNODE* pSubNode = NULL;
	char* pOriginalString = NULL;

	char* pMiddleString = NULL;
	char* pMiddleString2 = NULL;
	char* pMiddleString3 = NULL;
	char* pNextOneChar = NULL;
	char* pNextOneChar2 = NULL;
	char* pOneChar = NULL;
	char* pJongChar = NULL;
	char* pChoJungChar = NULL;
	char* pAddTwoString = NULL;
	char* pAddTwoString2 = NULL;
	char* pBuffer = NULL;

	int nLen = 0;
	int nCount = 0;
	int nTotalLen = 0;
	int nNextPos = 0;
	int nMinusLen = 0;



	pOriginalString = pNode->pHToken->strSource;
	nTotalLen = strlen(pOriginalString);

	if (pOriginalString[nStart] & 0x80)
	{
		nCount = nStart+2;
	}
	else
	{
		nCount = nStart+1;
	}

	//S(n+1~len)
	pMiddleString2 = GetUnRegisterMiddleCountChar(pOriginalString, nCount, nTotalLen-1, CALCULATE_STOP, CALCULATE_STOP);
	//S(n)
	pOneChar = GetUnRegisterOneChar(pOriginalString, nStart);
	//S(n+1)
	pNextOneChar = GetUnRegisterOneChar(pOriginalString, nCount);

	if (pOriginalString[nCount] & 0x80)
	{
		nNextPos = nCount+2;
	}
	else
	{
		nNextPos = nCount+1;
	}

	//S(n+1~n+2)
	if (nNextPos < nTotalLen)
	{
		pMiddleString3 = GetUnRegisterMiddleCountChar(pOriginalString, nCount, nNextPos+1, CALCULATE_STOP, CALCULATE_STOP);
		//S(n+1)
		pNextOneChar2 = GetUnRegisterOneChar(pOriginalString, nNextPos);
	}

	pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);
	pChoJungChar = GetChojungJongsungChar(pOneChar, CHAR_CHOJUNGSUNG);
	pAddTwoString2 = AddTwoStr(pJongChar, pMiddleString2);


	if (m_pOldBuffer != NULL)
	{
		CHAR_DELETE(pNode->pHToken->subStem->strResult);

		InsertStemString(pNode, m_pOldBuffer);
		SaveHTokenStem(pNode);
		ClassfyKindChar(pNode, m_pOldBuffer);

		m_LinkedList->CreateSubNode(pNode);
		InsertAddSubTail(pNode, m_pOldBuffer);
	}
	else
	{
		m_LinkedList->CreateSubNode(pNode);
		InsertAddSubTail(pNode, m_pOldBuffer);
	}

	//S(n~len)
	pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, nStart, nTotalLen-1, CALCULATE_STOP, CALCULATE_STOP);

	CHAR_DELETE(pAddTwoString2);

	pAddTwoString2 = AddTwoStr(pJongChar, pMiddleString2);

	if (IsExistEndingDic(pMiddleString))
	{
		CHAR_DELETE(m_pOldBuffer);
		CHAR_DELETE(pNode->pHToken->subEnding->strResult);

		nLen = strlen(pMiddleString);
		nLen += 1;
		pNode->pHToken->subEnding->strResult = new char[nLen];
		memset(pNode->pHToken->subEnding->strResult, 0x00, nLen);
		strcpy(pNode->pHToken->subEnding->strResult, pMiddleString);
		if (pNode->pHToken->subEnding->strFeature != NULL)
		{
			memset(pNode->pHToken->subEnding->strFeature, '0', ENDING_DIC_BASIC_FEATURE_NUMBER - 1);
		}
		SaveHTokenEnding(pNode);
	}
	//8n
	else if ((strcmp(pOneChar, "��") == 0 && IsExistEndingDic(pMiddleString2) == true &&
		strcmp(pMiddleString2, "��") != 0) ||
		(strcmp(pChoJungChar, "��") == 0 && IsExistEndingDic(pAddTwoString2) == true &&
			strcmp(pMiddleString2, "��") != 0) ||
			(strcmp(pOneChar, "��") == 0 && strcmp(pNextOneChar, "��") == 0) ||
		strcmp(pOneChar, "��") == 0 && IsExistEndingDic(pMiddleString2) == true)
	{
		//���ո�� �߰� 
		CHAR_DELETE(pNode->pHToken->subEnding->strResult);

		nLen = strlen(pMiddleString);
		nLen += 1;
		pNode->pHToken->subEnding->strResult = new char[nLen];
		memset(pNode->pHToken->subEnding->strResult, 0x00, nLen);
		strcpy(pNode->pHToken->subEnding->strResult, pMiddleString);
		if (pNode->pHToken->subEnding->strFeature != NULL)
			memset(pNode->pHToken->subEnding->strFeature, '0', ENDING_DIC_BASIC_FEATURE_NUMBER - 1);

		if (IsExistEndingDic(pAddTwoString2))
		{
			SaveHTokenEnding(pNode);
			AddEndingFeature(pNode, "��");
			AddHTokenEnding(pNode, 26);
			AddEndingFeature(pNode, pAddTwoString2);
			if (pNextOneChar2 != NULL && strcmp(pNextOneChar2, "��") == 0)
			{
				AddEndingFeature(pNode, pNextOneChar2);
			}
			DeleteHTokenEnding(pNode, 4);
			DeleteHTokenEnding(pNode, 2);
			DeleteHTokenEnding(pNode, 25);
		}
		else
		{
			SaveHTokenEnding(pNode);
			AddEndingFeature(pNode, "��");
			AddHTokenEnding(pNode, 26);
			AddEndingFeature(pNode, pJongChar);
			AddEndingFeature(pNode, pMiddleString2);
			if (pNextOneChar2 != NULL && strcmp(pNextOneChar2, "��") == 0)
			{
				AddEndingFeature(pNode, pNextOneChar2);
			}
			DeleteHTokenEnding(pNode, 4);
			DeleteHTokenEnding(pNode, 2);
			DeleteHTokenEnding(pNode, 25);
		}
	}
	//9n
	else if ((strcmp(pChoJungChar, "��") == 0 || strcmp(pChoJungChar, "��") == 0) &&
		strcmp(pJongChar, "��") == 0 &&
		(pMiddleString2 == NULL || IsExistEndingDic(pMiddleString2) == true))
	{
		//10y
		CHAR_DELETE(pAddTwoString);

		if (pNode->pHToken->subEnding->strFeature != NULL)
		{
			memset(pNode->pHToken->subEnding->strFeature, '0', ENDING_DIC_BASIC_FEATURE_NUMBER - 1);
		}
		else
		{
			pNode->pHToken->subEnding->strFeature = new char[ENDING_DIC_BASIC_FEATURE_NUMBER];
			memset(pNode->pHToken->subEnding->strFeature, '0', ENDING_DIC_BASIC_FEATURE_NUMBER - 1);
			pNode->pHToken->subEnding->strFeature[ENDING_DIC_BASIC_FEATURE_NUMBER - 1] = 0;
		}

		if (strcmp(pChoJungChar, "��") == 0)
		{
			pAddTwoString = AddTwoStr("��", pMiddleString2);

			InsertEndingString(pNode, pAddTwoString);
			AddEndingFeature(pNode, pMiddleString2);
			SaveHTokenEnding(pNode);
			AddHTokenEnding(pNode, E_Pol);
		}
		else if (strcmp(pChoJungChar, "��") == 0)
		{
			pAddTwoString = AddTwoStr("��", pMiddleString2);

			DeleteHTokenStem(pNode, S_V);
			DeleteHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_Cmp);

			InsertEndingString(pNode, pAddTwoString);
			AddEndingFeature(pNode, pMiddleString2);
			SaveHTokenEnding(pNode);
		}
	}
	//10n
	else if ((pNextOneChar != NULL && strcmp(pNextOneChar, "��") == 0) ||
		(pMiddleString3 != NULL &&
		(strcmp(pNextOneChar, "����") == 0 ||
			strcmp(pNextOneChar, "����") == 0 ||
			strcmp(pNextOneChar, "����") == 0)))
	{
		if (pNode->pHToken->subEnding->strFeature != NULL)
		{
			memset(pNode->pHToken->subEnding->strFeature, '0', ENDING_DIC_BASIC_FEATURE_NUMBER - 1);
		}
		else
		{
			pNode->pHToken->subEnding->strFeature = new char[ENDING_DIC_BASIC_FEATURE_NUMBER];
			memset(pNode->pHToken->subEnding->strFeature, '0', ENDING_DIC_BASIC_FEATURE_NUMBER - 1);
			pNode->pHToken->subEnding->strFeature[ENDING_DIC_BASIC_FEATURE_NUMBER - 1] = 0;
		}
		InsertEndingString(pNode, pMiddleString2);
		AddEndingFeature(pNode, pMiddleString2);
		SaveHTokenEnding(pNode);
	}
	else if (((strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0) && IsExistEndingDic(pMiddleString2) == true) ||
		(strcmp(pOneChar, "��") == 0 && strcmp(pMiddleString2, "�̶�") == 0))
	{
		CHAR_DELETE(pAddTwoString);
		pAddTwoString = AddTwoStr(pOneChar, pMiddleString2);

		nLen = strlen(pAddTwoString);
		nLen += 1;

		CHAR_DELETE(pNode->pHToken->subEnding->strResult);
		pNode->pHToken->subEnding->strResult = new char[nLen];

		memset(pNode->pHToken->subEnding->strResult, 0x00, nLen);
		strcpy(pNode->pHToken->subEnding->strResult, pAddTwoString);

		SaveHTokenEnding(pNode);
		AddEndingFeature(pNode, pMiddleString2);
		AddEndingFeature(pNode, pOneChar);
	}
	else if ((strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 ||
		strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 ||
		strcmp(pOneChar, "��") == 0) &&
		IsExistEndingDic(pMiddleString2) == true)
	{
		CHAR_DELETE(pAddTwoString);
		pAddTwoString = AddTwoStr(pOneChar, pMiddleString2);

		nLen = strlen(pAddTwoString);
		nLen += 1;

		CHAR_DELETE(pNode->pHToken->subEnding->strResult);
		pNode->pHToken->subEnding->strResult = new char[nLen];

		memset(pNode->pHToken->subEnding->strResult, 0x00, nLen);
		strcpy(pNode->pHToken->subEnding->strResult, pAddTwoString);

		SaveHTokenEnding(pNode);
		AddEndingFeature(pNode, pMiddleString2);
		AddEndingFeature(pNode, pOneChar);
	}
	else if (IsExistStemDic(pMiddleString))
	{
		if (strcmp(pMiddleString, "��") == 0 || strcmp(pMiddleString, "��") == 0 ||
			strcmp(pMiddleString, "ȭ") == 0)
		{
			CHAR_DELETE(pAddTwoString);

			if (pNode->pHToken->subStem->strResult != NULL)
			{
				pAddTwoString = AddTwoStr(pNode->pHToken->subStem->strResult, pMiddleString);
				nLen = strlen(pAddTwoString);
				nLen += 1;
				delete[]pNode->pHToken->subStem->strResult;
				pNode->pHToken->subStem->strResult = NULL;
				pNode->pHToken->subStem->strResult = new char[nLen];
				memset(pNode->pHToken->subStem->strResult, 0x00, nLen);
				strcpy(pNode->pHToken->subStem->strResult, pAddTwoString);

				if (pMiddleString != NULL)
				{
					m_LinkedList->CreateSubNode(pNode);
					InsertAddSubTail(pNode, pMiddleString);
				}
				else
				{
					m_LinkedList->CreateSubNode(pNode);
					InsertAddSubTail(pNode, pMiddleString);
				}

				//"��"�� �ٴ� ��翡�� [T151]���� ��
				/*09.03 ����*/
				if (strcmp(pMiddleString, "��") == 0)
				{
					if (pNode->pHToken->bSubCompCreate == true)
					{
						pSubNode = pNode->pHToken->pSubTailNode->pPrev;
						if (pSubNode != pNode->pHToken->pSubHeadNode)
						{
							if (strcmp(pSubNode->pCSubToken->strResult, "��") == 0)
							{
								pSubNode = pSubNode->pPrev;
								if (pSubNode != pNode->pHToken->pSubHeadNode)
								{
									pSubNode->pCSubToken->strFeature[S_T151] = '1';
								}
							}
						}
						else
							AddSubNodeTailFeature(pNode, S_T151);
					}
				}
			}
			else
			{
				nLen = strlen(pMiddleString);
				nLen += 1;
				InsertStemString(pNode, pMiddleString);
				SaveHTokenStem(pNode);
				ClassfyKindChar(pNode, pMiddleString);

				if (pMiddleString != NULL)
				{
					m_LinkedList->CreateSubNode(pNode);
					InsertAddSubTail(pNode, pMiddleString);
				}
				else
				{
					m_LinkedList->CreateSubNode(pNode);
					InsertAddSubTail(pNode, pMiddleString);
				}
			}
		}
		else
		{
			//���ո�� �߰� 
			CHAR_DELETE(pNode->pHToken->subStem->strResult);
			nLen = strlen(pMiddleString);
			nLen += 1;
			pNode->pHToken->subStem->strResult = new char[nLen];
			memset(pNode->pHToken->subStem->strResult, 0x00, nLen);
			strcpy(pNode->pHToken->subStem->strResult, pMiddleString);
			if (pNode->pHToken->subStem->strFeature != NULL)
			{
				memset(pNode->pHToken->subStem->strFeature, '0', STEM_DIC_ALL_FEATURE_NUMBER - 1);
			}

			SaveHTokenStem(pNode);
			ClassfyKindChar(pNode, pMiddleString);

			if (m_pOldBuffer != NULL)
			{
				m_LinkedList->CreateSubNode(pNode);
				InsertAddSubTail(pNode, pMiddleString);
			}
			else
			{
				m_LinkedList->CreateSubNode(pNode);
				InsertAddSubTail(pNode, pMiddleString);
			}

			CHAR_DELETE(m_pOldBuffer);

		}
	}
	else
	{
		nLen = strlen(m_pOldBuffer);
		nTotalLen = strlen(pOriginalString);
		nMinusLen = nTotalLen - nLen;
		pBuffer = new char[nMinusLen+1];
		memset(pBuffer, 0x00, nMinusLen+1);
		nCount = 0;
		for (nLen; nLen < nTotalLen; nLen++)
		{
			pBuffer[nCount] = pOriginalString[nLen];
			nCount += 1;
		}

		CHAR_DELETE(pNode->pHToken->strSource);

		nLen = strlen(pBuffer);
		nCount = 0;
		pNode->pHToken->strSource = new char[nLen+1];
		memset(pNode->pHToken->strSource, 0x00, nLen+1);
		strcpy(pNode->pHToken->strSource, pBuffer);

		CHAR_DELETE(m_pOldBuffer);
		CHAR_DELETE(pBuffer);
		CHAR_DELETE(pMiddleString);
		CHAR_DELETE(pMiddleString2);
		CHAR_DELETE(pOneChar);
		CHAR_DELETE(pJongChar);
		CHAR_DELETE(pChoJungChar);
		CHAR_DELETE(pAddTwoString);
		CHAR_DELETE(pAddTwoString2);
		nStart = -1;
		mStart = 0;
		GoUWR1_1(pNode, nStart, mStart);
	}

	CHAR_DELETE(pBuffer);
	CHAR_DELETE(pMiddleString);
	CHAR_DELETE(pMiddleString2);
	CHAR_DELETE(pMiddleString3);
	CHAR_DELETE(pNextOneChar);
	CHAR_DELETE(pNextOneChar2);
	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pJongChar);
	CHAR_DELETE(pChoJungChar);
	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pAddTwoString2);
}

void CNAHangulAnaly::GoSVA(H_NODE *pNode)
{
	char* pOriginalString = NULL;

	char* pOneChar = NULL;
	char* pNextOneChar = NULL;
	char* pMiddleString = NULL;
	char* pAddTwoString = NULL;

	int nStart = 0;
	unsigned int nLen = 0;
	unsigned int nCount = 0;
	unsigned int nTotalLen = 0;


	if (m_nUWRFlag != 2)
	{
		m_nUWRFlag = 1;
		return;
	}

	pOriginalString = pNode->pHToken->strSource;
	nTotalLen = strlen(pOriginalString);

	//S(1)
	pOneChar = GetUnRegisterOneChar(pOriginalString, nStart);
	//S(2)
	nStart = GetUnRegisterMoveRightCount(pOriginalString, nStart);
	pNextOneChar = GetUnRegisterOneChar(pOriginalString, nStart);

	if (strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0)
	{
		pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, nStart, nTotalLen-1, CALCULATE_STOP, CALCULATE_STOP);

		H_NODE* m_pThisNode;
		m_pThisNode = pNode;

		Insert_Str_LinkedList(pOneChar, m_pThisNode, m_pThisNode->pHToken->nOffset);
		Insert_Str_LinkedList(pMiddleString, m_pThisNode->pNext, m_pThisNode->pHToken->nOffset+strlen(pOneChar)+1);

		m_nInsertNodeCount = 2;
		m_nListPosition += 2;
		m_pThisNode->nSkip = -1;
		m_LinkedList->m_nTotalCount += 2;
	}
	else if (strcmp(pOneChar, "��") == 0 && strcmp(pNextOneChar, "��") == 0)
	{
		//S(1~2)
		pAddTwoString = AddTwoStr(pOneChar, pNextOneChar);
		//S(3)
		nStart = GetUnRegisterMoveRightCount(pOriginalString, nStart);
		pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, nStart, nTotalLen-1, CALCULATE_STOP, CALCULATE_STOP);

		H_NODE* m_pThisNode;
		m_pThisNode = pNode;

		Insert_Str_LinkedList(pAddTwoString, m_pThisNode, m_pThisNode->pHToken->nOffset);
		Insert_Str_LinkedList(pMiddleString, m_pThisNode->pNext, m_pThisNode->pHToken->nOffset+strlen(pAddTwoString)+1);

		m_nInsertNodeCount = 2;
		m_nListPosition += 2;
		m_pThisNode->nSkip = -1;
		m_LinkedList->m_nTotalCount += 2;
	}
	else
	{
		CHAR_DELETE(pOneChar);
		CHAR_DELETE(pNextOneChar);
		CHAR_DELETE(pMiddleString);
		CHAR_DELETE(pAddTwoString);
		GoSVA1_1(pNode);
	}

	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pNextOneChar);
	CHAR_DELETE(pMiddleString);
	CHAR_DELETE(pAddTwoString);
}

void CNAHangulAnaly::GoSVA1_1(H_NODE *pNode)
{
	char* pOriginalString = NULL;

	char* pOneChar = NULL;
	char* pMiddleString = NULL;
	char* pMiddleString2 = NULL;

	int nLen = 0;
	int nCount = 0;
	int nTotalLen = 0;
	int nStart = 0;
	int nLast = 0;

	bool bIsFind = false;

	pOriginalString = pNode->pHToken->strSource;
	nTotalLen = strlen(pOriginalString);
	nLast = nTotalLen;

	nStart = GetUnRegisterMoveRightCount(pOriginalString, nStart);

	if (pOriginalString[nLast - 1] & 0x80)
	{
		if (pOriginalString[nLast - 2] & 0x80)
		{
			nLast -= 3;
		}
	}
	else
	{
		nLast -= 1;
	}
	while (nStart <= nLast)
	{
		pOneChar = GetUnRegisterOneChar(pOriginalString, nStart);
		if (strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 ||
			strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "ã") == 0 || strcmp(pOneChar, "��") == 0 ||
			strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 ||
			strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 ||
			strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 ||
			strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0)

		{
			bIsFind = true;
			CHAR_DELETE(pOneChar);
			break;
		}
		else
		{
			CHAR_DELETE(pOneChar);
		}

		//n++
		UnRegisteMoveRight(pOriginalString, nStart);
	}

	if (bIsFind == true)
	{
		pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, 0, nStart - 1, CALCULATE_STOP, CALCULATE_STOP);
		pMiddleString2 = GetUnRegisterMiddleCountChar(pOriginalString, nStart, nTotalLen-1, CALCULATE_STOP, CALCULATE_STOP);

		H_NODE* m_pThisNode;
		m_pThisNode = pNode;

		Insert_Str_LinkedList(pMiddleString, m_pThisNode, m_pThisNode->pHToken->nOffset);
		Insert_Str_LinkedList(pMiddleString2, m_pThisNode->pNext, m_pThisNode->pHToken->nOffset+strlen(pMiddleString)+1);

		m_nInsertNodeCount = 2;
		m_nListPosition += 2;
		m_pThisNode->nSkip = -1;

		m_LinkedList->m_nTotalCount += 2;
	}
	else
	{
		CHAR_DELETE(pOneChar);
		CHAR_DELETE(pMiddleString);
		CHAR_DELETE(pMiddleString2);
		//Call GoSVA1_2 Function ...
		GoSVA1_2(pNode);
	}
	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pMiddleString);
	CHAR_DELETE(pMiddleString2);
}

void CNAHangulAnaly::GoSVA1_2(H_NODE *pNode)
{
	char* pOriginalString = NULL;

	char* pOneChar = NULL;
	char* pMiddleString = NULL;
	char* pMiddleLastString = NULL;
	char* pMiddleString2 = NULL;
	char* AddTwoString = NULL;
	char* pJongChar = NULL;
	char* pPrevOneChar = NULL;
	char* pMiddleString3 = NULL;

	int nLen = 0;
	int nCount = 0;
	int nTotalLen = 0;
	int nStart = 0;
	int nLast = 0;
	int nPos = 0;

	bool bIsFind = false;

	pOriginalString = pNode->pHToken->strSource;
	nTotalLen = strlen(pOriginalString);
	nLast = nTotalLen;

	nStart = GetUnRegisterMoveRightCount(pOriginalString, nStart);

	if (pOriginalString[nLast - 1] & 0x80)
	{
		if (pOriginalString[nLast - 2] & 0x80)
		{
			nLast -= 3;
		}
	}
	else
	{
		nLast -= 1;
	}
	while (nStart <= nLast)
	{
		pOneChar = GetUnRegisterOneChar(pOriginalString, nStart);
		if (strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 ||
			strcmp(pOneChar, "��") == 0)

		{
			bIsFind = true;
			CHAR_DELETE(pOneChar);
			break;
		}
		else
		{
			CHAR_DELETE(pOneChar);
		}
		//n++
		UnRegisteMoveRight(pOriginalString, nStart);
	}

	if (bIsFind == true)
	{
		nLen = nStart;
		nLen += 1;
		if (pOriginalString[nStart] & 0x80)
		{
			nLen += 1;
		}
		if ((unsigned)nLen+1 <= strlen(pOriginalString - 1))
		{
			nLen = nStart;
			nLen -= 1;
			if (pOriginalString[nStart] & 0x80)
			{
				nLen -= 1;
			}
			//S(1~n-1)
			pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, 0, nLen+1, CALCULATE_STOP, CALCULATE_STOP);
			//S(1~n)
			pMiddleString3 = GetUnRegisterMiddleCountChar(pOriginalString, 0, nLen+3, CALCULATE_STOP, CALCULATE_STOP);

			nPos = nStart;
			nPos += 1;
			if (pOriginalString[nStart] & 0x80)
			{
				nPos += 1;
			}

			CHAR_DELETE(pOneChar);

			pOneChar = GetUnRegisterOneChar(pOriginalString, nPos);
			pPrevOneChar = GetUnRegisterOneChar(pOriginalString, nStart);
			pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);

			if (pOneChar != NULL && (strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0) &&
				IsExistStemDic(pMiddleString) && IsExistFeature(pMiddleString, STEM_DIC, 4) == false)
			{
				CHAR_DELETE(AddTwoString);
				AddTwoString = AddTwoStr(pMiddleString, "����");

				InsertStemString(pNode, AddTwoString);
				SaveHTokenStem(pNode);
				SaveHTokenStemFeature(pNode, pMiddleString);
				AddHTokenStem(pNode, S_Fnc);
				DeleteHTokenStem(pNode, S_Act);
				ClassfyKindChar(pNode, AddTwoString);

				CHAR_DELETE(pMiddleString);

				nPos += 1;
				if (pOriginalString[nStart] & 0x80)
				{
					nPos += 1;
				}

				//S(n+2~len)
				pMiddleLastString = GetUnRegisterMiddleCountChar(pOriginalString, nPos, nTotalLen-1, CALCULATE_STOP, CALCULATE_STOP);

				CHAR_DELETE(AddTwoString);
				AddTwoString = AddTwoStr("��", pJongChar);
				CHAR_DELETE(pMiddleString);

				pMiddleString = AddTwoStr(AddTwoString, pMiddleLastString);

				InsertEndingString(pNode, pMiddleString);
				SaveHTokenEnding(pNode);
				AddEndingFeature(pNode, pJongChar);
				AddEndingFeature(pNode, pMiddleLastString);
			}
			if ((pPrevOneChar != NULL && (strcmp(pPrevOneChar, "��") == 0 || strcmp(pPrevOneChar, "��") == 0) && pOneChar != NULL && (strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0) &&
				IsExistStemDic(pMiddleString) && IsExistFeature(pMiddleString, STEM_DIC, 4) == false) ||
				(pPrevOneChar != NULL && strcmp(pPrevOneChar, "��") == 0 && pOneChar != NULL && (strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0) &&
					IsExistStemDic(pMiddleString) && IsExistFeature(pMiddleString, STEM_DIC, 4) == true))
			{
				pMiddleString2 = GetUnRegisterMiddleCountChar(pOriginalString, nPos, nTotalLen-1, CALCULATE_STOP, CALCULATE_STOP);

				H_NODE* m_pThisNode;
				m_pThisNode = pNode;

				Insert_Str_LinkedList(pMiddleString3, m_pThisNode, m_pThisNode->pHToken->nOffset);
				Insert_Str_LinkedList(pMiddleString2, m_pThisNode->pNext, m_pThisNode->pHToken->nOffset+strlen(pMiddleString)+1);

				m_nInsertNodeCount = 2;
				m_nListPosition += 2;
				m_pThisNode->nSkip = -1;
				m_LinkedList->m_nTotalCount += 2;
			}
			else if (pOneChar != NULL && strcmp(pJongChar, "��") == 0 &&
				IsExistStemDic(pMiddleString) && IsExistFeature(pMiddleString, STEM_DIC, 4) == false)
			{
				pMiddleString2 = GetUnRegisterMiddleCountChar(pOriginalString, nPos, nTotalLen-1, CALCULATE_STOP, CALCULATE_STOP);

				H_NODE* m_pThisNode;
				m_pThisNode = pNode;

				Insert_Str_LinkedList(pMiddleString3, m_pThisNode, m_pThisNode->pHToken->nOffset);
				Insert_Str_LinkedList(pMiddleString2, m_pThisNode->pNext, m_pThisNode->pHToken->nOffset+strlen(pMiddleString)+1);

				m_nInsertNodeCount = 2;
				m_nListPosition += 2;
				m_pThisNode->nSkip = -1;
				m_LinkedList->m_nTotalCount += 2;
			}
			else
			{
				bIsFind = false;
			}
		}
		else
		{
			bIsFind = false;
		}
	}
	//else
	if (bIsFind == false)
	{
		CHAR_DELETE(pOneChar);
		nStart = 0;
		nStart = GetUnRegisterMoveRightCount(pOriginalString, nStart);
		while (nStart <= nLast)
		{

			pOneChar = GetUnRegisterOneChar(pOriginalString, nStart);
			if (strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0)
			{
				bIsFind = true;
				CHAR_DELETE(pOneChar);
				break;
			}
			else
			{
				CHAR_DELETE(pOneChar);
			}
			//n++
			UnRegisteMoveRight(pOriginalString, nStart);
		}

		if (bIsFind == true)
		{
			nLen = nStart;
			nLen += 1;
			if (pOriginalString[nStart] & 0x80)
			{
				nLen += 1;
			}
			if ((unsigned)nLen+1 <= strlen(pOriginalString - 1))
			{
				nLen = nStart;
				nLen -= 1;
				if (pOriginalString[nStart] & 0x80)
				{
					nLen -= 1;
				}

				CHAR_DELETE(pMiddleString);

				//S(1~n-1)
				pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, 0, nLen+1, CALCULATE_STOP, CALCULATE_STOP);

				nPos = nStart;
				nPos += 1;
				if (pOriginalString[nStart] & 0x80)
				{
					nPos += 1;
				}

				CHAR_DELETE(pOneChar);
				CHAR_DELETE(pJongChar);

				pOneChar = GetUnRegisterOneChar(pOriginalString, nPos);
				pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);

				if (pOneChar != NULL && strcmp(pJongChar, "��") == 0 &&
					IsExistStemDic(pMiddleString) && IsExistFeature(pMiddleString, STEM_DIC, 4) == false)
				{
					CHAR_DELETE(pMiddleString);
					pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, 0, nStart+1, CALCULATE_STOP, CALCULATE_STOP);

					nStart += 1;
					if (pOriginalString[nStart] & 0x80)
					{
						nStart += 1;
					}

					pMiddleString2 = GetUnRegisterMiddleCountChar(pOriginalString, nStart, nTotalLen, CALCULATE_STOP, CALCULATE_STOP);

					H_NODE* m_pThisNode;
					m_pThisNode = pNode;

					Insert_Str_LinkedList(pMiddleString, m_pThisNode, m_pThisNode->pHToken->nOffset);
					Insert_Str_LinkedList(pMiddleString2, m_pThisNode->pNext, m_pThisNode->pHToken->nOffset+strlen(pMiddleString)+1);

					m_nInsertNodeCount = 2;
					m_nListPosition += 2;
					m_pThisNode->nSkip = -1;
					m_LinkedList->m_nTotalCount += 2;
				}
				else
				{
					GoUWR(pNode);
				}
			}
			else
			{
				GoUWR(pNode);
			}
		}
		else
		{
			GoUWR(pNode);
		}
	}
	/*
	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pMiddleString);
	CHAR_DELETE(pMiddleString2);
	CHAR_DELETE(pMiddleLastString);
	CHAR_DELETE(AddTwoString);
	CHAR_DELETE(pJongChar);
	*/
	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pMiddleString);
	CHAR_DELETE(pMiddleLastString);
	CHAR_DELETE(pMiddleString2);
	CHAR_DELETE(AddTwoString);
	CHAR_DELETE(pJongChar);
	CHAR_DELETE(pPrevOneChar);
	CHAR_DELETE(pMiddleString3);

}

void CNAHangulAnaly::GoSVA1_3(H_NODE *pNode)
{
	char* pOriginalString = NULL;

	char* pOneChar = NULL;
	char* pJongChar = NULL;
	char* pMiddleString = NULL;
	char* pMiddleString2 = NULL;
	char* AddTwoString = NULL;

	int nLen = 0;
	int nCount = 0;
	int nTotalLen = 0;
	int nStart = 0;
	int nLast = 0;
	int nPos = 0;

	bool bIsFind = false;

	pOriginalString = pNode->pHToken->strSource;
	nTotalLen = strlen(pOriginalString);
	nLast = nTotalLen;

	nStart = GetUnRegisterMoveRightCount(pOriginalString, nStart);

	if (pOriginalString[nLast - 1] & 0x80)
	{
		if (pOriginalString[nLast - 2] & 0x80)
		{
			nLast -= 3;
		}
	}
	else
	{
		nLast -= 1;
	}
	while (nStart <= nLast)
	{
		CHAR_DELETE(pOneChar);

		pOneChar = GetUnRegisterOneChar(pOriginalString, nStart);
		if (strcmp(pOneChar, "��") == 0)
		{
			bIsFind = true;
			break;
		}
		//n++
		UnRegisteMoveRight(pOriginalString, nStart);
	}

	if (bIsFind == true)
	{
		nLen = nStart;
		nLen += 1;
		if (pOriginalString[nStart] & 0x80)
			nLen += 1;

		if ((unsigned)nLen+1 <= strlen(pOriginalString - 1))
		{
			//S(len~n)
			pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, nStart, nTotalLen, CALCULATE_STOP, CALCULATE_STOP);
			//S(1~n)
			pMiddleString2 = GetUnRegisterMiddleCountChar(pOriginalString, 0, nStart, CALCULATE_STOP, CALCULATE_STOP);

			nPos = nStart;
			nPos += 1;
			if (pOriginalString[nStart] & 0x80)
			{
				nPos += 1;
			}

			CHAR_DELETE(pOneChar);
			CHAR_DELETE(pJongChar);

			pOneChar = GetUnRegisterOneChar(pOriginalString, nPos);
			pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);

			if (pOneChar != NULL &&
				(strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0) &&
				IsExistStemDic(pMiddleString2) && IsExistFeature(pMiddleString2, STEM_DIC, 4) == false)
			{
				nPos += 1;
				if (pOriginalString[nPos - 1] & 0x80)
				{
					nPos += 1;
				}

				CHAR_DELETE(pMiddleString2);
				//S(n+2~len)
				pMiddleString2 = GetUnRegisterMiddleCountChar(pOriginalString, nPos, nTotalLen, CALCULATE_STOP, CALCULATE_STOP);

				AddTwoString = AddTwoStr(pMiddleString, "����");

				InsertStemString(pNode, AddTwoString);
				SaveHTokenStem(pNode);
				SaveHTokenStemFeature(pNode, pMiddleString);
				AddHTokenStem(pNode, S_Fnc);
				DeleteHTokenStem(pNode, S_Act);
				ClassfyKindChar(pNode, AddTwoString);

				CHAR_DELETE(pMiddleString);
				CHAR_DELETE(AddTwoString);

				AddTwoString = AddTwoStr("��", pJongChar);
				pMiddleString = AddTwoStr(AddTwoString, pMiddleString2);

				InsertEndingString(pNode, pMiddleString);
				SaveHTokenEnding(pNode);
				AddEndingFeature(pNode, pJongChar);
				AddEndingFeature(pNode, pMiddleString2);
			}
			else if (pOneChar != NULL &&
				(strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0) &&
				IsExistStemDic(pMiddleString2) && IsExistFeature(pMiddleString2, STEM_DIC, 4) == false)
			{
				H_NODE* m_pThisNode;
				m_pThisNode = pNode;

				Insert_Str_LinkedList(pMiddleString2, m_pThisNode, m_pThisNode->pHToken->nOffset);
				Insert_Str_LinkedList(pMiddleString, m_pThisNode->pNext, m_pThisNode->pHToken->nOffset+strlen(pMiddleString)+1);

				m_nInsertNodeCount = 2;
				m_nListPosition += 2;
				m_pThisNode->nSkip = -1;
				m_LinkedList->m_nTotalCount += 2;
			}
			else
			{
				GoUWR(pNode);
				CHAR_DELETE(pOneChar);
				CHAR_DELETE(pMiddleString);
				CHAR_DELETE(pMiddleString2);
				CHAR_DELETE(AddTwoString);
			}
		}
		else
		{
			CHAR_DELETE(pOneChar);
			CHAR_DELETE(pMiddleString);
			CHAR_DELETE(pMiddleString2);
			CHAR_DELETE(AddTwoString);
			GoUWR(pNode);
		}
	}
	else
	{
		CHAR_DELETE(pOneChar);
		CHAR_DELETE(pMiddleString);
		CHAR_DELETE(pMiddleString2);
		CHAR_DELETE(AddTwoString);
		GoUWR(pNode);
	}
	/*
	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pMiddleString);
	CHAR_DELETE(pMiddleString2);
	CHAR_DELETE(AddTwoString);
	*/
	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pJongChar);
	CHAR_DELETE(pMiddleString);
	CHAR_DELETE(pMiddleString2);
	CHAR_DELETE(AddTwoString);
}


bool CNAHangulAnaly::CreateAmbiScript()
{
	bool bCreate = false;
	H_NODE* m_pNode;
	m_pNode = m_LinkedList->m_pHeadNode->pNext;
	m_nListPosition = m_LinkedList->m_nTotalCount;
	bool isStemAmbCheck = false;
	bool isEndingAmbCheck = false;
	while (m_nListPosition > 0)
	{
		isStemAmbCheck = false;
		isEndingAmbCheck = false;
		if (m_pNode->nSkip == 0 && m_pNode->pHToken != NULL)
		{
			if (m_pNode->pHToken->subStem != NULL && m_pNode->pHToken->subStem->strFeature != NULL)
			{
				if (m_pNode->pHToken->subStem->strFeature[1] == '1')
				{
					isStemAmbCheck = true;
				}
				else
				{
					isStemAmbCheck = false;
				}
			}
			if (m_pNode->pHToken->subEnding != NULL && m_pNode->pHToken->subEnding->strFeature != NULL)
			{
				if (m_pNode->pHToken->subEnding->strFeature[0] == '1')
				{
					isEndingAmbCheck = true;
				}
				else
				{
					isEndingAmbCheck = false;
				}
			}
			if (isStemAmbCheck == true && isEndingAmbCheck == true)
			{
				m_pAmbiguity->GoAmbiguityFunction(this, m_pNode, 3);
				RestorIrrigual(m_pNode);
				m_pNode = m_pNode->pNext;
				m_nListPosition -= 1;
			}
			else if (isStemAmbCheck == true && isEndingAmbCheck == false)
			{
				m_pAmbiguity->GoAmbiguityFunction(this, m_pNode, 2);
				RestorIrrigual(m_pNode);
				m_pNode = m_pNode->pNext;
				m_nListPosition -= 1;
			}
			else if (isStemAmbCheck == false && isEndingAmbCheck == true)
			{
				m_pAmbiguity->GoAmbiguityFunction(this, m_pNode, 1);
				RestorIrrigual(m_pNode);
				m_pNode = m_pNode->pNext;
				m_nListPosition -= 1;
			}
			else
			{
				RestorIrrigual(m_pNode);
				m_pNode = m_pNode->pNext;
				m_nListPosition -= 1;
			}
		}
		else
		{
			m_pNode = m_pNode->pNext;
			m_nListPosition -= 1;
		}
	}
	m_nListPosition = 0;

	return bCreate;
}

void CNAHangulAnaly::GoEND(H_NODE *pNode)
{
	char* pOriginalString = NULL;

	int nStart = 0;
	int mStart = 0;
	int nTotalLen = 0;

	pOriginalString = pNode->pHToken->strSource;
	nTotalLen = strlen(pOriginalString);
	nStart = nTotalLen;
	mStart = nTotalLen;

	CHAR_DELETE(m_pOldBuffer);

	m_bIsFirstWord = true;
	GoEND1_1(pNode, nStart, mStart);

	//��ü���ν�NER 03 
	GoNER03(pNode);
}

void CNAHangulAnaly::GoEND1_1(H_NODE *pNode, int &nStart, int &mStart)
{
	char* pOriginalString = NULL;

	char* pMiddleString = NULL;
	char* pThisStartCountChar = NULL;
	char* pOneChar = NULL;
	char* pJongChar = NULL;
	char* pThisEndCountChar = NULL;
	char* pAddTwoString = NULL;

	int nLen = 0;
	int nCount = 0;
	int i;

	pOriginalString = pNode->pHToken->strSource;

	if (m_bIsFirstWord == false)
	{
		MovingLeftPositionWord(pOriginalString, nStart);
		InitOneWordMarsk();
	}
	m_bIsFirstWord = false;

	if (nStart > 0)
	{
		pMiddleString = GetMiddleCountChar(pOriginalString, nStart, mStart, CALCULATE_STOP, CALCULATE_STOP);
		if (IsExistEndingDic(pMiddleString))
		{
			CHAR_DELETE(m_pOldBuffer);

			if (pMiddleString != NULL)
			{
				nLen = strlen(pMiddleString);
				m_pOldBuffer = new char[nLen+1];
				memset(m_pOldBuffer, 0x00, nLen+1);
				strcpy(m_pOldBuffer, pMiddleString);
			}

			CHAR_DELETE(pMiddleString);
			CHAR_DELETE(pThisStartCountChar);
			CHAR_DELETE(pOneChar);
			CHAR_DELETE(pJongChar);
			CHAR_DELETE(pThisEndCountChar);
			CHAR_DELETE(pAddTwoString);

			GoEND1_1(pNode, nStart, mStart);

		}
		else if (m_pOldBuffer == NULL)
		{
			nLen = strlen(pOriginalString);
			nLen -= 1;
			pOneChar = GetOneChar(pOriginalString, nLen);
			pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);
			if (strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 ||
				strcmp(pOneChar, "��") == 0)
			{
				nLen -= 1;
				if (pOriginalString[nLen] & 0x80)
				{
					nLen -= 1;
				}
				pThisEndCountChar = GetEndingPositionFromWord(pOriginalString, nLen);//S(len~2)
				pAddTwoString = AddTwoStr(pThisEndCountChar, pJongChar);

				CHAR_DELETE(pNode->pHToken->subStem->strResult);

				nLen = strlen(pAddTwoString);
				nLen += 1;
				pNode->pHToken->subStem->strResult = new char[nLen];
				memset(pNode->pHToken->subStem->strResult, 0x00, nLen);
				strcpy(pNode->pHToken->subStem->strResult, pAddTwoString);
				SaveHTokenStem(pNode);
				// ���� �ʱ�ȭ
				for (i = 0; i<STEM_DIC_ALL_FEATURE_NUMBER; i++)
				{
					pNode->pHToken->subStem->strFeature[i] = '0';
				}
				AddHTokenStem(pNode, 2);
				AddHTokenStem(pNode, 4);
				AddHTokenStem(pNode, 40);
				//���� N,Lex,Scmp ���������� �ش�.
				InsertEndingString(pNode, pJongChar);
				SaveHTokenEnding(pNode);
			}
			else
			{
				CHAR_DELETE(pMiddleString);
				CHAR_DELETE(pThisStartCountChar);
				CHAR_DELETE(pOneChar);
				CHAR_DELETE(pJongChar);
				CHAR_DELETE(pThisEndCountChar);
				CHAR_DELETE(pAddTwoString);

				GoEND1_1(pNode, nStart, mStart);
			}
		}
		else
		{
			InsertEndingString(pNode, m_pOldBuffer);
			MakeNewEndingFeature(pNode);
			AddEndingFeature(pNode, m_pOldBuffer);

			pThisStartCountChar = GetFromFirstToStartWord(pOriginalString, nStart);
			InsertStemString(pNode, pThisStartCountChar);
			SaveHTokenStem(pNode);
			// ���� �ʱ�ȭ
			for (i = 0; i<STEM_DIC_ALL_FEATURE_NUMBER; i++)
			{
				pNode->pHToken->subStem->strFeature[i] = '0';
			}
			AddHTokenStem(pNode, 2);
			AddHTokenStem(pNode, 4);
			AddHTokenStem(pNode, 40);
		}
	}
	else
	{
		if (pNode->pHToken->bSubCompCreate == 1)
		{
			pNode->pHToken->bSubCompCreate = 0;
			CHAR_DELETE(pNode->pHToken->subStem->strResult);
			CHAR_DELETE(pNode->pHToken->subStem->strFeature);

			InsertStemString(pNode, pNode->pHToken->strOriginalSource);
			SaveHTokenStem(pNode);
			if (pNode->pHToken->strOriginalSource != NULL)
			{
				if (pNode->pHToken->strOriginalSource[0] >= '0' &&
					pNode->pHToken->strOriginalSource[0] <= '9')
				{
					AddHTokenStem(pNode, 12);//Numb
				}
			}
			pNode->nSkip = 0;

			H_SUBNODE* pStartNode;
			H_SUBNODE* pDeleteNode;
			pStartNode = pNode->pHToken->pSubHeadNode->pNext;
			while (pStartNode != pNode->pHToken->pSubTailNode)
			{
				pDeleteNode = pStartNode;
				pStartNode = pStartNode->pNext;

				CHAR_DELETE(pDeleteNode->pCSubToken->strResult);
				CHAR_DELETE(pDeleteNode->pCSubToken->strFeature);
				CHAR_DELETE(pDeleteNode->pCSubToken);
				CHAR_DELETE(pDeleteNode);
			}
			pNode->pHToken->pSubHeadNode->pNext = NULL;//link free
			pNode->pHToken->pSubTailNode->pPrev = NULL; //link free
			if (pNode->pHToken->pSubHeadNode != NULL)
			{
				delete pNode->pHToken->pSubHeadNode;
				pNode->pHToken->pSubHeadNode = NULL;
			}
			if (pNode->pHToken->pSubTailNode != NULL)
			{
				delete pNode->pHToken->pSubTailNode;
				pNode->pHToken->pSubTailNode = NULL;
			}

			InsertStemString(pNode, pNode->pHToken->strOriginalSource);
			SaveHTokenStem(pNode);
			// ���� �ʱ�ȭ
			for (i = 0; i<STEM_DIC_ALL_FEATURE_NUMBER; i++)
			{
				pNode->pHToken->subStem->strFeature[i] = '0';
			}
			AddHTokenStem(pNode, 2);
			AddHTokenStem(pNode, 4);
			AddHTokenStem(pNode, 40);
		}
		else
		{
			CHAR_DELETE(pNode->pHToken->subStem->strResult);
			CHAR_DELETE(pNode->pHToken->subStem->strFeature);

			InsertStemString(pNode, pNode->pHToken->strOriginalSource);
			SaveHTokenStem(pNode);
			// ���� �ʱ�ȭ
			for (i = 0; i<STEM_DIC_ALL_FEATURE_NUMBER; i++)
			{
				pNode->pHToken->subStem->strFeature[i] = '0';
			}
			AddHTokenStem(pNode, 2);
			AddHTokenStem(pNode, 4);
			AddHTokenStem(pNode, 40);
			//���� N,Lex,Prp ���������� �ش�.
		}
	}

	//// ����
	if (pNode->pHToken->bSubCompCreate == 1)
	{
		pNode->pHToken->bSubCompCreate = 0;
		pNode->nSkip = 0;

		H_SUBNODE* pStartNode;
		H_SUBNODE* pDeleteNode;
		pStartNode = pNode->pHToken->pSubHeadNode->pNext;
		while (pStartNode != pNode->pHToken->pSubTailNode)
		{
			pDeleteNode = pStartNode;
			pStartNode = pStartNode->pNext;

			CHAR_DELETE(pDeleteNode->pCSubToken->strResult);
			CHAR_DELETE(pDeleteNode->pCSubToken->strFeature);
			CHAR_DELETE(pDeleteNode->pCSubToken);
			CHAR_DELETE(pDeleteNode);
		}
		pNode->pHToken->pSubHeadNode->pNext = NULL;//link free
		pNode->pHToken->pSubTailNode->pPrev = NULL; //link free
		if (pNode->pHToken->pSubHeadNode != NULL)
		{
			delete pNode->pHToken->pSubHeadNode;
			pNode->pHToken->pSubHeadNode = NULL;
		}
		if (pNode->pHToken->pSubTailNode != NULL)
		{
			delete pNode->pHToken->pSubTailNode;
			pNode->pHToken->pSubTailNode = NULL;
		}
	}

	CHAR_DELETE(pMiddleString);
	CHAR_DELETE(pThisStartCountChar);
	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pJongChar);
	CHAR_DELETE(pThisEndCountChar);
	CHAR_DELETE(pAddTwoString);
}

//��ü�� �ν�(NER 01)
bool CNAHangulAnaly::GoNER01(H_NODE* pNode)
{
	char* pOriginalString = NULL;

	char* pFirstOneChar = NULL;
	char* pFirstTwoChar = NULL;
	char* pLastOneChar = NULL;
	char* pFirst = NULL;

	int nLen = 0;
	int nCount = 0;

	bool bRet = false;

	pOriginalString = pNode->pHToken->strOriginalSource;
	nLen = strlen(pOriginalString);

	pFirstOneChar = GetOneChar(pOriginalString, nCount);//S(1)
	pLastOneChar = GetOneChar(pOriginalString, nLen);//S(len)

	pFirstTwoChar = GetFromFirstToStartWord(pOriginalString, 4);//S(1~2)

	nCount = GetNextPrevPositionWord(pOriginalString, nLen, 1, CALCULATE_MINUS);

	pFirst = GetFromFirstToStartWord(pOriginalString, nCount);

	if (nLen == 6 && IsExistNameTabe(pFirstOneChar) &&
		strstr("ȸ,��,��,��,��,ó,��,û", pLastOneChar) == 0 &&
		strstr("��,��,��,ȭ,��,��,��,��", pLastOneChar) == 0 &&
		strstr("��,��,��,��,��,��,��,��", pLastOneChar) == 0 &&
		strstr(pOriginalString, "��") == 0)
	{
		//1y
		InsertStemString(pNode, pOriginalString);
		SaveHTokenStem(pNode);
		AddHTokenStem(pNode, S_N);
		AddHTokenStem(pNode, S_Lex);
		AddHTokenStem(pNode, S_Ani);
		AddHTokenStem(pNode, S_Conc);
		AddHTokenStem(pNode, S_Hum);
		AddHTokenStem(pNode, S_Prp);
		bRet = true;
	}
	//1n
	else if ((nLen == 6 || nLen == 8) &&
		(strcmp(pFirstTwoChar, "����") == 0 || strcmp(pFirstTwoChar, "����") == 0 ||
			strcmp(pFirstTwoChar, "����") == 0 || strcmp(pFirstTwoChar, "���") == 0 ||
			strcmp(pFirstTwoChar, "����") == 0 || strcmp(pFirstTwoChar, "����") == 0 ||
			strcmp(pFirstTwoChar, "Ȳ��") == 0))
	{
		//2y
		InsertStemString(pNode, pOriginalString);
		SaveHTokenStem(pNode);
		AddHTokenStem(pNode, S_N);
		AddHTokenStem(pNode, S_Lex);
		AddHTokenStem(pNode, S_Ani);
		AddHTokenStem(pNode, S_Conc);
		AddHTokenStem(pNode, S_Hum);
		AddHTokenStem(pNode, S_Prp);
		bRet = true;
	}
	//2n
	else if ((strcmp(pLastOneChar, "��") == 0 || strcmp(pLastOneChar, "��") == 0 ||
		strcmp(pLastOneChar, "��") == 0) &&
		(IsExistFeature(pFirst, STEM_DIC, S_N) == true &&
			IsExistFeature(pFirst, STEM_DIC, S_Plc) == true))
	{
		//3y
		InsertStemString(pNode, pOriginalString);
		SaveHTokenStem(pNode);
		AddHTokenStem(pNode, S_N);
		AddHTokenStem(pNode, S_Lex);
		AddHTokenStem(pNode, S_Plc);
		AddHTokenStem(pNode, S_Prp);
		bRet = true;
	}
	//3n

	CHAR_DELETE(pFirstOneChar);
	CHAR_DELETE(pFirstTwoChar);
	CHAR_DELETE(pLastOneChar);
	CHAR_DELETE(pFirst);

	return bRet;
}

//��ü�� �ν�(NER 02)
bool CNAHangulAnaly::GoNER02(H_NODE* pNode)
{
	char* pSubNodeLastStr = NULL;
	char* pEndingStr = NULL;

	bool bRet = false;
	int nLen = 0;

	H_SUBNODE* pUnitNode = NULL;
	H_SUBNODE* pPreNode = NULL;

	if (pNode->pHToken != NULL && pNode->pHToken->bSubCompCreate == true &&
		pNode->pHToken->pSubHeadNode != NULL &&
		pNode->pHToken->pSubHeadNode->pNext != pNode->pHToken->pSubTailNode &&
		pNode->pHToken->pSubHeadNode->pNext != pNode->pHToken->pSubTailNode->pPrev)
	{
		pPreNode = pNode->pHToken->pSubTailNode->pPrev;
		pSubNodeLastStr = pNode->pHToken->pSubTailNode->pPrev->pCSubToken->strResult;
		nLen = strlen(pSubNodeLastStr);

		if (pPreNode->pPrev != pNode->pHToken->pSubHeadNode)
		{
			pPreNode = pPreNode->pPrev;
		}

		pEndingStr = pNode->pHToken->subEnding->strResult;

		if (pPreNode != NULL && pPreNode->pCSubToken != NULL && pPreNode->pCSubToken->strResult != NULL &&
			(strlen(pPreNode->pCSubToken->strResult) == 4 || strlen(pPreNode->pCSubToken->strResult) == 6) &&
			nLen == 2 && (strcmp(pSubNodeLastStr, "��") == 0 || strcmp(pSubNodeLastStr, "��") == 0))
		{
			//1y
			pUnitNode = SubNodeSectionUnity(pNode);
			pUnitNode->pCSubToken->strFeature[S_N] = '1';
			pUnitNode->pCSubToken->strFeature[S_Lex] = '1';
			pUnitNode->pCSubToken->strFeature[S_Ani] = '1';
			pUnitNode->pCSubToken->strFeature[S_Conc] = '1';
			pUnitNode->pCSubToken->strFeature[S_Hum] = '1';
			pUnitNode->pCSubToken->strFeature[S_Prp] = '1';

			bRet = true;
		}
		//1n
		else if (nLen == 2 &&
			(strcmp(pSubNodeLastStr, "ȸ") == 0 || strcmp(pSubNodeLastStr, "��") == 0 ||
				strcmp(pSubNodeLastStr, "��") == 0 || strcmp(pSubNodeLastStr, "��") == 0 ||
				strcmp(pSubNodeLastStr, "��") == 0 || strcmp(pSubNodeLastStr, "ó") == 0 ||
				strcmp(pSubNodeLastStr, "��") == 0 || strcmp(pSubNodeLastStr, "û") == 0))
		{
			//2y
			AddHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_Lex);
			AddHTokenStem(pNode, S_Org);
			AddHTokenStem(pNode, S_Prp);
			bRet = true;
		}
		//2n
		else if (nLen == 2 && (strcmp(pSubNodeLastStr, "��") == 0 || strcmp(pSubNodeLastStr, "��") == 0))
		{
			//3y
			AddHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_Lex);
			AddHTokenStem(pNode, S_Plc);
			AddHTokenStem(pNode, S_Prp);
		}
		//3n
		else if (nLen == 4 &&
			(strcmp(pSubNodeLastStr, "����") == 0 || strcmp(pSubNodeLastStr, "��ȸ") == 0 ||
				strcmp(pSubNodeLastStr, "��ȸ") == 0))
		{
			//4y
			AddHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_Lex);
			AddHTokenStem(pNode, S_Org);
			AddHTokenStem(pNode, S_Prp);
			bRet = true;
		}
		//4n
		else if (strstr(pSubNodeLastStr, "����ȸ") != 0 || strstr(pSubNodeLastStr, "ȸ�Ǽ�") != 0 ||
			strstr(pSubNodeLastStr, "��ɺ�") != 0)
		{
			//5y
			AddHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_Lex);
			AddHTokenStem(pNode, S_Org);
			AddHTokenStem(pNode, S_Prp);
			bRet = true;
		}
		//5n
		else if (pPreNode != NULL && pPreNode->pCSubToken != NULL && pPreNode->pCSubToken->strResult != NULL &&
			strlen(pPreNode->pCSubToken->strResult) == 6 &&
			pEndingStr != NULL && (strcmp(pEndingStr, "����") == 0 || strcmp(pEndingStr, "���Լ�") == 0 ||
				strcmp(pEndingStr, "���Է�") == 0 || strcmp(pEndingStr, "����") == 0 ||
				strcmp(pEndingStr, "��") == 0))
		{
			//6y
			AddHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_Lex);
			AddHTokenStem(pNode, S_Ani);
			AddHTokenStem(pNode, S_Conc);
			AddHTokenStem(pNode, S_Hum);
			AddHTokenStem(pNode, S_Prp);
			bRet = true;
		}
		//6n
		else if (pEndingStr != NULL && strcmp(pEndingStr, "��������") == 0)
		{
			//7y
			AddHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_Lex);
			AddHTokenStem(pNode, S_Plc);
			AddHTokenStem(pNode, S_Prp);
			bRet = true;
		}
		//7n
		else if (nLen == 2 && (strcmp(pSubNodeLastStr, "��") == 0 ||
			strcmp(pSubNodeLastStr, "��") == 0))
		{
			//8y
			AddHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_Lex);
			AddHTokenStem(pNode, S_Qnt);
			bRet = true;
		}
		//8n
		else if (nLen == 2 && strcmp(pSubNodeLastStr, "��") == 0 &&
			pNode->pHToken->pSubHeadNode->pNext->pCSubToken->strFeature[S_Org] == '1')
		{
			//9y
			AddHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_Lex);
			AddHTokenStem(pNode, S_Ani);
			AddHTokenStem(pNode, S_Conc);
			AddHTokenStem(pNode, S_Hum);
			bRet = true;
		}
		//9n
	}

	return bRet;
}

//��ü�� �ν�(NER 03)
void CNAHangulAnaly::GoNER03(H_NODE* pNode)
{
	char* pLastOneChar = NULL;
	char* pStemString = NULL;
	char* pStemFeature = NULL;
	char* pEndString = NULL;

	int nLen = 0;
	int i;


	if (pNode != NULL && pNode->pHToken != NULL &&
		pNode->pHToken->subStem != NULL && pNode->pHToken->subEnding != NULL &&
		pNode->pHToken->subStem->strResult != NULL && pNode->pHToken->subEnding->strResult != NULL)
	{
		pStemString = pNode->pHToken->subStem->strResult;
		pEndString = pNode->pHToken->subEnding->strResult;

		if (pNode->pHToken->subStem->strFeature != NULL)
		{
			pStemFeature = pNode->pHToken->subStem->strFeature;
		}


		nLen = strlen(pStemString);

		pLastOneChar = GetOneChar(pStemString, nLen);//S(len)

		if (pStemFeature[S_N] == '1' && pStemFeature[S_Lex] == '1' &&
			pStemFeature[S_Scmp] == '1')
		{
			//1y
			if (strcmp(pEndString, "����") == 0 || strcmp(pEndString, "���Լ�") == 0 ||
				strcmp(pEndString, "���Է�") == 0 || strcmp(pEndString, "����") == 0 ||
				strcmp(pEndString, "��") == 0)
			{
				//2y
				for (i = 0; i<STEM_DIC_ALL_FEATURE_NUMBER; i++) { pStemFeature[i] = '0'; } // ���� �ʱ�ȭ
				AddHTokenStem(pNode, S_N);
				AddHTokenStem(pNode, S_Lex);
				AddHTokenStem(pNode, S_Ani);
				AddHTokenStem(pNode, S_Conc);
				AddHTokenStem(pNode, S_Hum);
				AddHTokenStem(pNode, S_Prp);
			}
			//2n
			else if (strcmp(pEndString, "��������") == 0)
			{
				//3y
				for (i = 0; i<STEM_DIC_ALL_FEATURE_NUMBER; i++) { pStemFeature[i] = '0'; } // ���� �ʱ�ȭ
				AddHTokenStem(pNode, S_N);
				AddHTokenStem(pNode, S_Lex);
				AddHTokenStem(pNode, S_Plc);
				AddHTokenStem(pNode, S_Prp);
			}
			//3n
			else if (strcmp(pLastOneChar, "��") == 0 || strcmp(pLastOneChar, "��") == 0)
			{
				//4y
				for (i = 0; i<STEM_DIC_ALL_FEATURE_NUMBER; i++) { pStemFeature[i] = '0'; } // ���� �ʱ�ȭ
				AddHTokenStem(pNode, S_N);
				AddHTokenStem(pNode, S_Lex);
				AddHTokenStem(pNode, S_Ani);
				AddHTokenStem(pNode, S_Conc);
				AddHTokenStem(pNode, S_Hum);
				AddHTokenStem(pNode, S_Prp);
			}
			//4n
			else if (strcmp(pLastOneChar, "��") == 0 || strcmp(pLastOneChar, "��") == 0 ||
				strcmp(pLastOneChar, "��") == 0)
			{
				//5y
				for (i = 0; i<STEM_DIC_ALL_FEATURE_NUMBER; i++) { pStemFeature[i] = '0'; } // ���� �ʱ�ȭ
				AddHTokenStem(pNode, S_N);
				AddHTokenStem(pNode, S_Lex);
				AddHTokenStem(pNode, S_Plc);
				AddHTokenStem(pNode, S_Prp);
			}
			//5n
		}
	}

	CHAR_DELETE(pLastOneChar);
}

void CNAHangulAnaly::GoUnitNode(H_NODE* pNode)
{
	H_SUBNODE* pSubNode = NULL;

	int nSubNode[1024];
	int nCount = 0;
	int nLen = 0;
	bool btrue = true;

	if (pNode->pHToken != NULL && pNode->pHToken->bSubCompCreate == true &&
		pNode->pHToken->pSubHeadNode != NULL &&
		pNode->pHToken->pSubHeadNode->pNext != pNode->pHToken->pSubTailNode &&
		pNode->pHToken->pSubHeadNode->pNext != pNode->pHToken->pSubTailNode->pPrev)
	{
		pSubNode = pNode->pHToken->pSubHeadNode->pNext;
		while (pSubNode != pNode->pHToken->pSubTailNode)
		{
			if (pSubNode->pCSubToken != NULL &&
				pSubNode->pCSubToken->strResult != NULL)
			{
				if (strcmp(pSubNode->pCSubToken->strResult, "��") == 0 ||
					strcmp(pSubNode->pCSubToken->strResult, "��") == 0 ||
					strcmp(pSubNode->pCSubToken->strResult, "ȭ") == 0 ||
					strcmp(pSubNode->pCSubToken->strResult, "��") == 0)
				{
					btrue = false;
				}

				if (pSubNode->pCSubToken->nKindChar == ENGLISH_CHAR)
				{
					btrue = false;
				}

				nSubNode[nCount++] = strlen(pSubNode->pCSubToken->strResult);
				nLen += nSubNode[nCount - 1];
			}
			pSubNode = pSubNode->pNext;
		}
	}

	if (nCount >= 2 && btrue == true)
	{
		if (nSubNode[0] == 2 || nSubNode[nCount - 1] == 2)
		{
			AddHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_Lex);
		}
		else if (nSubNode[nCount - 1] == 4 && nLen == 6)
		{
			AddHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_Lex);
		}
	}
}

void CNAHangulAnaly::DeleteLinkedList()
{
}

void CNAHangulAnaly::StartHangulAnaly()
{
	H_NODE* m_pNode;
	int nTS, nTE, nNS, nNE, nComma;
	char* pFeature = NULL;

	m_nUWRFlag = 2;// ��ó�� ����(���ո��)
	m_pNode = m_LinkedList->m_pHeadNode->pNext;
	m_nListPosition = m_LinkedList->m_nTotalCount;

	m_nListCount = 1;

	while (m_pNode != m_LinkedList->m_pTailNode)
	{
		// bMarsk�� true�̸� ���¼� �м� ó�� ����(��ó������ �̹� �м���)
		if (m_pNode->nSkip == 0 && m_pNode->bMarsk == false)
		{
			nTS = 0; nTE = 0; nNS = 0; nNE = 0; nComma = 0;
			pFeature = NULL;
			if (m_pNode->pHToken != NULL 
				&& m_pNode->pHToken->subStem != NULL 
				&&m_pNode->pHToken->subStem->strFeature != NULL){
				pFeature = m_pNode->pHToken->subStem->strFeature;
				if (pFeature[211] == '1'){
					nTS = 1;
				}
				if (pFeature[212] == '1'){
					nTE = 1;
				}
				if (pFeature[213] == '1'){
					nNS = 1;
				}
				if (pFeature[214] == '1'){
					nNE = 1;
				}
				if (pFeature[215] == '1'){
					nComma = 1;
				}
			}

			#ifdef _DEBUG
				memset(m_pNode->pHToken->strFlowChartDebug, 0, 1024);
			#endif
			GoMAR1(m_pNode);//Go MAR1 Function......

			GoUnitNode(m_pNode);//���ո�� ��ó��

			pFeature = NULL;
			if (m_pNode->pHToken != NULL 
				&& m_pNode->pHToken->subStem != NULL 
				&& m_pNode->pHToken->subStem->strFeature != NULL){
				if (m_pNode->pHToken->bSubCompCreate == true){
					if (m_pNode->pHToken->pSubHeadNode != NULL 
						&& m_pNode->pHToken->pSubHeadNode->pNext != NULL 
						&& m_pNode->pHToken->pSubHeadNode->pNext != m_pNode->pHToken->pSubTailNode
						&& m_pNode->pHToken->pSubHeadNode->pNext->pCSubToken->strFeature != NULL){
						pFeature = m_pNode->pHToken->pSubHeadNode->pNext->pCSubToken->strFeature;
					}
				}
				else{
					pFeature = m_pNode->pHToken->subStem->strFeature;
				}

				if (pFeature != NULL){
					if (nTS == 1){
						pFeature[211] = '1';
					}
					if (nNS == 1){
						pFeature[213] = '1';
					}
				}
			}
			if (m_nInsertNodeCount == 2){
				if (m_pNode->pPrev != NULL){
					m_pNode = m_pNode->pPrev->pPrev;
					m_nInsertNodeCount = 0;
				}
			}
			else if (m_nInsertNodeCount == 1){
				m_pNode = m_pNode->pPrev;
				m_nInsertNodeCount = 0;
			}
			else{
				m_pNode = m_pNode->pNext;
				m_nListPosition -= 1;
				m_nListCount += 1;
			}

			pFeature = NULL;

			if (m_pNode->pPrev->pHToken != NULL 
				&& m_pNode->pPrev->pHToken->subStem != NULL 
				&& m_pNode->pPrev->pHToken->subStem->strFeature != NULL){
				if (m_pNode->pPrev->pHToken->bSubCompCreate == true){
					if (m_pNode->pPrev->pHToken->pSubTailNode != NULL 
						&& m_pNode->pPrev->pHToken->pSubTailNode->pPrev != NULL 
						&& m_pNode->pPrev->pHToken->pSubTailNode->pPrev != m_pNode->pPrev->pHToken->pSubHeadNode
						&& m_pNode->pPrev->pHToken->pSubTailNode->pPrev->pCSubToken->strFeature != NULL){
						pFeature = m_pNode->pPrev->pHToken->pSubTailNode->pPrev->pCSubToken->strFeature;
					}
				}
				else{
					pFeature = m_pNode->pPrev->pHToken->subStem->strFeature;
				}

				if (pFeature != NULL){
					if (nTE == 1){
						pFeature[212] = '1';
					}
					if (nNE == 1){
						pFeature[214] = '1';
					}
					if (nComma == 1){
						pFeature[215] = '1';
					}
				}
			}
		}
		else {//Skip�� ���
			m_pNode = m_pNode->pNext;
			m_nListPosition -= 1;
			m_nListCount += 1;
		}
	} //while
	m_nListPosition = 0;
	m_nListCount = 0;

	//******************************************************************************
	// ������ �м�
	//******************************************************************************
	CreateAmbiScript();

	//******************************************************************************
	// �������� �ε���/���Ǿ� �м�
	//******************************************************************************
}

void CNAHangulAnaly::Insert_Str_LinkedList(char *pStr, int nOffset)
{
	if (m_LinkedList != NULL)
		m_LinkedList->AddTail(pStr, nOffset);
}

void CNAHangulAnaly::Insert_Str_LinkedList(char *pStr, H_NODE *pNode, int nOffset)
{
	if (m_LinkedList != NULL)
		m_LinkedList->AddBeforeNode(pStr, pNode, nOffset);
}

void CNAHangulAnaly::Insert_Str_LinkedList(H_NODE *pNode)
{
	if (m_LinkedList != NULL)
		m_LinkedList->AddLinkTail(pNode);
}

void CNAHangulAnaly::InitRealOriginalString()
{
	CHAR_DELETE(m_pRealOriginalString);
}

int CNAHangulAnaly::IsKindCode(int high, int low)
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

int CNAHangulAnaly::GetSylCode(char *pKindCode)
{
	int syl = 0;
	char *p = pKindCode;

	syl = (*p << 8) & 0x0000FF00;
	syl |= *(p+1) & 0x000000FF;
	return syl;
}

int CNAHangulAnaly::GetHighLow(int nKindCode, int *high, int *low)
{
	int ret = 0;

	*high = (nKindCode & 0xFF00) >> 8;
	*low = (nKindCode & 0x00FF);

	ret = IsKindCode(*high, *low);
	return ret;
}

void CNAHangulAnaly::ClassfyKindChar(H_NODE *pNode, char *pString)
{
	if (pString != NULL)
	{
		unsigned int IsKindCode = 0;
		int syl, high, low;
		char OneChar[3];
		memset(OneChar, 0x00, 3);
		OneChar[0] = pString[0];
		if (OneChar[0] & 0x80)
		{
			OneChar[1] = pString[1];
			syl = GetSylCode(OneChar);
			IsKindCode = GetHighLow(syl, &high, &low);
			switch (IsKindCode)
			{
			case HANGUL_CHAR_WORD:
				pNode->pHToken->subStem->nKindChar = KOREAN_CHAR;
				break;
			case CHINESE_CHAR_WORD:
				pNode->pHToken->subStem->nKindChar = CHINESE_CHAR;
				break;
			case KATAGANA_CHAR_WORD:
			case HIRAGANA_CHAR_WORD:
				pNode->pHToken->subStem->nKindChar = JAPAN_CHAR;
				break;
			default:
				pNode->pHToken->subStem->nKindChar = ECT_CHAR;
				break;
			}
		}
		else
		{
			syl = GetSylCode(OneChar);
			IsKindCode = GetHighLow(syl, &high, &low);
			switch (IsKindCode)
			{
			case ENGLISH_CHAR_WORD:
				pNode->pHToken->subStem->nKindChar = ENGLISH_CHAR;
				break;
			case NUMBER_CHAR_WORD:
				pNode->pHToken->subStem->nKindChar = NUMBER_CHAR;
				break;
			default:
				pNode->pHToken->subStem->nKindChar = ECT_CHAR;
				break;
			}
		}
	}
}

void CNAHangulAnaly::ClassfyKindChar(H_SUBNODE* pNode, char *pString)
{
	if (pString != NULL)
	{
		unsigned int IsKindCode = 0;
		int syl, high, low;
		char OneChar[3];
		memset(OneChar, 0x00, 3);
		OneChar[0] = pString[0];
		if (OneChar[0] & 0x80)
		{
			OneChar[1] = pString[1];
			syl = GetSylCode(OneChar);
			IsKindCode = GetHighLow(syl, &high, &low);
			switch (IsKindCode)
			{
			case HANGUL_CHAR_WORD:
				pNode->pCSubToken->nKindChar = KOREAN_CHAR;
				break;
			case CHINESE_CHAR_WORD:
				pNode->pCSubToken->nKindChar = CHINESE_CHAR;
				break;
			case KATAGANA_CHAR_WORD:
			case HIRAGANA_CHAR_WORD:
				pNode->pCSubToken->nKindChar = JAPAN_CHAR;
				break;
			default:
				pNode->pCSubToken->nKindChar = ECT_CHAR;
				break;
			}
		}
		else
		{
			syl = GetSylCode(OneChar);
			IsKindCode = GetHighLow(syl, &high, &low);
			switch (IsKindCode)
			{
			case ENGLISH_CHAR_WORD:
				pNode->pCSubToken->nKindChar = ENGLISH_CHAR;
				break;
			case NUMBER_CHAR_WORD:
				pNode->pCSubToken->nKindChar = NUMBER_CHAR;
				break;
			default:
				pNode->pCSubToken->nKindChar = ECT_CHAR;
				break;
			}
		}
	}
}

void CNAHangulAnaly::RestorIrrigual(H_NODE *pNode)
{
	char* pOneChar = NULL;
	char* pStartCountChar = NULL;
	char* pChoJungChar1 = NULL;
	char* pString = NULL;
	char* pSaveString = NULL;

	int nLen = 0;
	int nCount = 0;
	int nMovePos = 0;
	int nPos = 0;

	if (pNode->pHToken->subStem->strResult != NULL)
	{
		if (pNode->pHToken->subStem->strFeature[65] == '1')
		{
			pString = pNode->pHToken->subStem->strResult;
			nLen = strlen(pString);
			pOneChar = GetOneChar(pString, nLen);
			if (strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0)
			{
				//n-1
				nCount = GetNextPrevPositionWord(pString, nLen, 1, CALCULATE_MINUS);
				if (nCount > 0)
				{
					pStartCountChar = GetFromFirstToStartWord(pString, nCount);
					nLen = strlen(pStartCountChar);
					nPos = nLen;

					CHAR_DELETE(pOneChar);

					pOneChar = GetOneChar(pStartCountChar, nLen);
					char pJongChar[3];
					char pChoJungChar[3];
					char pHapChar[3];

					memset(pJongChar, 0x00, 3);
					memset(pChoJungChar, 0x00, 3);
					memset(pHapChar, 0x00, 3);
					strcpy(pJongChar, "��");

					pChoJungChar1 = GetChojungJongsungChar(pOneChar, CHAR_CHOJUNGSUNG);
					if (pChoJungChar1 != NULL && pChoJungChar1 != "" && strlen(pChoJungChar1) > 0)
					{
						if (m_pConvert != NULL)
						{
							m_pConvert->EvaluateHapChar(pHapChar, pChoJungChar1, pJongChar);
						}
						//�ϼ��� �ڵ� ���� ���� ��� ������ ������ ó���Ѵ�.
						if (pHapChar != NULL && strlen(pHapChar) > 0)
						{
							if (CalculateWordCount(pStartCountChar, nPos, 1, CALCULATE_MINUS))//�ѱ��� ��� ó��_1
							{
								if (IsThisWordHangulAscii(pStartCountChar, nPos))
								{
									nPos -= 1;
								}
							}
							else
							{
								if (IsThisWordHangulAscii(pStartCountChar, nPos))
								{
									nPos -= 1;
								}
							}
							nLen = strlen(pStartCountChar);
							nCount = 0;
							pSaveString = new char[nLen+1];
							memset(pSaveString, 0x00, nLen+1);
							nMovePos = 1;
							while (nLen)
							{
								if (nMovePos != nPos)
								{
									pSaveString[nCount] = pStartCountChar[nCount];
									nLen -= 1;
									nCount += 1;
									nMovePos += 1;
								}
								else
								{
									pSaveString[nCount] = pHapChar[0];
									nCount += 1;
									pSaveString[nCount] = pHapChar[1];
									nCount += 1;
									nMovePos += 2;
									nLen -= 2;
								}
							}

							CHAR_DELETE(pNode->pHToken->subStem->strResult);

							nLen = strlen(pSaveString);
							pNode->pHToken->subStem->strResult = new char[nLen+1];
							memset(pNode->pHToken->subStem->strResult, 0x00, nLen+1);
							strcpy(pNode->pHToken->subStem->strResult, pSaveString);
						}
					}
				}
			}
			/*
			CHAR_DELETE(pOneChar);
			CHAR_DELETE(pStartCountChar);
			CHAR_DELETE(pChoJungChar1);
			CHAR_DELETE(pSaveString);
			*/
		}
	}
	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pStartCountChar);
	CHAR_DELETE(pChoJungChar1);
	CHAR_DELETE(pSaveString);
}

void CNAHangulAnaly::RepeatCreate()
{
	AllInitMemberVariable();
	InitRealOriginalString();

	m_nListPosition = 0;
	m_nListCount = 0;
	m_nInsertNodeCount = 0;
}

bool CNAHangulAnaly::IsExistTabel005(char *pString)
{
	bool bIsExist = false;
	int nCount = 0;
	if (pString != NULL)
	{
		for (nCount; nCount < ALL_TABLE_005; nCount++)
		{
			if (strcmp(Table005[nCount].strOneWord, pString) == 0)
			{
				bIsExist = true;
				break;
			}
		}
	}
	return bIsExist;
}

bool CNAHangulAnaly::IsExistTabel006(char *pString)
{
	bool bIsExist = false;
	int nCount = 0;
	if (pString != NULL)
	{
		for (nCount; nCount < ALL_TABLE_006; nCount++)
		{
			if (strcmp(Table006[nCount].strOneWord, pString) == 0)
			{
				bIsExist = true;
				break;
			}
		}
	}
	return bIsExist;
}

bool CNAHangulAnaly::IsExistTabel007(char *pString)
{
	bool bIsExist = false;
	int nCount = 0;
	if (pString != NULL)
	{
		for (nCount; nCount < ALL_TABLE_007; nCount++)
		{
			if (strcmp(Table007[nCount].strOneWord, pString) == 0)
			{
				bIsExist = true;
				break;
			}
		}
	}
	return bIsExist;
}

bool CNAHangulAnaly::IsExistTabel008(char *pString)
{
	bool bIsExist = false;
	int nCount = 0;
	if (pString != NULL)
	{
		for (nCount; nCount < ALL_TABLE_008; nCount++)
		{
			if (strcmp(Table008[nCount].strOneWord, pString) == 0)
			{
				bIsExist = true;
				break;
			}
		}
	}
	return bIsExist;
}

bool CNAHangulAnaly::IsExistTabel009(char *pString)
{
	bool bIsExist = false;
	int nCount = 0;
	if (pString != NULL)
	{
		for (nCount; nCount < ALL_TABLE_009; nCount++)
		{
			if (strcmp(Table009[nCount].strOneWord, pString) == 0)
			{
				bIsExist = true;
				break;
			}
		}
	}
	return bIsExist;
}

bool CNAHangulAnaly::IsExistNameTabe(char *pString)
{
	bool bIsExist = false;
	int nCount = 0;
	if (pString != NULL)
	{
		for (nCount; nCount < NAME_TABLE; nCount++)
		{
			if (strcmp(NAMETable[nCount].strOneWord, pString) == 0)
			{
				bIsExist = true;
				break;
			}
		}
	}
	return bIsExist;
}


bool CNAHangulAnaly::IsAalphabet(char *pOneString)
{
	bool isbtrue = false;
	if (pOneString != NULL)
	{
		if ((*pOneString >= 'a' && *pOneString <= 'z') ||
			(*pOneString >= 'A' && *pOneString <= 'Z'))
			isbtrue = true;
	}

	return isbtrue;
}

void CNAHangulAnaly::GoPFM(H_NODE *pNode, int &nStart, int &mStart)
{
	char* pOriginalString = NULL;

	char* pMiddleString = NULL;
	char* pNextMiddleString1 = NULL;
	char* pNextMiddleString2 = NULL;
	char* pNextMiddleString3 = NULL;
	char* pNextMiddleString4 = NULL;
	char* pOneChar = NULL;
	char* pAddTwoString = NULL;
	char* pAddTwoString2 = NULL;

	int nTotalLen = 0;
	int nLen = 0;
	int nCount = 0;
	int nStart1 = 0;
	int nNextPos1 = 0;
	int nNextPos2 = 0;
	int nNextPos3 = 0;

	pOriginalString = pNode->pHToken->strSource;

	nTotalLen = strlen(pOriginalString);

	pOneChar = GetUnRegisterOneChar(pOriginalString, nStart1);

	//S(1)
	nNextPos1 = GetUnRegisterMoveRightCount(pOriginalString, nStart1);
	//S(2)
	nNextPos2 = GetUnRegisterMoveRightCount(pOriginalString, nNextPos1);

	if (nNextPos1 == nTotalLen)
		nNextPos1 -= 1;
	//S(1~2)
	if (nNextPos1+1 < nTotalLen)
	{
		pNextMiddleString1 = GetUnRegisterMiddleCountChar(pOriginalString, nStart1, nNextPos1+1, CALCULATE_STOP, CALCULATE_STOP);
	}
	else
	{
		pNextMiddleString1 = new char[2];
		memset(pNextMiddleString1, 0x00, 2);
		strcpy(pNextMiddleString1, "");
	}

	//S(1~3)
	if (nNextPos2 == nTotalLen)
	{
		nNextPos2 -= 1;
	}
	if (nNextPos1+2 < nTotalLen)
	{
		//S(1~3)
		pNextMiddleString2 = GetUnRegisterMiddleCountChar(pOriginalString, nStart1, nNextPos2+1, CALCULATE_STOP, CALCULATE_STOP);
		//S(2~3)
		pNextMiddleString3 = GetUnRegisterMiddleCountChar(pOriginalString, nNextPos1, nNextPos2+1, CALCULATE_STOP, CALCULATE_STOP);
	}
	else
	{
		pNextMiddleString2 = new char[2];
		memset(pNextMiddleString2, 0x00, 2);
		strcpy(pNextMiddleString2, "");
		pNextMiddleString3 = new char[2];
		memset(pNextMiddleString3, 0x00, 2);
		strcpy(pNextMiddleString3, "");
	}

	if (IsExistStemDic(pNextMiddleString1))
	{
		if (IsExistStemDic(pNextMiddleString2))
		{
			nStart = -1;
			mStart = 0;
			GoUWR1_1(pNode, nStart, mStart);
		}
		else if (IsExistStemDic(pNextMiddleString3))
		{
			nNextPos3 = GetUnRegisterMoveRightCount(pOriginalString, nNextPos2);
			if (nNextPos3 == nTotalLen)
				nNextPos3 -= 1;
			//S(3~4)
			if (nNextPos3+1 < nTotalLen)
				pNextMiddleString4 = GetUnRegisterMiddleCountChar(pOriginalString, nNextPos2, nNextPos3+1, CALCULATE_STOP, CALCULATE_STOP);
			else
			{
				pNextMiddleString4 = new char[2];
				memset(pNextMiddleString4, 0x00, 2);
				strcpy(pNextMiddleString4, "");
			}
			if (IsExistStemDic(pNextMiddleString4))
			{
				nStart = -1;
				mStart = 0;
				GoUWR1_1(pNode, nStart, mStart);
			}
			else
			{
				pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, nNextPos1, nTotalLen-1, CALCULATE_STOP, CALCULATE_STOP);
				InsertStemString(pNode, pOneChar);
				SaveHTokenStem(pNode);
				ClassfyKindChar(pNode, pOneChar);

				if (pOneChar != NULL)
				{
					m_LinkedList->CreateSubNode(pNode);
					InsertAddSubTail(pNode, pOneChar);
				}
				else
				{
					m_LinkedList->CreateSubNode(pNode);
					InsertAddSubTail(pNode, pOneChar);
				}

				nLen = strlen(pMiddleString);

				CHAR_DELETE(pNode->pHToken->strSource);

				pNode->pHToken->strSource = new char[nLen+1];
				memset(pNode->pHToken->strSource, 0x00, nLen+1);
				strcpy(pNode->pHToken->strSource, pMiddleString);

				CHAR_DELETE(pAddTwoString);
				CHAR_DELETE(pAddTwoString2);
				CHAR_DELETE(pOneChar);
				CHAR_DELETE(pMiddleString);
				CHAR_DELETE(pNextMiddleString1);
				CHAR_DELETE(pNextMiddleString2);
				CHAR_DELETE(pNextMiddleString3);
				CHAR_DELETE(pNextMiddleString4);

				nStart = -1;
				mStart = 0;
				GoUWR1_1(pNode, nStart, mStart);
			}
		}
		else
		{
			CHAR_DELETE(pAddTwoString);
			CHAR_DELETE(pAddTwoString2);
			CHAR_DELETE(pOneChar);
			CHAR_DELETE(pMiddleString);
			CHAR_DELETE(pNextMiddleString1);
			CHAR_DELETE(pNextMiddleString2);
			CHAR_DELETE(pNextMiddleString3);
			CHAR_DELETE(pNextMiddleString4);

			nStart = -1;
			mStart = 0;
			GoUWR1_1(pNode, nStart, mStart);
		}
	}
	else if (IsExistStemDic(pNextMiddleString2))
	{
		CHAR_DELETE(pAddTwoString);
		CHAR_DELETE(pAddTwoString2);
		CHAR_DELETE(pOneChar);
		CHAR_DELETE(pMiddleString);
		CHAR_DELETE(pNextMiddleString1);
		CHAR_DELETE(pNextMiddleString2);
		CHAR_DELETE(pNextMiddleString3);
		CHAR_DELETE(pNextMiddleString4);

		nStart = -1;
		mStart = 0;
		GoUWR1_1(pNode, nStart, mStart);
	}
	else
	{
		pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, nNextPos1, nTotalLen-1, CALCULATE_STOP, CALCULATE_STOP);
		InsertStemString(pNode, pOneChar);
		SaveHTokenStem(pNode);
		ClassfyKindChar(pNode, pOneChar);

		if (pOneChar != NULL)
		{
			m_LinkedList->CreateSubNode(pNode);
			InsertAddSubTail(pNode, pOneChar/*m_pOldBuffer*/);
		}
		else
		{
			m_LinkedList->CreateSubNode(pNode);
			InsertAddSubTail(pNode, pOneChar);
		}

		nLen = strlen(pMiddleString);

		CHAR_DELETE(pNode->pHToken->strSource);

		pNode->pHToken->strSource = new char[nLen+1];
		memset(pNode->pHToken->strSource, 0x00, nLen+1);
		strcpy(pNode->pHToken->strSource, pMiddleString);

		CHAR_DELETE(pAddTwoString);
		CHAR_DELETE(pAddTwoString2);
		CHAR_DELETE(pOneChar);
		CHAR_DELETE(pMiddleString);
		CHAR_DELETE(pNextMiddleString1);
		CHAR_DELETE(pNextMiddleString2);
		CHAR_DELETE(pNextMiddleString3);
		CHAR_DELETE(pNextMiddleString4);

		nStart = -1;
		mStart = 0;
		GoUWR1_1(pNode, nStart, mStart);
	}

	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pAddTwoString2);
	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pMiddleString);
	CHAR_DELETE(pNextMiddleString1);
	CHAR_DELETE(pNextMiddleString2);
	CHAR_DELETE(pNextMiddleString3);
	CHAR_DELETE(pNextMiddleString4);
}

bool CNAHangulAnaly::IsNextOneCharSpecial(char *pOneChar)
{
	bool IsSpecial = false;
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
		strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "%") == 0 || strcmp(pOneChar, "$") == 0 || strcmp(pOneChar, "��") == 0)
		IsSpecial = true;
	return IsSpecial;

}

void CNAHangulAnaly::ReplaceSubNodeTailString(H_NODE *pNode, char* pStem)
{
	H_SUBNODE* pSubNode = NULL;
	int nLen = 0;
	int nCount = 0;

	if (pStem != NULL && pNode != NULL)
	{
		if (pNode->pHToken != NULL && pNode->pHToken->bSubCompCreate == true)
		{
			pSubNode = pNode->pHToken->pSubTailNode->pPrev;
			if (pSubNode != NULL)
			{
				CHAR_DELETE(pSubNode->pCSubToken->strResult);
				nLen = strlen(pStem);
				pSubNode->pCSubToken->strResult = new char[nLen+1];
				memset(pSubNode->pCSubToken->strResult, 0x00, nLen+1);

				while (nLen)
				{
					pSubNode->pCSubToken->strResult[nCount] = pStem[nCount];
					nLen -= 1;
					nCount += 1;
				}
				ClassfyKindChar(pSubNode, pStem);
			}
		}
	}
}

void CNAHangulAnaly::AddSubNodeTailFeature(H_NODE *pNode, char* pStem)
{
	char IS_CEHCK_FEATURE = '1';
	char* pFeature = NULL;
	H_SUBNODE* pSubNode = NULL;

	if (IsExistStemDic(pStem) == true)
	{
		if (pNode->pHToken != NULL && pNode->pHToken->bSubCompCreate == true)
		{
			pSubNode = pNode->pHToken->pSubTailNode->pPrev;
			if (pSubNode != NULL)
			{
				pFeature = pSubNode->pCSubToken->strFeature;
				if (pFeature == NULL)
				{
					pFeature = new char[STEM_DIC_ALL_FEATURE_NUMBER];
					memset(pFeature, 0x00, STEM_DIC_ALL_FEATURE_NUMBER);
					for (int Feature = 0; Feature < STEM_DIC_ALL_FEATURE_NUMBER; Feature++)
					{
						pFeature[Feature] = '0';
					}
				}

				IsExistStemDic(pStem, pFeature);
			}
		}
	}
}

void CNAHangulAnaly::AddSubNodeTailFeature(H_NODE *pNode, int nCase)
{
	char* pFeature = NULL;
	H_SUBNODE* pSubNode = NULL;

	if (pNode->pHToken != NULL && pNode->pHToken->bSubCompCreate == true)
	{
		pSubNode = pNode->pHToken->pSubTailNode->pPrev;
		if (pSubNode != NULL)
		{
			pFeature = pSubNode->pCSubToken->strFeature;
			if (pFeature == NULL)
			{
				pFeature = new char[STEM_DIC_ALL_FEATURE_NUMBER];
				memset(pFeature, 0x00, STEM_DIC_ALL_FEATURE_NUMBER);
				for (int Feature = 0; Feature < STEM_DIC_ALL_FEATURE_NUMBER; Feature++)
				{
					pFeature[Feature] = '0';
				}
			}

			pFeature[nCase] = '1';
		}
	}
}

void CNAHangulAnaly::DeleteAllSubNode(H_NODE *pNode)
{
	H_SUBNODE* pStartNode = NULL;
	H_SUBNODE* pDeleteNode = NULL;

	if (pNode != NULL && pNode->pHToken != NULL && pNode->pHToken->bSubCompCreate == true)
	{
		pNode->pHToken->bSubCompCreate = 0;
		pStartNode = pNode->pHToken->pSubHeadNode->pNext;
		while (pStartNode != pNode->pHToken->pSubTailNode)
		{
			pDeleteNode = pStartNode;
			pStartNode = pStartNode->pNext;

			CHAR_DELETE(pDeleteNode->pCSubToken->strResult);
			CHAR_DELETE(pDeleteNode->pCSubToken->strFeature);
			CHAR_DELETE(pDeleteNode->pCSubToken);
			CHAR_DELETE(pDeleteNode);
		}
		pNode->pHToken->pSubHeadNode->pNext = NULL;//link free
		pNode->pHToken->pSubTailNode->pPrev = NULL; //link free
		if (pNode->pHToken->pSubHeadNode != NULL)
		{
			delete pNode->pHToken->pSubHeadNode;
			pNode->pHToken->pSubHeadNode = NULL;
		}
		if (pNode->pHToken->pSubTailNode != NULL)
		{
			delete pNode->pHToken->pSubTailNode;
			pNode->pHToken->pSubTailNode = NULL;
		}
	}
}

void CNAHangulAnaly::DeleteSubNodeTail(H_NODE *pNode)
{
	H_SUBNODE* pStartNode = NULL;
	H_SUBNODE* pDeleteNode = NULL;

	if (pNode != NULL && pNode->pHToken != NULL && pNode->pHToken->bSubCompCreate == 1)
	{
		pStartNode = pNode->pHToken->pSubTailNode->pPrev;
		if (pStartNode != pNode->pHToken->pSubHeadNode)
		{
			pDeleteNode = pStartNode;
			pStartNode = pStartNode->pPrev;

			pNode->pHToken->pSubTailNode->pPrev = pStartNode;
			pStartNode->pNext = pNode->pHToken->pSubTailNode;

			CHAR_DELETE(pDeleteNode->pCSubToken->strResult);
			CHAR_DELETE(pDeleteNode->pCSubToken->strFeature);
			CHAR_DELETE(pDeleteNode->pCSubToken);
			CHAR_DELETE(pDeleteNode);
		}
	}
}

int CNAHangulAnaly::SubNodeCount(H_NODE *pNode)
{
	int nRet = 0;
	H_SUBNODE* pStartNode = NULL;

	if (pNode != NULL && pNode->pHToken != NULL && pNode->pHToken->bSubCompCreate == 1)
	{
		pStartNode = pNode->pHToken->pSubHeadNode->pNext;
		while (pStartNode != pNode->pHToken->pSubTailNode)
		{
			nRet++;
			pStartNode = pStartNode->pNext;
		}
	}
	return nRet;
}

void CNAHangulAnaly::DeleteSubNodeTailFeature(H_NODE *pNode, int nCase)
{
	char* pFeature = NULL;
	H_SUBNODE* pSubNode = NULL;

	if (pNode->pHToken != NULL && pNode->pHToken->bSubCompCreate == true)
	{
		pSubNode = pNode->pHToken->pSubTailNode->pPrev;
		if (pSubNode != NULL)
		{
			pFeature = pSubNode->pCSubToken->strFeature;
			if (pFeature != NULL)
			{
				pFeature[nCase] = '0';
			}
		}
	}
}


/*************************************************************************************
//����URL
//http://v.media.daum.net/v/20170814155436552?rcmd=rn
**************************************************************************************/