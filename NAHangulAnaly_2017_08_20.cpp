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

char* CNAHangulAnaly::GetStartCountChar(char *pStr, int nMoveCount)
{
	char* pOriginalStr = NULL;
	char* pReturnStr = NULL;
	int nCount = 0;

	pReturnStr = new char[nMoveCount + 1];
	memset(pReturnStr, 0x00, nMoveCount + 1);

	pOriginalStr = pStr;

	while (nMoveCount)
	{
		pReturnStr[nCount] = *pOriginalStr;
		nCount += 1;
		pOriginalStr++;
		nMoveCount -= 1;
	}
	return pReturnStr;
}

char* CNAHangulAnaly::GetOneChar(char *pStr, int ns_Pos, int ne_Pos)
{
	char OneChar[3];
	char* pOriginalStr = NULL;
	char* pIsHangulCode = NULL;
	char* pReturnStr = NULL;

	int nPosition = 0;

	pOriginalStr = pStr;
	pIsHangulCode = pStr;
	nPosition = ns_Pos;

	while (nPosition > 1)
	{
		pIsHangulCode++;
		nPosition -= 1;
	}

	memset(OneChar, 0x00, sizeof(OneChar));
	OneChar[0] = *pIsHangulCode;
	if (OneChar[0] & 0x80)
	{
		ns_Pos -= 1;
		nPosition = ns_Pos;
	}
	else
	{
		nPosition = ns_Pos;
	}

	while (nPosition > 1)
	{
		pOriginalStr++;
		nPosition -= 1;
	}

	memset(OneChar, 0x00, sizeof(OneChar));
	OneChar[0] = *pOriginalStr;

	if (OneChar[0] & 0x80)
	{
		pReturnStr = new char[3];
		memset(pReturnStr, 0x00, 3);

		pReturnStr[0] = *pOriginalStr;
		pOriginalStr++;
		pReturnStr[1] = *pOriginalStr;
	}
	else
	{
		pReturnStr = new char[2];
		memset(pReturnStr, 0x00, 2);
		pReturnStr[0] = *pOriginalStr;
	}
	return pReturnStr;
}

char* CNAHangulAnaly::GetEndCountChar(char *pString, int nPos)
{
	char* pReturnCountChar = NULL;
	char* pOriginalString = NULL;

	int nLen = 0;
	int nCount = 0;

	pOriginalString = pString;

	if (CalculateOneWordCount(pOriginalString, nPos))
		nPos -= 1;

	while (nPos > 1)
	{
		pOriginalString++;
		nPos -= 1;
	}

	nLen = strlen(pOriginalString);

	pReturnCountChar = new char[nLen + 1];
	memset(pReturnCountChar, 0x00, nLen + 1);

	nCount = 0;

	while (nLen)
	{
		pReturnCountChar[nCount] = pOriginalString[nCount];
		nCount += 1;
		nLen -= 1;
	}
	return pReturnCountChar;
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
			nMiddleCount = (nEndPos - nStartPos) + 1;
			if (nMiddleCount == 1)
				nMiddleCount += 2;

			if (nMiddleCount > 0)
			{
				pReturnString = new char[nMiddleCount + 1];
				memset(pReturnString, 0x00, nMiddleCount + 1);

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
		nMove = nTotalCount - nStartPos + 1;
		nCount = 0;

		pReturnString = new char[nMove + 1];
		memset(pReturnString, 0x00, nMove + 1);
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
					pReturnChar = new char[nLen + 1];
					memset(pReturnChar, 0x00, nLen + 1);
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
				pReturnChar = new char[nLen + 1];
				memset(pReturnChar, 0x00, nLen + 1);
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

				pRturnStirng = new char[nOneStringCount + nChoJungCount + nJongCount + 1];
				memset(pRturnStirng, 0x00, nOneStringCount + nChoJungCount + nJongCount + 1);

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

			pRturnStirng = new char[nChoJungCount + 1];
			memset(pRturnStirng, 0x00, nChoJungCount + 1);
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

	pAddTwoString = new char[nTotalLen + 1];
	memset(pAddTwoString, 0x00, nTotalLen + 1);

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
			nMovePos = nCount + 1;
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

bool CNAHangulAnaly::IsExistEndDic(char *pEnd)
{
	bool isExist = false;
	isExist = m_pStemEndDic->EndFindIdx(pEnd);
	return isExist;
}

bool CNAHangulAnaly::IsExistEndDic(char *pEnd, char *pSaveBuffer)
{
	bool isExist = false;
	char Feature_End[48];
	char IS_CEHCK_FEATURE = '1';
	memset(Feature_End, 0x00, sizeof(Feature_End));
	isExist = m_pStemEndDic->EndGetRecord(pEnd, (char *)Feature_End);
	if (isExist == true)
	{
		memcpy((char*)pSaveBuffer, (char*)Feature_End, END_FEATURE_COUNT + 1);
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
	char Feature_Stem[236];
	char IS_CEHCK_FEATURE = '1';
	memset(Feature_Stem, 0x00, sizeof(Feature_Stem));
	isExist = m_pStemEndDic->StemGetRecord(pStem, (char *)Feature_Stem);
	if (isExist == true)
	{
		for (int i = 0; i<STEM_FEATURE_COUNT; i++)
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
		pNode->pHToken->subEnding->strResult = new char[nLen + 1];
		memset(pNode->pHToken->subEnding->strResult, 0x00, nLen + 1);
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
			pInsertNode->pCSubToken->strResult = new char[nLen + 1];
			memset(pInsertNode->pCSubToken->strResult, 0x00, nLen + 1);
			strcpy(pInsertNode->pCSubToken->strResult, pString);

			ClassfyKindChar(pInsertNode, pString);

			pInsertNode->pCSubToken->strFeature = new char[236];
			memset(pInsertNode->pCSubToken->strFeature, 0x00, 236);

			bool isExist = false;
			char Feature_End[236];
			char IS_CEHCK_FEATURE = '1';
			memset(Feature_End, 0x00, sizeof(Feature_End));

			if (m_pStemEndDic->StemFindIdx(pString) == true)
			{
				m_pStemEndDic->StemGetRecord(pString, (char *)Feature_End);
				for (int Feature = 0; Feature < 234; Feature++)
				{
					if (Feature_End[Feature] == IS_CEHCK_FEATURE)
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
				memset(pInsertNode->pCSubToken->strFeature, '0', STEM_FEATURE_COUNT - 1);
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

		nLen = strlen(pCopy) + 1;
		if (nLen > 1)
		{
			/*
			if (pNode->pHToken->subStem->strResult != NULL)
			{
			delete[] pNode->pHToken->subStem->strResult;
			}
			*/
			CHAR_DELETE(pNode->pHToken->subStem->strResult);
			pNode->pHToken->subStem->strResult = new char[nLen];
			memset(pNode->pHToken->subStem->strResult, 0x00, nLen);
			strcpy(pNode->pHToken->subStem->strResult, pCopy);
			for (int i = 0; i<STEM_FEATURE_COUNT; i++)
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

		nLen = strlen(pCopy) + 1;
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
			for (int i = 0; i<STEM_FEATURE_COUNT; i++)
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
		pNode->pHToken->subStem->strResult = new char[nLen + 1];
		memset(pNode->pHToken->subStem->strResult, 0x00, nLen + 1);

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
	pOriginalString = new char[nLen + 1];
	memset(pOriginalString, 0x00, nLen + 1);

	pOneChar = GetOneChar(pNode->pHToken->strSource, nPos, nPos);

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

		pNode->pHToken->strSource = new char[nLen + 1];
		memset(pNode->pHToken->strSource, 0x00, nLen + 1);


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

		pNode->pHToken->strSource = new char[nLen + 1];
		memset(pNode->pHToken->strSource, 0x00, nLen + 1);

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
				if (nMovePos + 1 == nPos)
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
	pOriginalString = new char[nLen + 1];
	memset(pOriginalString, 0x00, nLen + 1);
	nCount = 0;
	while (nLen)
	{
		pOriginalString[nCount] = pNode->pHToken->strSource[nCount];
		nLen -= 1;
		nCount += 1;
	}

	pOneWord = GetOneChar(pOriginalString, nPos, nPos);

	ReplaceHangulJA(pJongChar, nWord);

	pChoJungChar = GetChojungJongsungChar(pOneWord, CHAR_CHOJUNGSUNG);
	if (pChoJungChar != NULL && pChoJungChar != "")
	{
		if (m_pConvert != NULL)
			m_pConvert->EvaluateHapChar(pHapChar, pChoJungChar, pJongChar);

		nPos = ReturnNextPrevCount(pOriginalString, nPos, 1, CALCULATE_MINUS);

		nLen = strlen(pOriginalString);
		nCount = 0;
		if (pNode->pHToken->strSource != NULL && strlen(pNode->pHToken->strSource) > 0)
		{
			delete[]pNode->pHToken->strSource;
			pNode->pHToken->strSource = NULL;
		}

		pNode->pHToken->strSource = new char[nLen + 1];
		memset(pNode->pHToken->strSource, 0x00, nLen + 1);

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

	pOneChar = GetOneChar(pOriginalString, nSavePos, nSavePos);

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
	pOriginalString = new char[nLen + 1];
	memset(pOriginalString, 0x00, nLen + 1);
	nCount = 0;
	while (nLen)
	{
		pOriginalString[nCount] = pNode->pHToken->strSource[nCount];
		nLen -= 1;
		nCount += 1;
	}

	pOneWord = GetOneChar(pOriginalString, nPos, nPos);

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

			nPos = ReturnNextPrevCount(pOriginalString, nPos, 1, CALCULATE_MINUS);

			nLen = strlen(pOriginalString);
			nCount = 0;
			if (pNode->pHToken->strSource != NULL && strlen(pNode->pHToken->strSource) > 0)
			{
				delete[]pNode->pHToken->strSource;
				pNode->pHToken->strSource = NULL;
			}

			pNode->pHToken->strSource = new char[nLen + 1];
			memset(pNode->pHToken->strSource, 0x00, nLen + 1);

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

	pOneChar = GetOneChar(pOriginalString, nSavePos, nSavePos);

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
	pOriginalString = new char[nLen + 1];
	memset(pOriginalString, 0x00, nLen + 1);

	nCount = 0;
	while (nLen)
	{
		pOriginalString[nCount] = pNode->pHToken->strSource[nCount];
		nLen -= 1;
		nCount += 1;
	}

	pOneChar = GetOneChar(pOriginalString, nPos, nPos);

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

				pNode->pHToken->strSource = new char[nLen + 1];
				memset(pNode->pHToken->strSource, 0x00, nLen + 1);

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

	pCountChar = GetStartCountChar(pOriginalString, nSavePos);

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

	pOriginalString = new char[nLen + 1];
	memset(pOriginalString, 0x00, nLen + 1);
	nCount = 0;
	while (nLen)
	{
		pOriginalString[nCount] = pNode->pHToken->strSource[nCount];
		nLen -= 1;
		nCount += 1;
	}

	pOneChar = GetOneChar(pOriginalString, nPos, nPos);

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

			pNode->pHToken->strSource = new char[nLen + 1];
			memset(pNode->pHToken->strSource, 0x00, nLen + 1);

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
	pStartCountChar = GetStartCountChar(pOriginalString, nSavePos);

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
	char* pEnding;
	char IS_CEHCK_FEATURE = '1';

	pEnding = pEndingNode->pHToken->subEnding->strResult;

	CHAR_DELETE(pEndingNode->pHToken->subEnding->strFeature);

	pEndingNode->pHToken->subEnding->strFeature = new char[48];
	memset(pEndingNode->pHToken->subEnding->strFeature, 0x00, 48);

	if (IsExistEndDic(pEnding, pEndingNode->pHToken->subEnding->strFeature) == false)
	{
		for (int Feature = 0; Feature < END_FEATURE_COUNT; Feature++)
		{
			pEndingNode->pHToken->subEnding->strFeature[Feature] = '0';
		}
	}
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
	char* pStem;
	char IS_CEHCK_FEATURE = '1';
	pStem = pStemNode->pHToken->subStem->strResult;
	if (pStemNode->pHToken->subStem->strFeature == NULL)
	{
		pStemNode->pHToken->subStem->strFeature = new char[236];
		memset(pStemNode->pHToken->subStem->strFeature, 0x00, 236);
		for (int Feature = 0; Feature < STEM_FEATURE_COUNT; Feature++)
		{
			pStemNode->pHToken->subStem->strFeature[Feature] = '0';
		}
	}
	//복합명사 일 경우
	else if (pStemNode->pHToken->bSubCompCreate == true)
	{
		// Added by KKH 2003-12-01
		if (pStemNode->pHToken->subStem->strFeature != NULL)
		{
			CHAR_DELETE(pStemNode->pHToken->subStem->strFeature);
		}
		pStemNode->pHToken->subStem->strFeature = new char[236];
		memset(pStemNode->pHToken->subStem->strFeature, 0x00, 236);
		// Added by KKH 2003-12-01
		for (int Feature = 0; Feature < STEM_FEATURE_COUNT; Feature++)
		{
			pStemNode->pHToken->subStem->strFeature[Feature] = '0';
		}
	}

	if (IsExistStemDic(pStem, pStemNode->pHToken->subStem->strFeature) == false)
	{
		//for(int Feature = 0; Feature < STEM_FEATURE_COUNT; Feature++)
		//{
		//	pStemNode->pHToken->subStem->strFeature[Feature] = '0';
		//}
	}
}

void CNAHangulAnaly::SaveHTokenStemFeature(H_NODE *pStemNode, char *pStem)
{
	char IS_CEHCK_FEATURE = '1';
	if (IsExistStemDic(pStem) == true)
	{
		if (pStemNode->pHToken->subStem->strFeature == NULL)
		{
			pStemNode->pHToken->subStem->strFeature = new char[236];
			memset(pStemNode->pHToken->subStem->strFeature, 0x00, 236);
			for (int Feature = 0; Feature < STEM_FEATURE_COUNT; Feature++)
			{
				pStemNode->pHToken->subStem->strFeature[Feature] = '0';
			}
		}
		if (IsExistStemDic(pStem, pStemNode->pHToken->subStem->strFeature) == false)
		{
		}
	}
	else
	{
	}
}

bool CNAHangulAnaly::IsExistFeature(char *pString, int nKindDic, int nFeature)
{
	bool isExist = false;
	char Feature_Stem[236];
	char Feature_End[58];
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
		if (IsExistEndDic(pString, (char *)Feature_End) == true)
		{
			if (Feature_End[nFeature] == IS_CHECK_FEATURE)
				isExist = true;
		}
		break;
	}
	return isExist;
}

int CNAHangulAnaly::ReturnNumberMoveLeftWordCount(char *pString, int &nCount)
{
	int nReturnNumber = 0;

	char OneChar[3];
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

bool CNAHangulAnaly::IsSameWordPos(H_NODE *pNode, int nSecondAlkong)
{
	char* pOriginalString = NULL;
	char* pBufferOneChar = NULL;
	char* pOneChar = NULL;
	bool isSameWord = false;
	int nCount = 0;
	int nLen = 0;

	pOriginalString = pNode->pHToken->strSource;

	pOneChar = GetOneChar(pOriginalString, nSecondAlkong, nSecondAlkong);

	if (m_pNewBuffer[0] & 0x80)
		nLen = 2;
	else
		nLen = 1;

	pBufferOneChar = new char[nLen + 1];
	memset(pBufferOneChar, 0x00, nLen + 1);
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

void CNAHangulAnaly::ChangePosAnyAlkong(H_NODE *pNode, int &nStart, int &nAlkong, int &nSecondAlkong, int nCase)
{
	int nTotalLen = 0;
	if (m_pOldBuffer != NULL)
	{
		if (m_pOldBuffer[0] & 0x80)
			nTotalLen = nStart + 2;
		else
			nTotalLen = nStart + 1;

		switch (nCase)
		{
		case CHANGE_FIRST_ALKONG:
			nStart = nTotalLen;
			break;
		case CHANGE_SECOND_ALKONG:
			nAlkong = nTotalLen;
			break;
		case CHANGE_THIRD_ALKONG:
			nSecondAlkong = nTotalLen;
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
	pOriginalString = new char[nLen + 1];
	memset(pOriginalString, 0x00, nLen + 1);

	nCount = 0;

	while (nLen)
	{
		pOriginalString[nCount] = pNode->pHToken->strSource[nCount];
		nLen -= 1;
		nCount += 1;
	}

	pOneChar = GetOneChar(pOriginalString, nPos, nPos);

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

			pNode->pHToken->strSource = new char[nLen + 1];
			memset(pNode->pHToken->strSource, 0x00, nLen + 1);

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

int CNAHangulAnaly::ReturnNextPrevCount(char *pOriginalStr, int nStartPos, int nCount, int nCase)
{
	int nReturnValue = 0;

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
			nReturnValue = nStartPos + 2;
			if (strlen(pOriginalStr) < (unsigned)nReturnValue)
				nReturnValue = strlen(pOriginalStr);
		}
		else
		{
			nReturnValue = nStartPos + 1;
			if (strlen(pOriginalStr) < (unsigned)nReturnValue)
				nReturnValue = strlen(pOriginalStr);
		}
		break;
	}

	return nReturnValue;
}

bool CNAHangulAnaly::IsExistNFeature(char *pString, int nStart, int nFeature)
{
	bool isExist = false;
	char Feature_Stem[236];
	int FindIndex = -1;
	char IS_CHECK_FEATURE = '1';
	char* pStartCountChar = NULL;
	pStartCountChar = GetStartCountChar(pString, nStart);

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
	for (nFeature = 0; nFeature < END_FEATURE_COUNT; nFeature++)
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
	for (nFeature = 0; nFeature < END_FEATURE_COUNT; nFeature++)
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
	char Feature_End[48];
	char IS_CEHCK_FEATURE = '1';
	memset(Feature_End, 0x00, sizeof(Feature_End));

	if (pString != NULL && strlen(pString) > 0)
	{
		if (IsExistEndDic(pString) == true)
		{
			IsExistEndDic(pString, (char *)Feature_End);
			if (pEndingNode->pHToken->subEnding->strFeature == "")
			{
				for (int Feature = 0; Feature < END_FEATURE_COUNT; Feature++)
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
					pEndingNode->pHToken->subEnding->strFeature = new char[48];
					memset(pEndingNode->pHToken->subEnding->strFeature, 0x00, 48);
				}
				for (int Feature = 0; Feature < END_FEATURE_COUNT; Feature++)
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
		pEndingNode->pHToken->subEnding->strFeature = new char[48];
		memset(pEndingNode->pHToken->subEnding->strFeature, 0x00, 48);
	}
	else
	{
		pEndingNode->pHToken->subEnding->strFeature = new char[48];
		memset(pEndingNode->pHToken->subEnding->strFeature, 0x00, 48);
	}
}


void CNAHangulAnaly::InsertArrayFeatureNew(char *pString)
{
	char Feature_End[48];
	char IS_CEHCK_FEATURE = '1';
	int Feature = 0;
	int FindIndex = -1;

	memset(Feature_End, 0x00, sizeof(Feature_End));

	if (pString != NULL && strlen(pString) > 0)
	{
		if (IsExistEndDic(pString) == true)
		{
			if (m_ArrayNewBuffer != "")
				memset(m_ArrayNewBuffer, 0x00, sizeof(m_ArrayNewBuffer));
			IsExistEndDic(pString, (char *)Feature_End);
			for (Feature = 0; Feature < END_FEATURE_COUNT; Feature++)
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
	char Feature_End[48];
	char IS_CEHCK_FEATURE = '1';
	int Feature = 0;
	int FindIndex = -1;
	int nLen = 0;

	memset(Feature_End, 0x00, sizeof(Feature_End));

	//에서는 -> 는 -> 서는 -> 에 + 서는 
	//으신 -> ㄴ + 시 + 으 
	if (pString != NULL && strlen(pString) > 0)
	{
		if (IsExistEndDic(pString) == true)
		{
			IsExistEndDic(pString, (char *)Feature_End);
			if (m_pPrevOldBuffer != NULL)
			{
				int nCount = 0;
				int nTotalLen = 0;
				nLen = strlen(m_pPrevOldBuffer);
				char* pBuffer = NULL;
				pBuffer = new char[nLen + 1];
				memset(pBuffer, 0x00, nLen + 1);
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

				//에서는 -> 는 -> 서는 -> 에 + 서는 
				//으신 -> ㄴ + 시 + 으 
			}
			else
			{
				if (m_ArrayOldBuffer != "")
					memset(m_ArrayOldBuffer, 0x00, sizeof(m_ArrayOldBuffer));
			}

			for (Feature = 0; Feature < END_FEATURE_COUNT; Feature++)
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

		m_pPrevOldBuffer = new char[nLen + 1];
		memset(m_pPrevOldBuffer, 0x00, nLen + 1);
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


	for (nLen = 0; nLen < END_FEATURE_COUNT; nLen++)
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
				pNode->pHToken->strAmbEndingSource = new char[nLen + 1];
				memset(pNode->pHToken->strAmbEndingSource, 0x00, nLen + 1);
				strcpy(pNode->pHToken->strAmbEndingSource, pAddTwoString);
			}
			else
			{
				nLen = strlen(pAddTwoString2);
				nCount = 0;
				pNode->pHToken->strAmbEndingSource = new char[nLen + 1];
				memset(pNode->pHToken->strAmbEndingSource, 0x00, nLen + 1);
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
				pNode->pHToken->strAmbEndingSource = new char[nLen + 1];
				memset(pNode->pHToken->strAmbEndingSource, 0x00, nLen + 1);
				strcpy(pNode->pHToken->strAmbEndingSource, pAddTwoString2);
			}
			else
			{
				nLen = strlen(pAddTwoString);
				nCount = 0;
				pNode->pHToken->strAmbEndingSource = new char[nLen + 1];
				memset(pNode->pHToken->strAmbEndingSource, 0x00, nLen + 1);
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
				pNode->pHToken->strAmbEndingSource = new char[nLen + 1];
				memset(pNode->pHToken->strAmbEndingSource, 0x00, nLen + 1);
				strcpy(pNode->pHToken->strAmbEndingSource, pAddTwoString2);
			}
			else
			{
				nLen = strlen(pAddTwoString);
				nCount = 0;
				pNode->pHToken->strAmbEndingSource = new char[nLen + 1];
				memset(pNode->pHToken->strAmbEndingSource, 0x00, nLen + 1);
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
				pNode->pHToken->strAmbEndingSource = new char[nLen + 1];
				memset(pNode->pHToken->strAmbEndingSource, 0x00, nLen + 1);
				strcpy(pNode->pHToken->strAmbEndingSource, pAddTwoString2);
			}
			else
			{
				nLen = strlen(pAddTwoString);
				nCount = 0;
				pNode->pHToken->strAmbEndingSource = new char[nLen + 1];
				memset(pNode->pHToken->strAmbEndingSource, 0x00, nLen + 1);
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
				pNode->pHToken->strAmbEndingSource = new char[nLen + 1];
				memset(pNode->pHToken->strAmbEndingSource, 0x00, nLen + 1);
				strcpy(pNode->pHToken->strAmbEndingSource, pAddTwoString2);
			}
			else
			{
				nLen = strlen(pAddTwoString);
				nCount = 0;
				pNode->pHToken->strAmbEndingSource = new char[nLen + 1];
				memset(pNode->pHToken->strAmbEndingSource, 0x00, nLen + 1);
				strcpy(pNode->pHToken->strAmbEndingSource, pAddTwoString);
			}
		}
	}
	if (m_pNewBuffer != NULL)
		nLen = strlen(m_pNewBuffer);

	if (m_pOldBuffer != NULL)
		nLen += strlen(m_pOldBuffer);

	pTempBuffer = new char[nLen + 1];
	memset(pTempBuffer, 0x00, nLen + 1);

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
	m_pOldBuffer = new char[nLen + 1];
	memset(m_pOldBuffer, 0x00, nLen + 1);
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


void CNAHangulAnaly::MoveLeftWordCount(char *pString, int &nCount)
{
	char OneChar[3];
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
}

void CNAHangulAnaly::SaveAmbstrEnding(H_NODE *pNode, char *pEndStr)
{
	bool bFindAmbFeature = false;
	char Feature_End[48];
	char IS_CEHCK_FEATURE = '1';
	memset(Feature_End, 0x00, sizeof(Feature_End));
	int nLen = 0;
	char* pAddTwoString = NULL;
	char* pAddTwoString2 = NULL;

	if (pEndStr != NULL)
	{
		if (IsExistEndDic(pEndStr) == true)
		{
			IsExistEndDic(pEndStr, (char *)Feature_End);
			if (Feature_End[0] == IS_CEHCK_FEATURE)
			{
				if (pNode->pHToken->strAmbEndingSource == NULL)
				{
					nLen = strlen(pEndStr);
					pNode->pHToken->strAmbEndingSource = new char[nLen + 1];
					memset(pNode->pHToken->strAmbEndingSource, 0x00, nLen + 1);
					strcpy(pNode->pHToken->strAmbEndingSource, pEndStr);
				}
				else
				{
					pAddTwoString = AddTwoStr(pNode->pHToken->strAmbEndingSource, "%");
					pAddTwoString2 = AddTwoStr(pAddTwoString, pEndStr);

					nLen = strlen(pAddTwoString2);

					CHAR_DELETE(pNode->pHToken->strAmbEndingSource);

					pNode->pHToken->strAmbEndingSource = new char[nLen + 1];
					memset(pNode->pHToken->strAmbEndingSource, 0x00, nLen + 1);
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
	pNode->pHToken->strSource = new char[nLen + 1];
	memset(pNode->pHToken->strSource, 0x00, nLen + 1);
	nCount = 0;
	strcpy(pNode->pHToken->strSource, m_pRealOriginalString);


	CHAR_DELETE(pNode->pHToken->subStem->strResult);
	//CHAR_DELETE(pNode->pHToken->subStem->strFeature);
	CHAR_DELETE(pNode->pHToken->subEnding->strResult);
	CHAR_DELETE(pNode->pHToken->subEnding->strFeature);

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
		m_pRealOriginalString = new char[nLen + 1];
		memset(m_pRealOriginalString, 0x00, nLen + 1);
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
	AllInitMemberVariable();

	char* pOriginalString = NULL;
	char* pOneChar = NULL;
	char* pJongChar = NULL;
	char* pStartCountChar = NULL;

	bool isExist = false;
	int nStart = 0;
	int mAlkong = 0;
	int mSecondAlkong = 0;
	int nTotalLen = 0;

	if (pNode == NULL)
	{
		return;
	}
	if (pNode->pHToken == NULL)
	{
		return;
	}
	if (pNode->pHToken->strSource == NULL)
	{
		return;
	}
	pOriginalString = pNode->pHToken->strSource;
	m_nInsertNodeCount = 0;
	InitRealOriginalString();
	SaveRealOriginalStr(pOriginalString);

	nTotalLen = strlen(pOriginalString);
	nStart = nTotalLen;

	char Feature_Stem[STEM_FEATURE_COUNT];
	char IS_CEHCK_FEATURE = '1';
	memset(Feature_Stem, 0x00, sizeof(Feature_Stem));
	//S(len～1)이 stem 사전에 있는가 ? /*전체 어절이 사전에 있을 경우, 기존 처리 그대로
	isExist = IsExistStemDic(pOriginalString, Feature_Stem);
	if (isExist == true)
	{
		//1y : 그 stem이[v]인가 ?
		if (Feature_Stem[7] == IS_CEHCK_FEATURE)
		{
			pOneChar = GetOneChar(pOriginalString, nStart, nStart);
			pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);
			//2y : S(1)이 “이”인가 ?
			if (strcmp(pOneChar, "이") == 0)
			{
				//3y : ①로 
				GoMAR1_1(pNode, nStart, mAlkong, mSecondAlkong);
			}
			//3n : C(1)이 “ㄹ”인가? 
			else if (strcmp(pJongChar, "ㄹ") == 0)
			{
				//4y : stem = S(len～1), ending = “ㄹ” + [v][aff][unc][fut]  
				pStartCountChar = GetStartCountChar(pOriginalString, nStart);
				InsertStemString(pNode, pStartCountChar);
				SaveHTokenStem(pNode);

				InsertEndingString(pNode, "ㄹ");
				SaveHTokenEnding(pNode);
				AddHTokenEnding(pNode, 3);
				AddHTokenEnding(pNode, 1);
				AddHTokenEnding(pNode, 42);
				AddHTokenEnding(pNode, 18);
			}
			else
			{
				//4n : stem = S(len～1), ending = “어” + [v][aff][con]
				pStartCountChar = GetStartCountChar(pOriginalString, nStart);
				InsertStemString(pNode, pStartCountChar);
				SaveHTokenStem(pNode);

				InsertEndingString(pNode, "어");
				SaveHTokenEnding(pNode);
				AddHTokenEnding(pNode, 3);
				AddHTokenEnding(pNode, 1);
				AddHTokenEnding(pNode, 28);
			}
		}
		//2n : stem = S(len～1), ending = null
		else
		{
			pStartCountChar = GetStartCountChar(pOriginalString, nStart);
			InsertStemString(pNode, pStartCountChar);
			SaveHTokenStem(pNode);
		}
	}
	else
	{
		GoMAR1_1(pNode, nStart, mAlkong, mSecondAlkong);
	}

	AllInitMemberVariable();
	InitRealOriginalString();

	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pJongChar);
	CHAR_DELETE(pStartCountChar);
}

void CNAHangulAnaly::GoMAR1_1(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong)
{
	char* pOriginalString = NULL;
	char* pOneChar = NULL;
	char* pJongChar = NULL;

	pOriginalString = pNode->pHToken->strSource;

	if (m_bIsFirstWord == false)
	{
		MoveLeftWordCount(pOriginalString, nStart);
		InitOneWordMarsk();
	}
	m_bIsFirstWord = false;

	//1n : ① n++ → n > len? 
	if (nStart <= 0)
	{
		//1'y : Error 
		GoErrorFunction(pNode);
	}
	else
	{

		pOneChar = GetOneChar(pOriginalString, nStart, nStart);
		pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);
		//1'n : S(n)이 [mask]||“럼”이 아니고, {S(n) == txx  ||“기”||“답”||“럽”|| C(n) == “ㅆ”||“ㅁ”||“ㄻ”}인가? 
		//txx == Table01[0], Table02[1], Table02[2], Table03[0][1],Table04[1][2][3], Table05, Table06, Table07
		//if ((m_bOneCharMask == false) &&
		if ((m_bOneCharMask == false || strcmp(pOneChar, "럼") == 0) 
			&&
			(CompareVariousTable01(pOneChar, 0, false) == true
				|| CompareVariousTable02(pOneChar, 1, false) == true
				|| CompareVariousTable02(pOneChar, 2, false) == true
				|| CompareVariousTable03(pOneChar, 0, false) == true
				|| CompareVariousTable03(pOneChar, 1, false) == true
				|| CompareVariousTable04(pOneChar, 1, false) == true
				|| CompareVariousTable04(pOneChar, 2, false) == true
				|| CompareVariousTable04(pOneChar, 3, false) == true
				|| CompareVariousTable05(pOneChar, ALL_TABLE_05, true) == true
				|| CompareVariousTable06(pOneChar, ALL_TABLE_06, true) == true
				|| CompareVariousTable07(pOneChar, ALL_TABLE_07, true) == true
				|| strcmp(pOneChar, "기") == 0
				|| strcmp(pOneChar, "답") == 0
				|| strcmp(pOneChar, "럽") == 0
				|| strcmp(pJongChar, "ㅆ") == 0
				|| strcmp(pJongChar, "ㅁ") == 0
				|| strcmp(pJongChar, "ㄻ") == 0
			)
		)
		{
			if (strcmp(pOneChar, "다") == 0 || strcmp(pOneChar, "야") == 0 ||
				strcmp(pOneChar, "여") == 0 || strcmp(pOneChar, "이") == 0 ||
				strcmp(pOneChar, "가") == 0 || strcmp(pOneChar, "터") == 0 ||
				strcmp(pOneChar, "럼") == 0 || strcmp(pOneChar, "려") == 0 ||
				strcmp(pOneChar, "큼") == 0)
			{
				GoMAR3(pNode, nStart, mAlkong, mSecondAlkong);
			}
			else
			{
				GoIMR1(pNode, nStart, mAlkong, mSecondAlkong);
			}
		}
		else
		{
			GoMAR1_2(pNode, nStart, mAlkong, mSecondAlkong);
		}
	}

	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pJongChar);
}

void CNAHangulAnaly::GoMAR1_2(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong)
{
	char* pOriginalString = NULL;

	char* pThisStartCountChar = NULL;
	char* pNextStartCountChar = NULL;
	char* pNextEndCountChar = NULL;
	char* pPrevEndCountChar = NULL;
	char* pThisEndCountChar = NULL;
	char* pLastOneChar = NULL;
	char* pOneChar = NULL;
	char* pOneChar2 = NULL;
	char* pOneChar3 = NULL;
	char* pJongChar = NULL;
	char* pJongChar2 = NULL;
	char* pJongChar3 = NULL;
	char* pOldJongChar = NULL;
	char* pChoJungChar = NULL;
	char* pAddTwoString = NULL;
	char* pAddTwoString2 = NULL;
	char* pAddTwoString3 = NULL;
	char* pMinusTwoString = NULL;

	int nLen = 0;
	int nCount = 0;
	int nRestoration = -1;

	pOriginalString = pNode->pHToken->strSource;

	nCount = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_MINUS);
	pThisEndCountChar = GetEndCountChar(pOriginalString, nStart);//S(n~1)
	pNextEndCountChar = GetEndCountChar(pOriginalString, nCount);//S(n+1~1)

	//2n : ② *S(n～1)이 ending 사전에 있는가 ?
	if (IsExistWildEndingDic(pThisEndCountChar) == true && strlen(pThisEndCountChar) > 0)
	{
		mAlkong = nStart;
		pOneChar = GetOneChar(pOriginalString, nStart, nStart);
		pLastOneChar = GetOneChar(pOriginalString, strlen(pOriginalString), strlen(pOriginalString));//S(1)
		//3y : m = n → S(n～1)이 ending 사전에 있는가 ?
		if (IsExistEndDic(pThisEndCountChar))
		{
			pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);  // S(len~n+1)
			//4y : S(len～n + 1)이 stem 사전에 있는가 ?
			if (IsExistStemDic(pNextStartCountChar))
			{
				CHAR_DELETE(pOneChar);

				pOneChar = GetOneChar(pOriginalString, nCount, nCount);
				pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);
				pAddTwoString = AddTwoStr(pJongChar, pThisEndCountChar);

				pOneChar2 = GetOneChar(pOriginalString, nStart, nStart);
				pJongChar2 = GetChojungJongsungChar(pOneChar2, CHAR_JONGSUNG);

				nRestoration = Restoration(pJongChar2);

				nCount = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_PLUS);
				pPrevEndCountChar = GetEndCountChar(pOriginalString, nCount);
				pAddTwoString2 = AddTwoStr(pJongChar2, pPrevEndCountChar);
				//5y : {S(len～n + 1) == “자신” || “길” || “안” || “틀” || “말” | “풀” || “끌” || “굴” || “얼” || “달” || “불” || “살” || “덜” || “알” || “놀” || “열” || “허물” || “열” || “팔”}인가 ?
				if (strcmp(pNextStartCountChar, "자신") == 0 || strcmp(pNextStartCountChar, "길") == 0 ||
					strcmp(pNextStartCountChar, "안") == 0 || strcmp(pNextStartCountChar, "틀") == 0 ||
					strcmp(pNextStartCountChar, "말") == 0 || strcmp(pNextStartCountChar, "풀") == 0 ||
					strcmp(pNextStartCountChar, "끌") == 0 || strcmp(pNextStartCountChar, "굴") == 0 ||
					strcmp(pNextStartCountChar, "얼") == 0 || strcmp(pNextStartCountChar, "달") == 0 ||
					strcmp(pNextStartCountChar, "불") == 0 || strcmp(pNextStartCountChar, "살") == 0 ||
					strcmp(pNextStartCountChar, "덜") == 0 || strcmp(pNextStartCountChar, "알") == 0 ||
					strcmp(pNextStartCountChar, "놀") == 0 || strcmp(pNextStartCountChar, "덥") == 0 ||
					strcmp(pNextStartCountChar, "열") == 0 || strcmp(pNextStartCountChar, "팔") == 0)
				{
					//6y : stem = S(len～n + 1), ending = S(n～1)
					InsertStemString(pNode, pNextStartCountChar); //6y
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, pThisEndCountChar);
					SaveHTokenEnding(pNode);
				}
				//6n : C(n + 1) + S(n ～ 1)이 ending 사전에 있는가 ?
				else if (IsExistEndDic(pAddTwoString) && strlen(pJongChar) > 0) //6n
				{
					nCount = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_MINUS);
					pMinusTwoString = MinusTwoStr(pNextStartCountChar, pOneChar, nCount);
					//7y : (S(len～n + 1) - C(n + 1))이 stem 사전에 있는가 ?
					if (IsExistStemDic(pMinusTwoString))
					{
						//8y : {(S(len～n + 1) - C(n + 1)) != :자시: || : 여느 : }이고
						//, { (S(len～n + 1) - C(n + 1)) == [N] && C(n + 1) + S(n～1) == [N] }이거나
						//, { (S(len～n + 1) - C(n + 1)) <>[N] && C(n + 1) + S(n～1) <>[N] }이거나
						//, { (S(len～n + 1) - C(n + 1)) == [amb] || C(n + 1) + S(n～1) == [amb] }인가 ?
						if (
							strcmp(pMinusTwoString, "자시") != 0 && strcmp(pMinusTwoString, "여느") != 0 
							&&
							(
								(IsExistFeature(pMinusTwoString, STEM_DIC, 4) == true && IsExistFeature(pAddTwoString, END_DIC, 2) == true) 
								||(IsExistFeature(pMinusTwoString, STEM_DIC, 4) == false && IsExistFeature(pAddTwoString, END_DIC, 2) == false) 
								||(IsExistFeature(pMinusTwoString, STEM_DIC, 1) == true || IsExistFeature(pAddTwoString, END_DIC, 0) == true)
							)
						)
						{
							//9y : stem = S(len～n + 1) - C(n + 1), ending = C(n + 1) + S(n) + OldBuf()
							InsertStemString(pNode, pMinusTwoString);
							SaveHTokenStem(pNode);

							CHAR_DELETE(pAddTwoString);
							CHAR_DELETE(pAddTwoString2);

							pAddTwoString = AddTwoStr(pJongChar, pOneChar2);
							pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);

							InsertEndingString(pNode, pAddTwoString2);
							if (IsExistEndDic(pAddTwoString2)) //9y
							{
								SaveHTokenEnding(pNode);
							}
							else
							{
								MakeNewEndingFeature(pNode);

								AddEndingFeature(pNode, pJongChar);
								AddEndingFeature(pNode, pOneChar);
								SaveHTokenEndingOldFeature(pNode);

								SaveAmbstrEnding(pNode, pJongChar);
								SaveAmbstrEnding(pNode, pOneChar2);
								SaveAmbstrEnding(pNode, m_pOldBuffer);
							}
						}
						//9n : {S(len～n + 1) == [N] && S(n～1) == [N]}이거나, { S(len～n + 1) <>[N] && S(n～1) <>[N] }이거나, { S(len～n + 1) == [amb] || S(n～1) == [amb] }인가 ?
						else if (
							(IsExistFeature(pNextStartCountChar, STEM_DIC, S_N) == true && IsExistFeature(pThisEndCountChar, END_DIC, E_N) == true) 
							||
							(
								(IsExistFeature(pNextStartCountChar, STEM_DIC, S_N) == false && IsExistFeature(pThisEndCountChar, END_DIC, E_N) == false) 
								||IsExistFeature(pNextStartCountChar, STEM_DIC, S_Amb) == true || IsExistFeature(pThisEndCountChar, END_DIC, E_Amb) == true
							)
						)
						{
							//10y : stem = S(len～n + 1), ending = S(n～1)/*인간 + 들
							InsertStemString(pNode, pNextStartCountChar);
							SaveHTokenStem(pNode);
							InsertEndingString(pNode, pThisEndCountChar);
							SaveHTokenEnding(pNode);
						}
						else
						{
							//10n : error /*교육실시
							GoErrorFunction(pNode);
						}
					}
					else
					{
						//8n : stem = S(len～n + 1), ending = S(n～1)/*시인 + 들
						InsertStemString(pNode, pNextStartCountChar);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, pThisEndCountChar);
						SaveHTokenEnding(pNode);
					}
				}
				//7n : C(n) + S(n - 1~1)이 ending 사전에 있는가 ?
				else if (IsExistEndDic(pAddTwoString2) && strlen(pJongChar2) > 0 && strlen(pAddTwoString2) > 0 && nStart != nCount)
				{
					pThisStartCountChar = GetStartCountChar(pOriginalString, nStart); // S(len~n)
					pMinusTwoString = MinusTwoStr(pThisStartCountChar, pOneChar2, nStart); // S(len~n)-C(n)
					//8y : S(len~n) - #C(n) 이 stem 사전에 있는가 ?
					if (IsExistStemDic(pMinusTwoString)) // 8y
					{
						//9y의 로직 누락 상태임
						InsertStemString(pNode, pMinusTwoString);
						SaveHTokenStem(pNode);

						InsertEndingString(pNode, pAddTwoString2);
						if (IsExistEndDic(pAddTwoString2))
						{	
							SaveHTokenEnding(pNode);
						}
						else
						{
							MakeNewEndingFeature(pNode);
							AddEndingFeature(pNode, pJongChar2);
							AddEndingFeature(pNode, pPrevEndCountChar);

							SaveAmbstrEnding(pNode, pJongChar2);
							SaveAmbstrEnding(pNode, pPrevEndCountChar);
						}
					}
					//9n : {S(len~n) == :가는:}인가 ?
					else if (strcmp(pThisStartCountChar, "가는") == 0)// 9n
					{
						InsertStemString(pNode, pThisStartCountChar);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, m_pOldBuffer);
						SaveHTokenEnding(pNode);
					}
					//10n : C(n) = “ㄹ” → S(len~n)이 stem 사전에 있고, S(len～n) != [n]인가 ?
					else if (ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03) && IsExistNFeature(pNode->pHToken->strSource, nStart, 4) == false)//C(n)='ㄹ' -> S(len~n)이 스템 사전에 있고 s(len~n) != [N]인가?
					{
						CHAR_DELETE(pThisStartCountChar);

						pOriginalString = pNode->pHToken->strSource;
						pThisStartCountChar = GetStartCountChar(pOriginalString, nStart);
						InsertStemString(pNode, pThisStartCountChar);
						SaveHTokenStem(pNode);

						CHAR_DELETE(pAddTwoString);

						pAddTwoString = AddTwoStr("ㄴ", m_pOldBuffer);
						//11y : stem = S(len~n), ending = “ㄴ” + OldBuf()/* 만든
						InsertEndingString(pNode, pAddTwoString);
						if (IsExistEndDic(pAddTwoString))
						{
							SaveHTokenEnding(pNode);
						}
						else
						{
							MakeNewEndingFeature(pNode);
							AddEndingFeature(pNode, "ㄴ");
							SaveHTokenEndingOldFeature(pNode);

							SaveAmbstrEnding(pNode, "ㄴ");
							SaveAmbstrEnding(pNode, m_pOldBuffer);
						}
					}
					else
					{
						JasoRestoration(pNode, nStart, FORCE_REPLACE_ENDWORD, nRestoration);
						pOriginalString = pNode->pHToken->strSource;

						CHAR_DELETE(pNextStartCountChar);
						CHAR_DELETE(pThisEndCountChar);

						nCount = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_MINUS);
						pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
						pThisEndCountChar = GetEndCountChar(pOriginalString, nStart);
						//11n : C(n) = #C(n) → { S(len～n + 1) == [N] && S(n～1) == [Ppo] }인가 ?
						if (IsExistFeature(pNextStartCountChar, STEM_DIC, 4) == true && IsExistFeature(pThisEndCountChar, END_DIC, 22) == true)
						{
							InsertStemString(pNode, pNextStartCountChar);
							SaveHTokenStem(pNode);

							CHAR_DELETE(pAddTwoString);
							CHAR_DELETE(pAddTwoString2);
							CHAR_DELETE(pOneChar);

							pOneChar = GetOneChar(pOriginalString, nStart, nStart);
							pAddTwoString = AddTwoStr("이", pOneChar);
							pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
							InsertEndingString(pNode, pAddTwoString2);
							//12y : stem = S(len～n + 1), ending = “이”[Pol] + S(n) + OldBuf()
							if (IsExistEndDic(pAddTwoString2))
							{
								SaveHTokenEnding(pNode);
								AddHTokenEnding(pNode, 26);
							}
							else
							{
								MakeNewEndingFeature(pNode);
								AddEndingFeature(pNode, "이");
								AddEndingFeature(pNode, pOneChar);
								SaveHTokenEndingOldFeature(pNode);
								AddHTokenEnding(pNode, 26);

								SaveAmbstrEnding(pNode, pOneChar);
								SaveAmbstrEnding(pNode, "이");
								SaveAmbstrEnding(pNode, m_pOldBuffer);
							}
						}
						//12n : C(n) = #C(n) → { S(len～n + 1) == [N] && S(n～1) == [N] }이거나, { S(len～n + 1) <>[N] && S(n～1) <>[N] }이거나{ S(len～n + 1) == [amb] || S(n～1) == [amb] }인가 ?
						else if ((IsExistFeature(pNextStartCountChar, STEM_DIC, 4) == true && IsExistFeature(pThisEndCountChar, END_DIC, 2) == true) ||
							(IsExistFeature(pNextStartCountChar, STEM_DIC, 4) == false && IsExistFeature(pThisEndCountChar, END_DIC, 2) == false) ||
							(IsExistFeature(pNextStartCountChar, STEM_DIC, 1) == true || IsExistFeature(pThisEndCountChar, END_DIC, 0) == true))
						{
							InsertStemString(pNode, pNextStartCountChar);
							SaveHTokenStem(pNode);

							CHAR_DELETE(pAddTwoString);
							CHAR_DELETE(pOneChar);

							pOneChar = GetOneChar(pOriginalString, nStart, nStart);
							pAddTwoString = AddTwoStr(pOneChar, m_pOldBuffer);
							//13y : stem = S(len～n + 1), endi]ng = S(n) + OldBuf()
							InsertEndingString(pNode, pAddTwoString);
							if (IsExistEndDic(pAddTwoString))
							{
								SaveHTokenEnding(pNode);
							}
							else
							{
								MakeNewEndingFeature(pNode);
								AddEndingFeature(pNode, pOneChar);
								SaveHTokenEndingOldFeature(pNode);

								SaveAmbstrEnding(pNode, pOneChar);
								SaveAmbstrEnding(pNode, m_pOldBuffer);
							}
						}
						else
						{
							//13n : OldBuf() = S(n～1) → ①로
							CHAR_DELETE(m_pOldBuffer);

							nLen = strlen(pThisEndCountChar);
							nCount = 0;
							m_pOldBuffer = new char[nLen + 1];
							memset(m_pOldBuffer, 0x00, nLen + 1);
							strcpy(m_pOldBuffer, pThisEndCountChar);
							InsertArrayFeatureOld(pThisEndCountChar);
							GoMAR1_1(pNode, nStart, mAlkong, mSecondAlkong);
						}
					}
				}
				//8n : {S(len～n + 1) == [N] && S(n) == [N]}이거나, { S(len～n + 1) <>[N] && S(n) <>[N] }이거나, { S(len～n + 1) == [amb] || S(n) == [amb] }인가 ?
				else if ((IsExistFeature(pNextStartCountChar, STEM_DIC, 4) == true && IsExistFeature(pOneChar2, END_DIC, 2) == true) ||
					(IsExistFeature(pNextStartCountChar, STEM_DIC, 4) == false && IsExistFeature(pOneChar2, END_DIC, 2) == false) ||
					(IsExistFeature(pNextStartCountChar, STEM_DIC, 1) == true || IsExistFeature(pOneChar2, END_DIC, 0) == true))
					//8n
				{
					pThisStartCountChar = GetStartCountChar(pOriginalString, nStart);
					if (strcmp(pOneChar2, pLastOneChar) == 0 &&
						(strcmp(pThisStartCountChar, "주어진") == 0 || strcmp(pThisStartCountChar, "나간") == 0 ||
							strcmp(pThisStartCountChar, "자길") == 0 || strcmp(pThisStartCountChar, "옮길") == 0 ||
							strcmp(pThisStartCountChar, "옮긴") == 0 || strcmp(pThisStartCountChar, "이길") == 0 ||
							strcmp(pThisStartCountChar, "터진") == 0 || strcmp(pThisStartCountChar, "맡긴") == 0 ||
							strcmp(pThisStartCountChar, "들를") == 0 || strcmp(pThisStartCountChar, "가를") == 0 ||
							strcmp(pThisStartCountChar, "깨진") == 0 || strcmp(pThisStartCountChar, "모를") == 0 ||
							strcmp(pThisStartCountChar, "부를") == 0 || strcmp(pThisStartCountChar, "배길") == 0 ||
							strcmp(pThisStartCountChar, "내려질") == 0 || strcmp(pThisStartCountChar, "숨진") == 0 ||
							strcmp(pThisStartCountChar, "못간") == 0 || strcmp(pThisStartCountChar, "이어질") == 0 ||
							strcmp(pThisStartCountChar, "어긴") == 0 || strcmp(pThisStartCountChar, "다질") == 0 ||
							strcmp(pThisStartCountChar, "맡길") == 0 || strcmp(pThisStartCountChar, "꺼질") == 0 ||
							strcmp(pThisStartCountChar, "켜진") == 0 || strcmp(pThisStartCountChar, "남긴") == 0 ||
							strcmp(pThisStartCountChar, "알려진") == 0 || strcmp(pThisStartCountChar, "꺼진") == 0))
					{
						//10y : stem = S(len～n)-C(n), ending = C(n) + OldBuf() 
						pMinusTwoString = MinusTwoStr(pThisStartCountChar, pOneChar2, nStart);
						InsertStemString(pNode, pMinusTwoString);
						SaveHTokenStem(pNode);

						CHAR_DELETE(pAddTwoString);

						pAddTwoString = AddTwoStr(pJongChar2, m_pOldBuffer);

						InsertEndingString(pNode, pAddTwoString);
						if (IsExistEndDic(pAddTwoString))
						{
							SaveHTokenEnding(pNode);
						}
						else
						{
							MakeNewEndingFeature(pNode);
							AddEndingFeature(pNode, pJongChar2);
							SaveHTokenEndingOldFeature(pNode);

							SaveAmbstrEnding(pNode, pJongChar2);
							SaveAmbstrEnding(pNode, m_pOldBuffer);
						}
					}
					//10n : S(n) == S(1)이고, S(len～n) == “만든” || “찌든” || “힘든” || “정든” || “건든” 인가 ?
					else if (strcmp(pOneChar2, pLastOneChar) == 0 && (strcmp(pThisStartCountChar, "만든") == 0 ||
						strcmp(pThisStartCountChar, "찌든") == 0 || strcmp(pThisStartCountChar, "힘든") == 0 ||
						strcmp(pThisStartCountChar, "정든") == 0 || strcmp(pThisStartCountChar, "건든") == 0))
					{
						//11y : stem = S(len～n) - C(n) + “ㄹ”, ending = “ㄴ" 
						bool bTmp;
						bTmp = ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);
						pOriginalString = pNode->pHToken->strSource;

						CHAR_DELETE(pThisEndCountChar);

						pThisEndCountChar = GetEndCountChar(pOriginalString, 0);

						InsertStemString(pNode, pThisEndCountChar);
						SaveHTokenStem(pNode);

						InsertEndingString(pNode, "ㄴ");
						SaveHTokenEnding(pNode);
					}
					//11n : S(len～n+1) == [N] && S(n~len) == [Ppo]인가? 
					else if (IsExistFeature(pNextStartCountChar, STEM_DIC, 4) == true
						&& IsExistFeature(pThisEndCountChar, END_DIC, 22) == true)
					{
						//12y : stem = S(len～n + 1), ending = S(n~len)
						InsertStemString(pNode, pNextStartCountChar);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, pThisEndCountChar);
						SaveHTokenEnding(pNode);
						AddHTokenEnding(pNode, 26);
					}
					else
					{
						//12n : stem = S(len～n + 1), ending = S(n) + OldBuf()
						InsertStemString(pNode, pNextStartCountChar);
						SaveHTokenStem(pNode);

						CHAR_DELETE(pAddTwoString);

						pAddTwoString = AddTwoStr(pOneChar2, m_pOldBuffer);
						InsertEndingString(pNode, pAddTwoString);
						if (IsExistEndDic(pAddTwoString))
						{
							SaveHTokenEnding(pNode);
						}
						else
						{
							MakeNewEndingFeature(pNode);
							AddEndingFeature(pNode, pOneChar2);
							SaveHTokenEndingOldFeature(pNode);

							SaveAmbstrEnding(pNode, pOneChar2);
							SaveAmbstrEnding(pNode, m_pOldBuffer);
						}
					}
				}
				//9n : S(n) == S(1)인가 ?
				else if (strcmp(pOneChar2, pLastOneChar) == 0) //9n
				{
					//10y :  C(n) == “ㄴ” || “ㄹ”인가 ?
					if (strcmp(pJongChar2, "ㄴ") == 0 || strcmp(pJongChar2, "ㄹ") == 0)
					{
						pThisStartCountChar = GetStartCountChar(pOriginalString, nStart);
						pMinusTwoString = MinusTwoStr(pThisStartCountChar, pOneChar2, nStart);
						//11y : S(len~n) - C(n) 이 stem 사전에 있는가 ?
						if (IsExistStemDic(pMinusTwoString))
						{
							//12y: stem = S(len ~n) - C(n), ending = “ㄴ” || “ㄹ”
							InsertStemString(pNode, pMinusTwoString);
							SaveHTokenStem(pNode);

							if (strcmp(pJongChar2, "ㄴ") == 0)
							{
								InsertEndingString(pNode, pJongChar2);
								MakeNewEndingFeature(pNode);
								AddEndingFeature(pNode, "ㄴ");
							}
							if (strcmp(pJongChar2, "ㄹ") == 0)
							{
								InsertEndingString(pNode, pJongChar2);
								MakeNewEndingFeature(pNode);
								AddEndingFeature(pNode, "ㄹ");
							}
						}
						//12n : C(n) == “ㄴ”인가 ?
						else if (strcmp(pJongChar2, "ㄴ") == 0)
						{
							//13y : C(n) = “ㄹ”->S(len~n) 이 stem 사전에 있는가 ?
							if (ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03) && IsExistNFeature(pNode->pHToken->strSource, nStart, 4) == false)//C(n)='ㄹ' -> S(len~n)이 스템 사전에 있고 s(len~n) != [N]인가?
							{
								pOriginalString = pNode->pHToken->strSource;

								CHAR_DELETE(pThisStartCountChar);

								pThisStartCountChar = GetStartCountChar(pOriginalString, nStart);
								//14y : S(len~n)이 “예정일”인가 ?
								if (strcmp(pThisStartCountChar, "예정일") == 0)
								{
									//15y : stem = S(len ~n + 1), ending = “이"+[Pol]+“ㄴ” 
									nCount = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_MINUS);

									CHAR_DELETE(pNextStartCountChar);

									pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);

									CHAR_DELETE(pAddTwoString);
									
									pAddTwoString = AddTwoStr("이", "ㄴ");
									InsertEndingString(pNode, pAddTwoString);
									if (IsExistEndDic(pAddTwoString))
									{
										SaveHTokenEnding(pNode);
										AddHTokenEnding(pNode, 26);
									}
									else
									{
										MakeNewEndingFeature(pNode);
										AddEndingFeature(pNode, "이");
										AddEndingFeature(pNode, "ㄴ");
										AddHTokenEnding(pNode, 26);

										SaveAmbstrEnding(pNode, "이");
										SaveAmbstrEnding(pNode, "ㄴ");
									}
								}
								else
								{
									//15n : stem = S(len ~n), ending = “ㄴ” /*정든
									InsertStemString(pNode, pThisStartCountChar);
									SaveHTokenStem(pNode);
									InsertEndingString(pNode, "ㄴ");
									MakeNewEndingFeature(pNode);
									AddEndingFeature(pNode, "ㄴ");
								}
							}
							else
							{
								JasoRestoration(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA01);
								pOriginalString = pNode->pHToken->strSource;

								nCount = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_MINUS);

								CHAR_DELETE(pNextStartCountChar);
								CHAR_DELETE(pThisEndCountChar);

								pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
								pThisEndCountChar = GetEndCountChar(pOriginalString, nStart);
								//14n : C(n) = “ㄴ” →{ S(len～n + 1) == [N] && S(n～1) == [Ppo] }인가 ?
								if (IsExistFeature(pNextStartCountChar, STEM_DIC, 4) == true && IsExistFeature(pThisEndCountChar, END_DIC, 22) == true)
								{
									//15y : stem = S(len～n + 1), ending = “이”[Pol] + S(1) + OldBuf()
									InsertStemString(pNode, pNextStartCountChar);
									SaveHTokenStem(pNode);

									CHAR_DELETE(pAddTwoString);
									CHAR_DELETE(pAddTwoString2);

									pAddTwoString = AddTwoStr("이", pLastOneChar);
									pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
									InsertEndingString(pNode, pAddTwoString2);
									if (IsExistEndDic(pAddTwoString2))
									{
										SaveHTokenEnding(pNode);
										AddHTokenEnding(pNode, 26);
									}
									else
									{
										MakeNewEndingFeature(pNode);
										AddEndingFeature(pNode, pLastOneChar);
										SaveHTokenEndingOldFeature(pNode);
										AddHTokenEnding(pNode, 26);

										SaveAmbstrEnding(pNode, "이");
										SaveAmbstrEnding(pNode, pLastOneChar);
										SaveAmbstrEnding(pNode, m_pOldBuffer);
									}
								}
								//15n : C(n) = “ㄴ” →{ S(len～n + 1) == [N] && S(n～1) == [N] }이거나, { S(len～n + 1) <>[N] && S(n～1) <>[N] }이거나{ S(len～n + 1) == [amb] || S(n～1) == [amb] }인가 ?
								else if ((IsExistFeature(pNextStartCountChar, STEM_DIC, 4) == true && IsExistFeature(pThisEndCountChar, END_DIC, 2) == true) ||
									(IsExistFeature(pNextStartCountChar, STEM_DIC, 4) == false && IsExistFeature(pThisEndCountChar, END_DIC, 2) == false) ||
									(IsExistFeature(pNextStartCountChar, STEM_DIC, 4) == false && IsExistFeature(pThisEndCountChar, END_DIC, 22) == false) ||
									(IsExistFeature(pNextStartCountChar, STEM_DIC, 1) == true || IsExistFeature(pThisEndCountChar, END_DIC, 0) == true))
								{
									//16y : stem = S(len～n + 1), ending = S(1) + OldBuf()
									InsertStemString(pNode, pNextStartCountChar);
									SaveHTokenStem(pNode);

									CHAR_DELETE(pOneChar);

									pOneChar = GetOneChar(pOriginalString, strlen(pOriginalString), strlen(pOriginalString));

									CHAR_DELETE(pAddTwoString);

									pAddTwoString = AddTwoStr(pOneChar, m_pOldBuffer);
									InsertEndingString(pNode, pAddTwoString);
									if (IsExistEndDic(pAddTwoString))
									{
										SaveHTokenEnding(pNode);
									}
									else
									{
										MakeNewEndingFeature(pNode);
										AddEndingFeature(pNode, pOneChar);
										SaveHTokenEndingOldFeature(pNode);

										SaveAmbstrEnding(pNode, pOneChar);
										SaveAmbstrEnding(pNode, m_pOldBuffer);
									}
								}
								else
								{
									//16n : OldBuf() = S(n～1) → ①로
									CHAR_DELETE(m_pOldBuffer);

									nLen = strlen(pThisEndCountChar);
									nCount = 0;
									m_pOldBuffer = new char[nLen + 1];
									memset(m_pOldBuffer, 0x00, nLen + 1);
									strcpy(m_pOldBuffer, pThisEndCountChar);
									InsertArrayFeatureOld(pThisEndCountChar);

									GoMAR1_1(pNode, nStart, mAlkong, mSecondAlkong);
								}
							}
						}
						else
						{
							//13n : Stem = S(len～n + 1), ending = S(1) + OldBuf()
							InsertStemString(pNode, pNextStartCountChar);
							SaveHTokenStem(pNode);

							CHAR_DELETE(pAddTwoString);

							pAddTwoString = AddTwoStr(pLastOneChar, m_pOldBuffer);
							InsertEndingString(pNode, pAddTwoString);
							if (IsExistEndDic(pAddTwoString))
							{
								SaveHTokenEnding(pNode);
							}
							else
							{
								MakeNewEndingFeature(pNode);
								AddEndingFeature(pNode, pLastOneChar);
								SaveHTokenEndingOldFeature(pNode);

								SaveAmbstrEnding(pNode, pLastOneChar);
								SaveAmbstrEnding(pNode, m_pOldBuffer);
							}
						}
					}
					//11n : {S(len～n + 1) == [N] && S(n～1) == [Ppo]}인가 ?
					else if (strcmp(pThisEndCountChar, "시") != 0 &&
						IsExistFeature(pNextStartCountChar, STEM_DIC, 4) == true &&
						IsExistFeature(pThisEndCountChar, END_DIC, 22) == true)
					{
						InsertStemString(pNode, pNextStartCountChar);
						SaveHTokenStem(pNode);

						CHAR_DELETE(pAddTwoString);
						CHAR_DELETE(pAddTwoString2);

						pAddTwoString = AddTwoStr("이", pLastOneChar);
						pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
						InsertEndingString(pNode, pAddTwoString2);
						//12y : stem = S(len～n + 1), ending = “이"[Pol]+S(1) + OldBuf() 
						if (IsExistEndDic(pAddTwoString2))
						{
							SaveHTokenEnding(pNode);
							AddHTokenEnding(pNode, 26);
						}
						else
						{
							MakeNewEndingFeature(pNode);
							AddEndingFeature(pNode, "이");
							AddEndingFeature(pNode, pLastOneChar);
							SaveHTokenEndingOldFeature(pNode);
							AddHTokenEnding(pNode, 26);

							SaveAmbstrEnding(pNode, "이");
							SaveAmbstrEnding(pNode, pLastOneChar);
							SaveAmbstrEnding(pNode, m_pOldBuffer);
						}
					}
					//12n : {S(len～n + 1) == [N] && S(n～1) == [N]}이거나, { S(len～n + 1) <>[N] && S(n～1) <>[N] }이거나{ S(len～n + 1) == [amb] || S(n～1) == [amb] }인가 ?
					else if (IsExistFeature(pNextStartCountChar, STEM_DIC, 4) == true &&
						strcmp(pThisEndCountChar, "시") == 0)
					{
						//13y : stem = S(len～n + 1), ending = S(1) + OldBuf()
						InsertStemString(pNode, pNextStartCountChar);
						SaveHTokenStem(pNode);
						if (pOneChar != NULL)
						{
							delete[]pOneChar;
							pOneChar = NULL;
						}
						if (pAddTwoString != NULL)
						{
							delete[]pAddTwoString;
							pAddTwoString = NULL;
						}
						pOneChar = GetOneChar(pOriginalString, strlen(pOriginalString), strlen(pOriginalString));
						pAddTwoString = AddTwoStr(pOneChar, m_pOldBuffer);
						InsertEndingString(pNode, pAddTwoString);
						if (IsExistEndDic(pAddTwoString))
						{
							SaveHTokenEnding(pNode);
							DeleteHTokenEnding(pNode, 3);
							DeleteHTokenEnding(pNode, 45);
							AddHTokenEnding(pNode, 2);
						}
						else
						{
							MakeNewEndingFeature(pNode);
							AddEndingFeature(pNode, pOneChar);
							SaveHTokenEndingOldFeature(pNode);

							SaveAmbstrEnding(pNode, pOneChar);
							SaveAmbstrEnding(pNode, m_pOldBuffer);
						}
					}
					//12n : {S(len～n + 1) == [N] && S(n～1) == [N]}이거나, { S(len～n + 1) <>[N] && S(n～1) <>[N] }이거나{ S(len～n + 1) == [amb] || S(n～1) == [amb] }인가 ?
					else if ((IsExistFeature(pNextStartCountChar, STEM_DIC, 4) == true && IsExistFeature(pThisEndCountChar, END_DIC, 2) == true) ||
						(IsExistFeature(pNextStartCountChar, STEM_DIC, 4) == false && IsExistFeature(pThisEndCountChar, END_DIC, 2) == false) ||
						(IsExistFeature(pNextStartCountChar, STEM_DIC, 4) == false && IsExistFeature(pThisEndCountChar, END_DIC, 22) == false) ||
						(IsExistFeature(pNextStartCountChar, STEM_DIC, 1) == true || IsExistFeature(pThisEndCountChar, END_DIC, 0) == true))
					{
						//13y : stem = S(len～n+1), ending = S(1) + OldBuf() 
						InsertStemString(pNode, pNextStartCountChar);
						SaveHTokenStem(pNode);

						CHAR_DELETE(pOneChar);
						CHAR_DELETE(pAddTwoString);

						pOneChar = GetOneChar(pOriginalString, strlen(pOriginalString), strlen(pOriginalString));
						pAddTwoString = AddTwoStr(pOneChar, m_pOldBuffer);
						InsertEndingString(pNode, pAddTwoString);
						if (IsExistEndDic(pAddTwoString))
						{
							SaveHTokenEnding(pNode);
						}
						else
						{
							MakeNewEndingFeature(pNode);
							AddEndingFeature(pNode, pOneChar);
							SaveHTokenEndingOldFeature(pNode);

							SaveAmbstrEnding(pNode, pOneChar);
							SaveAmbstrEnding(pNode, m_pOldBuffer);
						}
					}
					else
					{
						//13n : OldBuf() = S(n～1) → ①로
						CHAR_DELETE(m_pOldBuffer);

						nLen = strlen(pThisEndCountChar);
						m_pOldBuffer = new char[nLen + 1];
						memset(m_pOldBuffer, 0x00, nLen + 1);
						strcpy(m_pOldBuffer, pThisEndCountChar);
						InsertArrayFeatureOld(pThisEndCountChar);

						GoMAR1_1(pNode, nStart, mAlkong, mSecondAlkong);
					}
				}
				//10n : {S(len～n + 1) == [N] && S(n～1) == [Ppo]}인가 ?
				else if (IsExistFeature(pNextStartCountChar, STEM_DIC, 4) == true && IsExistFeature(pThisEndCountChar, END_DIC, 22) == true)
				{
					//11y : stem = S(len～n + 1), ending = “이"[Pol]+S(n~1) 
					InsertStemString(pNode, pNextStartCountChar);
					SaveHTokenStem(pNode);

					CHAR_DELETE(pAddTwoString);

					pAddTwoString = AddTwoStr("이", pThisEndCountChar);
					InsertEndingString(pNode, pAddTwoString);
					if (IsExistEndDic(pAddTwoString)) //11y
					{
						SaveHTokenEnding(pNode);
						AddHTokenEnding(pNode, 26);
					}
					else
					{
						MakeNewEndingFeature(pNode);
						AddEndingFeature(pNode, "이");
						AddEndingFeature(pNode, pThisEndCountChar);
						AddHTokenEnding(pNode, 26);

						SaveAmbstrEnding(pNode, "이");
						SaveAmbstrEnding(pNode, pThisEndCountChar);
					}
				}
				//11n : {S(len～n + 1) == [N] && S(n～1) == [N]}이거나, { S(len～n + 1) <>[N] && S(n～1) <>[N] }이거나{ S(len～n + 1) == [amb] || S(n～1) == [amb] }인가 ?
				else if ((IsExistFeature(pNextStartCountChar, STEM_DIC, 4) == true && IsExistFeature(pThisEndCountChar, END_DIC, 2) == true) ||
					(IsExistFeature(pNextStartCountChar, STEM_DIC, 4) == false && IsExistFeature(pThisEndCountChar, END_DIC, 2) == false) ||
					(IsExistFeature(pNextStartCountChar, STEM_DIC, 4) == true && IsExistFeature(pThisEndCountChar, END_DIC, 22) == true) ||
					(IsExistFeature(pNextStartCountChar, STEM_DIC, 1) == true || IsExistFeature(pThisEndCountChar, END_DIC, 0) == true))
					//11n
				{
					//12y : stem = S(len～n + 1), ending = S(n~1)
					InsertStemString(pNode, pNextStartCountChar);
					SaveHTokenStem(pNode);

					CHAR_DELETE(pAddTwoString);
					CHAR_DELETE(pAddTwoString2);

					pAddTwoString = AddTwoStr(pOneChar2, m_pNewBuffer);
					pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);

					InsertEndingString(pNode, pAddTwoString2);
					if (IsExistEndDic(pAddTwoString2))
					{
						SaveHTokenEnding(pNode);
					}
					else
					{
						MakeNewEndingFeature(pNode);
						AddEndingFeature(pNode, pOneChar2);
						SaveHTokenEndingOldFeature(pNode);
						SaveHTokenEndingNewFeature(pNode);

						SaveAmbstrEnding(pNode, pOneChar2);
						SaveAmbstrEnding(pNode, m_pOldBuffer);
						SaveAmbstrEnding(pNode, m_pNewBuffer);
					}
				}
				else //12n
				{
					//12n : Error(20030128 유혜원 수정 “전국지도”)
					GoErrorFunction(pNode);
				}
			}
			//5n : S(n) == S(1)인가 ?
			else if (strcmp(pOneChar, pLastOneChar) == 0)
			
			{
				CHAR_DELETE(pJongChar);

				pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);
				//6y :  C(n) == “ㄴ” || “ㄹ”인가 ?
				if (strcmp(pJongChar, "ㄴ") == 0 || strcmp(pJongChar, "ㄹ") == 0)
				{
					pThisStartCountChar = GetStartCountChar(pOriginalString, nStart);
					pMinusTwoString = MinusTwoStr(pThisStartCountChar, pOneChar, nStart);
					//7y : S(len~n) - C(n) 이 stem 사전에 있는가 ?
					if (IsExistStemDic(pMinusTwoString))
					{
						//8y: stem = S(len ~n) - C(n), ending = “ㄴ” || “ㄹ” /*어설픈,늦어진
						InsertStemString(pNode, pMinusTwoString);
						SaveHTokenStem(pNode);

						InsertEndingString(pNode, pJongChar);
						MakeNewEndingFeature(pNode);
						AddEndingFeature(pNode, pJongChar);
					}
					//8n : C(n) == “ㄴ”인가 ?
					else if (strcmp(pJongChar, "ㄴ") == 0)
					{
						//9y : C(n) = “ㄹ”->S(len~n) 이 stem 사전에 있는가 ?
						if (ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03))//C(n)='ㄹ' -> S(len~n)이 스템 사전에 있나?
						{
							//10y : stem = S(len ~n), ending = “ㄴ” /*정든
							pOriginalString = pNode->pHToken->strSource;

							CHAR_DELETE(pThisStartCountChar);

							pThisStartCountChar = GetStartCountChar(pOriginalString, nStart);
							InsertStemString(pNode, pThisStartCountChar);
							SaveHTokenStem(pNode);
							InsertEndingString(pNode, "ㄴ");
							MakeNewEndingFeature(pNode);
							AddEndingFeature(pNode, "ㄴ");
						}
						else
						{
							//10n : C(n) = “ㄴ”->OldBuf() = S(n～1) → ①로
							ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA01);
							pOriginalString = pNode->pHToken->strSource;

							CHAR_DELETE(pThisEndCountChar);

							//S(n~1)
							pThisEndCountChar = GetEndCountChar(pOriginalString, nStart);

							CHAR_DELETE(m_pOldBuffer);

							nLen = strlen(pThisEndCountChar);
							m_pOldBuffer = new char[nLen + 1];
							memset(m_pOldBuffer, 0x00, nLen + 1);
							strcpy(m_pOldBuffer, pThisEndCountChar);
							InsertArrayFeatureOld(pThisEndCountChar);

							GoMAR1_1(pNode, nStart, mAlkong, mSecondAlkong);
						}
					}
					else
					{
						//9n : OldBuf() = S(n - 1) → ①로
						CHAR_DELETE(m_pOldBuffer);

						nLen = strlen(pThisEndCountChar);
						m_pOldBuffer = new char[nLen + 1];
						memset(m_pOldBuffer, 0x00, nLen + 1);
						strcpy(m_pOldBuffer, pThisEndCountChar);
						InsertArrayFeatureOld(pThisEndCountChar);

						GoMAR1_1(pNode, nStart, mAlkong, mSecondAlkong);
					}
				}
				//7n : OldBuf()가 S(1)가 같지 않은가 ? (OldBuf() != S(1))
				else if (m_pOldBuffer != NULL && strlen(m_pOldBuffer) > 0)
				{
					if (strcmp(m_pOldBuffer, pLastOneChar) != 0)
					{
						//8y : OldBuf() = S(1) + OldBuf()→ ①로/*그녀에겐
						CHAR_DELETE(pAddTwoString);

						pAddTwoString = AddTwoStr(pLastOneChar, m_pOldBuffer);
						InsertArrayFeatureOld(pLastOneChar);

						CHAR_DELETE(m_pOldBuffer);

						nLen = strlen(pAddTwoString);
						m_pOldBuffer = new char[nLen + 1];
						memset(m_pOldBuffer, 0x00, nLen + 1);
						strcpy(m_pOldBuffer, pAddTwoString);

						GoMAR1_1(pNode, nStart, mAlkong, mSecondAlkong);
					}
					else
					{
						//8n : OldBuf() = S(n～1) → ①로
						CHAR_DELETE(m_pOldBuffer);

						nLen = strlen(pThisEndCountChar);
						m_pOldBuffer = new char[nLen + 1];
						memset(m_pOldBuffer, 0x00, nLen + 1);
						strcpy(m_pOldBuffer, pThisEndCountChar);
						InsertArrayFeatureOld(pThisEndCountChar);

						GoMAR1_1(pNode, nStart, mAlkong, mSecondAlkong);
					}
				}
				else
				{
					CHAR_DELETE(m_pOldBuffer);

					nLen = strlen(pThisEndCountChar);
					m_pOldBuffer = new char[nLen + 1];
					memset(m_pOldBuffer, 0x00, nLen + 1);
					strcpy(m_pOldBuffer, pThisEndCountChar);
					InsertArrayFeatureOld(pThisEndCountChar);

					GoMAR1_1(pNode, nStart, mAlkong, mSecondAlkong);
				}
			}
			else
			{
				pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);
				nCount = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_PLUS);
				pPrevEndCountChar = GetEndCountChar(pOriginalString, nCount);
				pAddTwoString = AddTwoStr(pJongChar, pPrevEndCountChar);
				//6n : C(n) + S(n - 1～1)이 ending사전에 있는가 ?
				if (IsExistEndDic(pAddTwoString))
				{
					pThisStartCountChar = GetStartCountChar(pOriginalString, nStart);
					pMinusTwoString = MinusTwoStr(pThisStartCountChar, pOneChar, nStart);
					//7y : S(len～n) - C(n)이 stem사전에 있는가 ?
					if (IsExistStemDic(pMinusTwoString))
					{
						//8y : {(S(len～n) - C(n)) == [N] && C(n) + S(n - 1～1) == [N]}이거나, { (S(len～n) - C(n)) <>[N] && C(n) + S(n - 1～1) <>[N] }이거나{ (S(len～n) - C(n)) == [amb] || C(n) + S(n - 1～1) == [amb] }인가 ?
						if ((IsExistFeature(pMinusTwoString, STEM_DIC, 4) == true && IsExistFeature(pAddTwoString, END_DIC, 2) == true) ||
							(IsExistFeature(pMinusTwoString, STEM_DIC, 4) == false && IsExistFeature(pAddTwoString, END_DIC, 2) == false) ||
							(IsExistFeature(pMinusTwoString, STEM_DIC, 1) == true || IsExistFeature(pAddTwoString, END_DIC, 0) == true))
						{
							//9y : Stem = S(len～n) - C(n), ending = C(n) + S(n - 1～1)
							InsertStemString(pNode, pMinusTwoString);
							SaveHTokenStem(pNode);

							InsertEndingString(pNode, pAddTwoString);
							if (IsExistEndDic(pAddTwoString))
							{
								SaveHTokenEnding(pNode);
							}
							else
							{
								MakeNewEndingFeature(pNode);
								AddEndingFeature(pNode, pJongChar);
								AddEndingFeature(pNode, pPrevEndCountChar);

								SaveAmbstrEnding(pNode, pJongChar);
								SaveAmbstrEnding(pNode, pPrevEndCountChar);

							}
						}
						else
						{
							//9n : OldBuf = S(n～1)  ①로
							CHAR_DELETE(m_pOldBuffer);

							nLen = strlen(pThisEndCountChar);
							m_pOldBuffer = new char[nLen + 1];
							memset(m_pOldBuffer, 0x00, nLen + 1);
							strcpy(m_pOldBuffer, pThisEndCountChar);
							InsertArrayFeatureOld(pThisEndCountChar);

							GoMAR1_1(pNode, nStart, mAlkong, mSecondAlkong);
						}
					}
					else
					{
						CHAR_DELETE(pAddTwoString);
						pAddTwoString = AddTwoStr(pOneChar, m_pOldBuffer);
						CHAR_DELETE(m_pOldBuffer);

						nLen = strlen(pAddTwoString);
						m_pOldBuffer = new char[nLen + 1];
						memset(m_pOldBuffer, 0x00, nLen + 1);
						strcpy(m_pOldBuffer, pAddTwoString);
						InsertArrayFeatureOld(pAddTwoString);

						GoMAR1_1(pNode, nStart, mAlkong, mSecondAlkong);
					}
				}
				else
				{
					//7n : S(n~1)이[Prs]인가 ? : 어미사전
					if (IsExistFeature(pThisEndCountChar, END_DIC, E_Prs) == true)
					{
						//8y : S(n + 1) == “으” || “시” || “이” || “하” || “되” || “러” || “우”인가 ?
						if (strcmp(pNextStartCountChar, "으") == 0 ||
							strcmp(pNextStartCountChar, "시") == 0 ||
							strcmp(pNextStartCountChar, "이") == 0 ||
							strcmp(pNextStartCountChar, "하") == 0 ||
							strcmp(pNextStartCountChar, "되") == 0 ||
							strcmp(pNextStartCountChar, "러") == 0 ||
							strcmp(pNextStartCountChar, "우") == 0)
						{
							//9y : OldBuf = S(n～1)  ①로
							CHAR_DELETE(m_pOldBuffer);

							nLen = strlen(pThisEndCountChar);
							m_pOldBuffer = new char[nLen + 1];
							memset(m_pOldBuffer, 0x00, nLen + 1);
							strcpy(m_pOldBuffer, pThisEndCountChar);
							InsertArrayFeatureOld(pThisEndCountChar);

							GoMAR1_1(pNode, nStart, mAlkong, mSecondAlkong);
						}
						else
						{
							//9n : Error(도예강사, 피부관리사)
							GoErrorFunction(pNode);
						}
					}
					else
					{
						//8n : OldBuf = S(n～1)  ①로 / *사람들은 = 사람들 + 은
						CHAR_DELETE(m_pOldBuffer);

						nLen = strlen(pThisEndCountChar);
						m_pOldBuffer = new char[nLen + 1];
						memset(m_pOldBuffer, 0x00, nLen + 1);
						strcpy(m_pOldBuffer, pThisEndCountChar);
						InsertArrayFeatureOld(pThisEndCountChar);

						GoMAR1_1(pNode, nStart, mAlkong, mSecondAlkong);
					}
				}
			}
		}
		else if (IsExistEndDic(pNextEndCountChar))
		{
			InsertArrayFeatureOld(pOneChar);

			CHAR_DELETE(pAddTwoString);

			pAddTwoString = AddTwoStr(pOneChar, m_pOldBuffer);

			CHAR_DELETE(m_pOldBuffer);

			nLen = strlen(pAddTwoString);
			m_pOldBuffer = new char[nLen + 1];
			memset(m_pOldBuffer, 0x00, nLen + 1);
			strcpy(m_pOldBuffer, pAddTwoString);
			InsertArrayFeatureOld(pAddTwoString);

			GoMAR1_1(pNode, nStart, mAlkong, mSecondAlkong);
		}
		//4n : S(n + 1～1)이 ending 사전에 있는가 ?
		else
		{
			CHAR_DELETE(pOneChar);

			pOneChar = GetOneChar(pOriginalString, nCount, nCount);
			pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);
			pAddTwoString = AddTwoStr(pJongChar, pThisEndCountChar);

			nCount = ReturnNextPrevCount(pOriginalString, nStart, 2, CALCULATE_MINUS);

			pOneChar3 = GetOneChar(pOriginalString, nCount, nCount);
			pJongChar3 = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);
			pAddTwoString3 = AddTwoStr(pJongChar, pNextEndCountChar);
			pOneChar2 = GetOneChar(pOriginalString, nStart, nStart);
			pJongChar2 = GetChojungJongsungChar(pOneChar2, CHAR_JONGSUNG);
			nCount = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_PLUS);
			pPrevEndCountChar = GetEndCountChar(pOriginalString, nCount);
			pAddTwoString2 = AddTwoStr(pJongChar2, pPrevEndCountChar);

			CHAR_DELETE(pOneChar);

			pOneChar = GetOneChar(pOriginalString, nStart, nStart);
			//4n : S(n + 1～1)이 ending 사전에 있는가 ?
			if (IsExistEndDic(pAddTwoString) && strlen(pJongChar) >0)
			{
				//5y : OldBuf() = S(n) + OldBuf() → ①로
				InsertArrayFeatureOld(pOneChar);

				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr(pOneChar, m_pOldBuffer);

				CHAR_DELETE(m_pOldBuffer);

				nLen = strlen(pAddTwoString);
				m_pOldBuffer = new char[nLen + 1];
				memset(m_pOldBuffer, 0x00, nLen + 1);
				strcpy(m_pOldBuffer, pAddTwoString);
				InsertArrayFeatureOld(pAddTwoString);

				GoMAR1_1(pNode, nStart, mAlkong, mSecondAlkong);
			}
			//5n : C(n + 1) + S(n～1)이 ending 사전에 있는가 ?
			else if (IsExistEndDic(pAddTwoString3) && strlen(pJongChar3) > 0 && nCount > 0)
			{
				//6y : OldBuf() = S(n) + OldBuf() → ①로
				CHAR_DELETE(pAddTwoString3);

				pAddTwoString3 = AddTwoStr(pOneChar, m_pOldBuffer);

				CHAR_DELETE(m_pOldBuffer);

				nLen = strlen(pAddTwoString3);
				nCount = 0;
				m_pOldBuffer = new char[nLen + 1];
				memset(m_pOldBuffer, 0x00, nLen + 1);
				strcpy(m_pOldBuffer, pAddTwoString3);
				InsertArrayFeatureOld(pAddTwoString3);

				GoMAR1_1(pNode, nStart, mAlkong, mSecondAlkong);
			}
			//6n : C(n + 2) + S(n + 1～1)이 ending 사전에 있는가 ?
			else if (IsExistEndDic(pAddTwoString2) && strlen(pJongChar2) > 0 && strlen(pOriginalString) + 1 > (unsigned)nCount)
			{
				pThisStartCountChar = GetStartCountChar(pOriginalString, nStart);
				pMinusTwoString = MinusTwoStr(pThisStartCountChar, pOneChar, nStart);
				//7y : OldBuf() = S(n) + OldBuf() → ①로 /*배울수록(test)
				if (IsExistStemDic(pMinusTwoString))
				{
					InsertStemString(pNode, pMinusTwoString);
					SaveHTokenStem(pNode);

					CHAR_DELETE(pJongChar);
					CHAR_DELETE(pAddTwoString);

					pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);
					pAddTwoString = AddTwoStr(pJongChar, m_pOldBuffer);

					InsertEndingString(pNode, pAddTwoString);
					if (IsExistEndDic(pAddTwoString))
					{
						SaveHTokenEnding(pNode);
					}
					else
					{
						MakeNewEndingFeature(pNode);
						AddEndingFeature(pNode, pJongChar);
						SaveHTokenEndingOldFeature(pNode);

						SaveAmbstrEnding(pNode, pJongChar);
						SaveAmbstrEnding(pNode, m_pOldBuffer);
					}
				}
				else
				{
					nLen = strlen(pAddTwoString2);
					nCount = 0;

					CHAR_DELETE(m_pOldBuffer);

					m_pOldBuffer = new char[nLen + 1];
					memset(m_pOldBuffer, 0x00, nLen + 1);
					strcpy(m_pOldBuffer, pAddTwoString2);
					InsertArrayFeatureOld(pAddTwoString2);
					ReplaceHangulJongsungNull(pNode, nStart);

					GoMAR2(pNode, nStart, mAlkong, mSecondAlkong);
				}
			}
			//7n : C(n) + S(n - 1~1)이 ending 사전에 있는가 ?
			else if (strcmp(pOneChar, pLastOneChar) == 0)
			{
				CHAR_DELETE(pJongChar);

				pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);
				pChoJungChar = GetChojungJongsungChar(pOneChar, CHAR_CHOJUNGSUNG);
				if (strcmp(pJongChar, "ㄴ") == 0)
				{
					pThisStartCountChar = GetStartCountChar(pOriginalString, nStart);
					pMinusTwoString = MinusTwoStr(pThisStartCountChar, pOneChar, nStart);
					if (IsExistStemDic(pMinusTwoString))
					{
						InsertStemString(pNode, pMinusTwoString);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, "ㄴ");
						MakeNewEndingFeature(pNode);
						AddEndingFeature(pNode, "ㄴ");
					}
					else if (strcmp(pChoJungChar, "하") == 0 || strcmp(pChoJungChar, "되") == 0)
					{
						CHAR_DELETE(m_pOldBuffer);

						m_pOldBuffer = new char[3];
						memset(m_pOldBuffer, 0x00, 3);
						strcpy(m_pOldBuffer, "ㄴ");
						InsertArrayFeatureOld("ㄴ");
						ReplaceHangulJongsungNull(pNode, nStart);

						GoIMR1(pNode, nStart, mAlkong, mSecondAlkong);
					}
					else if (ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03))
					{
						pOriginalString = pNode->pHToken->strSource;

						CHAR_DELETE(pThisStartCountChar);

						pThisStartCountChar = GetStartCountChar(pOriginalString, nStart);
						InsertStemString(pNode, pThisStartCountChar);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, "ㄴ");
						MakeNewEndingFeature(pNode);
						AddEndingFeature(pNode, "ㄴ");
					}
					else
					{
						CHAR_DELETE(m_pOldBuffer);

						m_pOldBuffer = new char[3];
						memset(m_pOldBuffer, 0x00, 3);
						strcpy(m_pOldBuffer, "ㄴ");

						InsertArrayFeatureOld("ㄴ");
						ReplaceHangulJongsungNull(pNode, nStart);

						GoMAR1_2(pNode, nStart, mAlkong, mSecondAlkong);
					}
				}
				else if (strcmp(pJongChar, "ㄹ") == 0)
				{
					pThisStartCountChar = GetStartCountChar(pOriginalString, nStart);
					pMinusTwoString = MinusTwoStr(pThisStartCountChar, pOneChar, nStart);
					if (IsExistStemDic(pMinusTwoString))
					{
						InsertStemString(pNode, pMinusTwoString);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, "ㄹ");
						MakeNewEndingFeature(pNode);
						AddEndingFeature(pNode, "ㄹ");
					}
					else
					{
						InsertArrayFeatureOld(pOneChar);

						CHAR_DELETE(pAddTwoString);

						pAddTwoString = AddTwoStr(pOneChar, m_pOldBuffer);

						CHAR_DELETE(m_pOldBuffer);

						nLen = strlen(pAddTwoString);
						nCount = 0;
						m_pOldBuffer = new char[nLen + 1];
						memset(m_pOldBuffer, 0x00, nLen + 1);
						strcpy(m_pOldBuffer, pAddTwoString);

						GoMAR1_1(pNode, nStart, mAlkong, mSecondAlkong);
					}
				}
				else
				{
					InsertArrayFeatureOld(pOneChar);

					CHAR_DELETE(pAddTwoString);

					pAddTwoString = AddTwoStr(pOneChar, m_pOldBuffer);

					CHAR_DELETE(m_pOldBuffer);

					nLen = strlen(pAddTwoString);
					nCount = 0;
					m_pOldBuffer = new char[nLen + 1];
					memset(m_pOldBuffer, 0x00, nLen + 1);
					strcpy(m_pOldBuffer, pAddTwoString);

					GoMAR1_1(pNode, nStart, mAlkong, mSecondAlkong);
				}
			}
			else
			{
				mAlkong = ReturnNextPrevCount(pOriginalString, mAlkong, 1, CALCULATE_PLUS);
				GoMAR2(pNode, nStart, mAlkong, mSecondAlkong);
			}
		}
	}
	else
	{
		GoMAR1_3(pNode, nStart, mAlkong, mSecondAlkong);
	}

	CHAR_DELETE(pThisStartCountChar);
	CHAR_DELETE(pNextStartCountChar);
	CHAR_DELETE(pNextEndCountChar);
	CHAR_DELETE(pPrevEndCountChar);
	CHAR_DELETE(pThisEndCountChar);
	CHAR_DELETE(pLastOneChar);
	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pOneChar2);
	CHAR_DELETE(pOneChar3);
	CHAR_DELETE(pJongChar);
	CHAR_DELETE(pJongChar2);
	CHAR_DELETE(pJongChar3);
	CHAR_DELETE(pOldJongChar);
	CHAR_DELETE(pChoJungChar);
	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pAddTwoString2);
	CHAR_DELETE(pAddTwoString3);
	CHAR_DELETE(pMinusTwoString);
}

void CNAHangulAnaly::GoMAR1_3(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong)
{
	char* pOriginalString = NULL;

	char* pThisStartCountChar = NULL;
	char* pOneChar = NULL;
	char* pJongChar = NULL;
	char* pChoJungChar = NULL;
	char* pMinusTwoString = NULL;

	int nLen = 0;
	int nTotalCount = 0;
	int nPos = 0;

	pOriginalString = pNode->pHToken->strSource;
	nTotalCount = strlen(pOriginalString);

	if (nTotalCount - nStart >= 0 && nTotalCount - nStart <= 1) //3n
	{
		pOneChar = GetOneChar(pOriginalString, nStart, nStart);
		pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);
		pChoJungChar = GetChojungJongsungChar(pOneChar, CHAR_CHOJUNGSUNG);
		if (strcmp(pJongChar, "ㄴ") == 0) //4y
		{
			pThisStartCountChar = GetStartCountChar(pOriginalString, nStart);
			pMinusTwoString = MinusTwoStr(pThisStartCountChar, pOneChar, nStart);
			if (IsExistStemDic(pMinusTwoString))
			{
				InsertStemString(pNode, pMinusTwoString);
				SaveHTokenStem(pNode);
				InsertEndingString(pNode, pJongChar);
				MakeNewEndingFeature(pNode);
				AddEndingFeature(pNode, pJongChar);
			}
			else if (ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03))//C(n)='ㄹ' -> S(len~n)이 스템 사전에 있나?
			{
				pOriginalString = pNode->pHToken->strSource;

				CHAR_DELETE(pThisStartCountChar);

				pThisStartCountChar = GetStartCountChar(pOriginalString, nStart);
				if (strcmp(pThisStartCountChar, "짝수일") == 0 || strcmp(pThisStartCountChar, "홀수일") == 0 ||
					strcmp(pThisStartCountChar, "예정일") == 0)
				{
					nPos = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_MINUS);
					CHAR_DELETE(pThisStartCountChar);
					pThisStartCountChar = GetStartCountChar(pOriginalString, nPos);

					InsertStemString(pNode, pThisStartCountChar);
					SaveHTokenStem(pNode);

					InsertEndingString(pNode, "이ㄴ");
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "ㄴ");
					AddHTokenEnding(pNode, E_Pol);
				}
				else
				{
					InsertStemString(pNode, pThisStartCountChar);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, "ㄴ");
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "ㄴ");
				}
			}
			else if (IsExistStrReplaceJaso(pNode, nStart, REPLACE_HANGUL_JA01, "으,시,이,하,되,러,우,키,기"))//C(n)='ㄴ'->S(n)-C(n)
			{
				CHAR_DELETE(m_pOldBuffer);

				m_pOldBuffer = new char[3];
				memset(m_pOldBuffer, 0x00, 3);
				strcpy(m_pOldBuffer, "ㄴ");
				InsertArrayFeatureOld("ㄴ");
				ReplaceHangulJongsungNull(pNode, nStart);
				if (strstr("이,하,되,러,키,우", pChoJungChar) != NULL)
				{
					GoIMR1(pNode, nStart, mAlkong, mSecondAlkong);
				}
				else if (strstr("으,시", pChoJungChar) != NULL)
				{
					GoMAR1_2(pNode, nStart, mAlkong, mSecondAlkong);
				}
			}
			else
			{
				GoErrorFunction(pNode);
			}
		}
		else if (strcmp(pOneChar, "술") != 0 && strcmp(pOneChar, "발") != 0 &&
			strcmp(pJongChar, "ㄹ") == 0) //5n
		{
			pThisStartCountChar = GetStartCountChar(pOriginalString, nStart);
			pMinusTwoString = MinusTwoStr(pThisStartCountChar, pOneChar, nStart);
			if (IsExistStemDic(pMinusTwoString)) //6n
			{
				InsertStemString(pNode, pMinusTwoString);
				SaveHTokenStem(pNode);
				InsertEndingString(pNode, "ㄹ");
				MakeNewEndingFeature(pNode);
				AddEndingFeature(pNode, "ㄹ");
			}
			else if (IsExistStemDic(pThisStartCountChar)) //7n
			{
				InsertStemString(pNode, pThisStartCountChar);
				SaveHTokenStem(pNode);
				InsertEndingString(pNode, "ㄹ");
				MakeNewEndingFeature(pNode);
				AddEndingFeature(pNode, "ㄹ");
			}
			else if (IsExistStemDic(pMinusTwoString) == false &&
				strstr("이,으,시,하,되,키", pChoJungChar) == NULL)
			{
				GoErrorFunction(pNode);
			}
			else if (CompareVariousTable01(pChoJungChar, 0, false) == true ||
				CompareVariousTable02(pChoJungChar, 1, false) == true ||
				CompareVariousTable02(pChoJungChar, 2, false) == true ||
				CompareVariousTable03(pChoJungChar, 0, false) == true ||
				CompareVariousTable03(pChoJungChar, 1, false) == true ||
				CompareVariousTable04(pChoJungChar, 1, false) == true ||
				CompareVariousTable04(pChoJungChar, 2, false) == true ||
				CompareVariousTable04(pChoJungChar, 3, false) == true ||
				CompareVariousTable05(pChoJungChar, ALL_TABLE_05, true) == true ||
				CompareVariousTable06(pChoJungChar, ALL_TABLE_06, true) == true ||
				CompareVariousTable07(pChoJungChar, ALL_TABLE_07, true) == true ||
				strcmp(pChoJungChar, "기") == 0)
				//8n
			{
				CHAR_DELETE(m_pOldBuffer);

				m_pOldBuffer = new char[3];
				memset(m_pOldBuffer, 0x00, 3);
				strcpy(m_pOldBuffer, "ㄹ");
				InsertArrayFeatureOld("ㄹ");
				ReplaceHangulJongsungNull(pNode, nStart);

				GoIMR1(pNode, nStart, mAlkong, mSecondAlkong);
			}
			else
			{
				CHAR_DELETE(m_pOldBuffer);

				nLen = strlen(pJongChar);
				m_pOldBuffer = new char[nLen + 1];
				memset(m_pOldBuffer, 0x00, nLen + 1);
				strcpy(m_pOldBuffer, pJongChar);
				InsertArrayFeatureOld(pJongChar);
				ReplaceHangulJongsungNull(pNode, nStart);

				GoMAR1_2(pNode, nStart, mAlkong, mSecondAlkong);
			}
		}
		else if (strcmp(pOneChar, "술") == 0 || strcmp(pOneChar, "발") == 0)
		{
			GoErrorFunction(pNode);
		}
		else if (IsExistEndDic(pJongChar))
		{
			pThisStartCountChar = GetStartCountChar(pOriginalString, nStart);
			pMinusTwoString = MinusTwoStr(pThisStartCountChar, pOneChar, nStart);
			if (IsExistStemDic(pMinusTwoString))
			{
				InsertStemString(pNode, pMinusTwoString);
				SaveHTokenStem(pNode);
				InsertEndingString(pNode, pJongChar);
				MakeNewEndingFeature(pNode);
				AddEndingFeature(pNode, pJongChar);
			}
			else if (CompareVariousTable01(pChoJungChar, 0, false) == true ||
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
				ReplaceHangulJongsungNull(pNode, nStart);

				GoIMR1(pNode, nStart, mAlkong, mSecondAlkong);
			}
			else
			{
				CHAR_DELETE(m_pOldBuffer);

				nLen = strlen(pJongChar);
				m_pOldBuffer = new char[nLen + 1];
				memset(m_pOldBuffer, 0x00, nLen + 1);
				strcpy(m_pOldBuffer, pJongChar);
				InsertArrayFeatureOld(pJongChar);
				ReplaceHangulJongsungNull(pNode, nStart);

				GoMAR1_2(pNode, nStart, mAlkong, mSecondAlkong);
			}
		}
		else
		{
			GoErrorFunction(pNode);
		}
	}
	else
	{
		GoMAR1_4(pNode, nStart, mAlkong, mSecondAlkong);
	}

	CHAR_DELETE(pThisStartCountChar);
	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pJongChar);
	CHAR_DELETE(pChoJungChar);
	CHAR_DELETE(pMinusTwoString);
}

void CNAHangulAnaly::GoMAR1_4(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong)
{
	char* pOriginalString = NULL;

	char* pThisStartCountChar = NULL;
	char* pPrevEndCountChar = NULL;
	char* pOneChar = NULL;
	char* pJongChar = NULL;

	int nLen = 0;
	int nCount = 0;

	pOriginalString = pNode->pHToken->strSource;
	pOneChar = GetOneChar(pOriginalString, nStart, nStart);
	pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);

	if (strcmp(pJongChar, "") == 0) //4n
	{
		//5y
		if (ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03))
		{
			pOriginalString = pNode->pHToken->strSource;
			nCount = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_PLUS);
			pThisStartCountChar = GetStartCountChar(pOriginalString, nStart);
			pPrevEndCountChar = GetEndCountChar(pOriginalString, nCount);

			if (IsExistFeature(pThisStartCountChar, STEM_DIC, 4) == false && IsExistFeature(pPrevEndCountChar, END_DIC, 2) == false)//)
			{
				if (IsExistFeature(pThisStartCountChar, STEM_DIC, 1) == true || IsExistFeature(pPrevEndCountChar, END_DIC, 0) == true)
				{
					if (IsExistFeature(pThisStartCountChar, STEM_DIC, 4) == true && IsExistFeature(pPrevEndCountChar, END_DIC, E_Ppo) == false)
					{
						GoErrorFunction(pNode);
					}
					else
					{
						InsertStemString(pNode, pThisStartCountChar);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, m_pOldBuffer);
						MakeNewEndingFeature(pNode);
						SaveHTokenEndingOldFeature(pNode);
					}
				}
				else
				{
					InsertStemString(pNode, pThisStartCountChar);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, m_pOldBuffer);
					MakeNewEndingFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
				}
			}
			else
			{
				ReplaceHangulJongsungNull(pNode, nStart);

				GoMAR2(pNode, nStart, mAlkong, mSecondAlkong);
			}
		}
		else
		{
			ReplaceHangulJongsungNull(pNode, nStart);

			GoMAR2(pNode, nStart, mAlkong, mSecondAlkong);
		}
	}
	else
	{
		GoMAR1_5(pNode, nStart, mAlkong, mSecondAlkong);
	}

	CHAR_DELETE(pThisStartCountChar);
	CHAR_DELETE(pPrevEndCountChar);
	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pJongChar);
}

void CNAHangulAnaly::GoMAR1_5(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong)
{
	char* pOriginalString = NULL;

	char* pThisStartCountChar = NULL;
	char* pNextStartCountChar = NULL;
	char* pPrevEndCountChar = NULL;
	char* pThisEndCountChar = NULL;
	char* pOneChar = NULL;
	char* pJongChar = NULL;
	char* pOriginJongChar = NULL;
	char* pOldJongChar = NULL;
	char* pChoJungChar = NULL;
	char* pAddTwoString = NULL;
	char* pMinusTwoString = NULL;

	int nLen = 0;
	int nCount = 0;
	int nRestoration = -1;

	pOriginalString = pNode->pHToken->strSource;

	nCount = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_PLUS);
	pOneChar = GetOneChar(pOriginalString, nStart, nStart);
	pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);
	pOldJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);
	nRestoration = Restoration(pJongChar);

	pPrevEndCountChar = GetEndCountChar(pOriginalString, nCount);
	pAddTwoString = AddTwoStr(pJongChar, pPrevEndCountChar);

	if (IsExistEndDic(pAddTwoString))
	{
		pThisStartCountChar = GetStartCountChar(pOriginalString, nStart);
		pMinusTwoString = MinusTwoStr(pThisStartCountChar, pOneChar, nStart);
		if (IsExistStemDic(pMinusTwoString))
		{
			if (IsExistFeature(pMinusTwoString, STEM_DIC, 4) == true && IsExistFeature(pAddTwoString, END_DIC, 22) == true)
			{
				InsertStemString(pNode, pMinusTwoString);
				SaveHTokenStem(pNode);

				CHAR_DELETE(pMinusTwoString);

				pMinusTwoString = AddTwoStr("이", pAddTwoString);
				InsertEndingString(pNode, pMinusTwoString);
				if (IsExistEndDic(pMinusTwoString))
				{
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, 26);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "이");
					AddEndingFeature(pNode, pAddTwoString);
					AddHTokenEnding(pNode, 26);

					SaveAmbstrEnding(pNode, "이");
					SaveAmbstrEnding(pNode, pAddTwoString);
				}
			}
			else if (
				(IsExistFeature(pMinusTwoString, STEM_DIC, 4) == true && IsExistFeature(pAddTwoString, END_DIC, 2) == true) ||
				(IsExistFeature(pMinusTwoString, STEM_DIC, 4) == false && IsExistFeature(pAddTwoString, END_DIC, 2) == false) ||
				(IsExistFeature(pMinusTwoString, STEM_DIC, 1) == true || IsExistFeature(pAddTwoString, END_DIC, 0) == true))
			{
				InsertStemString(pNode, pMinusTwoString);
				SaveHTokenStem(pNode);
				InsertEndingString(pNode, pAddTwoString);
				if (IsExistEndDic(pAddTwoString))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, pJongChar);
					AddEndingFeature(pNode, pPrevEndCountChar);
					SaveAmbstrEnding(pNode, pJongChar);
					SaveAmbstrEnding(pNode, pPrevEndCountChar);
				}
			}
			else if (ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03))
			{
				pOriginalString = pNode->pHToken->strSource;

				CHAR_DELETE(pThisStartCountChar);
				CHAR_DELETE(pPrevEndCountChar);
				CHAR_DELETE(pAddTwoString);
				CHAR_DELETE(pOneChar);
				CHAR_DELETE(pJongChar);

				pThisStartCountChar = GetStartCountChar(pOriginalString, nStart);
				nCount = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_PLUS);
				pPrevEndCountChar = GetEndCountChar(pOriginalString, nCount);
				pAddTwoString = AddTwoStr(pOldJongChar, pPrevEndCountChar);
				if ((IsExistFeature(pThisStartCountChar, STEM_DIC, 4) == true && IsExistFeature(pAddTwoString, END_DIC, 2) == true) ||
					(IsExistFeature(pThisStartCountChar, STEM_DIC, 4) == false && IsExistFeature(pAddTwoString, END_DIC, 2) == false) ||
					(IsExistFeature(pThisStartCountChar, STEM_DIC, 1) == true || IsExistFeature(pAddTwoString, END_DIC, 0) == true))
				{
					InsertStemString(pNode, pThisStartCountChar);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, pAddTwoString);
					if (IsExistEndDic(pAddTwoString))
					{
						SaveHTokenEnding(pNode);
					}
					else
					{
						MakeNewEndingFeature(pNode);
						AddEndingFeature(pNode, pOldJongChar);
						AddEndingFeature(pNode, pPrevEndCountChar);
						SaveAmbstrEnding(pNode, pJongChar);
						SaveAmbstrEnding(pNode, pPrevEndCountChar);
					}
				}
				else
				{
					JasoRestoration(pNode, nStart, FORCE_REPLACE_ENDWORD, nRestoration);
					pOriginalString = pNode->pHToken->strSource;
					nCount = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_MINUS);

					CHAR_DELETE(pThisStartCountChar);
					CHAR_DELETE(pPrevEndCountChar);

					pThisStartCountChar = GetStartCountChar(pOriginalString, nCount);
					pPrevEndCountChar = GetEndCountChar(pOriginalString, nStart);
					InsertStemString(pNode, pThisStartCountChar);
					SaveHTokenStem(pNode);
					InsertEndingString(pNode, pPrevEndCountChar);
					SaveHTokenEnding(pNode);
				}
			}
			else
			{
				JasoRestoration(pNode, nStart, FORCE_REPLACE_ENDWORD, nRestoration);
				pOriginalString = pNode->pHToken->strSource;

				nCount = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_MINUS);

				CHAR_DELETE(pThisStartCountChar);
				CHAR_DELETE(pPrevEndCountChar);

				pThisStartCountChar = GetStartCountChar(pOriginalString, nCount);
				pPrevEndCountChar = GetEndCountChar(pOriginalString, nStart);
				InsertStemString(pNode, pThisStartCountChar);
				SaveHTokenStem(pNode);
				InsertEndingString(pNode, pPrevEndCountChar);
				SaveHTokenEnding(pNode);
			}
		}
		else if (ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03))
		{
			pOriginalString = pNode->pHToken->strSource;

			CHAR_DELETE(pThisStartCountChar);
			CHAR_DELETE(pPrevEndCountChar);
			CHAR_DELETE(pAddTwoString);

			pThisStartCountChar = GetStartCountChar(pOriginalString, nStart);
			nCount = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_PLUS);
			pPrevEndCountChar = GetEndCountChar(pOriginalString, nCount);
			pAddTwoString = AddTwoStr(pOldJongChar, pPrevEndCountChar);
			if ((IsExistFeature(pThisStartCountChar, STEM_DIC, 4) == true && IsExistFeature(pAddTwoString, END_DIC, 2) == true) ||
				(IsExistFeature(pThisStartCountChar, STEM_DIC, 4) == false && IsExistFeature(pAddTwoString, END_DIC, 2) == false) ||
				(IsExistFeature(pThisStartCountChar, STEM_DIC, 1) == true || IsExistFeature(pAddTwoString, END_DIC, 0) == true))
			{
				InsertStemString(pNode, pThisStartCountChar);
				SaveHTokenStem(pNode);

				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr(pOldJongChar, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString);
				if (IsExistEndDic(pAddTwoString))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, pOldJongChar);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, pOldJongChar);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else
			{
				JasoRestoration(pNode, nStart, FORCE_REPLACE_ENDWORD, nRestoration);
				pOriginalString = pNode->pHToken->strSource;

				CHAR_DELETE(pOneChar);

				pOneChar = GetOneChar(pOriginalString, nStart, nStart);

				CHAR_DELETE(pJongChar);

				pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);
				InsertArrayFeatureOld(pJongChar);

				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr(pJongChar, m_pOldBuffer);
				nLen = strlen(pAddTwoString);

				CHAR_DELETE(m_pOldBuffer);

				m_pOldBuffer = new char[nLen + 1];
				memset(m_pOldBuffer, 0x00, nLen + 1);
				strcpy(m_pOldBuffer, pAddTwoString);
				ReplaceHangulJongsungNull(pNode, nStart);

				GoMAR2(pNode, nStart, mAlkong, mSecondAlkong);
			}
		}
		else
		{
			JasoRestoration(pNode, nStart, FORCE_REPLACE_ENDWORD, nRestoration);
			pOriginalString = pNode->pHToken->strSource;

			CHAR_DELETE(pOneChar);

			pOneChar = GetOneChar(pOriginalString, nStart, nStart);

			CHAR_DELETE(pJongChar);

			pOriginJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);
			pChoJungChar = GetChojungJongsungChar(pOneChar, CHAR_CHOJUNGSUNG);

			CHAR_DELETE(pOneChar);

			nCount = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_MINUS);
			pOneChar = GetOneChar(pOriginalString, nCount, nCount);

			CHAR_DELETE(pJongChar);

			pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);
			pThisEndCountChar = GetEndCountChar(pOriginalString, nStart);

			CHAR_DELETE(pAddTwoString);

			pAddTwoString = AddTwoStr(pJongChar, pThisEndCountChar);
			if (IsExistEndDic(pAddTwoString))
			{
				CHAR_DELETE(pNextStartCountChar);
				CHAR_DELETE(pMinusTwoString);

				pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
				pMinusTwoString = MinusTwoStr(pNextStartCountChar, pOneChar, nCount);
				if (IsExistStemDic(pMinusTwoString))
				{
					if ((IsExistFeature(pMinusTwoString, STEM_DIC, 4) == true && IsExistFeature(pAddTwoString, END_DIC, 2) == true) ||
						(IsExistFeature(pMinusTwoString, STEM_DIC, 4) == false && IsExistFeature(pAddTwoString, END_DIC, 2) == false) ||
						(IsExistFeature(pMinusTwoString, STEM_DIC, 1) == true || IsExistFeature(pAddTwoString, END_DIC, 0) == true))
					{
						InsertStemString(pNode, pMinusTwoString);
						SaveHTokenStem(pNode);
						InsertEndingString(pNode, pAddTwoString);
						if (IsExistEndDic(pAddTwoString))
						{
							SaveHTokenEnding(pNode);
						}
						else
						{
							MakeNewEndingFeature(pNode);
							AddEndingFeature(pNode, pJongChar);
							AddEndingFeature(pNode, pThisEndCountChar);
							SaveAmbstrEnding(pNode, pJongChar);
							SaveAmbstrEnding(pNode, pThisEndCountChar);
						}
					}
					else if (IsExisteTableReplaceJaso(pNode, nStart, FORCE_REPLACE_ENDWORD, nRestoration))
					{
						pOriginalString = pNode->pHToken->strSource;
						nCount = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_PLUS);

						CHAR_DELETE(pPrevEndCountChar);

						pPrevEndCountChar = GetEndCountChar(pOriginalString, nCount);

						CHAR_DELETE(pAddTwoString);

						pAddTwoString = AddTwoStr(pJongChar, pPrevEndCountChar);
						nLen = strlen(pAddTwoString);

						CHAR_DELETE(m_pOldBuffer);

						m_pOldBuffer = new char[nLen + 1];
						memset(m_pOldBuffer, 0x00, nLen + 1);
						strcpy(m_pOldBuffer, pAddTwoString);
						InsertArrayFeatureOld(pAddTwoString);
						ReplaceHangulJongsungNull(pNode, nStart);

						GoIMR1(pNode, nStart, mAlkong, mSecondAlkong);
					}
					else
					{
						pOriginalString = pNode->pHToken->strSource;
						nCount = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_PLUS);

						CHAR_DELETE(pPrevEndCountChar);

						pPrevEndCountChar = GetEndCountChar(pOriginalString, nCount);

						CHAR_DELETE(pAddTwoString);

						pAddTwoString = AddTwoStr(pJongChar, pPrevEndCountChar);
						nLen = strlen(pAddTwoString);

						CHAR_DELETE(m_pOldBuffer);

						m_pOldBuffer = new char[nLen + 1];
						memset(m_pOldBuffer, 0x00, nLen + 1);
						strcpy(m_pOldBuffer, pAddTwoString);
						InsertArrayFeatureOld(pAddTwoString);
						ReplaceHangulJongsungNull(pNode, nStart);

						GoMAR2(pNode, nStart, mAlkong, mSecondAlkong);
					}
				}
				else if (IsExisteTableReplaceJaso(pNode, nStart, FORCE_REPLACE_ENDWORD, nRestoration))
				{
					pOriginalString = pNode->pHToken->strSource;
					nCount = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_PLUS);

					CHAR_DELETE(pPrevEndCountChar);

					pPrevEndCountChar = GetEndCountChar(pOriginalString, nCount);

					CHAR_DELETE(pAddTwoString);

					pAddTwoString = AddTwoStr(pJongChar, pPrevEndCountChar);
					nLen = strlen(pAddTwoString);

					CHAR_DELETE(m_pOldBuffer);

					m_pOldBuffer = new char[nLen + 1];
					memset(m_pOldBuffer, 0x00, nLen + 1);
					strcpy(m_pOldBuffer, pAddTwoString);
					InsertArrayFeatureOld(pAddTwoString);
					ReplaceHangulJongsungNull(pNode, nStart);

					GoIMR1(pNode, nStart, mAlkong, mSecondAlkong);
				}
				else
				{
					pOriginalString = pNode->pHToken->strSource;
					nCount = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_PLUS);

					CHAR_DELETE(pPrevEndCountChar);

					pPrevEndCountChar = GetEndCountChar(pOriginalString, nCount);

					CHAR_DELETE(pAddTwoString);

					pAddTwoString = AddTwoStr(pJongChar, pPrevEndCountChar);
					nLen = strlen(pAddTwoString);

					CHAR_DELETE(m_pOldBuffer);

					m_pOldBuffer = new char[nLen + 1];
					memset(m_pOldBuffer, 0x00, nLen + 1);
					strcpy(m_pOldBuffer, pAddTwoString);
					InsertArrayFeatureOld(pAddTwoString);
					ReplaceHangulJongsungNull(pNode, nStart);

					GoMAR2(pNode, nStart, mAlkong, mSecondAlkong);
				}
			}
			else if (IsExistFeature(pOriginJongChar, END_DIC, E_Prs) == true &&
				IsExistStemDic(pMinusTwoString) == false &&
				strstr("시,이,하,으,되", pChoJungChar) == NULL)
			{
				GoErrorFunction(pNode);
			}
			else if (IsExisteTableReplaceJaso(pNode, nStart, FORCE_REPLACE_ENDWORD, nRestoration))
			{
				pOriginalString = pNode->pHToken->strSource;
				nCount = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_PLUS);

				CHAR_DELETE(pPrevEndCountChar);

				pPrevEndCountChar = GetEndCountChar(pOriginalString, nCount);

				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr(pOldJongChar, pPrevEndCountChar);
				nLen = strlen(pAddTwoString);

				CHAR_DELETE(m_pOldBuffer);

				m_pOldBuffer = new char[nLen + 1];
				memset(m_pOldBuffer, 0x00, nLen + 1);
				strcpy(m_pOldBuffer, pAddTwoString);
				InsertArrayFeatureOld(pAddTwoString);
				ReplaceHangulJongsungNull(pNode, nStart);

				GoIMR1(pNode, nStart, mAlkong, mSecondAlkong);
			}
			else
			{
				pOriginalString = pNode->pHToken->strSource;
				nCount = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_PLUS);

				CHAR_DELETE(pPrevEndCountChar);

				pPrevEndCountChar = GetEndCountChar(pOriginalString, nCount);

				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr(pOldJongChar, pPrevEndCountChar);
				nLen = strlen(pAddTwoString);

				CHAR_DELETE(m_pOldBuffer);

				m_pOldBuffer = new char[nLen + 1];
				memset(m_pOldBuffer, 0x00, nLen + 1);
				strcpy(m_pOldBuffer, pAddTwoString);
				InsertArrayFeatureOld(pAddTwoString);
				ReplaceHangulJongsungNull(pNode, nStart);

				GoMAR2(pNode, nStart, mAlkong, mSecondAlkong);
			}
		}
	}
	else
	{
		GoMAR2(pNode, nStart, mAlkong, mSecondAlkong);
	}


	CHAR_DELETE(pThisStartCountChar);
	CHAR_DELETE(pNextStartCountChar);
	CHAR_DELETE(pPrevEndCountChar);
	CHAR_DELETE(pThisEndCountChar);
	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pJongChar);
	CHAR_DELETE(pOriginJongChar);
	CHAR_DELETE(pOldJongChar);
	CHAR_DELETE(pChoJungChar);
	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pMinusTwoString);

}


/////////////////////////////////////////////////////////
//MAR2
/////////////////////////////////////////////////////////
void CNAHangulAnaly::GoMAR2(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong)
{
	GoMAR2_1(pNode, nStart, mAlkong, mSecondAlkong);
}

void CNAHangulAnaly::GoMAR2_1(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong)
{
	char* pOriginalString = NULL;
	char* pOneChar = NULL;
	char* pJongChar = NULL;

	pOriginalString = pNode->pHToken->strSource;
	pOneChar = GetOneChar(pOriginalString, nStart, nStart);
	pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);

	if (m_bOneCharMask == false && strcmp(pOneChar, "려") == 0)
	{
		GoMAR3(pNode, nStart, mAlkong, mSecondAlkong);
	}
	else
	{
		if ((m_bOneCharMask == false) && (CompareVariousTable01(pOneChar, 0, false) == true ||
			CompareVariousTable02(pOneChar, 1, false) == true ||
			CompareVariousTable02(pOneChar, 2, false) == true ||
			CompareVariousTable03(pOneChar, 0, false) == true ||
			CompareVariousTable03(pOneChar, 1, false) == true ||
			CompareVariousTable04(pOneChar, 1, false) == true ||
			CompareVariousTable04(pOneChar, 2, false) == true ||
			CompareVariousTable04(pOneChar, 3, false) == true ||
			CompareVariousTable05(pOneChar, ALL_TABLE_05, true) == true ||
			CompareVariousTable06(pOneChar, ALL_TABLE_06, true) == true ||
			CompareVariousTable07(pOneChar, ALL_TABLE_07, true) == true ||
			strcmp(pJongChar, "ㅆ") == 0 || strcmp(pJongChar, "ㅁ") == 0 ||
			strcmp(pJongChar, "ㄻ") == 0 ||
			strcmp(pOneChar, "기") == 0 || strcmp(pOneChar, "우") == 0))
		{
			GoIMR1(pNode, nStart, mAlkong, mSecondAlkong);
		}
		else
		{
			GoMAR2_2(pNode, nStart, mAlkong, mSecondAlkong);
		}
	}

	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pJongChar);
}

void CNAHangulAnaly::GoMAR2_2(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong)
{
	char* pOriginalString = NULL;
	char* pMiddleCountChar = NULL;
	int nCount = 0;

	pOriginalString = pNode->pHToken->strSource;

	if (nStart == mAlkong)
		mAlkong = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_PLUS);

	pMiddleCountChar = GetMiddleCountChar(pOriginalString, nStart, mAlkong, CALCULATE_STOP, CALCULATE_MINUS);

	//if (IsExistWildEndingDic(pMiddleCountChar) > 0 || strlen(pMiddleCountChar) == 0){
	if (IsExistWildEndingDic(pMiddleCountChar) == true || strlen(pMiddleCountChar) == 0) {
		GoMAR2_3(pNode, nStart, mAlkong, mSecondAlkong);
	}
	else
	{
		GoMAR2_4(pNode, nStart, mAlkong, mSecondAlkong);
	}

	CHAR_DELETE(pMiddleCountChar);
}

void CNAHangulAnaly::GoMAR2_3(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong)
{
	char* pOriginalString = NULL;

	char* pMiddleCountChar = NULL;
	char* pThisStartCountChar = NULL;
	char* pNextStartCountChar = NULL;
	char* pThisEndCountChar = NULL;
	char* pOneChar = NULL;
	char* pJongChar = NULL;
	char* pNextOneChar = NULL;
	char* pAddTwoString = NULL;
	char* pAddTwoString2 = NULL;
	char* pMinusTwoString = NULL;
	char* pMiddleCountChar1 = NULL;
	char* pMiddleCountChar2 = NULL;
	char* pMiddleCountChar3 = NULL;
	char* tempOneChar = NULL; // Added by KKH 2003-11-07
	char* tempJongSung = NULL; // Added by KKH 2003-11-07

	int nLen = 0;
	int nCount = 0;
	int nBLen = 0; // Added by KKH 2003-11-07

	pOriginalString = pNode->pHToken->strSource;

	mSecondAlkong = nStart;
	//S(n~m+1)
	pMiddleCountChar = GetMiddleCountChar(pOriginalString, nStart, mAlkong, CALCULATE_STOP, CALCULATE_MINUS);

	if (IsExistEndDic(pMiddleCountChar))
	{
		CHAR_DELETE(m_pNewBuffer);

		nLen = strlen(pMiddleCountChar);
		if (nLen > 0)
		{
			m_pNewBuffer = new char[nLen + 1];
			memset(m_pNewBuffer, 0x00, nLen + 1);
			strcpy(m_pNewBuffer, pMiddleCountChar);
			InsertArrayFeatureNew(pMiddleCountChar);
		}
		nCount = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_MINUS);
		pNextOneChar = GetOneChar(pOriginalString, nCount, nCount);
		tempJongSung = GetChojungJongsungChar(pNextOneChar, CHAR_JONGSUNG);
		pNextStartCountChar = GetStartCountChar(pOriginalString, nCount); //S(len~n+1)
		if (IsExistStemDic(pNextStartCountChar))//2y
		{
			pOneChar = GetOneChar(pOriginalString, nCount, nCount);
			pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);
			pAddTwoString = AddTwoStr(pJongChar, pMiddleCountChar);
			if (IsExistEndDic(pAddTwoString) && strlen(pJongChar) > 0)//3y
			{
				pMinusTwoString = MinusTwoStr(pNextStartCountChar, pOneChar, nCount);
				if (IsExistStemDic(pMinusTwoString))//4y
				{
					if ((IsExistFeature(pMinusTwoString, STEM_DIC, 4) == true && IsExistFeature(pAddTwoString, END_DIC, 2) == true) ||
						(IsExistFeature(pMinusTwoString, STEM_DIC, 4) == false && IsExistFeature(pAddTwoString, END_DIC, 2) == false) ||
						(IsExistFeature(pMinusTwoString, STEM_DIC, 1) == true || IsExistFeature(pAddTwoString, END_DIC, 0) == true))
						//5y
					{
						if ((IsExistFeature(pAddTwoString, END_DIC, 4) == true && IsExistFeature(m_pOldBuffer, END_DIC, 2) == true) ||
							(IsExistFeature(pAddTwoString, END_DIC, 4) == false && IsExistFeature(m_pOldBuffer, END_DIC, 2) == false) ||
							(IsExistFeature(pAddTwoString, END_DIC, 1) == true || IsExistFeature(m_pOldBuffer, END_DIC, 0) == true))
						{
							if ((strcmp(pNextStartCountChar, "자신") == 0 || strcmp(pNextStartCountChar, "분") == 0) &&
								(IsExistFeature(m_pNewBuffer, END_DIC, 0) == true || IsExistFeature(m_pNewBuffer, END_DIC, 2) == true) ||
								(IsExistFeature(m_pOldBuffer, END_DIC, 0) == true || IsExistFeature(m_pOldBuffer, END_DIC, 2) == true))
							{
								InsertStemString(pNode, pNextStartCountChar);
								SaveHTokenStem(pNode);
								pAddTwoString2 = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
								InsertEndingString(pNode, pAddTwoString2);
								if (IsExistEndDic(pAddTwoString2))
								{
									SaveHTokenEnding(pNode);
								}
								else
								{
									MakeNewEndingFeature(pNode);
									SaveHTokenEndingOldFeature(pNode);
									SaveHTokenEndingNewFeature(pNode);
								}
							}
							else
							{
								InsertStemString(pNode, pMinusTwoString);
								SaveHTokenStem(pNode);
								pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
								InsertEndingString(pNode, pAddTwoString2);
								if (IsExistEndDic(pAddTwoString2))
								{
									SaveHTokenEnding(pNode);
								}
								else
								{
									MakeNewEndingFeature(pNode);
									AddEndingFeature(pNode, pAddTwoString);
									SaveHTokenEndingOldFeature(pNode);
									SaveAmbstrEnding(pNode, pAddTwoString);
									SaveAmbstrEnding(pNode, m_pOldBuffer);
								}
							}
						}
						else
						{
							AddOldNewBuffer(pNode);

							CHAR_DELETE(m_pNewBuffer);

							InsertStemString(pNode, pNextStartCountChar);
							SaveHTokenStem(pNode);
							pThisEndCountChar = GetEndCountChar(pOriginalString, nStart);
							if (IsExistStemDic(pThisEndCountChar) &&
								IsExistFeature(pThisEndCountChar, STEM_DIC, 7) == false &&
								IsExistFeature(pThisEndCountChar, STEM_DIC, 1) == false &&
								strcmp(pThisEndCountChar, "과도") != 0)
							{
								m_LinkedList->CreateSubNode(pNode);
								InsertAddSubTail(pNode, pNextStartCountChar);
								InsertAddSubTail(pNode, pThisEndCountChar);
							}
							else
							{
								InsertEndingString(pNode, m_pOldBuffer);
								MakeNewEndingFeature(pNode);
								AddEndingFeature(pNode, m_pOldBuffer);
								SaveHTokenEndingOldFeature(pNode);
							}
						}
					}
					else if (IsExistFeature(m_pNewBuffer, END_DIC, E_N) == true &&
						IsExistFeature(m_pOldBuffer, END_DIC, E_V) == true)
					{
						GoErrorFunction(pNode);
					}
					else
					{
						AddOldNewBuffer(pNode);

						CHAR_DELETE(m_pNewBuffer);

						InsertStemString(pNode, pNextStartCountChar);
						SaveHTokenStem(pNode);
						pThisEndCountChar = GetEndCountChar(pOriginalString, nStart);
						if (IsExistStemDic(pThisEndCountChar) &&
							IsExistFeature(pThisEndCountChar, STEM_DIC, 7) == false &&
							IsExistFeature(pThisEndCountChar, STEM_DIC, 1) == false &&
							strcmp(pThisEndCountChar, "과도") != 0)
						{
							m_LinkedList->CreateSubNode(pNode);
							InsertAddSubTail(pNode, pNextStartCountChar);
							InsertAddSubTail(pNode, pThisEndCountChar);
						}
						else
						{
							InsertEndingString(pNode, m_pOldBuffer);
							MakeNewEndingFeature(pNode);
							AddEndingFeature(pNode, m_pOldBuffer);
							SaveHTokenEndingOldFeature(pNode);
						}
					}
				}
				else // 5n
				{
					AddOldNewBuffer(pNode);

					CHAR_DELETE(m_pNewBuffer);

					InsertStemString(pNode, pNextStartCountChar);
					SaveHTokenStem(pNode);
					/*
					종성복원
					*/
					pThisEndCountChar = GetEndCountChar(pOriginalString, nStart);
					if (IsExistStemDic(pThisEndCountChar) &&
						IsExistFeature(pThisEndCountChar, STEM_DIC, 7) == false &&
						IsExistFeature(pThisEndCountChar, STEM_DIC, 1) == false &&
						strcmp(pThisEndCountChar, "과도") != 0)
					{
						m_LinkedList->CreateSubNode(pNode);
						InsertAddSubTail(pNode, pNextStartCountChar);
						InsertAddSubTail(pNode, pThisEndCountChar);
					}
					else if (strcmp(m_pOldBuffer, "가저ㄴ") == 0) //생활가전
					{
						ReplaceHgngulTokenSource(pNode, nCount + 4, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA01);
						GoUWR(pNode);
					}
					else if (strcmp(m_pOldBuffer, "세이ㄹ") == 0) //세일세일
					{
						ReplaceHgngulTokenSource(pNode, nCount + 4, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);
						GoUWR(pNode);
					}
					else
					{
						InsertEndingString(pNode, m_pOldBuffer);
						MakeNewEndingFeature(pNode);
						AddEndingFeature(pNode, m_pOldBuffer);
						SaveHTokenEndingOldFeature(pNode);
					}
				}
			}
			// 03.06.04 수정
			else if ((IsExistFeature(pNextStartCountChar, STEM_DIC, S_N) == true && IsExistFeature(m_pNewBuffer, END_DIC, E_N) == true) ||
				(IsExistFeature(pNextStartCountChar, STEM_DIC, S_N) == false && IsExistFeature(m_pNewBuffer, END_DIC, E_N) == false) ||
				IsExistFeature(pNextStartCountChar, STEM_DIC, S_Amb) == true || IsExistFeature(m_pNewBuffer, END_DIC, E_Amb) == true) //4n
			{
				if ((IsExistFeature(m_pNewBuffer, END_DIC, E_N) == true &&
					IsExistFeature(m_pOldBuffer, END_DIC, E_V) == true) ||
					IsExistFeature(m_pNewBuffer, END_DIC, E_Pss) == true ||
					strcmp(m_pNewBuffer, "도") == 0 && strcmp(m_pOldBuffer, "구") == 0 ||
					strcmp(m_pNewBuffer, "지") == 0 && strcmp(m_pOldBuffer, "정") == 0 ||
					strcmp(m_pNewBuffer, "차") == 0 && strcmp(m_pOldBuffer, "도") == 0)
				{
					GoErrorFunction(pNode);
				}
				else
				{
					AddOldNewBuffer(pNode);

					CHAR_DELETE(m_pNewBuffer);

					InsertStemString(pNode, pNextStartCountChar);
					SaveHTokenStem(pNode);
					pThisEndCountChar = GetEndCountChar(pOriginalString, nStart);
					if (strcmp(pThisEndCountChar, "별로") != 0 &&
						strcmp(pThisEndCountChar, "측은") != 0 &&
						strcmp(pThisEndCountChar, "측이") != 0 &&
						IsExistStemDic(pThisEndCountChar) &&
						IsExistFeature(pNextStartCountChar, STEM_DIC, 4) == true &&
						(IsExistFeature(pThisEndCountChar, STEM_DIC, 4) == true ||
							IsExistFeature(pThisEndCountChar, STEM_DIC, 10) == true))
						if (strcmp(m_pOldBuffer, "아사ㄴ") == 0) // 현대아산
						{
							ReplaceHgngulTokenSource(pNode, nCount + 4, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA01);
							GoUWR(pNode);
						}
						else
						{
							m_LinkedList->CreateSubNode(pNode);
							InsertAddSubTail(pNode, pNextStartCountChar);
							InsertAddSubTail(pNode, pThisEndCountChar);
						}
					else if (strcmp(m_pOldBuffer, "가저ㄴ") == 0) //음향가전
					{
						ReplaceHgngulTokenSource(pNode, nCount + 4, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA01);
						GoUWR(pNode);
					}
					else if (strcmp(m_pOldBuffer, "시서ㄹ") == 0 || strcmp(m_pOldBuffer, "건서ㄹ") == 0) //음향시설 현대건설
					{
						ReplaceHgngulTokenSource(pNode, nCount + 4, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03);
						GoUWR(pNode);
					}
					else
					{
						InsertEndingString(pNode, m_pOldBuffer);
						MakeNewEndingFeature(pNode);
						AddEndingFeature(pNode, m_pOldBuffer);
						SaveHTokenEndingOldFeature(pNode);
						// Added by KKH 2003-11-07
						nBLen = strlen(m_pOldBuffer);
						for (int i = 0; i < nBLen; i = i + 2)
						{
							tempOneChar = GetOneChar(m_pOldBuffer, i, i + 1);
							if (strcmp(tempOneChar, "음") == 0 ||
								strcmp(tempOneChar, "기") == 0)
							{
								DeleteHTokenStem(pNode, 7);
								AddHTokenStem(pNode, 39);
								CHAR_DELETE(tempOneChar);
								break;
							}
							else
							{
								CHAR_DELETE(tempOneChar);
							}
						}
						// Added by KKH 2003-11-07
					}
				}
			}
			else //5n
			{
				GoErrorFunction(pNode);
			}
		}
		// Added by KKH 2003-11-07
		else if (strcmp(tempJongSung, "") == 0 && ReplaceHgngulTokenSource(pNode, nCount, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03))
		{
			pThisStartCountChar = GetStartCountChar(pOriginalString, nCount);
			InsertStemString(pNode, pThisStartCountChar);
			SaveHTokenStem(pNode);
			AddHTokenStem(pNode, 39);
			DeleteHTokenStem(pNode, 7);
			pAddTwoString = AddTwoStr(pAddTwoString, m_pNewBuffer);
			//pAddTwoString = AddTwoStr(pAddTwoString, m_pOldBuffer);
			//2017-08-15 수정
			//pAddTowString 포인터에 두번 연속 문자열을 합침으로 인해 메모리 릭 발생
			pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
			//InsertEndingString(pNode, pAddTwoString);
			InsertEndingString(pNode, pAddTwoString2);
			//if (IsExistEndDic(pAddTwoString))
			if (IsExistEndDic(pAddTwoString2))
			{
				SaveHTokenEnding(pNode);
			}
			else
			{
				MakeNewEndingFeature(pNode);
				AddEndingFeature(pNode, "시");
				AddEndingFeature(pNode, "음");
				SaveHTokenEndingNewFeature(pNode);
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, "시");
				SaveAmbstrEnding(pNode, "음");
				SaveAmbstrEnding(pNode, m_pNewBuffer);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
			}
			for (int i = 0; i < nBLen; i = i + 2)
			{
				tempOneChar = GetOneChar(m_pOldBuffer, i, i + 1);
				if (strcmp(tempOneChar, "음") == 0 ||
					strcmp(tempOneChar, "기") == 0)
				{
					DeleteHTokenStem(pNode, 7);
					AddHTokenStem(pNode, 39);
					CHAR_DELETE(tempOneChar);
					break;
				}
				else
				{
					CHAR_DELETE(tempOneChar);
				}
			}

			CHAR_DELETE(pThisStartCountChar);
			CHAR_DELETE(pAddTwoString);
			CHAR_DELETE(pAddTwoString2);
			CHAR_DELETE(tempOneChar);
			CHAR_DELETE(m_pNewBuffer);
			CHAR_DELETE(m_pOldBuffer);
		}
		// Added by KKH 2003-11-07
		// 03.06.04 수정
		else if (!(strcmp(m_pNewBuffer, "시") == 0 && (strcmp(pNextOneChar, "하") == 0 || strcmp(pNextOneChar, "으") == 0)) &&
			(IsExistFeature(m_pNewBuffer, END_DIC, E_Prs) == true ||
				IsExistFeature(m_pNewBuffer, END_DIC, E_Pss) == true))
		{
			if (strcmp(tempJongSung, "") == 0)
			{
				ReplaceHangulJongsungNull(pNode, nCount);
			}
			GoErrorFunction(pNode);
		}
		else if (ReturnNumberMoveLeftWordCount(pOriginalString, nStart) < 0)
		{
			if (strcmp(tempJongSung, "") == 0)
			{
				ReplaceHangulJongsungNull(pNode, nCount);
			}
			GoErrorFunction(pNode);
		}
		else
		{
			if (strcmp(tempJongSung, "") == 0)
			{
				ReplaceHangulJongsungNull(pNode, nCount);
			}
			GoMAR2_1(pNode, nStart, mAlkong, mSecondAlkong);
		}
	}
	else if (ReturnNumberMoveLeftWordCount(pOriginalString, nStart) < 0)
	{
		GoErrorFunction(pNode);
	}
	else
	{
		CHAR_DELETE(pOneChar);
		pOneChar = GetOneChar(pOriginalString, nStart, nStart);
		int nNextCount = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_PLUS);
		int nPrevCount = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_MINUS);

		pMiddleCountChar1 = GetMiddleCountChar(pOriginalString, nStart, nNextCount, CALCULATE_STOP, CALCULATE_STOP);

		if (nPrevCount > 0)
		{
			pMiddleCountChar2 = GetMiddleCountChar(pOriginalString, nPrevCount, nNextCount, CALCULATE_STOP, CALCULATE_STOP);
		}

		nPrevCount = ReturnNextPrevCount(pOriginalString, nPrevCount, 1, CALCULATE_MINUS);// n+2
		nNextCount = ReturnNextPrevCount(pOriginalString, nNextCount, 1, CALCULATE_PLUS);	// n-2

		if (nPrevCount > 0)
		{
			pMiddleCountChar3 = GetMiddleCountChar(pOriginalString, nPrevCount, nNextCount, CALCULATE_STOP, CALCULATE_STOP);
		}

		if ((pMiddleCountChar1 != NULL && IsExistEndDic(pMiddleCountChar1)) ||
			(pMiddleCountChar2 != NULL && IsExistEndDic(pMiddleCountChar2)) ||
			(pMiddleCountChar3 != NULL && IsExistEndDic(pMiddleCountChar3)))
		{
			nCount = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_PLUS);

			CHAR_DELETE(pOneChar);

			pOneChar = GetOneChar(pOriginalString, nCount, nCount);

			CHAR_DELETE(pAddTwoString);

			pAddTwoString = AddTwoStr(pOneChar, m_pNewBuffer);

			CHAR_DELETE(m_pNewBuffer);

			nCount = strlen(pAddTwoString);
			m_pNewBuffer = new char[nCount + 1];
			memset(m_pNewBuffer, 0x00, nCount + 1);
			strcpy(m_pNewBuffer, pAddTwoString);
			mAlkong = nCount;

			GoMAR2_1(pNode, nStart, mAlkong, mSecondAlkong);
		}
		else
		{
			GoErrorFunction(pNode);
		}
	}
	/*
	CHAR_DELETE(pMiddleCountChar);
	CHAR_DELETE(pMiddleCountChar1);
	CHAR_DELETE(pMiddleCountChar2);
	CHAR_DELETE(pMiddleCountChar3);
	CHAR_DELETE(pThisStartCountChar);
	CHAR_DELETE(pNextStartCountChar);
	CHAR_DELETE(pThisEndCountChar);
	CHAR_DELETE(pThisEndCountChar);
	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pJongChar);
	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pAddTwoString2);
	CHAR_DELETE(pMinusTwoString);
	CHAR_DELETE(tempOneChar); // Added by KKH 2003-11-07
	*/
	CHAR_DELETE(pMiddleCountChar);
	CHAR_DELETE(pThisStartCountChar);
	CHAR_DELETE(pNextStartCountChar);
	CHAR_DELETE(pThisEndCountChar);
	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pJongChar);
	CHAR_DELETE(pNextOneChar);
	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pAddTwoString2);
	CHAR_DELETE(pMinusTwoString);
	CHAR_DELETE(pMiddleCountChar1);
	CHAR_DELETE(pMiddleCountChar2);
	CHAR_DELETE(pMiddleCountChar3);
	CHAR_DELETE(tempOneChar);
	CHAR_DELETE(tempJongSung);
}

void CNAHangulAnaly::GoMAR2_4(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong)
{
	char* pOriginalString = NULL;

	char* pMiddleString = NULL;
	char* pMiddleString2 = NULL;
	char* pNextStartCountChar = NULL;
	char* pThisStartCountChar = NULL;
	char* pThisEndCountChar = NULL;
	char* pOneChar = NULL;
	char* pOneChar2 = NULL;
	char* pJongChar = NULL;
	char* pJongChar2 = NULL;
	char* pMinusTwoString = NULL;
	char* pAddTwoString = NULL;
	char* pAddTwoString2 = NULL;

	bool bisFlag = false;

	int nCount = 0;
	int nLen = 0;

	pOriginalString = pNode->pHToken->strSource;
	nCount = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_MINUS);
	pOneChar = GetOneChar(pOriginalString, nCount, nCount);
	pOneChar2 = GetOneChar(pOriginalString, nStart, nStart);
	pJongChar2 = GetChojungJongsungChar(pOneChar2, CHAR_JONGSUNG);
	pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);
	pMiddleString = GetMiddleCountChar(pOriginalString, nStart, mAlkong, CALCULATE_STOP, CALCULATE_MINUS);
	pMiddleString2 = GetMiddleCountChar(pOriginalString, nStart, mAlkong, CALCULATE_PLUS, CALCULATE_MINUS);

	pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);

	if (pMiddleString2 != NULL && strcmp(pMiddleString2, "") != 0)
		bisFlag = true;
	pAddTwoString = AddTwoStr(pJongChar, pMiddleString);
	pAddTwoString2 = AddTwoStr(pJongChar2, pMiddleString2);

	if (IsExistEndDic(pAddTwoString))
	{
		pMinusTwoString = MinusTwoStr(pNextStartCountChar, pOneChar, nCount);
		if (IsExistStemDic(pMinusTwoString) && strlen(pJongChar) > 0)
		{
			InsertStemString(pNode, pMinusTwoString);
			SaveHTokenStem(pNode);

			CHAR_DELETE(pAddTwoString2);

			pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
			InsertEndingString(pNode, pAddTwoString2);
			if (IsExistEndDic(pAddTwoString2))
			{
				SaveHTokenEnding(pNode);
			}
			else
			{
				MakeNewEndingFeature(pNode);
				AddEndingFeature(pNode, pAddTwoString);
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, pAddTwoString);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
			}
		}
		else if (IsExistEndDic(pAddTwoString2) && bisFlag == true && strlen(pJongChar2) > 0)
		{
			CHAR_DELETE(pMinusTwoString);

			pThisStartCountChar = GetStartCountChar(pOriginalString, nStart);
			pMinusTwoString = MinusTwoStr(pThisStartCountChar, pOneChar2, nStart);
			if (IsExistStemDic(pMinusTwoString))
			{
				InsertStemString(pNode, pMinusTwoString);
				SaveHTokenStem(pNode);

				CHAR_DELETE(pAddTwoString2);

				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, pAddTwoString);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, pAddTwoString);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else
			{
				CHAR_DELETE(pAddTwoString2);

				InsertArrayFeatureOld(pAddTwoString);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);

				CHAR_DELETE(m_pOldBuffer);

				nLen = strlen(pAddTwoString2);
				m_pOldBuffer = new char[nLen + 1];
				memset(m_pOldBuffer, 0x00, nLen + 1);
				strcpy(m_pOldBuffer, pAddTwoString2);
				ReplaceHangulJongsungNull(pNode, nStart);
				nCount = ReturnNextPrevCount(pOriginalString, mAlkong, 1, CALCULATE_MINUS);
				mAlkong = nCount;
				GoMAR2_1(pNode, nStart, mAlkong, mSecondAlkong);
			}
		}
		else
		{
			CHAR_DELETE(pAddTwoString2);

			pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
			InsertEndingString(pNode, pAddTwoString2);
			if (IsExistEndDic(pAddTwoString2))
			{
				SaveHTokenEnding(pNode);
			}
			else
			{
				MakeNewEndingFeature(pNode);
				AddEndingFeature(pNode, pAddTwoString);
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, pAddTwoString);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
			}
			GoMAR2_1(pNode, nStart, mAlkong, mSecondAlkong);
		}
	}
	//2n
	else if (IsExistEndDic(pAddTwoString2) && bisFlag == true && strlen(pJongChar2) > 0)
	{
		if (IsExistFeature(pAddTwoString2, END_DIC, E_Pss))
		{
			GoErrorFunction(pNode);
		}
		else
		{
			CHAR_DELETE(pMinusTwoString);

			pThisStartCountChar = GetStartCountChar(pOriginalString, nStart);
			pMinusTwoString = MinusTwoStr(pThisStartCountChar, pOneChar2, nStart);
			if (IsExistStemDic(pMinusTwoString))
			{
				InsertStemString(pNode, pMinusTwoString);
				SaveHTokenStem(pNode);

				CHAR_DELETE(pAddTwoString);
				CHAR_DELETE(pAddTwoString2);

				pAddTwoString = AddTwoStr(pJongChar2, m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, pAddTwoString);
					AddEndingFeature(pNode, m_pNewBuffer);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, pAddTwoString);
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else
			{
				CHAR_DELETE(pAddTwoString);
				CHAR_DELETE(pAddTwoString2);

				InsertArrayFeatureOld(pJongChar2);
				pAddTwoString = AddTwoStr(pJongChar2, m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);

				CHAR_DELETE(m_pOldBuffer);

				nLen = strlen(pAddTwoString2);
				m_pOldBuffer = new char[nLen + 1];
				memset(m_pOldBuffer, 0x00, nLen + 1);
				strcpy(m_pOldBuffer, pAddTwoString2);

				CHAR_DELETE(m_pNewBuffer);

				ReplaceHangulJongsungNull(pNode, nStart);
				pOriginalString = pNode->pHToken->strSource;
				nCount = ReturnNextPrevCount(pOriginalString, mAlkong, 1, CALCULATE_MINUS);
				mAlkong = nCount;
				GoMAR2_1(pNode, nStart, mAlkong, mSecondAlkong);
			}
		}
	}
	else if (IsExistEndDic(pJongChar2)) //3n
	{
		CHAR_DELETE(pMinusTwoString);

		pThisStartCountChar = GetStartCountChar(pOriginalString, nStart);
		pMinusTwoString = MinusTwoStr(pThisStartCountChar, pOneChar2, nStart);
		if (IsExistStemDic(pMinusTwoString))
		{
			if (strstr("ㄴ,ㄹ", pJongChar2) != NULL ||
				IsExistFeature(pMinusTwoString, STEM_DIC, S_N) == true)
			{
				GoErrorFunction(pNode);
			}
			else
			{
				InsertStemString(pNode, pMinusTwoString);
				SaveHTokenStem(pNode);

				CHAR_DELETE(pAddTwoString);
				CHAR_DELETE(pAddTwoString2);

				pAddTwoString = AddTwoStr(pJongChar2, m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, pAddTwoString);
					AddEndingFeature(pNode, m_pNewBuffer);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, pAddTwoString);
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
		}
		else if (IsExistEndDic(pOneChar2))
		{
			if (IsExistStemDic(pNextStartCountChar))
			{
				if ((IsExistFeature(pNextStartCountChar, STEM_DIC, 4) == true && IsExistFeature(pOneChar2, END_DIC, 2) == true) ||
					(IsExistFeature(pNextStartCountChar, STEM_DIC, 4) == false && IsExistFeature(pOneChar2, END_DIC, 2) == false))
				{
					InsertStemString(pNode, pNextStartCountChar);
					SaveHTokenStem(pNode);

					CHAR_DELETE(pAddTwoString);
					CHAR_DELETE(pAddTwoString2);

					pAddTwoString = AddTwoStr(pOneChar2, m_pNewBuffer);
					pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
					InsertEndingString(pNode, pAddTwoString2);
					if (IsExistEndDic(pAddTwoString2))
					{
						SaveHTokenEnding(pNode);
					}
					else
					{
						MakeNewEndingFeature(pNode);
						AddEndingFeature(pNode, pOneChar2);
						SaveAmbstrEnding(pNode, m_pOldBuffer);
						AddEndingFeature(pNode, m_pNewBuffer);
						SaveHTokenEndingOldFeature(pNode);

						SaveAmbstrEnding(pNode, pOneChar2);
						SaveAmbstrEnding(pNode, m_pNewBuffer);
						SaveAmbstrEnding(pNode, m_pOldBuffer);
					}
				}
				else
				{
					GoErrorFunction(pNode);
				}
			}
			else
			{
				GoErrorFunction(pNode);
			}
		}
		else
		{
			GoErrorFunction(pNode);
		}
	}
	else if (IsExistStemDic(pNextStartCountChar)) //4n
	{
		CHAR_DELETE(pAddTwoString);

		pAddTwoString = AddTwoStr(pOneChar2, m_pNewBuffer);
		if (IsExistStemDic(pAddTwoString))
		{
			GoErrorFunction(pNode);
		}
		else
		{
			InsertStemString(pNode, pNextStartCountChar);
			SaveHTokenStem(pNode);
			pThisEndCountChar = GetEndCountChar(pOriginalString, nStart);
			if (IsExistStemDic(pThisEndCountChar) &&
				IsExistFeature(pThisEndCountChar, STEM_DIC, 7) == false &&
				IsExistFeature(pThisEndCountChar, STEM_DIC, 1) == false &&
				strcmp(pThisEndCountChar, "과도") != 0) // Original string값 변경문제
			{
				if (IsExistFeature(pNextStartCountChar, STEM_DIC, 7) == true)
				{
					CHAR_DELETE(pAddTwoString);
					CHAR_DELETE(pAddTwoString2);

					pAddTwoString = AddTwoStr(pOneChar2, m_pOldBuffer);
					pAddTwoString2 = AddTwoStr(pAddTwoString, m_pNewBuffer);

					InsertEndingString(pNode, pAddTwoString2);
					if (IsExistEndDic(pAddTwoString2))
					{
						SaveHTokenEnding(pNode);
					}
					else
					{
						MakeNewEndingFeature(pNode);
						AddEndingFeature(pNode, pOneChar2);
						SaveAmbstrEnding(pNode, m_pOldBuffer);
						AddEndingFeature(pNode, m_pNewBuffer);
						SaveHTokenEndingOldFeature(pNode);

						SaveAmbstrEnding(pNode, pOneChar2);
						SaveAmbstrEnding(pNode, m_pNewBuffer);
						SaveAmbstrEnding(pNode, m_pOldBuffer);
					}
				}
				else
				{
					m_LinkedList->CreateSubNode(pNode);
					InsertAddSubTail(pNode, pNextStartCountChar);
					InsertAddSubTail(pNode, pThisEndCountChar);
				}
			}
			else if (IsExistEndDic(pOneChar2))
			{
				InsertStemString(pNode, pNextStartCountChar);
				SaveHTokenStem(pNode);

				CHAR_DELETE(pAddTwoString);
				CHAR_DELETE(pAddTwoString2);

				pAddTwoString = AddTwoStr(pOneChar2, m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);

				// 03.06.04 수정
				if ((IsExistFeature(pNextStartCountChar, STEM_DIC, S_N) == true && IsExistFeature(pOneChar2, END_DIC, E_N) == true) ||
					(IsExistFeature(pNextStartCountChar, STEM_DIC, S_N) == false && IsExistFeature(pOneChar2, END_DIC, E_N) == false))
				{
					if (IsExistEndDic(pAddTwoString2))
					{
						SaveHTokenEnding(pNode);
					}
					else
					{
						MakeNewEndingFeature(pNode);
						AddEndingFeature(pNode, pOneChar2);
						SaveAmbstrEnding(pNode, m_pOldBuffer);
						AddEndingFeature(pNode, m_pNewBuffer);
						SaveHTokenEndingOldFeature(pNode);

						SaveAmbstrEnding(pNode, pOneChar2);
						SaveAmbstrEnding(pNode, m_pNewBuffer);
						SaveAmbstrEnding(pNode, m_pOldBuffer);
					}
				}
				else
				{
					GoErrorFunction(pNode);
				}
			}
			else
			{
				GoErrorFunction(pNode);
			}
		}
	}
	else
	{
		GoMAR2_5(pNode, nStart, mAlkong, mSecondAlkong);
	}

	CHAR_DELETE(pMiddleString);
	CHAR_DELETE(pMiddleString2);
	CHAR_DELETE(pNextStartCountChar);
	CHAR_DELETE(pThisStartCountChar);
	CHAR_DELETE(pThisEndCountChar);
	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pOneChar2);
	CHAR_DELETE(pJongChar);
	CHAR_DELETE(pJongChar2);
	CHAR_DELETE(pMinusTwoString);
	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pAddTwoString2);
}

void CNAHangulAnaly::GoMAR2_5(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong)
{
	char* pOriginalString = NULL;
	char* pMiddleString = NULL;
	char* pThisStartCountChar = NULL;

	pOriginalString = pNode->pHToken->strSource;

	if (m_pNewBuffer == NULL)
	{
		GoMAR3(pNode, nStart, mAlkong, mSecondAlkong);
	}
	else if (IsSameWordPos(pNode, mSecondAlkong))
	{
		AddOldNewBuffer(pNode);

		CHAR_DELETE(m_pNewBuffer);

		ChangePosAnyAlkong(pNode, nStart, mAlkong, mSecondAlkong, CHANGE_SECOND_ALKONG);
		pMiddleString = GetMiddleCountChar(pOriginalString, nStart, mAlkong, CALCULATE_STOP, CALCULATE_MINUS);
		pThisStartCountChar = GetStartCountChar(pOriginalString, nStart);
		//if (IsExistWildEndingDic(pMiddleString) > 0 && strcmp(pMiddleString, "") != 0)
		if (IsExistWildEndingDic(pMiddleString) == true && strcmp(pMiddleString, "") != 0)
		{
			mSecondAlkong = nStart;
			GoMAR2_3(pNode, nStart, mAlkong, mSecondAlkong);
		}
		else if (IsExistStemDic(pThisStartCountChar))
		{
			InsertStemString(pNode, pThisStartCountChar);
			SaveHTokenStem(pNode);
			InsertEndingString(pNode, m_pOldBuffer);
			MakeNewEndingFeature(pNode);
			SaveHTokenEndingOldFeature(pNode);
		}
		else
		{
			GoMAR3(pNode, nStart, mAlkong, mSecondAlkong);
		}
	}
	else
	{
		AddOldNewBuffer(pNode);

		CHAR_DELETE(m_pNewBuffer);

		ChangePosAnyAlkong(pNode, nStart, mAlkong, mSecondAlkong, CHANGE_FIRST_ALKONG);
		nStart -= 2;
		ChangePosAnyAlkong(pNode, nStart, mAlkong, mSecondAlkong, CHANGE_SECOND_ALKONG);
		pMiddleString = GetMiddleCountChar(pOriginalString, nStart, mAlkong, CALCULATE_STOP, CALCULATE_MINUS);
		if (/*IsWildEndingDic*/IsExistWildEndingDic(pMiddleString) && strcmp(pMiddleString, "") != 0)
		{
			mSecondAlkong = nStart;
			GoMAR2_3(pNode, nStart, mAlkong, mSecondAlkong);
		}
		else
		{
			if (m_pOldBuffer != NULL)
			{
				InsertEndingString(pNode, m_pOldBuffer);
				MakeNewEndingFeature(pNode);
				SaveHTokenEndingOldFeature(pNode);
			}
			GoMAR3(pNode, nStart, mAlkong, mSecondAlkong);
		}
	}

	CHAR_DELETE(pMiddleString);
	CHAR_DELETE(pThisStartCountChar);
}


////////////////////////////////////////////////////////
//MAR3
////////////////////////////////////////////////////////
void CNAHangulAnaly::GoMAR3(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong)
{
	char* pOriginalString = NULL;

	char* pOneChar = NULL;
	char* pNextStartCountChar = NULL;
	char* pAddTwoString = NULL;
	char* pAddTwoString2 = NULL;
	char* pNextOneChar = NULL;
	char* pLastOneChar = NULL;

	int nCount = 0;
	int nLen = 0;

	pOriginalString = pNode->pHToken->strSource;
	pOneChar = GetOneChar(pOriginalString, nStart, nStart);
	if (m_bOneCharMask == false && strcmp(pOneChar, "럼") == 0)
	{
		SetOneWordMarsk();
		GoMAR1_2(pNode, nStart, mAlkong, mSecondAlkong);
	}
	else if (strcmp(pOneChar, "여") == 0)
	{
		nCount = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_MINUS);
		pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
		pAddTwoString = AddTwoStr(pNextStartCountChar, "이");
		pNextOneChar = GetOneChar(pOriginalString, nCount, nCount);
		pLastOneChar = GetOneChar(pOriginalString, strlen(pOriginalString), strlen(pOriginalString));
		if (IsExistStemDic(pAddTwoString) && (IsExistFeature(pAddTwoString, STEM_DIC, 1) == true ||
			IsExistFeature(pAddTwoString, STEM_DIC, 7) == true))
		{
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStem(pNode);
			pAddTwoString2 = AddTwoStr("어", m_pOldBuffer);
			InsertEndingString(pNode, pAddTwoString2);
			if (IsExistEndDic(pAddTwoString2))
			{
				SaveHTokenEnding(pNode);
			}
			else
			{
				MakeNewEndingFeature(pNode);
				AddEndingFeature(pNode, "어");
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, "어");
				SaveAmbstrEnding(pNode, m_pOldBuffer);
			}
		}
		else if (IsExistStemDic(pNextStartCountChar))
		{
			if (strcmp(pNextOneChar, "하") == 0 && IsExistFeature(pNextStartCountChar, STEM_DIC, 7))
			{
				InsertStemString(pNode, pNextStartCountChar);
				SaveHTokenStem(pNode);

				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr(pOneChar, m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, pOneChar);
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, pOneChar);
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else if (strcmp(pOneChar, pLastOneChar) == 0)
			{
				InsertStemString(pNode, pNextStartCountChar);
				SaveHTokenStem(pNode);

				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr(pOneChar, m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, pOneChar);
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, pOneChar);
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else
			{
				GoMAR1_2(pNode, nStart, mAlkong, mSecondAlkong);
			}
		}
		else
		{
			GoMAR1_2(pNode, nStart, mAlkong, mSecondAlkong);
		}
	}
	else
	{
		GoMAR3_1(pNode, nStart, mAlkong, mSecondAlkong);
	}
	/*
	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pNextStartCountChar);
	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pAddTwoString2);
	*/
	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pNextStartCountChar);
	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pAddTwoString2);
	CHAR_DELETE(pNextOneChar);
	CHAR_DELETE(pLastOneChar);
}

void CNAHangulAnaly::GoMAR3_1(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong)
{
	char* pOriginalString = NULL;

	char* pOneChar = NULL;
	char* pLastOneChar = NULL;
	char* pNextOneChar = NULL;
	char* pMiddleString = NULL;
	char* pNextStartCountChar = NULL;
	char* pThisStartCountChar = NULL;
	char* pAddTwoString = NULL;
	char* pAddTwoString2 = NULL;


	int nCount = 0;
	int nLen = 0;
	pOriginalString = pNode->pHToken->strSource;

	pOneChar = GetOneChar(pOriginalString, nStart, nStart);
	pLastOneChar = GetOneChar(pOriginalString, strlen(pOriginalString), strlen(pOriginalString));

	if (m_bOneCharMask == false && (strcmp(pOneChar, "다") == 0 ||
		strcmp(pOneChar, "야") == 0 ||
		strcmp(pOneChar, "여") == 0))
	{
		nCount = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_MINUS);
		pMiddleString = GetMiddleCountChar(pOriginalString, nStart, nStart, CALCULATE_STOP, CALCULATE_PLUS);
		pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
		if (strcmp(pOneChar, "다") == 0 && IsExistEndDic(pMiddleString) && IsExistStemDic(pNextStartCountChar))
		{
			InsertStemString(pNode, pNextStartCountChar);
			SaveHTokenStem(pNode);
			pAddTwoString = AddTwoStr("다", m_pNewBuffer);
			pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
			InsertEndingString(pNode, pAddTwoString2);
			if (IsExistEndDic(pAddTwoString2))
			{
				SaveHTokenEnding(pNode);
			}
			else
			{
				MakeNewEndingFeature(pNode);
				AddEndingFeature(pNode, "다");
				SaveHTokenEndingNewFeature(pNode);
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, "다");
				SaveAmbstrEnding(pNode, m_pNewBuffer);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
			}
		}
		else if (ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA13))
		{
			pOriginalString = pNode->pHToken->strSource;
			pThisStartCountChar = GetStartCountChar(pOriginalString, nStart);
			InsertStemString(pNode, pThisStartCountChar);
			SaveHTokenStem(pNode);
			pAddTwoString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
			InsertEndingString(pNode, pAddTwoString);
			if (IsExistEndDic(pAddTwoString))
			{
				SaveHTokenEnding(pNode);
			}
			else
			{
				MakeNewEndingFeature(pNode);
				SaveHTokenEndingNewFeature(pNode);
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, m_pNewBuffer);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
			}
		}
		else
		{
			ReplaceHangulJongsungNull(pNode, nStart);
			GoMAR1_2(pNode, nStart, mAlkong, mSecondAlkong);

		}
	}
	else if (m_bOneCharMask == false && (strcmp(pOneChar, "이") == 0 || strcmp(pOneChar, "가") == 0))
	{
		if (strcmp(pOneChar, pLastOneChar) == 0)
		{
			GoMAR1_2(pNode, nStart, mAlkong, mSecondAlkong);
		}
		else
		{
			GoIMR1(pNode, nStart, mAlkong, mSecondAlkong);
		}
	}
	else if (m_bOneCharMask == false && strcmp(pOneChar, "터") == 0)
	{
		nCount = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_MINUS);
		pNextOneChar = GetOneChar(pOriginalString, nCount, nCount);
		if (strcmp(pNextOneChar, "부") == 0)
		{
			nCount = ReturnNextPrevCount(pOriginalString, nStart, 2, CALCULATE_MINUS);
			pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
			if (IsExistStemDic(pNextStartCountChar))
			{
				InsertStemString(pNode, pNextStartCountChar);
				SaveHTokenStem(pNode);
				pMiddleString = GetMiddleCountChar(pOriginalString, nStart, nStart, CALCULATE_MINUS, CALCULATE_STOP);
				pAddTwoString = AddTwoStr(pMiddleString, m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);

				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else
			{
				InsertArrayFeatureOld("부터");
				pAddTwoString = AddTwoStr("부터", m_pOldBuffer);

				CHAR_DELETE(m_pOldBuffer);

				nLen = strlen(pAddTwoString);
				m_pOldBuffer = new char[nLen + 1];
				memset(m_pOldBuffer, 0x00, nLen + 1);
				strcpy(m_pOldBuffer, pAddTwoString);
				MoveLeftWordCount(pOriginalString, nStart);
				mAlkong = nStart;
				MoveLeftWordCount(pOriginalString, nStart);
				InitOneWordMarsk();
				if (nStart < 0)
				{
					GoErrorFunction(pNode);
				}
				else
				{
					GoMAR2(pNode, nStart, mAlkong, mSecondAlkong);
				}
			}
		}
		else
		{
			GoIMR1(pNode, nStart, mAlkong, mSecondAlkong);
		}
	}
	else
	{
		GoMAR3_2(pNode, nStart, mAlkong, mSecondAlkong);
	}

	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pLastOneChar);
	CHAR_DELETE(pNextOneChar);
	CHAR_DELETE(pMiddleString);
	CHAR_DELETE(pNextStartCountChar);
	CHAR_DELETE(pThisStartCountChar);
	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pAddTwoString2);
}

void CNAHangulAnaly::GoMAR3_2(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong)
{
	char* pOriginalString = NULL;

	char* pOneChar = NULL;
	char* pNextOneChar = NULL;
	char* pJongChar = NULL;
	char* pNextJongChar = NULL;
	char* pThisStartCountChar = NULL;
	char* pNextStartCountChar = NULL;
	char* pAddTwoString = NULL;
	char* pAddTwoString2 = NULL;

	int nCount = 0;
	int nLen = 0;
	pOriginalString = pNode->pHToken->strSource;

	pOneChar = GetOneChar(pOriginalString, nStart, nStart);
	pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);
	if (strcmp(pJongChar, "ㄴ") == 0)
	{
		if (ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA01))
		{
			pOriginalString = pNode->pHToken->strSource;
			pThisStartCountChar = GetStartCountChar(pOriginalString, nStart);
			char Feature_Stem[236];
			char IS_CEHCK_FEATURE = '1';
			bool isExist;
			memset(Feature_Stem, 0x00, sizeof(Feature_Stem));
			isExist = IsExistStemDic(pThisStartCountChar, Feature_Stem);
			if (Feature_Stem[66] == IS_CEHCK_FEATURE)
			{
				InsertStemString(pNode, pThisStartCountChar);
				SaveHTokenStem(pNode);
				InsertArrayFeatureOld("ㄴ");
				pAddTwoString = AddTwoStr("ㄴ", m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else
			{
				ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA01);
				GoIMR1(pNode, nStart, mAlkong, mSecondAlkong);
			}
		}
		else
		{
			ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA01);
			GoIMR1(pNode, nStart, mAlkong, mSecondAlkong);
		}
	}
	else if (strcmp(pJongChar, "ㅂ") == 0)
	{
		if (ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA01))
		{
			pOriginalString = pNode->pHToken->strSource;
			pThisStartCountChar = GetStartCountChar(pOriginalString, nStart);
			char Feature_Stem[236];
			char IS_CEHCK_FEATURE = '1';
			bool isExist;
			memset(Feature_Stem, 0x00, sizeof(Feature_Stem));
			isExist = IsExistStemDic(pThisStartCountChar, Feature_Stem);
			if (Feature_Stem[66] == IS_CEHCK_FEATURE)
			{
				InsertStemString(pNode, pThisStartCountChar);
				SaveHTokenStem(pNode);
				InsertArrayFeatureOld("ㅂ");
				pAddTwoString = AddTwoStr("ㅂ", m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else
			{
				ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA05);
				GoErrorFunction(pNode);
			}
		}
		else
		{
			ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA05);
			GoErrorFunction(pNode);
		}
	}
	else if (m_bOneCharMask == false && strcmp(pOneChar, "려") == 0)
	{
		nCount = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_MINUS);
		pNextOneChar = GetOneChar(pOriginalString, nCount, nCount);
		pNextJongChar = GetChojungJongsungChar(pNextOneChar, CHAR_JONGSUNG);
		pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
		if ((strcmp(pNextOneChar, "알") != 0 && strcmp(pNextJongChar, "ㄹ") == 0) ||
			strcmp(pNextJongChar, "ㅀ") == 0 || strcmp(pNextOneChar, "꾸") == 0 ||
			strcmp(pNextOneChar, "어") == 0 || strcmp(pNextOneChar, "그") == 0 ||
			strcmp(pNextOneChar, "내") == 0 || strcmp(pNextOneChar, "때") == 0 ||
			strcmp(pNextOneChar, "추") == 0 || strcmp(pNextOneChar, "드") == 0 ||
			strcmp(pNextOneChar, "거") == 0 || strcmp(pNextOneChar, "다") == 0 ||
			strcmp(pNextOneChar, "뿌") == 0 || strcmp(pNextOneChar, "느") == 0 ||
			strcmp(pNextOneChar, "버") == 0 || strcmp(pNextOneChar, "꺼") == 0 ||
			strcmp(pNextOneChar, "부") == 0)
		{
			pAddTwoString = AddTwoStr(pNextStartCountChar, "리");
			if (IsExistStemDic(pAddTwoString))
			{
				InsertStemString(pNode, pAddTwoString);
				SaveHTokenStem(pNode);

				CHAR_DELETE(pAddTwoString);
				CHAR_DELETE(pAddTwoString2);

				// Edited by KKH 2004-01-12
				if ((pNode->pHToken->subEnding->strFeature != NULL &&
					pNode->pHToken->subEnding->strFeature[E_Pst] == '0') ||
					pNode->pHToken->subEnding->strFeature == NULL)
				{
					InsertArrayFeatureOld("어");
					pAddTwoString = AddTwoStr("어", m_pNewBuffer);
					pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
					InsertEndingString(pNode, pAddTwoString2);
				}
				// Edited by KKH 2004-01-12
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else
			{
				SetOneWordMarsk();
				GoMAR2(pNode, nStart, mAlkong, mSecondAlkong);
			}
		}
		else if (IsExistStemDic(pNextStartCountChar))
		{
			char Feature_Stem[236];
			char IS_CEHCK_FEATURE = '1';
			bool isExist;
			memset(Feature_Stem, 0x00, sizeof(Feature_Stem));
			isExist = IsExistStemDic(pNextStartCountChar, Feature_Stem);
			if (Feature_Stem[4] != IS_CEHCK_FEATURE)
			{
				InsertStemString(pNode, pNextStartCountChar);
				SaveHTokenStem(pNode);
				InsertArrayFeatureOld("려");
				pAddTwoString = AddTwoStr("려", m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else
			{
				SetOneWordMarsk();
				GoMAR1_2(pNode, nStart, mAlkong, mSecondAlkong);
			}
		}
		else
		{
			SetOneWordMarsk();
			GoMAR1_2(pNode, nStart, mAlkong, mSecondAlkong);
		}
	}
	else
	{
		GoMAR3_3(pNode, nStart, mAlkong, mSecondAlkong);
	}

	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pNextOneChar);
	CHAR_DELETE(pJongChar);
	CHAR_DELETE(pNextJongChar);
	CHAR_DELETE(pThisStartCountChar);
	CHAR_DELETE(pNextStartCountChar);
	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pAddTwoString2);
}
void CNAHangulAnaly::GoMAR3_3(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong)
{
	char* pOriginalString = NULL;

	char* pOneChar = NULL;
	char* pNextOneChar = NULL;
	char* pPrevOneChar = NULL;
	char* pJongChar = NULL;
	char* pThisStartCountChar = NULL;
	char* pNextStartCountChar = NULL;
	char* pAddTwoString = NULL;
	char* pAddTwoString2 = NULL;

	int nCount = 0;
	int nLen = 0;
	pOriginalString = pNode->pHToken->strSource;

	pOneChar = GetOneChar(pOriginalString, nStart, nStart);
	pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);
	if (strcmp(pOneChar, "큼") == 0)
	{
		nCount = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_MINUS);
		pNextOneChar = GetOneChar(pOriginalString, nCount, nCount);
		nCount = ReturnNextPrevCount(pOriginalString, nStart, 2, CALCULATE_MINUS);
		pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
		if (strcmp(pNextOneChar, "만") == 0 && IsExistStemDic(pNextStartCountChar))
		{
			InsertStemString(pNode, pNextStartCountChar);
			SaveHTokenStem(pNode);
			pAddTwoString = AddTwoStr("만큼", m_pNewBuffer);
			pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
			InsertEndingString(pNode, pAddTwoString2);
			if (IsExistEndDic(pAddTwoString2))
			{
				SaveHTokenEnding(pNode);
			}
			else
			{
				MakeNewEndingFeature(pNode);
				AddEndingFeature(pNode, "만큼");
				SaveHTokenEndingNewFeature(pNode);
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, "만큼");
				SaveAmbstrEnding(pNode, m_pNewBuffer);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
			}
		}
		else
		{
			SetOneWordMarsk();
			GoMAR2(pNode, nStart, mAlkong, mSecondAlkong);
		}
	}
	else if (strcmp(pJongChar, "") == 0)
	{
		nCount = ReturnNextPrevCount(pOriginalString, nStart, 1, CALCULATE_PLUS);
		pPrevOneChar = GetOneChar(pOriginalString, nCount, nCount);
		if (strcmp(pPrevOneChar, "는") == 0 ||
			strcmp(pPrevOneChar, "시") == 0 ||
			strcmp(pPrevOneChar, "셔") == 0 ||
			strcmp(pPrevOneChar, "셨") == 0 ||
			strcmp(pPrevOneChar, "오") == 0 ||
			strcmp(pPrevOneChar, "온") == 0)
		{
			if (ReplaceHgngulTokenSource(pNode, nStart, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03))
			{
				pOriginalString = pNode->pHToken->strSource;
				char Feature_Stem[236];
				char IS_CEHCK_FEATURE = '1';
				bool isExist;
				memset(Feature_Stem, 0x00, sizeof(Feature_Stem));
				pThisStartCountChar = GetStartCountChar(pOriginalString, nStart);
				isExist = IsExistStemDic(pThisStartCountChar, Feature_Stem);
				if (Feature_Stem[64] == IS_CEHCK_FEATURE)
				{
					InsertStemString(pNode, pThisStartCountChar);
					SaveHTokenStem(pNode);
					pAddTwoString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
					InsertEndingString(pNode, pAddTwoString);
					if (IsExistEndDic(pAddTwoString))
					{
						SaveHTokenEnding(pNode);
					}
					else
					{
						MakeNewEndingFeature(pNode);
						SaveHTokenEndingNewFeature(pNode);
						SaveHTokenEndingOldFeature(pNode);
						SaveAmbstrEnding(pNode, m_pNewBuffer);
						SaveAmbstrEnding(pNode, m_pOldBuffer);
					}
				}
				else
				{
					ReplaceHangulJongsungNull(pNode, nStart);
					GoErrorFunction(pNode);
				}
			}
			else
			{
				ReplaceHangulJongsungNull(pNode, nStart);
				GoErrorFunction(pNode);
			}
		}
		else
		{
			ReplaceHangulJongsungNull(pNode, nStart);
			GoErrorFunction(pNode);
		}
	}
	else
	{
		pThisStartCountChar = GetStartCountChar(pOriginalString, nStart);
		InsertStemString(pNode, pThisStartCountChar);
		SaveHTokenStem(pNode);
		pAddTwoString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
		InsertEndingString(pNode, pAddTwoString);
		if (IsExistEndDic(pAddTwoString))
		{
			SaveHTokenEnding(pNode);
		}
		else
		{
			MakeNewEndingFeature(pNode);
			SaveHTokenEndingNewFeature(pNode);
			SaveHTokenEndingOldFeature(pNode);
			SaveAmbstrEnding(pNode, m_pNewBuffer);
			SaveAmbstrEnding(pNode, m_pOldBuffer);
		}
		GoErrorFunction(pNode);
	}

	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pNextOneChar);
	CHAR_DELETE(pPrevOneChar);
	CHAR_DELETE(pJongChar);
	CHAR_DELETE(pThisStartCountChar);
	CHAR_DELETE(pNextStartCountChar);
	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pAddTwoString2);
}

////////////////////////////////////////////////////////////////
//IMR1
/////////////////////////////////////////////////////////////////
void CNAHangulAnaly::GoIMR1(H_NODE *pNode, int &nFirstOlive, int &mSecondOlive, int &mSecondAlkong)
{
	char* pOriginalString = NULL;

	char* pOneChar = NULL;
	char* pNextStartCountChar = NULL;
	char* pAddTwoString = NULL;
	char* pAddTwoString2 = NULL;

	int nCount = 0;
	pOriginalString = pNode->pHToken->strSource;

	pOneChar = GetOneChar(pOriginalString, nFirstOlive, nFirstOlive);
	if (strcmp(pOneChar, "케") == 0)
	{
		nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_MINUS);
		pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
		pAddTwoString = AddTwoStr(pNextStartCountChar, "하");
		if (IsExistStemDic(pAddTwoString) && (strcmp(pAddTwoString, "하") != 0))
		{
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStem(pNode);
			DeleteHTokenStem(pNode, 4);
			DeleteHTokenStem(pNode, 1);
			AddHTokenStem(pNode, 7);
			AddHTokenStem(pNode, 2);
			if (m_pOldBuffer == NULL)
			{
				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr("게", m_pNewBuffer);
				InsertEndingString(pNode, pAddTwoString);
				if (IsExistEndDic(pAddTwoString))
				{
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, 3);
					AddHTokenEnding(pNode, 1);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "게");
					SaveHTokenEndingNewFeature(pNode);
					SaveAmbstrEnding(pNode, "게");
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					AddHTokenEnding(pNode, 3);
					AddHTokenEnding(pNode, 1);
				}
			}
			else
			{
				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr("게", m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, 3);
					AddHTokenEnding(pNode, 1);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "게");
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, "게");
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
					AddHTokenEnding(pNode, 3);
					AddHTokenEnding(pNode, 1);
				}
			}
		}
		else if (IsExistStemDic(pNextStartCountChar) &&
			IsExistFeature(pNextStartCountChar, STEM_DIC, S_Conc) == false)
		{
			CHAR_DELETE(pAddTwoString);

			pAddTwoString = AddTwoStr(pNextStartCountChar, "하");
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStem(pNode);
			SaveHTokenStemFeature(pNode, pNextStartCountChar);
			DeleteHTokenStem(pNode, 4);//n
			DeleteHTokenStem(pNode, 1);//amb
			AddHTokenStem(pNode, 7);//v
			AddHTokenStem(pNode, 2);//lex
			if (m_pOldBuffer == NULL)
			{
				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr("게", m_pNewBuffer);
				InsertEndingString(pNode, pAddTwoString);
				if (IsExistEndDic(pAddTwoString))
				{
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, 3);
					AddHTokenEnding(pNode, 1);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "게");
					SaveHTokenEndingNewFeature(pNode);
					SaveAmbstrEnding(pNode, "게");
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					AddHTokenEnding(pNode, 3);
					AddHTokenEnding(pNode, 1);
				}
			}
			else
			{
				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr("게", m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, 3);
					AddHTokenEnding(pNode, 1);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "게");
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, "게");
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
					AddHTokenEnding(pNode, 3);
					AddHTokenEnding(pNode, 1);
				}
			}
		}
		else
		{
			GoErrorFunction(pNode);
		}
	}
	else
	{
		GoIMR1_1(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
	}

	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pNextStartCountChar);
	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pAddTwoString2);
}

void CNAHangulAnaly::GoIMR1_1(H_NODE *pNode, int &nFirstOlive, int &mSecondOlive, int &mSecondAlkong)
{
	char* pOriginalString = NULL;

	char* pOneChar = NULL;
	char* pPrevOneChar = NULL;
	char* pNextStartCountChar = NULL;
	char* pAddTwoString = NULL;
	char* pAddTwoString2 = NULL;

	int nCount = 0;
	pOriginalString = pNode->pHToken->strSource;

	pOneChar = GetOneChar(pOriginalString, nFirstOlive, nFirstOlive);
	if (strcmp(pOneChar, "치") == 0)
	{
		nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_MINUS);
		pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
		nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_PLUS);

		if (nCount > nFirstOlive)
		{
			pPrevOneChar = GetOneChar(pOriginalString, nCount, nCount);
		}
		else
		{
			pPrevOneChar = new char[2];
			memset(pPrevOneChar, 0x00, 2);
		}

		pAddTwoString = AddTwoStr(pNextStartCountChar, "하");
		if ((strcmp(pPrevOneChar, "고") == 0 && IsExistFeature(pNextStartCountChar, STEM_DIC, 4) == true) ||
			(strcmp(pPrevOneChar, "고") == 0 && IsExistFeature(pNextStartCountChar, STEM_DIC, 1) == true))
		{
			SetOneWordMarsk();
			GoMAR1_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
		}
		else if (IsExistStemDic(pAddTwoString) &&
			(strcmp(pPrevOneChar, "도") == 0 || pPrevOneChar == NULL || strlen(pPrevOneChar) == 0))
		{
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStem(pNode);
			DeleteHTokenStem(pNode, 4);//n
			DeleteHTokenStem(pNode, 1);//amb
			AddHTokenStem(pNode, 7);//v
			AddHTokenStem(pNode, 2);//lex
			if (m_pOldBuffer == NULL)
			{
				MakeNewEndingFeature(pNode);
				AddEndingFeature(pNode, "지");
				AddHTokenEnding(pNode, 3);
				AddHTokenEnding(pNode, 1);

				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr("지", m_pNewBuffer);
				InsertEndingString(pNode, pAddTwoString);
				if (IsExistEndDic(pAddTwoString))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "지");
					SaveHTokenEndingNewFeature(pNode);
					SaveAmbstrEnding(pNode, "지");
					SaveAmbstrEnding(pNode, m_pNewBuffer);
				}
			}
			else
			{
				MakeNewEndingFeature(pNode);
				AddEndingFeature(pNode, "지");
				AddHTokenEnding(pNode, 1);

				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr("지", m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, "지");
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
		}
		else if (IsExistStemDic(pNextStartCountChar) &&
			IsExistFeature(pNextStartCountChar, STEM_DIC, S_Conc) == false && (strcmp(pPrevOneChar, "도") == 0 || pPrevOneChar == NULL))
		{
			CHAR_DELETE(pAddTwoString);

			pAddTwoString = AddTwoStr(pNextStartCountChar, "하");
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStem(pNode);
			SaveHTokenStemFeature(pNode, pNextStartCountChar);
			DeleteHTokenStem(pNode, 4);//n
			DeleteHTokenStem(pNode, 1);//amb
			AddHTokenStem(pNode, 7);//v
			AddHTokenStem(pNode, 2);//lex
			if (m_pOldBuffer == NULL)
			{
				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr("지", m_pNewBuffer);
				InsertEndingString(pNode, pAddTwoString);
				if (IsExistEndDic(pAddTwoString))
				{
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, 3);
					AddHTokenEnding(pNode, 1);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "지");
					SaveHTokenEndingNewFeature(pNode);
					SaveAmbstrEnding(pNode, "지");
					SaveAmbstrEnding(pNode, m_pOldBuffer);
					AddHTokenEnding(pNode, 3);
					AddHTokenEnding(pNode, 1);
				}
			}
			else
			{
				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr("지", m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, 3);
					AddHTokenEnding(pNode, 1);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "지");
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, "지");
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
					AddHTokenEnding(pNode, 3);
					AddHTokenEnding(pNode, 1);
				}
			}
		}
		else
		{
			GoErrorFunction(pNode);
		}
	}
	else
	{
		GoIMR1_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
	}

	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pPrevOneChar);
	CHAR_DELETE(pNextStartCountChar);
	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pAddTwoString2);
}

void CNAHangulAnaly::GoIMR1_2(H_NODE *pNode, int &nFirstOlive, int &mSecondOlive, int &mSecondAlkong)
{
	char* pOriginalString = NULL;

	char* pOneChar = NULL;
	char* pNextOneChar = NULL;
	char* pNextStartCountChar = NULL;
	char* pAddTwoString = NULL;
	char* pAddTwoString2 = NULL;

	int nCount = 0;
	pOriginalString = pNode->pHToken->strSource;

	pOneChar = GetOneChar(pOriginalString, nFirstOlive, nFirstOlive);
	if (strcmp(pOneChar, "록") == 0)
	{
		nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_MINUS);
		pNextOneChar = GetOneChar(pOriginalString, nCount, nCount);
		if (strcmp(pNextOneChar, "토") == 0)
		{
			nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 2, CALCULATE_MINUS);
			pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
			pAddTwoString = AddTwoStr(pNextStartCountChar, "하");
			if (IsExistStemDic(pAddTwoString))
			{
				InsertStemString(pNode, pAddTwoString);
				SaveHTokenStem(pNode);
				DeleteHTokenStem(pNode, 4);//n
				DeleteHTokenStem(pNode, 1);//amb
				AddHTokenStem(pNode, 7);//v
				AddHTokenStem(pNode, 2);//lex
				if (m_pOldBuffer == NULL)
				{
					CHAR_DELETE(pAddTwoString);

					pAddTwoString = AddTwoStr("도록", m_pNewBuffer);
					InsertEndingString(pNode, pAddTwoString);
					if (IsExistEndDic(pAddTwoString))
					{
						SaveHTokenEnding(pNode);
						AddHTokenEnding(pNode, 3);
						AddHTokenEnding(pNode, 1);
						AddHTokenEnding(pNode, 28);
						AddHTokenEnding(pNode, 36);
					}
					else
					{
						MakeNewEndingFeature(pNode);
						AddEndingFeature(pNode, "도록");
						SaveHTokenEndingNewFeature(pNode);
						SaveAmbstrEnding(pNode, "도록");
						SaveAmbstrEnding(pNode, m_pNewBuffer);
						AddHTokenEnding(pNode, 3);
						AddHTokenEnding(pNode, 1);
						AddHTokenEnding(pNode, 28);
						AddHTokenEnding(pNode, 36);
					}
				}
				else
				{
					CHAR_DELETE(pAddTwoString);

					pAddTwoString = AddTwoStr("도록", m_pNewBuffer);
					pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
					InsertEndingString(pNode, pAddTwoString2);
					if (IsExistEndDic(pAddTwoString2))
					{
						SaveHTokenEnding(pNode);
						AddHTokenEnding(pNode, 3);
						AddHTokenEnding(pNode, 1);
						AddHTokenEnding(pNode, 28);
						AddHTokenEnding(pNode, 36);
					}
					else
					{
						MakeNewEndingFeature(pNode);
						AddEndingFeature(pNode, "도록");
						SaveHTokenEndingNewFeature(pNode);
						SaveHTokenEndingOldFeature(pNode);
						SaveAmbstrEnding(pNode, "도록");
						SaveAmbstrEnding(pNode, m_pNewBuffer);
						SaveAmbstrEnding(pNode, m_pOldBuffer);
						AddHTokenEnding(pNode, 3);
						AddHTokenEnding(pNode, 1);
						AddHTokenEnding(pNode, 28);
						AddHTokenEnding(pNode, 36);
					}
				}
			}
			else if (IsExistStemDic(pNextStartCountChar) &&
				IsExistFeature(pNextStartCountChar, STEM_DIC, S_Conc) == false)
			{
				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr(pNextStartCountChar, "하");
				InsertStemString(pNode, pAddTwoString);
				SaveHTokenStem(pNode);
				DeleteHTokenStem(pNode, 4);//n
				DeleteHTokenStem(pNode, 1);//amb
				AddHTokenStem(pNode, 7);//v
				AddHTokenStem(pNode, 2);//lex

				if (m_pOldBuffer == NULL)
				{
					CHAR_DELETE(pAddTwoString);

					pAddTwoString = AddTwoStr("도록", m_pNewBuffer);
					InsertEndingString(pNode, pAddTwoString);
					if (IsExistEndDic(pAddTwoString))
					{
						SaveHTokenEnding(pNode);
						AddHTokenEnding(pNode, 3);
						AddHTokenEnding(pNode, 1);
						AddHTokenEnding(pNode, 28);
						AddHTokenEnding(pNode, 36);
					}
					else
					{
						MakeNewEndingFeature(pNode);
						AddEndingFeature(pNode, "도록");
						SaveHTokenEndingNewFeature(pNode);
						SaveAmbstrEnding(pNode, "도록");
						SaveAmbstrEnding(pNode, m_pNewBuffer);
						AddHTokenEnding(pNode, 3);
						AddHTokenEnding(pNode, 1);
						AddHTokenEnding(pNode, 28);
						AddHTokenEnding(pNode, 36);
					}
				}
				else
				{
					CHAR_DELETE(pAddTwoString);

					pAddTwoString = AddTwoStr("도록", m_pNewBuffer);
					pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
					InsertEndingString(pNode, pAddTwoString2);
					if (IsExistEndDic(pAddTwoString2))
					{
						SaveHTokenEnding(pNode);
						AddHTokenEnding(pNode, 3);
						AddHTokenEnding(pNode, 1);
						AddHTokenEnding(pNode, 28);
						AddHTokenEnding(pNode, 36);
					}
					else
					{
						MakeNewEndingFeature(pNode);
						AddEndingFeature(pNode, "도록");
						SaveHTokenEndingNewFeature(pNode);
						SaveHTokenEndingOldFeature(pNode);
						SaveAmbstrEnding(pNode, "도록");
						SaveAmbstrEnding(pNode, m_pNewBuffer);
						SaveAmbstrEnding(pNode, m_pOldBuffer);
						AddHTokenEnding(pNode, 3);
						AddHTokenEnding(pNode, 1);
						AddHTokenEnding(pNode, 28);
						AddHTokenEnding(pNode, 36);
					}
				}
			}
			else
			{
				GoErrorFunction(pNode);
			}
		}
		else if (strcmp(pNextOneChar, "도") == 0)
		{
			nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 2, CALCULATE_MINUS);

			CHAR_DELETE(pNextOneChar);

			pNextOneChar = GetOneChar(pOriginalString, nCount, nCount);
			pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
			if (IsExistStemDic(pNextStartCountChar))
			{
				InsertStemString(pNode, pNextStartCountChar);
				SaveHTokenStem(pNode);

				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr("도록", m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, 3);
					AddHTokenEnding(pNode, 1);
					AddHTokenEnding(pNode, 28);
					AddHTokenEnding(pNode, 36);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "도록");
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, "도록");
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
					AddHTokenEnding(pNode, 3);
					AddHTokenEnding(pNode, 1);
					AddHTokenEnding(pNode, 28);
					AddHTokenEnding(pNode, 36);
				}
			}
			else if (strcmp(pNextOneChar, "하") == 0 || strcmp(pNextOneChar, "되") == 0)
			{
				nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 2, CALCULATE_MINUS);
				nCount = ReturnNextPrevCount(pOriginalString, nCount, 1, CALCULATE_MINUS);

				CHAR_DELETE(pNextStartCountChar);

				pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
				pAddTwoString = AddTwoStr(pNextStartCountChar, pNextOneChar);
				InsertStemString(pNode, pAddTwoString);
				SaveHTokenStem(pNode);
				SaveHTokenStemFeature(pNode, pNextStartCountChar);
				DeleteHTokenStem(pNode, 4);//n
				DeleteHTokenStem(pNode, 1);//amb
				AddHTokenStem(pNode, 7);//v
				AddHTokenStem(pNode, 2);//lex
				if (m_pOldBuffer == NULL)
				{
					CHAR_DELETE(pAddTwoString);

					pAddTwoString = AddTwoStr("도록", m_pNewBuffer);
					InsertEndingString(pNode, pAddTwoString);
					if (IsExistEndDic(pAddTwoString))
					{
						SaveHTokenEnding(pNode);
						AddHTokenEnding(pNode, 3);
						AddHTokenEnding(pNode, 1);
						AddHTokenEnding(pNode, 28);
						AddHTokenEnding(pNode, 36);
					}
					else
					{
						MakeNewEndingFeature(pNode);
						AddEndingFeature(pNode, "도록");
						SaveHTokenEndingNewFeature(pNode);
						SaveAmbstrEnding(pNode, "도록");
						SaveAmbstrEnding(pNode, m_pNewBuffer);
						AddHTokenEnding(pNode, 3);
						AddHTokenEnding(pNode, 1);
						AddHTokenEnding(pNode, 28);
						AddHTokenEnding(pNode, 36);
					}
				}
				else
				{
					CHAR_DELETE(pAddTwoString);

					pAddTwoString = AddTwoStr("도록", m_pNewBuffer);
					pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
					InsertEndingString(pNode, pAddTwoString2);
					if (IsExistEndDic(pAddTwoString2))
					{
						SaveHTokenEnding(pNode);
						AddHTokenEnding(pNode, 3);//[V]
						AddHTokenEnding(pNode, 1);//[Aff]
						AddHTokenEnding(pNode, 28);//[Con]
						AddHTokenEnding(pNode, 36);//[Rsu]
					}
					else
					{
						MakeNewEndingFeature(pNode);
						AddEndingFeature(pNode, "도록");
						SaveHTokenEndingNewFeature(pNode);
						SaveHTokenEndingOldFeature(pNode);
						SaveAmbstrEnding(pNode, "도록");
						SaveAmbstrEnding(pNode, m_pNewBuffer);
						SaveAmbstrEnding(pNode, m_pOldBuffer);
						AddHTokenEnding(pNode, 3);
						AddHTokenEnding(pNode, 1);
						AddHTokenEnding(pNode, 28);
						AddHTokenEnding(pNode, 36);
					}
				}
			}
			else
			{
				SetOneWordMarsk();
				GoMAR1_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
			}
		}
		else
		{
			SetOneWordMarsk();
			GoMAR1_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
		}
	}
	else
	{
		GoIMR1_3(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
	}

	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pNextOneChar);
	CHAR_DELETE(pNextStartCountChar);
	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pAddTwoString2);
}

void CNAHangulAnaly::GoIMR1_3(H_NODE *pNode, int &nFirstOlive, int &mSecondOlive, int &mSecondAlkong)
{
	char* pOriginalString = NULL;

	char* pOneChar = NULL;
	char* pJongChar = NULL;
	char* pNextOneChar = NULL;
	char* pNextStartCountChar = NULL;
	char* pAddTwoString = NULL;
	char* pAddTwoString2 = NULL;
	char* pTmpNextStartCountChar = NULL;

	int nCount = 0;

	pOriginalString = pNode->pHToken->strSource;
	pOneChar = GetOneChar(pOriginalString, nFirstOlive, nFirstOlive);
	pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);
	if (strcmp(pOneChar, "있") == 0)
	{
		nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_MINUS);
		pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
		if (IsExistStemDic(pNextStartCountChar))
		{
			pAddTwoString = AddTwoStr(pNextStartCountChar, "있");
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStem(pNode);
			SaveHTokenStemFeature(pNode, pNextStartCountChar);
			AddHTokenStem(pNode, 7);
			AddHTokenStem(pNode, 9);
			DeleteHTokenStem(pNode, 4);
			pAddTwoString2 = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
			InsertEndingString(pNode, pAddTwoString2);
			if (IsExistEndDic(pAddTwoString2))
			{
				SaveHTokenEnding(pNode);
			}
			else
			{
				MakeNewEndingFeature(pNode);
				SaveHTokenEndingNewFeature(pNode);
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, m_pNewBuffer);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
			}
		}
		else
		{
			GoErrorFunction(pNode);
		}
	}
	else if (strcmp(pOneChar, "없") == 0)
	{
		nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_MINUS);
		pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
		if (IsExistStemDic(pNextStartCountChar))
		{
			pAddTwoString = AddTwoStr(pNextStartCountChar, "없");
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStem(pNode);
			SaveHTokenStemFeature(pNode, pNextStartCountChar);
			AddHTokenStem(pNode, 7);
			AddHTokenStem(pNode, 9);
			AddHTokenStem(pNode, 27);
			DeleteHTokenStem(pNode, 4);
			pAddTwoString2 = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
			InsertEndingString(pNode, pAddTwoString2);
			if (strcmp(pAddTwoString2, "이") == 0)
			{
				MakeNewEndingFeature(pNode);
				AddHTokenEnding(pNode, 1);
				AddHTokenEnding(pNode, 3);
			}
			else if (IsExistEndDic(pAddTwoString2))
			{
				SaveHTokenEnding(pNode);
			}
			else
			{
				MakeNewEndingFeature(pNode);
				SaveHTokenEndingNewFeature(pNode);
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, m_pNewBuffer);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
			}
		}
		else
		{
			GoErrorFunction(pNode);
		}
	}
	else if (strcmp(pOneChar, "셔") == 0)
	{
		nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_MINUS);
		pNextOneChar = GetOneChar(pOriginalString, nCount, nCount);
		if (strcmp(pNextOneChar, "으") == 0)
		{
			nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 2, CALCULATE_MINUS);
			pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
			if (IsExistStemDic(pNextStartCountChar))
			{
				InsertStemString(pNode, pNextStartCountChar);
				SaveHTokenStem(pNode);

				pAddTwoString = AddTwoStr("으시어", m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "으시어");
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, "으시어");
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else
			{
				//ending = "으시어" + m_pNewbuffer + m+_pOldBuffer;
				//n= n+2 -> MAR2
				CHAR_DELETE(pAddTwoString2);

				pAddTwoString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
				pAddTwoString2 = AddTwoStr("으시어", pAddTwoString);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					if (m_pNewBuffer != NULL)
					{
						SaveHTokenEndingNewFeature(pNode);
					}
					if (m_pOldBuffer != NULL)
					{
						SaveHTokenEndingOldFeature(pNode);
					}

					AddEndingFeature(pNode, "으시어");

					SaveAmbstrEnding(pNode, "으시어");
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
				MoveLeftWordCount(pOriginalString, nFirstOlive);
				MoveLeftWordCount(pOriginalString, nFirstOlive);
				InitOneWordMarsk();
				if (nFirstOlive < 0)
				{
					GoErrorFunction(pNode);
				}
				else
				{
					SetOneWordMarsk();
					GoMAR2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
				}
			}
		}
		else
		{
			nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_MINUS);
			pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
			pAddTwoString = AddTwoStr(pNextStartCountChar, "시");
			if (IsExistStemDic(pAddTwoString))
			{
				InsertStemString(pNode, pAddTwoString);
				SaveHTokenStem(pNode);

				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr("어", m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "어");
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, "어");
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else if (IsExistStemDic(pNextStartCountChar))
			{
				InsertStemString(pNode, pNextStartCountChar);
				SaveHTokenStem(pNode);

				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr("시어", m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "시어");
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, "시어");
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else
			{
				CHAR_DELETE(pNextStartCountChar);

				nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 2, CALCULATE_MINUS);
				pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
				nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_MINUS);
				pTmpNextStartCountChar = GetStartCountChar(pOriginalString, nCount);

				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr("시어", m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "시어");
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, "시어");
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
				MoveLeftWordCount(pOriginalString, nFirstOlive);
				InitOneWordMarsk();
				if (nFirstOlive < 0)
				{
					GoErrorFunction(pNode);
				}
				else if ((strcmp(pNextOneChar, "하") == 0 || strcmp(pNextOneChar, "되") == 0 || strcmp(pNextOneChar, "받") == 0)
					&& IsExistStemDic(pNextStartCountChar) == true)
				{
					InsertStemString(pNode, pTmpNextStartCountChar);
					SaveHTokenStem(pNode);
					AddHTokenStem(pNode, 2);
					AddHTokenStem(pNode, 7);
					AddHTokenStem(pNode, 8);

				}
				else
				{
					SetOneWordMarsk();
					GoMAR2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
				}
			}
		}
	}
	else if (strcmp(pOneChar, "키") == 0)
	{
		nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_MINUS);
		pNextOneChar = GetOneChar(pOriginalString, nCount, nCount);
		pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
		pAddTwoString = AddTwoStr(pNextStartCountChar, "하");
		///////////////////////////////////////////
		if (IsExistStemDic(pAddTwoString))
		{
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStem(pNode);
			AddHTokenStem(pNode, 39);
			DeleteHTokenStem(pNode, 7);
			if (m_pOldBuffer == NULL)
			{
				pAddTwoString2 = AddTwoStr("기", m_pNewBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "기");
					SaveHTokenEndingNewFeature(pNode);
					SaveAmbstrEnding(pNode, m_pNewBuffer);
				}
				AddHTokenEnding(pNode, 3);
				AddHTokenEnding(pNode, 1);
			}
			else
			{
				pAddTwoString2 = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
				AddHTokenEnding(pNode, 3);
				DeleteHTokenEnding(pNode, 2);
			}
		}
		else if (IsExistStemDic(pNextStartCountChar) &&
			IsExistFeature(pNextStartCountChar, STEM_DIC, S_N) == true &&
			IsExistFeature(pNextStartCountChar, STEM_DIC, S_Conc) == false &&
			IsExistFeature(pNextStartCountChar, STEM_DIC, S_Whq) == false &&
			IsExistFeature(pNextStartCountChar, STEM_DIC, S_Num) == false)
		{
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStem(pNode);
			SaveHTokenStemFeature(pNode, pNextStartCountChar);
			AddHTokenStem(pNode, 39);
			AddHTokenStem(pNode, 2);
			AddHTokenStem(pNode, S_T077);
			DeleteHTokenStem(pNode, 4);
			if (m_pOldBuffer == NULL)
			{
				pAddTwoString2 = AddTwoStr("기", m_pNewBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "기");
					SaveHTokenEndingNewFeature(pNode);

					SaveAmbstrEnding(pNode, m_pNewBuffer);
				}
				AddHTokenEnding(pNode, 3);
				AddHTokenEnding(pNode, 1);
			}
			else
			{
				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr("기", m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
				AddHTokenEnding(pNode, 3);
				AddHTokenEnding(pNode, 1);
			}
		}
		else if (strcmp(pNextOneChar, "시") == 0)
		{
			CHAR_DELETE(pNextStartCountChar);

			nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 2, CALCULATE_MINUS);
			pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
			if (IsExistStemDic(pNextStartCountChar))
			{
				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr(pNextStartCountChar, "시키");
				InsertStemString(pNode, pAddTwoString);
				SaveHTokenStem(pNode);
				SaveHTokenStemFeature(pNode, pNextStartCountChar);
				AddHTokenStem(pNode, 7);
				AddHTokenStem(pNode, 8);
				DeleteHTokenStem(pNode, 4);

				pAddTwoString2 = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else
			{
				SetOneWordMarsk();
				GoMAR1_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
			}
		}
		else
		{
			SetOneWordMarsk();
			GoMAR1_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
		}
	}
	else if (strcmp(pOneChar, "하") == 0)
	{
		nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_MINUS);
		pNextOneChar = GetOneChar(pOriginalString, nCount, nCount);
		if (strcmp(pNextOneChar, "당") == 0)
		{
			nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_MINUS);
			pTmpNextStartCountChar = GetStartCountChar(pOriginalString, nCount);

			nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 2, CALCULATE_MINUS);
			pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);

			if (IsExistStemDic(pTmpNextStartCountChar))
			{
				GoIMR4(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
			}
			else if (IsExistStemDic(pNextStartCountChar))
			{
				pAddTwoString = AddTwoStr(pNextStartCountChar, "당하");
				InsertStemString(pNode, pAddTwoString);
				SaveHTokenStem(pNode);
				SaveHTokenStemFeature(pNode, pNextStartCountChar);
				AddHTokenStem(pNode, 7);
				AddHTokenStem(pNode, 35);
				AddHTokenStem(pNode, S_T159); // Added by KKH 2003-11-18
				DeleteHTokenStem(pNode, 8);
				DeleteHTokenStem(pNode, 4);

				pAddTwoString2 = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else
			{
				SetOneWordMarsk();
				GoMAR1_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
			}
		}
		else
		{
			SetOneWordMarsk();
			GoIMR4(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
		}
	}
	else if (strcmp(pOneChar, "세") == 0)
	{
		nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_MINUS);
		pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
		if (strcmp(pNextStartCountChar, "계") == 0 || strcmp(pNextStartCountChar, "주무") == 0 ||
			strcmp(pNextStartCountChar, "잡수") == 0)
		{
			pAddTwoString = AddTwoStr(pNextStartCountChar, "시");
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStem(pNode);
			SaveHTokenStemFeature(pNode, pNextStartCountChar);

			pAddTwoString2 = AddTwoStr("어", m_pOldBuffer);
			InsertEndingString(pNode, pAddTwoString2);
			if (IsExistEndDic(pAddTwoString2))
			{
				SaveHTokenEnding(pNode);
			}
			else
			{
				MakeNewEndingFeature(pNode);
				AddEndingFeature(pNode, "어");
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
			}
		}
		else
		{
			SetOneWordMarsk();
			GoMAR1_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
		}
	}
	else if (strcmp(pJongChar, "ㅆ") == 0)
	{
		GoIMR2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
	}
	else if (strcmp(pJongChar, "ㅁ") == 0 || strcmp(pJongChar, "ㄻ") == 0 || strcmp(pOneChar, "기") == 0)
	{
		GoIMR3(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
	}
	else if (CompareVariousTable01(pOneChar, 0, false) == true ||
		CompareVariousTable02(pOneChar, 1, false) == true ||
		CompareVariousTable02(pOneChar, 2, false) == true ||
		CompareVariousTable03(pOneChar, 0, false) == true ||
		CompareVariousTable03(pOneChar, 1, false) == true ||
		CompareVariousTable04(pOneChar, 1, false) == true ||
		CompareVariousTable04(pOneChar, 2, false) == true ||
		CompareVariousTable04(pOneChar, 3, false) == true ||
		CompareVariousTable05(pOneChar, ALL_TABLE_05, true) == true ||
		CompareVariousTable06(pOneChar, ALL_TABLE_06, true) == true ||
		CompareVariousTable07(pOneChar, ALL_TABLE_07, true) == true ||
		strcmp(pOneChar, "답") == 0 || strcmp(pOneChar, "럽") == 0 || strcmp(pOneChar, "우") == 0)
	{
		GoIMR4(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
	}
	else
	{
		GoErrorFunction(pNode);
	}

	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pJongChar);
	CHAR_DELETE(pNextOneChar);
	CHAR_DELETE(pNextStartCountChar);
	CHAR_DELETE(pTmpNextStartCountChar);
	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pAddTwoString2);
}


/////////////////////////////////////////////////////////////////
//IMR2
/////////////////////////////////////////////////////////////////
void CNAHangulAnaly::GoIMR2(H_NODE* pNode, int& nFirstOlive, int& mSecondOlive, int &mSecondAlkong)
{
	char* pOriginalString = NULL;

	char* pOneChar = NULL;
	char* pNextStartCountChar = NULL;
	char* pAddTwoString = NULL;
	char* pAddTwoString2 = NULL;
	char* tempOneChar = NULL; // Added by KKH 2003-11-07

	int nCount = 0;
	int nLen = 0;
	int nBLen = 0; // Added by KKH 2003-11-07

	pOriginalString = pNode->pHToken->strSource;
	pOneChar = GetOneChar(pOriginalString, nFirstOlive, nFirstOlive);
	if (strcmp(pOneChar, "었") == 0 || strcmp(pOneChar, "았") == 0 || strcmp(pOneChar, "겠") == 0)
	{
		nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_MINUS);
		pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
		if (IsExistStemDic(pNextStartCountChar))
		{
			InsertStemString(pNode, pNextStartCountChar);
			SaveHTokenStem(pNode);
			pAddTwoString = AddTwoStr(pOneChar, m_pNewBuffer);
			pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
			InsertEndingString(pNode, pAddTwoString2);
			if (IsExistEndDic(pAddTwoString2))
			{
				SaveHTokenEnding(pNode);
			}
			else
			{
				MakeNewEndingFeature(pNode);
				AddEndingFeature(pNode, pOneChar);
				SaveHTokenEndingNewFeature(pNode);
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, pOneChar);
				SaveAmbstrEnding(pNode, m_pNewBuffer);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
				nBLen = strlen(pAddTwoString2);
				for (int i = 0; i < nBLen; i = i + 2)
				{
					tempOneChar = GetOneChar(pAddTwoString2, nBLen - i, nBLen - i - 1);
					if (strcmp(tempOneChar, "음") == 0 || strcmp(tempOneChar, "기") == 0)
					{
						DeleteHTokenStem(pNode, 7);
						AddHTokenStem(pNode, 39);
						CHAR_DELETE(tempOneChar);
						break;
					}
					else
					{
						CHAR_DELETE(tempOneChar);
					}
				}
			}
		}
		else
		{
			InsertArrayFeatureOld(pOneChar);
			InsertArrayFeatureOld(m_pNewBuffer);
			pAddTwoString = AddTwoStr(pOneChar, m_pNewBuffer);
			pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);

			CHAR_DELETE(m_pOldBuffer);

			nLen = strlen(pAddTwoString2);
			m_pOldBuffer = new char[nLen + 1];
			memset(m_pOldBuffer, 0x00, nLen + 1);
			strcpy(m_pOldBuffer, pAddTwoString2);

			CHAR_DELETE(m_pNewBuffer);

			mSecondOlive = nFirstOlive;
			MoveLeftWordCount(pOriginalString, nFirstOlive);
			InitOneWordMarsk();
			if (nFirstOlive < 0)
			{
				GoErrorFunction(pNode);
			}
			else
			{
				GoMAR2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
			}
		}
	}
	else
	{
		GoIMR2_1(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
	}

	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pNextStartCountChar);
	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pAddTwoString2);
	CHAR_DELETE(tempOneChar); // Added by KKH 2003-11-07
}

void CNAHangulAnaly::GoIMR2_1(H_NODE* pNode, int& nFirstOlive, int& mSecondOlive, int &mSecondAlkong)
{
	char* pOriginalString = NULL;

	char* pOneChar = NULL;
	char* pNextOneChar = NULL;
	char* pJongChar = NULL;
	char* pChoJungChar = NULL;
	char* pThisStartCountChar = NULL;
	char* pNextStartCountChar = NULL;
	char* pNextStartCountChar2 = NULL;
	char* pAddTwoString = NULL;
	char* pAddTwoString2 = NULL;
	char* pMinusTwoString = NULL;
	char* tempOneChar = NULL; // Added by KKH 2003-11-07
	char* ptmpFeature = NULL; // Added by KKH 2004-01-13

	int nRestoration = 0;
	int nCount = 0;
	int nLen = 0;
	int nBLen = 0; // Added by KKH 2003-11-07

	pOriginalString = pNode->pHToken->strSource;
	pOneChar = GetOneChar(pOriginalString, nFirstOlive, nFirstOlive);
	pChoJungChar = GetChojungJongsungChar(pOneChar, CHAR_CHOJUNGSUNG);
	pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);
	nRestoration = Restoration(pJongChar);
	pThisStartCountChar = GetStartCountChar(pOriginalString, nFirstOlive);
	pMinusTwoString = MinusTwoStr(pThisStartCountChar, pOneChar, nFirstOlive);
	ptmpFeature = new char[256]; // Added by KKH 2004-01-13
	memset(ptmpFeature, 0x00, 256); // Added by KKH 2004-01-13

	if (strcmp(pOneChar, "였") == 0)
	{
		nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_MINUS);
		pNextOneChar = GetOneChar(pOriginalString, nCount, nCount);
		pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
		pAddTwoString = AddTwoStr(pNextStartCountChar, "이");
		if (strcmp(pNextOneChar, "하") == 0 || strcmp(pNextOneChar, "되") == 0)
		{
			nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 2, CALCULATE_MINUS);
			pNextStartCountChar2 = GetStartCountChar(pOriginalString, nCount);
			if (IsExistStemDic(pNextStartCountChar))
			{
				InsertStemString(pNode, pNextStartCountChar);
				SaveHTokenStem(pNode);
				DeleteHTokenStem(pNode, 4);
				DeleteHTokenStem(pNode, 1);
				AddHTokenStem(pNode, 8); // Added by KKH 2003-10-14
				AddHTokenStem(pNode, 7);
				AddHTokenStem(pNode, 2);

				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr("었", m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "었");
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, "었");
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
					if (strcmp(m_pOldBuffer, "기") == 0 || strstr(m_pOldBuffer, "음") != 0)
					{
						AddHTokenStem(pNode, 39);
						DeleteHTokenStem(pNode, 7);
					}
					if (strcmp(pNextOneChar, "되") == 0)
					{
						AddHTokenStem(pNode, 35);
					}
				}
			}
			else if (IsExistStemDic(pNextStartCountChar2))
			{
				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr(pNextStartCountChar2, pNextOneChar);
				InsertStemString(pNode, pAddTwoString);
				SaveHTokenStem(pNode);
				SaveHTokenStemFeature(pNode, pNextStartCountChar2);
				DeleteHTokenStem(pNode, 4);
				DeleteHTokenStem(pNode, 1);
				AddHTokenStem(pNode, 7);
				AddHTokenStem(pNode, 2);

				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr("었", m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "었");
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, "었");
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
					if (strcmp(m_pOldBuffer, "기") == 0 || strstr(m_pOldBuffer, "음") != 0)
					{
						AddHTokenStem(pNode, 39);
						DeleteHTokenStem(pNode, 7);
					}
					if (strcmp(pNextOneChar, "되") == 0)
					{
						AddHTokenStem(pNode, 35);
					}
				}
			}
			else if (IsExistStemDic(pNextStartCountChar2))
			{
				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr(pNextStartCountChar2, pNextOneChar);
				InsertStemString(pNode, pAddTwoString);
				SaveHTokenStem(pNode);
				SaveHTokenStemFeature(pNode, pNextStartCountChar2);
				DeleteHTokenStem(pNode, 4);
				DeleteHTokenStem(pNode, 1);
				AddHTokenStem(pNode, 7);
				AddHTokenStem(pNode, 2);

				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr("었", m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "었");
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, "었");
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
					if (strcmp(m_pOldBuffer, "기") == 0)
					{
						AddHTokenStem(pNode, 39);
						DeleteHTokenStem(pNode, 7);
					}
				}
			}
			else
			{
				GoErrorFunction(pNode);
			}
		}
		else if (IsExistStemDic(pAddTwoString))
		{
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStem(pNode);

			CHAR_DELETE(pAddTwoString);

			pAddTwoString = AddTwoStr("었", m_pNewBuffer);
			pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
			InsertEndingString(pNode, pAddTwoString2);
			if (IsExistEndDic(pAddTwoString2))
			{
				SaveHTokenEnding(pNode);
			}
			else
			{
				MakeNewEndingFeature(pNode);
				AddEndingFeature(pNode, "었");
				SaveHTokenEndingNewFeature(pNode);
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, "었");
				SaveAmbstrEnding(pNode, m_pNewBuffer);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
			}
		}
		else if (IsExistStemDic(pNextStartCountChar))
		{
			InsertStemString(pNode, pNextStartCountChar);
			SaveHTokenStem(pNode);
			SaveHTokenStemFeature(pNode, pNextStartCountChar);

			CHAR_DELETE(pAddTwoString);

			pAddTwoString = AddTwoStr("이었", m_pNewBuffer);
			pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
			InsertEndingString(pNode, pAddTwoString2);
			if (IsExistEndDic(pAddTwoString2))
			{
				SaveHTokenEnding(pNode);
				AddHTokenEnding(pNode, 26);
				DeleteHTokenEnding(pNode, 2);
			}
			else
			{
				MakeNewEndingFeature(pNode);
				AddEndingFeature(pNode, "었");
				SaveHTokenEndingNewFeature(pNode);
				SaveHTokenEndingOldFeature(pNode);
				AddHTokenEnding(pNode, 26);
				DeleteHTokenEnding(pNode, 2);
				SaveAmbstrEnding(pNode, "이");
				SaveAmbstrEnding(pNode, "었");
				SaveAmbstrEnding(pNode, m_pNewBuffer);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
			}
		}
		else
		{
			CHAR_DELETE(pAddTwoString);

			pAddTwoString = AddTwoStr("었", m_pNewBuffer);
			pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);

			CHAR_DELETE(m_pOldBuffer);

			nLen = strlen(pAddTwoString2);
			m_pOldBuffer = new char[nLen + 1];
			memset(m_pOldBuffer, 0x00, nLen + 1);
			strcpy(m_pOldBuffer, pAddTwoString2);
			InsertArrayFeatureOld("었");
			InsertArrayFeatureOld(m_pNewBuffer);

			CHAR_DELETE(m_pNewBuffer);

			mSecondOlive = nFirstOlive;
			if (ReturnNumberMoveLeftWordCount(pOriginalString, nFirstOlive) < 0)
			{
				GoErrorFunction(pNode);
			}
			else
			{
				GoMAR2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
			}
		}
	}
	else if (strcmp(pOneChar, "셨") == 0)
	{
		nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_MINUS);
		pNextOneChar = GetOneChar(pOriginalString, nCount, nCount);
		pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
		pAddTwoString = AddTwoStr(pNextStartCountChar, "시");
		if (strcmp(pNextOneChar, "으") == 0)
		{
			nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 2, CALCULATE_MINUS);
			pNextStartCountChar2 = GetStartCountChar(pOriginalString, nCount);
			if (IsExistStemDic(pNextStartCountChar2))
			{
				InsertStemString(pNode, pNextStartCountChar2);
				SaveHTokenStem(pNode);

				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr("으시었", m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "으시");
					AddEndingFeature(pNode, "었");
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, "으시");
					SaveAmbstrEnding(pNode, "었");
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
					// Added by KKH 2003-11-07
					nBLen = strlen(pAddTwoString2);
					for (int i = 0; i < nBLen; i = i + 2)
					{
						tempOneChar = GetOneChar(pAddTwoString2, nBLen - i, nBLen - i - 1);
						if (strcmp(tempOneChar, "음") == 0 ||
							strcmp(tempOneChar, "기") == 0)
						{
							DeleteHTokenStem(pNode, 7);
							AddHTokenStem(pNode, 39);
							CHAR_DELETE(tempOneChar);
							break;
						}
						else
						{
							CHAR_DELETE(tempOneChar);
						}
					}
					// Added by KKH 2003-11-07
				}
			}
			else
			{
				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr("으시었", m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "으시");
					AddEndingFeature(pNode, "었");
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, "으시");
					SaveAmbstrEnding(pNode, "었");
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
				MoveLeftWordCount(pOriginalString, nFirstOlive);
				MoveLeftWordCount(pOriginalString, nFirstOlive);
				InitOneWordMarsk();

				GoMAR2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
			}
		}
		else if (IsExistStemDic(pAddTwoString))
		{
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStem(pNode);

			CHAR_DELETE(pAddTwoString);

			pAddTwoString = AddTwoStr("었", m_pNewBuffer);
			pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
			InsertEndingString(pNode, pAddTwoString2);
			if (IsExistEndDic(pAddTwoString2))
			{
				SaveHTokenEnding(pNode);
			}
			else
			{
				MakeNewEndingFeature(pNode);
				AddEndingFeature(pNode, "었");
				SaveHTokenEndingNewFeature(pNode);
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, "었");
				SaveAmbstrEnding(pNode, m_pNewBuffer);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
			}
		}
		else if (IsExistStemDic(pNextStartCountChar))
		{
			InsertStemString(pNode, pNextStartCountChar);
			SaveHTokenStem(pNode);

			CHAR_DELETE(pAddTwoString);

			pAddTwoString = AddTwoStr("시었", m_pNewBuffer);
			pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
			InsertEndingString(pNode, pAddTwoString2);
			if (IsExistEndDic(pAddTwoString2))
			{
				SaveHTokenEnding(pNode);
			}
			else
			{
				MakeNewEndingFeature(pNode);
				AddEndingFeature(pNode, "시");
				AddEndingFeature(pNode, "었");
				SaveHTokenEndingNewFeature(pNode);
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, "시");
				SaveAmbstrEnding(pNode, "었");
				SaveAmbstrEnding(pNode, m_pNewBuffer);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
				// Added by KKH 2003-11-07
				nBLen = strlen(pAddTwoString2);
				for (int i = 0; i < nBLen; i = i + 2)
				{
					tempOneChar = GetOneChar(pAddTwoString2, nBLen - i, nBLen - i - 1);
					if (strcmp(tempOneChar, "음") == 0 ||
						strcmp(tempOneChar, "기") == 0)
					{
						DeleteHTokenStem(pNode, 7);
						AddHTokenStem(pNode, 39);
						CHAR_DELETE(tempOneChar);
						break;
					}
					else
					{
						CHAR_DELETE(tempOneChar);
					}
				}
				// Added by KKH 2003-11-07
			}
		}
		else
		{
			CHAR_DELETE(pAddTwoString);

			pAddTwoString = AddTwoStr("시었", m_pNewBuffer);
			pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);

			CHAR_DELETE(m_pOldBuffer);

			nLen = strlen(pAddTwoString2);
			m_pOldBuffer = new char[nLen + 1];
			memset(m_pOldBuffer, 0x00, nLen + 1);
			strcpy(m_pOldBuffer, pAddTwoString2);
			InsertArrayFeatureOld("시");
			InsertArrayFeatureOld("었");
			InsertArrayFeatureOld(m_pNewBuffer);

			CHAR_DELETE(m_pNewBuffer);

			MoveLeftWordCount(pOriginalString, nFirstOlive);
			InitOneWordMarsk();
			if (nFirstOlive < 0)
			{
				GoErrorFunction(pNode);
			}
			else if (ReplaceHgngulTokenSource(pNode, nFirstOlive, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03) && IsExistNFeature(pNode->pHToken->strSource, nFirstOlive, 4) == false)//C(n)='ㄹ' -> S(len~n) && != [N]
			{
				pOriginalString = pNode->pHToken->strSource;

				CHAR_DELETE(pThisStartCountChar);

				pThisStartCountChar = GetStartCountChar(pOriginalString, nFirstOlive);
				InsertStemString(pNode, pThisStartCountChar);
				SaveHTokenStem(pNode);
				InsertEndingString(pNode, m_pOldBuffer);
				MakeNewEndingFeature(pNode);
				SaveHTokenEndingOldFeature(pNode);
				// Added by KKH 2003-11-07
				nBLen = strlen(pAddTwoString2);
				for (int i = 0; i < nBLen; i = i + 2)
				{
					tempOneChar = GetOneChar(pAddTwoString2, nBLen - i, nBLen - i - 1);
					if (strcmp(tempOneChar, "음") == 0 ||
						strcmp(tempOneChar, "기") == 0)
					{
						DeleteHTokenStem(pNode, 7);
						AddHTokenStem(pNode, 39);
						CHAR_DELETE(tempOneChar);
						break;
					}
					else
					{
						CHAR_DELETE(tempOneChar);
					}
				}
				// Added by KKH 2003-11-07
			}
			else
			{
				ReplaceHangulJongsungNull(pNode, nFirstOlive);
				GoMAR2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
			}
		}
	}
	else if (strcmp(pOneChar, "갔") == 0)
	{
		if (IsExistStemDic(pMinusTwoString))
		{
			InsertStemString(pNode, pMinusTwoString);
			SaveHTokenStem(pNode);

			CHAR_DELETE(pAddTwoString);

			pAddTwoString = AddTwoStr("었", m_pNewBuffer);
			pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
			InsertEndingString(pNode, pAddTwoString2);
			if (IsExistEndDic(pAddTwoString2))
			{
				SaveHTokenEnding(pNode);
			}
			else
			{
				MakeNewEndingFeature(pNode);
				AddEndingFeature(pNode, "었");
				SaveHTokenEndingNewFeature(pNode);
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, "었");
				SaveAmbstrEnding(pNode, m_pNewBuffer);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
				// Added by KKH 2003-11-07
				nBLen = strlen(pAddTwoString2);
				for (int i = 0; i < nBLen; i = i + 2)
				{
					tempOneChar = GetOneChar(pAddTwoString2, nBLen - i, nBLen - i - 1);
					if (strcmp(tempOneChar, "음") == 0 ||
						strcmp(tempOneChar, "기") == 0)
					{
						DeleteHTokenStem(pNode, 7);
						AddHTokenStem(pNode, 39);
						CHAR_DELETE(tempOneChar);
						break;
					}
					else
					{
						CHAR_DELETE(tempOneChar);
					}
				}
				// Added by KKH 2003-11-07
			}
		}
		else
		{
			GoErrorFunction(pNode);
		}
	}
	else if (IsExistStemDic(pMinusTwoString, ptmpFeature) &&
		ptmpFeature != NULL && ptmpFeature[S_N] == '0') // Edited by KKH 2004-01-13
	{
		InsertStemString(pNode, pMinusTwoString);
		SaveHTokenStem(pNode);

		CHAR_DELETE(pAddTwoString);

		pAddTwoString = AddTwoStr("었", m_pNewBuffer);
		pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
		InsertEndingString(pNode, pAddTwoString2);
		if (IsExistEndDic(pAddTwoString2))
		{
			SaveHTokenEnding(pNode);
		}
		else
		{
			MakeNewEndingFeature(pNode);
			AddEndingFeature(pNode, "었");
			SaveHTokenEndingNewFeature(pNode);
			SaveHTokenEndingOldFeature(pNode);
			SaveAmbstrEnding(pNode, "었");
			SaveAmbstrEnding(pNode, m_pNewBuffer);
			SaveAmbstrEnding(pNode, m_pOldBuffer);
		}
	}
	else if (CompareVariousTable01(pChoJungChar, 0, false) == true ||
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
		InsertArrayFeatureOld("었");
		InsertArrayFeatureOld(m_pNewBuffer);

		CHAR_DELETE(pAddTwoString);

		pAddTwoString = AddTwoStr("었", m_pNewBuffer);
		pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);

		CHAR_DELETE(m_pOldBuffer);

		CHAR_DELETE(m_pNewBuffer);

		nLen = strlen(pAddTwoString2);
		m_pOldBuffer = new char[nLen + 1];
		memset(m_pOldBuffer, 0x00, nLen + 1);
		strcpy(m_pOldBuffer, pAddTwoString2);

		ReplaceHangulJongsungNull(pNode, nFirstOlive);
		GoIMR4(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
	}
	else
	{
		CHAR_DELETE(pAddTwoString);

		pAddTwoString = AddTwoStr("었", m_pNewBuffer);
		pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
		InsertEndingString(pNode, pAddTwoString2);
		if (IsExistEndDic(pAddTwoString2))
		{
			SaveHTokenEnding(pNode);
		}
		else
		{
			MakeNewEndingFeature(pNode);
			AddEndingFeature(pNode, "었");
			SaveHTokenEndingNewFeature(pNode);
			SaveHTokenEndingOldFeature(pNode);
			SaveAmbstrEnding(pNode, "었");
			SaveAmbstrEnding(pNode, m_pNewBuffer);
			SaveAmbstrEnding(pNode, m_pOldBuffer);
		}
		InsertStemString(pNode, pMinusTwoString);
		SaveHTokenStem(pNode);
		GoErrorFunction(pNode);
	}

	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pNextOneChar);
	CHAR_DELETE(pJongChar);
	CHAR_DELETE(pChoJungChar);
	CHAR_DELETE(pThisStartCountChar);
	CHAR_DELETE(pNextStartCountChar);
	CHAR_DELETE(pNextStartCountChar2);
	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pAddTwoString2);
	CHAR_DELETE(pMinusTwoString);
	CHAR_DELETE(tempOneChar); // Added by KKH 2003-11-07
							  //2017-08-15 누락된 DELETE 추가
	CHAR_DELETE(ptmpFeature);

}


/////////////////////////////////////////////////////////////////
//IMR3
/////////////////////////////////////////////////////////////////
void CNAHangulAnaly::GoIMR3(H_NODE* pNode, int& nFirstOlive, int& mSecondOlive, int &mSecondAlkong)
{
	char* pOriginalString = NULL;

	char* pOneChar = NULL;
	char* pJongChar = NULL;
	char* pThisStartCountChar = NULL;
	char* pNextStartCountChar = NULL;
	char* pMinusTwoString = NULL;
	char* pAddTwoString = NULL;
	char* pAddTwoString2 = NULL;

	int nCount = 0;
	int nLen = 0;

	pOriginalString = pNode->pHToken->strSource;
	pOneChar = GetOneChar(pOriginalString, nFirstOlive, nFirstOlive);
	pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);
	if (strcmp(pJongChar, "ㄻ") == 0)
	{
		if (ReplaceHgngulTokenSource(pNode, nFirstOlive, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03))
		{
			pOriginalString = pNode->pHToken->strSource;
			pThisStartCountChar = GetStartCountChar(pOriginalString, nFirstOlive);
			InsertStemString(pNode, pThisStartCountChar);
			SaveHTokenStem(pNode);
			AddHTokenStem(pNode, 39);
			DeleteHTokenStem(pNode, 7);
			pAddTwoString = AddTwoStr("음", m_pNewBuffer);
			pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
			InsertEndingString(pNode, pAddTwoString2);
			if (IsExistEndDic(pAddTwoString2))
			{
				SaveHTokenEnding(pNode);
			}
			else
			{
				MakeNewEndingFeature(pNode);
				AddEndingFeature(pNode, "음");
				SaveHTokenEndingNewFeature(pNode);
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, "음");
				SaveAmbstrEnding(pNode, m_pNewBuffer);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
			}
		}
		else
			GoErrorFunction(pNode);
	}
	else if (strcmp(pOneChar, "임") == 0)
	{
		pThisStartCountChar = GetStartCountChar(pOriginalString, nFirstOlive);
		if (IsExistStemDic(pThisStartCountChar))
		{
			GoErrorFunction(pNode);
		}
		else
		{
			pMinusTwoString = MinusTwoStr(pThisStartCountChar, pOneChar, nFirstOlive);
			nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_MINUS);
			pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
			if (IsExistStemDic(pMinusTwoString))
			{
				InsertStemString(pNode, pMinusTwoString);
				SaveHTokenStem(pNode);
				AddHTokenStem(pNode, 39);
				DeleteHTokenStem(pNode, 7);
				pAddTwoString = AddTwoStr("음", m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "음");
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, "음");
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else if (IsExistStemDic(pNextStartCountChar))
			{
				InsertStemString(pNode, pNextStartCountChar);
				SaveHTokenStem(pNode);
				SaveHTokenStemFeature(pNode, pNextStartCountChar);
				pAddTwoString = AddTwoStr("이음", m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, 26);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "음");
					AddHTokenEnding(pNode, 26);
					DeleteHTokenEnding(pNode, 4);
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, "이");
					SaveAmbstrEnding(pNode, "음");
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else
			{
				pAddTwoString = AddTwoStr("음", m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "음");
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, "음");
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
				MoveLeftWordCount(pOriginalString, nFirstOlive);
				InitOneWordMarsk();
				if (nFirstOlive < 0)
					GoErrorFunction(pNode);
				else
					GoMAR2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
			}
		}
	}
	else if (strcmp(pOneChar, "음") == 0)
	{
		pThisStartCountChar = GetStartCountChar(pOriginalString, nFirstOlive);
		pMinusTwoString = MinusTwoStr(pThisStartCountChar, pOneChar, nFirstOlive);
		nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_MINUS);
		pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
		if (IsExistStemDic(pMinusTwoString))
		{
			InsertStemString(pNode, pMinusTwoString);
			SaveHTokenStem(pNode);
			AddHTokenStem(pNode, 39);
			DeleteHTokenStem(pNode, 7);
			pAddTwoString = AddTwoStr("음", m_pNewBuffer);
			pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
			InsertEndingString(pNode, pAddTwoString2);
			if (IsExistEndDic(pAddTwoString2))
			{
				SaveHTokenEnding(pNode);
			}
			else
			{
				MakeNewEndingFeature(pNode);
				AddEndingFeature(pNode, "음");
				SaveHTokenEndingNewFeature(pNode);
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, "음");
				SaveAmbstrEnding(pNode, m_pNewBuffer);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
			}
		}
		else if (IsExistStemDic(pNextStartCountChar))//4n
		{
			CHAR_DELETE(pOneChar);
			CHAR_DELETE(pJongChar);

			pOneChar = GetOneChar(pOriginalString, nCount, nCount);
			pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);
			if (pJongChar != NULL && IsExistFeature(pNextStartCountChar, STEM_DIC, S_N) == false)
			{
				InsertStemString(pNode, pNextStartCountChar);
				SaveHTokenStem(pNode);
				AddHTokenStem(pNode, 39);
				DeleteHTokenStem(pNode, 7);
				pAddTwoString = AddTwoStr("음", m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "음");
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, "음");
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else
			{
				GoErrorFunction(pNode);
			}
		}
		else // 5n
		{
			pAddTwoString = AddTwoStr("음", m_pNewBuffer);
			pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
			InsertArrayFeatureOld("음");
			InsertArrayFeatureOld(m_pNewBuffer);
			if (m_pOldBuffer != NULL)
			{
				delete m_pOldBuffer;
				m_pOldBuffer = NULL;
			}
			nLen = strlen(pAddTwoString2);
			m_pOldBuffer = new char[nLen + 1];
			memset(m_pOldBuffer, 0x00, nLen + 1);
			strcpy(m_pOldBuffer, pAddTwoString2);
			mSecondOlive = nFirstOlive;
			MoveLeftWordCount(pOriginalString, nFirstOlive);
			InitOneWordMarsk();
			if (nFirstOlive < 0)
			{
				GoErrorFunction(pNode);//Call GoErrorFunction ...
			}
			else
			{
				GoMAR2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);//Call GoMAR2 Function ...	
			}
		}
	}
	else
	{
		GoIMR3_1(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
	}

	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pJongChar);
	CHAR_DELETE(pThisStartCountChar);
	CHAR_DELETE(pNextStartCountChar);
	CHAR_DELETE(pMinusTwoString);
	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pAddTwoString2);
}
void CNAHangulAnaly::GoIMR3_1(H_NODE* pNode, int& nFirstOlive, int& mSecondOlive, int &mSecondAlkong)
{
	char* pOriginalString = NULL;

	char* pOneChar = NULL;
	char* pJongChar = NULL;
	char* pThisStartCountChar = NULL;
	char* pNextStartCountChar = NULL;
	char* pMinusTwoString = NULL;
	char* pAddTwoString = NULL;
	char* pAddTwoString2 = NULL;
	char* tempOneChar = NULL; // Added by KKH 2003-11-07
	char* tempJongSung = NULL; // Added by KKH 2003-11-07

	int nCount = 0;
	int nLen = 0;
	int nBLen = 0; // Added by KKH 2003-11-07

	pOriginalString = pNode->pHToken->strSource;

	pOneChar = GetOneChar(pOriginalString, nFirstOlive, nFirstOlive);

	if (strcmp(pOneChar, "기") == 0)
	{
		nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_MINUS);
		pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
		if (IsExistStemDic(pNextStartCountChar))
		{
			if (IsExistFeature(pNextStartCountChar, STEM_DIC, S_N) == false)
			{
				InsertStemString(pNode, pNextStartCountChar);
				SaveHTokenStem(pNode);
				AddHTokenStem(pNode, 39);
				DeleteHTokenStem(pNode, 7);
				pAddTwoString = AddTwoStr("기", m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "기");
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, "기");
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else
			{
				GoErrorFunction(pNode);
			}
		}
		else
		{
			pAddTwoString = AddTwoStr("기", m_pNewBuffer);
			pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
			InsertArrayFeatureOld("기");
			InsertArrayFeatureOld(m_pNewBuffer);
			if (m_pOldBuffer != NULL)
			{
				delete m_pOldBuffer;
				m_pOldBuffer = NULL;
			}
			nLen = strlen(pAddTwoString2);
			m_pOldBuffer = new char[nLen + 1];
			memset(m_pOldBuffer, 0x00, nLen + 1);
			strcpy(m_pOldBuffer, pAddTwoString2);
			mSecondOlive = nFirstOlive;
			MoveLeftWordCount(pOriginalString, nFirstOlive);
			InitOneWordMarsk();

			if (nFirstOlive < 0)
			{
				GoErrorFunction(pNode);//Call GoErrorFunction ...
			}
			else
			{
				GoMAR2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);//Call GoMAR2 Function ...	
			}
		}
	}
	else
	{
		pThisStartCountChar = GetStartCountChar(pOriginalString, nFirstOlive);
		pMinusTwoString = MinusTwoStr(pThisStartCountChar, pOneChar, nFirstOlive);
		if (IsExistStemDic(pMinusTwoString))
		{
			InsertStemString(pNode, pMinusTwoString);
			SaveHTokenStem(pNode);
			AddHTokenStem(pNode, 39);
			DeleteHTokenStem(pNode, 7);
			pAddTwoString = AddTwoStr("음", m_pNewBuffer);
			pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
			InsertEndingString(pNode, pAddTwoString2);
			if (IsExistEndDic(pAddTwoString2))
			{
				SaveHTokenEnding(pNode);
			}
			else
			{
				MakeNewEndingFeature(pNode);
				AddEndingFeature(pNode, "음");
				SaveHTokenEndingNewFeature(pNode);
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, "음");
				SaveAmbstrEnding(pNode, m_pNewBuffer);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
			}
		}
		else if (strcmp(pOneChar, "함") == 0)
		{
			nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_MINUS);
			pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
			if (IsExistStemDic(pNextStartCountChar))
			{
				pAddTwoString = AddTwoStr(pNextStartCountChar, "하");
				InsertStemString(pNode, pAddTwoString);
				SaveHTokenStem(pNode);
				SaveHTokenStemFeature(pNode, pNextStartCountChar);
				AddHTokenStem(pNode, 39);
				AddHTokenStem(pNode, 2);
				DeleteHTokenStem(pNode, 4);
				DeleteHTokenStem(pNode, 1);

				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr("음", m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "음");
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, "음");
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else
			{
				SetOneWordMarsk();
				GoMAR2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
			}
		}
		else if (strcmp(pOneChar, "됨") == 0)
		{
			nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_MINUS);
			pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
			if (IsExistStemDic(pNextStartCountChar))
			{

				pAddTwoString = AddTwoStr(pNextStartCountChar, "되");
				InsertStemString(pNode, pAddTwoString);
				SaveHTokenStem(pNode);
				SaveHTokenStemFeature(pNode, pNextStartCountChar);
				AddHTokenStem(pNode, 39);
				AddHTokenStem(pNode, S_T159); // Added by KKH 2003-11-19
				DeleteHTokenStem(pNode, 7);
				DeleteHTokenStem(pNode, 4);
				DeleteHTokenStem(pNode, 1);

				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr("음", m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "음");
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, "음");
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else
			{
				SetOneWordMarsk();
				GoMAR2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
			}
		}
		else if (strcmp(pOneChar, "심") == 0)
		{
			nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_MINUS);
			pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
			tempOneChar = GetOneChar(pNextStartCountChar, nCount, nCount - 2); // Added by KKH 2003-11-07
			tempJongSung = GetChojungJongsungChar(tempOneChar, CHAR_JONGSUNG);
			if (IsExistFeature(pNextStartCountChar, STEM_DIC, 4) == false &&
				IsExistStemDic(pNextStartCountChar))
			{
				InsertStemString(pNode, pNextStartCountChar);
				SaveHTokenStem(pNode);
				AddHTokenStem(pNode, 39);
				DeleteHTokenStem(pNode, 7);
				pAddTwoString = AddTwoStr("시음", m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "시");
					AddEndingFeature(pNode, "음");
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, "시");
					SaveAmbstrEnding(pNode, "음");
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			// Added by KKH 2003-11-07
			else if (strcmp(tempOneChar, "으") == 0)
			{
				nCount = nCount - 2;
				pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
				if (IsExistFeature(pNextStartCountChar, STEM_DIC, 4) == false &&
					IsExistStemDic(pNextStartCountChar))
				{
					InsertStemString(pNode, pNextStartCountChar);
					SaveHTokenStem(pNode);
					AddHTokenStem(pNode, 39);
					DeleteHTokenStem(pNode, 7);
					pAddTwoString = AddTwoStr("시음", m_pNewBuffer);
					pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
					InsertEndingString(pNode, pAddTwoString2);
					if (IsExistEndDic(pAddTwoString2))
					{
						SaveHTokenEnding(pNode);
					}
					else
					{
						MakeNewEndingFeature(pNode);
						AddEndingFeature(pNode, "시");
						AddEndingFeature(pNode, "음");
						SaveHTokenEndingNewFeature(pNode);
						SaveHTokenEndingOldFeature(pNode);
						SaveAmbstrEnding(pNode, "시");
						SaveAmbstrEnding(pNode, "음");
						SaveAmbstrEnding(pNode, m_pNewBuffer);
						SaveAmbstrEnding(pNode, m_pOldBuffer);
						// Added by KKH 2003-11-07
						nBLen = strlen(pAddTwoString2);
						for (int i = 0; i < nBLen; i = i + 2)
						{
							tempOneChar = GetOneChar(pAddTwoString2, nBLen - i, nBLen - i - 1);
							if (strcmp(tempOneChar, "음") == 0 ||
								strcmp(tempOneChar, "기") == 0)
							{
								DeleteHTokenStem(pNode, 7);
								AddHTokenStem(pNode, 39);
								CHAR_DELETE(tempOneChar);
								break;
							}
							else
							{
								CHAR_DELETE(tempOneChar);
							}
						}
						// Added by KKH 2003-11-07
					}
				}
			}
			else if (strcmp(tempJongSung, "") == 0 &&
				ReplaceHgngulTokenSource(pNode, nCount, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03))
			{
				pThisStartCountChar = GetStartCountChar(pOriginalString, nCount);
				InsertStemString(pNode, pThisStartCountChar);
				SaveHTokenStem(pNode);
				AddHTokenStem(pNode, 39);
				DeleteHTokenStem(pNode, 7);
				pAddTwoString = AddTwoStr("시음", m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "시");
					AddEndingFeature(pNode, "음");
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, "시");
					SaveAmbstrEnding(pNode, "음");
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
					// Added by KKH 2003-11-07
					nBLen = strlen(pAddTwoString2);
					for (int i = 0; i < nBLen; i = i + 2)
					{
						tempOneChar = GetOneChar(pAddTwoString2, nBLen - i, nBLen - i - 1);
						if (strcmp(tempOneChar, "음") == 0 ||
							strcmp(tempOneChar, "기") == 0)
						{
							DeleteHTokenStem(pNode, 7);
							AddHTokenStem(pNode, 39);
							CHAR_DELETE(tempOneChar);
							break;
						}
						else
						{
							CHAR_DELETE(tempOneChar);
						}
					}
					// Added by KKH 2003-11-07
				}
			}
			// Added by KKH 2003-11-07
			else
			{
				// Added by KKH 2003-11-07
				if (strcmp(tempJongSung, "") == 0)
				{
					ReplaceHangulJongsungNull(pNode, nCount);
				}
				CHAR_DELETE(tempJongSung);
				SetOneWordMarsk();
				GoMAR1_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
			}
		}
		else
		{
			SetOneWordMarsk();
			GoMAR1_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
		}

	}
	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pJongChar);
	CHAR_DELETE(pThisStartCountChar);
	CHAR_DELETE(pNextStartCountChar);
	CHAR_DELETE(pMinusTwoString);
	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pAddTwoString2);
	CHAR_DELETE(tempOneChar); // Added by KKH 2003-11-07
	CHAR_DELETE(tempJongSung); // Added by KKH 2003-11-07
}


/////////////////////////////////////////////////////////////////
//IMR4
/////////////////////////////////////////////////////////////////
void CNAHangulAnaly::GoIMR4(H_NODE* pNode, int& nFirstOlive, int& mSecondOlive, int &mSecondAlkong)
{
	char* pOriginalString = NULL;

	char* pOneChar = NULL;
	char* pTwoChar = NULL;
	char* pNextOneChar = NULL;
	char* pPrevOneChar = NULL;
	char* pLastOneChar = NULL;
	char* pThisStartCountChar = NULL;
	char* pNextStartCountChar = NULL;
	char* pNextStartCountChar2 = NULL;
	char* pPrevStartCountChar = NULL;
	char* pMiddleCountChar = NULL;
	char* pEndCountChar = NULL;
	char* pAddTwoString = NULL;
	char* pAddTwoString2 = NULL;
	char* pPrevEndCountChar = NULL;
	char* tempEnding = NULL; // Added by KKH 2003-11-07
	char* tempOneChar = NULL; // Added by KKH 2003-11-07

	int nCount = 0;
	int nBLen = 0;
	char* pNewAddTwoString = NULL;

	pOriginalString = pNode->pHToken->strSource;
	pOneChar = GetOneChar(pOriginalString, nFirstOlive, nFirstOlive);
	nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_PLUS);
	pTwoChar = GetOneChar(pOriginalString, nCount, nCount);
	pNewAddTwoString = AddTwoStr(pOneChar, pTwoChar);
	if (CompareVariousTable05(pOneChar, ALL_TABLE_05, true) == true)
	{
		if (ReplaceHgngulTokenSource(pNode, nFirstOlive, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA02))
		{
			pOriginalString = pNode->pHToken->strSource;
			pThisStartCountChar = GetStartCountChar(pOriginalString, nFirstOlive);
			nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_PLUS);
			pEndCountChar = GetEndCountChar(pOriginalString, nCount);
			if (IsExistFeature(pThisStartCountChar, STEM_DIC, 4) == false && IsExistFeature(pEndCountChar, END_DIC, 2) == false)
			{
				InsertStemString(pNode, pThisStartCountChar);
				SaveHTokenStem(pNode);
				pAddTwoString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString);
				if (IsExistEndDic(pAddTwoString))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else if (ReplaceHgngulTokenSource(pNode, nFirstOlive, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03))
			{
				pOriginalString = pNode->pHToken->strSource;
				pThisStartCountChar = GetStartCountChar(pOriginalString, nFirstOlive);

				char Feature_Stem[236];
				char IS_CEHCK_FEATURE = '1';
				memset(Feature_Stem, 0x00, sizeof(Feature_Stem));
				bool isExist = false;
				isExist = IsExistStemDic(pNextStartCountChar, Feature_Stem);
				if (Feature_Stem[64] == IS_CEHCK_FEATURE)
				{
					InsertStemString(pNode, pThisStartCountChar);
					SaveHTokenStem(pNode);
					pAddTwoString = AddTwoStr("ㄹ", m_pNewBuffer);
					pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
					InsertEndingString(pNode, pAddTwoString2);
					if (IsExistEndDic(pAddTwoString2))
					{
						SaveHTokenEnding(pNode);
					}
					else
					{
						MakeNewEndingFeature(pNode);
						AddEndingFeature(pNode, "ㄹ");
						SaveHTokenEndingNewFeature(pNode);
						SaveHTokenEndingOldFeature(pNode);
						SaveAmbstrEnding(pNode, m_pNewBuffer);
						SaveAmbstrEnding(pNode, m_pOldBuffer);
					}
				}
				else
				{
					InsertStemString(pNode, pThisStartCountChar);
					SaveHTokenStem(pNode);
					pAddTwoString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
					InsertEndingString(pNode, pAddTwoString);
					if (IsExistEndDic(pAddTwoString))
					{
						SaveHTokenEnding(pNode);
					}
					else
					{
						MakeNewEndingFeature(pNode);
						SaveHTokenEndingNewFeature(pNode);
						SaveHTokenEndingOldFeature(pNode);
						SaveAmbstrEnding(pNode, m_pNewBuffer);
						SaveAmbstrEnding(pNode, m_pOldBuffer);
					}
				}
			}
			else
			{
				SetOneWordMarsk();
				GoMAR1_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
			}
		}
		else if (ReplaceHgngulTokenSource(pNode, nFirstOlive, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA03))
		{
			pOriginalString = pNode->pHToken->strSource;
			pThisStartCountChar = GetStartCountChar(pOriginalString, nFirstOlive);
			if (IsExistFeature(pThisStartCountChar, STEM_DIC, 64) == true)
			{
				InsertStemString(pNode, pThisStartCountChar);
				SaveHTokenStem(pNode);
				pAddTwoString = AddTwoStr("ㄹ", m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else
			{
				InsertStemString(pNode, pThisStartCountChar);
				SaveHTokenStem(pNode);
				pAddTwoString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString);
				if (IsExistEndDic(pAddTwoString))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
		}
		else
		{
			SetOneWordMarsk();
			GoMAR1_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
		}
	}
	else if (CompareVariousTable06(pOneChar, ALL_TABLE_06, true) == true)//1n
	{
		if (ReplaceHgngulTokenSource(pNode, nFirstOlive, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA06))
		{
			pOriginalString = pNode->pHToken->strSource;
			nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_PLUS);
			pThisStartCountChar = GetStartCountChar(pOriginalString, nFirstOlive);
			pEndCountChar = GetEndCountChar(pOriginalString, nCount);
			// Edited by KKH 2003-11-07
			if (IsExistFeature(pThisStartCountChar, STEM_DIC, 7) == true)//&& IsExistFeature(pEndCountChar, END_DIC, 3) == true)
			{
				InsertStemString(pNode, pThisStartCountChar);
				SaveHTokenStem(pNode);
				tempEnding = pNode->pHToken->subEnding->strResult;
				if (tempEnding == NULL)
				{
					pAddTwoString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
					InsertEndingString(pNode, pAddTwoString);
				}
				if (IsExistEndDic(pAddTwoString))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
					// Added by KKH 2003-11-07
					if (tempEnding != NULL)
					{
						nBLen = strlen(tempEnding);
						for (int i = 0; i < nBLen; i = i + 2)
						{
							tempOneChar = GetOneChar(tempEnding, nBLen - i, nBLen - i - 1);
							if (strcmp(tempOneChar, "음") == 0 ||
								strcmp(tempOneChar, "기") == 0)
							{
								DeleteHTokenStem(pNode, 7);
								AddHTokenStem(pNode, 39);
								CHAR_DELETE(tempOneChar);
								break;
							}
							else
							{
								CHAR_DELETE(tempOneChar);
							}
						}
					}
					// Added by KKH 2003-11-07
				}
				// Edited by KKH 2003-11-07
			}
			else if (DeleteHgngulTokenSource(pNode, nFirstOlive, FORCE_REPLACE_ENDWORD))
			{
				pOriginalString = pNode->pHToken->strSource;

				CHAR_DELETE(pThisStartCountChar);

				pThisStartCountChar = GetStartCountChar(pOriginalString, nFirstOlive);
				InsertStemString(pNode, pThisStartCountChar);
				SaveHTokenStem(pNode);
				pAddTwoString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString);
				if (IsExistEndDic(pAddTwoString))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else
			{
				pOriginalString = pNode->pHToken->strSource;
				nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_MINUS);
				pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
				if (IsExistStemDic(pNextStartCountChar))
				{
					bool isExist = false;
					char Feature_Stem[236];
					char IS_CEHCK_FEATURE = '1';
					memset(Feature_Stem, 0x00, sizeof(Feature_Stem));
					isExist = IsExistStemDic(pNextStartCountChar, Feature_Stem);

					CHAR_DELETE(pOneChar);

					pOneChar = GetOneChar(pOriginalString, nFirstOlive, nFirstOlive);
					if (strcmp(pOneChar, "이") == 0 && (Feature_Stem[4] == IS_CEHCK_FEATURE || Feature_Stem[1] == IS_CEHCK_FEATURE || Feature_Stem[39] == IS_CEHCK_FEATURE))
					{
						InsertStemString(pNode, pNextStartCountChar);
						SaveHTokenStemFeature(pNode, pNextStartCountChar);
						pAddTwoString = AddTwoStr("이", m_pNewBuffer);
						pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
						InsertEndingString(pNode, pAddTwoString2);
						if (IsExistEndDic(pAddTwoString2))
						{
							SaveHTokenEnding(pNode);
							AddHTokenEnding(pNode, 26);
							DeleteHTokenEnding(pNode, 2);
						}
						else
						{
							MakeNewEndingFeature(pNode);
							AddEndingFeature(pNode, "이");
							SaveHTokenEndingNewFeature(pNode);
							SaveHTokenEndingOldFeature(pNode);
							AddHTokenEnding(pNode, 26);
							DeleteHTokenEnding(pNode, 2);
							SaveAmbstrEnding(pNode, "이");
							SaveAmbstrEnding(pNode, m_pNewBuffer);
							SaveAmbstrEnding(pNode, m_pOldBuffer);
						}
					}
					else
					{
						SetOneWordMarsk();
						GoMAR1_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
					}
				}
				else
				{
					SetOneWordMarsk();
					GoMAR1_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
				}
			}
		}
		else if (DeleteHgngulTokenSource(pNode, nFirstOlive, FORCE_REPLACE_ENDWORD)) //3n
		{
			pOriginalString = pNode->pHToken->strSource;
			nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_PLUS);

			CHAR_DELETE(pThisStartCountChar);

			pThisStartCountChar = GetStartCountChar(pOriginalString, nFirstOlive);
			pEndCountChar = GetEndCountChar(pOriginalString, nCount);
			if ((IsExistFeature(pThisStartCountChar, STEM_DIC, 4) == true && IsExistFeature(pEndCountChar, END_DIC, 2) == true) ||
				(IsExistFeature(pThisStartCountChar, STEM_DIC, 4) == false && IsExistFeature(pEndCountChar, END_DIC, 2) == false) ||
				(IsExistFeature(pThisStartCountChar, STEM_DIC, 1) == true || IsExistFeature(pEndCountChar, END_DIC, 0) == true))
			{
				InsertStemString(pNode, pThisStartCountChar);
				SaveHTokenStem(pNode);
				pAddTwoString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString);
				if (IsExistEndDic(pAddTwoString))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else
			{
				SetOneWordMarsk();
				GoMAR1_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
			}
		}
		else
		{
			pOriginalString = pNode->pHToken->strSource;
			bool isExist = false;
			char Feature_Stem[236];
			char Feature_Stem1[236];
			char Feature_End[48];
			char IS_CEHCK_FEATURE = '1';
			memset(Feature_Stem, 0x00, sizeof(Feature_Stem));
			memset(Feature_Stem1, 0x00, sizeof(Feature_Stem1));
			memset(Feature_End, 0x00, sizeof(Feature_End));
			nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_MINUS);
			pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);

			CHAR_DELETE(pOneChar);

			pOneChar = GetOneChar(pOriginalString, nFirstOlive, nFirstOlive);
			pNextOneChar = GetOneChar(pOriginalString, nCount, nCount);
			nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_MINUS);
			nCount = ReturnNextPrevCount(pOriginalString, nCount, 1, CALCULATE_MINUS);
			pNextStartCountChar2 = GetStartCountChar(pOriginalString, nCount);
			pLastOneChar = GetOneChar(pOriginalString, strlen(pOriginalString), strlen(pOriginalString));
			isExist = IsExistStemDic(pNextStartCountChar, Feature_Stem);
			isExist = IsExistStemDic(pNextStartCountChar2, Feature_Stem1);
			isExist = IsExistEndDic(pNextOneChar, Feature_End);

			nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_PLUS);
			pPrevOneChar = GetOneChar(pOriginalString, nCount, nCount);
			if (strcmp(pOneChar, pPrevOneChar) == 0)
			{
				delete pPrevOneChar;
				pPrevOneChar = NULL;
			}

			if (strcmp(pOneChar, "이") == 0 && (Feature_Stem[4] == IS_CEHCK_FEATURE ||
				Feature_Stem[1] == IS_CEHCK_FEATURE || Feature_Stem[39] == IS_CEHCK_FEATURE ||
				Feature_Stem[10] == IS_CEHCK_FEATURE))
				//4n
			{
				pEndCountChar = GetEndCountChar(pOriginalString, nFirstOlive);
				nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_PLUS);
				pPrevEndCountChar = GetEndCountChar(pOriginalString, nCount);
				if ((strcmp(pOneChar, pLastOneChar) != 0 && IsExistEndDic(pEndCountChar)) ||
					(IsExistFeature(m_pNewBuffer, END_DIC, E_V) == true && IsExistEndDic(m_pOldBuffer)))//5y
				{
					InsertStemString(pNode, pNextStartCountChar);
					SaveHTokenStem(pNode);
					SaveHTokenStemFeature(pNode, pNextStartCountChar);
					pAddTwoString = AddTwoStr("이", m_pNewBuffer);
					pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
					InsertEndingString(pNode, pAddTwoString2);
					if (IsExistEndDic(pAddTwoString2))
					{
						SaveHTokenEnding(pNode);
						AddHTokenEnding(pNode, 26);
						DeleteHTokenEnding(pNode, 2);
					}
					else
					{
						MakeNewEndingFeature(pNode);
						AddEndingFeature(pNode, "이");
						DeleteHTokenEnding(pNode, 2);
						DeleteHTokenEnding(pNode, 4);
						SaveHTokenEndingNewFeature(pNode);
						SaveHTokenEndingOldFeature(pNode);
						SaveAmbstrEnding(pNode, "이");
						SaveAmbstrEnding(pNode, m_pNewBuffer);
						SaveAmbstrEnding(pNode, m_pOldBuffer);
					}
				}
				else if (IsExistFeature(pPrevEndCountChar, END_DIC, E_Prs) ||
					IsExistFeature(m_pNewBuffer, END_DIC, E_N) ||
					(m_pNewBuffer != NULL && strcmp(m_pNewBuffer, "야") == 0 && m_pOldBuffer != NULL) ||
					(m_pNewBuffer != NULL && strcmp(m_pNewBuffer, "자") == 0 && m_pOldBuffer != NULL) ||
					(m_pNewBuffer != NULL && strcmp(m_pNewBuffer, "유") == 0 && m_pOldBuffer != NULL))
				{
					GoErrorFunction(pNode);
				}
				else if (pPrevOneChar != NULL) //7n
				{
					if ((IsExistFeature(pPrevOneChar, END_DIC, 2) == true && IsExistFeature(pPrevOneChar, END_DIC, 1) == true) ||
						(strcmp(pPrevOneChar, "도") == 0 || strcmp(pPrevOneChar, "는") == 0))
					{
						InsertStemString(pNode, pNextStartCountChar);
						SaveHTokenStem(pNode);
						pAddTwoString = AddTwoStr("이", m_pNewBuffer);
						pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
						InsertEndingString(pNode, pAddTwoString2);
						if (IsExistEndDic(pAddTwoString2))
						{
							SaveHTokenEnding(pNode);
							DeleteHTokenEnding(pNode, 4);
							DeleteHTokenEnding(pNode, 25);
						}
						else
						{
							MakeNewEndingFeature(pNode);
							AddEndingFeature(pNode, "이");
							DeleteHTokenEnding(pNode, 4);
							DeleteHTokenEnding(pNode, 25);
							SaveHTokenEndingNewFeature(pNode);
							SaveHTokenEndingOldFeature(pNode);

							SaveAmbstrEnding(pNode, "이");
							SaveAmbstrEnding(pNode, m_pNewBuffer);
							SaveAmbstrEnding(pNode, m_pOldBuffer);
						}
					}
					else
					{
						InsertStemString(pNode, pNextStartCountChar);
						SaveHTokenStem(pNode);
						pAddTwoString = AddTwoStr("이", m_pNewBuffer);
						pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
						InsertEndingString(pNode, pAddTwoString2);
						if (IsExistEndDic(pAddTwoString2))
						{
							SaveHTokenEnding(pNode);
							AddHTokenEnding(pNode, 26);
							DeleteHTokenEnding(pNode, 2);
						}
						else
						{
							MakeNewEndingFeature(pNode);
							AddEndingFeature(pNode, "이");
							SaveHTokenEndingNewFeature(pNode);
							SaveHTokenEndingOldFeature(pNode);

							AddHTokenEnding(pNode, 26);
							DeleteHTokenEnding(pNode, 4);
							DeleteHTokenEnding(pNode, 2);
							DeleteHTokenEnding(pNode, 25);
							AddHTokenEnding(pNode, 3);

							SaveAmbstrEnding(pNode, "이");
							SaveAmbstrEnding(pNode, m_pNewBuffer);
							SaveAmbstrEnding(pNode, m_pOldBuffer);
						}
					}
				}
				else
				{
					InsertStemString(pNode, pNextStartCountChar);
					SaveHTokenStem(pNode);
					pAddTwoString = AddTwoStr("이", m_pNewBuffer);
					pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
					InsertEndingString(pNode, pAddTwoString2);
					if (IsExistEndDic(pAddTwoString2))
					{
						SaveHTokenEnding(pNode);
						AddHTokenEnding(pNode, 26);
						DeleteHTokenEnding(pNode, 2);
					}
					else
					{
						MakeNewEndingFeature(pNode);
						AddEndingFeature(pNode, "이");
						AddHTokenEnding(pNode, 26);
						DeleteHTokenEnding(pNode, 2);
						DeleteHTokenEnding(pNode, 4);
						SaveHTokenEndingNewFeature(pNode);
						SaveHTokenEndingOldFeature(pNode);


						SaveAmbstrEnding(pNode, "이");
						SaveAmbstrEnding(pNode, m_pNewBuffer);
						SaveAmbstrEnding(pNode, m_pOldBuffer);
					}
				}
			}
			else if (strcmp(pOneChar, "이") == 0 &&
				(Feature_End[2] == IS_CEHCK_FEATURE && Feature_End[1] == IS_CEHCK_FEATURE && Feature_End[24] == IS_CEHCK_FEATURE) &&
				(Feature_Stem1[4] == IS_CEHCK_FEATURE || Feature_Stem1[1] == IS_CEHCK_FEATURE || Feature_Stem1[39] == IS_CEHCK_FEATURE))
				//5n
			{
				pMiddleCountChar = GetMiddleCountChar(pOriginalString, nFirstOlive, nFirstOlive, CALCULATE_MINUS, CALCULATE_STOP);
				InsertStemString(pNode, pNextStartCountChar2);
				SaveHTokenStem(pNode);
				SaveHTokenStemFeature(pNode, pNextStartCountChar2);
				pAddTwoString = AddTwoStr(pMiddleCountChar, m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					AddEndingFeature(pNode, pMiddleCountChar);
					AddEndingFeature(pNode, m_pOldBuffer);
					AddHTokenEnding(pNode, 26);
					SaveAmbstrEnding(pNode, pMiddleCountChar);
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else
			{
				SetOneWordMarsk();
				GoMAR1_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
			}
		}
	}
	else
	{
		GoIMR4_1(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
	}

	CHAR_DELETE(pNewAddTwoString);


	CHAR_DELETE(pTwoChar);
	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pLastOneChar);
	CHAR_DELETE(pNextOneChar);
	CHAR_DELETE(pThisStartCountChar);
	CHAR_DELETE(pNextStartCountChar);
	CHAR_DELETE(pNextStartCountChar2);
	CHAR_DELETE(pPrevStartCountChar);
	CHAR_DELETE(pMiddleCountChar);
	CHAR_DELETE(pEndCountChar);
	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pAddTwoString2);
	CHAR_DELETE(pPrevOneChar);
	CHAR_DELETE(pPrevEndCountChar);
	CHAR_DELETE(tempOneChar); // Added by KKH 2003-11-07

}

void CNAHangulAnaly::GoIMR4_1(H_NODE* pNode, int& nFirstOlive, int& mSecondOlive, int &mSecondAlkong)
{
	char* pOriginalString = NULL;

	char* pOneChar = NULL;
	char* pLastOneChar = NULL;
	char* pPrevOneChar = NULL;
	char* pEndCountChar = NULL;
	char* pThisStartCountChar = NULL;
	char* pNextStartCountChar = NULL;
	char* pNext2StartCountChar = NULL;
	char* pMiddleCountChar = NULL;
	char* pAddTwoString = NULL;
	char* pAddTwoString2 = NULL;
	char* pNextOneChar = NULL;
	char* pNextOneChar2 = NULL;
	char* pNext3StartCountChar = NULL;
	char* tempOneChar = NULL; // Added by KKH 2003-11-07
	char tmpChar[3] = ""; // Added by KKH 2004-01-13

	int nPos = 0;
	int nPos2 = 0;

	int nCount = 0;
	int nBLen = 0; // Added by KKH 2003-11-07

	pOriginalString = pNode->pHToken->strSource;

	pOneChar = GetOneChar(pOriginalString, nFirstOlive, nFirstOlive);
	pThisStartCountChar = GetStartCountChar(pOriginalString, nFirstOlive);

	if (strcmp(pThisStartCountChar, "퍼") == 0) //2n
	{
		InsertStemString(pNode, "푸");
		SaveHTokenStem(pNode);
		if (m_pNewBuffer == NULL)
		{
			MakeNewEndingFeature(pNode);
			AddEndingFeature(pNode, "어");
			// Edited by KKH 2004-01-13
			if (m_pOldBuffer != NULL)
			{
				tmpChar[0] = m_pOldBuffer[0];
				tmpChar[1] = m_pOldBuffer[1];
				tmpChar[2] = 0;
			}
			if (strcmp(tmpChar, "었") != 0 && strcmp(tmpChar, "았") != 0)
			{
				pAddTwoString = AddTwoStr("어", m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
			}
			else
			{
				pAddTwoString2 = AddTwoStr("", m_pOldBuffer);
			}
			// Edited by KKH 2004-01-13
			InsertEndingString(pNode, pAddTwoString2);
			if (IsExistEndDic(pAddTwoString2))
			{
				SaveHTokenEnding(pNode);
				AddHTokenEnding(pNode, 3);
				AddHTokenEnding(pNode, 1);
				AddHTokenEnding(pNode, 28);
			}
			else
			{
				AddHTokenEnding(pNode, 3);
				AddHTokenEnding(pNode, 1);
				AddHTokenEnding(pNode, 28);
				SaveHTokenEndingNewFeature(pNode);
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, m_pNewBuffer);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
			}
		}
		else
		{
			pAddTwoString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
			InsertEndingString(pNode, pAddTwoString);
			if (IsExistEndDic(pAddTwoString))
			{
				SaveHTokenEnding(pNode);
			}
			else
			{
				MakeNewEndingFeature(pNode);
				SaveHTokenEndingNewFeature(pNode);
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, m_pNewBuffer);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
			}
		}
	}
	else if (CompareVariousTable01(pOneChar, 0, false) == true || CompareVariousTable02(pOneChar, 1, false) == true ||
		CompareVariousTable02(pOneChar, 2, false) == true) //3n
	{
		char* pTableValue;
		if (m_nTableNumber == 1)
		{
			pTableValue = Table01[m_nTableIndex].strTwoWord;
		}
		if (m_nTableNumber == 2)
		{
			pTableValue = Table02[m_nTableIndex].strOneWord;
		}
		nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_MINUS);
		pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);

		pNextOneChar = GetOneChar(pOriginalString, nCount, nCount);
		nPos = ReturnNextPrevCount(pOriginalString, nCount, 1, CALCULATE_MINUS);
		pNextOneChar2 = GetOneChar(pOriginalString, nPos, nPos);
		pNext2StartCountChar = GetStartCountChar(pOriginalString, nPos);

		nPos2 = ReturnNextPrevCount(pOriginalString, nPos, 1, CALCULATE_MINUS);
		pNext3StartCountChar = GetStartCountChar(pOriginalString, nPos2);

		pAddTwoString = AddTwoStr(pNextStartCountChar, pTableValue);
		if (IsExistStemDic(pAddTwoString))
		{
			bool isExist = false;
			char Feature_Stem[236];
			char IS_CEHCK_FEATURE = '1';
			memset(Feature_Stem, 0x00, sizeof(Feature_Stem));
			isExist = IsExistStemDic(pAddTwoString, Feature_Stem);
			if (Feature_Stem[4] == IS_CEHCK_FEATURE || Feature_Stem[10] == IS_CEHCK_FEATURE || Feature_Stem[11] == IS_CEHCK_FEATURE)
			{
				SetOneWordMarsk();
				GoMAR1_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
			}
			else
			{
				InsertStemString(pNode, pAddTwoString);
				SaveHTokenStem(pNode);
				MakeNewEndingFeature(pNode);
				AddEndingFeature(pNode, "어");

				CHAR_DELETE(pAddTwoString);

				// Edited by KKH 2004-01-13
				if (m_pOldBuffer != NULL)
				{
					tmpChar[0] = m_pOldBuffer[0];
					tmpChar[1] = m_pOldBuffer[1];
					tmpChar[2] = 0;
				}
				if (strcmp(tmpChar, "었") != 0 && strcmp(tmpChar, "았") != 0)
				{
					pAddTwoString = AddTwoStr("어", m_pNewBuffer);
					pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				}
				else
				{
					pAddTwoString2 = AddTwoStr("", m_pOldBuffer);
				}
				// Edited by KKH 2004-01-13
				// Added by KKH 2003-11-07
				nBLen = strlen(pAddTwoString2);
				for (int i = 0; i < nBLen; i = i + 2)
				{
					tempOneChar = GetOneChar(pAddTwoString2, nBLen - i, nBLen - i - 1);
					if (strcmp(tempOneChar, "음") == 0 ||
						strcmp(tempOneChar, "기") == 0)
					{
						DeleteHTokenStem(pNode, 7);
						AddHTokenStem(pNode, 39);
						CHAR_DELETE(tempOneChar);
						break;
					}
					else
					{
						CHAR_DELETE(tempOneChar);
					}
				}
				// Added by KKH 2003-11-07
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					AddHTokenEnding(pNode, 3);
					AddHTokenEnding(pNode, 1);
					AddHTokenEnding(pNode, 28);
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
		}
		else if (strcmp(pOneChar, "떠") == 0 || strcmp(pOneChar, "거") == 0 || strcmp(pOneChar, "러") == 0)
		{
			CHAR_DELETE(pAddTwoString);
			pAddTwoString = AddTwoStr(pThisStartCountChar, "ㅎ");
			if (IsExistStemDic(pAddTwoString))
			{
				InsertStemString(pNode, pAddTwoString);
				SaveHTokenStem(pNode);
				pAddTwoString2 = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else
			{
				SetOneWordMarsk();
				GoMAR1_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
			}
		}
		else if (strcmp(pOneChar, "켜") == 0 && strcmp(pNextOneChar, "시") == 0
			&& IsExistFeature(pNext2StartCountChar, STEM_DIC, S_N) == true)
		{
			CHAR_DELETE(pAddTwoString);
			pAddTwoString = AddTwoStr(pNext2StartCountChar, "시키");
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStemFeature(pNode, pNext2StartCountChar);

			DeleteHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_V);
			AddHTokenStem(pNode, S_Act);

			CHAR_DELETE(pAddTwoString);
			pAddTwoString = AddTwoStr("어", m_pNewBuffer);
			pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
			InsertEndingString(pNode, pAddTwoString2);
			if (IsExistEndDic(pAddTwoString2))
			{
				SaveHTokenEnding(pNode);
			}
			else
			{
				MakeNewEndingFeature(pNode);
				SaveHTokenEndingNewFeature(pNode);
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, m_pNewBuffer);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
			}
		}
		else if (strcmp(pOneChar, "워") == 0 && strcmp(pNextOneChar, "다") == 0
			&& IsExistFeature(pNext2StartCountChar, STEM_DIC, S_N) == true)
		{
			CHAR_DELETE(pAddTwoString);
			pAddTwoString = AddTwoStr(pNext2StartCountChar, "답");
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStemFeature(pNode, pNext2StartCountChar);

			DeleteHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_V);
			AddHTokenStem(pNode, S_Sta);

			CHAR_DELETE(pAddTwoString);
			pAddTwoString = AddTwoStr("어", m_pNewBuffer);
			pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
			InsertEndingString(pNode, pAddTwoString2);
			if (IsExistEndDic(pAddTwoString2))
			{
				SaveHTokenEnding(pNode);
			}
			else
			{
				MakeNewEndingFeature(pNode);
				SaveHTokenEndingNewFeature(pNode);
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, m_pNewBuffer);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
			}
		}
		else if (strcmp(pOneChar, "워") == 0 && strcmp(pNextOneChar, "러") == 0 &&
			strcmp(pNextOneChar2, "스") == 0 && IsExistFeature(pNext3StartCountChar, STEM_DIC, S_N) == true)
		{
			CHAR_DELETE(pAddTwoString);
			pAddTwoString = AddTwoStr(pNext3StartCountChar, "스럽");
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStemFeature(pNode, pNext3StartCountChar);

			DeleteHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_V);
			AddHTokenStem(pNode, S_Sta);

			CHAR_DELETE(pAddTwoString);
			pAddTwoString = AddTwoStr("어", m_pNewBuffer);
			pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
			InsertEndingString(pNode, pAddTwoString2);
			if (IsExistEndDic(pAddTwoString2))
			{
				SaveHTokenEnding(pNode);
			}
			else
			{
				MakeNewEndingFeature(pNode);
				SaveHTokenEndingNewFeature(pNode);
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, m_pNewBuffer);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
			}
		}
		else
		{
			SetOneWordMarsk();
			GoMAR1_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
		}
	}
	else if (CompareVariousTable03(pOneChar, 0, false) == true) //4n
	{
		nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_MINUS);
		pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
		if (IsExistStemDic(pThisStartCountChar)) //5y
		{
			InsertStemString(pNode, pThisStartCountChar);
			SaveHTokenStem(pNode);
			pAddTwoString2 = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
			InsertEndingString(pNode, pAddTwoString2);
			if (IsExistEndDic(pAddTwoString2))
			{
				SaveHTokenEnding(pNode);
			}
			else
			{
				MakeNewEndingFeature(pNode);
				SaveHTokenEndingNewFeature(pNode);
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, m_pNewBuffer);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
			}
		}
		else if (IsExistStemDic(pNextStartCountChar)) //6n
		{
			pLastOneChar = GetOneChar(pOriginalString, strlen(pOriginalString), strlen(pOriginalString));
			nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_PLUS);
			pPrevOneChar = GetOneChar(pOriginalString, nCount, nCount);
			if (m_pOldBuffer == NULL && strcmp(pOneChar, pLastOneChar) == 0 &&
				strcmp(pOneChar, "되") == 0)
			{
				InsertStemString(pNode, pNextStartCountChar);
				SaveHTokenStem(pNode);
				InsertEndingString(pNode, pOneChar);
				SaveHTokenEnding(pNode);
			}
			else if (strcmp(pOneChar, "하") == 0 &&
				(strcmp(pPrevOneChar, "에") == 0 ||
				((strcmp(pPrevOneChar, "고") == 0 || strcmp(pPrevOneChar, "며") == 0) &&
					IsExistFeature(pNextStartCountChar, STEM_DIC, 16) == true)))
			{
				bool bSame = false;
				pMiddleCountChar = GetMiddleCountChar(pOriginalString, nFirstOlive, nFirstOlive, CALCULATE_STOP, CALCULATE_PLUS);
				nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_PLUS);
				if (strlen(pOriginalString) == (unsigned int)nCount)
				{
					bSame = true;
				}
				nCount = ReturnNextPrevCount(pOriginalString, nCount, 1, CALCULATE_PLUS);
				pEndCountChar = GetEndCountChar(pOriginalString, nCount);

				InsertStemString(pNode, pNextStartCountChar);
				SaveHTokenStem(pNode);
				if (bSame == false)
				{
					CHAR_DELETE(pAddTwoString);

					pAddTwoString = AddTwoStr(pMiddleCountChar, pEndCountChar);
					InsertEndingString(pNode, pAddTwoString);
					if (IsExistEndDic(pAddTwoString))
					{
						SaveHTokenEnding(pNode);
					}
					else
					{
						MakeNewEndingFeature(pNode);
						AddEndingFeature(pNode, pMiddleCountChar);
						AddHTokenEnding(pNode, 2);
						AddHTokenEnding(pNode, 1);
						AddHTokenEnding(pNode, 8);
						DeleteHTokenEnding(pNode, 3);
						AddEndingFeature(pNode, pEndCountChar);
						SaveAmbstrEnding(pNode, pMiddleCountChar);
						SaveAmbstrEnding(pNode, pEndCountChar);
					}
				}
				else
				{
					InsertEndingString(pNode, pMiddleCountChar);
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, 2);
					AddHTokenEnding(pNode, 1);
					AddHTokenEnding(pNode, 8);
					DeleteHTokenEnding(pNode, 3);
					SaveAmbstrEnding(pNode, pMiddleCountChar);
				}
			}
			else if (strcmp(pOneChar, "되") == 0)
			{
				pAddTwoString = AddTwoStr(pNextStartCountChar, Table03[m_nTableIndex].strOneWord);
				InsertStemString(pNode, pAddTwoString);
				SaveHTokenStem(pNode);
				SaveHTokenStemFeature(pNode, pNextStartCountChar);
				AddHTokenStem(pNode, 7);
				AddHTokenStem(pNode, 2);
				AddHTokenStem(pNode, 35);
				AddHTokenStem(pNode, S_T078);
				DeleteHTokenStem(pNode, 4);
				DeleteHTokenStem(pNode, 1);
				DeleteHTokenStem(pNode, 10);
				DeleteHTokenStem(pNode, 8);
				DeleteHTokenStem(pNode, S_T104);
				pAddTwoString2 = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);

				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else
			{
				pAddTwoString = AddTwoStr(pNextStartCountChar, Table03[m_nTableIndex].strOneWord);
				InsertStemString(pNode, pAddTwoString);
				SaveHTokenStem(pNode);
				SaveHTokenStemFeature(pNode, pNextStartCountChar);
				AddHTokenStem(pNode, 7);
				AddHTokenStem(pNode, 2);
				AddHTokenStem(pNode, S_T077);
				DeleteHTokenStem(pNode, 4);
				DeleteHTokenStem(pNode, 1);
				DeleteHTokenStem(pNode, 10);

				if ((pPrevOneChar != NULL && strcmp(pPrevOneChar, "기") == 0) ||
					(m_pOldBuffer != NULL && strstr(m_pOldBuffer, "음") != 0))
				{
					AddHTokenStem(pNode, 39);
				}

				pAddTwoString2 = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
		}
		else
		{
			SetOneWordMarsk();
			GoMAR1_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
		}
	}
	else if (CompareVariousTable03(pOneChar, 1, false) == true) //5n
	{
		nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_MINUS);
		pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
		pPrevOneChar = GetOneChar(pOriginalString, nCount, nCount);
		nCount = ReturnNextPrevCount(pOriginalString, nCount, 1, CALCULATE_MINUS);
		pNext2StartCountChar = GetStartCountChar(pOriginalString, nCount);

		pAddTwoString = AddTwoStr(pNextStartCountChar, Table03[m_nTableIndex].strOneWord);
		if (IsExistStemDic(pAddTwoString)) //6y
		{
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStem(pNode);

			CHAR_DELETE(pAddTwoString);

			MakeNewEndingFeature(pNode);
			// Edited by KKH 2004-01-13
			if (m_pOldBuffer != NULL)
			{
				tmpChar[0] = m_pOldBuffer[0];
				tmpChar[1] = m_pOldBuffer[1];
				tmpChar[2] = 0;
			}
			if (strcmp(tmpChar, "었") != 0 && strcmp(tmpChar, "았") != 0)
			{
				AddEndingFeature(pNode, "어");
				pAddTwoString = AddTwoStr("어", m_pNewBuffer);
				pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
			}
			else
			{
				pAddTwoString2 = AddTwoStr("", m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString2);
			}
			// Added by KKH 2003-11-07
			nBLen = strlen(pAddTwoString2);
			for (int i = 0; i < nBLen; i = i + 2)
			{
				tempOneChar = GetOneChar(pAddTwoString2, nBLen - i, nBLen - i - 1);
				if (strcmp(tempOneChar, "음") == 0 ||
					strcmp(tempOneChar, "기") == 0)
				{
					DeleteHTokenStem(pNode, 7);
					AddHTokenStem(pNode, 39);
					CHAR_DELETE(tempOneChar);
					break;
				}
				else
				{
					CHAR_DELETE(tempOneChar);
				}
			}
			// Added by KKH 2003-11-07
			if (IsExistEndDic(pAddTwoString2))
			{
				SaveHTokenEnding(pNode);
			}
			else
			{
				SaveHTokenEndingNewFeature(pNode);
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, m_pNewBuffer);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
			}
		}
		else if (IsExistStemDic(pNextStartCountChar)) //7n
		{
			if (strcmp(pOneChar, "되") == 0 || strcmp(pOneChar, "돼") == 0)
			{
				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr(pNextStartCountChar, Table03[m_nTableIndex].strOneWord);
				InsertStemString(pNode, pAddTwoString);
				SaveHTokenStem(pNode);
				SaveHTokenStemFeature(pNode, pNextStartCountChar);
				AddHTokenStem(pNode, 7);
				AddHTokenStem(pNode, 2);
				AddHTokenStem(pNode, 35);
				AddHTokenStem(pNode, S_T078);
				DeleteHTokenStem(pNode, 4);
				DeleteHTokenStem(pNode, 1);
				DeleteHTokenStem(pNode, 8);
				DeleteHTokenStem(pNode, S_T104);

				CHAR_DELETE(pAddTwoString);

				MakeNewEndingFeature(pNode);
				AddEndingFeature(pNode, "어");

				// Edited by KKH 2004-01-13
				if (m_pOldBuffer != NULL)
				{
					tmpChar[0] = m_pOldBuffer[0];
					tmpChar[1] = m_pOldBuffer[1];
					tmpChar[2] = 0;
				}
				if (strcmp(tmpChar, "었") != 0 && strcmp(tmpChar, "았") != 0)
				{
					pAddTwoString = AddTwoStr("어", m_pNewBuffer);
					pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
				}
				else
				{
					pAddTwoString2 = AddTwoStr("", m_pOldBuffer);
				}
				// Edited by KKH 2004-01-13
				InsertEndingString(pNode, pAddTwoString2);
				if (IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else if (IsExistFeature(pNextStartCountChar, STEM_DIC, S_Conc) == false &&
				IsExistFeature(pNextStartCountChar, STEM_DIC, S_Tm) == false)
			{
				CHAR_DELETE(pAddTwoString);

				pAddTwoString = AddTwoStr(pNextStartCountChar, Table03[m_nTableIndex].strOneWord);
				InsertStemString(pNode, pAddTwoString);
				SaveHTokenStem(pNode);
				SaveHTokenStemFeature(pNode, pNextStartCountChar);
				AddHTokenStem(pNode, 7);
				AddHTokenStem(pNode, 2);
				AddHTokenStem(pNode, 8);
				AddHTokenStem(pNode, S_T077);
				DeleteHTokenStem(pNode, 4);
				DeleteHTokenStem(pNode, 1);

				if (m_pOldBuffer != NULL &&
					(strstr(m_pOldBuffer, "기") != 0 && strstr(m_pOldBuffer, "음") != 0))
				{
					AddHTokenStem(pNode, 39);
					DeleteHTokenStem(pNode, 8);
				}

				CHAR_DELETE(pAddTwoString);

				MakeNewEndingFeature(pNode);
				// Edited by KKH 2004-01-12
				if (m_pOldBuffer != NULL &&
					strstr(m_pOldBuffer, "었") != 0 &&
					strstr(m_pOldBuffer, "았") != 0 &&
					strstr(m_pOldBuffer, "ㅆ") != 0)
				{
					AddEndingFeature(pNode, "어");
					pAddTwoString = AddTwoStr("어", m_pNewBuffer);
					pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
					InsertEndingString(pNode, pAddTwoString2);
				}
				else if (m_pOldBuffer != NULL)
				{
					InsertEndingString(pNode, m_pOldBuffer);
				}

				if (pAddTwoString2 != NULL && IsExistEndDic(pAddTwoString2))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					if (m_pNewBuffer != NULL)
					{
						SaveAmbstrEnding(pNode, m_pNewBuffer);
					}
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
				// Edited by KKH 2004-01-12
			}
			else
			{
				GoErrorFunction(pNode);
			}
		}
		else if (strcmp(pPrevOneChar, "당") == 0 &&
			IsExistFeature(pNext2StartCountChar, STEM_DIC, S_N) == true)
		{
			CHAR_DELETE(pAddTwoString);
			pAddTwoString = AddTwoStr(pNext2StartCountChar, "당하");
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStemFeature(pNode, pNext2StartCountChar);

			DeleteHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_V);
			AddHTokenStem(pNode, S_Fnc);
			AddHTokenStem(pNode, S_T159); // Added by KKH 2003-11-18

			CHAR_DELETE(pAddTwoString);

			MakeNewEndingFeature(pNode);
			AddEndingFeature(pNode, "어");
			pAddTwoString = AddTwoStr("어", m_pNewBuffer);
			pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
			InsertEndingString(pNode, pAddTwoString2);
			if (IsExistEndDic(pAddTwoString2))
			{
				SaveHTokenEnding(pNode);
			}
			else
			{
				SaveHTokenEndingNewFeature(pNode);
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, m_pNewBuffer);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
			}
		}
		else
		{
			SetOneWordMarsk();
			GoMAR1_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
		}
	}
	else
	{
		GoIMR4_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
	}

	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pNextOneChar);
	CHAR_DELETE(pLastOneChar);
	CHAR_DELETE(pPrevOneChar);
	CHAR_DELETE(pThisStartCountChar);
	CHAR_DELETE(pNextStartCountChar);
	CHAR_DELETE(pNext2StartCountChar);
	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pAddTwoString2);
	CHAR_DELETE(pMiddleCountChar);
	CHAR_DELETE(pEndCountChar);
	CHAR_DELETE(tempOneChar); // Added by KKH 2003-11-07

							  //누락된 DELETE 대상 추가  2017-08-15
	CHAR_DELETE(pNextOneChar2);
	CHAR_DELETE(pNext3StartCountChar);
}

void CNAHangulAnaly::GoIMR4_2(H_NODE* pNode, int& nFirstOlive, int& mSecondOlive, int &mSecondAlkong)
{
	char* pOriginalString = NULL;

	char* pOneChar = NULL;
	char* pNextOneChar = NULL;
	char* pNextJongChar = NULL;
	char* pThisStartCountChar = NULL;
	char* pThisStartCountChar1 = NULL;
	char* pNextStartCountChar = NULL;
	char* pMinusTwoString = NULL;
	char* pAddTwoString = NULL;
	char* pAddTwoString1 = NULL;
	char* pAddTwoString2 = NULL;
	char* pAddTwoString3 = NULL;
	char* pAddTwoString4 = NULL;
	char* pAddTwoString5 = NULL; // Added by KKH 2004-01-13
	char* pNext2StartCountChar = NULL;
	char* pNext3StartCountChar = NULL;
	char* pNextOneChar2 = NULL;
	char tmpChar[3] = ""; // Added by KKH 2004-01-13

	int nCount = 0;
	int nPos = 0;
	int nPos2 = 0;

	char Feature_Stem[236];
	char IS_CEHCK_FEATURE = '1';
	memset(Feature_Stem, 0x00, sizeof(Feature_Stem));
	bool isExist = false;

	pOriginalString = pNode->pHToken->strSource;
	pOneChar = GetOneChar(pOriginalString, nFirstOlive, nFirstOlive);
	if (strcmp(pOneChar, "라") == 0 || strcmp(pOneChar, "러") == 0)
	{
		nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_MINUS);
		pNextOneChar = GetOneChar(pOriginalString, nCount, nCount);
		pNextJongChar = GetChojungJongsungChar(pNextOneChar, CHAR_JONGSUNG);
		pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);
		pMinusTwoString = MinusTwoStr(pNextStartCountChar, pNextOneChar, nCount);
		pAddTwoString = AddTwoStr(pMinusTwoString, "르");
		pAddTwoString2 = AddTwoStr(pNextStartCountChar, "르");

		nPos = ReturnNextPrevCount(pOriginalString, nCount, 1, CALCULATE_MINUS);
		pNext2StartCountChar = GetStartCountChar(pOriginalString, nPos);

		if (strcmp(pNextJongChar, "ㄹ") == 0)
		{
			if (IsExistStemDic(pAddTwoString))
			{
				isExist = IsExistStemDic(pAddTwoString, Feature_Stem);
				if (Feature_Stem[66] == IS_CEHCK_FEATURE)
				{
					InsertStemString(pNode, pAddTwoString);
					SaveHTokenStem(pNode);
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "어");
					// Edited by KKH 2004-01-13
					if (m_pOldBuffer != NULL)
					{
						tmpChar[0] = m_pOldBuffer[0];
						tmpChar[1] = m_pOldBuffer[1];
						tmpChar[2] = 0;
					}
					if (strcmp(tmpChar, "었") != 0 && strcmp(tmpChar, "았") != 0)
					{
						pAddTwoString3 = AddTwoStr("어", m_pNewBuffer);
						pAddTwoString4 = AddTwoStr(pAddTwoString3, m_pOldBuffer);
					}
					else
					{
						pAddTwoString4 = AddTwoStr("", m_pOldBuffer);
					}
					// Edited by KKH 2004-01-13
					InsertEndingString(pNode, pAddTwoString4);
					if (IsExistEndDic(pAddTwoString4))
					{
						SaveHTokenEnding(pNode);
						AddHTokenEnding(pNode, 3);
						AddHTokenEnding(pNode, 1);
						AddHTokenEnding(pNode, 28);
					}
					else
					{
						AddHTokenEnding(pNode, 3);
						AddHTokenEnding(pNode, 1);
						AddHTokenEnding(pNode, 28);
						SaveHTokenEndingNewFeature(pNode);
						SaveHTokenEndingOldFeature(pNode);
						SaveAmbstrEnding(pNode, m_pNewBuffer);
						SaveAmbstrEnding(pNode, m_pOldBuffer);
					}
				}
				else
				{
					SetOneWordMarsk();
					GoMAR1_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
				}
			}
			else if (IsExistStemDic(pAddTwoString2))
			{
				isExist = IsExistStemDic(pAddTwoString2, Feature_Stem);
				if (Feature_Stem[69] == IS_CEHCK_FEATURE)
				{
					InsertStemString(pNode, pAddTwoString2);
					SaveHTokenStem(pNode);
					MakeNewEndingFeature(pNode);
					AddEndingFeature(pNode, "어");
					// Edited by KKH 2004-01-13
					if (m_pOldBuffer != NULL)
					{
						tmpChar[0] = m_pOldBuffer[0];
						tmpChar[1] = m_pOldBuffer[1];
						tmpChar[2] = 0;
					}
					if (strcmp(tmpChar, "었") != 0 && strcmp(tmpChar, "았") != 0)
					{
						pAddTwoString3 = AddTwoStr("어", m_pNewBuffer);
						pAddTwoString4 = AddTwoStr(pAddTwoString3, m_pOldBuffer);
					}
					else
					{
						pAddTwoString4 = AddTwoStr("", m_pOldBuffer);
					}
					// Edited by KKH 2004-01-13
					InsertEndingString(pNode, pAddTwoString4);
					if (IsExistEndDic(pAddTwoString4))
					{
						SaveHTokenEnding(pNode);
						AddHTokenEnding(pNode, 3);
						AddHTokenEnding(pNode, 1);
						AddHTokenEnding(pNode, 28);
					}
					else
					{
						AddHTokenEnding(pNode, 3);
						AddHTokenEnding(pNode, 1);
						AddHTokenEnding(pNode, 28);
						SaveHTokenEndingNewFeature(pNode);
						SaveHTokenEndingOldFeature(pNode);
						SaveAmbstrEnding(pNode, m_pNewBuffer);
						SaveAmbstrEnding(pNode, m_pOldBuffer);
					}
				}
				else
				{
					SetOneWordMarsk();
					GoMAR1_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
				}
			}
			else
			{
				SetOneWordMarsk();
				GoMAR1_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
			}
		}
		else if (IsExistStemDic(pAddTwoString2, Feature_Stem) &&
			Feature_Stem != NULL && Feature_Stem[S_V] == '1') // Edited by KKH 2004-01-13
		{
			if (Feature_Stem[69] == IS_CEHCK_FEATURE)
			{
				InsertStemString(pNode, pAddTwoString2);
				SaveHTokenStem(pNode);
				MakeNewEndingFeature(pNode);
				AddEndingFeature(pNode, "어");
				// Edited by KKH 2004-01-13
				if (m_pOldBuffer != NULL)
				{
					tmpChar[0] = m_pOldBuffer[0];
					tmpChar[1] = m_pOldBuffer[1];
					tmpChar[2] = 0;
				}
				if (strcmp(tmpChar, "었") != 0 && strcmp(tmpChar, "았") != 0)
				{
					pAddTwoString3 = AddTwoStr("어", m_pNewBuffer);
					pAddTwoString4 = AddTwoStr(pAddTwoString3, m_pOldBuffer);
				}
				else
				{
					pAddTwoString4 = AddTwoStr("", m_pOldBuffer);
				}
				// Edited by KKH 2004-01-13
				InsertEndingString(pNode, pAddTwoString4);
				if (IsExistEndDic(pAddTwoString4))
				{
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, 3);
					AddHTokenEnding(pNode, 1);
					AddHTokenEnding(pNode, 28);
				}
				else
				{
					AddHTokenEnding(pNode, 3);
					AddHTokenEnding(pNode, 1);
					AddHTokenEnding(pNode, 28);
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else
			{
				SetOneWordMarsk();
				GoMAR1_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
			}
		}
		else if (IsExistStemDic(pNextStartCountChar, Feature_Stem) &&
			Feature_Stem != NULL && Feature_Stem[S_V] == '1') // Edited by KKH 2004-01-13
		{
			if (Feature_Stem[S_Ruir] == IS_CEHCK_FEATURE)
			{
				InsertStemString(pNode, pNextStartCountChar);
				SaveHTokenStem(pNode);
				MakeNewEndingFeature(pNode);
				AddEndingFeature(pNode, "어");
				// Edited by KKH 2004-01-13
				if (m_pOldBuffer != NULL)
				{
					tmpChar[0] = m_pOldBuffer[0];
					tmpChar[1] = m_pOldBuffer[1];
					tmpChar[2] = 0;
				}
				if (strcmp(tmpChar, "었") != 0 && strcmp(tmpChar, "았") != 0)
				{
					pAddTwoString3 = AddTwoStr("어", m_pNewBuffer);
					pAddTwoString4 = AddTwoStr(pAddTwoString3, m_pOldBuffer);
				}
				else
				{
					pAddTwoString4 = AddTwoStr("", m_pOldBuffer);
				}
				// Edited by KKH 2004-01-13
				InsertEndingString(pNode, pAddTwoString4);
				if (IsExistEndDic(pAddTwoString4))
				{
					SaveHTokenEnding(pNode);
					AddHTokenEnding(pNode, 3);
					AddHTokenEnding(pNode, 1);
					AddHTokenEnding(pNode, 28);
				}
				else
				{
					AddHTokenEnding(pNode, 3);
					AddHTokenEnding(pNode, 1);
					AddHTokenEnding(pNode, 28);
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else
			{
				SetOneWordMarsk();
				GoMAR1_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
			}
		}
		else if (ReplaceHgngulTokenSource(pNode, nFirstOlive, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA13))
		{
			pOriginalString = pNode->pHToken->strSource;

			CHAR_DELETE(pThisStartCountChar);

			pThisStartCountChar = GetStartCountChar(pOriginalString, nFirstOlive);
			InsertStemString(pNode, pThisStartCountChar);
			SaveHTokenStem(pNode);
			pAddTwoString3 = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
			InsertEndingString(pNode, pAddTwoString3);
			if (IsExistEndDic(pAddTwoString3))
			{
				SaveHTokenEnding(pNode);
			}
			else
			{
				MakeNewEndingFeature(pNode);
				SaveHTokenEndingNewFeature(pNode);
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, m_pNewBuffer);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
			}
		}
		else if (ReplaceHgngulTokenSource(pNode, nFirstOlive, FORCE_REPLACE_ENDWORD, REPLACE_HANGUL_JA05))
		{
			pOriginalString = pNode->pHToken->strSource;

			CHAR_DELETE(pThisStartCountChar);

			pThisStartCountChar = GetStartCountChar(pOriginalString, nFirstOlive);
			InsertStemString(pNode, pThisStartCountChar);
			SaveHTokenStem(pNode);
			pAddTwoString3 = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
			InsertEndingString(pNode, pAddTwoString3);
			if (IsExistEndDic(pAddTwoString3))
				SaveHTokenEnding(pNode);
			else
			{
				MakeNewEndingFeature(pNode);
				SaveHTokenEndingNewFeature(pNode);
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, m_pNewBuffer);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
			}
		}
		else if (strcmp(pNextOneChar, "스") == 0 &&
			(IsExistFeature(pNext2StartCountChar, STEM_DIC, S_N) ||
				IsExistFeature(pNext2StartCountChar, STEM_DIC, S_Amb)))
		{
			CHAR_DELETE(pAddTwoString);
			pAddTwoString = AddTwoStr(pNext2StartCountChar, "스럽");
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStemFeature(pNode, pNext2StartCountChar);

			DeleteHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_V);
			AddHTokenStem(pNode, S_Sta);

			if (m_pOldBuffer != NULL && strcmp(m_pOldBuffer, "우ㄴ") == 0)
			{
				CHAR_DELETE(m_pOldBuffer);
				pAddTwoString = AddTwoStr("ㄴ", m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString);
				if (IsExistEndDic(pAddTwoString))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else if (m_pNewBuffer != NULL && strcmp(m_pNewBuffer, "우") == 0)
			{
				InsertEndingString(pNode, m_pOldBuffer);
				if (IsExistEndDic(m_pOldBuffer))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			else
			{
				pAddTwoString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString);
				if (IsExistEndDic(pAddTwoString))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
		}
		else
		{
			ReplaceHangulJongsungNull(pNode, nFirstOlive);
			SetOneWordMarsk();
			GoMAR1_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
		}
	}
	else if (CompareVariousTable04(pOneChar, 3, false) == true)
	{
		nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_MINUS);
		pThisStartCountChar = GetStartCountChar(pOriginalString, nCount);
		pThisStartCountChar1 = GetStartCountChar(pOriginalString, nFirstOlive);
		if (pThisStartCountChar == NULL || strlen(pThisStartCountChar) <= 0)
		{
			SetOneWordMarsk();
			GoMAR1_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
		}
		else
		{
			pAddTwoString = AddTwoStr(pThisStartCountChar, Table04[m_nTableIndex].strOneWord);
			pAddTwoString5 = AddTwoStr(pThisStartCountChar, Table04[m_nTableIndex + 1].strOneWord); // Added by KKH 2004-01-13
			if (IsExistStemDic(pAddTwoString))
			{
				if (m_pOldBuffer == NULL)
				{
					InsertStemString(pNode, pAddTwoString);
					SaveHTokenStem(pNode);
					pAddTwoString3 = AddTwoStr("어", m_pNewBuffer);
					InsertEndingString(pNode, pAddTwoString3);
					if (IsExistEndDic(pAddTwoString3))
					{
						SaveHTokenEnding(pNode);
					}
					else
					{
						MakeNewEndingFeature(pNode);
						AddEndingFeature(pNode, "어");
						AddHTokenEnding(pNode, 3);
						AddHTokenEnding(pNode, 1);
						AddHTokenEnding(pNode, 28);
						SaveHTokenEndingNewFeature(pNode);
						SaveAmbstrEnding(pNode, "어");
						SaveAmbstrEnding(pNode, m_pNewBuffer);
					}
				}
				else
				{
					pAddTwoString3 = AddTwoStr(pThisStartCountChar, Table04[m_nTableIndex].strOneWord);
					InsertStemString(pNode, pAddTwoString3);
					SaveHTokenStem(pNode);

					CHAR_DELETE(pAddTwoString3);

					// Edited by KKH 2004-01-13
					if (m_pOldBuffer != NULL)
					{
						tmpChar[0] = m_pOldBuffer[0];
						tmpChar[1] = m_pOldBuffer[1];
						tmpChar[2] = 0;
					}
					if (strcmp(tmpChar, "었") != 0 && strcmp(tmpChar, "았") != 0)
					{
						pAddTwoString3 = AddTwoStr("어", m_pNewBuffer);
						pAddTwoString4 = AddTwoStr(pAddTwoString3, m_pOldBuffer);
					}
					else
					{
						pAddTwoString4 = AddTwoStr("", m_pOldBuffer);
					}
					// Edited by KKH 2004-01-13
					InsertEndingString(pNode, pAddTwoString4);
					if (IsExistEndDic(pAddTwoString4))
					{
						SaveHTokenEnding(pNode);
						AddHTokenEnding(pNode, 3);
						AddHTokenEnding(pNode, 1);
						AddHTokenEnding(pNode, 28);
					}
					else
					{
						MakeNewEndingFeature(pNode);
						AddEndingFeature(pNode, "어");
						AddHTokenEnding(pNode, 3);
						AddHTokenEnding(pNode, 1);
						AddHTokenEnding(pNode, 28);
						SaveHTokenEndingNewFeature(pNode);
						SaveHTokenEndingOldFeature(pNode);
						SaveAmbstrEnding(pNode, "어");
						SaveAmbstrEnding(pNode, m_pNewBuffer);
						SaveAmbstrEnding(pNode, m_pOldBuffer);
					}
				}
			}
			// Added by KKH 2004-01-13
			else if (IsExistStemDic(pAddTwoString5))
			{
				if (m_pOldBuffer == NULL)
				{
					InsertStemString(pNode, pAddTwoString5);
					SaveHTokenStem(pNode);

					pAddTwoString3 = AddTwoStr("어", m_pNewBuffer);
					InsertEndingString(pNode, pAddTwoString3);
					if (IsExistEndDic(pAddTwoString3))
					{
						SaveHTokenEnding(pNode);
					}
					else
					{
						MakeNewEndingFeature(pNode);
						AddEndingFeature(pNode, "어");
						AddHTokenEnding(pNode, 3);
						AddHTokenEnding(pNode, 1);
						AddHTokenEnding(pNode, 28);
						SaveHTokenEndingNewFeature(pNode);
						SaveAmbstrEnding(pNode, "어");
						SaveAmbstrEnding(pNode, m_pNewBuffer);
					}
				}
				else
				{
					pAddTwoString3 = AddTwoStr(pThisStartCountChar, Table04[m_nTableIndex + 1].strOneWord);
					InsertStemString(pNode, pAddTwoString3);
					SaveHTokenStem(pNode);

					CHAR_DELETE(pAddTwoString3);

					// Edited by KKH 2004-01-13
					if (m_pOldBuffer != NULL)
					{
						tmpChar[0] = m_pOldBuffer[0];
						tmpChar[1] = m_pOldBuffer[1];
						tmpChar[2] = 0;
					}
					if (strcmp(tmpChar, "었") != 0 && strcmp(tmpChar, "았") != 0)
					{
						pAddTwoString3 = AddTwoStr("어", m_pNewBuffer);
						pAddTwoString4 = AddTwoStr(pAddTwoString3, m_pOldBuffer);
					}
					else
					{
						pAddTwoString4 = AddTwoStr("", m_pOldBuffer);
					}
					// Edited by KKH 2004-01-13
					InsertEndingString(pNode, pAddTwoString4);
					if (IsExistEndDic(pAddTwoString4))
					{
						SaveHTokenEnding(pNode);
						AddHTokenEnding(pNode, 3);
						AddHTokenEnding(pNode, 1);
						AddHTokenEnding(pNode, 28);
					}
					else
					{
						MakeNewEndingFeature(pNode);
						AddEndingFeature(pNode, "어");
						AddHTokenEnding(pNode, 3);
						AddHTokenEnding(pNode, 1);
						AddHTokenEnding(pNode, 28);
						SaveHTokenEndingNewFeature(pNode);
						SaveHTokenEndingOldFeature(pNode);
						SaveAmbstrEnding(pNode, "어");
						SaveAmbstrEnding(pNode, m_pNewBuffer);
						SaveAmbstrEnding(pNode, m_pOldBuffer);
					}
				}
			}
			// Added by KKH 2004-01-13
			else if (IsExistStemDic(pThisStartCountChar1))
			{
				if (IsExistFeature(pThisStartCountChar1, STEM_DIC, 4) == true ||
					IsExistFeature(pThisStartCountChar1, STEM_DIC, 10) == true ||
					IsExistFeature(pThisStartCountChar1, STEM_DIC, 11) == true)
				{
					SetOneWordMarsk();
					GoMAR1_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
				}
				else
				{
					InsertStemString(pNode, pThisStartCountChar1);
					SaveHTokenStem(pNode);
					pAddTwoString3 = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
					InsertEndingString(pNode, pAddTwoString3);
					if (IsExistEndDic(pAddTwoString3))
					{
						SaveHTokenEnding(pNode);
					}
					else
					{
						MakeNewEndingFeature(pNode);
						SaveHTokenEndingNewFeature(pNode);
						SaveHTokenEndingOldFeature(pNode);
						SaveAmbstrEnding(pNode, m_pNewBuffer);
						SaveAmbstrEnding(pNode, m_pOldBuffer);
					}
				}
			}
			else
			{
				SetOneWordMarsk();
				GoMAR1_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
			}
		}
	}
	else if (CompareVariousTable04(pOneChar, 1, false) == true || CompareVariousTable04(pOneChar, 2, false) == true)
	{
		nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_MINUS);
		pThisStartCountChar = GetStartCountChar(pOriginalString, nCount);
		pAddTwoString = AddTwoStr(pThisStartCountChar, Table04[m_nTableIndex].strOneWord);

		CHAR_DELETE(pThisStartCountChar);

		pThisStartCountChar = GetStartCountChar(pOriginalString, nFirstOlive);
		if (IsExistStemDic(pAddTwoString))
		{
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStem(pNode);
			if (m_nTableString == 2)
			{
				pAddTwoString1 = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
				InsertEndingString(pNode, pAddTwoString1);
				if (IsExistEndDic(pAddTwoString1))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					MakeNewEndingFeature(pNode);
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
			if (m_nTableString == 3)
			{
				pAddTwoString3 = AddTwoStr("ㄴ", m_pNewBuffer);
				pAddTwoString4 = AddTwoStr(pAddTwoString3, m_pOldBuffer);
				MakeNewEndingFeature(pNode);
				AddEndingFeature(pNode, "ㄴ");
				InsertEndingString(pNode, pAddTwoString4);
				if (IsExistEndDic(pAddTwoString4))
				{
					SaveHTokenEnding(pNode);
				}
				else
				{
					SaveHTokenEndingNewFeature(pNode);
					SaveHTokenEndingOldFeature(pNode);
					SaveAmbstrEnding(pNode, m_pNewBuffer);
					SaveAmbstrEnding(pNode, m_pOldBuffer);
				}
			}
		}
		else if (IsExistStemDic(pThisStartCountChar))
		{
			InsertStemString(pNode, pThisStartCountChar);
			SaveHTokenStem(pNode);
			CHAR_DELETE(pAddTwoString3);

			pAddTwoString3 = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
			InsertEndingString(pNode, pAddTwoString3);
			if (IsExistEndDic(pAddTwoString3))
			{
				SaveHTokenEnding(pNode);
			}
			else
			{
				MakeNewEndingFeature(pNode);
				SaveHTokenEndingNewFeature(pNode);
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, m_pNewBuffer);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
			}
		}
		else
		{
			SetOneWordMarsk();
			GoMAR1_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
		}
	}
	else
	{
		nCount = ReturnNextPrevCount(pOriginalString, nFirstOlive, 1, CALCULATE_MINUS);
		pNextOneChar = GetOneChar(pOriginalString, nCount, nCount);
		pNextStartCountChar = GetStartCountChar(pOriginalString, nCount);

		nPos = ReturnNextPrevCount(pOriginalString, nCount, 1, CALCULATE_MINUS);
		pNextOneChar2 = GetOneChar(pOriginalString, nPos, nPos);
		pNext2StartCountChar = GetStartCountChar(pOriginalString, nPos);
		pAddTwoString = AddTwoStr(pNextOneChar, pNextOneChar2);

		nPos2 = ReturnNextPrevCount(pOriginalString, nPos, 1, CALCULATE_MINUS);
		pNext3StartCountChar = GetStartCountChar(pOriginalString, nPos2);

		if (strcmp(pOneChar, "답") == 0 &&
			(IsExistFeature(pNextStartCountChar, STEM_DIC, S_N) ||
				IsExistFeature(pNextStartCountChar, STEM_DIC, S_Amb)))
		{
			CHAR_DELETE(pAddTwoString);
			pAddTwoString = AddTwoStr(pNextStartCountChar, "답");
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStemFeature(pNode, pNextStartCountChar);

			DeleteHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_V);
			AddHTokenStem(pNode, S_Sta);

			CHAR_DELETE(pAddTwoString);
			pAddTwoString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
			InsertEndingString(pNode, pAddTwoString);
			if (IsExistEndDic(pAddTwoString))
			{
				SaveHTokenEnding(pNode);
			}
			else
			{
				MakeNewEndingFeature(pNode);
				SaveHTokenEndingNewFeature(pNode);
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, m_pNewBuffer);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
			}
		}
		else if (strcmp(pOneChar, "럽") == 0 &&
			strcmp(pNextOneChar, "스") == 0 &&
			(IsExistFeature(pNext2StartCountChar, STEM_DIC, S_N) ||
				IsExistFeature(pNext2StartCountChar, STEM_DIC, S_Amb)))
		{
			CHAR_DELETE(pAddTwoString);
			pAddTwoString = AddTwoStr(pNext2StartCountChar, "스럽");
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStemFeature(pNode, pNext2StartCountChar);

			DeleteHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_V);
			AddHTokenStem(pNode, S_Sta);

			CHAR_DELETE(pAddTwoString);
			pAddTwoString = AddTwoStr(m_pNewBuffer, m_pOldBuffer);
			InsertEndingString(pNode, pAddTwoString);
			if (IsExistEndDic(pAddTwoString))
			{
				SaveHTokenEnding(pNode);
			}
			else
			{
				MakeNewEndingFeature(pNode);
				SaveHTokenEndingNewFeature(pNode);
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, m_pNewBuffer);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
			}
		}
		else if (strcmp(pOneChar, "우") == 0 &&
			strcmp(pNextOneChar, "다") == 0 &&
			(IsExistFeature(pNext2StartCountChar, STEM_DIC, S_N) ||
				IsExistFeature(pNext2StartCountChar, STEM_DIC, S_Amb)))
		{
			CHAR_DELETE(pAddTwoString);
			pAddTwoString = AddTwoStr(pNext2StartCountChar, "답");
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStemFeature(pNode, pNext2StartCountChar);

			DeleteHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_V);
			AddHTokenStem(pNode, S_Sta);

			CHAR_DELETE(pAddTwoString);
			CHAR_DELETE(pAddTwoString2);
			pAddTwoString = AddTwoStr("으", m_pNewBuffer);
			pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
			InsertEndingString(pNode, pAddTwoString2);
			if (IsExistEndDic(pAddTwoString2))
			{
				SaveHTokenEnding(pNode);
			}
			else
			{
				MakeNewEndingFeature(pNode);
				SaveHTokenEndingNewFeature(pNode);
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, m_pNewBuffer);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
			}
		}
		else if (strcmp(pOneChar, "우") == 0 &&
			strcmp(pAddTwoString, "스러") == 0 &&
			(IsExistFeature(pNext3StartCountChar, STEM_DIC, S_N) ||
				IsExistFeature(pNext3StartCountChar, STEM_DIC, S_Amb)))
		{
			CHAR_DELETE(pAddTwoString);
			pAddTwoString = AddTwoStr(pNext2StartCountChar, "스럽");
			InsertStemString(pNode, pAddTwoString);
			SaveHTokenStemFeature(pNode, pNext3StartCountChar);

			DeleteHTokenStem(pNode, S_N);
			AddHTokenStem(pNode, S_V);
			AddHTokenStem(pNode, S_Sta);

			CHAR_DELETE(pAddTwoString);
			CHAR_DELETE(pAddTwoString2);
			pAddTwoString = AddTwoStr("으", m_pNewBuffer);
			pAddTwoString2 = AddTwoStr(pAddTwoString, m_pOldBuffer);
			InsertEndingString(pNode, pAddTwoString2);
			if (IsExistEndDic(pAddTwoString2))
			{
				SaveHTokenEnding(pNode);
			}
			else
			{
				MakeNewEndingFeature(pNode);
				SaveHTokenEndingNewFeature(pNode);
				SaveHTokenEndingOldFeature(pNode);
				SaveAmbstrEnding(pNode, m_pNewBuffer);
				SaveAmbstrEnding(pNode, m_pOldBuffer);
			}
		}
		else
		{
			SetOneWordMarsk();
			GoMAR1_2(pNode, nFirstOlive, mSecondOlive, mSecondAlkong);
		}
	}
	/*
	CHAR_DELETE(pThisStartCountChar1);
	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pNextOneChar);
	CHAR_DELETE(pNextJongChar);
	CHAR_DELETE(pThisStartCountChar);
	CHAR_DELETE(pNextStartCountChar);
	CHAR_DELETE(pMinusTwoString);
	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pAddTwoString1);
	CHAR_DELETE(pAddTwoString2);
	CHAR_DELETE(pAddTwoString3);
	CHAR_DELETE(pAddTwoString4);
	*/
	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pNextOneChar);
	CHAR_DELETE(pNextJongChar);
	CHAR_DELETE(pThisStartCountChar);
	CHAR_DELETE(pThisStartCountChar1);
	CHAR_DELETE(pNextStartCountChar);
	CHAR_DELETE(pMinusTwoString);
	CHAR_DELETE(pAddTwoString);
	CHAR_DELETE(pAddTwoString1);
	CHAR_DELETE(pAddTwoString2);
	CHAR_DELETE(pAddTwoString3);
	CHAR_DELETE(pAddTwoString4);
	CHAR_DELETE(pAddTwoString5); // Added by KKH 2004-01-13
	CHAR_DELETE(pNext2StartCountChar);
	CHAR_DELETE(pNext3StartCountChar);
	CHAR_DELETE(pNextOneChar2);

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

	if (nThisPos + 1 < nTotalCount)
	{
		if (pString[nThisPos] & 0x80)
		{
			if (pString[nThisPos + 1] & 0x80)
			{
				pReturnOneChar = new char[3];
				memset(pReturnOneChar, 0x00, 3);
				pReturnOneChar[0] = pString[nThisPos];
				pReturnOneChar[1] = pString[nThisPos + 1];
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
		if (nCount + 1 < nTotalCount)
		{
			if (pString[nCount + 1] & 0x80)
				nCount += 2;
			else
				nCount += 1;
		}
		else if (nCount + 1 == (nTotalCount - 1))
			nCount = nTotalCount;
		else
			nCount += 1;
	}
	else
	{
		if (nCount + 1 < nTotalCount)
			nCount += 1;
		else if (nCount + 1 == (nTotalCount - 1))
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
		if (nCount + 1 < nTotalCount)
		{
			if (pString[nCount + 1] & 0x80)
			{
				nReturnNumber = nCount + 2;
			}
			else
			{
				nReturnNumber = nCount + 1;
			}
		}
	}
	else
	{
		if (nCount + 1 < nTotalCount)
			nReturnNumber = nCount + 1;
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
			if (nStartPos + 1 < nTotalCount)
			{
				if (pString[nStartPos + 1] & 0x80)
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
			if (pString[nStartPos + 1] & 0x80)
			{
				pReturnMiddleString = new char[3];
				memset(pReturnMiddleString, 0x00, 3);
				pReturnMiddleString[0] = pString[nStartPos];
				pReturnMiddleString[1] = pString[nStartPos + 1];
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
		nLen = nEndPos - nStartPos + 1;
		if (nLen > 0)
		{
			nCount = 0;
			pReturnMiddleString = new char[nLen + 1];
			memset(pReturnMiddleString, 0x00, nLen + 1);
			for (nStartPos; nStartPos <= nEndPos; nStartPos++)
			{
				pReturnMiddleString[nCount] = pString[nStartPos];
				nCount += 1;
			}
		}
		else
		{
			if ((unsigned)nEndPos + 1 <= strlen(pString))
			{
				pReturnMiddleString = new char[2];
				memset(pReturnMiddleString, 0x00, 2);
				pReturnMiddleString[0] = pString[nEndPos + 1];
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
	char Feature_End[48];
	char IS_CEHCK_FEATURE = '1';
	memset(Feature_End, 0x00, sizeof(Feature_End));

	if (pString != NULL && strlen(pString) > 0)
	{
		if (IsExistEndDic(pString) == true)
		{
			IsExistEndDic(pString, (char *)Feature_End);
			for (int Feature = 0; Feature < END_FEATURE_COUNT; Feature++)
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

	pEndingNode->pHToken->subEnding->strFeature = new char[48];
	memset(pEndingNode->pHToken->subEnding->strFeature, 0x00, 48);
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
			for (nPos = nLen - 1; nPos >= 0; nPos--)
			{
				if (pTmOri[nPos] >= '0' && pTmOri[nPos] <= '9')
					break;
			}
			if (nPos >= 0)
			{
				for (nPos = nPos + 1; nPos<nLen; nPos++)
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

			m_pOldBuffer = new char[nLen + 1];
			memset(m_pOldBuffer, 0x00, nLen + 1);
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
			m_pOldBuffer = new char[nLen + 1];
			memset(m_pOldBuffer, 0x00, nLen + 1);
			strcpy(m_pOldBuffer, pOneChar);

			pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, nStart, nTotalCount - 1, CALCULATE_PLUS, CALCULATE_STOP);
			if (IsExistEndDic(pMiddleString))
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
				pNode->pHToken->strSource = new char[nLen + 1];
				memset(pNode->pHToken->strSource, 0x00, nLen + 1);
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
		m_pOldBuffer = new char[nLen + 1];
		memset(m_pOldBuffer, 0x00, nLen + 1);
		strcpy(m_pOldBuffer, pOneChar);
		pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, nStart, nTotalCount - 1, CALCULATE_PLUS, CALCULATE_STOP);
		if (IsExistEndDic(pMiddleString))
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
			pNode->pHToken->strSource = new char[nLen + 1];
			memset(pNode->pHToken->strSource, 0x00, nLen + 1);
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
		pNextMiddleString2 = GetUnRegisterMiddleCountChar(pOriginalString, nNextPos1, nNextPos2 + 1, CALCULATE_STOP, CALCULATE_STOP);
	}

	if ((unsigned)nNextPos3 == strlen(pOriginalString))
	{
		nNextPos3 -= 1;
	}
	if (nNextPos3 >= nNextPos1)
	{
		pNextMiddleString3 = GetUnRegisterMiddleCountChar(pOriginalString, nNextPos1, nNextPos3 + 1, CALCULATE_STOP, CALCULATE_STOP);
	}

	if ((unsigned)nNextPos4 == strlen(pOriginalString))
	{
		nNextPos4 -= 1;
	}

	if (nNextPos4 >= nNextPos1)
	{
		pNextMiddleString4 = GetUnRegisterMiddleCountChar(pOriginalString, nNextPos1, nNextPos4 + 1, CALCULATE_STOP, CALCULATE_STOP);
	}

	if ((unsigned)nNextPos5 == strlen(pOriginalString))
	{
		nNextPos5 -= 1;
	}
	if (nNextPos5 >= nNextPos1)
	{
		pNextMiddleString5 = GetUnRegisterMiddleCountChar(pOriginalString, nNextPos1, nNextPos5 + 1, CALCULATE_STOP, CALCULATE_STOP);
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
	pMiddleString12 = GetUnRegisterMiddleCountChar(pOriginalString, nNextPos1, nNextPos2 + 1, CALCULATE_STOP, CALCULATE_STOP);
	//S(n+2~n+3)
	pMiddleString23 = GetUnRegisterMiddleCountChar(pOriginalString, nNextPos2, nNextPos3 + 1, CALCULATE_STOP, CALCULATE_STOP);

	//7.24수정
	//3n
	if (IsExistEndDic(pNextMiddleString1)) // S(n+1~len)이 Ending 사전에 있는가?
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
		strcmp(pNextOneChar, "일") == 0 && IsExistEndDic(pMiddleString))
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

		if (IsExistEndDic(pNextMiddleString1))
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
	else if ((isbFind005 = (strcmp(pNextOneChar, "이") == 0 && IsExistEndDic(pMiddleString))) ||
		(isbFind006 = (strcmp(pChoJungChar, "이") == 0 && IsExistEndDic(pAddTwoString))) ||
		(isbFind007 = (strcmp(pChoJungChar, "이") == 0 && IsExistEndDic(pJongChar) &&
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
		IsExistEndDic(pMiddleString))
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
				pBuffer = new char[nMinusLen + 1];
				memset(pBuffer, 0x00, nMinusLen + 1);
				nPos = 0;
				for (nLen; nLen<nTotalCount; nLen++)
				{
					pBuffer[nPos] = pOriginalString[nLen];
					nPos += 1;
				}

				CHAR_DELETE(pNode->pHToken->strSource);

				nLen = strlen(pBuffer);
				nPos = 0;
				pNode->pHToken->strSource = new char[nLen + 1];
				memset(pNode->pHToken->strSource, 0x00, nLen + 1);
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
			pBuffer = new char[nMinusLen + 1];
			memset(pBuffer, 0x00, nMinusLen + 1);
			nPos = 0;
			for (nLen; nLen<nTotalCount; nLen++)
			{
				pBuffer[nPos] = pOriginalString[nLen];
				nPos += 1;
			}

			CHAR_DELETE(pNode->pHToken->strSource);

			nLen = strlen(pBuffer);
			nPos = 0;
			pNode->pHToken->strSource = new char[nLen + 1];
			memset(pNode->pHToken->strSource, 0x00, nLen + 1);
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
		else if (IsExistTabel009(pNextOneChar) && IsExistEndDic(pMiddleString) &&
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
			if ((IsExistTabel009(pNextOneChar) && strcmp(pNextOneChar, "이") == 0 && IsExistEndDic(pMiddleString1)) ||
				(strcmp(pAddTwoString2, "이") == 0 && IsExistEndDic(pAddTwoString)) ||
				(strcmp(pAddTwoString2, "이") == 0 && IsExistEndDic(pJongChar) && nTotalCount <= nNextPos2) ||
				((strcmp(pNextOneChar2, "였") == 0 || strcmp(pNextOneChar2, "여") == 0) && IsExistEndDic(pMiddleString1))
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
		pBuffer = new char[nMinusLen + 1];
		memset(pBuffer, 0x00, nMinusLen + 1);
		nPos = 0;
		for (nLen; nLen<nTotalCount; nLen++)
		{
			pBuffer[nPos] = pOriginalString[nLen];
			nPos += 1;
		}

		CHAR_DELETE(pNode->pHToken->strSource);

		nLen = strlen(pBuffer);
		nPos = 0;
		pNode->pHToken->strSource = new char[nLen + 1];
		memset(pNode->pHToken->strSource, 0x00, nLen + 1);
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

		if (nTotalCount > nPos1 + 1)
		{
			nPos = nPos1 + 1;
			if (pOriginalString[nPos1] & 0x80)
			{
				nPos += 1;
			}

			pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, nPos, nTotalCount - 1, CALCULATE_STOP, CALCULATE_STOP);
			pThisOneChar = GetUnRegisterOneChar(pOriginalString, nPos);
			pChoJungChar = GetChojungJongsungChar(pThisOneChar, CHAR_CHOJUNGSUNG);
			pJongChar = GetChojungJongsungChar(pThisOneChar, CHAR_JONGSUNG);
			if (nTotalCount > nPos + 1)
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

			if (nTotalCount > nPos1 + 1)
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

		if (IsExistEndDic(pMiddleString) ||
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
			(strcmp(pThisOneChar, "이") == 0 && pMiddleString1 != NULL && IsExistEndDic(pMiddleString1)) ||
			(strcmp(pChoJungChar, "이") == 0 && pAddTwoString != NULL && IsExistEndDic(pAddTwoString)) ||
			(strcmp(pChoJungChar, "이") == 0 && pJongChar != NULL && IsExistEndDic(pJongChar) &&
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
			pMiddleString1 != NULL && IsExistEndDic(pMiddleString1))
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
		else if (strcmp(pThisOneChar, "대") == 0 && IsExistEndDic(pMiddleString1))
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
				(pMiddleString12 == NULL || (pMiddleString12 != NULL && IsExistEndDic(pMiddleString12))))
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

			m_pOldBuffer = new char[strlen(pAddTwoString) + 1];
			memset(m_pOldBuffer, 0x00, strlen(pAddTwoString) + 1);
			strcpy(m_pOldBuffer, pAddTwoString);

			nStart = nNextPos1 + 1;
			if (pOriginalString[nNextPos1 + 1] & 0x80)
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
			pBuffer = new char[nMinusLen + 1];
			memset(pBuffer, 0x00, nMinusLen + 1);
			nPos = 0;
			for (nLen; nLen<nTotalCount; nLen++)
			{
				pBuffer[nPos] = pOriginalString[nLen];
				nPos += 1;
			}

			CHAR_DELETE(pNode->pHToken->strSource);

			nLen = strlen(pBuffer);
			nPos = 0;
			pNode->pHToken->strSource = new char[nLen + 1];
			memset(pNode->pHToken->strSource, 0x00, nLen + 1);
			strcpy(pNode->pHToken->strSource, pBuffer);

			CHAR_DELETE(pBuffer);
			CHAR_DELETE(m_pOldBuffer);

			GoUWR(pNode);
		}
	}
	else if (IsExistTabel009(pNextOneChar))
	{
		if (nTotalCount > nNextPos1 + 1)
		{
			nPos = nNextPos1 + 1;
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
			if (nTotalCount > nPos + 1)
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

			if (nTotalCount > nPos + 1)
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

			if (nTotalCount > nPos + 1)
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
			if (IsExistEndDic(pMiddleString))
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
				(strcmp(pThisOneChar, "이") == 0 && IsExistEndDic(pMiddleString1)) ||
				(strcmp(pChoJungChar, "이") == 0 && IsExistEndDic(pAddTwoString)) ||
				(strcmp(pChoJungChar, "이") == 0 && IsExistEndDic(pJongChar) &&
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
				(IsExistEndDic(pMiddleString1) || pMiddleString1 == NULL))
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
				(pMiddleString12 != NULL && IsExistEndDic(pMiddleString12) || pMiddleString12 == NULL || strlen(pMiddleString12) == 0))
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

				m_pOldBuffer = new char[strlen(pAddTwoString) + 1];
				memset(m_pOldBuffer, 0x00, strlen(pAddTwoString) + 1);
				strcpy(m_pOldBuffer, pAddTwoString);

				nStart = nNextPos1 + 1;
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
				(pMiddleString12 != NULL &&IsExistEndDic(pMiddleString12)) ||
					(pNextOneChar3 != NULL && pNextOneChar4 != NULL && strcmp(pNextOneChar3, "이") == 0 && (strcmp(pNextOneChar4, "었") == 0 || strcmp(pNextOneChar4, "였") == 0)) ||
					(pNextOneChar3 != NULL && pMiddleString23 != NULL && strcmp(pNextOneChar3, "이") == 0 &&
						IsExistStemDic(pMiddleString23) &&
						IsExistFeature(pMiddleString23, STEM_DIC, S_N) == false &&
						IsExistFeature(pMiddleString23, STEM_DIC, S_Aff) == false) ||
						(pChoJungChar3 != NULL && pAddTwoString3 != NULL &&
							strcmp(pChoJungChar3, "이") == 0 && IsExistEndDic(pAddTwoString3)) ||
							(pChoJungChar3 != NULL && pJongChar3 != NULL && pNextOneChar3 != NULL &&
								strcmp(pChoJungChar3, "이") == 0 && IsExistEndDic(pJongChar3) &&
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
				(pMiddleString1 != NULL && IsExistEndDic(pMiddleString1)) ||
					(pNextOneChar2 != NULL && pNextOneChar3 != NULL &&
						strcmp(pNextOneChar2, "이") == 0 && (strcmp(pNextOneChar3, "었") == 0 || strcmp(pNextOneChar3, "였") == 0)) ||
						(pNextOneChar2 != NULL && pMiddleString12 != NULL &&
							strcmp(pNextOneChar2, "이") == 0 &&
							IsExistStemDic(pMiddleString12) &&
							IsExistFeature(pMiddleString12, STEM_DIC, S_N) == false &&
							IsExistFeature(pMiddleString12, STEM_DIC, S_Aff) == false) ||
							(pChoJungChar2 != NULL && pAddTwoString2 != NULL &&
								strcmp(pChoJungChar2, "이") == 0 && IsExistEndDic(pAddTwoString2)) ||
								(pChoJungChar2 != NULL && pJongChar2 != NULL && pNextOneChar2 != NULL &&
									strcmp(pChoJungChar2, "이") == 0 && IsExistEndDic(pJongChar2) &&
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
				&& (pMiddleString12 == NULL || strlen(pMiddleString12) == 0 || pMiddleString12 != NULL && IsExistEndDic(pMiddleString12)))
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
				pBuffer = new char[nMinusLen + 1];
				memset(pBuffer, 0x00, nMinusLen + 1);
				nPos = 0;
				for (nLen; nLen<nTotalCount; nLen++)
				{
					pBuffer[nPos] = pOriginalString[nLen];
					nPos += 1;
				}

				CHAR_DELETE(pNode->pHToken->strSource);

				nLen = strlen(pBuffer);
				nPos = 0;
				pNode->pHToken->strSource = new char[nLen + 1];
				memset(pNode->pHToken->strSource, 0x00, nLen + 1);
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
			IsExistEndDic(pMiddleString12) == true)//S(n)==숫자 && S(n+1)==특수기호 && S(n+2~len)이 엔딩 사전에 있는가?
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
			pBuffer = new char[nMinusLen + 1];
			memset(pBuffer, 0x00, nMinusLen + 1);
			nPos = 0;
			for (nLen; nLen<nTotalCount; nLen++)
			{
				pBuffer[nPos] = pOriginalString[nLen];
				nPos += 1;
			}

			CHAR_DELETE(pNode->pHToken->strSource);

			nLen = strlen(pBuffer);
			nPos = 0;
			pNode->pHToken->strSource = new char[nLen + 1];
			memset(pNode->pHToken->strSource, 0x00, nLen + 1);
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
	int mAlkong = 0;
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
			GoPFM(pNode, nStart, mAlkong);
		}
		else
		{
			CHAR_DELETE(pThisOneChar);

			if (m_nUWRFlag == 2)
			{
				GoUWR1_1(pNode, nStart, mAlkong);
				//개체명인식NER 02
				GoNER02(pNode);
			}
			else
				m_nUWRFlag = 1;
		}
	}

	CHAR_DELETE(pThisOneChar);
}


void CNAHangulAnaly::GoUWR1_1(H_NODE *pNode, int &nStart, int &mAlkong)
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
			pNode->pHToken->strSource = new char[nLen + 1];
			memset(pNode->pHToken->strSource, 0x00, nLen + 1);
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
			nCount = nStart + 2;
			if (pOriginalString[nCount] & 0x80)
			{
				nCount = nCount + 1;
			}
		}
		else
		{
			nCount = nStart + 1;
			if (pOriginalString[nCount] & 0x80)
			{
				nCount = nCount + 2;
			}
		}
		pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, mAlkong, nStart + 1, CALCULATE_STOP, CALCULATE_STOP);
		if (IsExistStemDic(pMiddleString))
		{
			CHAR_DELETE(m_pOldBuffer);

			nLen = strlen(pMiddleString);
			m_pOldBuffer = new char[nLen + 1];
			memset(m_pOldBuffer, 0x00, nLen + 1);
			strcpy(m_pOldBuffer, pMiddleString);
			if (nStart + 1 == nTotalLen - 1 || nStart == nTotalLen - 1)
			{
				if (nStart == mAlkong &&
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
						memset(pNode->pHToken->subEnding->strFeature, '0', END_FEATURE_COUNT - 1);
					}
					SaveHTokenEnding(pNode);
				}
				else if (nStart == mAlkong &&
					(strcmp(m_pOldBuffer, "할") == 0 || strcmp(m_pOldBuffer, "한") == 0 ||
						strcmp(m_pOldBuffer, "함") == 0))
				{
					pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);

					CHAR_DELETE(pNode->pHToken->subStem->strResult);

					if (pNode->pHToken->subStem->strFeature != NULL)
					{
						memset(pNode->pHToken->subStem->strFeature, '0', STEM_FEATURE_COUNT - 1);
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
						memset(pNode->pHToken->subEnding->strFeature, '0', END_FEATURE_COUNT - 1);
					}
					SaveHTokenEnding(pNode);
				}
				else if (nStart == mAlkong &&
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

				GoUWR1_1(pNode, nStart, mAlkong);
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

				GoUWR1_1(pNode, nStart, mAlkong);
			}
			else
			{
				CHAR_DELETE(pOneChar);
				CHAR_DELETE(pJongChar);
				CHAR_DELETE(pMiddleString);
				CHAR_DELETE(pAddTwoString);
				CHAR_DELETE(pAddTwoString2);

				GoUWR1_2(pNode, nStart, mAlkong);
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

void CNAHangulAnaly::GoUWR1_2(H_NODE *pNode, int &nStart, int &mAlkong)
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
		nCount = nStart + 2;
	}
	else
	{
		nCount = nStart + 1;
	}
	//S(m~n+1)
	pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, mAlkong, nCount - 1, CALCULATE_STOP, CALCULATE_STOP);
	if ((unsigned)nCount + 1 < strlen(pOriginalString))
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
	pMiddleString2 = GetUnRegisterMiddleCountChar(pOriginalString, mAlkong, nCount - 1, CALCULATE_STOP, CALCULATE_STOP);

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

		GoUWR1_1(pNode, nStart, mAlkong);
	}
	else if (IsExistStemDic(pMiddleString2, ptmpFeature2))
	{
		CHAR_DELETE(pMiddleString);

		if (pOriginalString[nTotalLen - 1] & 0x80)
		{
			nThisPos = nTotalLen - 2;
		}
		else
		{
			nThisPos = nTotalLen - 1;
		}
		//S(len)
		pLastOneChar = GetUnRegisterOneChar(pOriginalString, nThisPos);
		//S(n+2)
		pOneChar = GetUnRegisterOneChar(pOriginalString, nCount);
		if ((unsigned)nCount + 1 < strlen(pOriginalString))
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
			pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, mAlkong, nCount - 1, CALCULATE_STOP, CALCULATE_STOP);
		}
		if (pMiddleString != NULL)
		{
			if (strcmp(pLastOneChar, pOneChar) != 0 && IsExistEndDic(pMiddleString))
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
					memset(pNode->pHToken->subEnding->strFeature, '0', END_FEATURE_COUNT - 1);
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
				GoUWR1_1(pNode, nStart, mAlkong);
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
			GoUWR1_1(pNode, nStart, mAlkong);
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

		GoUWR1_3(pNode, nStart, mAlkong);
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

void CNAHangulAnaly::GoUWR1_3(H_NODE *pNode, int &nStart, int &mAlkong)
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
				nCount = nStart + 2;
			}
			else
			{
				nCount = nStart + 1;
			}

			//S(n+1~len)
			pMiddleString2 = GetUnRegisterMiddleCountChar(pOriginalString, nCount, nTotalLen - 1, CALCULATE_STOP, CALCULATE_STOP);
			if ((unsigned)nCount + 1 < strlen(pOriginalString))
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
			pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, nStart, nTotalLen - 1, CALCULATE_STOP, CALCULATE_STOP);
			if (IsExistEndDic(pMiddleString))
			{
				CHAR_DELETE(pNode->pHToken->subEnding->strResult);

				nLen = strlen(pMiddleString);
				nLen += 1;
				pNode->pHToken->subEnding->strResult = new char[nLen];
				memset(pNode->pHToken->subEnding->strResult, 0x00, nLen);
				strcpy(pNode->pHToken->subEnding->strResult, pMiddleString);
				if (pNode->pHToken->subEnding->strFeature != NULL)
				{
					memset(pNode->pHToken->subEnding->strFeature, '0', END_FEATURE_COUNT - 1);
				}
				SaveHTokenEnding(pNode);
			}
			else if ((strcmp(pOneChar, "이") == 0 && IsExistEndDic(pMiddleString2)) ||
				(strcmp(pChoJungChar, "이") == 0 && IsExistEndDic(pAddTwoString)))
			{
				CHAR_DELETE(pNode->pHToken->subEnding->strResult);

				nLen = strlen(pMiddleString);
				nLen += 1;
				pNode->pHToken->subEnding->strResult = new char[nLen];
				memset(pNode->pHToken->subEnding->strResult, 0x00, nLen);
				strcpy(pNode->pHToken->subEnding->strResult, pMiddleString);

				if (pNode->pHToken->subEnding->strFeature != NULL)
				{
					memset(pNode->pHToken->subEnding->strFeature, '0', END_FEATURE_COUNT - 1);
				}

				if (IsExistEndDic(pAddTwoString))
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
				(strlen(pMiddleString2) == 0 || IsExistEndDic(pMiddleString2) == true))))
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
				(strlen(pMiddleString2) == 0 || IsExistEndDic(pMiddleString2) == true))))
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
					memset(pNode->pHToken->subStem->strFeature, '0', STEM_FEATURE_COUNT - 1);
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
				pBuffer = new char[nMinusLen + 1];
				memset(pBuffer, 0x00, nMinusLen + 1);
				nCount = 0;
				for (nLen; nLen < nTotalLen; nLen++)
				{
					pBuffer[nCount] = pOriginalString[nLen];
					nCount += 1;
				}

				CHAR_DELETE(pNode->pHToken->strSource);

				nLen = strlen(pBuffer);
				nCount = 0;
				pNode->pHToken->strSource = new char[nLen + 1];
				memset(pNode->pHToken->strSource, 0x00, nLen + 1);
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
				mAlkong = 0;
				GoUWR1_1(pNode, nStart, mAlkong);
			}

			CHAR_DELETE(m_pOldBuffer);
		}
		else if (strcmp(m_pOldBuffer, "한") == 0)
		{
			//S(n~len)
			pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, nStart, nTotalLen - 1, CALCULATE_STOP, CALCULATE_STOP);

			nCount = nStart - 1;
			//S(n-1)
			pOneChar = GetOneChar(pOriginalString, nCount, nCount);
			pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);
			pAddTwoString2 = AddTwoStr(pJongChar, pMiddleString);

			if (IsExistEndDic(pAddTwoString2))
			{

				CHAR_DELETE(pNode->pHToken->subStem->strResult);

				if (pNode->pHToken->subStem->strFeature != NULL)
				{
					memset(pNode->pHToken->subStem->strFeature, '0', STEM_FEATURE_COUNT - 1);
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
					memset(pNode->pHToken->subEnding->strFeature, '0', END_FEATURE_COUNT - 1);
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

				if (IsExistEndDic(pMiddleString))
				{
					CHAR_DELETE(pNode->pHToken->subEnding->strResult);

					nLen = strlen(pMiddleString);
					nLen += 1;
					pNode->pHToken->subEnding->strResult = new char[nLen];
					memset(pNode->pHToken->subEnding->strResult, 0x00, nLen);
					strcpy(pNode->pHToken->subEnding->strResult, pMiddleString);
					if (pNode->pHToken->subEnding->strFeature != NULL)
					{
						memset(pNode->pHToken->subEnding->strFeature, '0', END_FEATURE_COUNT - 1);
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
						memset(pNode->pHToken->subStem->strFeature, '0', STEM_FEATURE_COUNT - 1);
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
					pBuffer = new char[nMinusLen + 1];
					memset(pBuffer, 0x00, nMinusLen + 1);
					nCount = 0;
					for (nLen; nLen < nTotalLen; nLen++)
					{
						pBuffer[nCount] = pOriginalString[nLen];
						nCount += 1;
					}

					CHAR_DELETE(pNode->pHToken->strSource);

					nLen = strlen(pBuffer);
					nCount = 0;
					pNode->pHToken->strSource = new char[nLen + 1];
					memset(pNode->pHToken->strSource, 0x00, nLen + 1);
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
					mAlkong = 0;
					GoUWR1_1(pNode, nStart, mAlkong);
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
				nNextPos = nStart + 2;
			}
			else
			{
				nNextPos = nStart + 1;
			}

			//S(n+1~len)
			pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, nNextPos, nTotalLen - 1, CALCULATE_STOP, CALCULATE_STOP);
			pAddTwoString2 = AddTwoStr(pJongChar, pMiddleString);

			//7n
			if ((strcmp(pChoJungChar, "하") == 0 || strcmp(pChoJungChar, "되") == 0) &&
				strcmp(pJongChar, "ㅁ") == 0 &&
				(pMiddleString == NULL || (pMiddleString != NULL &&
				(strlen(pMiddleString) == 0 || IsExistEndDic(pMiddleString) == true))))
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
				(strlen(pAddTwoString2) == 0 || IsExistEndDic(pAddTwoString2) == true))))
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

				GoUWR1_4(pNode, nStart, mAlkong);
			}
		}
	}
	else
	{
		GoUWR1_4(pNode, nStart, mAlkong);
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

void CNAHangulAnaly::GoUWR1_4(H_NODE *pNode, int &nStart, int &mAlkong)
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
		nCount = nStart + 2;
	}
	else
	{
		nCount = nStart + 1;
	}

	//S(n+1~len)
	pMiddleString2 = GetUnRegisterMiddleCountChar(pOriginalString, nCount, nTotalLen - 1, CALCULATE_STOP, CALCULATE_STOP);
	//S(n)
	pOneChar = GetUnRegisterOneChar(pOriginalString, nStart);
	//S(n+1)
	pNextOneChar = GetUnRegisterOneChar(pOriginalString, nCount);

	if (pOriginalString[nCount] & 0x80)
	{
		nNextPos = nCount + 2;
	}
	else
	{
		nNextPos = nCount + 1;
	}

	//S(n+1~n+2)
	if (nNextPos < nTotalLen)
	{
		pMiddleString3 = GetUnRegisterMiddleCountChar(pOriginalString, nCount, nNextPos + 1, CALCULATE_STOP, CALCULATE_STOP);
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
	pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, nStart, nTotalLen - 1, CALCULATE_STOP, CALCULATE_STOP);

	CHAR_DELETE(pAddTwoString2);

	pAddTwoString2 = AddTwoStr(pJongChar, pMiddleString2);

	if (IsExistEndDic(pMiddleString))
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
			memset(pNode->pHToken->subEnding->strFeature, '0', END_FEATURE_COUNT - 1);
		}
		SaveHTokenEnding(pNode);
	}
	//8n
	else if ((strcmp(pOneChar, "이") == 0 && IsExistEndDic(pMiddleString2) == true &&
		strcmp(pMiddleString2, "상") != 0) ||
		(strcmp(pChoJungChar, "이") == 0 && IsExistEndDic(pAddTwoString2) == true &&
			strcmp(pMiddleString2, "상") != 0) ||
			(strcmp(pOneChar, "이") == 0 && strcmp(pNextOneChar, "었") == 0) ||
		strcmp(pOneChar, "였") == 0 && IsExistEndDic(pMiddleString2) == true)
	{
		//복합명사 추가 
		CHAR_DELETE(pNode->pHToken->subEnding->strResult);

		nLen = strlen(pMiddleString);
		nLen += 1;
		pNode->pHToken->subEnding->strResult = new char[nLen];
		memset(pNode->pHToken->subEnding->strResult, 0x00, nLen);
		strcpy(pNode->pHToken->subEnding->strResult, pMiddleString);
		if (pNode->pHToken->subEnding->strFeature != NULL)
			memset(pNode->pHToken->subEnding->strFeature, '0', END_FEATURE_COUNT - 1);

		if (IsExistEndDic(pAddTwoString2))
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
		(pMiddleString2 == NULL || IsExistEndDic(pMiddleString2) == true))
	{
		//10y
		CHAR_DELETE(pAddTwoString);

		if (pNode->pHToken->subEnding->strFeature != NULL)
		{
			memset(pNode->pHToken->subEnding->strFeature, '0', END_FEATURE_COUNT - 1);
		}
		else
		{
			pNode->pHToken->subEnding->strFeature = new char[END_FEATURE_COUNT];
			memset(pNode->pHToken->subEnding->strFeature, '0', END_FEATURE_COUNT - 1);
			pNode->pHToken->subEnding->strFeature[END_FEATURE_COUNT - 1] = 0;
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
			memset(pNode->pHToken->subEnding->strFeature, '0', END_FEATURE_COUNT - 1);
		}
		else
		{
			pNode->pHToken->subEnding->strFeature = new char[END_FEATURE_COUNT];
			memset(pNode->pHToken->subEnding->strFeature, '0', END_FEATURE_COUNT - 1);
			pNode->pHToken->subEnding->strFeature[END_FEATURE_COUNT - 1] = 0;
		}
		InsertEndingString(pNode, pMiddleString2);
		AddEndingFeature(pNode, pMiddleString2);
		SaveHTokenEnding(pNode);
	}
	else if (((strcmp(pOneChar, "들") == 0 || strcmp(pOneChar, "씨") == 0 || strcmp(pOneChar, "대") == 0) && IsExistEndDic(pMiddleString2) == true) ||
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
		IsExistEndDic(pMiddleString2) == true)
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
				memset(pNode->pHToken->subStem->strFeature, '0', STEM_FEATURE_COUNT - 1);
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
		pBuffer = new char[nMinusLen + 1];
		memset(pBuffer, 0x00, nMinusLen + 1);
		nCount = 0;
		for (nLen; nLen < nTotalLen; nLen++)
		{
			pBuffer[nCount] = pOriginalString[nLen];
			nCount += 1;
		}

		CHAR_DELETE(pNode->pHToken->strSource);

		nLen = strlen(pBuffer);
		nCount = 0;
		pNode->pHToken->strSource = new char[nLen + 1];
		memset(pNode->pHToken->strSource, 0x00, nLen + 1);
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
		mAlkong = 0;
		GoUWR1_1(pNode, nStart, mAlkong);
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
		pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, nStart, nTotalLen - 1, CALCULATE_STOP, CALCULATE_STOP);

		H_NODE* m_pThisNode;
		m_pThisNode = pNode;

		Insert_Str_LinkedList(pOneChar, m_pThisNode, m_pThisNode->pHToken->nOffset);
		Insert_Str_LinkedList(pMiddleString, m_pThisNode->pNext, m_pThisNode->pHToken->nOffset + strlen(pOneChar) + 1);

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
		pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, nStart, nTotalLen - 1, CALCULATE_STOP, CALCULATE_STOP);

		H_NODE* m_pThisNode;
		m_pThisNode = pNode;

		Insert_Str_LinkedList(pAddTwoString, m_pThisNode, m_pThisNode->pHToken->nOffset);
		Insert_Str_LinkedList(pMiddleString, m_pThisNode->pNext, m_pThisNode->pHToken->nOffset + strlen(pAddTwoString) + 1);

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
		pMiddleString2 = GetUnRegisterMiddleCountChar(pOriginalString, nStart, nTotalLen - 1, CALCULATE_STOP, CALCULATE_STOP);

		H_NODE* m_pThisNode;
		m_pThisNode = pNode;

		Insert_Str_LinkedList(pMiddleString, m_pThisNode, m_pThisNode->pHToken->nOffset);
		Insert_Str_LinkedList(pMiddleString2, m_pThisNode->pNext, m_pThisNode->pHToken->nOffset + strlen(pMiddleString) + 1);

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
		if ((unsigned)nLen + 1 <= strlen(pOriginalString - 1))
		{
			nLen = nStart;
			nLen -= 1;
			if (pOriginalString[nStart] & 0x80)
			{
				nLen -= 1;
			}
			//S(1~n-1)
			pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, 0, nLen + 1, CALCULATE_STOP, CALCULATE_STOP);
			//S(1~n)
			pMiddleString3 = GetUnRegisterMiddleCountChar(pOriginalString, 0, nLen + 3, CALCULATE_STOP, CALCULATE_STOP);

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
				pMiddleLastString = GetUnRegisterMiddleCountChar(pOriginalString, nPos, nTotalLen - 1, CALCULATE_STOP, CALCULATE_STOP);

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
				pMiddleString2 = GetUnRegisterMiddleCountChar(pOriginalString, nPos, nTotalLen - 1, CALCULATE_STOP, CALCULATE_STOP);

				H_NODE* m_pThisNode;
				m_pThisNode = pNode;

				Insert_Str_LinkedList(pMiddleString3, m_pThisNode, m_pThisNode->pHToken->nOffset);
				Insert_Str_LinkedList(pMiddleString2, m_pThisNode->pNext, m_pThisNode->pHToken->nOffset + strlen(pMiddleString) + 1);

				m_nInsertNodeCount = 2;
				m_nListPosition += 2;
				m_pThisNode->nSkip = -1;
				m_LinkedList->m_nTotalCount += 2;
			}
			else if (pOneChar != NULL && strcmp(pJongChar, "ㅆ") == 0 &&
				IsExistStemDic(pMiddleString) && IsExistFeature(pMiddleString, STEM_DIC, 4) == false)
			{
				pMiddleString2 = GetUnRegisterMiddleCountChar(pOriginalString, nPos, nTotalLen - 1, CALCULATE_STOP, CALCULATE_STOP);

				H_NODE* m_pThisNode;
				m_pThisNode = pNode;

				Insert_Str_LinkedList(pMiddleString3, m_pThisNode, m_pThisNode->pHToken->nOffset);
				Insert_Str_LinkedList(pMiddleString2, m_pThisNode->pNext, m_pThisNode->pHToken->nOffset + strlen(pMiddleString) + 1);

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
			if ((unsigned)nLen + 1 <= strlen(pOriginalString - 1))
			{
				nLen = nStart;
				nLen -= 1;
				if (pOriginalString[nStart] & 0x80)
				{
					nLen -= 1;
				}

				CHAR_DELETE(pMiddleString);

				//S(1~n-1)
				pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, 0, nLen + 1, CALCULATE_STOP, CALCULATE_STOP);

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
					pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, 0, nStart + 1, CALCULATE_STOP, CALCULATE_STOP);

					nStart += 1;
					if (pOriginalString[nStart] & 0x80)
					{
						nStart += 1;
					}

					pMiddleString2 = GetUnRegisterMiddleCountChar(pOriginalString, nStart, nTotalLen, CALCULATE_STOP, CALCULATE_STOP);

					H_NODE* m_pThisNode;
					m_pThisNode = pNode;

					Insert_Str_LinkedList(pMiddleString, m_pThisNode, m_pThisNode->pHToken->nOffset);
					Insert_Str_LinkedList(pMiddleString2, m_pThisNode->pNext, m_pThisNode->pHToken->nOffset + strlen(pMiddleString) + 1);

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

		if ((unsigned)nLen + 1 <= strlen(pOriginalString - 1))
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
				Insert_Str_LinkedList(pMiddleString, m_pThisNode->pNext, m_pThisNode->pHToken->nOffset + strlen(pMiddleString) + 1);

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
	int mAlkong = 0;
	int nTotalLen = 0;

	pOriginalString = pNode->pHToken->strSource;
	nTotalLen = strlen(pOriginalString);
	nStart = nTotalLen;
	mAlkong = nTotalLen;

	CHAR_DELETE(m_pOldBuffer);

	m_bIsFirstWord = true;
	GoEND1_1(pNode, nStart, mAlkong);

	//개체명인식NER 03 
	GoNER03(pNode);
}

void CNAHangulAnaly::GoEND1_1(H_NODE *pNode, int &nStart, int &mAlkong)
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
		MoveLeftWordCount(pOriginalString, nStart);
		InitOneWordMarsk();
	}
	m_bIsFirstWord = false;

	if (nStart > 0)
	{
		pMiddleString = GetMiddleCountChar(pOriginalString, nStart, mAlkong, CALCULATE_STOP, CALCULATE_STOP);
		if (IsExistEndDic(pMiddleString))
		{
			CHAR_DELETE(m_pOldBuffer);

			if (pMiddleString != NULL)
			{
				nLen = strlen(pMiddleString);
				m_pOldBuffer = new char[nLen + 1];
				memset(m_pOldBuffer, 0x00, nLen + 1);
				strcpy(m_pOldBuffer, pMiddleString);
			}

			CHAR_DELETE(pMiddleString);
			CHAR_DELETE(pThisStartCountChar);
			CHAR_DELETE(pOneChar);
			CHAR_DELETE(pJongChar);
			CHAR_DELETE(pThisEndCountChar);
			CHAR_DELETE(pAddTwoString);

			GoEND1_1(pNode, nStart, mAlkong);

		}
		else if (m_pOldBuffer == NULL)
		{
			nLen = strlen(pOriginalString);
			nLen -= 1;
			pOneChar = GetOneChar(pOriginalString, nLen, nLen);
			pJongChar = GetChojungJongsungChar(pOneChar, CHAR_JONGSUNG);
			if (strcmp(pOneChar, "된") == 0 || strcmp(pOneChar, "될") == 0 ||
				strcmp(pOneChar, "됨") == 0)
			{
				nLen -= 1;
				if (pOriginalString[nLen] & 0x80)
				{
					nLen -= 1;
				}
				pThisEndCountChar = GetEndCountChar(pOriginalString, nLen);//S(len~2)
				pAddTwoString = AddTwoStr(pThisEndCountChar, pJongChar);

				CHAR_DELETE(pNode->pHToken->subStem->strResult);

				nLen = strlen(pAddTwoString);
				nLen += 1;
				pNode->pHToken->subStem->strResult = new char[nLen];
				memset(pNode->pHToken->subStem->strResult, 0x00, nLen);
				strcpy(pNode->pHToken->subStem->strResult, pAddTwoString);
				SaveHTokenStem(pNode);
				// 자질 초기화
				for (i = 0; i<STEM_FEATURE_COUNT; i++)
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

				GoEND1_1(pNode, nStart, mAlkong);
			}
		}
		else
		{
			InsertEndingString(pNode, m_pOldBuffer);
			MakeNewEndingFeature(pNode);
			AddEndingFeature(pNode, m_pOldBuffer);

			pThisStartCountChar = GetStartCountChar(pOriginalString, nStart);
			InsertStemString(pNode, pThisStartCountChar);
			SaveHTokenStem(pNode);
			// 자질 초기화
			for (i = 0; i<STEM_FEATURE_COUNT; i++)
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
			for (i = 0; i<STEM_FEATURE_COUNT; i++)
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
			for (i = 0; i<STEM_FEATURE_COUNT; i++)
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

	pFirstOneChar = GetOneChar(pOriginalString, 0, 0);//S(1)
	pLastOneChar = GetOneChar(pOriginalString, nLen, nLen);//S(len)

	pFirstTwoChar = GetStartCountChar(pOriginalString, 4);//S(1~2)

	nCount = ReturnNextPrevCount(pOriginalString, nLen, 1, CALCULATE_MINUS);

	pFirst = GetStartCountChar(pOriginalString, nCount);

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

		pLastOneChar = GetOneChar(pStemString, nLen, nLen);//S(len)

		if (pStemFeature[S_N] == '1' && pStemFeature[S_Lex] == '1' &&
			pStemFeature[S_Scmp] == '1')
		{
			//1y
			if (strcmp(pEndString, "에게") == 0 || strcmp(pEndString, "에게서") == 0 ||
				strcmp(pEndString, "에게로") == 0 || strcmp(pEndString, "께서") == 0 ||
				strcmp(pEndString, "님") == 0)
			{
				//2y
				for (i = 0; i<STEM_FEATURE_COUNT; i++) { pStemFeature[i] = '0'; } // 자질 초기화
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
				for (i = 0; i<STEM_FEATURE_COUNT; i++) { pStemFeature[i] = '0'; } // 자질 초기화
				AddHTokenStem(pNode, S_N);
				AddHTokenStem(pNode, S_Lex);
				AddHTokenStem(pNode, S_Plc);
				AddHTokenStem(pNode, S_Prp);
			}
			//3n
			else if (strcmp(pLastOneChar, "씨") == 0 || strcmp(pLastOneChar, "님") == 0)
			{
				//4y
				for (i = 0; i<STEM_FEATURE_COUNT; i++) { pStemFeature[i] = '0'; } // 자질 초기화
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
				for (i = 0; i<STEM_FEATURE_COUNT; i++) { pStemFeature[i] = '0'; } // 자질 초기화
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
			if (m_pNode->pHToken != NULL && m_pNode->pHToken->subStem != NULL &&
				m_pNode->pHToken->subStem->strFeature != NULL)
			{
				pFeature = m_pNode->pHToken->subStem->strFeature;
				if (pFeature[211] == '1')
				{
					nTS = 1;
				}
				if (pFeature[212] == '1')
				{
					nTE = 1;
				}
				if (pFeature[213] == '1')
				{
					nNS = 1;
				}
				if (pFeature[214] == '1')
				{
					nNE = 1;
				}
				if (pFeature[215] == '1')
				{
					nComma = 1;
				}
			}
			GoMAR1(m_pNode);//Go MAR1 Function......

			GoUnitNode(m_pNode);//복합명사 미처리

			pFeature = NULL;
			if (m_pNode->pHToken != NULL && m_pNode->pHToken->subStem != NULL &&
				m_pNode->pHToken->subStem->strFeature != NULL)
			{
				if (m_pNode->pHToken->bSubCompCreate == true)
				{
					if (m_pNode->pHToken->pSubHeadNode != NULL &&
						m_pNode->pHToken->pSubHeadNode->pNext != NULL &&
						m_pNode->pHToken->pSubHeadNode->pNext != m_pNode->pHToken->pSubTailNode
						&& m_pNode->pHToken->pSubHeadNode->pNext->pCSubToken->strFeature != NULL)
					{
						pFeature = m_pNode->pHToken->pSubHeadNode->pNext->pCSubToken->strFeature;
					}
				}
				else
				{
					pFeature = m_pNode->pHToken->subStem->strFeature;
				}

				if (pFeature != NULL)
				{
					if (nTS == 1)
					{
						pFeature[211] = '1';
					}
					if (nNS == 1)
					{
						pFeature[213] = '1';
					}
				}
			}
			if (m_nInsertNodeCount == 2)
			{
				if (m_pNode->pPrev != NULL)
				{
					m_pNode = m_pNode->pPrev->pPrev;
					m_nInsertNodeCount = 0;
				}
			}
			else if (m_nInsertNodeCount == 1)
			{
				m_pNode = m_pNode->pPrev;
				m_nInsertNodeCount = 0;
			}
			else
			{
				m_pNode = m_pNode->pNext;
				m_nListPosition -= 1;
				m_nListCount += 1;
			}

			pFeature = NULL;

			if (m_pNode->pPrev->pHToken != NULL && m_pNode->pPrev->pHToken->subStem != NULL &&
				m_pNode->pPrev->pHToken->subStem->strFeature != NULL)
			{
				if (m_pNode->pPrev->pHToken->bSubCompCreate == true)
				{
					if (m_pNode->pPrev->pHToken->pSubTailNode != NULL &&
						m_pNode->pPrev->pHToken->pSubTailNode->pPrev != NULL &&
						m_pNode->pPrev->pHToken->pSubTailNode->pPrev != m_pNode->pPrev->pHToken->pSubHeadNode
						&& m_pNode->pPrev->pHToken->pSubTailNode->pPrev->pCSubToken->strFeature != NULL)
					{
						pFeature = m_pNode->pPrev->pHToken->pSubTailNode->pPrev->pCSubToken->strFeature;
					}
				}
				else
				{
					pFeature = m_pNode->pPrev->pHToken->subStem->strFeature;
				}

				if (pFeature != NULL)
				{
					if (nTE == 1)
					{
						pFeature[212] = '1';
					}
					if (nNE == 1)
					{
						pFeature[214] = '1';
					}
					if (nComma == 1)
					{
						pFeature[215] = '1';
					}
				}
			}
		}
		else//Skip인 경우
		{
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

void CNAHangulAnaly::SaveCompEndDic(H_NODE *pNode, char *pString)
{

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
	syl |= *(p + 1) & 0x000000FF;
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
			pOneChar = GetOneChar(pString, nLen, nLen);
			if (strcmp(pOneChar, "우") == 0 || strcmp(pOneChar, "오") == 0)
			{
				//n-1
				nCount = ReturnNextPrevCount(pString, nLen, 1, CALCULATE_MINUS);
				if (nCount > 0)
				{
					pStartCountChar = GetStartCountChar(pString, nCount);
					nLen = strlen(pStartCountChar);
					nPos = nLen;

					CHAR_DELETE(pOneChar);

					pOneChar = GetOneChar(pStartCountChar, nLen, nLen);
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
							pSaveString = new char[nLen + 1];
							memset(pSaveString, 0x00, nLen + 1);
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
							pNode->pHToken->subStem->strResult = new char[nLen + 1];
							memset(pNode->pHToken->subStem->strResult, 0x00, nLen + 1);
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

void CNAHangulAnaly::GoPFM(H_NODE *pNode, int &nStart, int &mAlkong)
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
	if (nNextPos1 + 1 < nTotalLen)
	{
		pNextMiddleString1 = GetUnRegisterMiddleCountChar(pOriginalString, nStart1, nNextPos1 + 1, CALCULATE_STOP, CALCULATE_STOP);
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
	if (nNextPos1 + 2 < nTotalLen)
	{
		//S(1~3)
		pNextMiddleString2 = GetUnRegisterMiddleCountChar(pOriginalString, nStart1, nNextPos2 + 1, CALCULATE_STOP, CALCULATE_STOP);
		//S(2~3)
		pNextMiddleString3 = GetUnRegisterMiddleCountChar(pOriginalString, nNextPos1, nNextPos2 + 1, CALCULATE_STOP, CALCULATE_STOP);
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
			mAlkong = 0;
			GoUWR1_1(pNode, nStart, mAlkong);
		}
		else if (IsExistStemDic(pNextMiddleString3))
		{
			nNextPos3 = GetUnRegisterMoveRightCount(pOriginalString, nNextPos2);
			if (nNextPos3 == nTotalLen)
				nNextPos3 -= 1;
			//S(3~4)
			if (nNextPos3 + 1 < nTotalLen)
				pNextMiddleString4 = GetUnRegisterMiddleCountChar(pOriginalString, nNextPos2, nNextPos3 + 1, CALCULATE_STOP, CALCULATE_STOP);
			else
			{
				pNextMiddleString4 = new char[2];
				memset(pNextMiddleString4, 0x00, 2);
				strcpy(pNextMiddleString4, "");
			}
			if (IsExistStemDic(pNextMiddleString4))
			{
				nStart = -1;
				mAlkong = 0;
				GoUWR1_1(pNode, nStart, mAlkong);
			}
			else
			{
				pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, nNextPos1, nTotalLen - 1, CALCULATE_STOP, CALCULATE_STOP);
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

				pNode->pHToken->strSource = new char[nLen + 1];
				memset(pNode->pHToken->strSource, 0x00, nLen + 1);
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
				mAlkong = 0;
				GoUWR1_1(pNode, nStart, mAlkong);
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
			mAlkong = 0;
			GoUWR1_1(pNode, nStart, mAlkong);
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
		mAlkong = 0;
		GoUWR1_1(pNode, nStart, mAlkong);
	}
	else
	{
		pMiddleString = GetUnRegisterMiddleCountChar(pOriginalString, nNextPos1, nTotalLen - 1, CALCULATE_STOP, CALCULATE_STOP);
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

		pNode->pHToken->strSource = new char[nLen + 1];
		memset(pNode->pHToken->strSource, 0x00, nLen + 1);
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
		mAlkong = 0;
		GoUWR1_1(pNode, nStart, mAlkong);
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
				pSubNode->pCSubToken->strResult = new char[nLen + 1];
				memset(pSubNode->pCSubToken->strResult, 0x00, nLen + 1);

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
					pFeature = new char[236];
					memset(pFeature, 0x00, 236);
					for (int Feature = 0; Feature < STEM_FEATURE_COUNT; Feature++)
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
				pFeature = new char[236];
				memset(pFeature, 0x00, 236);
				for (int Feature = 0; Feature < STEM_FEATURE_COUNT; Feature++)
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