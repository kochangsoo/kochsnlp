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

	m_pStemDicWord = NULL;//스템사전 액섹스 단어
	m_pStemDicDeleteJongWord = NULL;//스템사전 액섹스 단어(종성제거된 어절)
	m_pEndingDicWord = NULL;//엔딩사전 액섹스 단어
	m_pEndingDicDeleteJongWord = NULL;//엔딩전 액섹스 단어(종성제거된 어절)

	m_pOneNPluseNumChar = NULL;//n+1번째의 단어
	m_pOneNMinusNumChar = NULL;//n-1번째의 단어
	m_pNthOneChar = NULL;//N번째 해당 단어
	m_pJongChar = NULL;
	m_pChoJungChar = NULL;
	m_nPluseSyllable = 0;//nStart 기준으로 +1(우->좌) 위치에 해당하는 음절의 값
	m_nMinusSyllable = 0;//nStart 기준으로 -1(우->좌) 위치에 해당하는 음절의 값

	m_stemFeature01[STEM_DIC_ALL_FEATURE_NUMBER+1] = {0,};//스템자질
	m_stemFeature02[STEM_DIC_ALL_FEATURE_NUMBER + 1] = { 0, };//스템자질
	m_stemFeature03[STEM_DIC_ALL_FEATURE_NUMBER + 1] = { 0, };//스템자질
	m_endingFeature01[ENDING_DIC_BASIC_FEATURE_NUMBER + 1] = { 0, };//엔딩자질
	m_endingFeature02[ENDING_DIC_BASIC_FEATURE_NUMBER + 1] = { 0, };//엔딩자질
	m_endingFeature03[ENDING_DIC_BASIC_FEATURE_NUMBER + 1] = { 0, };//엔딩자질

	IS_CEHCK_FEATURE = '1';

	//문자열 결합
	m_p1thAddString = NULL;
	m_p2thAddString = NULL;
	m_p3thAddString = NULL;

	m_nRestorationJongChar = NULL;//N번째 종성변경 전의 원래 종성값
	m_nLen = 0;//문자열길이
	m_pLastOneChar = NULL;//S(1)의 문자
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

//형태소분석 함수에 사용되는 변수 초기화
void CNAHangulAnaly::IntitFunctionUsingValue()
{
	CHAR_DELETE(m_pFormLenToNWord);//S(len~n)
	CHAR_DELETE(m_pFormNTo1Word);//S(n~1)
	CHAR_DELETE(m_pFormNPluseNumTo1Word);//S(n+1~1)

	CHAR_DELETE(m_pStemDicWord);//스템사전 액섹스 단어
	CHAR_DELETE(m_pStemDicDeleteJongWord);//스템사전 액섹스 단어(종성제거된 어절)
	CHAR_DELETE(m_pEndingDicWord);//엔딩사전 액섹스 단어
	CHAR_DELETE(m_pEndingDicDeleteJongWord);//엔딩사전 액섹스 단어(종성제거된 어절)

	CHAR_DELETE(m_pOneNPluseNumChar);//n+1번째의 단어
	CHAR_DELETE(m_pOneNMinusNumChar);//n-1번째의 단어
	CHAR_DELETE(m_pNthOneChar);//N번째 해당 단어
	CHAR_DELETE(m_pJongChar);
	CHAR_DELETE(m_pChoJungChar);
	m_nPluseSyllable = 0;//nStart 기준으로 +1(우->좌) 위치에 해당하는 음절의 값
	m_nMinusSyllable = 0;//nStart 기준으로 -1(우->좌) 위치에 해당하는 음절의 값

	//스템자질/엔딩자질
	memset(m_stemFeature01, 0x00, sizeof(m_stemFeature01));
	memset(m_stemFeature02, 0x00, sizeof(m_stemFeature02));
	memset(m_stemFeature03, 0x00, sizeof(m_stemFeature03));

	memset(m_endingFeature01, 0x00, sizeof(m_endingFeature01));
	memset(m_endingFeature02, 0x00, sizeof(m_endingFeature02));
	memset(m_endingFeature03, 0x00, sizeof(m_endingFeature03));


	//문자열 결합
	CHAR_DELETE(m_p1thAddString);
	CHAR_DELETE(m_p2thAddString);
	CHAR_DELETE(m_p3thAddString);

	m_nRestorationJongChar =  -1;//N번째 종성변경 전의 원래 종성값
	m_nLen = 0;//문자열길이
	CHAR_DELETE(m_pLastOneChar);//S(1)의 문자
	CHAR_DELETE(m_pAddTwoString);
	CHAR_DELETE(m_pMinusTwoString);
	
}

int CNAHangulAnaly::GetWordFinalConsonant(char* pWord){
	int nHangulJa = 14;
	if(pWord != NULL)
	{
		if(strcmp(pWord,"ㄱ") == 0)  nHangulJa = REPLACE_HANGUL_JA00;
		else if (strcmp(pWord, "ㄴ") == 0) nHangulJa = REPLACE_HANGUL_JA01;
		else if (strcmp(pWord, "ㄷ") == 0) nHangulJa = REPLACE_HANGUL_JA02;
		else if (strcmp(pWord, "ㄹ") == 0) nHangulJa = REPLACE_HANGUL_JA03;
		else if (strcmp(pWord, "ㅁ") == 0) nHangulJa = REPLACE_HANGUL_JA04;
		else if (strcmp(pWord, "ㅂ") == 0) nHangulJa = REPLACE_HANGUL_JA05;
		else if (strcmp(pWord, "ㅅ") == 0) nHangulJa = REPLACE_HANGUL_JA06;
		else if (strcmp(pWord, "ㅇ") == 0) nHangulJa = REPLACE_HANGUL_JA07;
		else if (strcmp(pWord, "ㅈ") == 0) nHangulJa = REPLACE_HANGUL_JA08;
		else if (strcmp(pWord, "ㅊ") == 0) nHangulJa = REPLACE_HANGUL_JA09;
		else if (strcmp(pWord, "ㅋ") == 0) nHangulJa = REPLACE_HANGUL_JA10;
		else if (strcmp(pWord, "ㅌ") == 0) nHangulJa = REPLACE_HANGUL_JA11;
		else if (strcmp(pWord, "ㅍ") == 0) nHangulJa = REPLACE_HANGUL_JA12;
		else if (strcmp(pWord, "ㅎ") == 0) nHangulJa = REPLACE_HANGUL_JA13;
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
			case HANGUL_CHAR_WORD:		// 한글
			case CHINESE_CHAR_WORD:		// 한자  
			case KATAGANA_CHAR_WORD:	// 가카가나 
			case HIRAGANA_CHAR_WORD:	// 히라가나
			case SPECIAL_CHAR_WORD:		// 특수문자
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
				nStartPos = nPos - 1;//시작위치
				OneChar[0] = pString[nPos - 1];
			}
		}
		else
		{
			nStartPos = nPos - 1;//시작위치
			OneChar[0] = pString[nPos - 1];
		}
		
		syl = GetSylCode(OneChar);
		IsKindCode = GetHighLow(syl, &high, &low);
		switch (IsKindCode)
		{
			case HANGUL_CHAR_WORD:		// 한글
			case CHINESE_CHAR_WORD:		// 한자  
			case KATAGANA_CHAR_WORD:	// 가카가나 
			case HIRAGANA_CHAR_WORD:	// 히라가나
			case SPECIAL_CHAR_WORD:		// 특수문자
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

			if (strcmp(pDeleteNode->pCSubToken->strResult, "씨") == 0 || strcmp(pDeleteNode->pCSubToken->strResult, "님") == 0)
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
			//2017-08-15 Node 삭제 문장 추가
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


			// 자질 초기화
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


		// 초중성이 있으면
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

	if (strcmp(pJongChar, "ㄱ") == 0)
		nReturnValue = REPLACE_HANGUL_JA00;
	if (strcmp(pJongChar, "ㄴ") == 0)
		nReturnValue = REPLACE_HANGUL_JA01;
	if (strcmp(pJongChar, "ㄷ") == 0)
		nReturnValue = REPLACE_HANGUL_JA02;
	if (strcmp(pJongChar, "ㄹ") == 0)
		nReturnValue = REPLACE_HANGUL_JA03;
	if (strcmp(pJongChar, "ㅁ") == 0)
		nReturnValue = REPLACE_HANGUL_JA04;
	if (strcmp(pJongChar, "ㅂ") == 0)
		nReturnValue = REPLACE_HANGUL_JA05;
	if (strcmp(pJongChar, "ㅅ") == 0)
		nReturnValue = REPLACE_HANGUL_JA06;
	if (strcmp(pJongChar, "ㅇ") == 0)
		nReturnValue = REPLACE_HANGUL_JA07;
	if (strcmp(pJongChar, "ㅈ") == 0)
		nReturnValue = REPLACE_HANGUL_JA08;
	if (strcmp(pJongChar, "ㅊ") == 0)
		nReturnValue = REPLACE_HANGUL_JA09;
	if (strcmp(pJongChar, "ㅋ") == 0)
		nReturnValue = REPLACE_HANGUL_JA10;
	if (strcmp(pJongChar, "ㅌ") == 0)
		nReturnValue = REPLACE_HANGUL_JA11;
	if (strcmp(pJongChar, "ㅍ") == 0)
		nReturnValue = REPLACE_HANGUL_JA12;
	if (strcmp(pJongChar, "ㅎ") == 0)
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
		strcpy(pStr, "ㄱ");
		break;
	case REPLACE_HANGUL_JA01:
		strcpy(pStr, "ㄴ");
		break;
	case REPLACE_HANGUL_JA02:
		strcpy(pStr, "ㄷ");
		break;
	case REPLACE_HANGUL_JA03:
		strcpy(pStr, "ㄹ");
		break;
	case REPLACE_HANGUL_JA04:
		strcpy(pStr, "ㅁ");
		break;
	case REPLACE_HANGUL_JA05:
		strcpy(pStr, "ㅂ");
		break;
	case REPLACE_HANGUL_JA06:
		strcpy(pStr, "ㅅ");
		break;
	case REPLACE_HANGUL_JA07:
		strcpy(pStr, "ㅇ");
		break;
	case REPLACE_HANGUL_JA08:
		strcpy(pStr, "ㅈ");
		break;
	case REPLACE_HANGUL_JA09:
		strcpy(pStr, "ㅊ");
		break;
	case REPLACE_HANGUL_JA10:
		strcpy(pStr, "ㅋ");
		break;
	case REPLACE_HANGUL_JA11:
		strcpy(pStr, "ㅌ");
		break;
	case REPLACE_HANGUL_JA12:
		strcpy(pStr, "ㅍ");
		break;
	case REPLACE_HANGUL_JA13:
		strcpy(pStr, "ㅎ");
		break;
	default://종성삭제
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
			case HANGUL_CHAR_WORD:		// 한글
			case CHINESE_CHAR_WORD:		// 한자  
			case KATAGANA_CHAR_WORD:	// 가카가나 
			case HIRAGANA_CHAR_WORD:	// 히라가나
			case SPECIAL_CHAR_WORD:		// 특수문자
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
	
	//예외상황철리
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

	//에서는 -> 는 -> 서는 -> 에+서는 
	//으신 -> ㄴ+시+으 
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

				//에서는 -> 는 -> 서는 -> 에+서는 
				//으신 -> ㄴ+시+으 
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
			case HANGUL_CHAR_WORD:		// 한글
			case CHINESE_CHAR_WORD:		// 한자  
			case KATAGANA_CHAR_WORD:	// 가카가나 
			case HIRAGANA_CHAR_WORD:	// 히라가나
			case SPECIAL_CHAR_WORD:		// 특수문자
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

	IntitFunctionUsingValue();//형태소 분석에 사용되는 변수 초기화
	//미등록어 처리 함수
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
	IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
	AllInitMemberVariable();
	nStart = strlen(pNode->pHToken->strSource);
	LoggingFunction(pNode, "MAR1_00000+ ");
	if (IsExistStemDic(pNode->pHToken->strSource, m_stemFeature01))
	{//S(len~1)이 stem 사전에 있는가? /*전체 어절이 사전에 있을 경우, 기존 처리 그대로 MAR1_00000
		LoggingFunction(pNode, "MAR1_00001+ ");
		if (m_stemFeature01[STEM_DIC_V] == IS_CEHCK_FEATURE)
		{//1y : 그 stem이[v]인가 ? MAR1_00001
			LoggingFunction(pNode, "MAR1_00002+ ");
			m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));
			if (m_pNthOneChar != NULL && strcmp(m_pNthOneChar, "이") == 0)
			{//2y : S(1)이 “이”인가 ? MAR1_00002
				LoggingFunction(pNode, "MAR1_00003+ ");

				//3y : ①로 MAR1_00003
				GoMAR1_1(pNode, nStart, mStart, lStart);
			}
			else 
			{
				LoggingFunction(pNode, "MAR1_00004+ ");
				m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);
				if (strcmp(m_pJongChar, "ㄹ") == 0)
				{//3n : C(1)이 “ㄹ”인가 ? MAR1_00004
					LoggingFunction(pNode, "MAR1_00005+ ");
					//4y : stem = S(len~1), ending = “ㄹ”+[v][aff][unc][fut]  MAR1_00005
					m_pFormNTo1Word = GetFromFirstToStartWord(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));
					InsertStemString(pNode, m_pFormNTo1Word);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, "ㄹ");
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, ENDING_DIC_V);
					AddHTokenEnding(pNode, ENDING_DIC_AFF);
					AddHTokenEnding(pNode, ENDING_DIC_UNC);
					AddHTokenEnding(pNode, ENDING_DIC_FUT);
				}
				else
				{
					LoggingFunction(pNode, "MAR1_00006+ ");
					//4n : stem = S(len~1), ending = “어”+[v][aff][con]   MAR1_00006
					m_pFormNTo1Word = GetFromFirstToStartWord(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));
					InsertStemString(pNode, m_pFormNTo1Word);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, "어");
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

	IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
	AllInitMemberVariable();
	InitRealOriginalString();

	
}

void CNAHangulAnaly::GoMAR1_1(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화

	//1n : ① n++ → n > len ? MAR1_1_00001
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
		if((m_bOneCharMask == false || strcmp(m_pNthOneChar, "럼") != 0)//1'n : S(n)이 [mask]||“럼”이 아니고, {S(n) == txx  ||“기”||“답”||“럽”|| C(n) == “ㅆ”||“ㅁ”||“ㄻ”}인가? MAR1_1_00003
			&&(CompareVariousTable01(m_pNthOneChar, 0, false) == true
				|| CompareVariousTable02(m_pNthOneChar, 1, false) == true || CompareVariousTable02(m_pNthOneChar, 2, false) == true
				|| CompareVariousTable03(m_pNthOneChar, 0, false) == true || CompareVariousTable03(m_pNthOneChar, 1, false) == true
				|| CompareVariousTable04(m_pNthOneChar, 1, false) == true || CompareVariousTable04(m_pNthOneChar, 2, false) == true || CompareVariousTable04(m_pNthOneChar, 3, false) == true
				|| CompareVariousTable05(m_pNthOneChar, ALL_TABLE_05, true) == true
				|| CompareVariousTable06(m_pNthOneChar, ALL_TABLE_06, true) == true
				|| CompareVariousTable07(m_pNthOneChar, ALL_TABLE_07, true) == true
				|| strcmp(m_pNthOneChar, "기") == 0 || strcmp(m_pNthOneChar, "답") == 0 || strcmp(m_pNthOneChar, "럽") == 0
				|| strcmp(m_pJongChar, "ㅆ") == 0 || strcmp(m_pJongChar, "ㅁ") == 0 || strcmp(m_pJongChar, "ㄻ") == 0))
		{
			LoggingFunction(pNode, "MAR1_1_00004+ ");
			if (strcmp(m_pNthOneChar, "다") == 0 || strcmp(m_pNthOneChar, "야") == 0 || strcmp(m_pNthOneChar, "여") == 0 || strcmp(m_pNthOneChar, "이") == 0
				|| strcmp(m_pNthOneChar, "가") == 0 || strcmp(m_pNthOneChar, "터") == 0 || strcmp(m_pNthOneChar, "럼") == 0 || strcmp(m_pNthOneChar, "려") == 0 || strcmp(m_pNthOneChar, "큼") == 0)
			{//2y : S(n) == “다” || “야” || “여” || “이” || “가” || “터” || “럼” || “려” || “큼”인가 ? MAR1_1_00004
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
	
	IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
}

void CNAHangulAnaly::GoMAR1_2(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화

	LoggingFunction(pNode, "MAR1_2_00001+ ");
	m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
	if (IsExistWildEndingDic(m_pFormNTo1Word) == true)
	{//2n : ② *S(n~1)이 ending 사전에 있는가 ? MAR1_2_00001
		LoggingFunction(pNode, "MAR1_2_00002+ ");
		mStart = nStart;
		if (IsExistEndingDic(m_pFormNTo1Word, m_endingFeature01))
		{//3y : m = n → S(n~1)이 ending 사전에 있는가 ? MAR1_2_00002
			LoggingFunction(pNode, "MAR1_2_00003+ ");
			m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
			m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
			if (IsExistStemDic(m_pStemDicWord, m_stemFeature01))
			{//4y : S(len~n+1)이 stem 사전에 있는가 ? MAR1_2_00003
				LoggingFunction(pNode, "MAR1_2_00004+ ");
				if (strcmp(m_pStemDicWord, "자신") == 0 || strcmp(m_pStemDicWord, "길") == 0
					|| strcmp(m_pStemDicWord, "안") == 0 || strcmp(m_pStemDicWord, "틀") == 0
					|| strcmp(m_pStemDicWord, "말") == 0 || strcmp(m_pStemDicWord, "풀") == 0
					|| strcmp(m_pStemDicWord, "끌") == 0 || strcmp(m_pStemDicWord, "굴") == 0
					|| strcmp(m_pStemDicWord, "얼") == 0 || strcmp(m_pStemDicWord, "달") == 0
					|| strcmp(m_pStemDicWord, "불") == 0 || strcmp(m_pStemDicWord, "살") == 0
					|| strcmp(m_pStemDicWord, "덜") == 0 || strcmp(m_pStemDicWord, "알") == 0
					|| strcmp(m_pStemDicWord, "놀") == 0 || strcmp(m_pStemDicWord, "열") == 0
					|| strcmp(m_pStemDicWord, "허물") == 0 || strcmp(m_pStemDicWord, "팔") == 0)
				{//5y : {S(len~n+1) == “자신” || “길” || “안” || “틀” || “말” | “풀” || “끌” || “굴” || “얼” || “달” || “불” || “살” || “덜” || “알” || “놀” || “열” || “허물” || “열” || “팔”}인가 ? MAR1_2_00004
					LoggingFunction(pNode, "MAR1_2_00005+ ");
					//6y : stem = S(len~n+1), ending = S(n~1) MAR1_2_00005
					InsertStemString(pNode, m_pStemDicWord);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_pFormNTo1Word);
					SaveHTokenEnding(pNode);
				}
				else
				{
					m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//n+1의 음절
					m_pJongChar = GetChojungJongsungChar(m_pOneNPluseNumChar, CHAR_JONGSUNG);//C(n+1)의 음소
					m_pEndingDicWord = AddTwoStr(m_pJongChar, m_pFormNTo1Word);//C(n+1)+S(n~1)
					LoggingFunction(pNode, "MAR1_2_00006+ ");
					if(IsExistEndingDic(m_pEndingDicWord, m_endingFeature02))
					{//6n : C(n+1)+S(n ~ 1)이 ending 사전에 있는가 ? MAR1_2_00006
						LoggingFunction(pNode, "MAR1_2_00007+ ");
						m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pOneNPluseNumChar, m_nPluseSyllable);
						if (IsExistStemDic(m_pStemDicDeleteJongWord, m_stemFeature02))
						{//7y : (S(len~n+1)-C(n+1))이 stem 사전에 있는가 ? MAR1_2_00007
							LoggingFunction(pNode, "MAR1_2_00008+ ");
							if (strcmp(m_pStemDicDeleteJongWord, "자시") != 0
								&&((m_stemFeature02[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature02[ENDING_DIC_N] == IS_CEHCK_FEATURE)
								|| (m_stemFeature02[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature02[ENDING_DIC_N] != IS_CEHCK_FEATURE)
								|| (m_stemFeature02[STEM_DIC_AMB] == IS_CEHCK_FEATURE && m_endingFeature02[ENDING_DIC_AMB] == IS_CEHCK_FEATURE)
							)) 
							{//8y : {(S(len~n+1)-C(n+1)) != :자시: || : 여느 : }이고, { (S(len~n+1)-C(n+1)) == [N] && C(n+1)+S(n~1) == [N] }이거나, { (S(len~n+1)-C(n+1)) <>[N] && C(n+1)+S(n~1) <>[N] }이거나, { (S(len~n+1)-C(n+1)) == [amb] || C(n+1)+S(n~1) == [amb] }인가 ? MAR1_2_00008
								LoggingFunction(pNode, "MAR1_2_00009+ ");
								m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//n의 음절
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
								{//9n : {S(len~n+1) == [N] && S(n~1) == [N]}이거나, { S(len~n+1) <>[N] && S(n~1) <>[N] }이거나, { S(len~n+1) == [amb] || S(n~1) == [amb] }인가 ? MAR1_2_00010
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
						IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
						m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
						m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)의 음소
						m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
						m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
						m_pEndingDicWord = AddTwoStr(m_pJongChar, m_pFormNTo1Word);//C(n)+S(n-1~1)
						LoggingFunction(pNode, "MAR1_2_00014+ ");
						if (IsExistEndingDic(m_pEndingDicWord, m_endingFeature01))
						{//7n : C(n)+S(n-1~1)이 ending 사전에 있는가? MAR1_2_00014
							LoggingFunction(pNode, "MAR1_2_00015+ ");
							m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
							m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
							if (IsExistStemDic(m_pStemDicDeleteJongWord, m_stemFeature01))
							{//8y : S(len~n)- #C(n) 이 stem 사전에 있는가? MAR1_2_00015
								LoggingFunction(pNode, "MAR1_2_00016+ ");
								if ((m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] == IS_CEHCK_FEATURE)
									|| (m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] != IS_CEHCK_FEATURE)
									|| (m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_AMB] == IS_CEHCK_FEATURE))
								{//9y : { S(len~n)- #C(n) == [N] && C(n)+S(n-1~1) == [N]}이거나, { S(len~n)- #C(n) <> [N] && C(n)+S(n-1~1) <> [N]}이거나, { S(len~n)- #C(n) == [amb] || C(n)+S(n-1~1) == [amb]} MAR1_2_00016
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
									IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
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
								if (strcmp(m_pStemDicWord, "가는") == 0)
								{//9n : {S(len~n) == :가는:}인가? MAR1_2_00019
									LoggingFunction(pNode, "MAR1_2_00020+ ");
									//10y : stem = S(len~n), ending = OldBuf() MAR1_2_00020
									InsertStemString(pNode, m_pStemDicWord);
									SaveHTokenStem(pNode);
									InsertEndingString(pNode, m_pOldBuffer);
									SaveHTokenEnding(pNode);
								}
								else
								{
									IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
									m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
									m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)의 음소
									m_nRestorationJongChar = Restoration(m_pJongChar);//복원해야 할 종성값
									ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);//C(n) = “ㄹ”
									m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
									bool bFind = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
									LoggingFunction(pNode, "MAR1_2_00021+ ");
									if (bFind == true && m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE)
									{//10n : C(n) = “ㄹ” → S(len~n)이 stem 사전에 있고, S(len~n) != [n]인가? MAR1_2_00021
										LoggingFunction(pNode, "MAR1_2_00022+ ");
										//11y : stem = S(len~n), ending = “ㄴ”+OldBuf() MAR1_2_00022
										InsertStemString(pNode, m_pStemDicWord);
										SaveHTokenStem(pNode);
										m_p1thAddString = AddTwoStr("ㄴ", m_pOldBuffer);
										InsertEndingString(pNode, m_p1thAddString);
										SaveHTokenStem(pNode);
									}
									else
									{
										//원래의 종성값으로 복원
										JasoRestoration(pNode, nStart, FORCE_REPLACE_ENDWORD, m_nRestorationJongChar);// C(n) = #C(n)
										IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
										LoggingFunction(pNode, "MAR1_2_00023+ ");
										m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
										m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
										m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
										bool bFindStemWord = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
										bool bFindEndingWord = IsExistEndingDic(m_pFormNTo1Word, m_endingFeature01);
										if (bFindStemWord == true && bFindEndingWord == true && m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_PPO] == IS_CEHCK_FEATURE)
										{//11n : C(n) = #C(n) → {S(len~n+1) == [N] && S(n~1) == [Ppo]}인가? MAR1_2_00023
											LoggingFunction(pNode, "MAR1_2_00024+ ");
											m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
											m_p1thAddString = AddTwoStr("이", m_pNthOneChar);
											m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
											//12y : stem = S(len~n+1), ending = “이”[Pol]+S(n)+OldBuf() MAR1_2_00024
											InsertStemString(pNode, m_pStemDicWord);
											SaveHTokenStem(pNode);
											InsertEndingString(pNode, m_p2thAddString);
											SaveHTokenEnding(pNode);
											AddHTokenEnding(pNode, ENDING_DIC_POL);//[Pol]자질부여
										}
										else
										{
											LoggingFunction(pNode, "MAR1_2_00025+ ");
											if ((m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] == IS_CEHCK_FEATURE)
												|| (m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] != IS_CEHCK_FEATURE)
												|| (m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_AMB] == IS_CEHCK_FEATURE))
											{//12n : C(n) = #C(n) → {S(len~n+1) == [N] && S(n~1) == [N]}이거나, {S(len~n+1) <> [N] && S(n~1) <> [N]}이거나 {S(len~n+1) == [amb] || S(n~1) == [amb]}인가? MAR1_2_00025
												LoggingFunction(pNode, "MAR1_2_00026+ ");
												m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
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
												//13n : OldBuf() = S(n~1) → ①로 MAR1_2_00027
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
							IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
							m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
							m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
							m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
							m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)의 음소
							bool bFindStemWord = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
							bool bFindEndingWord = IsExistEndingDic(m_pNthOneChar, m_endingFeature01);
							LoggingFunction(pNode, "MAR1_2_00028+ ");
							if ((bFindStemWord == true && bFindEndingWord == true) 
								&&((m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] == IS_CEHCK_FEATURE) || (m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] != IS_CEHCK_FEATURE) || (m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_AMB] == IS_CEHCK_FEATURE)))
							{//8n : {S(len~n+1) == [N] && S(n) == [N]}이거나, {S(len~n+1) <> [N] && S(n) <> [N]}이거나, {S(len~n+1) == [amb] || S(n) == [amb]}인가? MAR1_2_00028
								m_pLastOneChar = GetOneChar(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));//S(1)
								m_pFormLenToNWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
								LoggingFunction(pNode, "MAR1_2_00029+ ");
								if (strcmp(m_pNthOneChar, m_pLastOneChar) == 0
									&& (strcmp(m_pFormLenToNWord, "주어진") == 0 || strcmp(m_pFormLenToNWord, "나간") == 0 || strcmp(m_pFormLenToNWord, "자길") == 0
									|| strcmp(m_pFormLenToNWord, "옮길") == 0 || strcmp(m_pFormLenToNWord, "옮긴") == 0 || strcmp(m_pFormLenToNWord, "들를") == 0
									|| strcmp(m_pFormLenToNWord, "이길") == 0 || strcmp(m_pFormLenToNWord, "터진") == 0 || strcmp(m_pFormLenToNWord, "맡긴") == 0
									|| strcmp(m_pFormLenToNWord, "가를") == 0 || strcmp(m_pFormLenToNWord, "깨진") == 0 || strcmp(m_pFormLenToNWord, "모를") == 0
									|| strcmp(m_pFormLenToNWord, "부를") == 0 || strcmp(m_pFormLenToNWord, "배길") == 0 || strcmp(m_pFormLenToNWord, "내려질") == 0
									|| strcmp(m_pFormLenToNWord, "숨진") == 0 || strcmp(m_pFormLenToNWord, "못간") == 0 || strcmp(m_pFormLenToNWord, "이어질") == 0
									|| strcmp(m_pFormLenToNWord, "어긴") == 0 || strcmp(m_pFormLenToNWord, "맡길") == 0 || strcmp(m_pFormLenToNWord, "켜진") == 0
									|| strcmp(m_pFormLenToNWord, "남긴") == 0 || strcmp(m_pFormLenToNWord, "알려진") == 0
									)) 
								{//9y : S(n) == S(1)이고, S(len~n) == “주어진”|| “나간”|| “자길”|| “옮길”|| “옮긴”|| “들를”|| “이길”|| “터진”|| “맡긴”|| “가를”|| “깨진”|| “모를”|| “부를” || “배길”|| “내려질”|| “숨진”|| “못간”|| “이어질”|| “어긴”|| “맡길”|| “켜진”|| “남긴”|| “알려진”인가? MAR1_2_00029
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
										&& (strcmp(m_pFormLenToNWord, "만든") == 0 || strcmp(m_pFormLenToNWord, "찌든") == 0
										|| strcmp(m_pFormLenToNWord, "힘든") == 0 || strcmp(m_pFormLenToNWord, "정든") == 0))
									{//10n : S(n) == S(1)이고, S(len~n) == “만든”||“찌든”||“힘든”||“정든” 인가? MAR1_2_00031
										LoggingFunction(pNode, "MAR1_2_00032+ ");
										IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
										//11y : stem = S(len~n)-C(n)+“ㄹ”, ending = “ㄴ" MAR1_2_00032
										ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);//S(len~n)-C(n)+“ㄹ”
										m_pFormLenToNWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
										InsertStemString(pNode, m_pFormLenToNWord);
										SaveHTokenStem(pNode);
										InsertEndingString(pNode, "ㄴ");
										SaveHTokenEnding(pNode);
									}
									else
									{
										LoggingFunction(pNode, "MAR1_2_00033+ ");
										IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
										m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
										m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
										m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
										bool bFindStemWord = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
										bool bFindEndingWord = IsExistEndingDic(m_pFormNTo1Word, m_endingFeature01);
										if (bFindStemWord == true && bFindEndingWord == true && m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_PPO] == IS_CEHCK_FEATURE)
										{//11n : S(len~n+1) == [N] && S(n~1) == [Ppo]인가? MAR1_2_00033
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
								IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
								m_pLastOneChar = GetOneChar(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));//S(1)
								m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
								LoggingFunction(pNode, "MAR1_2_00036+ ");
								if(strcmp(m_pNthOneChar, m_pLastOneChar) == 0)
								{//9n : S(n) == S(1)인가? MAR1_2_00036
									m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)의 음소
									LoggingFunction(pNode, "MAR1_2_00037+ ");
									if(strcmp(m_pJongChar, "ㄴ") == 0 || strcmp(m_pJongChar, "ㄹ") == 0)
									{ //10y :  C(n) == “ㄴ” || “ㄹ”인가 ? MAR1_2_00037 
										m_pFormLenToNWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
										m_pStemDicDeleteJongWord = MinusTwoStr(m_pFormLenToNWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
										LoggingFunction(pNode, "MAR1_2_00038+ ");
										if (IsExistStemDic(m_pStemDicDeleteJongWord, m_stemFeature01))
										{//11y : S(len~n)-C(n) 이 stem 사전에 있는가? MAR1_2_00038
											LoggingFunction(pNode, "MAR1_2_00039+ ");
											//12y: stem = S(len ~ n)-C(n), ending = “ㄴ” || “ㄹ” MAR1_2_00039
											InsertStemString(pNode, m_pStemDicDeleteJongWord);
											SaveHTokenStem(pNode);
											InsertEndingString(pNode, m_pJongChar);
											SaveHTokenEnding(pNode);
										}
										else
										{
											LoggingFunction(pNode, "MAR1_2_00040+ ");
											if (strcmp(m_pJongChar, "ㄴ") == 0)
											{//12n : C(n) == “ㄴ”인가? MAR1_2_00040
												IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
												ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);//C(n) = “ㄹ”->S(len~n)
												m_pFormLenToNWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
												LoggingFunction(pNode, "MAR1_2_00041+ ");
												if (IsExistStemDic(m_pFormLenToNWord, m_stemFeature01))
												{//13y : C(n) = “ㄹ”->S(len~n) 이 stem 사전에 있는가? MAR1_2_00041
													LoggingFunction(pNode, "MAR1_2_00042+ ");
													if (strcmp(m_pFormLenToNWord, "예정일") == 0)
													{//14y : S(len~n)이 “예정일”인가? MAR1_2_00042
														LoggingFunction(pNode, "MAR1_2_00043+ ");
														//15y : stem = S(len ~ n+1), ending=“이"+[Pol]+“ㄴ” MAR1_2_00043
														m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
														m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
														m_p1thAddString = AddTwoStr("이", "ㄴ");
														InsertStemString(pNode, m_pStemDicWord);
														SaveHTokenStem(pNode);
														InsertEndingString(pNode, m_p1thAddString);
														SaveHTokenEnding(pNode);
														AddHTokenEnding(pNode, ENDING_DIC_POL);
													}
													else
													{
														LoggingFunction(pNode, "MAR1_2_00044+ ");
														//15n : stem = S(len ~ n), ending = “ㄴ” MAR1_2_00044
														InsertStemString(pNode, m_pFormLenToNWord);
														SaveHTokenStem(pNode);
														InsertEndingString(pNode, "ㄴ");
														SaveHTokenEnding(pNode);
													}
												}
												else
												{
													IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
													JasoRestoration(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA01);//C(n) = “ㄴ”
													m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
													m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
													m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
													bool bFindStemWord = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
													bool bFindEndingWord = IsExistEndingDic(m_pFormNTo1Word, m_endingFeature01);
													LoggingFunction(pNode, "MAR1_2_00045+ ");
													if (bFindStemWord == true && bFindEndingWord == true && m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_PPO] == IS_CEHCK_FEATURE)
													{//14n : C(n) = “ㄴ” → {S(len~n+1) == [N] && S(n~1) == [Ppo]}인가? MAR1_2_00045
														LoggingFunction(pNode, "MAR1_2_00046+ ");
														m_pLastOneChar = GetOneChar(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));//S(1)
														m_p1thAddString = AddTwoStr("이", m_pLastOneChar);
														m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
														//15y : stem = S(len~n+1), ending= “이”[Pol]+S(1)+OldBuf() MAR1_2_00046
														InsertStemString(pNode, m_pStemDicWord);
														SaveHTokenStem(pNode);
														InsertEndingString(pNode, m_p2thAddString);
														SaveHTokenEnding(pNode);
														AddHTokenEnding(pNode, ENDING_DIC_POL);//[Pol]자질부여
													}
													else
													{
														LoggingFunction(pNode, "MAR1_2_00047+ ");
														if ((bFindStemWord == true && bFindEndingWord == true)
															&& ((m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] == IS_CEHCK_FEATURE)
																|| (m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] != IS_CEHCK_FEATURE)
																|| (m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_AMB] == IS_CEHCK_FEATURE)))
														{//15n : C(n) = “ㄴ” → {S(len~n+1) == [N] && S(n~1) == [N]}이거나, {S(len~n+1) <> [N] && S(n~1) <> [N]}이거나 {S(len~n+1) == [amb] || S(n~1) == [amb]}인가? MAR1_2_00047
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
															//16n : OldBuf() = S(n~1) → ①로 MAR1_2_00049
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
												IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
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
										IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
										m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
										m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
										m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
										bool bFindStemWord = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
										bool bFindEndingWord = IsExistEndingDic(m_pFormNTo1Word, m_endingFeature01);
										if ((bFindStemWord == true && bFindEndingWord == true) && (m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_PPO] == IS_CEHCK_FEATURE))
										{//11n : {S(len~n+1) == [N] && S(n~1) == [Ppo]}인가? MAR1_2_00051
											
											LoggingFunction(pNode, "MAR1_2_00052+ ");
											CHAR_DELETE(m_pLastOneChar);
											m_pLastOneChar = GetOneChar(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));//S(1)
											m_p1thAddString = AddTwoStr("이", m_pLastOneChar);
											m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
											//12y : stem = S(len~n+1), ending =“이"[Pol]+S(1)+OldBuf() MAR1_2_00052
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
											{//12n : {S(len~n+1) == [N] && S(n~1) == [N]}이거나, {S(len~n+1) <> [N] && S(n~1) <> [N]}이거나 {S(len~n+1) == [amb] || S(n~1) == [amb]}인가? MAR1_2_00053
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
												//13n : OldBuf() = S(n~1) → ①로 MAR1_2_00055
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
									IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
									m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
									m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
									m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
									bool bFindStemWord = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
									bool bFindEndingWord = IsExistEndingDic(m_pFormNTo1Word, m_endingFeature01);
									LoggingFunction(pNode, "MAR1_2_00056+ ");
									if ((bFindStemWord == true && bFindEndingWord == true) && (m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_PPO] == IS_CEHCK_FEATURE))
									{//10n : {S(len~n+1) == [N] && S(n~1) == [Ppo]}인가? MAR1_2_00056
										LoggingFunction(pNode, "MAR1_2_00057+ ");
										m_p1thAddString = AddTwoStr("이", m_pFormNTo1Word);
										//11y : stem = S(len~n+1), ending = “이"[Pol]+S(n~1) MAR1_2_00057
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
										{//11n : {S(len~n+1) == [N] && S(n~1) == [N]}이거나, {S(len~n+1) <> [N] && S(n~1) <> [N]}이거나 {S(len~n+1) == [amb] || S(n~1) == [amb]}인가? MAR1_2_00058
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

											//12n : Error(20030128 유혜원 수정 “전국지도”) MAR1_2_00060
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
				IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
				m_pLastOneChar = GetOneChar(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));//S(1)
				m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
				LoggingFunction(pNode, "MAR1_2_00061+ ");
				if (strcmp(m_pNthOneChar, m_pLastOneChar) == 0)
				{//5n : S(n) == S(1)인가? MAR1_2_00061
					m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)의 음소
					LoggingFunction(pNode, "MAR1_2_00062+ ");
					if (strcmp(m_pJongChar, "ㄴ") == 0 || strcmp(m_pJongChar, "ㄹ") == 0)
					{//6y :  C(n) == “ㄴ” || “ㄹ”인가? MAR1_2_00062
						m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
						m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
						LoggingFunction(pNode, "MAR1_2_00063+ ");
						if (IsExistStemDic(m_pStemDicDeleteJongWord))
						{//7y : S(len~n)-C(n) 이 stem 사전에 있는가? MAR1_2_00063
							LoggingFunction(pNode, "MAR1_2_00064+ ");
							//8y: stem = S(len ~ n)-C(n), ending = “ㄴ” || “ㄹ”  MAR1_2_00064
							InsertStemString(pNode, m_pStemDicDeleteJongWord);
							SaveHTokenStem(pNode);
							InsertEndingString(pNode, m_pJongChar);
							SaveHTokenEnding(pNode);
						}
						else
						{
							LoggingFunction(pNode, "MAR1_2_00065+ ");
							if (strcmp(m_pJongChar, "ㄴ") == 0)
							{//8n : C(n) == “ㄴ”인가? MAR1_2_00065
								CHAR_DELETE(m_pStemDicWord);
								m_nRestorationJongChar = Restoration(m_pJongChar);//복원해야 할 종성값
								ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);//C(n) = “ㄹ”
								m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
								LoggingFunction(pNode, "MAR1_2_00066+ ");
								if (IsExistStemDic(m_pStemDicWord))
								{//9y : C(n) = “ㄹ” ->  S(len~n) 이 stem 사전에 있는가? MAR1_2_00066
									LoggingFunction(pNode, "MAR1_2_00067+ ");
									//10y : stem = S(len ~ n), ending = “ㄴ”  MAR1_2_00067
									InsertStemString(pNode, m_pStemDicWord);
									SaveHTokenStem(pNode);
									InsertEndingString(pNode, "ㄴ");
									SaveHTokenEnding(pNode);
								}
								else
								{
									LoggingFunction(pNode, "MAR1_2_00068+ ");
									//10n : C(n) = “ㄴ” -> OldBuf() = S(n~1) → ①로 MAR1_2_00068
									ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA01);//C(n) = “ㄴ”
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
								//9n : OldBuf() = S(n-1) → ①로 MAR1_2_00069
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
						IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
						m_pLastOneChar = GetOneChar(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));//S(1)
						m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
						LoggingFunction(pNode, "MAR1_2_00070+ ");
						if (m_pOldBuffer != NULL && strcmp(m_pOldBuffer, m_pLastOneChar) != 0)
						{//7n : OldBuf()가 S(1)가 같지 않은가?(OldBuf() != S(1)) MAR1_2_00070
							LoggingFunction(pNode, "MAR1_2_00071+ ");
							//8y : OldBuf() = S(1)+OldBuf()→ ①로 MAR1_2_00071
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
							//8n : OldBuf() = S(n~1) → ①로 MAR1_2_00072
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
					IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
					m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
					m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)의 음소
					m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
					m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
					m_pEndingDicWord = AddTwoStr(m_pJongChar, m_pFormNTo1Word);//C(n)+S(n-1~1)
					LoggingFunction(pNode, "MAR1_2_00073+ ");
					if (IsExistEndingDic(m_pEndingDicWord, m_endingFeature01))
					{//6n : C(n)+S(n-1~1)이 ending사전에 있는가? MAR1_2_00073
						m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
						m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
						LoggingFunction(pNode, "MAR1_2_00074+ ");
						if (IsExistStemDic(m_pStemDicDeleteJongWord, m_stemFeature01))
						{//7y : S(len~n)-C(n)이 stem사전에 있는가? MAR1_2_00074
							LoggingFunction(pNode, "MAR1_2_00075+ ");
							if ((m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] == IS_CEHCK_FEATURE)
								|| (m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] != IS_CEHCK_FEATURE)
								|| (m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_AMB] == IS_CEHCK_FEATURE))
							{//8y : {(S(len~n)-C(n)) == [N] && C(n) +S(n-1~1) == [N]}이거나, {(S(len~n)-C(n)) <> [N] && C(n) +S(n-1~1) <> [N]}이거나  {(S(len~n)-C(n)) == [amb] || C(n) +S(n-1~1) == [amb]}인가? MAR1_2_00075
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
								//9n : OldBuf = S(n~1)  ①로 MAR1_2_00077
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
							//8n : OldBuf = S(n~1)  ①로 MAR1_2_00078
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
						IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
						m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
						bool bFindEndingWord = IsExistEndingDic(m_pFormNTo1Word, m_endingFeature01);
						LoggingFunction(pNode, "MAR1_2_00079+ ");
						if (bFindEndingWord == true && m_endingFeature01[ENDING_DIC_PRS] == IS_CEHCK_FEATURE)
						{//7n : S(n~1)이 [Prs]인가? MAR1_2_00079
							m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
							m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)
							LoggingFunction(pNode, "MAR1_2_00080+ ");
							if (strcmp(m_pNthOneChar, "으") == 0 || strcmp(m_pNthOneChar, "시") == 0 || strcmp(m_pNthOneChar, "이") == 0 || strcmp(m_pNthOneChar, "하") == 0 || strcmp(m_pNthOneChar, "되") == 0 || strcmp(m_pNthOneChar, "러") == 0 || strcmp(m_pNthOneChar, "우") == 0)
							{//8y : S(n+1) == “으” || “시” || “이” || “하” || “되” || “러” || “우”인가? MAR1_2_00080
								LoggingFunction(pNode, "MAR1_2_00081+ ");
								//9y : OldBuf = S(n~1)  ①로 MAR1_2_00081
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

								//9n : Error(도예강사, 피부관리사) MAR1_2_00082
								GoErrorFunction(pNode);
							}
						}
						else
						{
							LoggingFunction(pNode, "MAR1_2_00083+ ");
							//8n : OldBuf = S(n~1)  ①로 / *사람들은 = 사람들+은 MAR1_2_00083
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
			IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
			m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
			m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1~1)
			LoggingFunction(pNode, "MAR1_2_00084+ ");
			if (IsExistEndingDic(m_pFormNTo1Word, m_endingFeature01))
			{//4n : S(n+1~1)이 ending 사전에 있는가? MAR1_2_00084
				LoggingFunction(pNode, "MAR1_2_00085+ ");
				m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)
				m_p1thAddString = AddTwoStr(m_pNthOneChar, m_pOldBuffer);
				//5y : OldBuf() = S(n)+OldBuf() → ①로 MAR1_2_00085
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
				IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
				m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
				m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
				m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//n+1의 음절
				m_pJongChar = GetChojungJongsungChar(m_pOneNPluseNumChar, CHAR_JONGSUNG);//C(n+1)의 음소
				m_pEndingDicWord = AddTwoStr(m_pJongChar, m_pFormNTo1Word);//C(n+1)+S(n~1)
				LoggingFunction(pNode, "MAR1_2_00086+ ");
				if (IsExistEndingDic(m_pEndingDicWord, m_endingFeature01))
				{//5n : C(n+1)+S(n~1)이 ending 사전에 있는가? MAR1_2_00086
					LoggingFunction(pNode, "MAR1_2_00087+ ");
					m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)
					m_p1thAddString = AddTwoStr(m_pNthOneChar, m_pOldBuffer);
					//6y : OldBuf() = S(n)+OldBuf() → ①로 MAR1_2_00087
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
					IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
					m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
					m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1~1)
					m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
					m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//n+2의 음절
					m_pJongChar = GetChojungJongsungChar(m_pOneNPluseNumChar, CHAR_JONGSUNG);//C(n+2)의 음소
					m_pEndingDicWord = AddTwoStr(m_pJongChar, m_pFormNTo1Word);//C(n+2)+S(n+1~1)
					LoggingFunction(pNode, "MAR1_2_00088+ ");
					if (IsExistEndingDic(m_pEndingDicWord, m_endingFeature01))
					{//6n : C(n+2)+S(n+1~1)이 ending 사전에 있는가? MAR1_2_00088
						LoggingFunction(pNode, "MAR1_2_00089+ ");
						m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)
						m_p1thAddString = AddTwoStr(m_pNthOneChar, m_pOldBuffer);
						//7y : OldBuf() = S(n)+OldBuf() → ①로  MAR1_2_00089
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
						IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
						m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
						m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)의 음소
						m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
						m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
						m_pEndingDicWord = AddTwoStr(m_pJongChar, m_pFormNTo1Word);//C(n)+S(n-1~1)
						LoggingFunction(pNode, "MAR1_2_00090+ ");
						if (IsExistEndingDic(m_pEndingDicWord, m_endingFeature01))
						{//7n : C(n)+S(n-1~1)이 ending 사전에 있는가? MAR1_2_00090
							m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
							m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
							LoggingFunction(pNode, "MAR1_2_00091+ ");
							if (IsExistStemDic(m_pStemDicDeleteJongWord, m_stemFeature01))
							{//8y : S(len~n)-C(n)이 stem 사전에 있는가? MAR1_2_00091
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
								//9n : OldBuf() = C(n)+S(n-1~1), S(n) = S(n)-C(n) , MAR2로 MAR1_2_00093
								CHAR_DELETE(m_pOldBuffer);
								int nLen = strlen(m_pEndingDicWord);
								m_pOldBuffer = new char[nLen+1];
								memset(m_pOldBuffer, 0x00, nLen+1);
								strcpy(m_pOldBuffer, m_pEndingDicWord);
								InsertArrayFeatureOld(m_pEndingDicWord);
								ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA14);//S(n)-C(n) 종성을 없애라는 의미임 :REPLACE_HANGUL_JA14 NULL의미
								
								GoMAR2(pNode, nStart, mStart, lStart);
							}
						}
						else
						{
							IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
							m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
							m_pLastOneChar = GetOneChar(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));//S(1)
							m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)의 음소
							LoggingFunction(pNode, "MAR1_2_00094+ ");
							if (strcmp(m_pNthOneChar, m_pLastOneChar) == 0)
							{//8n : S(n) == S(1)인가? MAR1_2_00094
								LoggingFunction(pNode, "MAR1_2_00095+ ");
								if (strcmp(m_pJongChar, "ㄴ") == 0)
								{//9y : C(n) == “ㄴ” 인가? MAR1_2_00095
									m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
									m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
									LoggingFunction(pNode, "MAR1_2_00096+ ");
									if (IsExistStemDic(m_pStemDicDeleteJongWord, m_stemFeature01))
									{//10y : S(len~n)-C(n) 이 stem 사전에 있는가? MAR1_2_00096
										LoggingFunction(pNode, "MAR1_2_00097+ ");
										//11y : stem = S(len ~ n)-C(n), ending = “ㄴ”  MAR1_2_00097
										InsertStemString(pNode, m_pStemDicDeleteJongWord);
										SaveHTokenStem(pNode);
										InsertEndingString(pNode, "ㄴ");
										SaveHTokenEnding(pNode);
									}
									else
									{
										m_pChoJungChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_CHOJUNGSUNG);//S(n)-C(n)
										LoggingFunction(pNode, "MAR1_2_00098+ ");
										if (strcmp(m_pChoJungChar, "하") == 0 || strcmp(m_pChoJungChar, "되") == 0)
										{//11n : S(n)-C(n) == “하”||“되”인가? MAR1_2_00098
											LoggingFunction(pNode, "MAR1_2_00099+ ");
											//12y : OldBuf() = “ㄴ” -> IRM으로  MAR1_2_00099
											CHAR_DELETE(m_pOldBuffer);
											int nLen = strlen("ㄴ");
											m_pOldBuffer = new char[nLen+1];
											memset(m_pOldBuffer, 0x00, nLen+1);
											strcpy(m_pOldBuffer, "ㄴ");
											InsertArrayFeatureOld("ㄴ");
											
											GoIMR1(pNode, nStart, mStart,lStart);
										}
										else
										{
											m_nRestorationJongChar = Restoration(m_pJongChar);//복원해야 할 종성값
											ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);//C(n) = “ㄹ”
											m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
											LoggingFunction(pNode, "MAR1_2_00100+ ");
											if (IsExistStemDic(m_pStemDicWord, m_stemFeature01))
											{//12n : C(n) = “ㄹ” ->  S(len~n) 이 stem 사전에 있는가? MAR1_2_00100
												LoggingFunction(pNode, "MAR1_2_00101+ ");
												//13y : stem = S(len ~ n), ending = “ㄴ” MAR1_2_00101
												InsertStemString(pNode, m_pStemDicWord);
												SaveHTokenStem(pNode);
												InsertEndingString(pNode, "ㄴ");
												SaveHTokenEnding(pNode);
											}
											else
											{
												LoggingFunction(pNode, "MAR1_2_00102+ ");
												//13n : OldBuf() = “ㄴ” , C(n) = Null, ②로 MAR1_2_00102
												CHAR_DELETE(m_pOldBuffer);
												int nLen = strlen("ㄴ");
												m_pOldBuffer = new char[nLen+1];
												memset(m_pOldBuffer, 0x00, nLen+1);
												strcpy(m_pOldBuffer, "ㄴ");
												InsertArrayFeatureOld("ㄴ");
												ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA14);//S(n)-C(n) 종성을 없애라는 의미임 :REPLACE_HANGUL_JA14 NULL의미
												
												GoMAR1_2(pNode, nStart, mStart, lStart);
											}
										}
									}
								}
								else
								{
									IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
									m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
									m_pLastOneChar = GetOneChar(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));//S(1)
									m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)의 음소
									LoggingFunction(pNode, "MAR1_2_00103+ ");
									if (strcmp(m_pJongChar, "ㄹ") == 0)
									{//10n : C(n) == “ㄹ” 인가? MAR1_2_00103
										m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
										m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
										LoggingFunction(pNode, "MAR1_2_00104+ ");
										if (IsExistStemDic(m_pStemDicDeleteJongWord, m_stemFeature01))
										{//11y : S(len~n)-C(n) 이 stem 사전에 있는가? MAR1_2_00104
											LoggingFunction(pNode, "MAR1_2_00105+ ");
											//12y : stem = S(len ~ n)-C(n), ending = “ㄹ” MAR1_2_00105
											InsertStemString(pNode, m_pStemDicDeleteJongWord);
											SaveHTokenStem(pNode);
											InsertEndingString(pNode, "ㄹ");
											SaveHTokenEnding(pNode);
										}
										else
										{
											LoggingFunction(pNode, "MAR1_2_00106+ ");
											//12n : OldBuf() = S(n)+OldBuf(), ①로 MAR1_2_00106
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
										//11n : OldBuf() = S(n)+OldBuf(), ①로 MAR1_2_00107
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
								//9n : m = m - 1 → MAR2로 MAR1_2_00108
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
		//……………………………………………………………………………………………………………………………MAR1_3
		//3n : n == 1인가 ? MAR1_3_00001 
		GoMAR1_3(pNode, nStart, mStart, lStart);
	}
}

	
void CNAHangulAnaly::GoMAR1_3(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
	m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)의 음소
	LoggingFunction(pNode, "MAR1_3_00001+ ");
	if (strlen(pNode->pHToken->strOriginalSource) == nStart)
	{//3n : n == 1인가 ? MAR1_3_00001
		LoggingFunction(pNode, "MAR1_3_00002+ ");
		if (strcmp(m_pJongChar, "ㄴ") == 0)
		{//4y : C(n) == “ㄴ”인가 ? MAR1_3_00002
			m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
			m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
			LoggingFunction(pNode, "MAR1_3_00003+ ");
			if (IsExistStemDic(m_pStemDicDeleteJongWord))
			{//5y : S(len~n)-C(n)이 stem 사전에 있는가 ? MAR1_3_00003
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
				m_nRestorationJongChar = Restoration(m_pJongChar);//복원해야 할 종성값
				ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);//C(n) = “ㄹ”
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
				LoggingFunction(pNode, "MAR1_3_00005+ ");
				if (IsExistStemDic(m_pStemDicWord))
				{//6n : C(n) = “ㄹ” → S(len~n)이 stem 사전에 있는가 ? MAR1_3_00005
					LoggingFunction(pNode, "MAR1_3_00006+ ");
					if (strcmp(m_pStemDicWord, "짝수일") == 0 || strcmp(m_pStemDicWord, "홀수일") == 0)
					{//7y : S(len~n) == “짝수일” || “홀수일”인가 ? MAR1_3_00006
						LoggingFunction(pNode, "MAR1_3_00007+ ");
						m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
						m_pFormLenToNWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
						m_p1thAddString = AddTwoStr("이", "ㄴ");
						//8y : stem = S(len~n+1), ending = “이”+[pol]+“ㄴ" MAR1_3_00007
						InsertStemString(pNode, m_pFormLenToNWord);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, m_p1thAddString);
						SaveHTokenEnding(pNode);
						AddHTokenEnding(pNode, ENDING_DIC_POL);
					}
					else
					{
						LoggingFunction(pNode, "MAR1_3_00008+ ");
						//8n : stem = S(len~n), ending = “ㄴ” MAR1_3_00008
						InsertStemString(pNode, m_pStemDicWord);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, "ㄴ");
						SaveHTokenEnding(pNode);
					}
				}
				else
				{
					CHAR_DELETE(m_pNthOneChar);
					ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA01);//C(n) = “ㄴ”
					m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
					m_pChoJungChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_CHOJUNGSUNG);//S(n)-C(n)
					LoggingFunction(pNode, "MAR1_3_00009+ ");
					if (strcmp(m_pChoJungChar, "으") == 0 || strcmp(m_pChoJungChar, "시") == 0 || strcmp(m_pChoJungChar, "이") == 0
						|| strcmp(m_pChoJungChar, "하") == 0 || strcmp(m_pChoJungChar, "되") == 0 || strcmp(m_pChoJungChar, "러") == 0
						|| strcmp(m_pChoJungChar, "우") == 0 || strcmp(m_pChoJungChar, "키") == 0 || strcmp(m_pChoJungChar, "기") == 0)
					{//7n : C(n) = “ㄴ” → S(n)-C(n) ==  “으”|| “시”|| “이”|| “하”|| “되”|| “러”|| “우”|| “키" || “기”인가? (20030128,유혜원 수정, “리튬이온”, “한자사전”)) MAR1_3_00009
						LoggingFunction(pNode, "MAR1_3_00010+ ");
						//8y : OldBuf = “ㄴ”, S(n) = S(n)-C(n) MAR1_3_00010
						//→ S(n)-C(n) == “이”||“하”||“되”||“러”||“키”||“우”이면, IRM으로 MAR1_3_00011
						//→ S(n)-C(n) == “시”||“으” MAR1의 ②로 MAR1_3_00012
						CHAR_DELETE(m_pOldBuffer);
						int nLen = strlen("ㄴ");
						m_pOldBuffer = new char[nLen+1];
						memset(m_pOldBuffer, 0x00, nLen+1);
						strcpy(m_pOldBuffer, "ㄴ");
						InsertArrayFeatureOld("ㄴ");
						ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA14);//S(n)-C(n) 종성을 없애라는 의미임 :REPLACE_HANGUL_JA14 NULL의미

						if (strcmp(m_pChoJungChar, "이") == 0 || strcmp(m_pChoJungChar, "하") == 0 || strcmp(m_pChoJungChar, "되") == 0
							|| strcmp(m_pChoJungChar, "러") == 0 || strcmp(m_pChoJungChar, "키") == 0 || strcmp(m_pChoJungChar, "우") == 0)
						{
							LoggingFunction(pNode, "MAR1_3_00011+ ");

							GoIMR1(pNode, nStart, mStart,lStart);
						}
						else if (strcmp(m_pChoJungChar, "이") == 0 || strcmp(m_pChoJungChar, "하") == 0)
						{
							LoggingFunction(pNode, "MAR1_3_00012+ ");

							GoMAR1_2(pNode, nStart, mStart, lStart);
						}
					}
					else
					{
						LoggingFunction(pNode, "MAR1_3_00013+ ");

						//8n : Error(기존 알고리듬 일부 삭제) MAR1_3_00013
						GoErrorFunction(pNode);
					}
				}
			}
		}
		else
		{
			LoggingFunction(pNode, "MAR1_3_00014+ ");
			if (strcmp(m_pNthOneChar, "술") != 0 && strcmp(m_pNthOneChar, "발") != 0 && strcmp(m_pJongChar, "ㄹ") == 0)
			{//5n : S(n) != “술”&&“발”  && C(n) == “ㄹ”인가? MAR1_3_00014
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
				m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
				LoggingFunction(pNode, "MAR1_3_00015+ ");
				if (IsExistStemDic(m_pStemDicDeleteJongWord))
				{//6y : S(len~n)-C(n)이 stem 사전에 있는가? MAR1_3_00015
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
					{//7n : S(len~n)이 stem 사전에 있는가? MAR1_3_00017
						LoggingFunction(pNode, "MAR1_3_00018+ ");
						//8y : stem = S(len~n), ending = ㄹ MAR1_3_00018
						InsertStemString(pNode, m_pStemDicWord);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, "ㄹ");
						SaveHTokenEnding(pNode);
					}
					else
					{
						m_pChoJungChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_CHOJUNGSUNG);//S(n)-C(n)
						LoggingFunction(pNode, "MAR1_3_00019+ ");
						//S(len~n)-C(n)이 Stem사전에 없고 ..6y에서 이미 확인됨 따라서 로직에 포함시키지 않음 else문에 이미 false로직이 포함됨
						if (strcmp(m_pChoJungChar, "이") == 0 || strcmp(m_pChoJungChar, "으") == 0 || strcmp(m_pChoJungChar, "시") == 0
							|| strcmp(m_pChoJungChar, "하") == 0 || strcmp(m_pChoJungChar, "되") == 0 || strcmp(m_pChoJungChar, "키") == 0)
						{//8n : S(len~n)-C(n)이 Stem사전에 없고, S(n)-C(n) != “이”|| “으”|| “시”|| “하”|| “되”|| “키”인지 확인하라. MAR1_3_00019
							LoggingFunction(pNode, "MAR1_3_00020+ ");

							//9y : Error(나이스디지털, 바디오일 20030321 유혜원 수정) MAR1_3_00020
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
								|| strcmp(m_pChoJungChar, "기") == 0)
							{//9n : S(n)-C(n) == txx ||“기”인가? MAR1_3_00021
								LoggingFunction(pNode, "MAR1_3_00022+ ");
								//10y : OldBuf = “ㄹ” , S(n) = S(n)-C(n) &&IRM으로 MAR1_3_00022
								CHAR_DELETE(m_pOldBuffer);
								int nLen = strlen("ㄹ");
								m_pOldBuffer = new char[nLen+1];
								memset(m_pOldBuffer, 0x00, nLen+1);
								strcpy(m_pOldBuffer, "ㄹ");
								InsertArrayFeatureOld("ㄹ");
								ReplaceHangulJongsungNull(pNode, nStart);
								
								GoIMR1(pNode, nStart, mStart, lStart);
							}
							else
							{
								LoggingFunction(pNode, "MAR1_3_00023+ ");
								//10n : OldBuf = C(n), S(n) = S(n)-C(n), MAR1_2로 MAR1_3_00023
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
				if (strcmp(m_pNthOneChar, "술") == 0 || strcmp(m_pNthOneChar, "발") == 0)
				{//6n : S(n) = “술”||“발”인가? MAR1_3_00024
					LoggingFunction(pNode, "MAR1_3_00025+ ");

					//7y : Error MAR1_3_00025
					GoErrorFunction(pNode);
				}
				else
				{
					LoggingFunction(pNode, "MAR1_3_00026+ ");
					if (IsExistEndingDic(m_pJongChar))
					{//7n : C(n)이 ending 사전에 있는가? MAR1_3_00026
						m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
						m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
						LoggingFunction(pNode, "MAR1_3_00027+ ");
						if (IsExistStemDic(m_pStemDicDeleteJongWord))
						{//8y : S(len~n)-C(n)이 stem 사전에 있는가? MAR1_3_00027
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
							{//9n : S(n)-C(n) == txx인가? MAR1_3_00030
								LoggingFunction(pNode, "MAR1_3_00029+ ");
								ReplaceHangulJongsungNull(pNode, nStart);
								//10y : S(n) = S(n)-C(n) && IRM으로 MAR1_3_00030
								
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
								//10n : OldBuf = C(n), S(n) = S(n)-C(n), MAR1_2로 MAR1_3_00031
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
		//4n : C(n) == Null인가 ? MAR1_4_00001
		GoMAR1_4(pNode, nStart, mStart, lStart);
	}
}


void CNAHangulAnaly::GoMAR1_4(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
	m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)의 음소
	LoggingFunction(pNode, "MAR1_4_00001+ ");
	if(m_pJongChar == NULL || strcmp(m_pJongChar,"")== 0)
	{//4n : C(n) == Null인가 ? MAR1_4_00001
		ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);//C(n) = “ㄹ”
		m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
		LoggingFunction(pNode, "MAR1_4_00002+ ");
		if (IsExistStemDic(m_pStemDicWord, m_stemFeature01))
		{//5y : C(n) = “ㄹ” → S(len~n)이 stem 사전에 있는가 ? MAR1_4_00002
			m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
			m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
			bool bFind = IsExistEndingDic(m_pFormNTo1Word, m_endingFeature01);
			LoggingFunction(pNode, "MAR1_4_00003+ ");
			if(bFind == true && m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] != IS_CEHCK_FEATURE)
			{//6y : {S(len~n) <>[N] && S(n-1~1) <>[N]}인가 ? MAR1_4_00003
				LoggingFunction(pNode, "MAR1_4_00004+ ");
				if(m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_AMB] == IS_CEHCK_FEATURE)
				{//7y : {S(len~n) == [amb] || S(n-1~1) == [amb]}인가 ? MAR1_4_00004
					LoggingFunction(pNode, "MAR1_4_00005+ ");
					if(m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_PPO] != IS_CEHCK_FEATURE)
					{//8y : {S(len~n) == [N] && S(n-1~1) != [Ppo]}인가 ? MAR1_4_00005
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
				//7n : C(n) = Null → MAR2로 MAR1_4_00009
				ReplaceHangulJongsungNull(pNode, nStart);

				GoMAR2(pNode, nStart, mStart, lStart);
			}
		}
		else
		{
			LoggingFunction(pNode, "MAR1_4_00010+ ");
			//6n : C(n) = Null → MAR2로 MAR1_4_00010
			ReplaceHangulJongsungNull(pNode, nStart);

			GoMAR2(pNode, nStart, mStart, lStart);

		}
	}
	else
	{
		//5n : #C(n)+S(n-1 ~ 1)이 ending 사전에 있는가 ? MAR1_5_00001
		GoMAR1_5(pNode, nStart, mStart, lStart);
	}
}

void CNAHangulAnaly::GoMAR1_5(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
	m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)의 음소
	m_nRestorationJongChar = Restoration(m_pJongChar);//복원해야 할 종성값 #C(n)
	m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
	m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
	m_pEndingDicWord = AddTwoStr(m_pJongChar, m_pFormNTo1Word);//C(n)+S(n-1~1)
	LoggingFunction(pNode, "MAR1_5_00001+ ");
	if(IsExistEndingDic(m_pEndingDicWord, m_endingFeature01))
	{//5n : #C(n)+S(n-1~1)이 ending 사전에 있는가 ? MAR1_5_00001
		m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
		m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
		LoggingFunction(pNode, "MAR1_5_00002+ ");
		if (IsExistStemDic(m_pStemDicDeleteJongWord, m_stemFeature01))
		{//6y : (S(len~n)-C(n))이 stem 사전에 있는가 ? MAR1_5_00002
			if(m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_PPO] == IS_CEHCK_FEATURE)
			{//7y : {(S(len~n)-C(n)) == [N] && C(n)+S(n-1~1) == [Ppo]}인가 ? MAR1_5_00003
				LoggingFunction(pNode, "MAR1_5_00004+ ");
				m_p1thAddString = AddTwoStr("이", m_pEndingDicWord);
				//8y : stem = S(len~n)-C(n), ending = “이”[Pol]+C(n)+S(n-1~1) MAR1_5_00004
				InsertStemString(pNode, m_pStemDicDeleteJongWord);
				SaveHTokenStem(pNode);
				InsertEndingString(pNode, m_p1thAddString);
				SaveHTokenEnding(pNode);
				AddHTokenEnding(pNode, ENDING_DIC_POL);//[Pol]자질부여
			}
			else
			{
				LoggingFunction(pNode, "MAR1_5_00005+ ");
				if((m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] == IS_CEHCK_FEATURE)
					|| (m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] != IS_CEHCK_FEATURE)
					|| (m_stemFeature01[STEM_DIC_AMB] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_AMB] != IS_CEHCK_FEATURE))
				{//8n : {(S(len~n)-C(n)) == [N] && C(n) +S(n-1~1) == [N]}이거나, {(S(len~n)-C(n)) <> [N] && C(n) +S(n-1~1) <> [N]}이거나  {(S(len~n)-C(n)) == [amb] || C(n) +S(n-1~1) == [amb]}인가? MAR1_5_00005
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
					ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);//C(n) = “ㄹ”
					m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n-1~1)
					LoggingFunction(pNode, "MAR1_5_00007+ ");
					if (IsExistStemDic(m_pFormNTo1Word, m_stemFeature02))
					{//9n : C(n) = “ㄹ” , S(len~n)이 stem 사전에 있는가? MAR1_5_00007
						LoggingFunction(pNode, "MAR1_5_00008+ ");
						if((m_stemFeature02[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] == IS_CEHCK_FEATURE)
							|| (m_stemFeature02[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] != IS_CEHCK_FEATURE)
							|| (m_stemFeature02[STEM_DIC_AMB] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_AMB] != IS_CEHCK_FEATURE))
						{//10y :  {S(len~n) == [N] && C(n) +S(n-1~1) == [N]}이거나, {S(len~n)  <> [N] && C(n) +S(n-1~1) <> [N]}이거나  {S(len~n) == [amb] || C(n) +S(n-1~1) == [amb]}인가? MAR1_5_00008
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
							//원래의 종성값으로 복원
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
			ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);//C(n) = “ㄹ”
			m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
			LoggingFunction(pNode, "MAR1_5_00012+ ");
			if (IsExistStemDic(m_pStemDicDeleteJongWord, m_stemFeature01))
			{//7n : {S(len~n)-#C(n) } && C(n) = “ㄹ”→S(len~n)이 stem 사전에 있는가? MAR1_5_00012
				LoggingFunction(pNode, "MAR1_5_00013+ ");
				if((m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] == IS_CEHCK_FEATURE)
					|| (m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] != IS_CEHCK_FEATURE)
					|| (m_stemFeature01[STEM_DIC_AMB] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_AMB] != IS_CEHCK_FEATURE))
				{//8y : {S(len~n) == [N] && #C(n)+S(n-1~1) == [N]}이거나, {S(len~n) <> [N] && #C(n)+S(n-1~1) <> [N]}이거나  {S(len~n) == [amb] || #C(n)+S(n-1~1) == [amb]}인가? MAR1_5_00013
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
					//9n : C(n) = #C(n) , OldBuf() = C(n)+OldBuf(), S(n) = S(n)-C(n)→MAR2로 MAR1_5_00015
					CHAR_DELETE(m_pOldBuffer);
					int nLen = strlen(m_p1thAddString);
					m_pOldBuffer = new char[nLen + 1];
					memset(m_pOldBuffer, 0x00, nLen + 1);
					strcpy(m_pOldBuffer, m_p1thAddString);
					InsertArrayFeatureOld(m_p1thAddString);
					//종성값삭제
					ReplaceHangulJongsungNull(pNode, nStart);

					GoMAR2(pNode, nStart, mStart, lStart);
				}
			}
			else
			{
				//원래의 종성값으로 복원
				JasoRestoration(pNode, nStart, FORCE_REPLACE_ENDWORD, m_nRestorationJongChar);//C(n)=#C(n)
				IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
				m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
				m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)의 음절
				m_pJongChar = GetChojungJongsungChar(m_pOneNPluseNumChar, CHAR_JONGSUNG);//C(n+1)의 음소
				m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
				m_pEndingDicWord = AddTwoStr(m_pJongChar, m_pFormNTo1Word);//C(n+1)+S(n~1)
				LoggingFunction(pNode, "MAR1_5_00016+ ");
				if (IsExistEndingDic(m_pEndingDicWord, m_endingFeature01))
				{//8n : C(n) = #C(n) → C(n+1)+S(n~1)이 ending사전에 있는가? MAR1_5_00016
					m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
					m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pOneNPluseNumChar, m_nPluseSyllable);//S(len~n+1)-C(n+1)
					LoggingFunction(pNode, "MAR1_5_00017+ ");
					if (IsExistStemDic(m_pStemDicDeleteJongWord, m_stemFeature01))
					{//9y : S(len~n+1)-C(n+1)이 Stem 사전에 있는가? MAR1_5_00017
						LoggingFunction(pNode, "MAR1_5_00018+ ");
						if((m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] == IS_CEHCK_FEATURE)
							|| (m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] != IS_CEHCK_FEATURE)
							|| (m_stemFeature01[STEM_DIC_AMB] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_AMB] != IS_CEHCK_FEATURE))
						{//10y : {(S(len~n+1)-C(n+1)) == [N] && C(n+1) +S(n~1) == [N]}이거나, {(S(len~n+1)-C(n+1)) <> [N] && C(n+1) +S(n~1) <> [N]}이거나  {(S(len~n+1)-C(n+1)) == [amb] || C(n+1) +S(n~1) == [amb]}인가? MAR1_5_00018
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
							{//11n : S(n)-C(n) == txx인가? MAR1_5_00020
								LoggingFunction(pNode, "MAR1_5_00021_01+ ");
								IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
								m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
								m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)의 음소
								m_nRestorationJongChar = Restoration(m_pJongChar);//복원해야 할 종성값 #C(n)
								m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
								m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
								m_pEndingDicWord = AddTwoStr(m_pJongChar, m_pFormNTo1Word);//C(n)+S(n-1~1)

								CHAR_DELETE(m_pOldBuffer);
								int nLen = strlen(m_pJongChar);
								m_pOldBuffer = new char[nLen + 1];
								memset(m_pOldBuffer, 0x00, nLen + 1);
								strcpy(m_pOldBuffer, m_pEndingDicWord);
								//12y : OldBuf = C(n)+S(n-1~1), S(n) = S(n)-C(n) && IRM으로 MAR1_5_00021_01
								InsertArrayFeatureOld(m_pEndingDicWord);
								ReplaceHangulJongsungNull(pNode, nStart);

								GoIMR1(pNode, nStart, mStart, lStart);
							}
							else
							{
								LoggingFunction(pNode, "MAR1_5_00021_02+ ");
								
								IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
								m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
								m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)의 음소
								m_nRestorationJongChar = Restoration(m_pJongChar);//복원해야 할 종성값 #C(n)
								m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
								m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
								m_pEndingDicWord = AddTwoStr(m_pJongChar, m_pFormNTo1Word);//C(n)+S(n-1~1)
								CHAR_DELETE(m_pOldBuffer);
								int nLen = strlen(m_pJongChar);
								m_pOldBuffer = new char[nLen + 1];
								memset(m_pOldBuffer, 0x00, nLen + 1);
								strcpy(m_pOldBuffer, m_pEndingDicWord);
								//12n : OldBuf = C(n)+S(n-1~1), S(n) = S(n)-C(n) && MAR2로 ? MAR1_5_00021_02
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
						{//10n : S(n)-C(n) == txx인가? MAR1_5_00022
							LoggingFunction(pNode, "MAR1_5_00023+ ");
							
							IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
							m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
							m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)의 음소
							m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
							m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
							m_pEndingDicWord = AddTwoStr(m_pJongChar, m_pFormNTo1Word);//C(n)+S(n-1~1)
							CHAR_DELETE(m_pOldBuffer);
							int nLen = strlen(m_pEndingDicWord);
							m_pOldBuffer = new char[nLen + 1];
							memset(m_pOldBuffer, 0x00, nLen + 1);
							strcpy(m_pOldBuffer, m_pEndingDicWord);
							//11y : OldBuf = C(n)+S(n-1~1), S(n) = S(n)-C(n) && IRM으로 MAR1_5_00023
							InsertArrayFeatureOld(m_pEndingDicWord);

							ReplaceHangulJongsungNull(pNode, nStart);

							GoIMR2(pNode, nStart, mStart, lStart);
						}
						else
						{
							LoggingFunction(pNode, "MAR1_5_00024+ ");
							
							IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
							m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
							m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)의 음소
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
					IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화

					m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
					m_pChoJungChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_CHOJUNGSUNG);//S(n)-C(n)
					m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)의 음소
					m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
					m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
					bool bFindEnding = IsExistEndingDic(m_pJongChar, m_endingFeature01);
					bool bFindStem = IsExistStemDic(m_pStemDicDeleteJongWord, m_stemFeature01);
					LoggingFunction(pNode, "MAR1_5_00025+ ");
					if((bFindEnding == true && m_endingFeature01[ENDING_DIC_PRS] == IS_CEHCK_FEATURE) && bFindStem == false
						&&(strcmp(m_pChoJungChar,"으") == 0 || strcmp(m_pChoJungChar, "시") == 0 || strcmp(m_pChoJungChar, "하") == 0 || strcmp(m_pChoJungChar, "되") == 0 || strcmp(m_pChoJungChar, "이") == 0))
					{//9n : C(n) == [Prs] && S(len~n)-C(n)이 Stem사전에 없고, S(n)-C(n) != “으”||“시”||“하”||“되”||“이”지 확인하라. MAR1_5_00025
						LoggingFunction(pNode, "MAR1_5_00026+ ");

						//10y : Error(20030123 유혜원, “국내완구, 신생아완구”, “나이스디지털”, “바디오일) MAR1_5_00026
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
						{//10n : S(n)-C(n) == txx인가? MAR1_5_00027
							LoggingFunction(pNode, "MAR1_5_00028+ ");

							IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
							m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
							m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)의 음소
							m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
							m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
							m_pEndingDicWord = AddTwoStr(m_pJongChar, m_pFormNTo1Word);//C(n)+S(n-1~1)
							CHAR_DELETE(m_pOldBuffer);
							int nLen = strlen(m_pEndingDicWord);
							m_pOldBuffer = new char[nLen + 1];
							memset(m_pOldBuffer, 0x00, nLen + 1);
							strcpy(m_pOldBuffer, m_pEndingDicWord);
							//11y : OldBuf = C(n)+S(n-1~1), S(n) = S(n)-C(n) && IRM으로 MAR1_5_00028
							InsertArrayFeatureOld(m_pEndingDicWord);
							ReplaceHangulJongsungNull(pNode, nStart);

							GoIMR2(pNode, nStart, mStart, lStart);
						}
						else
						{
							LoggingFunction(pNode, "MAR1_5_00029+ ");

							IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
							m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
							m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)의 음소
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
		//6n : MAR2로(다시 수정함) MAR1_5_00030
		LoggingFunction(pNode, "MAR1_5_00030+ ");
		GoMAR2(pNode, nStart, mStart, lStart);
	}
}


/////////////////////////////////////////////////////////
//MAR2
/////////////////////////////////////////////////////////
void CNAHangulAnaly::GoMAR2(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
	LoggingFunction(pNode, "MAR2_00001+ ");
	lStart = 0;//초기화
	GoMAR2_1(pNode, nStart, mStart, lStart);
}

void CNAHangulAnaly::GoMAR2_1(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
	m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)의 음소
	LoggingFunction(pNode, "MAR2_1_00001+ ");
	if (m_bOneCharMask == false && strcmp(m_pNthOneChar, "려") == 0)
	{//①	 S(n) = [mask]가 아닌 “려”인가 ? MAR2_1_00001
		LoggingFunction(pNode, "MAR2_1_00002+ ");

		//0'y : MAR3로 MAR2_1_00002
		GoMAR3(pNode, nStart, mStart, lStart);
	}
	else
	{
		LoggingFunction(pNode, "MAR2_1_00003+ ");
		if((m_bOneCharMask == false || strcmp(m_pNthOneChar, "럼") != 0)
			&& ((CompareVariousTable01(m_pNthOneChar, 0, false) == true
				|| CompareVariousTable02(m_pNthOneChar, 1, false) == true || CompareVariousTable02(m_pNthOneChar, 2, false) == true
				|| CompareVariousTable03(m_pNthOneChar, 0, false) == true || CompareVariousTable03(m_pNthOneChar, 1, false) == true
				|| CompareVariousTable04(m_pNthOneChar, 1, false) == true || CompareVariousTable04(m_pNthOneChar, 2, false) == true || CompareVariousTable04(m_pNthOneChar, 3, false) == true
				|| CompareVariousTable05(m_pNthOneChar, ALL_TABLE_05, true) == true
				|| CompareVariousTable06(m_pNthOneChar, ALL_TABLE_06, true) == true
				|| CompareVariousTable07(m_pNthOneChar, ALL_TABLE_07, true) == true
				|| strcmp(m_pNthOneChar, "기") == 0 || strcmp(m_pNthOneChar, "우") == 0)
				|| (strcmp(m_pJongChar, "ㅆ") == 0 || strcmp(m_pJongChar, "ㅁ") == 0 || strcmp(m_pJongChar, "ㄻ") == 0)))
		{//0'n : S(n)이 [mask]||“럼”이 아니고, {S(n) = txx || “기” || “우” } || { C(n) == ㅆ || ㅁ || ㄻ } 인가? MAR2_1_00003
			LoggingFunction(pNode, "MAR2_1_00004+ ");

			//0y : IRM으로 MAR2_1_00004
			GoIMR1(pNode, nStart, mStart, lStart);
		}
		else
		{
			//0n : m = n이면 m = n - 1, *S(n~m+1)이 ending 사전에 있는가 ? MAR2_2_00001
			GoMAR2_2(pNode, nStart, mStart, lStart);
		}
	}
}

void CNAHangulAnaly::GoMAR2_2(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
	LoggingFunction(pNode, "MAR2_2_00001+ ");
	if (nStart == mStart)
	{
		mStart = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
	}
	m_pEndingDicWord = GetMiddleCountChar(pNode->pHToken->strSource, nStart, mStart, CALCULATE_STOP, CALCULATE_MINUS);//S(n~m+1)
	if (IsExistEndingDic(m_pEndingDicWord))
	{//0n : m = n이면 m = n - 1, *S(n~m+1)이 ending 사전에 있는가 ? MAR2_2_00001
		GoMAR2_3(pNode, nStart, mStart, lStart);
	}
	else
	{
		//1n : C(n+1)+S(n~m+1)이 ending 사전에 존재하는가 ? MAR2_4_00001
		GoMAR2_4(pNode, nStart, mStart, lStart);
	}
}

void CNAHangulAnaly::GoMAR2_3(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화

	lStart = nStart;

	m_pEndingDicWord = GetMiddleCountChar(pNode->pHToken->strSource, nStart, mStart, CALCULATE_STOP, CALCULATE_MINUS);//S(n~m+1)
	LoggingFunction(pNode, "MAR2_3_00001+ ");
	if (IsExistEndingDic(m_pEndingDicWord))
	{//1y: 1 = n → ② S(n~m+1)이 ending 사전에 있는가 ? MAR2_3_00001
		CHAR_DELETE(m_pNewBuffer);
		int nLen = strlen(m_pEndingDicWord);
		m_pNewBuffer = new char[nLen + 1];//NewBuf() = S(n~m+1)
		memset(m_pNewBuffer, 0x00, nLen + 1);
		strcpy(m_pNewBuffer, m_pEndingDicWord);
		InsertArrayFeatureNew(m_pEndingDicWord);
		m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
		m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)의 음절
		m_pJongChar = GetChojungJongsungChar(m_pOneNPluseNumChar, CHAR_JONGSUNG);//C(n+1)의 음소
		m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
		LoggingFunction(pNode, "MAR2_3_00002+ ");
		if (IsExistStemDic(m_pStemDicWord, m_stemFeature01))
		{//2y : NewBuf() = S(n~m+1) → S(len~n+1)이 stem 사전에 존재하는가 ? MAR2_3_00002
			CHAR_DELETE(m_pEndingDicWord);
			m_pEndingDicWord = AddTwoStr(m_pJongChar, m_pEndingDicWord);//C(n+1)+S(n~m+1)
			LoggingFunction(pNode, "MAR2_3_00003+ ");
			if (IsExistEndingDic(m_pEndingDicWord, m_endingFeature01))
			{//3y : C(n+1)+S(n~m+1)이 ending 사전에 존재하는가 ? MAR2_3_00003
				m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pOneNPluseNumChar, m_nPluseSyllable);//S(len~n+1)-C(n+1)
				LoggingFunction(pNode, "MAR2_3_00004+ ");
				if (IsExistStemDic(m_pStemDicDeleteJongWord, m_stemFeature01))
				{//4y : S(len~n+1)-C(n+1)이 stem 사전에 존재하는가 ? MAR2_3_00004
					LoggingFunction(pNode, "MAR2_3_00005+ ");
					if((m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] == IS_CEHCK_FEATURE)
						|| (m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] != IS_CEHCK_FEATURE)
						|| (m_stemFeature01[STEM_DIC_AMB] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_AMB] != IS_CEHCK_FEATURE))
					{//5y : {S(len~n+1)-C(n+1) == [N] && C(n+1)+S(n~m+1) == [N]} ∥{ S(len~n+1)-C(n+1) <>[N] && C(n+1)+S(n~m+1) <>[N] } || {S(len~n+1)-C(n+1) == [amb] || C(n+1)+S(n~m+1) == [amb]}인가 ? MAR2_3_00005
						LoggingFunction(pNode, "MAR2_3_00006+ ");
						if((m_endingFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_pOldBuffer[ENDING_DIC_N] == IS_CEHCK_FEATURE)
							|| (m_endingFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_pOldBuffer[ENDING_DIC_N] != IS_CEHCK_FEATURE)
							|| (m_endingFeature01[STEM_DIC_AMB] != IS_CEHCK_FEATURE && m_pOldBuffer[ENDING_DIC_AMB] != IS_CEHCK_FEATURE))
						{//6y : {C(n+1)+S(n~m+1) == [n] && Oldbuf == [n]} || {C(n+1)+S(n~m+1) <>[n] && Oldbuf <>[n]} || {C(n+1)+S(n~m+1) == [amb] || Oldbuf == [amb]}인가 ? MAR2_3_00006
							CHAR_DELETE(m_pStemDicWord);
							m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
							LoggingFunction(pNode, "MAR2_3_00007+ ");
							bool bFindEndingFirst = IsExistEndingDic(m_pNewBuffer, m_endingFeature02);
							bool bFindEndingSecond = IsExistEndingDic(m_pOldBuffer, m_endingFeature03);
							if((strcmp(m_pStemDicWord, "자신") == 0 || strcmp(m_pStemDicWord, "분") == 0)
								&& bFindEndingFirst == true && bFindEndingSecond == true  && (m_endingFeature02[ENDING_DIC_N] == IS_CEHCK_FEATURE || m_endingFeature02[ENDING_DIC_AMB] == IS_CEHCK_FEATURE || m_endingFeature02[ENDING_DIC_N] == IS_CEHCK_FEATURE || m_endingFeature02[ENDING_DIC_AMB] == IS_CEHCK_FEATURE))
							{//7y : {S(len~n+1) == “자신” || “분”} && {(NewBuf() == [N] || [amb]) || (OldBuf() == [N] || [amb])}인가 ? MAR2_3_00007
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
								&& (m_stemFeature01[STEM_DIC_V] != IS_CEHCK_FEATURE && m_stemFeature01[STEM_DIC_AMB] != IS_CEHCK_FEATURE) && strcmp(m_pFormNTo1Word,"과도") != 0)
							{//7n : OldBuf() = NewBuf+OldBuf(), NewBuf()=Null → stem = S(len~n+1), S(n~1)이 stem 사전에 있고[v] || [amb] || “과도”가 아닌가 ? MAR2_3_00010
								LoggingFunction(pNode, "MAR2_3_00011+ ");
								//8y : 복합명사 stem = S(n~1), ->end MAR2_3_00011 복합명사라 지정된 경우, 명사 두개 저장
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
						{//6n : NewBuf()의 항 == [N] && OldBuf()의 항 == [V]인가 ? MAR2_3_00013
							LoggingFunction(pNode, "MAR2_3_00014+ ");

							//7y : Error(20030128 유혜원 수정, “신혼가구”) MAR2_3_00014
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
								&& (m_stemFeature01[STEM_DIC_V] != IS_CEHCK_FEATURE && m_stemFeature01[STEM_DIC_AMB] != IS_CEHCK_FEATURE) && strcmp(m_pFormNTo1Word, "과도") != 0)
							{//7n : OldBuf() = NewBuf+OldBuf(), NewBuf()=Null → stem = S(len~n+1), S(n~1)이 stem 사전에 있고[v] || [amb] || “과도”가 아닌가 ? MAR2_3_00010
								LoggingFunction(pNode, "MAR2_3_00016+ ");
								//8y : 복합명사 stem = S(n~1), ->end MAR2_3_00011 복합명사라 지정된 경우, 명사 두개 저장
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
						&& (m_stemFeature01[STEM_DIC_V] != IS_CEHCK_FEATURE && m_stemFeature01[STEM_DIC_AMB] != IS_CEHCK_FEATURE) && strcmp(m_pFormNTo1Word, "과도") != 0)
					{//5n : OldBuf() = NewBuf+OldBuf() → stem = S(len~n+1), S(n~1)이 stem 사전에 있고[v] || [amb] || “과도”가 아닌가 ? MAR2_3_00018
						LoggingFunction(pNode, "MAR2_3_00019+ ");
						//6y : 복합명사 stem = S(n~1), ->end MAR2_3_00019 복합명사라 지정된 경우, 명사 두개 저장
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
				IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
				m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
				m_pFormLenToNWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
				bool bFindSecond = IsExistStemDic(m_pFormLenToNWord, m_stemFeature01);
				bool bFindNewBuffer = IsExistStemDic(m_pNewBuffer, m_stemFeature02);
				bool bFindOldBuffer = IsExistStemDic(m_pOldBuffer, m_stemFeature03);
				LoggingFunction(pNode, "MAR2_3_00021+ ");
				if((m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_stemFeature02[ENDING_DIC_N] == IS_CEHCK_FEATURE)
					|| (m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_stemFeature02[ENDING_DIC_N] != IS_CEHCK_FEATURE)
					|| (m_stemFeature01[STEM_DIC_AMB] != IS_CEHCK_FEATURE && m_stemFeature02[ENDING_DIC_AMB] != IS_CEHCK_FEATURE))
				{//4n : {S(len~n+1) == [N] && NewBuf() == [N]} || {S(len~n+1)<>[N] && NewBuf()<>[N]} || { S(len~n+1) == [Amb]} || { NewBuf() == [Amb]}인가 ? MAR2_3_00021
					LoggingFunction(pNode, "MAR2_3_00022+ ");
					if((m_stemFeature02[ENDING_DIC_N] == IS_CEHCK_FEATURE && m_stemFeature03[ENDING_DIC_V] == IS_CEHCK_FEATURE)
						|| m_stemFeature02[ENDING_DIC_PPR] == IS_CEHCK_FEATURE
						|| (strcmp(m_pNewBuffer, "도") == 0 && (m_pOldBuffer != NULL && strcmp(m_pOldBuffer, "구") == 0))
						|| (strcmp(m_pNewBuffer, "차") && (m_pOldBuffer != NULL && strcmp(m_pOldBuffer, "정") == 0)))
					{//5y : {NewBuf()의 항 == [N] && OldBuf()의 항 == [V]} || {NewBuf()의 항 == [Pps]} || {NewBuf()의 항 == “도” && OldBuf() == “구”} || {NewBuf()의 항 == “차” && OldBuf() == “도”} || {NewBuf()의 항 == “지” && OldBuf() == “정”}인가 ? MAR2_3_00022
						LoggingFunction(pNode, "MAR2_3_00023+ ");

						//6y : Error(20030128 유혜원 수정, “포장이사” “해외이사”, “기저귀가게”, “조리도구”, “전국지도를”) MAR2_3_00023
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
							&& strcmp(m_pFormNTo1Word, "별로") != 0 && strcmp(m_pFormNTo1Word, "측은") != 0 && strcmp(m_pFormNTo1Word, "측이") != 0)
						{//6n : OldBuf() = NewBuf+OldBuf(), NewBuf()=Null → stem = S(len~n+1), S(n~1)이 stem 사전에 있고, { S(len~n+1) == [N] } && {S(n~1) == [N] || [Adv]} && {S(n~1) != “별로”} && {S(n~1) != “측은”} && {S(n~1) != “측이”}인가 ? MAR2_3_00024
							LoggingFunction(pNode, "MAR2_3_00025+ ");
							//7y : 복합명사 stem = S(n~1), ->end MAR2_3_00025 복합명사라 지정된 경우, 명사 두개 저장
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
			IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
			m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
			m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)의 음절
			LoggingFunction(pNode, "MAR2_3_00028+ ");
			IsExistEndingDic(m_pNewBuffer, m_endingFeature01);//m_pNewBuffer의 자질액세스
			if((strcmp(m_pOneNPluseNumChar, "하") != 0 && strcmp(m_pOneNPluseNumChar, "으") != 0) && (m_pNewBuffer != NULL && strcmp(m_pNewBuffer, "시")) && m_endingFeature01[ENDING_DIC_PRS] == IS_CEHCK_FEATURE || m_endingFeature01[ENDING_DIC_PSS] == IS_CEHCK_FEATURE)
			{//3n: NOT{ S(n+1) == “하”|| “으” && NewBuf() == “시”} && NewBuf() == [Prs]||[Pss]인가? MAR2_3_00028
				LoggingFunction(pNode, "MAR2_3_00029+ ");

				//4y: Error  MAR2_3_00029
				GoErrorFunction(pNode);
			}
			else
			{
				LoggingFunction(pNode, "MAR2_3_00030(n++→n>len)+ ");
				int nMovingLeftPosition = ReturnNumberMovingLeftPositionWord(pNode->pHToken->strSource,nStart);
				if(nMovingLeftPosition < 0)
				{//4n: n++→n>len? MAR2_3_00030
					LoggingFunction(pNode, "MAR2_3_00031+ ");

					//5y: Error MAR2_3_00031
					GoErrorFunction(pNode);
				}
				else
				{
					LoggingFunction(pNode, "MAR2_3_00032+ ");
					//5n: <삭제> -> ①로 가라. MAR2_3_00032 종성삭제하라는 의미임(?) 삭제의 의미 파악 안됨
					ReplaceHangulJongsungNull(pNode, nStart);

					GoMAR2_1(pNode, nStart, mStart, lStart);
				}
			}
		}
	}
	else
	{ 
		IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
		int nMovingLeftPosition = ReturnNumberMovingLeftPositionWord(pNode->pHToken->strSource, nStart);
		LoggingFunction(pNode, "MAR2_3_00033(n++->n>len)+ ");
		if (nMovingLeftPosition <= 0)
		{//2n: n++ → n > len인가? MAR2_3_00033
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
			{//3n:  S(n~n-1)이 ending 사전에 있는가? MAR2_3_00035
				LoggingFunction(pNode, "MAR2_3_00036+ ");
				m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1)의 음절
				//4y : NewBuf() = S(n-1)+NewBuf() → ①로 가라. MAR2_3_00036
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
				//(n + 1 > len이면, Error)
				if(m_nPluseSyllable == nStart)
				{
					LoggingFunction(pNode, "MAR2_3_00037 n+1 > len이면, Error + ");
					GoErrorFunction(pNode);
				}
				else
				{
					LoggingFunction(pNode, "MAR2_3_00037+ ");
					if (IsExistStemDic(m_pFormLenToNWord))
					{//4n : S(n+1~n-1)이 Ending 사전에 있는가?(n+1 > len이면, Error) MAR2_3_00037
						LoggingFunction(pNode, "MAR2_3_00038+ ");
						//5y : NewBuf() = S(n-1)+NewBuf() → ①로 가라. MAR2_3_00038
						m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1)의 음절
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
						//(n + 1 > len이면, Error)
						if (m_nPluseSyllable == nStart)
						{
							LoggingFunction(pNode, "MAR2_3_00039 n+1 > len이면, Error + ");
							GoErrorFunction(pNode);
						}
						else
						{
							LoggingFunction(pNode, "MAR2_3_00039+ ");
							if (IsExistStemDic(m_pFormLenToNWord))
							{ //5n : S(n+2~n-2)가 ending 사전에 있는가? (n+1 > len이면, Error) MAR2_3_00039
								LoggingFunction(pNode, "MAR2_3_00040+ ");
								//6y : NewBuf() = S(n-1)+NewBuf() → ①로 가라. MAR2_3_00040
								m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
								m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1)의 음절
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

								//6n : Errorr  유혜원 수정 “도예강사”, “명품감상”, “고주파치료기” MAR2_3_00041
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
	IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
	m_pEndingDicWord = GetMiddleCountChar(pNode->pHToken->strSource, nStart, mStart, CALCULATE_STOP, CALCULATE_MINUS);//S(n~m+1)
	m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
	m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)의 음절
	m_pJongChar = GetChojungJongsungChar(m_pOneNPluseNumChar, CHAR_JONGSUNG);//C(n+1)의 음소
	m_pFormLenToNWord = AddTwoStr(m_pJongChar, m_pEndingDicWord);//C(n+1)+S(n~m+1)
	LoggingFunction(pNode, "MAR2_4_00001+ ");
	if (IsExistEndingDic(m_pFormLenToNWord))
	{//1n : C(n+1)+S(n~m+1)이 ending 사전에 존재하는가 ? MAR2_4_00001
		m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
		m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pOneNPluseNumChar, m_nPluseSyllable);//S(len~n+1)-C(n+1)
		LoggingFunction(pNode, "MAR2_4_00002+ ");
		if (IsExistEndingDic(m_pStemDicDeleteJongWord))
		{//2y : S(len~n+1)-C(n+1)이 stem 사전에 존재하는가 ? MAR2_4_00002
			LoggingFunction(pNode, "MAR2_4_00003+ ");
			//3y : stem = S(len~n+1)-C(n+1), ending = C(n+1)+S(n~m+1)+OldBuf()→end MAR2_4_00003
			InsertStemString(pNode, m_pStemDicDeleteJongWord);
			SaveHTokenStem(pNode);
			InsertEndingString(pNode, m_pFormLenToNWord);
			SaveHTokenEnding(pNode);
		}
		else
		{
			IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
			m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
			m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)의 음소
			m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
			m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, mStart, 1, CALCULATE_MINUS);//m+1
			m_pFormLenToNWord = GetMiddleCountChar(pNode->pHToken->strSource, m_nPluseSyllable, m_nMinusSyllable, CALCULATE_STOP, CALCULATE_STOP);//S(n-1~m+1)
			m_pEndingDicWord = AddTwoStr(m_pJongChar, m_pFormLenToNWord);//C(n)+S(n-1~m+1)
			LoggingFunction(pNode, "MAR2_4_00004+ ");
			if (IsExistEndingDic(m_pFormLenToNWord))
			{//3n : C(n)+S(n-1~m+1)가 ending 사전에 존재하는가 ? MAR2_4_00004
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
				m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
				LoggingFunction(pNode, "MAR2_4_00005+ ");
				if(IsExistStemDic(m_pStemDicDeleteJongWord))
				{//4y : S(len~n)-C(n)이 stem 사전에 MAR2_4_00006 ? MAR2_4_00005
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
					//5n : OldBuf = C(n)+S(n-1~m+1)+OldBuf(), S(n) = S(n)-C(n), m=m+1 → MA_02() ①로 MAR2_4_00007
					m_p1thAddString = AddTwoStr(m_pJongChar, m_pFormLenToNWord);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					CHAR_DELETE(m_pOldBuffer);
					int nLen = strlen(m_p2thAddString);
					m_pOldBuffer = new char[nLen + 1];
					memset(m_pOldBuffer, 0x00, nLen + 1);
					strcpy(m_pOldBuffer, m_p2thAddString);
					InsertArrayFeatureOld(m_p2thAddString);

					ReplaceHangulJongsungNull(pNode, nStart);//S(n)=S(n)-C(n) 종성삭제
					ReturnNumberMovingLeftPositionWord(pNode->pHToken->strSource, mStart);//m=m+1
					
					GoMAR2_1(pNode,nStart,mStart,lStart);
				}
			}
			else
			{
				IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
				LoggingFunction(pNode, "MAR2_4_00008+ ");
				m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
				m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)의 음절
				m_pJongChar = GetChojungJongsungChar(m_pOneNPluseNumChar, CHAR_JONGSUNG);//C(n+1)의 음소
				m_pFormLenToNWord = AddTwoStr(m_pJongChar, m_pEndingDicWord);//C(n+1)+S(n~m+1)
				//4n : ending = C(n+1)+S(n~m+1)+OldBuf() , tmp_stem = S(len~n+1)-C(n+1) → MA_02() ①로 MAR2_4_00008
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
		IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
		m_pEndingDicWord = GetMiddleCountChar(pNode->pHToken->strSource, nStart, mStart, CALCULATE_PLUS, CALCULATE_MINUS);//S(n-1~m+1)
		m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
		m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)의 음소
		m_pFormLenToNWord = AddTwoStr(m_pJongChar, m_pEndingDicWord);//C(n)+S(n-1~m+1)
		LoggingFunction(pNode, "MAR2_4_00009+ ");
		if (IsExistEndingDic(m_pFormLenToNWord, m_endingFeature01))
		{//2n : C(n)+S(n-1~m+1)가 ending 사전에 존재하는가? MAR2_4_00009
			LoggingFunction(pNode, "MAR2_4_00010+ ");
			if(m_endingFeature01[ENDING_DIC_PSS] == IS_CEHCK_FEATURE)
			{//3y : C(n)+S(n-1~m+1) == [Pss]인가?(20030128, 유혜원 수정) MAR2_4_00010
				LoggingFunction(pNode, "MAR2_4_00011+ ");

				//4y : Error(“피부관리”) MAR2_4_00011
				GoErrorFunction(pNode);
			}
			else
			{
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
				m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
				LoggingFunction(pNode, "MAR2_4_00012+ ");
				if (IsExistStemDic(m_pStemDicDeleteJongWord))
				{//4n : S(len~n)-C(n)이 stem 사전에 존재하는가? MAR2_4_00012
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
					LoggingFunction(pNode, "MAR2_4_00014( m = m+1 ->①)+ ");
					m_p1thAddString = AddTwoStr(m_pJongChar, m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p2thAddString, m_pOldBuffer);
					//5n : OldBuf = C(n)+NewBuf()+OldBuf(), S(n) = S(n)-C(n), NewBuf()=Null, m = m+1 → MA_02() ①로 MAR2_4_00014
					CHAR_DELETE(m_pOldBuffer);
					CHAR_DELETE(m_pNewBuffer);
					int nLen = strlen(m_p2thAddString);
					m_pOldBuffer = new char[nLen + 1];
					memset(m_pOldBuffer, 0x00, nLen + 1);
					strcpy(m_pOldBuffer, m_p2thAddString);
					InsertArrayFeatureOld(m_p2thAddString);

					ReplaceHangulJongsungNull(pNode, nStart);//S(n)=S(n)-C(n) 종성삭제

					ReturnNumberMovingLeftPositionWord(pNode->pHToken->strSource, mStart);//m=m+1
					
					GoMAR2_1(pNode, nStart, mStart, lStart);
				}
			}
		}
		else
		{
			LoggingFunction(pNode, "MAR2_4_00015+ ");
			if (IsExistEndingDic(m_pJongChar))
			{//3n : C(n)이 ending 사전에 있는가?(뛸 듯이, 벌인다는데) MAR2_4_00015
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
				m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
				LoggingFunction(pNode, "MAR2_4_00016+ ");
				if (IsExistStemDic(m_pStemDicDeleteJongWord, m_stemFeature01))
				{//4y : S(len~n)-C(n)이 stem 사전에 있는가? MAR2_4_00016
					LoggingFunction(pNode, "MAR2_4_00017+ ");
					if ((strcmp(m_pJongChar, "ㄴ") == 0 || strcmp(m_pJongChar, "ㄹ") == 0) && m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE)
					{//5y : {C(n) == “ㄴ”||“ㄹ”} && {S(len~n-C(n) == [N]}인가? MAR2_4_00017
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
					IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
					LoggingFunction(pNode, "MAR2_4_00020+ ");
					m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
					if (IsExistEndingDic(m_pNthOneChar, m_endingFeature01))
					{//5n : S(n)이 ending 사전에 있는가? MAR2_4_00020
						m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
						m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)의 음절
						m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
						LoggingFunction(pNode, "MAR2_4_00021+ ");
						if (IsExistStemDic(m_pStemDicDeleteJongWord, m_stemFeature01))
						{//6y : S(len~n+1)이 stem 사전에 있는가? MAR2_4_00021
							LoggingFunction(pNode, "MAR2_4_00022+ ");
							if((m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] == IS_CEHCK_FEATURE) || (m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] != IS_CEHCK_FEATURE))
							{//7y : {S_S(len~n+1) == [N] && E_S(n) == [N]} || {S_S(len~n+1) != [N] && E_S(n) != [N]}인가? MAR2_4_00022
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
				IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
				m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
				m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
				m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)의 음절
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
				LoggingFunction(pNode, "MAR2_4_00027+ ");
				if (IsExistStemDic(m_pStemDicWord, m_stemFeature01))
				{//4n : S(len~n+1)이 stem 사전에 있는가? MAR2_4_00027
					m_p1thAddString = AddTwoStr(m_pNthOneChar,m_pNewBuffer);
					LoggingFunction(pNode, "MAR2_4_00028+ ");
					if (IsExistStemDic(m_p1thAddString))
					{//5y : S(n)+NewBuf()가 stem 사전에 있는가? MAR2_4_00028
						LoggingFunction(pNode, "MAR2_4_00029+ ");

						//6y : Error MAR2_4_00029
						GoErrorFunction(pNode);
					}
					else
					{
						m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
						bool bFind = IsExistStemDic(m_pFormNTo1Word, m_stemFeature02);
						LoggingFunction(pNode, "MAR2_4_00030+ ");
						if(strcmp(m_pFormNTo1Word,"과도") != 0 && bFind == true && (m_stemFeature02[STEM_DIC_V] != IS_CEHCK_FEATURE && m_stemFeature02[STEM_DIC_AMB] != IS_CEHCK_FEATURE))
						{//6n : Stem = S(len~n+1), S(n~1) != “과도” && S(n~1)이 stem 사전에 있고 [v] || [amb]가 아닌가? MAR2_4_00030
							LoggingFunction(pNode, "MAR2_4_00031+ ");
							if(m_stemFeature01[STEM_DIC_V] == IS_CEHCK_FEATURE)
							{//7y : S(len~n+1) == [V]인가? MAR2_4_00031
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
								//8n : 복합명사 stem = S(n~1),-> end MAR2_4_00033 복합명사라 지정된 경우, 명사 두개 저장
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
							{//7n : S(n)이 ending사전에 있는가? MAR2_4_00034
								IsExistEndingDic(m_pNewBuffer, m_endingFeature02);//m_pNewBuffer의 자질액세스
								LoggingFunction(pNode, "MAR2_4_00035+ ");
								if((m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature02[STEM_DIC_N] == IS_CEHCK_FEATURE)
									|| (m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature02[STEM_DIC_N] != IS_CEHCK_FEATURE))
								{//8y : {S(len~n+1) == [N] && NewBuf() == [N]} || {S(len~n+1) <> [N] && NewBuf() <> [N]}인가? MAR2_4_00035
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
					{//5n : ③ NewBuf() ==Null? MAR2_4_00039
						LoggingFunction(pNode, "MAR2_4_00040+ ");

						//6y : MAR3 MAR2_4_00040
						GoMAR3(pNode, nStart, mStart, lStart);
					}
					else
					{
						bool bSameWrod = IsSameWordPos(pNode, lStart);
						LoggingFunction(pNode, "MAR2_4_00041+ ");
						if(bSameWrod == true)
						{//6n : l == last(of NewBuf)인가? MAR2_4_00041
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
							{//7y : OldBuf() = NewBuf()+OldBuf() , NewBuf()=Null → m = last(of Old Buf) → *S(n~m+1)이 ending사전에 있는가? MAR2_4_00042
								LoggingFunction(pNode, "MAR2_4_00043(l=n → ②)+ ");
								//8y : l =n → ②로 가라 MAR2_4_00043
								lStart = nStart;

								GoMAR2_3(pNode,nStart,mStart,lStart);
							}
							else
							{
								CHAR_DELETE(m_pEndingDicWord);
								m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
								LoggingFunction(pNode, "MAR2_4_00044+ ");
								if (IsExistStemDic(m_pStemDicWord))
								{//8n : S(len~n)이 stem 사전에 있는가 MAR2_4_00044
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

									//9n : MAR3으로 MAR2_4_00046
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
							//→ n = last(of OldBuf)+1, m = last (of OldBuf) MAR2_4_00048
							m_pEndingDicWord = GetMiddleCountChar(pNode->pHToken->strSource, nStart, mStart, CALCULATE_STOP, CALCULATE_MINUS);//S(n~m+1)
							if (IsExistEndingDic(m_pEndingDicWord))
							{//→ *S(n~m+1)이 ending 사전에 존재하는가? MAR2_4_00049
								LoggingFunction(pNode, "MAR2_4_00050( l =n → ②)+ ");
								//8y : l =n → ②로 가라 MAR2_4_00050
								lStart = nStart;

								GoMAR2_3(pNode, nStart, mStart, lStart);
							}
							else
							{
								LoggingFunction(pNode, "MAR2_4_00051+ ");
								//8n : ending = OldBuf(), MAR3으로 MAR2_4_00051
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
	IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
	LoggingFunction(pNode, "MAR3_00001+ ");
	if(strcmp(m_pNthOneChar, "럼") == 0)
	{//S(n) == “럼”인가 ? MAR3_00001
		LoggingFunction(pNode, "MAR3_00002+ ");
		//0y : S(n) = [mask], ->MAR1의  ②로 MAR3_00002
		SetOneWordMarsk();
		GoMAR1_2(pNode, nStart, mStart, lStart);
	}
	else
	{
		LoggingFunction(pNode, "MAR3_00003+ ");
		if (strcmp(m_pNthOneChar, "여") == 0)
		{//0n : S(n) == “여”인가 ? MAR3_00003
			m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
			m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)의 음절
			m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
			m_p1thAddString = AddTwoStr(m_pStemDicWord,"이");
			bool bFind = IsExistStemDic(m_p1thAddString, m_stemFeature01);
			LoggingFunction(pNode, "MAR3_00004+ ");
			if(bFind == true && (m_stemFeature01[STEM_DIC_V] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE))
			{//1y : S(len~n+1) + “이” 가 stem 사전에 있고, S(len~n+1) + “이” == [V] || [amb]인가 ? MAR3_00004
				LoggingFunction(pNode, "MAR3_00005+ ");
				m_p2thAddString = AddTwoStr("어",m_pOldBuffer);
				//2y : stem = (len~n+1) + “이”, ending = “어” + OldBuf()  MAR3_00005
				InsertStemString(pNode, m_p1thAddString);//stem = (len~n+1) + “이”
				SaveHTokenStem(pNode);
				InsertEndingString(pNode, m_p2thAddString);//ending = “어” + OldBuf()
				SaveHTokenEnding(pNode);
			}
			else
			{
				LoggingFunction(pNode, "MAR3_00006+ ");
				if(IsExistStemDic(m_pStemDicWord, m_stemFeature02))
				{//2n : S(len~n+1)이 stem 사전에 있는가? MAR3_00006
					LoggingFunction(pNode, "MAR3_00007+ ");
					if(strcmp(m_pOneNPluseNumChar, "하") == 0 && m_stemFeature02[STEM_DIC_V] == IS_CEHCK_FEATURE)
					{//3y : S(n+1) == “하” && S(len~n+1) == [V]인가? MAR3_00007
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
						{//4n : S(n) == S(1)인가? MAR3_00009
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

							//5n : MAR1의 ②로 MAR3_00011
							GoMAR1_2(pNode, nStart, mStart, lStart);
						}
					}
				}
				else
				{
					LoggingFunction(pNode, "MAR3_00012+ ");

					//3n : MAR1의 ②로 MAR3_00012
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
	IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
	LoggingFunction(pNode, "MAR3_1_00001+ ");
	if(strcmp(m_pNthOneChar, "다") == 0 || strcmp(m_pNthOneChar, "야") == 0 || strcmp(m_pNthOneChar, "여") == 0)
	{//1n : S(n) == “다” || “야” || “여”인가 ? MAR3_1_00001
		m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
		m_pEndingDicWord = GetMiddleCountChar(pNode->pHToken->strSource, nStart, m_nMinusSyllable, CALCULATE_STOP, CALCULATE_STOP);//S(n~n-1)
		m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
		m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)의 음절
		m_pJongChar = GetChojungJongsungChar(m_pOneNPluseNumChar, CHAR_JONGSUNG);//C(n+1)의 음소
		m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
		bool bFindEndingDic = IsExistEndingDic(m_pEndingDicWord);
		bool bFindStemDic = IsExistStemDic(m_pStemDicWord);
		LoggingFunction(pNode, "MAR3_1_00002+ ");
		if(strcmp(m_pNthOneChar, "다") == 0 && bFindStemDic == true && bFindStemDic == true)
		{//2y : S(n) == “다”이고, S(n~n-1)이 ending 사전에 있고, S(len~n+1)이 stem 사전에 있는가 ? MAR3_1_00002
			m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
			m_pFormLenToNWord = GetMiddleCountChar(pNode->pHToken->strSource, nStart, m_nMinusSyllable, CALCULATE_STOP, CALCULATE_STOP);//S(n~n-1)
			m_p1thAddString = AddTwoStr(m_pJongChar,m_pEndingDicWord);//C(n+1)+S(n~n-1)
			m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pOneNPluseNumChar, m_nPluseSyllable);//S(len~n+1)-C(n+1)
			bool bFindEndingDic = IsExistEndingDic(m_p1thAddString);
			bool bFindStemDic = IsExistEndingDic(m_pStemDicDeleteJongWord);
			LoggingFunction(pNode, "MAR3_1_00003+ ");
			if(bFindEndingDic == true && bFindStemDic == true)
			{//3y : C(n+1)+S(n~n-1)이 ending사전에 있고, S(len~n+1)-C(n+1)이 stem사전에 있는가 ? MAR3_1_00003
				LoggingFunction(pNode, "MAR3_1_00004+ ");
				//4y : S(len~n+1) - C(n+1), ending = C(n+1) + “다” + NewBuf()+OldBuf() MAR3_1_00004
				CHAR_DELETE(m_p1thAddString);
				m_p1thAddString = AddTwoStr(m_pJongChar, "다");
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
				//4n : Stem = S(len~n+1), ending = “다”+NewBuf()+OldBuf( MAR3_1_00005
				CHAR_DELETE(m_p1thAddString);
				m_p1thAddString = AddTwoStr("다", m_pNewBuffer);
				m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
				InsertStemString(pNode, m_pStemDicWord);
				SaveHTokenStem(pNode);
				InsertEndingString(pNode, m_p2thAddString);
				SaveHTokenEnding(pNode);
			}
		}
		else
		{
			ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA13);//C(n) = “ㅎ”
			m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
			m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)의 음소
			m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
			m_p1thAddString = AddTwoStr(m_pStemDicWord, m_pJongChar);
			LoggingFunction(pNode, "MAR3_1_00006+ ");
			if(IsExistStemDic(m_p1thAddString))
			{//3n : C(n) = ㅎ → S(len~n)+C(n) 이 stem 사전에 있는가? MAR3_1_00006
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
				//4n : C(n) = NULL → MAR1의 ②로 MAR3_1_00008
				ReplaceHangulJongsungNull(pNode, nStart);//S(n)=S(n)-C(n) 종성삭제

				GoMAR1_2(pNode, nStart, mStart, lStart);
			}
		}
	}
	else
	{
		LoggingFunction(pNode, "MAR3_1_00009+ ");
		if (strcmp(m_pNthOneChar, "이") == 0 || strcmp(m_pNthOneChar, "가") == 0)
		{//2n : S(n) == “이” || “가” 인가 MAR3_1_00009
			m_pLastOneChar = GetOneChar(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));//S(1)
			if(strcmp(m_pNthOneChar, m_pLastOneChar) == 0)
			{//3y : S(n) == S(1)인가? MAR3_1_00010
				LoggingFunction(pNode, "MAR3_1_00011+ ");
				
				//4y : MAR1의 ②로 MAR3_1_00011
				GoMAR1_2(pNode, nStart, mStart, lStart);
			}
			else
			{ 
				LoggingFunction(pNode, "MAR3_1_00012+ ");

				//4n : IRM으로 MAR3_1_00012
				GoIMR1(pNode, nStart, mStart, lStart);
			}
		}
		else
		{
			LoggingFunction(pNode, "MAR3_1_00013+ ");
			if (strcmp(m_pNthOneChar, "터") == 0)
			{//3n : S(n) == “터”인가 MAR3_1_00013
				m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
				m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)의 음절
				LoggingFunction(pNode, "MAR3_1_00014+ ");
				if(strcmp(m_pOneNPluseNumChar, "부") == 0)
				{//4y : S(n+1) == “부”인가? MAR3_1_00014
					m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
					m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+2)
					LoggingFunction(pNode, "MAR3_1_00015+ ");
					if (IsExistStemDic(m_pStemDicWord))
					{//5y : S(len~n+2)가 Stem 사전에 있는가? MAR3_1_00015
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
						LoggingFunction(pNode, "MAR3_1_00017(n++ → m=n → n++ → MAR2)+ ");
						//6n : OldBuf() = “부터”+OldBuf() → n++ → m=n → n++ → MAR2로 MAR3_1_00017
						m_p1thAddString = AddTwoStr("부터", m_pOldBuffer);
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

					//5n : IRM으로 MAR3_1_00018
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
	IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
	m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)의 음소
	m_nRestorationJongChar = Restoration(m_pJongChar);//복원해야 할 종성값
	LoggingFunction(pNode, "MAR3_2_00001+ ");
	if (strcmp(m_pJongChar, "ㄴ") == 0)
	{//4n : C(n) == “ㄴ”인가 ? MAR3_2_00001
		ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);//C(n) = “ㄹ”
		m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
		LoggingFunction(pNode, "MAR3_2_00002+ ");
		if (IsExistStemDic(m_pStemDicWord, m_stemFeature01))
		{//5y : C(n) = ㄹ → S(len~n)가 stem 사전에 있는가 ? MAR3_2_00002
			LoggingFunction(pNode, "MAR3_2_00003+ ");
			if(m_stemFeature01[STEM_DIC_LIR] == IS_CEHCK_FEATURE)
			{//6y : S(len~n)이 Lir인가 ? MAR3_2_00003
				LoggingFunction(pNode, "MAR3_2_00004+ ");
				m_p1thAddString = AddTwoStr("ㄴ", m_pNewBuffer);
				m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
				//7y : stem = S(len~n), ending = ㄴ + NewBuf()+OldBuf() MAR3_2_00004
				InsertStemString(pNode, m_pStemDicWord);
				SaveHTokenStem(pNode);
				InsertEndingString(pNode, m_p2thAddString);
				SaveHTokenEnding(pNode);
			}
			else
			{
				LoggingFunction(pNode, "MAR3_2_00005+ ");
				//7n : C(n) = ㄴ → IRM으로 MAR3_2_00005
				//원래의 종성값으로 복원
				JasoRestoration(pNode, nStart, FORCE_REPLACE_ENDWORD, m_nRestorationJongChar);// C(n) = #C(n)

				GoIMR1(pNode, nStart, mStart, lStart);
			}
		}
		else
		{
			LoggingFunction(pNode, "MAR3_2_00006+ ");
			//6n : C(n) = ㄴ → IRM으로 MAR3_2_00006
			//원래의 종성값으로 복원
			JasoRestoration(pNode, nStart, FORCE_REPLACE_ENDWORD, m_nRestorationJongChar);// C(n) = #C(n)

			GoIMR1(pNode, nStart, mStart, lStart);
		}
	}
	else
	{ 
		LoggingFunction(pNode, "MAR3_2_00007+ ");
		if (strcmp(m_pJongChar, "ㅂ") == 0)
		{//5n : C(n) == “ㅂ”인가 ? MAR3_2_00007
			ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);//C(n) = “ㄹ”
			m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
			LoggingFunction(pNode, "MAR3_2_00008+ ");
			if (IsExistStemDic(m_pStemDicWord, m_stemFeature01))
			{//6y : C(n) = ㄹ → S(len~n)가 stem 사전에 있는가 ? MAR3_2_00008
				LoggingFunction(pNode, "MAR3_2_00009+ ");
				if (m_stemFeature01[STEM_DIC_LIR] == IS_CEHCK_FEATURE)
				{//7y : S(len~n)이 Lir인가 ? MAR3_2_00009
					LoggingFunction(pNode, "MAR3_2_00010+ ");
					m_p1thAddString = AddTwoStr("ㅂ", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					//8y : stem = S(len~n), ending = ㅂ + NewBuf()+OldBuf() MAR3_2_00010
					InsertStemString(pNode, m_pStemDicWord);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_p2thAddString);
					SaveHTokenEnding(pNode);

				}
				else
				{
					LoggingFunction(pNode, "MAR3_2_00011+ ");
					//8n : C(n) = ㅂ → Error MAR3_2_00011
					//원래의 종성값으로 복원
					JasoRestoration(pNode, nStart, FORCE_REPLACE_ENDWORD, m_nRestorationJongChar);// C(n) = #C(n)

					GoErrorFunction(pNode);
				}
			}
			else
			{
				LoggingFunction(pNode, "MAR3_2_00012+ ");
				//7n : C(n) = ㅂ → Error MAR3_2_00012
				//원래의 종성값으로 복원
				JasoRestoration(pNode, nStart, FORCE_REPLACE_ENDWORD, m_nRestorationJongChar);// C(n) = #C(n)

				GoErrorFunction(pNode);
			}
		}
		else
		{
			LoggingFunction(pNode, "MAR3_2_00013+ ");
			if (strcmp(m_pNthOneChar, "려") == 0)
			{//6n : S(n)이 “려”인가 ? MAR3_2_00013
				CHAR_DELETE(m_pJongChar);
				m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
				m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)의 음절
				m_pJongChar = GetChojungJongsungChar(m_pOneNPluseNumChar, CHAR_JONGSUNG);//C(n+1)의 음소
				LoggingFunction(pNode, "MAR3_2_00014+ ");
				if(strcmp(m_pOneNPluseNumChar, "알") != 0 &&
					(strcmp(m_pJongChar, "ㄹ") == 0 || strcmp(m_pJongChar, "ㅀ") == 0
					|| strcmp(m_pOneNPluseNumChar, "꾸") == 0 || strcmp(m_pOneNPluseNumChar, "어") == 0 || strcmp(m_pOneNPluseNumChar, "그") == 0
					|| strcmp(m_pOneNPluseNumChar, "내") == 0 || strcmp(m_pOneNPluseNumChar, "때") == 0 || strcmp(m_pOneNPluseNumChar, "추") == 0
					|| strcmp(m_pOneNPluseNumChar, "드") == 0 || strcmp(m_pOneNPluseNumChar, "거") == 0 || strcmp(m_pOneNPluseNumChar, "다") == 0
					|| strcmp(m_pOneNPluseNumChar, "뿌") == 0 || strcmp(m_pOneNPluseNumChar, "느") == 0 || strcmp(m_pOneNPluseNumChar, "버") == 0
					|| strcmp(m_pOneNPluseNumChar, "꺼") == 0 || strcmp(m_pOneNPluseNumChar, "부”") == 0))
				{//7y : S(n+1) != “알” && C(n+1) == “ㄹ” || “ㅀ” || S(n+1) == “꾸” || “어” || “그” || “내” || “때” || “추” || “드” || “거” || “다” || “뿌” || “느” || “버” || “꺼” || “부” 인가 ? MAR3_2_00014
					m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
					m_pAddTwoString = AddTwoStr(m_pStemDicWord,"리");
					LoggingFunction(pNode, "MAR3_2_00015+ ");
					if (IsExistStemDic(m_pAddTwoString))
					{//8y : S(len~n+1)+“리”가 stem 사전에 있는가 MAR3_2_00015
						LoggingFunction(pNode, "MAR3_2_00016+ ");
						m_p1thAddString = AddTwoStr("어", m_pNewBuffer);
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
						//9y : stem = S(len~n+1)+“리”, ending = “어” + NewBuf()+OldBuf()  MAR3_2_00016
						InsertStemString(pNode, m_pAddTwoString);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, m_p2thAddString);
						SaveHTokenEnding(pNode);
					}
					else
					{
						LoggingFunction(pNode, "MAR3_2_00017+ ");
						//9n : S(n) = [mask], MAR2로 MAR3_2_00017
						SetOneWordMarsk();

						GoMAR2(pNode, nStart, mStart, lStart);
					}
				}
				else
				{
					m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
					LoggingFunction(pNode, "MAR3_2_00018+ ");
					if (IsExistStemDic(m_pStemDicWord, m_stemFeature01))
					{//8n :  S(len~n+1)이 stem 사전에 있는가? MAR3_2_00018
						LoggingFunction(pNode, "MAR3_2_00019+ ");
						if(m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE)
						{//9y : S(len~n+1) <> [N]인가? MAR3_2_00019
							LoggingFunction(pNode, "MAR3_2_00020+ ");
							m_p1thAddString = AddTwoStr("려", m_pNewBuffer);
							m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
							//10y : stem = S(len~n+1),  ending = “려”+NewBuf() +OldBuf() MAR3_2_00020
							InsertStemString(pNode, m_pStemDicWord);
							SaveHTokenStem(pNode);
							InsertEndingString(pNode, m_p2thAddString);
							SaveHTokenEnding(pNode);
						}
						else
						{
							LoggingFunction(pNode, "MAR3_2_00021+ ");
							//10n : S(n) = [mask], MAR1_2로 MAR3_2_00021
							SetOneWordMarsk();

							GoMAR1_2(pNode, nStart, mStart, lStart);
						}
					}
					else
					{
						LoggingFunction(pNode, "MAR3_2_0002+ ");
						//9n : S(n) = [mask], MAR1_2로 MAR3_2_0002
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
	IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
	m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)의 음소
	LoggingFunction(pNode, "MAR3_3_00001+ ");
	if (strcmp(m_pNthOneChar, "큼") == 0)
	{//7n : S(n) == “큼”인가 ? MAR3_3_00001
		m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
		m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)의 음절
		m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
		m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+2)
		LoggingFunction(pNode, "MAR3_3_00002+ ");
		if(strcmp(m_pOneNPluseNumChar, "만") == 0 && IsExistStemDic(m_pStemDicWord))
		{//8y : S(n+1)이 “만”이고, S(len~n+2)가 stem 사전에 있는가 ? MAR3_3_00002
			LoggingFunction(pNode, "MAR3_3_00003+ ");
			m_p1thAddString = AddTwoStr("만큼", m_pNewBuffer);
			m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
			//9y : Stem = S(len~n+2), ending = “만큼” + NewBuf()+OldBuf() MAR3_3_00003
			InsertStemString(pNode, m_pStemDicWord);
			SaveHTokenStem(pNode);
			InsertEndingString(pNode, m_p2thAddString);
			SaveHTokenEnding(pNode);
		}
		else
		{
			LoggingFunction(pNode, "MAR3_3_00004+ ");
			//9n : S(n) = [mask], MAR2로 MAR3_3_00004
			SetOneWordMarsk();

			GoMAR2(pNode, nStart, mStart, lStart);
		}
	}
	else
	{
		LoggingFunction(pNode, "MAR3_3_00005+ ");
		if(m_pJongChar == NULL || strcmp(m_pJongChar,"") == 0)
		{//8n : C(n) == Null인가 ? MAR3_3_00005
			CHAR_DELETE(m_pNthOneChar);
			m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
			m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1)의 음절
			LoggingFunction(pNode, "MAR3_3_00006+ ");
			if(strcmp(m_pNthOneChar, "는") == 0 || strcmp(m_pNthOneChar, "시") == 0 || strcmp(m_pNthOneChar, "셔") == 0
				|| strcmp(m_pNthOneChar, "셨") == 0 || strcmp(m_pNthOneChar, "오") == 0 || strcmp(m_pNthOneChar, "온") == 0)
			{//9y : S(n-1)이 “는” || “시” || “셔” || “셨” || “오” || “온” 인가 ? MAR3_3_00006
				ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);//C(n) = “ㄹ”
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
				LoggingFunction(pNode, "MAR3_3_00007+ ");
				if (IsExistStemDic(m_pStemDicWord, m_stemFeature01))
				{//10y : C(n)="ㄹ" → S(len~n)가 stem 사전에 있는가 ? MAR3_3_00007
					LoggingFunction(pNode, "MAR3_3_00008+ ");
					if (m_stemFeature01[STEM_DIC_LIR] == IS_CEHCK_FEATURE)
					{//11y : S(len~n)이 Lir인가 ? MAR3_3_00008
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
						//12n : C(n) = Null → Error MAR3_3_00010
						ReplaceHangulJongsungNull(pNode, nStart);

						GoErrorFunction(pNode);
					}
				}
				else
				{
					LoggingFunction(pNode, "MAR3_3_00011+ ");
					//11n : C(n) = Null → Error MAR3_3_00011
					ReplaceHangulJongsungNull(pNode, nStart);

					GoErrorFunction(pNode);
				}
			}
			else
			{
				LoggingFunction(pNode, "MAR3_3_00012+ ");
				//10n : C(n) = Null → Error MAR3_3_00012
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
	IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
	LoggingFunction(pNode, "IMR1_00001+ ");
	if (strcmp(m_pNthOneChar, "케") == 0)
	{//S(n) == “케” 인가 ? IMR1_00001
		m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
		m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
		m_pAddTwoString = AddTwoStr(m_pStemDicWord, "하");//S(len~n+1)+“하”
		LoggingFunction(pNode, "IMR1_00002+ ");
		if (IsExistStemDic(m_pAddTwoString))
		{//1y : (S(len~n+1)+“하”)가 stem 사전에 있는가 ? IMR1_00003
			LoggingFunction(pNode, "IMR1_00002+ ");
			if (m_pOldBuffer == NULL || strcmp(m_pOldBuffer,"") == 0)
			{//2y : stem = S(len~n+1)+“하” → OldBuf()가 Null인가 ? IMR1_00003
				LoggingFunction(pNode, "IMR1_00004+ ");
				m_p1thAddString = AddTwoStr("게", m_pNewBuffer);
				//3y : ending = “게” + [V][Aff] + NewBuf() IMR1_00004
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
				//3n : ending = “게” + [V][Aff] + NewBuf()+OldBuf() IMR1_00005
				m_p1thAddString = AddTwoStr("게", m_pNewBuffer);
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
			{//2n : S(len~n+1)가 stem 사전에 있고, S(len~n+1) != [Conc]인가 ? (20030128 유혜원 수정) IMR1_00006
				LoggingFunction(pNode, "IMR1_00007+ ");
				if (m_pOldBuffer == NULL || strcmp(m_pOldBuffer, "") == 0)
				{//3y : stem = S(len~n+1)+“하” + [V][Lex] - [N] → OldBuf()가 Null인가 ? IMR1_00007
					LoggingFunction(pNode, "IMR1_00008+ ");
					m_p1thAddString = AddTwoStr("게", m_pNewBuffer);
					//4y : ending = “게” + [V][Aff] + NewBuf() IMR1_00008
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
					m_p1thAddString = AddTwoStr("게", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					//4n : ending = “게” + [V][Aff] + NewBuf()+OldBuf() IMR1_00009
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
	IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
	LoggingFunction(pNode, "IMR1_1_00001+ ");
	if (strcmp(m_pNthOneChar, "치") == 0)
	{ //1n : S(n)이 “치”인가 ? IMR1_1_00001
		CHAR_DELETE(m_pNthOneChar);
		m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
		m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1)의 음절
		m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
		m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
		bool bFind = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
		LoggingFunction(pNode, "IMR1_1_00002+ ");
		if(strcmp(m_pNthOneChar, "고") == 0 && (bFind == true && (m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE)))
		{//2y : {S(n-1) == “고”} && {S(len~n+1) == [N] || [Amb]} 인가 ? IMR1_1_00002
			LoggingFunction(pNode, "IMR1_1_00003+ ");
			//3y : S(n) = [mask] → MAR1의 ②로 IMR1_1_00003
			SetOneWordMarsk();

			GoMAR1_2(pNode, nStart, mStart, lStart);
		}
		else
		{
			CHAR_DELETE(m_pNthOneChar);
			m_pAddTwoString = AddTwoStr(m_pStemDicWord, "하");//S(len~n+1)+“하”
			m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
			m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+2)의 음절
			LoggingFunction(pNode, "IMR1_1_00004+ ");
			if(IsExistStemDic(m_pAddTwoString) && (m_pNthOneChar == NULL || strcmp(m_pNthOneChar, "") == 0 || strcmp(m_pNthOneChar, "도") == 0))
			{//3n : (S(len~n+1)+“하”)가 stem 사전에 있고, S(n+2) == “도” || NULL인가 ? IMR1_1_00004
				if (m_pOldBuffer == NULL || strcmp(m_pOldBuffer, "") == 0)
				{//4y : stem = S(len~n+1)+“하”[V][Lex] - [N] → OldBuf()이 Null인가 ? IMR1_1_00005
					LoggingFunction(pNode, "IMR1_1_00006+ ");
					m_p1thAddString = AddTwoStr("지", m_pNewBuffer);
					//5y : ending = “지” + [V][Aff] + NewBuf() IMR1_1_00006
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
					m_p1thAddString = AddTwoStr("지", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					//5n : ending = “지” + [V][Aff] + NewBuf()+OldBuf() IMR1_1_00007
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
				if ((bFind == true && m_stemFeature01[STEM_DIC_CONC] != IS_CEHCK_FEATURE) && (m_pNthOneChar == NULL || strcmp(m_pNthOneChar, "") == 0 || strcmp(m_pNthOneChar, "도") == 0))
				{//4n : S(len~n+1)이 stem 사전에 있고, S(len~n+1) != [Conc]이고, S(n+2) == “도” || NULL인가 ? IMR1_1_00008
					if (m_pOldBuffer == NULL || strcmp(m_pOldBuffer, "") == 0)
					{//5y : stem = S(len~n+1)+“하” + [V][Lex] - [N] → OldBuf()이 Null인가 ? IMR1_1_00009
						m_p1thAddString = AddTwoStr("지", m_pNewBuffer);
						//6y : ending = “지” + [V][Aff] + NewBuf() IMR1_1_00010
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
						m_p1thAddString = AddTwoStr("지", m_pNewBuffer);
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
						//6n : ending = “지” + [V][Aff] + NewBuf()+OldBuf() IMR1_1_00011
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
	IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
	LoggingFunction(pNode, "IMR1_2_00001+ ");
	if (strcmp(m_pNthOneChar, "록") == 0)
	{//2n : S(n)이 “록”인가 ? IMR1_2_00001
		CHAR_DELETE(m_pNthOneChar);
		m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
		m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)의 음절
		LoggingFunction(pNode, "IMR1_2_00002+ ");
		if (strcmp(m_pNthOneChar, "토") == 0)
		{//3y : S(n+1)이 “토”인가 ? IMR1_2_00002
			m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
			m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+2)
			m_pAddTwoString = AddTwoStr(m_pStemDicWord, "하");//S(len~n+2)+“하”
			LoggingFunction(pNode, "IMR1_2_00003+ ");
			if (IsExistStemDic(m_pAddTwoString))
			{//4y : (S(len~n+2)+“하”)가 stem 사전에 있는가 ? IMR1_2_00003
				LoggingFunction(pNode, "IMR1_2_00004+ ");
				if (m_pOldBuffer == NULL || strcmp(m_pOldBuffer, "") == 0)
				{//5y : stem = S(len~n+2)+“하”[V][Lex] - [N] → OldBuf()가 Null인가 ? IMR1_2_00004
					LoggingFunction(pNode, "IMR1_2_00005+ ");
					m_p1thAddString = AddTwoStr("도록", m_pNewBuffer);
					//6y : ending = “도록” + [V][Aff][Con][Rsu] + NewBuf() IMR1_2_00005
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
					m_p1thAddString = AddTwoStr("도록", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					//6n : “도록” + [V][Aff][Con][Rsu] + NewBuf()+OldBuf() IMR1_2_00006
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
				{//5n : S(len~n+2)가 stem 사전에 있고, S(len~n+2) != [Conc]인가 ? (20030128유혜원 수정) IMR1_2_00007
					LoggingFunction(pNode, "IMR1_2_00008+ ");
					if (m_pOldBuffer == NULL || strcmp(m_pOldBuffer, "") == 0)
					{//6y : stem = S(len~n+2)+“하” || “되” + [V][Lex] - [N] → OldBuf()가 Null인가 ? IMR1_2_00008 : "하"를 기본으로 취하게 함
						LoggingFunction(pNode, "IMR1_2_00009+ ");
						m_p1thAddString = AddTwoStr("도록", m_pNewBuffer);
						//7y : ending = “도록” + [V][Aff][Con][Rsu] + NewBuf() IMR1_2_00009
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
						
						m_p1thAddString = AddTwoStr("도록", m_pNewBuffer);
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
						//7n : ending = “도록” + [V][Aff][Con][Rsu] + NewBuf()+OldBuf() IMR1_2_00010
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
			if (strcmp(m_pNthOneChar, "도") == 0)
			{//4n : S(n+1)이 “도”인가 ? IMR1_2_00012
				m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+2)
				LoggingFunction(pNode, "IMR1_2_00013+ ");
				if(IsExistStemDic(m_pStemDicWord))
				{//5y : S(len~n+2)이 stem 사전에 있는가 ? IMR1_2_00013
					LoggingFunction(pNode, "IMR1_2_00014+ ");
					m_p1thAddString = AddTwoStr("도록", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					//6y : stem = S(len~n+2), ending = “도록” + [V][Aff][Con][Rsu] + NewBuf()+OldBuf() IMR1_2_00014
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
					m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+2)의 음절
					LoggingFunction(pNode, "IMR1_2_00015+ ");
					if(strcmp(m_pNthOneChar, "하") == 0 || strcmp(m_pNthOneChar, "되") == 0)
					{//6n : S(n+2)가 “하”||“되”인가? IMR1_2_00015
						CHAR_DELETE(m_pStemDicWord);
						m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 3, CALCULATE_MINUS);//n+3
						m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+3)
						LoggingFunction(pNode, "IMR1_2_00016+ ");
						if (IsExistStemDic(m_pStemDicWord))
						{//7y : S(len~n+3)이 stem 사전에 있는가? IMR1_2_00016
							LoggingFunction(pNode, "IMR1_2_00017+ ");
							if (m_pOldBuffer == NULL || strcmp(m_pOldBuffer, "") == 0)
							{//8y : stem = S(len~n+3)+“하”||“되”+[V][Lex]-[N] → OldBuf( )가 Null인가? IMR1_2_00017
								LoggingFunction(pNode, "IMR1_2_00018+ ");
								m_p1thAddString = AddTwoStr("도록", m_pNewBuffer);
								m_pAddTwoString = AddTwoStr(m_pStemDicWord, m_pNthOneChar);
								//9y : ending = “도록”+[V][Aff][Con][Rsu]+NewBuf()  IMR1_2_00018
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
								m_p1thAddString = AddTwoStr("도록", m_pNewBuffer);
								m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
								//9n : ending = “도록”+[V][Aff][Con][Rsu]+NewBuf()+OldBuf( ) IMR1_2_00019
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
							m_p1thAddString = AddTwoStr("도록", m_pNewBuffer);
							m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
							//8n : ending =“도록”+[V][Aff][Con][Rsu]+NewBuf()+OldBuf( )
							//stem = S(len~n+2) → Error IMR1_2_00020 stem를 세팅하고 에러함수를 호출하는지 의미가 불명확
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

						//7n : S(n) = [mask] -> MAR1의 ②로(추가됨) IMR1_2_00021
						SetOneWordMarsk();
						GoMAR1_2(pNode, nStart, mStart, lStart);
					}
				}
			}
			else
			{
				LoggingFunction(pNode, "IMR1_2_00022+ ");

				//5n :  S(n) = [mask] -> MAR1의 ②로 IMR1_2_00022
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
	IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
	m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)의 음소
	LoggingFunction(pNode, "IMR1_3_00001+ ");
	if(strcmp(m_pNthOneChar, "있") == 0)
	{//3n : S(n)이 “있”인가 ? IMR1_3_00001
		m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
		m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
		LoggingFunction(pNode, "IMR1_3_00002+ ");
		if(IsExistStemDic(m_pStemDicWord))
		{//4y : S(len~n+1)이 stem 사전에 있는가 ? IMR1_3_00002
			LoggingFunction(pNode, "IMR1_3_00003+ ");
			//5y : stem = S(len~n+1) + “있” + [V][Sta] - [N], ending = NewBuf()+OldBuf() IMR1_3_00003
			m_pAddTwoString = AddTwoStr(m_pStemDicWord,"있");
			m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
			InsertStemString(pNode, m_pAddTwoString);//stem = S(len~n+1) + “있”
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
		if (strcmp(m_pNthOneChar, "없") == 0)
		{//4n : S(n)이 “없”인가 ? IMR1_3_00005
			m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
			m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
			LoggingFunction(pNode, "IMR1_3_00006+ ");
			if (IsExistStemDic(m_pStemDicWord))
			{//5y : S(len~n+1)이 stem 사전에 있는가 ? IMR1_3_00006
				LoggingFunction(pNode, "IMR1_3_00007+ ");
				//6y : stem = S(len~n) + “없” + [V][Sta][neg] - [N], ending = NewBuf()+OldBuf() IMR1_3_00007
				CHAR_DELETE(m_pStemDicWord);
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
				m_pAddTwoString = AddTwoStr(m_pStemDicWord, "없");
				m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
				InsertStemString(pNode, m_pAddTwoString);//stem = S(len~n+1) + “없”
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
			if (strcmp(m_pNthOneChar, "셔") == 0)
			{//5n : S(n) 이 “셔”인가 ? IMR1_3_00009
				CHAR_DELETE(m_pNthOneChar);
				m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
				m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n+1)의 음절
				LoggingFunction(pNode, "IMR1_3_00010+ ");
				if(strcmp(m_pNthOneChar,"으") ==0)
				{//6y : S(n+1)이 “으”인가 ? IMR1_3_00010
					CHAR_DELETE(m_pStemDicWord);
					m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
					m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+2)
					LoggingFunction(pNode, "IMR1_3_00011+ ");
					if (IsExistStemDic(m_pStemDicWord))
					{//7y : S(len~n+2)가 stem 사전에 있는가 ? IMR1_3_00011
						LoggingFunction(pNode, "IMR1_3_00012+ ");
						m_p1thAddString = AddTwoStr("으시어", m_pNewBuffer);
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
						//8y : stem = S(len~n+2), ending = “으시어” + NewBuf()+OldBuf() IMR1_3_00012
						InsertStemString(pNode, m_pStemDicWord);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, m_p2thAddString);
						SaveHTokenEnding(pNode);
					}
					else
					{
						LoggingFunction(pNode, "IMR1_3_00013+ ");
						//8n : ending = “으시어” + NewBuf()+OldBuf(), n=n+2->MAR2로 IMR1_3_00013
						m_p1thAddString = AddTwoStr("으시어", m_pNewBuffer);
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
					m_pAddTwoString = AddTwoStr(m_pStemDicWord, "시");//S(len~n+1)+“시”
					LoggingFunction(pNode, "IMR1_3_00014+ ");
					if (IsExistStemDic(m_pAddTwoString))
					{//7n : S(len~n+1) + “시”가 stem 사전에 존재하는가 ? IMR1_3_00014
						LoggingFunction(pNode, "IMR1_3_00015+ ");
						m_p1thAddString = AddTwoStr("어", m_pNewBuffer);
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
						//8y : stem = S(len~n+1) + “시”, ending = “어” + NewBuf()+OldBuf() IMR1_3_00015
						InsertStemString(pNode, m_pAddTwoString);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, m_p2thAddString);
						SaveHTokenEnding(pNode);
					}
					else
					{
						LoggingFunction(pNode, "IMR1_3_00016+ ");
						if (IsExistStemDic(m_pStemDicWord))
						{//8n : S(len~n+1)이 stem 사전에 존재하는가? IMR1_3_00016
							m_p1thAddString = AddTwoStr("시어", m_pNewBuffer);
							m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
							//9y : stem = S(len~n+1), ending = “시어”+ NewBuf()+OldBuf() IMR1_3_00017
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

							MovingLeftPositionWord(pNode->pHToken->strSource, nStart);//n++ → n > len
							InitOneWordMarsk();

							m_p1thAddString = AddTwoStr("시어", m_pNewBuffer);
							m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
							
							InsertEndingString(pNode, m_p2thAddString);
							SaveHTokenEnding(pNode);
							
							LoggingFunction(pNode, "IMR1_3_00018+ ");
							if (nStart <= 0)
							{//9n : ending = “시어”+NewBuf()+OldBuf(), n++ → n > len? IMR1_3_00018
								LoggingFunction(pNode, "IMR1_3_00019+ ");

								//10y : Error IMR1_3_00019
								GoErrorFunction(pNode);//1'y : Error MAR1_1_00002
							}
							else
							{	
								LoggingFunction(pNode, "IMR1_3_00020+ ");
								if((strcmp(m_pNthOneChar, "하") == 0 || strcmp(m_pNthOneChar, "되") == 0 || strcmp(m_pNthOneChar, "받") == 0) && IsExistStemDic(m_pStemDicWord))
								{//10n : S(n+1) == “하” || “되” || “받” && S(len~n+2)가 Stem 사전에 있는가? IMR1_3_00020
									LoggingFunction(pNode, "IMR1_3_00021+ ");
									m_p1thAddString = AddTwoStr("시어", m_pNewBuffer);
									m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
									//11y : Stem = S(len~n+1), Ending =“시어”+ NewBuf()+OldBuf()  IMR1_3_00021
									InsertStemString(pNode, m_pStemDicWord);
									SaveHTokenStem(pNode);
									InsertEndingString(pNode, m_p2thAddString);
									SaveHTokenEnding(pNode);
								}
								else
								{
									//11n : S(n) = [mask]  → MAR2로 IMR1_3_00022
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
				if (strcmp(m_pNthOneChar, "키") == 0)
				{//6n : S(n) 이 “키”인가? IMR1_3_00023
					m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
					m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
					m_pAddTwoString = AddTwoStr(m_pStemDicWord, "하");//S(len~n+1)+“하”
					bool bFind = IsExistStemDic(m_pAddTwoString, m_stemFeature01);
					LoggingFunction(pNode, "IMR1_3_00024+ ");
					if (bFind == true)
					{//7y : {S(len~n+1)+“하”}가 stem사전에 있는가? IMR1_3_00024
						LoggingFunction(pNode, "IMR1_3_00025+ ");
						if (m_pOldBuffer == NULL || strcmp(m_pOldBuffer, "") == 0)
						{//8y : stem = S(len~n+1)+“하”[VN][Lex]-[N] → OldBuf()이 Null인가? IMR1_3_00025
							LoggingFunction(pNode, "IMR1_3_00026+ ");
							m_p1thAddString = AddTwoStr("기", m_pNewBuffer);
							//9y : ending = “기”+[V][Aff]+NewBuf() IMR1_3_00026
							InsertStemString(pNode, m_pAddTwoString);
							SaveHTokenStem(pNode);
							AddHTokenStem(pNode, STEM_DIC_V);
							AddHTokenStem(pNode, STEM_DIC_CMP);//VN->CMP로 변경함
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
							m_p1thAddString = AddTwoStr("기", m_pNewBuffer);
							m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
							//9n : ending = “기”+[V][Aff]+NewBuf()+OldBuf() IMR1_3_00027
							InsertStemString(pNode, m_pAddTwoString);
							SaveHTokenStem(pNode);
							AddHTokenStem(pNode, STEM_DIC_V);
							AddHTokenStem(pNode, STEM_DIC_CMP);//VN->CMP로 변경함
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
						{//8n : S(len~n+1)이 stem 사전에 있고, S(len~n+1) != [Conc]인가? IMR1_3_00028
							LoggingFunction(pNode, "IMR1_3_00029+ ");
							if (m_pOldBuffer == NULL || strcmp(m_pOldBuffer, "") == 0)
							{//9y : stem = S(len~n+1)+“하”[VN][Lex]-[N] → OldBuf()이 Null인가? IMR1_3_00029
								LoggingFunction(pNode, "IMR1_3_00030+ ");
								m_p1thAddString = AddTwoStr("기", m_pNewBuffer);
								//10y : ending = “기”+[V][Aff]+NewBuf() IMR1_3_00030
								InsertStemString(pNode, m_pAddTwoString);
								SaveHTokenStem(pNode);
								AddHTokenStem(pNode, STEM_DIC_V);
								AddHTokenStem(pNode, STEM_DIC_CMP);//VN->CMP로 변경함
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
								m_p1thAddString = AddTwoStr("기", m_pNewBuffer);
								m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
								//10n : ending = “기”+[V][Aff]+NewBuf()+OldBuf( ) IMR1_3_00031
								InsertStemString(pNode, m_pAddTwoString);
								SaveHTokenStem(pNode);
								AddHTokenStem(pNode, STEM_DIC_V);
								AddHTokenStem(pNode, STEM_DIC_CMP);//VN->CMP로 변경함
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
							m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)의 음절
							LoggingFunction(pNode, "IMR1_3_00032+ ");
							if(strcmp(m_pNthOneChar, "시") == 0)
							{//9n : S(n+1)이 “시”인가? IMR1_3_00032
								CHAR_DELETE(m_pStemDicWord);
								m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
								m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+2)
								LoggingFunction(pNode, "IMR1_3_00033+ ");
								if(IsExistStemDic(m_pStemDicWord))
								{//10y : S(len~n+2)가 stem 사전에 있는가? IMR1_3_00033
									LoggingFunction(pNode, "IMR1_3_00034+ ");
									m_pAddTwoString = AddTwoStr(m_pStemDicWord, "시키");
									m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
									//11y : stem = S(len~n+2) +“시키”+[V][Act]-[N], ending = NewBuf()+OldBuf() IMR1_3_00034
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
									//11n : S(n) = [mask]  → MAR1의 ②로 IMR1_3_00035
									SetOneWordMarsk();

									GoMAR1_2(pNode, nStart, mStart, lStart);
								}
							}
							else
							{
								LoggingFunction(pNode, "IMR1_3_00036+ ");
								//10n : S(n) = [mask]  → MAR1의 ②로 IMR1_3_00036
								SetOneWordMarsk();

								GoMAR1_2(pNode, nStart, mStart, lStart);
							}
						}
					}
				}
				else
				{
					LoggingFunction(pNode, "IMR1_3_00037+ ");
					if (strcmp(m_pNthOneChar, "하") == 0)
					{//7n : S(n) 이 “하”인가? IMR1_3_00037
						CHAR_DELETE(m_pNthOneChar);
						m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
						m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)의 음절	
						if(strcmp(m_pNthOneChar,"당") ==0)
						{//8y : S(n+1)이 “당”인가? IMR1_3_00038
							CHAR_DELETE(m_pStemDicWord);
							m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
							m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+2)
							LoggingFunction(pNode, "IMR1_3_00039+ ");
							if (IsExistStemDic(m_pStemDicWord))
							{//9y : S(len~n+2)가 stem 사전에 있는가? IMR1_3_00039
								LoggingFunction(pNode, "IMR1_3_00040+ ");
								m_pAddTwoString = AddTwoStr(m_pStemDicWord, "당하");
								m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
								//10y : stem = S(len~n+2)+“당하” +[V][Act]-[N], ending = NewBuf()+OldBuf() IMR1_3_00040
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
								//10n : S(n) = [mask]  → MAR1의 ②로 IMR1_3_00041
								SetOneWordMarsk();

								GoMAR1_2(pNode, nStart, mStart, lStart);
							}
						}
						else
						{
							LoggingFunction(pNode, "IMR1_3_00042+ ");
							//9n : S(n) = [mask]  → IRM4 로 IMR1_3_00042
							SetOneWordMarsk();

							GoIMR4(pNode, nStart, mStart, lStart);
						}
					}
					else
					{
						LoggingFunction(pNode, "IMR1_3_00043+ ");
						if (strcmp(m_pNthOneChar, "세") == 0)
						{//8n : S(n) 이 “세”인가? IMR1_3_00043
							CHAR_DELETE(m_pStemDicWord);
							m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
							m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
							LoggingFunction(pNode, "IMR1_3_00044+ ");
							if(strcmp(m_pStemDicWord, "계") == 0 || strcmp(m_pStemDicWord, "주무") == 0 || strcmp(m_pStemDicWord, "잡수") == 0)
							{//9y : S(len~n+1)이 “계”|| “주무”|| “잡수” 인가? IMR1_3_00044
								LoggingFunction(pNode, "IMR1_3_00045+ ");
								m_pAddTwoString = AddTwoStr(m_pStemDicWord, "시");
								m_p1thAddString = AddTwoStr("어", m_pOldBuffer);
								//10y : stem = S(len~n+1) +“시”, ending = “어”+ OldBuf() IMR1_3_00045
								InsertStemString(pNode, m_pStemDicWord);//S(len~n+1)의 사전에 존재한 자질값으로 세팅 후, "시"의 음절을 추가한다.
								SaveHTokenStem(pNode);
								InsertStemString(pNode, m_pAddTwoString);
								InsertEndingString(pNode, m_p1thAddString);
								SaveHTokenEnding(pNode);
							}
							else
							{
								//10n : S(n) = [mask] -> MAR1의 ②로 IMR1_3_00046
								SetOneWordMarsk();

								GoMAR1_2(pNode, nStart, mStart, lStart);
							}
						}
						else
						{
							LoggingFunction(pNode, "IMR1_3_00047+ ");
							if (strcmp(m_pJongChar, "ㅆ") == 0)
							{//9n : C(n)이 “ㅆ”인가 IMR1_3_00047
								//10y : IRM2로 IMR1_3_00048
								LoggingFunction(pNode, "IMR1_3_00048+ ");
								GoIMR2(pNode, nStart, mStart, lStart);
							}
							else
							{
								LoggingFunction(pNode, "IMR1_3_00049+ ");
								if ((strcmp(m_pJongChar, "ㅁ") == 0 || strcmp(m_pJongChar, "ㄻ") == 0) || strcmp(m_pNthOneChar, "기") == 0)
								{//10n : {C(n) == ㅁ || ㄻ} || {S(n) == “기”}인가? IMR1_3_00049
									LoggingFunction(pNode, "IMR1_3_00050+ ");
									//11y : IRM3으로   IMR1_3_00050

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
										strcmp(m_pNthOneChar, "답") == 0 || strcmp(m_pNthOneChar, "럽") == 0 || strcmp(m_pNthOneChar, "우") == 0)
									{//11n : S(n)이 txx || “답”||“럽”||“우” 인가? IMR1_3_00051
										LoggingFunction(pNode, "IMR1_3_00052+ ");
										//12y : IRM4로 IMR1_3_00052

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
	IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
	m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)의 음소
	LoggingFunction(pNode, "IMR2_00001+ ");
	if (strcmp(m_pNthOneChar, "었") == 0 || strcmp(m_pNthOneChar, "았") == 0 || strcmp(m_pNthOneChar, "겠") == 0)
	{//S(n) == “었” || “았” || “겠”인가 IMR2_00001
		m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
		m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
		LoggingFunction(pNode, "IMR2_00002+ ");
		if (IsExistStemDic(m_pStemDicWord))
		{//1y : S(len~n+1)이 stem 사전에 있는가 IMR2_00002
			LoggingFunction(pNode, "IMR2_00002+ ");
			m_p1thAddString = AddTwoStr(m_pNthOneChar,m_pNewBuffer);
			m_p2thAddString = AddTwoStr(m_p2thAddString, m_pOldBuffer);
			//2y : stem = S(len~n+1), ending = “었” || “았” || “겠” + NewBuf()+OldBuf() IMR2_00003
			InsertStemString(pNode, m_pStemDicWord);
			SaveHTokenStem(pNode);
			InsertStemString(pNode, m_pAddTwoString);
			InsertEndingString(pNode, m_p2thAddString);
			SaveHTokenEnding(pNode);
		}
		else
		{	
			//2n : OldBuf = “었” || “았” || “겠” + NewBuf()+OldBuf(), NewBuf()=Null IMR2_00004
			m_p1thAddString = AddTwoStr(m_pNthOneChar, m_pNewBuffer);
			m_p2thAddString = AddTwoStr(m_p1thAddString, m_pNewBuffer);
			CHAR_DELETE(m_pOldBuffer);
			int nLen = strlen(m_p2thAddString);
			m_pOldBuffer = new char[nLen + 1];
			memset(m_pOldBuffer, 0x00, nLen + 1);
			strcpy(m_pOldBuffer, m_p2thAddString);
			InsertArrayFeatureOld(m_p2thAddString);

			LoggingFunction(pNode, "IMR2_00005(m = n →  n++->n>le)+ ");
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
				//3n : MAR2로 IMR2_00007
				
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
	IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
	m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)의 음소
	LoggingFunction(pNode, "IMR2_1_00001+ ");
	if (strcmp(m_pNthOneChar, "였") == 0)
	{//1n : S(n)이 “였”인가 ? IMR2_1_00001
		CHAR_DELETE(m_pNthOneChar);
		m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
		m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)의 음절	
		m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
		if(strcmp(m_pNthOneChar, "하") == 0 || strcmp(m_pNthOneChar, "되") == 0)
		{//2y : S(n+1)이 “하” || “되”인가 ? IMR2_1_00002
			if (IsExistStemDic(m_pStemDicWord))
			{//3y : S(len~n+1)이 stem 사전에 존재하는가 ? IMR2_1_00003
				LoggingFunction(pNode, "IMR2_1_00004+ ");
				m_pAddTwoString = AddTwoStr("었", m_pNewBuffer);
				m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
				//4y : stem = S(len~n+1) - [N], ending = “었” + NewBuf()+OldBuf() IMR2_1_00004
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
				{//4n : S(len~n+2)가 stem 사전에 존재하는가 ? IMR2_1_00005
					LoggingFunction(pNode, "IMR2_1_00006+ ");
					m_pAddTwoString = AddTwoStr(m_pStemDicWord, m_pNthOneChar);
					m_p1thAddString = AddTwoStr("었", m_pOldBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pNewBuffer);

					//5y : stem = S(len~n+2) + “하” || “되” + [V] - [N], ending = “었” + NewBuf()+OldBuf() IMR2_1_00006
					InsertStemString(pNode, m_pStemDicWord);//S(len~n+1)의 사전에 존재한 자질값으로 세팅 후, "시"의 음절을 추가한다.
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
			m_pAddTwoString = AddTwoStr(m_pStemDicWord, "이");
			LoggingFunction(pNode, "IMR2_1_00008+ ");
			if (IsExistStemDic(m_pAddTwoString))
			{//3n : S(len~n+1) + “이”가 stem 사전에 존재하는가 ? IMR2_1_00008
				LoggingFunction(pNode, "IMR2_1_00009+ ");
				m_p1thAddString = AddTwoStr("었", m_pOldBuffer);
				m_p2thAddString = AddTwoStr(m_p1thAddString, m_pNewBuffer);
				//4y : stem = S(len~n+1) + “이”, ending = “었” + NewBuf()+OldBuf() IMR2_1_00009
				InsertStemString(pNode, m_pStemDicWord);//S(len~n+1)
				SaveHTokenStem(pNode);//S(len~n+1)의 자질정보세팅
				InsertStemString(pNode, m_pAddTwoString);//S(len~n+1) + “이”
				InsertEndingString(pNode, m_p2thAddString);
				SaveHTokenEnding(pNode);
			}
			else
			{
				LoggingFunction(pNode, "IMR2_1_00010+ ");
				if (IsExistStemDic(m_pStemDicWord))
				{//4n : stem = S(len~n+1)이 stem 사전에 존재하는가 ? IMR2_1_00010
					LoggingFunction(pNode, "IMR2_1_00011+ ");
					m_p1thAddString = AddTwoStr("이", "었");
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pNewBuffer);
					m_p3thAddString = AddTwoStr(m_p2thAddString, m_pOldBuffer);
					//5y : stem = S(len~n+1), ending = “이”[Pol] - [N] + “었” + NewBuf()+OldBuf() IMR2_1_00011
					InsertStemString(pNode, m_pStemDicWord);//S(len~n+1)
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_p3thAddString);
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, ENDING_DIC_POL);//[Pol]자질부여
				}
				else
				{
					//5n : OldBuf() = “었” + NewBuf()+OldBuf(), NewBuf()=Null, m = n, n++ → n > len ? IMR2_1_00012
					m_p1thAddString = AddTwoStr("었", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pNewBuffer);
					CHAR_DELETE(m_pOldBuffer);
					CHAR_DELETE(m_pNewBuffer);
					int nLen = strlen(m_p2thAddString);
					m_pOldBuffer = new char[nLen + 1];
					memset(m_pOldBuffer, 0x00, nLen + 1);
					strcpy(m_pOldBuffer, m_p2thAddString);
					InsertArrayFeatureOld(m_p2thAddString);
					
					LoggingFunction(pNode, "IMR2_1_00012(m = n →  n++->n>le)+ ");
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
						//6n : MAR2로 IMR2_1_00014

						GoMAR2(pNode, nStart, mStart, lStart);
					}

				}
			}
		}
	}
	else
	{
		LoggingFunction(pNode, "IMR2_1_00015+ ");
		if (strcmp(m_pNthOneChar, "셨") == 0)
		{//2n : S(n)이 “셨”인가 ? IMR2_1_00015
			CHAR_DELETE(m_pNthOneChar);
			m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
			m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)의 음절	
			LoggingFunction(pNode, "IMR2_1_00016+ ");
			if(strcmp(m_pNthOneChar, "으") == 0)
			{//3y : S(n+1)이 “으”인가 ? IMR2_1_00016
				m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+2)
				LoggingFunction(pNode, "IMR2_1_00017+ ");
				if (IsExistStemDic(m_pStemDicWord))
				{//4y : S(len~n+2)가 stem 사전에 있는가 ? IMR2_1_00017
					LoggingFunction(pNode, "IMR2_1_00018+ ");
					m_p1thAddString = AddTwoStr("으시", "었");
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pNewBuffer);
					m_p3thAddString = AddTwoStr(m_p2thAddString, m_pOldBuffer);
					//5y : stem = S(len~n+2), ending = “으시” + “었” + NewBuf()+OldBuf() IMR2_1_00018
					InsertStemString(pNode, m_pStemDicWord);//S(len~n+2)
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_p3thAddString);
					SaveHTokenEnding(pNode);
				}
				else
				{
					LoggingFunction(pNode, "IMR2_1_00019(n=n+2)+ ");
					//5n : ending = “으시” + “었” + NewBuf()+OldBuf(), n = n + 2->MAR2로 IMR2_1_00019
					m_p1thAddString = AddTwoStr("으시", "었");
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
				m_pAddTwoString = AddTwoStr(m_pStemDicWord,"시");
				LoggingFunction(pNode, "IMR2_1_00020+ ");
				if (IsExistStemDic(m_pAddTwoString))
				{//4n : S(len~n+1) + “시”가 stem 사전에 존재하는가 ? IMR2_1_00020
					m_p1thAddString = AddTwoStr("었", m_pOldBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pNewBuffer);
					//5y : stem = S(len~n+1) + “시”, ending = “었” + NewBuf()+OldBuf() IMR2_1_00021
					InsertStemString(pNode, m_pStemDicWord);//S(len~n+1)
					SaveHTokenStem(pNode);
					InsertStemString(pNode, m_pAddTwoString);
					InsertEndingString(pNode, m_p2thAddString);
					SaveHTokenEnding(pNode);
				
				}
				else
				{
					if (IsExistStemDic(m_pStemDicWord))
					{//5n : S(len~n+1)이 stem 사전에 존재하는가 ? IMR2_1_00022
						m_p1thAddString = AddTwoStr("시었", m_pOldBuffer);
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pNewBuffer);
						//6y : stem = S(len~n+1), ending = “시” + “었” + NewBuf()+OldBuf() IMR2_1_00023
						InsertStemString(pNode, m_pStemDicWord);//S(len~n+1)
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, m_p2thAddString);
						SaveHTokenEnding(pNode);
					}
					else
					{
						//6n: OldBuf() = “시” + “었” + NewBuf() + OldBuf(), NewBuf() = Null, n++ → n>len ? IMR2_1_00024
						m_p1thAddString = AddTwoStr("시었", m_pNewBuffer);
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
						CHAR_DELETE(m_pOldBuffer);
						CHAR_DELETE(m_pNewBuffer);
						int nLen = strlen(m_p2thAddString);
						m_pOldBuffer = new char[nLen + 1];
						memset(m_pOldBuffer, 0x00, nLen + 1);
						strcpy(m_pOldBuffer, m_p2thAddString);
						InsertArrayFeatureOld(m_p2thAddString);


						LoggingFunction(pNode, "IMR2_1_00024(m = n →  n++->n>le)+ ");
						
						int nMovingLeftPosition = ReturnNumberMovingLeftPositionWord(pNode->pHToken->strSource, nStart);

						if (nMovingLeftPosition <= 0)
						{//7y : Error IMR2_1_00025
							LoggingFunction(pNode, "IMR2_1_00025+ ");

							GoErrorFunction(pNode);
						}
						else
						{
							m_nRestorationJongChar = Restoration(m_pJongChar);//복원해야 할 종성값
							ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);//C(n) = “ㄹ”
							CHAR_DELETE(m_pStemDicWord);
							m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
							bool bFind = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
							LoggingFunction(pNode, "IMR2_1_00026+ ");
							if(bFind == true && m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE)
							{//7n : C(n)=“ㄹ” → S(len~n)이 stem 사전에 있고, S(len ~n) != [N]인가 ? IMR2_1_00026
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
								//8n : C(n) = #C(n), → MAR2로  IMR2_1_00028
								//원래의 종성값으로 복원
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
			if (strcmp(m_pNthOneChar, "갔") == 0)
			{//3n : S(n)이 “갔”인가? IMR2_1_00029
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
				m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
				LoggingFunction(pNode, "IMR2_1_00030+ ");
				if (IsExistStemDic(m_pStemDicDeleteJongWord))
				{//4y : S(len~n)-C(n)이 stem 사전에 있는가? IMR2_1_00030
					LoggingFunction(pNode, "IMR2_1_00031+ ");
					m_p1thAddString = AddTwoStr("었", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					//5y : stem = S(len~n)- C(n), ending = “었”+NewBuf()+OldBuf() IMR2_1_00031
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
				{//4n : S(len~n)-C(n)이 stem 사전에 있는가?(놀랐습니다) IMR2_1_00033
					LoggingFunction(pNode, "IMR2_1_00034+ ");
					m_p1thAddString = AddTwoStr("었", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					//5y : Stem = S(len~n)-C(n), ending = “었”+NewBuf()+OldBuf() IMR2_1_00034
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
					{//5n : S(n)-C(n)이 txx인가 IMR2_1_00035
						LoggingFunction(pNode, "IMR2_1_00036+ ");
						//6y : OldBuf() = “었”+NewBuf()+OldBuf(), NewBuf()=Null, S(n) = S(n)-C(n) → IRM4로 IMR2_1_00036
						m_p1thAddString = AddTwoStr("었", m_pNewBuffer);
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
						CHAR_DELETE(m_pNewBuffer);
						CHAR_DELETE(m_pOldBuffer);
						int nLen = strlen(m_p2thAddString);
						m_pOldBuffer = new char[nLen + 1];
						memset(m_pOldBuffer, 0x00, nLen + 1);
						strcpy(m_pOldBuffer, m_p2thAddString);
						InsertArrayFeatureOld(m_p2thAddString);
						
						ReplaceHangulJongsungNull(pNode, nStart);//S(n)=S(n)-C(n) 종성삭제

						GoIMR4(pNode, nStart, mStart, lStart);

					}
					else
					{
						LoggingFunction(pNode, "IMR2_1_00037+ ");
						m_p1thAddString = AddTwoStr("었", m_pNewBuffer);
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
						//6n : ending = “었”+NewBuf()+OldBuf(), stem = S(len~n)-C(n) -> Error IMR2_1_00037
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
	IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
	m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)의 음소
	LoggingFunction(pNode, "IMR3_00001+ ");
	if (strcmp(m_pJongChar, "ㄻ") == 0)
	{//C(n)이 “ㄻ'인가? IMR3_00001
		m_nRestorationJongChar = Restoration(m_pJongChar);//복원해야 할 종성값
		ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);//C(n) = “ㄹ”
		m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
		LoggingFunction(pNode, "IMR3_00002+ ");
		if (IsExistStemDic(m_pStemDicWord))
		{//1y : C(n)=“ㄹ" → S(len~n)이 stem 사전에 있는가? IMR3_00002
			LoggingFunction(pNode, "IMR3_00003+ ");
			m_p1thAddString = AddTwoStr("음", m_pNewBuffer);
			m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
			//2y : stem = S(len~n) + [VN] - [V], ending = “음” + NewBuf() + OldBuf() IMR3_00003
			InsertStemString(pNode, m_pStemDicWord);
			SaveHTokenStem(pNode);
			AddHTokenStem(pNode, STEM_DIC_V);
			AddHTokenStem(pNode, STEM_DIC_CMP);//VN->CMP로 변경함
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
		if (strcmp(m_pNthOneChar, "임") == 0)
		{//1n : S(n)이 “임”인가 ? IMR3_00005
			m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
			LoggingFunction(pNode, "IMR3_00006+ ");
			if (IsExistStemDic(m_pStemDicWord))
			{//2y : S(len~n)이 stem 사전에 있는가 ? IMR3_00006
				LoggingFunction(pNode, "IMR3_00007+ ");
				//3y : Error(복합명사분석 * / 취임전, 사임전) IMR3_00007

				GoErrorFunction(pNode);
			}
			else
			{
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
				m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
				LoggingFunction(pNode, "IMR3_00008+ ");
				if (IsExistStemDic(m_pStemDicDeleteJongWord))
				{//3n : S(len~n) - C(n)이 stem 사전에 있는가 ? IMR3_00008
					LoggingFunction(pNode, "IMR3_00009+ ");
					m_p1thAddString = AddTwoStr("음", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					//4y : stem = S(len~n) - C(n) + [VN] - [N], ending = “음” + NewBuf() + OldBuf() IMR3_00009
					InsertStemString(pNode, m_pStemDicDeleteJongWord);
					SaveHTokenStem(pNode);
					AddHTokenStem(pNode, STEM_DIC_V);
					AddHTokenStem(pNode, STEM_DIC_CMP);//VN->CMP로 변경함
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
					{//4n : S(len~n+1)이 stem 사전에 있는가 ? IMR3_00010
						LoggingFunction(pNode, "IMR3_00011+ ");
						//5y : stem = S(len~n+1), ending = “이”[Pol] + “음” + NewBuf() + OldBuf() - 자질 IMR3_00011
						m_p1thAddString = AddTwoStr("이", "음");
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
						m_p1thAddString = AddTwoStr("음", m_pNewBuffer);
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
							//6n : MAR2로 IMR3_00014

							GoMAR2(pNode, nStart, mStart, lStart);
						}
					}
				}
			}
		}
		else
		{
			LoggingFunction(pNode, "IMR3_00015+ ");
			if (strcmp(m_pNthOneChar, "음") == 0)
			{//2n : S(n)이 “음”인가 ? IMR3_00015
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
				m_pStemDicDeleteJongWord = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, nStart);//S(len~n)-C(n)
				LoggingFunction(pNode, "IMR3_00016+ ");
				if (IsExistStemDic(m_pStemDicDeleteJongWord))
				{//3y : (S(len~n)-“ㅁ”)이 stem 사전에 있는가 ? IMR3_00016
					LoggingFunction(pNode, "IMR3_00017+ ");
					m_p1thAddString = AddTwoStr("음", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					//4y : stem = S(len~n)-“ㅁ” + [VN] - [V], ending = “음” + NewBuf() + OldBuf() IMR3_00017
					InsertStemString(pNode, m_pStemDicDeleteJongWord);
					SaveHTokenStem(pNode);
					AddHTokenStem(pNode, STEM_DIC_CMP);//VN->CMP로 변경함
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
					{//4n : S(len~n+1)이 stem 사전에 있는가 ? IMR3_00018
						m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)의 음절
						m_pJongChar = GetChojungJongsungChar(m_pOneNPluseNumChar, CHAR_JONGSUNG);//C(n+1)의 음소
						LoggingFunction(pNode, "IMR3_00018+ ");
						if(m_pJongChar != NULL && strcmp(m_pJongChar, "") != 0 && m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE)
						{//5y : {C(n+1) != Null} && {S(len~n+1) != [N]}인가 ? IMR3_00019
							LoggingFunction(pNode, "IMR3_00020+ ");
							m_p1thAddString = AddTwoStr("음", m_pNewBuffer);
							m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
							//6y : stem = S(len~n+1) + [VN] - [V], ending = “음” + NewBuf() + OldBuf() IMR3_00020
							InsertStemString(pNode, m_pStemDicWord);
							SaveHTokenStem(pNode);
							AddHTokenStem(pNode, STEM_DIC_CMP);//VN->CMP로 변경함
							DeleteHTokenStem(pNode, STEM_DIC_V);
							InsertEndingString(pNode, m_p2thAddString);
							SaveHTokenEnding(pNode);
						}
						else
						{
							LoggingFunction(pNode, "IMR3_00021+ ");
							//6n : Error(20030129 유혜원 수정 “아기음료”) IMR3_00021

							GoErrorFunction(pNode);
						}
					}
					else
					{
						m_p1thAddString = AddTwoStr("기", m_pNewBuffer);
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
						InsertEndingString(pNode, m_p2thAddString);
						SaveHTokenEnding(pNode);

						LoggingFunction(pNode, "IMR3_00022(m = n → n++ → n>len)+ ");
						mStart = nStart;

						int nMovingLeftPosition = ReturnNumberMovingLeftPositionWord(pNode->pHToken->strSource, nStart);

						if (nMovingLeftPosition <= 0)
						{//5n : tmp_Stem = S(len~n+1) + [VN - [V], ending = “기” + NewBuf() + OldBuf(), m = n → n++ → n>len ? IMR3_00022
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
	IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
	m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)의 음소
	LoggingFunction(pNode, "IMR3_1_00001+ ");
	if (strcmp(m_pNthOneChar, "기") == 0)
	{//3n : S(n)이 “기”인가 ? (20030129 순서 변경 유혜원 수정) IMR3_1_00001
		m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
		m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
		LoggingFunction(pNode, "IMR3_1_00002+ ");
		if (IsExistStemDic(m_pStemDicWord, m_stemFeature01))
		{//3'y : S(len~n+1)이 stem 사전에 있는가? IMR3_1_00002
			LoggingFunction(pNode, "IMR3_1_00003+ ");
			if(m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE)
			{//4y : {S(len~n+1) != [N]}인가 ? IMR3_1_00003
				LoggingFunction(pNode, "IMR3_1_00004+ ");
				m_p1thAddString = AddTwoStr("기", m_pNewBuffer);
				m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
				//5y : stem = S(len~n+1) + [Cmp] - [V], ending = “기” + NewBuf() + OldBuf() IMR3_1_00004
				InsertStemString(pNode, m_pStemDicWord);
				SaveHTokenStem(pNode);
				AddHTokenStem(pNode, STEM_DIC_CMP);//VN->CMP로 변경함
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
			m_p1thAddString = AddTwoStr("기", m_pNewBuffer);
			m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
			InsertEndingString(pNode, m_p2thAddString);
			SaveHTokenEnding(pNode);

			LoggingFunction(pNode, "IMR3_1_00006(m = n → n++ → n>len)+ ");
			mStart = nStart;

			int nMovingLeftPosition = ReturnNumberMovingLeftPositionWord(pNode->pHToken->strSource, nStart);

			if (nMovingLeftPosition <= 0)
			{//4n : tmp_Stem = S(len~n+1) + [Cmp] - [V], ending = “기” + NewBuf() + OldBuf(), m = n → n++ → n>len ? IMR3_1_00006
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
		{//3'n : S(len~n)-C(n)이 stem 사전에 있는가? IMR3_1_00009
			LoggingFunction(pNode, "IMR3_1_00010+ ");
			m_p1thAddString = AddTwoStr("음", m_pNewBuffer);
			m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
			//4y : stem = S(len~n)-C(n) + [VN] - [V], ending = “음” + NewBuf() + OldBuf() IMR3_1_00010
			InsertStemString(pNode, m_pStemDicWord);
			SaveHTokenStem(pNode);
			AddHTokenStem(pNode, STEM_DIC_CMP);//VN->CMP로 변경함
			DeleteHTokenStem(pNode, STEM_DIC_V);
			InsertEndingString(pNode, m_p2thAddString);
			SaveHTokenEnding(pNode);
		}
		else
		{ 
			LoggingFunction(pNode, "IMR3_1_00011+ ");
			if(strcmp(m_pNthOneChar, "함") == 0)
			{//4n : S(n)이 “함”인가 ? IMR3_1_00011
				CHAR_DELETE(m_pStemDicWord);
				m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
				LoggingFunction(pNode, "IMR3_1_00012+ ");
				if (IsExistStemDic(m_pStemDicWord, m_stemFeature01))
				{//5y : S(len~n+1)이 stem 사전에 있는가 ? IMR3_1_00012
					LoggingFunction(pNode, "IMR3_1_00013+ ");
					m_pAddTwoString = AddTwoStr(m_pStemDicWord, "하");
					m_p1thAddString = AddTwoStr("음", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					//6y : stem = S(len~n+1) + “하” + [VN] - [N], ending = “음” + NewBuf() + OldBuf() IMR3_1_00013
					InsertStemString(pNode, m_pStemDicWord);
					SaveHTokenStem(pNode);
					InsertStemString(pNode, m_pAddTwoString);
					AddHTokenStem(pNode, STEM_DIC_CMP);//VN->CMP로 변경함
					DeleteHTokenStem(pNode, STEM_DIC_N);
					InsertEndingString(pNode, m_p2thAddString);
					SaveHTokenEnding(pNode);
				}
				else
				{
					LoggingFunction(pNode, "IMR3_1_00014+ ");
					//6n : S(n) = [mask]->MAR2로 IMR3_1_00014
					SetOneWordMarsk();

					GoMAR2(pNode, nStart, mStart, lStart);
				}
			}
			else
			{
				LoggingFunction(pNode, "IMR3_1_00015+ ");
				if (strcmp(m_pNthOneChar, "됨") == 0)
				{//5n : S(n)이 “됨”인가 ? IMR3_1_00015
					CHAR_DELETE(m_pStemDicWord);
					m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
					m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
					LoggingFunction(pNode, "IMR3_1_00016+ ");
					if (IsExistStemDic(m_pStemDicWord, m_stemFeature01))
					{//6y : S(len~n+1)이 stem 사전에 있는가 ? IMR3_1_00016
						LoggingFunction(pNode, "IMR3_1_00017+ ");
						m_pAddTwoString = AddTwoStr(m_pStemDicWord, "되");
						m_p1thAddString = AddTwoStr("음", m_pNewBuffer);
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
						//7y : stem = S(len~n+1) + “되” + [VN] - [V], ending = “음” + NewBuf() + OldBuf() IMR3_1_00017
						InsertStemString(pNode, m_pStemDicWord);
						SaveHTokenStem(pNode);
						InsertStemString(pNode, m_pAddTwoString);
						AddHTokenStem(pNode, STEM_DIC_CMP);//VN->CMP로 변경함
						DeleteHTokenStem(pNode, STEM_DIC_V);
						InsertEndingString(pNode, m_p2thAddString);
						SaveHTokenEnding(pNode);
					}
					else
					{
						LoggingFunction(pNode, "IMR3_1_00018+ ");
						//7n : S(n) = [mask]->MAR2로 IMR3_1_00018
						SetOneWordMarsk();

						GoMAR2(pNode, nStart, mStart, lStart);
					}
				}
				else
				{
					LoggingFunction(pNode, "IMR3_1_00019+ ");
					if (strcmp(m_pNthOneChar, "심") == 0)
					{//6n : S(n)이 “심”인가 ? IMR3_1_00019
						CHAR_DELETE(m_pStemDicWord);
						m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
						m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
						LoggingFunction(pNode, "IMR3_1_00020+ ");
						if (IsExistStemDic(m_pStemDicWord, m_stemFeature01))
						{//7y : S(len~n+1)이 stem 사전에 있는가 ? IMR3_1_00020
							LoggingFunction(pNode, "IMR3_1_00021+ ");
							m_p1thAddString = AddTwoStr("시", "음");
							m_p2thAddString = AddTwoStr(m_p1thAddString, m_pNewBuffer);
							m_p3thAddString = AddTwoStr(m_p2thAddString, m_pOldBuffer);
							//8y : stem = (len~n + 1), ending = “시” + “음” + NewBuf() + OldBuf() IMR3_1_00021
							InsertStemString(pNode, m_pStemDicWord);
							SaveHTokenStem(pNode);
							InsertEndingString(pNode, m_p3thAddString);
							SaveHTokenEnding(pNode);
						}
						else
						{
							
							LoggingFunction(pNode, "IMR3_1_00022+ ");
							//8n : S(n) = [mask] -> MAR1의 ②로 IMR3_1_00022
							SetOneWordMarsk();

							GoMAR1_2(pNode, nStart, mStart, lStart);
						}
					}
					else
					{
						LoggingFunction(pNode, "IMR3_1_00023+ ");
						//8n : S(n) = [mask] → MAR1-2로 IMR3_1_00023
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
	IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
	m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)의 음소
	LoggingFunction(pNode, "IMR4_00001+ ");
	if (CompareVariousTable05(m_pNthOneChar, ALL_TABLE_05, true) == true)
	{//S(n)이 Table05인가 ? IMR4_00001
		m_nRestorationJongChar = Restoration(m_pJongChar);//복원해야 할 종성값
		ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA02);//C(n) = “ㄷ”
		m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
		m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
		m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
		bool bFindStem = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
		bool bFindEnding = IsExistEndingDic(m_pFormNTo1Word, m_endingFeature01);
		LoggingFunction(pNode, "IMR4_00002+ ");
		if(bFindStem == true)
		{//1y : C(n)=“ㄷ” → S(len~n)이 stem 사전에 있는가? IMR4_00002
			LoggingFunction(pNode, "IMR4_00003+ ");
			if(m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && bFindEnding == true && m_endingFeature01[ENDING_DIC_N] != IS_CEHCK_FEATURE)
			{//2y : S(len~n) != [n] && S(N-1~1) != [n]인가?  IMR4_00003
				//3y : stem = S(len~n), ending = S(n-1~1)[=NewBuf()+OldBuf()] IMR4_00004
				InsertStemString(pNode, m_pStemDicWord);
				SaveHTokenStem(pNode);
				InsertEndingString(pNode, m_pFormNTo1Word);
				SaveHTokenEnding(pNode);
			}
			else
			{
				ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);//C(n) = “ㄹ”
				CHAR_DELETE(m_pStemDicWord);
				CHAR_DELETE(m_pFormNTo1Word);
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
				m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
				m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
				LoggingFunction(pNode, "IMR4_00005+ ");
				if(IsExistStemDic(m_pStemDicWord))
				{//3n : C(n)=“ㄹ” → S(len~n)이 stem 사전에 있는가? IMR4_00005
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

					//4n : S(n) = [mask] -> MAR1의 ②로 IMR4_00007
					SetOneWordMarsk();

					GoMAR1_2(pNode, nStart, mStart, lStart);
				}
			}
		}
		else
		{
			CHAR_DELETE(m_pStemDicWord);
			CHAR_DELETE(m_pFormNTo1Word);
			ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);//C(n) = “ㄹ”
			m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
			m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
			m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
			LoggingFunction(pNode, "IMR4_00008+ ");
			if (IsExistStemDic(m_pStemDicWord, m_stemFeature01))
			{//2n : C(n)=“ㄹ” → S(len~n)이 stem 사전에 있는가? IMR4_00008
				LoggingFunction(pNode, "IMR4_00009+ ");
				if(m_stemFeature01[STEM_DIC_LIR] == IS_CEHCK_FEATURE)
				{//3y : S(len~n)이 [Lir]인가? IMR4_00009
					LoggingFunction(pNode, "IMR4_00010+ ");
					m_p1thAddString = AddTwoStr("ㄹ", m_pFormNTo1Word);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pNewBuffer);
					m_p3thAddString = AddTwoStr(m_p2thAddString, m_pOldBuffer);
					//4y : stem = S(len~n), ending = “ㄹ”+ S(n-1~1)+NewBuf()+OldBuf()] IMR4_00010
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

				//3n :  S(n) = [mask] -> MAR1의 ②로 IMR4_00012
				SetOneWordMarsk();

				GoMAR1_2(pNode, nStart, mStart, lStart);
			}
		}
	}
	else
	{
		LoggingFunction(pNode, "IMR4_00013+ ");
		if (CompareVariousTable06(m_pNthOneChar, ALL_TABLE_06, true) == true)
		{//1n : S(n)이 Table06인가?   IMR4_00013
			m_nRestorationJongChar = Restoration(m_pJongChar);//복원해야 할 종성값
			ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA06);//C(n) = “ㅅ”
			m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
			m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
			m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
			bool bFindStem = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
			bool bFindEnding = IsExistEndingDic(m_pFormNTo1Word, m_endingFeature01);
			LoggingFunction(pNode, "IMR4_00014+ ");
			if(bFindStem == true)
			{//2y : C(n)=“ㅅ” → S(len~n)이 stem 사전에 있는가? IMR4_00014
				LoggingFunction(pNode, "IMR4_00015+ ");
				if (m_stemFeature01[STEM_DIC_V] == IS_CEHCK_FEATURE && bFindEnding == true && m_endingFeature01[ENDING_DIC_V] == IS_CEHCK_FEATURE)
				{//3y : S(len~n) == [v] && S(n-1~1) == [v]인가?  IMR4_00015
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
					ReplaceHangulJongsungNull(pNode, nStart);//delete “ㅅ”
					m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
					m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
					m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
					LoggingFunction(pNode, "IMR4_00017+ ");
					if(IsExistStemDic(m_pStemDicWord))
					{//4n : delete “ㅅ” → S(len~n)이 stem 사전에 있는가? IMR4_00017
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
						m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
						m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
						m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
						LoggingFunction(pNode, "IMR4_00019+ ");
						if (IsExistStemDic(m_pStemDicWord, m_stemFeature01))
						{//5n : S(len~n+1)이 stem 사전에 있는가? IMR4_00019
							LoggingFunction(pNode, "IMR4_00020+ ");
							if(strcmp(m_pNthOneChar, "이") == 0 && (m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_CMP] == IS_CEHCK_FEATURE))
							{//6y : S(n)이 “이”이고, S(len~n+1) == [n]||[amb]||[Cmp]인가? IMR4_00020
								LoggingFunction(pNode, "IMR4_00021+ ");
								m_p1thAddString = AddTwoStr("이", m_pNewBuffer);
								m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
								//7y : stem = S(len~n+1), ending = “이”[Pol]-[N]+NewBuf()+OldBuf() IMR4_00021
								InsertStemString(pNode, m_pStemDicWord);
								SaveHTokenStem(pNode);
								InsertEndingString(pNode, m_p2thAddString);
								SaveHTokenEnding(pNode);
								AddHTokenEnding(pNode, ENDING_DIC_POL);//[Pol]자질부여
								DeleteHTokenEnding(pNode, ENDING_DIC_N);//[N]자질삭제
							}
							else
							{
								LoggingFunction(pNode, "IMR4_00022+ ");

								//7n : S(n) = [mask] -> MAR1의 ②로 IMR4_00022
								SetOneWordMarsk();

								GoMAR1_2(pNode, nStart, mStart, lStart);
							}
						}
						else
						{
							LoggingFunction(pNode, "IMR4_00023+ ");

							//6n : S(n) = [mask] -> MAR1의 ②로 IMR4_00023
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
				ReplaceHangulJongsungNull(pNode, nStart);//delete “ㅅ”
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
				m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
				m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
				bool bFindStem = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
				bool bFindEnding = IsExistEndingDic(m_pFormNTo1Word, m_endingFeature01);
				LoggingFunction(pNode, "IMR4_00024+ ");
				if(bFindStem == true)
				{//3n : delete “ㅅ” → S(len~n)이 stem 사전에 있는가? IMR4_00024
					LoggingFunction(pNode, "IMR4_00025+ ");
					if((m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] == IS_CEHCK_FEATURE) || (m_stemFeature01[STEM_DIC_N] != IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_N] != IS_CEHCK_FEATURE) || (m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE && m_endingFeature01[ENDING_DIC_AMB] == IS_CEHCK_FEATURE))
					{//4y : {S(len~n) == [N] && S(n-1~1) == [N]}이거나, {S(len~n) <> [N] && S(n-1~1) <> [N]}이거나, {S(len~n) == [amb] || S(n-1) == [amb]}인가? IMR4_00025
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

						//5n : S(n) = [mask] → MAR1의 ②로 IMR4_00027
						SetOneWordMarsk();

						GoMAR1_2(pNode, nStart, mStart, lStart);
					}
				}
				else
				{
					CHAR_DELETE(m_pNthOneChar);
					CHAR_DELETE(m_pStemDicWord);
					CHAR_DELETE(m_pFormNTo1Word);
					m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
					m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
					m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
					bool bFindStem = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
					LoggingFunction(pNode, "IMR4_00028+ ");
					if(strcmp(m_pNthOneChar, "이") == 0 && (m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_CMP] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_ADV] == IS_CEHCK_FEATURE))
					{//4n : S(n) == “이” && S(len~n+1) == [n]||[amb]||[Cmp]||[Adv]인가? IMR4_00028
						m_pLastOneChar = GetOneChar(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));//S(1)
						m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
						bool bFindFirst = IsExistEndingDic(m_pFormNTo1Word, m_endingFeature01);
						bool bFindSecond = IsExistEndingDic(m_pNewBuffer, m_endingFeature02);
						bool bFindThird = IsExistEndingDic(m_pOldBuffer, m_endingFeature03);
						LoggingFunction(pNode, "IMR4_00029+ ");
						if(strcmp(m_pNthOneChar, m_pLastOneChar) != 0  && (bFindFirst == true || (bFindSecond == true && m_endingFeature02[ENDING_DIC_V] == IS_CEHCK_FEATURE && bFindThird == true)))
						{//5y : S(n) != S(1) && {IsExistEndDic_S(n~1) || (NewBuffer == E_V && IsExistEndDic_OldBuffer)}인가? IMR4_00029
							//6y : stem = S(len~n+1), ending=“이”[Pol]-[N]+NewBuf()+OldBuf() IMR4_00030
							LoggingFunction(pNode, "IMR4_00030+ ");
							m_p1thAddString = AddTwoStr("이", m_pNewBuffer);
							m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
							//6y : stem = S(len~n+1), ending=“이”[Pol]-[N]+NewBuf()+OldBuf() IMR4_00030
							InsertStemString(pNode, m_pStemDicWord);
							SaveHTokenStem(pNode);
							InsertEndingString(pNode, m_p2thAddString);
							SaveHTokenEnding(pNode);
							AddHTokenEnding(pNode, ENDING_DIC_POL);//[Pol]자질부여
							DeleteHTokenEnding(pNode, ENDING_DIC_N);//[N]자질삭제
						}
						else
						{
							CHAR_DELETE(m_pFormNTo1Word);
							m_nMinusSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_PLUS);//n-1
							m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1~1)
							bool bFindEnding = IsExistEndingDic(m_pFormNTo1Word, m_endingFeature01);
							LoggingFunction(pNode, "IMR4_00031+ ");
							if(m_endingFeature01[ENDING_DIC_PRS] == IS_CEHCK_FEATURE || m_endingFeature02[ENDING_DIC_N] == IS_CEHCK_FEATURE 
								|| (strcmp(m_pNewBuffer,"야") == 0 && m_pOldBuffer != NULL && strcmp(m_pOldBuffer, "") != 0)
								|| (strcmp(m_pNewBuffer, "자") == 0 && m_pOldBuffer != NULL && strcmp(m_pOldBuffer, "") != 0)
								|| (strcmp(m_pNewBuffer, "유") == 0 && m_pOldBuffer != NULL && strcmp(m_pOldBuffer, "") != 0))
							{//6n : S(n-1~1) == [prs] || NewBuf() == [N] || (NewBuffer == “야” && OldBuffer != NULL) || (NewBuffer == “자” && OldBuffer != NULL) || (NewBuffer == “유” && OldBuffer != NULL)인가? IMR4_00031
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
								if((bFind == true && m_endingFeature01[ENDING_DIC_N] == IS_CEHCK_FEATURE) || strcmp(m_pFormNTo1Word, "도") == 0 || strcmp(m_pFormNTo1Word, "는") == 0)
								{//7n :  {S(n~1) == [N]||:도:||:는}인가? IMR4_00033
									LoggingFunction(pNode, "IMR4_00034+ ");
									m_p1thAddString = AddTwoStr("이", m_pNewBuffer);
									m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
									//8y : stem = S(len~n+1), ending = “이”-[Agt][Pss]+NewBuf()+OldBuf() IMR4_00034
									InsertStemString(pNode, m_pStemDicWord);
									SaveHTokenStem(pNode);
									InsertEndingString(pNode, m_p2thAddString);
									SaveHTokenEnding(pNode);
									DeleteHTokenEnding(pNode, ENDING_DIC_AGT);//[AGT]자질삭제
									DeleteHTokenEnding(pNode, ENDING_DIC_PSS);//[PSS]자질삭제
								}
								else
								{
									LoggingFunction(pNode, "IMR4_00035+ ");
									m_p1thAddString = AddTwoStr("이", m_pNewBuffer);
									m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
									//8n : stem = S(len~n+1), ending = “이”[Pol]-[N]+NewBuf()+OldBuf() IMR4_00035
									InsertStemString(pNode, m_pStemDicWord);
									SaveHTokenStem(pNode);
									InsertEndingString(pNode, m_p2thAddString);
									SaveHTokenEnding(pNode);
									AddHTokenEnding(pNode, ENDING_DIC_POL);//[POL]자질추가
									DeleteHTokenEnding(pNode, ENDING_DIC_N);//[N]자질삭제
								}
							}
						}
					}
					else
					{
						CHAR_DELETE(m_pStemDicWord);
						m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
						m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n+1)의 음절
						m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
						m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+2)
						bool bFindEnding = IsExistEndingDic(m_pOneNPluseNumChar, m_endingFeature01);
						bool bFindStem = IsExistEndingDic(m_pStemDicWord, m_stemFeature01);
						LoggingFunction(pNode, "IMR4_00036+ ");
						if(strcmp(m_pNthOneChar, "이") == 0
							&& (m_endingFeature01[ENDING_DIC_N] == IS_CEHCK_FEATURE || m_endingFeature01[ENDING_DIC_AFF] == IS_CEHCK_FEATURE || m_endingFeature01[ENDING_DIC_CAU] == IS_CEHCK_FEATURE)
							&& (m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE))
						{//5n : S(n) == “이”이고, S(n+1)이 [n, aff, cau]이고, S(len~n+2) == [n] || [amb] || [VN]인가? IMR4_00036
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

							//6n : S(n) = [mask] -> MAR1의 ②로 IMR4_00038
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
	IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
	m_pFormLenToNWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
	LoggingFunction(pNode, "IMR4_1_00001+ ");
	if(strcmp(m_pFormLenToNWord,"퍼") == 0)
	{//2n : S(len~n) == “퍼”인가 ? IMR4_1_00001
		InsertStemString(pNode, "푸");
		SaveHTokenStem(pNode);
		LoggingFunction(pNode, "IMR4_1_00002+ ");
		if (m_pNewBuffer == NULL)
		{//3y : stem = “푸” → tmp_end == null인가? IMR4_1_00002
			LoggingFunction(pNode, "IMR4_1_00003+ ");
			//4y : ending = “어”+[V][AFF][Con]+NewBuf()+OldBuf() IMR4_1_00003
			m_p1thAddString = AddTwoStr("어", m_pNewBuffer);
			m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
			InsertEndingString(pNode, m_p2thAddString);
			SaveHTokenEnding(pNode);
			AddHTokenEnding(pNode, ENDING_DIC_V);//[V]자질추가
			AddHTokenEnding(pNode, ENDING_DIC_AFF);//[AFF]자질추가
			AddHTokenEnding(pNode, ENDING_DIC_CON);//[CON]자질추가
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
		IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
		m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
		m_pFormLenToNWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
		LoggingFunction(pNode, "IMR4_1_00005+ ");
		if(CompareVariousTable01(m_pNthOneChar, 0, false) == true || CompareVariousTable02(m_pNthOneChar, 1, false) == true || CompareVariousTable02(m_pNthOneChar, 2, false) == true)
		{//3n : S(n)이 Table01[0] || Table02[1] || Table02[2] 인가? IMR4_1_00005
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
			{//4y : Table01[0]인 경우 :S(len~n+1)+table01[1] 이 stem 사전에 있는가?  IMR4_1_00006
				//Table02[1]||Table02[2]인 경우 : S(len~n+1)+table02[0]이 stem 사전에 있는가? IMR4_1_00007
				LoggingFunction(pNode, "IMR4_1_00008+ ");
				if(m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_ADV] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_IC] == IS_CEHCK_FEATURE)
				{//5y : [{S(len~n+1)+table01[1]}||{S(len~n+1)+Table02[0]}] == [n] || [{S(len~n+1)+table01[1]}||{S(len~n+1)+Table02[0]}] == [ADV] || [{S(len~n+1)+table01[1]}||{S(len~n+1)+Table02[0]}] == [IC]인가? IMR4_1_00008
					LoggingFunction(pNode, "IMR4_1_00009+ ");
					//6y : S(n) = [mask] -> MAR1의 ②로 IMR4_1_00009
					SetOneWordMarsk();

					GoMAR1_2(pNode, nStart, mStart, lStart);
				}
				else
				{
					LoggingFunction(pNode, "IMR4_1_00010+ ");
					m_p1thAddString = AddTwoStr("어", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);

					//6n : stem = S(len~n+1)+table01[1]||table02[0] , ending = “어”+[V][Aff][Con]+NewBuf()+OldBuf() IMR4_1_00010
					InsertStemString(pNode, m_pAddTwoString);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_p2thAddString);
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, ENDING_DIC_V);//[V]자질추가
					AddHTokenEnding(pNode, ENDING_DIC_AFF);//[AFF]자질추가
					AddHTokenEnding(pNode, ENDING_DIC_CON);//[CON]자질추가
				}
			}
			else
			{
				LoggingFunction(pNode, "IMR4_1_00011+ ");
				if(strcmp(m_pNthOneChar, "떠") == 0 || strcmp(m_pNthOneChar, "거") == 0 || strcmp(m_pNthOneChar, "러") == 0)
				{//5n : S(n) == “떠” || “거” || “러”인가? IMR4_1_00011
					CHAR_DELETE(m_pAddTwoString);
					m_pAddTwoString = AddTwoStr(m_pFormLenToNWord,"ㅎ");
					LoggingFunction(pNode, "IMR4_1_00012+ ");
					if(IsExistStemDic(m_pAddTwoString))
					{//6y : S(len~n)+ㅎ 이 stem 사전에 있는가? IMR4_1_00012
						m_p1thAddString = AddTwoStr( m_pNewBuffer, m_pOldBuffer);
						LoggingFunction(pNode, "IMR4_1_00013+ ");
						//7y : stem = S(len~n)+“ㅎ”, ending = NewBuf()+OldBuf() IMR4_1_00013
						InsertStemString(pNode, m_pAddTwoString);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, m_p1thAddString);
						SaveHTokenEnding(pNode);
					}
					else
					{
						//IMR4_1_00014~IMR4_1_00017 주석을 잘못 표시함.
						LoggingFunction(pNode, "IMR4_1_00018+ ");

						//7n : S(n) = [mask] -> MAR1의 IMR4_1_00018
						SetOneWordMarsk();
						GoMAR1_2(pNode, nStart, mStart, lStart);
					}
				}
				else
				{
					m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
					m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)의 음절
					m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
					m_pFormLenToNWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+2)
					bool bFind = IsExistStemDic(m_pFormLenToNWord, m_stemFeature01);
					LoggingFunction(pNode, "IMR4_1_00019+ ");
					if(strcmp(m_pNthOneChar, "켜") == 0 && strcmp(m_pOneNPluseNumChar, "시") == 0 && (bFind == true && (m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE)))
					{//6n : S(n) == “켜” && S(n+1) == “시” && S(len~n+2) == [N]||[Amb]인가? IMR4_1_00019
						LoggingFunction(pNode, "IMR4_1_00020+ ");
						m_pAddTwoString = AddTwoStr(m_pFormLenToNWord, "시키");
						m_p1thAddString = AddTwoStr("어", m_pNewBuffer);
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
						//7y : stem = S(len~n+2)+시키+[V][Act]-[N], ending = “어”+OldBuf()+NewBuf()IMR4_1_00020
						InsertStemString(pNode, m_pFormLenToNWord);
						SaveHTokenStem(pNode);
						InsertStemString(pNode, m_pAddTwoString);
						AddHTokenStem(pNode, STEM_DIC_V);//[V]자질추가
						AddHTokenStem(pNode, STEM_DIC_ACT);//[ACT]자질추가
						DeleteHTokenStem(pNode, STEM_DIC_N);//[N]자질삭제
						InsertEndingString(pNode, m_p2thAddString);
						SaveHTokenEnding(pNode);
					}
					else
					{
						LoggingFunction(pNode, "IMR4_1_00021+ ");
						if (strcmp(m_pNthOneChar, "워") == 0 && strcmp(m_pOneNPluseNumChar, "다") == 0 && (bFind == true && (m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE)))
						{//7n : S(n) == “워” && S(n+1) == “다” && S(len~n+2) == [N]||[Amb]인가? IMR4_1_00021
							LoggingFunction(pNode, "IMR4_1_00022+ ");
							m_pAddTwoString = AddTwoStr(m_pFormLenToNWord, "답");
							m_p1thAddString = AddTwoStr("어", m_pNewBuffer);
							m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
							//8y : stem = S(len~n+2)+답+[V][Sta]-[N], ending = “어”+OldBuf()+NewBuf() IMR4_1_00022
							InsertStemString(pNode, m_pFormLenToNWord);
							SaveHTokenStem(pNode);
							InsertStemString(pNode, m_pAddTwoString);
							AddHTokenStem(pNode, STEM_DIC_V);//[V]자질추가
							AddHTokenStem(pNode, STEM_DIC_STA);//[STA]자질추가
							DeleteHTokenStem(pNode, STEM_DIC_N);//[N]자질삭제
							InsertEndingString(pNode, m_p2thAddString);
							SaveHTokenEnding(pNode);
						}
						else
						{
							CHAR_DELETE(m_pFormLenToNWord);
							m_pFormNTo1Word = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+2)의 음절
							m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+3
							m_pFormLenToNWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+3)
							bool bFind = IsExistStemDic(m_pFormLenToNWord, m_stemFeature01);
							LoggingFunction(pNode, "IMR4_1_00023+ ");
							if((strcmp(m_pNthOneChar, "워") == 0 && strcmp(m_pOneNPluseNumChar, "러") == 0 && strcmp(m_pFormNTo1Word, "스") == 0) && (bFind == true && (m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE)))
							{//8n : S(n) == “워” && S(n+1) == “러” && S(n+2 ) == “스” && S(len~n+3) == [N]||[Amb]인가? IMR4_1_00023
								LoggingFunction(pNode, "IMR4_1_00024+ ");
								m_pAddTwoString = AddTwoStr(m_pFormLenToNWord, "스럽");
								m_p1thAddString = AddTwoStr("어", m_pNewBuffer);
								m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
								//9y : stem = S(len~n+3)+스럽+[V][Sta]-[N], ending = “어”+OldBuf()+NewBuf() IMR4_1_00024
								InsertStemString(pNode, m_pFormLenToNWord);
								SaveHTokenStem(pNode);
								InsertStemString(pNode, m_pAddTwoString);
								AddHTokenStem(pNode, STEM_DIC_V);//[V]자질추가
								AddHTokenStem(pNode, STEM_DIC_STA);//[STA]자질추가
								DeleteHTokenStem(pNode, STEM_DIC_N);//[N]자질삭제
								InsertEndingString(pNode, m_p2thAddString);
								SaveHTokenEnding(pNode);
							}
							else
							{
								LoggingFunction(pNode, "IMR4_1_00025+ ");
								//9n : S(n) = [mask] -> MAR1의 ②로 IMR4_1_00025
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
			IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
			m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
			m_pFormLenToNWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)

			LoggingFunction(pNode, "IMR4_1_00026+ ");
			if (CompareVariousTable03(m_pNthOneChar, 0, false) == true)
			{//4n : S(n)이 Table03[0]인지 확인하라.IMR4_1_00026
				LoggingFunction(pNode, "IMR4_1_00026+ ");
				if(IsExistStemDic(m_pFormLenToNWord))
				{//5y : S(len~n)이 stem 사전에 있는가? IMR4_1_00027
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
					{//6n : S(len~n+1)이 stem 사전에 있는가? IMR4_1_00029
						m_pLastOneChar = GetOneChar(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource));//S(1)
						LoggingFunction(pNode, "IMR4_1_00030+ ");
						if((m_pOldBuffer == NULL || strcmp(m_pOldBuffer, "") == 0) && strcmp(m_pNthOneChar, m_pLastOneChar) == 0 && strcmp(m_pNthOneChar, "되") == 0)
						{//7y : {OldBuf() == NULL} && {S(n) == S(1)} && {S(n) == "되"} 인가? IMR4_1_00030
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
							m_pOneNMinusNumChar = GetOneChar(pNode->pHToken->strSource, m_nMinusSyllable);//S(n-1)의 음절
							LoggingFunction(pNode, "IMR4_1_00032+ ");
							if(strcmp(m_pNthOneChar, "하") == 0 && (strcmp(m_pOneNMinusNumChar, "에") == 0 || strcmp(m_pOneNMinusNumChar, "고") == 0 || strcmp(m_pOneNMinusNumChar, "며") == 0) && (bFind == true && m_stemFeature01[STEM_DIC_CONC] == IS_CEHCK_FEATURE))
							{//8n : {S(n) == "하"} && {S(n-1)==“에” || ((S(n-1) == "고" || "며")&& S(len~n+1) == [Conc]) }인가? IMR4_1_00032
								LoggingFunction(pNode, "IMR4_1_00033+ ");
								m_pFormNTo1Word = GetEndingPositionFromWord(pNode->pHToken->strSource, nStart);//S(n~1)
								//9y : Stem =  S(len~n+1), Ending = S(n~n-1)+[N][Cor]-[V]+ S(n-2~1) IMR4_1_00033
								InsertStemString(pNode, m_pStemDicWord);
								SaveHTokenStem(pNode);
								InsertEndingString(pNode, m_pFormNTo1Word);
								SaveHTokenEnding(pNode);
								AddHTokenEnding(pNode, ENDING_DIC_N);//[N]자질추가
								AddHTokenEnding(pNode, ENDING_DIC_COR);//[COR]자질추가
								DeleteHTokenEnding(pNode, ENDING_DIC_V);//[V]자질삭제
							}
							else
							{
								LoggingFunction(pNode, "IMR4_1_00034+ ");
								if (strcmp(m_pNthOneChar, "되") == 0)
								{//9n : S(n) == “되”인가? IMR4_1_00034
									LoggingFunction(pNode, "IMR4_1_00035+ ");
									m_pFormNTo1Word = AddTwoStr(m_pStemDicWord, Table03[m_nTableIndex].strOneWord);//S(len~n+1)+table03[0]
									m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
									//10y : stem = S(len~n+1)+table03[0]+[V] [Fnc] [T078]-[N] [Act] [T104], ending = NewBuf()+OldBuf() IMR4_1_00035
									InsertStemString(pNode, m_pStemDicWord);//S(len~n+1)의 사전 정보로 자질정보를 세팅한다.
									SaveHTokenStem(pNode);
									InsertStemString(pNode, m_pFormNTo1Word);//S(len~n+1)+table03[0]
									AddHTokenStem(pNode, STEM_DIC_V);//[V]자질추가
									AddHTokenStem(pNode, STEM_DIC_FNC);//[FNC]자질추가
									AddHTokenStem(pNode, S_T078);//[T078]자질추가
									DeleteHTokenStem(pNode, STEM_DIC_N);//[N]자질삭제
									DeleteHTokenStem(pNode, STEM_DIC_ACT);//[N]자질삭제
									DeleteHTokenStem(pNode, S_T104);//[T104]자질삭제
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
									AddHTokenStem(pNode, STEM_DIC_V);//[V]자질추가
									AddHTokenStem(pNode, S_T077);//[T077]자질추가
									DeleteHTokenStem(pNode, STEM_DIC_N);//[N]자질삭제
									InsertEndingString(pNode, m_p1thAddString);
									SaveHTokenEnding(pNode);
								}
							}
						}
					}
					else
					{
						LoggingFunction(pNode, "IMR4_1_00037+ ");

						//7n :S(n) = [mask] -> MAR1의 ②로 IMR4_1_00037
						SetOneWordMarsk();

						GoMAR1_2(pNode, nStart, mStart, lStart);
					}
				}
			}
			else
			{
				IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
				m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
				m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
				LoggingFunction(pNode, "IMR4_1_00038+ ");
				if (CompareVariousTable03(m_pNthOneChar, 1, false) == true)
				{//5n : S(n)이 Table03[1]인지 확인하라. IMR4_1_00038
					LoggingFunction(pNode, "IMR4_1_00037+ ");
					m_pFormNTo1Word = AddTwoStr(m_pStemDicWord, Table03[m_nTableIndex].strOneWord);//S(len~n+1)+table03[0]
					m_p1thAddString = AddTwoStr("어", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					LoggingFunction(pNode, "IMR4_1_00039+ ");
					if(IsExistStemDic(m_pFormNTo1Word))
					{//6y : S(len~n+1)+Table03[0]이 stem 사전에 있는가? IMR4_1_00039
						LoggingFunction(pNode, "IMR4_1_00040+ ");
						//7y : stem = S(len~n+1)+Table03[0], ending=“어”+NewBuf()+Oldbuf() IMR4_1_00040
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
						{//7n : S(len~n+1)이 stem 사전에 있는가? IMR4_1_00041
							LoggingFunction(pNode, "IMR4_1_00042+ ");
							if(strcmp(m_pNthOneChar, "되") == 0 || strcmp(m_pNthOneChar, "돼") == 0)
							{//8y : S(n) == “되” || “돼”인가? IMR4_1_00042
								LoggingFunction(pNode, "IMR4_1_00043+ ");
								m_pFormNTo1Word = AddTwoStr(m_pStemDicWord, Table03[m_nTableIndex].strOneWord);//S(len~n+1)+table03[0]
								m_p1thAddString = AddTwoStr("어", m_pNewBuffer);
								m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
								//9y : stem = S(len~n+1)+table03[0]+[V][Fnc][T078]-[N][Act][T104], ending = “어”+NewBuf()+OldBuf() IMR4_1_00043
								InsertStemString(pNode, m_pStemDicWord);//S(len~n+1)의 자질정보 세팅
								SaveHTokenStem(pNode);
								InsertStemString(pNode, m_pFormNTo1Word);//S(len~n+1)+table03[0]
								AddHTokenStem(pNode, STEM_DIC_V);//[V]자질추가
								AddHTokenStem(pNode, STEM_DIC_FNC);//[FNC]자질추가
								AddHTokenStem(pNode, S_T078);//[T078]자질추가
								DeleteHTokenStem(pNode, STEM_DIC_N);//[N]자질삭제
								DeleteHTokenStem(pNode, STEM_DIC_ACT);//[N]자질삭제
								DeleteHTokenStem(pNode, S_T104);//[T104]자질삭제
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
								{//9n : S(len~n+1) != [Conc] && S(len~n+1) != [Tm] && ending 사전에S(n-1~1)!=[N]인가? IMR4_1_00044
									LoggingFunction(pNode, "IMR4_1_00045+ ");
									m_pFormNTo1Word = AddTwoStr(m_pStemDicWord, Table03[m_nTableIndex].strOneWord);//S(len~n+1)+table03[0]
									m_p1thAddString = AddTwoStr("어", m_pNewBuffer);
									m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
									//10y : stem = S(len~n+1)+table03[0] +[V][T077]-[N], ending = “어”+NewBuf()+OldBuf() IMR4_1_00045
									InsertStemString(pNode, m_pStemDicWord);//S(len~n+1)의 자질정보 세팅
									SaveHTokenStem(pNode);
									InsertStemString(pNode, m_pFormNTo1Word);//S(len~n+1)+table03[0]
									AddHTokenStem(pNode, STEM_DIC_V);//[V]자질추가
									AddHTokenStem(pNode, S_T077);//[T077]자질추가
									DeleteHTokenStem(pNode, STEM_DIC_N);//[N]자질삭제
									InsertEndingString(pNode, m_p2thAddString);
									SaveHTokenEnding(pNode);
								}
								else
								{
									LoggingFunction(pNode, "IMR4_1_00046+ ");
									//10n : Error(20030212 유혜원 수정 “양띠해”)  IMR4_1_00046

									GoErrorFunction(pNode);
								}
							}
						}
						else
						{
							LoggingFunction(pNode, "IMR4_1_00047+ ");
							//8n : S(n) = [mask] -> MAR1의 ②로 IMR4_1_00047
							SetOneWordMarsk();

							GoMAR1_2(pNode, nStart, mStart, lStart);
						}
					}
				}
				else
				{
					LoggingFunction(pNode, "IMR4_1_00048+ ");
					//6n : 다음으로 IMR4_1_00048


					GoIMR4_2(pNode, nStart, mStart, lStart);
				}
			}
		}
	}

}

void CNAHangulAnaly::GoIMR4_2(H_NODE *pNode, int &nStart, int &mStart, int &lStart)
{
	IntitFunctionUsingValue();//형태소 분석에 필요한 변수 초기화
	m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, nStart);//S(n)의 음절
	LoggingFunction(pNode, "IMR4_2_00001+ ");
	if(strcmp(m_pNthOneChar,"라") == 0 || strcmp(m_pNthOneChar, "러") == 0)
	{//(n) == “라” || “러”인가?IMR4_2_00001
		m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//n+1의 음절
		m_pJongChar = GetChojungJongsungChar(m_pOneNPluseNumChar, CHAR_JONGSUNG);//C(n+1)의 음소
		LoggingFunction(pNode, "IMR4_2_00002+ ");
		if(strcmp(m_pJongChar, "ㄹ") == 0)
		{//1y : C(n+1)이 “ㄹ”인가? IMR4_2_00002
			CHAR_DELETE(m_pNthOneChar);
			CHAR_DELETE(m_pJongChar);
			m_pJongChar = GetChojungJongsungChar(m_pNthOneChar, CHAR_JONGSUNG);//C(n)의 음소
			m_nRestorationJongChar = Restoration(m_pJongChar);//복원해야 할 종성값
			m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
			m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)의 음절
			m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
			m_pMinusTwoString = MinusTwoStr(m_pStemDicWord, m_pNthOneChar, m_nPluseSyllable);//S(len~n+1)-C(n+1)
			m_pAddTwoString = AddTwoStr(m_pMinusTwoString,"르");
			bool bFindStem = IsExistStemDic(m_pAddTwoString, m_stemFeature01);
			LoggingFunction(pNode, "IMR4_2_00003+ ");
			if(bFindStem == true)
			{//2y : (S(len~n+1)-C(n+1)+“르”)가 stem 사전에 있는가? IMR4_2_00003
				LoggingFunction(pNode, "IMR4_2_00004+ ");
				if(m_stemFeature01[STEM_DIC_RIR] == IS_CEHCK_FEATURE)
				{//3y : (S(len~n+1)-C(n+1)+“르”)가 [Rir]인가? IMR4_2_00004
					LoggingFunction(pNode, "IMR4_2_00005+ ");
					m_p1thAddString = AddTwoStr("어", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					//4y : stem = S(len~n+1)-C(n+1)+“르”, ending=“어”+[V][Aff][Con]+NewBuf()+OldBuf() IMR4_2_00005
					InsertStemString(pNode, m_pAddTwoString);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_p2thAddString);
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, ENDING_DIC_V);//[V]자질추가
					AddHTokenEnding(pNode, ENDING_DIC_AFF);//[AFF]자질추가
					AddHTokenEnding(pNode, ENDING_DIC_CON);//[CON]자질추가
				}
				else
				{
					CHAR_DELETE(m_pAddTwoString);
					m_pAddTwoString = AddTwoStr(m_pStemDicWord, "르");
					bFindStem = IsExistStemDic(m_pAddTwoString, m_stemFeature01);
					LoggingFunction(pNode, "IMR4_2_00006+ ");
					if(bFindStem == true)
					{//4n : (S(len~n+1)+“르”)가 stem 사전에 있는가? IMR4_2_00006
						LoggingFunction(pNode, "IMR4_2_00007+ ");
						if (m_stemFeature01[STEM_DIC_UIR] == IS_CEHCK_FEATURE)
						{//5y : (S(len~n+1)+“르”)가 [Uir]인가? IMR4_2_00007
							LoggingFunction(pNode, "IMR4_2_00008+ ");
							m_p1thAddString = AddTwoStr("어", m_pNewBuffer);
							m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
							//6y : stem = S(len~n+1)+“르”, ending=“어”+[V][Aff][Con]+NewBuf()+OldBuf() IMR4_2_00008
							InsertStemString(pNode, m_pAddTwoString);
							SaveHTokenStem(pNode);
							InsertEndingString(pNode, m_p2thAddString);
							SaveHTokenEnding(pNode);
							AddHTokenEnding(pNode, ENDING_DIC_V);//[V]자질추가
							AddHTokenEnding(pNode, ENDING_DIC_AFF);//[AFF]자질추가
							AddHTokenEnding(pNode, ENDING_DIC_CON);//[CON]자질추가
						}
						else
						{
							LoggingFunction(pNode, "IMR4_2_00009+ ");
							//6n : S(n) = [mask] -> MAR1의 ②로 IMR4_2_00009
							SetOneWordMarsk();

							GoMAR1_2(pNode, nStart, mStart, lStart);
						}
					}
					else
					{
						LoggingFunction(pNode, "IMR4_2_00010+ ");
						//5n : S(n) = [mask] -> MAR1의 ②로 IMR4_2_00010
						SetOneWordMarsk();

						GoMAR1_2(pNode, nStart, mStart, lStart);
					}
				}
			}
			else
			{
				LoggingFunction(pNode, "IMR4_2_00011+ ");
				//3n : S(n) = [mask] -> MAR1의 ②로 IMR4_2_00011
				SetOneWordMarsk();

				GoMAR1_2(pNode, nStart, mStart, lStart);
			}
		}
		else
		{
			m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
			m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
			m_pAddTwoString = AddTwoStr(m_pMinusTwoString, "르");
			bool bFindStem = IsExistStemDic(m_pAddTwoString, m_stemFeature01);
			LoggingFunction(pNode, "IMR4_2_00012+ ");
			if(bFindStem == true)
			{//2n : (S(len~n+1)+“르”)가 stem 사전에 있는가? IMR4_2_00012
				LoggingFunction(pNode, "IMR4_2_00013+ ");
				if (m_stemFeature01[STEM_DIC_UIR] == IS_CEHCK_FEATURE)
				{//3y : (S(len~n+1)+“르”)가 [Uir]인가? IMR4_2_00013
					LoggingFunction(pNode, "IMR4_2_00014+ ");
					m_p1thAddString = AddTwoStr("어", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					//4y : stem = S(len~2)+“르”, ending=“어” +[V][Aff][Con]+NewBuf()+OldBuf() IMR4_2_00014
					InsertStemString(pNode, m_pAddTwoString);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_p2thAddString);
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, ENDING_DIC_V);//[V]자질추가
					AddHTokenEnding(pNode, ENDING_DIC_AFF);//[AFF]자질추가
					AddHTokenEnding(pNode, ENDING_DIC_CON);//[CON]자질추가
				}
				else
				{
					LoggingFunction(pNode, "IMR4_2_00015+ ");
					//4n : S(n) = [mask] -> MAR1의 ②로 IMR4_2_00015
					SetOneWordMarsk();

					GoMAR1_2(pNode, nStart, mStart, lStart);
				}
			}
			else
			{
				bFindStem = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
				LoggingFunction(pNode, "IMR4_2_00016+ ");
				if(bFindStem == true)
				{//3n : S(len~n+1)이 stem 사전에 있는가? IMR4_2_00016
					LoggingFunction(pNode, "IMR4_2_00017+ ");
					if (m_stemFeature01[STEM_DIC_UIR] == IS_CEHCK_FEATURE)
					{//4y : S(len~n+1)이 [Ruir]인가? IMR4_2_00017
						LoggingFunction(pNode, "IMR4_2_00018+ ");
						m_p1thAddString = AddTwoStr("어", m_pNewBuffer);
						m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
						//5y : stem = S(len~n+1), ending=“어”+[V][Aff][Con]+NewBuf()+OldBuf() IMR4_2_00018
						InsertStemString(pNode, m_pAddTwoString);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, m_p2thAddString);
						SaveHTokenEnding(pNode);
						AddHTokenEnding(pNode, ENDING_DIC_V);//[V]자질추가
						AddHTokenEnding(pNode, ENDING_DIC_AFF);//[AFF]자질추가
						AddHTokenEnding(pNode, ENDING_DIC_CON);//[CON]자질추가
					}
					else
					{	 
						LoggingFunction(pNode, "IMR4_2_00019+ ");
						//5n : S(n) = [mask] -> MAR1의 ②로 IMR4_2_00019
						SetOneWordMarsk();

						GoMAR1_2(pNode, nStart, mStart, lStart);
					}
				}
				else
				{
					CHAR_DELETE(m_pStemDicWord);
					ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA13);//C(n) = “ㅎ”
					m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
					LoggingFunction(pNode, "IMR4_2_00020+ ");
					if(IsExistStemDic(m_pStemDicWord))
					{//4n : C(n) = ㅎ → S(len~n)이 stem 사전에 있는가? IMR4_2_00020
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
						ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA05);//C(n) = “ㅂ”
						m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
						LoggingFunction(pNode, "IMR4_2_00022+ ");
						if (IsExistStemDic(m_pStemDicWord))
						{//5n : C(n) = “ㅂ” → S(len~n)이 stem 사전에 있는가? IMR4_2_00022
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
							m_pNthOneChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)의 음절
							m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
							m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+2)
							bFindStem = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
							LoggingFunction(pNode, "IMR4_2_00025+ ");
							if(bFindStem == true && (m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE) && strcmp(m_pNthOneChar,"스") == 0)
							{//6n : S(len~n+2) == [N]||[Amb] && S(n+1) == “스”인가? IMR4_2_00025
								LoggingFunction(pNode, "IMR4_2_00025+ ");
								m_pAddTwoString = AddTwoStr(m_pStemDicWord,"스럽");
								m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
								//7y : stem = S(len~n+2)+스럽+[V][Sta]-[N], ending = NewBuf()+OldBuf() IMR4_2_00025
								InsertStemString(pNode, m_pAddTwoString);
								SaveHTokenStem(pNode);
								AddHTokenStem(pNode, STEM_DIC_V);//[V]자질추가
								AddHTokenStem(pNode, STEM_DIC_STA);//[STA]자질추가
								DeleteHTokenStem(pNode, STEM_DIC_N);//[N]자질삭제
								InsertEndingString(pNode, m_p1thAddString);
								SaveHTokenEnding(pNode);
							}
							else
							{
								LoggingFunction(pNode, "IMR4_2_00026+ ");
								//7n : C(n) = Null → S(n) = [mask] -> MAR1의 ②로 IMR4_2_00026
								ReplaceHangulJongsungNull(pNode, nStart);//종성삭제
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
		{//1n : S(n)이 Table04[3]인가? IMR4_2_00027
			m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
			m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
			m_pAddTwoString = AddTwoStr(m_pStemDicWord, Table04[m_nTableIndex].strOneWord);//S(len~n+1)+Table04[0]
			LoggingFunction(pNode, "IMR4_2_00028+ ");
			if (IsExistStemDic(m_pAddTwoString))
			{//2y : S(len~n+1)+Table04[0]이 stem 사전에 있는가? IMR4_2_00028	
				LoggingFunction(pNode, "IMR4_2_00029+ ");
				if(m_pOldBuffer == NULL || strcmp(m_pOldBuffer,"") == 0)
 				{//3y : stem = S(len~n+1)+Table03[0]→ OldBuf() == Null인가? IMR4_2_00029
					LoggingFunction(pNode, "IMR4_2_00030+ ");
					m_p1thAddString = AddTwoStr("어", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					//4y : ending = “어”+[V][Aff][Con] +NewBuf() IMR4_2_00030
					InsertStemString(pNode, m_pAddTwoString);
					SaveHTokenStem(pNode);
					CHAR_DELETE(m_pAddTwoString);
					m_pAddTwoString = AddTwoStr(m_pStemDicWord, Table03[m_nTableIndex].strOneWord);//S(len~n+1)+Table03[0]
					InsertStemString(pNode, m_pAddTwoString);
					InsertEndingString(pNode, m_p2thAddString);
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, ENDING_DIC_V);//[V]자질추가
					AddHTokenEnding(pNode, ENDING_DIC_AFF);//[AFF]자질추가
					AddHTokenEnding(pNode, ENDING_DIC_CON);//[CON]자질추가
					
				}
				else
				{
					LoggingFunction(pNode, "IMR4_2_00031+ ");
					m_p1thAddString = AddTwoStr("어", m_pNewBuffer);
					m_p2thAddString = AddTwoStr(m_p1thAddString, m_pOldBuffer);
					//4n : stem = S(len~n+1)+Table04[0], ending =“어”+[V][Aff][Con]+NewBuf()+OldBuf() IMR4_2_00031
					InsertStemString(pNode, m_pAddTwoString);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_p2thAddString);
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, ENDING_DIC_V);//[V]자질추가
					AddHTokenEnding(pNode, ENDING_DIC_AFF);//[AFF]자질추가
					AddHTokenEnding(pNode, ENDING_DIC_CON);//[CON]자질추가
				}
			}
			else
			{
				CHAR_DELETE(m_pStemDicWord);
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, nStart);//S(len~n)
				bool bFindStem = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
				LoggingFunction(pNode, "IMR4_2_00032+ ");
				if(bFindStem == true)
				{//3n : S(len~n)이 stem에 있는가? IMR4_2_00032
					LoggingFunction(pNode, "IMR4_2_00033+ ");
					if(m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_ADV] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_IC] == IS_CEHCK_FEATURE)
					{//4y : S(len~n) == [N] || [ADV] || [IC]인가? IMR4_2_00033
						LoggingFunction(pNode, "IMR4_2_00034+ ");
						//5y : S(n) = [mask]  -> MAR1의 ②로 IMR4_2_00034
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
					//4n : S(n) = [mask]  -> MAR1의 ②로 IMR4_2_00036
					SetOneWordMarsk();

					GoMAR1_2(pNode, nStart, mStart, lStart);
				}
			}
		}
		else
		{	 
			LoggingFunction(pNode, "IMR4_2_00037+ ");
			if(CompareVariousTable04(m_pNthOneChar, 1, false) == true || CompareVariousTable04(m_pNthOneChar, 2, false) == true)
			{//2n : S(n)이 Table04[1] || Table04[2] 인가? IMR4_2_00037
				m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
				m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+1)
				m_pAddTwoString = AddTwoStr(m_pStemDicWord, Table04[m_nTableIndex].strOneWord);//S(len~n+1)+Table04[0]
				LoggingFunction(pNode, "IMR4_2_00038+ ");
				if(IsExistStemDic(m_pAddTwoString))
				{//3y : S(len~n+1)+Table04[0]이 stem 사전에 있는가? IMR4_2_00038
					LoggingFunction(pNode, "IMR4_2_00039+ ");
					//4y : stem = S(len~n+1)+Table04[0]
					//→ Table04[1] : ending = NewBuf()+OldBuf()
					//→ Table04[2] : ending = ㄴ+NewBuf()+OldBuf() IMR4_2_00039
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
						m_p1thAddString = AddTwoStr("ㄴ", m_pNewBuffer);
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
					{//4n : S(len~n)이 stem 사전에 있는가? IMR4_2_00040
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
						//5n : S(n) = [mask] -> MAR1의 ②로 IMR4_2_00042
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
				if(strcmp(m_pNthOneChar, "답") == 0 && bFindStem == true && (m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE))
				{//3n : S(n) == “답” && S(len~n+1) == [N]||[Amb]인가? IMR4_2_00043
					LoggingFunction(pNode, "IMR4_2_00044+ ");
					m_pAddTwoString = AddTwoStr(m_pStemDicWord,"답");
					m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
					//4y : stem = S(len~n+1)+답+[V][Sta]-[N], Ending = NewBuf()+OldBuf() IMR4_2_00044
					InsertStemString(pNode, m_pStemDicWord);
					SaveHTokenStem(pNode);
					InsertStemString(pNode, m_pAddTwoString);
					AddHTokenStem(pNode, STEM_DIC_V);//[V]자질추가
					AddHTokenStem(pNode, STEM_DIC_STA);//[STA]자질추가
					DeleteHTokenStem(pNode, STEM_DIC_N);//[N]자질삭제
					InsertEndingString(pNode, m_p1thAddString);
					SaveHTokenEnding(pNode);
				}
				else
				{
					CHAR_DELETE(m_pStemDicWord);
					m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 1, CALCULATE_MINUS);//n+1
					m_pOneNPluseNumChar = GetOneChar(pNode->pHToken->strSource, m_nPluseSyllable);//S(n+1)의 음절
					m_nPluseSyllable = GetNextPrevPositionWord(pNode->pHToken->strSource, nStart, 2, CALCULATE_MINUS);//n+2
					m_pStemDicWord = GetFromFirstToStartWord(pNode->pHToken->strSource, m_nPluseSyllable);//S(len~n+2)
					bool bFindStem = IsExistStemDic(m_pStemDicWord, m_stemFeature01);
					LoggingFunction(pNode, "IMR4_2_00045+ ");
					if(strcmp(m_pNthOneChar, "럽") == 0 && strcmp(m_pOneNPluseNumChar, "스") == 0 && bFindStem == true && (m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE))
					{//4n : S(n) == “럽” && S(n+1) == “스” && S(len~n+2) == [N]||[Amb]인가? IMR4_2_00045
						LoggingFunction(pNode, "IMR4_2_00046+ ");
						m_pAddTwoString = AddTwoStr(m_pStemDicWord, "스럽");
						m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
						//5y : stem = S(len~n+2)+스럽+[V][Sta]-[N], Ending = NewBuf()+OldBuf() IMR4_2_00046
						InsertStemString(pNode, m_pStemDicWord);
						SaveHTokenStem(pNode);
						InsertStemString(pNode, m_pAddTwoString);
						AddHTokenStem(pNode, STEM_DIC_V);//[V]자질추가
						AddHTokenStem(pNode, STEM_DIC_STA);//[STA]자질추가
						DeleteHTokenStem(pNode, STEM_DIC_N);//[N]자질삭제
						InsertEndingString(pNode, m_p1thAddString);
						SaveHTokenEnding(pNode);
					}
					else
					{
						LoggingFunction(pNode, "IMR4_2_00047+ ");
						if (strcmp(m_pNthOneChar, "우") == 0 && strcmp(m_pOneNPluseNumChar, "다") == 0 && bFindStem == true && (m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE))
						{//5n : S(n) == “우” && S(n+1) == “다” && S(len~n+2) == [N]||[Amb]인가? IMR4_2_00047
							LoggingFunction(pNode, "IMR4_2_00048+ "); 
							m_pAddTwoString = AddTwoStr(m_pStemDicWord, "답");
							m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
							//6y : stem = S(len~n+2)+답+[V][Sta]-[N], Ending = NewBuf()+OldBuf() IMR4_2_00048
							InsertStemString(pNode, m_pStemDicWord);
							SaveHTokenStem(pNode);
							InsertStemString(pNode, m_pAddTwoString);
							AddHTokenStem(pNode, STEM_DIC_V);//[V]자질추가
							AddHTokenStem(pNode, STEM_DIC_STA);//[STA]자질추가
							DeleteHTokenStem(pNode, STEM_DIC_N);//[N]자질삭제
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
							if (strcmp(m_pNthOneChar, "우") == 0 && strcmp(m_pFormLenToNWord, "스러") == 0 && bFindStem == true && (m_stemFeature01[STEM_DIC_N] == IS_CEHCK_FEATURE || m_stemFeature01[STEM_DIC_AMB] == IS_CEHCK_FEATURE))
							{//6n : S(n) == “우” && S(n+1~n+2) == “스러” && S(len~n+3) == [N]||[Amb]인가? IMR4_2_00049
								LoggingFunction(pNode, "IMR4_2_00050+ ");
								m_pAddTwoString = AddTwoStr(m_pStemDicWord, "스럽");
								m_p1thAddString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
								//7y : stem = S(len~n+3)+스럽+[V][Sta]-[N], Ending = NewBuf()+OldBuf() IMR4_2_00050
								InsertStemString(pNode, m_pStemDicWord);
								SaveHTokenStem(pNode);
								InsertStemString(pNode, m_pAddTwoString);
								AddHTokenStem(pNode, STEM_DIC_V);//[V]자질추가
								AddHTokenStem(pNode, STEM_DIC_STA);//[STA]자질추가
								DeleteHTokenStem(pNode, STEM_DIC_N);//[N]자질삭제
								InsertEndingString(pNode, m_p1thAddString);
								SaveHTokenEnding(pNode);
							}
							else
							{
								LoggingFunction(pNode, "IMR4_2_00051+ ");
								//7n : S(n) = [mask]->MAR1의 ②로 IMR4_2_00051

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
// 미등록어 분석 알고리듬
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
					if (strcmp(pTm, "초") == 0 || strcmp(pTm, "분") == 0 ||
						strcmp(pTm, "시") == 0 || strcmp(pTm, "일") == 0 ||
						strcmp(pTm, "월") == 0 || strcmp(pTm, "년") == 0 ||
						strcmp(pTm, "년간") == 0 || strcmp(pTm, "년도") == 0 ||
						strcmp(pTm, "년경") == 0 || strcmp(pTm, "시간") == 0 ||
						strcmp(pTm, "세기") == 0 || strcmp(pTm, "박") == 0 ||
						strcmp(pTm, "학년") == 0 || strcmp(pTm, "주년") == 0 ||
						strcmp(pTm, "학기") == 0 || strcmp(pTm, "주간") == 0 ||
						strcmp(pTm, "개월") == 0 || strcmp(pTm, "주일") == 0 ||
						strcmp(pTm, "분경") == 0 || strcmp(pTm, "일") == 0 ||
						strcmp(pTm, "년대") == 0 || strcmp(pTm, "일경") == 0 ||
						strcmp(pTm, "여분") == 0 || strcmp(pTm, "일간") == 0 ||
						strcmp(pTm, "분간") == 0 || strcmp(pTm, "초간") == 0 ||
						strcmp(pTm, "시경") == 0 || strcmp(pTm, "분경") == 0 ||
						strcmp(pTm, "월경") == 0 || strcmp(pTm, "월말") == 0 ||
						strcmp(pTm, "월초") == 0 || strcmp(pTm, "년말") == 0 ||
						strcmp(pTm, "년초") == 0 || strcmp(pTm, "주초") == 0 ||
						strcmp(pTm, "주말") == 0 || strcmp(pTm, "시반") == 0 ||
						strcmp(pTm, "시반경") == 0)
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
		strcmp(pOneChar, "·") == 0 || strcmp(pOneChar, "~") == 0)
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
				strcmp(pOneChar, "·") == 0 || strcmp(pOneChar, "~") == 0)
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
	if ((strcmp(pOneChar, "주") == 0 || strcmp(pOneChar, "제") == 0 ||
		strcmp(pOneChar, "년") == 0 || strcmp(pOneChar, "연") == 0) &&
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
			strcmp(pOneChar, "=") == 0 || strcmp(pOneChar, "·") == 0 ||
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

	//7.24수정
	//3n
	if (IsExistEndingDic(pNextMiddleString1)) // S(n+1~len)이 Ending 사전에 있는가?
	{
		//4y'
		if (IsExistFeature(pNextMiddleString1, END_DIC, E_Ppo) == true &&
			strcmp(pNextOneChar, "시") != 0)
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
			if (strcmp(pNextOneChar, "기") == 0 || strcmp(pNextOneChar, "시") == 0 ||
				strcmp(pNextOneChar, "층") == 0 || strcmp(pNextOneChar, "단") == 0 ||
				strcmp(pNextOneChar, "구") == 0 || strcmp(pNextOneChar, "대") == 0 ||
				strcmp(pNextOneChar, "세") == 0 || strcmp(pNextOneChar, "자") == 0 ||
				strcmp(pNextOneChar, "당") == 0 || strcmp(pNextOneChar, "매") == 0 ||
				strcmp(pNextOneChar, "차") == 0 || strcmp(pNextOneChar, "건") == 0 ||
				strcmp(pNextOneChar, "리") == 0 || strcmp(pNextOneChar, "중") == 0)
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
				strcmp(pNextOneChar, "가") == 0)
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
		(strcmp(pNextMiddleString1, "일") == 0 || strcmp(pNextMiddleString1, "인") == 0))
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
		strcmp(pNextOneChar, "일") == 0 && IsExistEndingDic(pMiddleString))
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
	else if (strcmp(pNextOneChar, "씩") == 0 ||
		strcmp(pMiddleString12, "까지") == 0 ||
		strcmp(pMiddleString12, "부터") == 0 ||
		strcmp(pMiddleString12, "에서") == 0)
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
	else if ((isbFind005 = (strcmp(pNextOneChar, "이") == 0 && IsExistEndingDic(pMiddleString))) ||
		(isbFind006 = (strcmp(pChoJungChar, "이") == 0 && IsExistEndingDic(pAddTwoString))) ||
		(isbFind007 = (strcmp(pChoJungChar, "이") == 0 && IsExistEndingDic(pJongChar) &&
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

		//Ending 추가
		CHAR_DELETE(pAddTwoString2);
		pAddTwoString2 = AddTwoStr("이", pEndTemp);
		InsertEndingString(pNode, pAddTwoString2);
		SaveHTokenEnding(pNode);
		AddHTokenEnding(pNode, E_Pol);
		AddEndingFeature(pNode, pEndTemp);
	}
	//7n
	else if (strcmp(pNextOneChar, "여") == 0 && IsExistTabel008(pMiddleString23))
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
	else if (strcmp(pNextOneChar, "여") == 0 && IsExistTabel009(pNextOneChar2))
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
	else if ((strcmp(pNextOneChar, "였") == 0 || strcmp(pNextOneChar, "여") == 0) &&
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

		//Ending 추가
		CHAR_DELETE(pAddTwoString2);
		pAddTwoString2 = AddTwoStr("이어", pAddTwoString);
		InsertEndingString(pNode, pAddTwoString2);
		SaveHTokenEnding(pNode);
		AddHTokenEnding(pNode, E_Pol);
		AddEndingFeature(pNode, "이어");
		AddEndingFeature(pNode, pJongChar);
		AddEndingFeature(pNode, pMiddleString);
	}
	//8n
	else if (IsAalphabet(pThisOneChar))
	{
		//9y
		if (IsExistStemDic(pNextMiddleString1))
		{
			if ((strcmp(pNextOneChar, "한") == 0 || strcmp(pNextOneChar, "할") == 0 || strcmp(pNextOneChar, "함") == 0) &&
				nTotalCount <= nLen)
			{
				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr(m_pOldBuffer, "하");
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
		else if (strcmp(pNextOneChar, "하") == 0 || strcmp(pNextOneChar, "되") == 0)
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
		else if (strcmp(pChoJungChar, "하") == 0 || strcmp(pChoJungChar, "되") == 0)
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
		else if (strcmp(pNextOneChar, "한") == 0 || strcmp(pNextOneChar, "할") == 0 ||
			strcmp(pNextOneChar, "함") == 0)
		{
			CHAR_DELETE(pAddTwoString);

			pAddTwoString = AddTwoStr(m_pOldBuffer, "하");
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
			strcmp(pNextMiddleString1, "도시") != 0)
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
			//S(n+2)-C(n+2) : 초중성
			pAddTwoString2 = GetChojungJongsungChar(pNextOneChar2, CHAR_CHOJUNGSUNG);
			if ((IsExistTabel009(pNextOneChar) && strcmp(pNextOneChar, "이") == 0 && IsExistEndingDic(pMiddleString1)) ||
				(strcmp(pAddTwoString2, "이") == 0 && IsExistEndingDic(pAddTwoString)) ||
				(strcmp(pAddTwoString2, "이") == 0 && IsExistEndingDic(pJongChar) && nTotalCount <= nNextPos2) ||
				((strcmp(pNextOneChar2, "였") == 0 || strcmp(pNextOneChar2, "여") == 0) && IsExistEndingDic(pMiddleString1))
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
		else if ((strcmp(pThisOneChar, "이") == 0 && ((strcmp(pNextOneChar, "였") == 0 ||
			strcmp(pNextOneChar, "여") == 0))) ||
			(strcmp(pThisOneChar, "이") == 0 && pMiddleString1 != NULL && IsExistEndingDic(pMiddleString1)) ||
			(strcmp(pChoJungChar, "이") == 0 && pAddTwoString != NULL && IsExistEndingDic(pAddTwoString)) ||
			(strcmp(pChoJungChar, "이") == 0 && pJongChar != NULL && IsExistEndingDic(pJongChar) &&
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

			pAddTwoString = AddTwoStr("이", pJongChar);
			pAddTwoString2 = AddTwoStr(pAddTwoString, pMiddleString1);

			InsertEndingString(pNode, pAddTwoString2);
			MakeNewEndingFeature(pNode);
			AddEndingFeature(pNode, pJongChar);
			AddEndingFeature(pNode, pMiddleString1);
			AddHTokenEnding(pNode, 26);
		}
		//14n
		else if (pNextOneChar != NULL && (strcmp(pNextOneChar, "였") == 0 || strcmp(pNextOneChar, "여") == 0) &&
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

			pAddTwoString = AddTwoStr("이어", pJongChar);
			pAddTwoString2 = AddTwoStr(pAddTwoString, pMiddleString1);

			InsertEndingString(pNode, pAddTwoString2);
			MakeNewEndingFeature(pNode);
			AddHTokenEnding(pNode, E_Pol);
			AddEndingFeature(pNode, pJongChar);
			AddEndingFeature(pNode, pMiddleString1);
			AddHTokenEnding(pNode, 26);
		}
		else if (strcmp(pThisOneChar, "대") == 0 && IsExistEndingDic(pMiddleString1))
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
			((strcmp(pThisOneChar, "어") == 0 && strcmp(pNextOneChar, "치") == 0) ||
			(strcmp(pThisOneChar, "가") == 0 && strcmp(pNextOneChar, "량") == 0) || strcmp(pThisOneChar, "") == 0 && strcmp(pNextOneChar, "치") == 0 &&
				(strcmp(pThisOneChar, "이") == 0 && strcmp(pNextOneChar, "상") == 0) ||
				(strcmp(pThisOneChar, "이") == 0 && strcmp(pNextOneChar, "하") == 0)) &&
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
			else if ((strcmp(pThisOneChar, "이") == 0 && ((strcmp(pNextOneChar, "였") == 0 ||
				strcmp(pNextOneChar, "여") == 0))) ||
				(strcmp(pThisOneChar, "이") == 0 && IsExistEndingDic(pMiddleString1)) ||
				(strcmp(pChoJungChar, "이") == 0 && IsExistEndingDic(pAddTwoString)) ||
				(strcmp(pChoJungChar, "이") == 0 && IsExistEndingDic(pJongChar) &&
					strcmp(pThisOneChar, pLastOneChar) == 0 && IsExistFeature(pNextOneChar, STEM_DIC, S_N) &&
					IsExistFeature(pNextOneChar, STEM_DIC, S_Aff)))
			{
				CHAR_DELETE(pAddTwoString);
				//2017-08-16 pAddTwoString2 DELETE 추가
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

				pAddTwoString = AddTwoStr("이", pJongChar);
				pAddTwoString2 = AddTwoStr(pAddTwoString, pMiddleString1);

				InsertEndingString(pNode, pAddTwoString2);
				MakeNewEndingFeature(pNode);
				AddHTokenEnding(pNode, 26);
				AddEndingFeature(pNode, pJongChar);
				AddEndingFeature(pNode, pMiddleString1);


			}
			else if ((strcmp(pThisOneChar, "씩") == 0 || strcmp(pThisOneChar, "만") == 0) &&
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
			else if (strcmp(pThisOneChar, "반") == 0 && strcmp(pNextOneChar2, "경") == 0 &&
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
			else if (pMiddleString23 != NULL && (strcmp(pMiddleString23, "까지") == 0 ||
				strcmp(pMiddleString23, "부터") == 0 ||
				strcmp(pMiddleString23, "에서") == 0))
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
			//20n S(n+2) 숫자인가?
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
					(pNextOneChar3 != NULL && pNextOneChar4 != NULL && strcmp(pNextOneChar3, "이") == 0 && (strcmp(pNextOneChar4, "었") == 0 || strcmp(pNextOneChar4, "였") == 0)) ||
					(pNextOneChar3 != NULL && pMiddleString23 != NULL && strcmp(pNextOneChar3, "이") == 0 &&
						IsExistStemDic(pMiddleString23) &&
						IsExistFeature(pMiddleString23, STEM_DIC, S_N) == false &&
						IsExistFeature(pMiddleString23, STEM_DIC, S_Aff) == false) ||
						(pChoJungChar3 != NULL && pAddTwoString3 != NULL &&
							strcmp(pChoJungChar3, "이") == 0 && IsExistEndingDic(pAddTwoString3)) ||
							(pChoJungChar3 != NULL && pJongChar3 != NULL && pNextOneChar3 != NULL &&
								strcmp(pChoJungChar3, "이") == 0 && IsExistEndingDic(pJongChar3) &&
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
						pAddTwoString = AddTwoStr("이", pAddTwoString3);
						InsertEndingString(pNode, pAddTwoString);
						SaveHTokenEnding(pNode);
						AddHTokenEnding(pNode, E_Pol);
						AddEndingFeature(pNode, pAddTwoString3);
					}
					else
					{
						InsertEndingString(pNode, "이");
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
						strcmp(pNextOneChar2, "이") == 0 && (strcmp(pNextOneChar3, "었") == 0 || strcmp(pNextOneChar3, "였") == 0)) ||
						(pNextOneChar2 != NULL && pMiddleString12 != NULL &&
							strcmp(pNextOneChar2, "이") == 0 &&
							IsExistStemDic(pMiddleString12) &&
							IsExistFeature(pMiddleString12, STEM_DIC, S_N) == false &&
							IsExistFeature(pMiddleString12, STEM_DIC, S_Aff) == false) ||
							(pChoJungChar2 != NULL && pAddTwoString2 != NULL &&
								strcmp(pChoJungChar2, "이") == 0 && IsExistEndingDic(pAddTwoString2)) ||
								(pChoJungChar2 != NULL && pJongChar2 != NULL && pNextOneChar2 != NULL &&
									strcmp(pChoJungChar2, "이") == 0 && IsExistEndingDic(pJongChar2) &&
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
						pAddTwoString = AddTwoStr("이", pAddTwoString2);
						InsertEndingString(pNode, pAddTwoString);
						SaveHTokenEnding(pNode);
						AddHTokenEnding(pNode, E_Pol);
						AddEndingFeature(pNode, pAddTwoString2);
					}
					else
					{
						InsertEndingString(pNode, "이");
						SaveHTokenEnding(pNode);
						AddHTokenEnding(pNode, E_Pol);
					}
				}
			}
			else if (strcmp(pThisOneChar, "여") == 0 && pNextOneChar2 != NULL && IsExistTabel009(pNextOneChar2)
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
	else if (strcmp(pNextOneChar, "하") == 0 || strcmp(pNextOneChar, "되") == 0)
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
	else if (strcmp(pChoJungChar, "하") == 0 || strcmp(pChoJungChar, "되") == 0)
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
			IsExistEndingDic(pMiddleString12) == true)//S(n)==숫자 && S(n+1)==특수기호 && S(n+2~len)이 엔딩 사전에 있는가?
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

	//개체명인식NER 01
	if (GoNER01(pNode) == false)
	{
		pOriginalString = pNode->pHToken->strSource;
		pThisOneChar = GetUnRegisterOneChar(pOriginalString, nCount);
		if (strcmp(pThisOneChar, "비") == 0 || strcmp(pThisOneChar, "재") == 0 || strcmp(pThisOneChar, "반") == 0)
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
				//개체명인식NER 02
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
					(strcmp(m_pOldBuffer, "인") == 0 || strcmp(m_pOldBuffer, "일") == 0 ||
						strcmp(m_pOldBuffer, "임") == 0 ||
						strcmp(m_pOldBuffer, "만") == 0))
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
					(strcmp(m_pOldBuffer, "할") == 0 || strcmp(m_pOldBuffer, "한") == 0 ||
						strcmp(m_pOldBuffer, "함") == 0))
				{
					pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);

					CHAR_DELETE(pNode->pHToken->subStem->strResult);

					if (pNode->pHToken->subStem->strFeature != NULL)
					{
						memset(pNode->pHToken->subStem->strFeature, '0', STEM_DIC_ALL_FEATURE_NUMBER - 1);
					}

					pNode->pHToken->subStem->strResult = new char[3];
					memset(pNode->pHToken->subStem->strResult, 0x00, 3);
					InsertStemString(pNode, "하");
					SaveHTokenStem(pNode);
					ClassfyKindChar(pNode, "하");

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
					(strcmp(m_pOldBuffer, "적") == 0 || strcmp(m_pOldBuffer, "성") == 0 ||
						strcmp(m_pOldBuffer, "화") == 0) && pNode->pHToken->subStem->strResult != NULL)
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
			if (m_pOldBuffer == NULL)//m_pOldBuffer가 널인가?
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
		if ((strcmp(m_pOldBuffer, "적") == 0 ||
			strcmp(m_pOldBuffer, "성") == 0 ||
			strcmp(m_pOldBuffer, "화") == 0) &&
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


			///////////복합명사 추가 부분
			nSubNodeCount = SubNodeCount(pNode);

			//"적"이 붙는 명사에는 [T151]자질 추가
			if (strcmp(m_pOldBuffer, "적") == 0)
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
			else if ((strcmp(pOneChar, "이") == 0 && IsExistEndingDic(pMiddleString2)) ||
				(strcmp(pChoJungChar, "이") == 0 && IsExistEndingDic(pAddTwoString)))
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
					AddEndingFeature(pNode, "이");
					AddHTokenEnding(pNode, 26);
					AddEndingFeature(pNode, pAddTwoString);
					DeleteHTokenEnding(pNode, 4);
					DeleteHTokenEnding(pNode, 2);
					DeleteHTokenEnding(pNode, 25);
				}
				else
				{
					SaveHTokenEnding(pNode);
					AddEndingFeature(pNode, "이");
					AddHTokenEnding(pNode, 26);
					AddEndingFeature(pNode, pJongChar);
					AddEndingFeature(pNode, pMiddleString2);
					DeleteHTokenEnding(pNode, 4);
					DeleteHTokenEnding(pNode, 2);
					DeleteHTokenEnding(pNode, 25);
				}
			}
			else if ((strcmp(pChoJungChar, "하") == 0 || strcmp(pChoJungChar, "되") == 0) &&
				strcmp(pJongChar, "ㅁ") == 0 &&
				(pMiddleString2 == NULL || (pMiddleString2 != NULL &&
				(strlen(pMiddleString2) == 0 || IsExistEndingDic(pMiddleString2) == true))))
			{
				CHAR_DELETE(pAddTwoString);
				//복합명사 존재여부에 따라 처리 방법
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
			else if ((strcmp(pChoJungChar, "하") == 0 || strcmp(pChoJungChar, "되") == 0) &&
				(strcmp(pJongChar, "ㄹ") == 0 || strcmp(pJongChar, "ㄴ") == 0) &&
				(pMiddleString2 == NULL || (pMiddleString2 != NULL &&
				(strlen(pMiddleString2) == 0 || IsExistEndingDic(pMiddleString2) == true))))
			{
				CHAR_DELETE(pAddTwoString);
				//복합명사 존재여부에 따라 처리 방법
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
					if (strcmp(pChoJungChar, "하") == 0)
					{
						AddHTokenStem(pNode, S_Act);
						AddHTokenStem(pNode, S_V);
						DeleteHTokenStem(pNode, S_N);
					}
					else if (strcmp(pChoJungChar, "되") == 0)
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
					if (strcmp(pChoJungChar, "하") == 0)
					{
						AddSubNodeTailFeature(pNode, S_Act);
						AddSubNodeTailFeature(pNode, S_V);
						DeleteSubNodeTailFeature(pNode, S_N);
					}
					else if (strcmp(pChoJungChar, "되") == 0)
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
		else if (strcmp(m_pOldBuffer, "한") == 0)
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
				InsertStemString(pNode, "하");
				SaveHTokenStem(pNode);
				ClassfyKindChar(pNode, "하");

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
					//복합명사 추가 
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
			if ((strcmp(pChoJungChar, "하") == 0 || strcmp(pChoJungChar, "되") == 0) &&
				strcmp(pJongChar, "ㅁ") == 0 &&
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
			else if ((strcmp(pChoJungChar, "하") == 0 || strcmp(pChoJungChar, "되") == 0) &&
				(strcmp(pJongChar, "ㄹ") == 0 || strcmp(pJongChar, "ㄴ") == 0) &&
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
				if (strcmp(pChoJungChar, "하") == 0)
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
				else if (strcmp(pChoJungChar, "되") == 0)
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
	else if ((strcmp(pOneChar, "이") == 0 && IsExistEndingDic(pMiddleString2) == true &&
		strcmp(pMiddleString2, "상") != 0) ||
		(strcmp(pChoJungChar, "이") == 0 && IsExistEndingDic(pAddTwoString2) == true &&
			strcmp(pMiddleString2, "상") != 0) ||
			(strcmp(pOneChar, "이") == 0 && strcmp(pNextOneChar, "었") == 0) ||
		strcmp(pOneChar, "였") == 0 && IsExistEndingDic(pMiddleString2) == true)
	{
		//복합명사 추가 
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
			AddEndingFeature(pNode, "이");
			AddHTokenEnding(pNode, 26);
			AddEndingFeature(pNode, pAddTwoString2);
			if (pNextOneChar2 != NULL && strcmp(pNextOneChar2, "을") == 0)
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
			AddEndingFeature(pNode, "이");
			AddHTokenEnding(pNode, 26);
			AddEndingFeature(pNode, pJongChar);
			AddEndingFeature(pNode, pMiddleString2);
			if (pNextOneChar2 != NULL && strcmp(pNextOneChar2, "을") == 0)
			{
				AddEndingFeature(pNode, pNextOneChar2);
			}
			DeleteHTokenEnding(pNode, 4);
			DeleteHTokenEnding(pNode, 2);
			DeleteHTokenEnding(pNode, 25);
		}
	}
	//9n
	else if ((strcmp(pChoJungChar, "이") == 0 || strcmp(pChoJungChar, "하") == 0) &&
		strcmp(pJongChar, "ㅁ") == 0 &&
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

		if (strcmp(pChoJungChar, "이") == 0)
		{
			pAddTwoString = AddTwoStr("임", pMiddleString2);

			InsertEndingString(pNode, pAddTwoString);
			AddEndingFeature(pNode, pMiddleString2);
			SaveHTokenEnding(pNode);
			AddHTokenEnding(pNode, E_Pol);
		}
		else if (strcmp(pChoJungChar, "하") == 0)
		{
			pAddTwoString = AddTwoStr("함", pMiddleString2);

			DeleteHTokenStem(pNode, S_V);
			DeleteHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_Cmp);

			InsertEndingString(pNode, pAddTwoString);
			AddEndingFeature(pNode, pMiddleString2);
			SaveHTokenEnding(pNode);
		}
	}
	//10n
	else if ((pNextOneChar != NULL && strcmp(pNextOneChar, "씩") == 0) ||
		(pMiddleString3 != NULL &&
		(strcmp(pNextOneChar, "까지") == 0 ||
			strcmp(pNextOneChar, "부터") == 0 ||
			strcmp(pNextOneChar, "에서") == 0)))
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
	else if (((strcmp(pOneChar, "들") == 0 || strcmp(pOneChar, "씨") == 0 || strcmp(pOneChar, "대") == 0) && IsExistEndingDic(pMiddleString2) == true) ||
		(strcmp(pOneChar, "씩") == 0 && strcmp(pMiddleString2, "이라도") == 0))
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
	else if ((strcmp(pOneChar, "겠") == 0 || strcmp(pOneChar, "시") == 0 ||
		strcmp(pOneChar, "에") == 0 || strcmp(pOneChar, "과") == 0 ||
		strcmp(pOneChar, "로") == 0) &&
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
		if (strcmp(pMiddleString, "적") == 0 || strcmp(pMiddleString, "성") == 0 ||
			strcmp(pMiddleString, "화") == 0)
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

				//"적"이 붙는 명사에는 [T151]자질 추
				/*09.03 수정*/
				if (strcmp(pMiddleString, "적") == 0)
				{
					if (pNode->pHToken->bSubCompCreate == true)
					{
						pSubNode = pNode->pHToken->pSubTailNode->pPrev;
						if (pSubNode != pNode->pHToken->pSubHeadNode)
						{
							if (strcmp(pSubNode->pCSubToken->strResult, "적") == 0)
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
			//복합명사 추가 
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

	if (strcmp(pOneChar, "싸") == 0 || strcmp(pOneChar, "퍼") == 0)
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
	else if (strcmp(pOneChar, "건") == 0 && strcmp(pNextOneChar, "너") == 0)
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
		if (strcmp(pOneChar, "싶") == 0 || strcmp(pOneChar, "쌓") == 0 || strcmp(pOneChar, "놓") == 0 || strcmp(pOneChar, "있") == 0 ||
			strcmp(pOneChar, "좋") == 0 || strcmp(pOneChar, "먹") == 0 || strcmp(pOneChar, "찾") == 0 || strcmp(pOneChar, "밝") == 0 ||
			strcmp(pOneChar, "낳") == 0 || strcmp(pOneChar, "넣") == 0 || strcmp(pOneChar, "옮") == 0 || strcmp(pOneChar, "잃") == 0 ||
			strcmp(pOneChar, "붙") == 0 || strcmp(pOneChar, "눕") == 0 || strcmp(pOneChar, "없") == 0 || strcmp(pOneChar, "젊") == 0 ||
			strcmp(pOneChar, "옳") == 0 || strcmp(pOneChar, "밝") == 0 || strcmp(pOneChar, "늦") == 0 || strcmp(pOneChar, "낡") == 0 ||
			strcmp(pOneChar, "춥") == 0 || strcmp(pOneChar, "붉") == 0 || strcmp(pOneChar, "줍") == 0)

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
		if (strcmp(pOneChar, "어") == 0 || strcmp(pOneChar, "아") == 0 ||
			strcmp(pOneChar, "해") == 0)

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

			if (pOneChar != NULL && (strcmp(pOneChar, "졌") == 0 || strcmp(pOneChar, "져") == 0) &&
				IsExistStemDic(pMiddleString) && IsExistFeature(pMiddleString, STEM_DIC, 4) == false)
			{
				CHAR_DELETE(AddTwoString);
				AddTwoString = AddTwoStr(pMiddleString, "어지");

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
				AddTwoString = AddTwoStr("어", pJongChar);
				CHAR_DELETE(pMiddleString);

				pMiddleString = AddTwoStr(AddTwoString, pMiddleLastString);

				InsertEndingString(pNode, pMiddleString);
				SaveHTokenEnding(pNode);
				AddEndingFeature(pNode, pJongChar);
				AddEndingFeature(pNode, pMiddleLastString);
			}
			if ((pPrevOneChar != NULL && (strcmp(pPrevOneChar, "어") == 0 || strcmp(pPrevOneChar, "아") == 0) && pOneChar != NULL && (strcmp(pOneChar, "줬") == 0 || strcmp(pOneChar, "줘") == 0) &&
				IsExistStemDic(pMiddleString) && IsExistFeature(pMiddleString, STEM_DIC, 4) == false) ||
				(pPrevOneChar != NULL && strcmp(pPrevOneChar, "해") == 0 && pOneChar != NULL && (strcmp(pOneChar, "줬") == 0 || strcmp(pOneChar, "줘") == 0) &&
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
			else if (pOneChar != NULL && strcmp(pJongChar, "ㅆ") == 0 &&
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
			if (strcmp(pOneChar, "러") == 0 || strcmp(pOneChar, "려") == 0)
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

				if (pOneChar != NULL && strcmp(pJongChar, "ㅆ") == 0 &&
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
		if (strcmp(pOneChar, "내") == 0)
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
				(strcmp(pOneChar, "졌") == 0 || strcmp(pOneChar, "져") == 0) &&
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

				AddTwoString = AddTwoStr(pMiddleString, "어지");

				InsertStemString(pNode, AddTwoString);
				SaveHTokenStem(pNode);
				SaveHTokenStemFeature(pNode, pMiddleString);
				AddHTokenStem(pNode, S_Fnc);
				DeleteHTokenStem(pNode, S_Act);
				ClassfyKindChar(pNode, AddTwoString);

				CHAR_DELETE(pMiddleString);
				CHAR_DELETE(AddTwoString);

				AddTwoString = AddTwoStr("어", pJongChar);
				pMiddleString = AddTwoStr(AddTwoString, pMiddleString2);

				InsertEndingString(pNode, pMiddleString);
				SaveHTokenEnding(pNode);
				AddEndingFeature(pNode, pJongChar);
				AddEndingFeature(pNode, pMiddleString2);
			}
			else if (pOneChar != NULL &&
				(strcmp(pOneChar, "왔") == 0 || strcmp(pOneChar, "와") == 0) &&
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

	//개체명인식NER 03 
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
			if (strcmp(pOneChar, "된") == 0 || strcmp(pOneChar, "될") == 0 ||
				strcmp(pOneChar, "됨") == 0)
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
				// 자질 초기화
				for (i = 0; i<STEM_DIC_ALL_FEATURE_NUMBER; i++)
				{
					pNode->pHToken->subStem->strFeature[i] = '0';
				}
				AddHTokenStem(pNode, 2);
				AddHTokenStem(pNode, 4);
				AddHTokenStem(pNode, 40);
				//자질 N,Lex,Scmp 강제적으로 준다.
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
			// 자질 초기화
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
			// 자질 초기화
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
			// 자질 초기화
			for (i = 0; i<STEM_DIC_ALL_FEATURE_NUMBER; i++)
			{
				pNode->pHToken->subStem->strFeature[i] = '0';
			}
			AddHTokenStem(pNode, 2);
			AddHTokenStem(pNode, 4);
			AddHTokenStem(pNode, 40);
			//자질 N,Lex,Prp 강제적으로 준다.
		}
	}

	//// 수정
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

//개체명 인식(NER 01)
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
		strstr("회,위,단,협,부,처,원,청", pLastOneChar) == 0 &&
		strstr("와,적,성,화,에,게,가,는", pLastOneChar) == 0 &&
		strstr("전,이,로,과,랑,개,율,률", pLastOneChar) == 0 &&
		strstr(pOriginalString, "씨") == 0)
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
		(strcmp(pFirstTwoChar, "남궁") == 0 || strcmp(pFirstTwoChar, "선우") == 0 ||
			strcmp(pFirstTwoChar, "제갈") == 0 || strcmp(pFirstTwoChar, "사공") == 0 ||
			strcmp(pFirstTwoChar, "서문") == 0 || strcmp(pFirstTwoChar, "독고") == 0 ||
			strcmp(pFirstTwoChar, "황보") == 0))
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
	else if ((strcmp(pLastOneChar, "동") == 0 || strcmp(pLastOneChar, "읍") == 0 ||
		strcmp(pLastOneChar, "리") == 0) &&
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

//개체명 인식(NER 02)
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
			nLen == 2 && (strcmp(pSubNodeLastStr, "씨") == 0 || strcmp(pSubNodeLastStr, "님") == 0))
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
			(strcmp(pSubNodeLastStr, "회") == 0 || strcmp(pSubNodeLastStr, "위") == 0 ||
				strcmp(pSubNodeLastStr, "단") == 0 || strcmp(pSubNodeLastStr, "협") == 0 ||
				strcmp(pSubNodeLastStr, "부") == 0 || strcmp(pSubNodeLastStr, "처") == 0 ||
				strcmp(pSubNodeLastStr, "원") == 0 || strcmp(pSubNodeLastStr, "청") == 0))
		{
			//2y
			AddHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_Lex);
			AddHTokenStem(pNode, S_Org);
			AddHTokenStem(pNode, S_Prp);
			bRet = true;
		}
		//2n
		else if (nLen == 2 && (strcmp(pSubNodeLastStr, "산") == 0 || strcmp(pSubNodeLastStr, "강") == 0))
		{
			//3y
			AddHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_Lex);
			AddHTokenStem(pNode, S_Plc);
			AddHTokenStem(pNode, S_Prp);
		}
		//3n
		else if (nLen == 4 &&
			(strcmp(pSubNodeLastStr, "본부") == 0 || strcmp(pSubNodeLastStr, "원회") == 0 ||
				strcmp(pSubNodeLastStr, "협회") == 0))
		{
			//4y
			AddHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_Lex);
			AddHTokenStem(pNode, S_Org);
			AddHTokenStem(pNode, S_Prp);
			bRet = true;
		}
		//4n
		else if (strstr(pSubNodeLastStr, "위원회") != 0 || strstr(pSubNodeLastStr, "회의소") != 0 ||
			strstr(pSubNodeLastStr, "사령부") != 0)
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
			pEndingStr != NULL && (strcmp(pEndingStr, "에게") == 0 || strcmp(pEndingStr, "에게서") == 0 ||
				strcmp(pEndingStr, "에게로") == 0 || strcmp(pEndingStr, "께서") == 0 ||
				strcmp(pEndingStr, "님") == 0))
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
		else if (pEndingStr != NULL && strcmp(pEndingStr, "에서부터") == 0)
		{
			//7y
			AddHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_Lex);
			AddHTokenStem(pNode, S_Plc);
			AddHTokenStem(pNode, S_Prp);
			bRet = true;
		}
		//7n
		else if (nLen == 2 && (strcmp(pSubNodeLastStr, "률") == 0 ||
			strcmp(pSubNodeLastStr, "율") == 0))
		{
			//8y
			AddHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_Lex);
			AddHTokenStem(pNode, S_Qnt);
			bRet = true;
		}
		//8n
		else if (nLen == 2 && strcmp(pSubNodeLastStr, "장") == 0 &&
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

//개체명 인식(NER 03)
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
			if (strcmp(pEndString, "에게") == 0 || strcmp(pEndString, "에게서") == 0 ||
				strcmp(pEndString, "에게로") == 0 || strcmp(pEndString, "께서") == 0 ||
				strcmp(pEndString, "님") == 0)
			{
				//2y
				for (i = 0; i<STEM_DIC_ALL_FEATURE_NUMBER; i++) { pStemFeature[i] = '0'; } // 자질 초기화
				AddHTokenStem(pNode, S_N);
				AddHTokenStem(pNode, S_Lex);
				AddHTokenStem(pNode, S_Ani);
				AddHTokenStem(pNode, S_Conc);
				AddHTokenStem(pNode, S_Hum);
				AddHTokenStem(pNode, S_Prp);
			}
			//2n
			else if (strcmp(pEndString, "에서부터") == 0)
			{
				//3y
				for (i = 0; i<STEM_DIC_ALL_FEATURE_NUMBER; i++) { pStemFeature[i] = '0'; } // 자질 초기화
				AddHTokenStem(pNode, S_N);
				AddHTokenStem(pNode, S_Lex);
				AddHTokenStem(pNode, S_Plc);
				AddHTokenStem(pNode, S_Prp);
			}
			//3n
			else if (strcmp(pLastOneChar, "씨") == 0 || strcmp(pLastOneChar, "님") == 0)
			{
				//4y
				for (i = 0; i<STEM_DIC_ALL_FEATURE_NUMBER; i++) { pStemFeature[i] = '0'; } // 자질 초기화
				AddHTokenStem(pNode, S_N);
				AddHTokenStem(pNode, S_Lex);
				AddHTokenStem(pNode, S_Ani);
				AddHTokenStem(pNode, S_Conc);
				AddHTokenStem(pNode, S_Hum);
				AddHTokenStem(pNode, S_Prp);
			}
			//4n
			else if (strcmp(pLastOneChar, "산") == 0 || strcmp(pLastOneChar, "강") == 0 ||
				strcmp(pLastOneChar, "섬") == 0)
			{
				//5y
				for (i = 0; i<STEM_DIC_ALL_FEATURE_NUMBER; i++) { pStemFeature[i] = '0'; } // 자질 초기화
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
				if (strcmp(pSubNode->pCSubToken->strResult, "적") == 0 ||
					strcmp(pSubNode->pCSubToken->strResult, "성") == 0 ||
					strcmp(pSubNode->pCSubToken->strResult, "화") == 0 ||
					strcmp(pSubNode->pCSubToken->strResult, "등") == 0)
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

	m_nUWRFlag = 2;// 전처리 변수(복합명사)
	m_pNode = m_LinkedList->m_pHeadNode->pNext;
	m_nListPosition = m_LinkedList->m_nTotalCount;

	m_nListCount = 1;

	while (m_pNode != m_LinkedList->m_pTailNode)
	{
		// bMarsk가 true이면 형태소 분석 처리 안함(전처리에서 이미 분석함)
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

			GoUnitNode(m_pNode);//복합명사 미처리

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
		else {//Skip인 경우
			m_pNode = m_pNode->pNext;
			m_nListPosition -= 1;
			m_nListCount += 1;
		}
	} //while
	m_nListPosition = 0;
	m_nListCount = 0;

	//******************************************************************************
	// 중위성 분석
	//******************************************************************************
	CreateAmbiScript();

	//******************************************************************************
	// 질의응답 인덱스/질의어 분석
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
			if (strcmp(pOneChar, "우") == 0 || strcmp(pOneChar, "오") == 0)
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
					strcpy(pJongChar, "ㅂ");

					pChoJungChar1 = GetChojungJongsungChar(pOneChar, CHAR_CHOJUNGSUNG);
					if (pChoJungChar1 != NULL && pChoJungChar1 != "" && strlen(pChoJungChar1) > 0)
					{
						if (m_pConvert != NULL)
						{
							m_pConvert->EvaluateHapChar(pHapChar, pChoJungChar1, pJongChar);
						}
						//완성형 코드 값이 없을 경우 이전의 값으로 처리한다.
						if (pHapChar != NULL && strlen(pHapChar) > 0)
						{
							if (CalculateWordCount(pStartCountChar, nPos, 1, CALCULATE_MINUS))//한글일 경우 처리_1
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
		strcmp(pOneChar, "°") == 0 || strcmp(pOneChar, "±") == 0 || strcmp(pOneChar, "˚") == 0 || strcmp(pOneChar, "%") == 0 || strcmp(pOneChar, "$") == 0 || strcmp(pOneChar, "￦") == 0)
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
//참고URL
//http://v.media.daum.net/v/20170814155436552?rcmd=rn
**************************************************************************************/