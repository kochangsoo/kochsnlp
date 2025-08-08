#include "MorphoLogicalAnalysis.h"


CMorphoLogicalAnalysis::CMorphoLogicalAnalysis()
{
	m_AnalyzeAgainFromTheBeginning = false;
	m_AnalyzeWhatCallFunction = "";

	m_LinkedList = NULL;
	m_pConvert = new CConvert;
	m_pAmbiguity = new CNAAmbiguity;
	m_pCNAHangulUtil = new CNAHangulUtil();
}


CMorphoLogicalAnalysis::~CMorphoLogicalAnalysis()
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
	if (m_pCNAHangulUtil != NULL)
	{
		delete m_pCNAHangulUtil;
		m_pCNAHangulUtil = NULL;
	}
}

bool CMorphoLogicalAnalysis::IsExistStemDic(H_NODE* pNode, char *pStemWord)
{
	bool isExist = false;
	char Feature_Stem[STEM_DIC_ALL_FEATURE_NUMBER + 1];
	char IS_CEHCK_FEATURE = '1';
	memset(Feature_Stem, 0x00, sizeof(Feature_Stem));
	isExist = m_pStemEndDic->StemGetRecord(pStemWord, (char *)Feature_Stem);
	if (isExist == true)
	{
		CHAR_DELETE(pNode->pHToken->subStem->strResult);
		CHAR_DELETE(pNode->pHToken->subStem->strFeature);
		int nLen = strlen(pStemWord);
		pNode->pHToken->subStem->strResult = new char[nLen + 1];
		memset(pNode->pHToken->subStem->strResult, 0x00, nLen + 1);
		strcpy(pNode->pHToken->subStem->strResult, pStemWord);
		pNode->pHToken->subStem->strFeature = new char[STEM_DIC_ALL_FEATURE_NUMBER + 1];
		memset(pNode->pHToken->subStem->strFeature, 0x00, sizeof(pNode->pHToken->subStem->strFeature));
		strcpy(pNode->pHToken->subStem->strFeature, Feature_Stem);
	}
	return isExist;
}

bool CMorphoLogicalAnalysis::IsExistEndingDic(H_NODE* pNode, char *pEndingWord)
{
	bool isExist = false;
	char Feature_End[ENDING_DIC_BASIC_FEATURE_NUMBER + 1];
	char IS_CEHCK_FEATURE = '1';
	memset(Feature_End, 0x00, sizeof(Feature_End));
	isExist = m_pStemEndDic->EndGetRecord(pEndingWord, (char *)Feature_End);
	if (isExist == true)
	{
		CHAR_DELETE(pNode->pHToken->subEnding->strResult);
		CHAR_DELETE(pNode->pHToken->subEnding->strFeature);
		int nLen = strlen(pEndingWord);
		pNode->pHToken->subEnding->strResult = new char[nLen + 1];
		memset(pNode->pHToken->subEnding->strResult, 0x00, nLen + 1);
		strcpy(pNode->pHToken->subEnding->strResult, pEndingWord);
		pNode->pHToken->subEnding->strFeature = new char[ENDING_DIC_BASIC_FEATURE_NUMBER + 1];
		memset(pNode->pHToken->subEnding->strFeature, 0x00, sizeof(pNode->pHToken->subEnding->strFeature));
		strcpy(pNode->pHToken->subEnding->strFeature, Feature_End);
	}
	return isExist;
}


bool CMorphoLogicalAnalysis::IsExistEndingDic(char *pEndingOneSyllable, int nReverse)
{
	bool isExist = false;
	isExist = m_pStemEndDic->EndFindIdx(pEndingOneSyllable, nReverse);
	return isExist;
}


bool CMorphoLogicalAnalysis::IsExistStemDic(char *IsExistStemDic, char *pSaveBuffer)
{
	bool isExist = false;
	return isExist;
}

bool CMorphoLogicalAnalysis::IsExistStemDic(char *pStemSeveralSyllables)
{
	bool isExist = false;
	isExist = m_pStemEndDic->StemFindIdx(pStemSeveralSyllables);
	return isExist;
}

//한글분석의 시작
void CMorphoLogicalAnalysis::StartMorphoLogicalAnalysis()
{
	H_NODE* pNode;
	char* pFeature = NULL;
	m_AnalyzeAgainFromTheBeginning = false;
	m_ApplyIrregularAlgorithm = false;
	m_AnalyzeWhatCallFunction = "";

	pNode = m_LinkedList->m_pHeadNode->pNext;

	while (pNode != m_LinkedList->m_pTailNode)
	{
		if (pNode->pHToken != NULL && pNode->pHToken->strSource != NULL)
		{
			//---------------------------------------------------------------------------------------------------------------------------
			//한글인 경우 문자열이 2Byte이므로 10음절 이하만 분석한다.
			//---------------------------------------------------------------------------------------------------------------------------
			if (m_pCNAHangulUtil->GetFirstToEndSyllableCount(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource) - 1) <= 20)
			{
				#ifdef _DEBUG
					memset(pNode->pHToken->strFlowChartDebug, 0, 1024);
					memset(pNode->pHToken->strAnalysisError, 0, 1024);
				#endif

				//전체어절 어간사전 탐색 모듈
				if (AnalysisModule_00(pNode))
				{
					pNode->pHToken->nKindError = 0;
				}
				else
				{
					pNode->pHToken->nKindError = -999;
					//최장일치(어간 및 어미 분석 후보 결정) 탐색 모듈
					AnalysisModule_01(pNode);

					//복합명사분해
					ExceptionModule_04(pNode);
					//A,B사이의 분석되지 않는 음절 처리
					AfterModule_02(pNode);
				}
			}
			else
			{
				//분석실패
				//분석대상 어절이 10음절 이상
				pNode->pHToken->nKindError = -999;
			}
		}
		m_AnalyzeAgainFromTheBeginning = false;
		m_ApplyIrregularAlgorithm = false;
		m_AnalyzeWhatCallFunction = "";
		pNode = pNode->pNext;
	}

	// 중위성 분석
	CreateAmbiScript();
	
}



//전체어절 어간사전 탐색 모듈
bool CMorphoLogicalAnalysis::AnalysisModule_00(H_NODE* pNode)
{
	return IsExistStemDic(pNode, pNode->pHToken->strSource);
}

//최장일치(어간 및 어미 분석 후보 결정) 탐색 모듈
bool CMorphoLogicalAnalysis::AnalysisModule_01(H_NODE* pNode)
{
	bool bOK = false;
	char* pStemWord = NULL;
	char* pEndingWord = NULL;
	char* pLastWord = NULL;
	char* pFirstWord = NULL;
	char* pStemWord_01 = NULL;
	char* pStemWord_02 = NULL;
	char* pStemWord_03 = NULL;
	char* pStemWord_04 = NULL;
	char* pStemWord_05 = NULL;
	char* pEndingWord_01 = NULL;
	char* pEndingWord_02 = NULL;
	char* pEndingWord_03 = NULL;
	char* pEndingWord_04 = NULL;
	char* pEndingWord_05 = NULL;

	bool bStemFind_01 = false;
	bool bStemFind_02 = false;
	bool bStemFind_03 = false;
	bool bStemFind_04 = false;
	bool bStemFind_05 = false;
	bool bEndingFind_01 = false;
	bool bEndingFind_02 = false;
	bool bEndingFind_03 = false;
	bool bEndingFind_04 = false;
	bool bEndingFind_05 = false;

	int nStartPosition = 0;
	int nEndPosition = 0;
	int nLen = 0;
	int nPos = 0;
	int nABLen = 0;
	

	nEndPosition = strlen(pNode->pHToken->strSource) - 1;
	nLen = strlen(pNode->pHToken->strSource);


	//각 함수에서 최장일치함수를 재 호출 시, 복합(어간,어미) 정보는 초기화 한다.
	if(m_AnalyzeAgainFromTheBeginning == true)
	{
		AnalyzeNodeInitialization(pNode);
	}

	nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, nEndPosition, DIC_ENDING);
	if (nPos >= 0)
	{
		pEndingWord_01 = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, nPos, nEndPosition);
		if (pEndingWord_01 != NULL && strcmp(pEndingWord_01, "") != 0)bEndingFind_01 = IsExistEndingDic(pEndingWord_01, FIND_NOT_REVERSE);
	}

	if (nPos >= 0 && pEndingWord_01 != NULL && strlen(pEndingWord_01) == 2) nPos = nPos - 1;
	if (nPos >= 0)
	{
		nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, nPos, DIC_ENDING);
		pEndingWord_02 = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, nPos, nEndPosition);
		if (pEndingWord_02 != NULL && strcmp(pEndingWord_02, "") != 0) bEndingFind_02 = IsExistEndingDic(pEndingWord_02, FIND_NOT_REVERSE);
	}

	if (nPos >= 0 && pEndingWord_02 != NULL && strlen(pEndingWord_02) == 4) nPos = nPos - 1;
	if (nPos >= 0)
	{
		nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, nPos, DIC_ENDING);
		pEndingWord_03 = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, nPos, nEndPosition);
		if (pEndingWord_03 != NULL && strcmp(pEndingWord_03, "") != 0) bEndingFind_03 = IsExistEndingDic(pEndingWord_03, FIND_NOT_REVERSE);
	}

	if (nPos >= 0 && pEndingWord_03 != NULL && strlen(pEndingWord_03) == 6) nPos = nPos - 1;
	if (nPos >= 0)
	{
		nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, nPos, DIC_ENDING);
		pEndingWord_04 = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, nPos, nEndPosition);
		if (pEndingWord_04 != NULL && strcmp(pEndingWord_04, "") != 0) bEndingFind_04 = IsExistEndingDic(pEndingWord_04, FIND_NOT_REVERSE);
	}

	if (nPos >= 0 && pEndingWord_04 != NULL && strlen(pEndingWord_04) == 8) nPos = nPos - 1;
	if (nPos >= 0)
	{
		nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, nPos, DIC_ENDING);
		pEndingWord_05 = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, nPos, nEndPosition);
		if (pEndingWord_05 != NULL && strcmp(pEndingWord_05, "") != 0) bEndingFind_05 = IsExistEndingDic(pEndingWord_05, FIND_NOT_REVERSE);
	}


	nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, nStartPosition, DIC_STEM);
	if (nPos <= nEndPosition)
	{
		pStemWord_01 = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, nStartPosition, nPos);
		if (pStemWord_01 != NULL && strcmp(pStemWord_01, "") != 0)bStemFind_01 = IsExistStemDic(pStemWord_01);
	}

	if (nPos <= nEndPosition && pStemWord_01 != NULL && strlen(pStemWord_01) == 2) nPos = nPos + 1;
	if (nPos <= nEndPosition)
	{
		nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, nPos, DIC_STEM);
		pStemWord_02 = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, nStartPosition, nPos);
		if (pStemWord_02 != NULL && strcmp(pStemWord_02, "") != 0)bStemFind_02 = IsExistStemDic(pStemWord_02);
	}

	if (nPos <= nEndPosition && pStemWord_02 != NULL && strlen(pStemWord_02) == 4) nPos = nPos + 1;
	if (nPos <= nEndPosition)
	{
		nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, nPos, DIC_STEM);
		pStemWord_03 = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, nStartPosition, nPos);
		if (pStemWord_03 != NULL && strcmp(pStemWord_03, "") != 0)bStemFind_03 = IsExistStemDic(pStemWord_03);
	}

	if (nPos <= nEndPosition && pStemWord_03 != NULL && strlen(pStemWord_03) == 6) nPos = nPos + 1;
	if (nPos <= nEndPosition)
	{
		nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, nPos, DIC_STEM);
		pStemWord_04 = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, nStartPosition, nPos);
		if (pStemWord_04 != NULL && strcmp(pStemWord_04, "") != 0)bStemFind_04 = IsExistStemDic(pStemWord_04);
	}

	if (nPos <= nEndPosition && pStemWord_04 != NULL && strlen(pStemWord_04) == 8) nPos = nPos + 1;
	if (nPos <= nEndPosition)
	{
		nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, nPos, DIC_STEM);
		pStemWord_05 = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, nStartPosition, nPos);
		if (pStemWord_05 != NULL && strcmp(pStemWord_05, "") != 0)bStemFind_05 = IsExistStemDic(pStemWord_05);
	}
	
	
	if(bEndingFind_01 == false && bEndingFind_02 == false && bEndingFind_03 == false && bEndingFind_04 == false && bEndingFind_05 == false)
	{
		if(bStemFind_01 == true || bStemFind_02 == true || bStemFind_03 == true || bStemFind_04 == true || bStemFind_05 == true)
		{
			if(bStemFind_05 == true)
			{
				IsExistStemDic(pNode, pStemWord_05);
				pNode->pHToken->subStem->nFindPos = strlen(pStemWord_05) - 1;
			}
			else if (bStemFind_04 == true)
			{
				IsExistStemDic(pNode, pStemWord_04);
				pNode->pHToken->subStem->nFindPos = strlen(pStemWord_04) - 1;
			}
			else if (bStemFind_03 == true)
			{
				IsExistStemDic(pNode, pStemWord_03);
				pNode->pHToken->subStem->nFindPos = strlen(pStemWord_03) - 1;
			}
			else if (bStemFind_02 == true)
			{
				IsExistStemDic(pNode, pStemWord_02);
				pNode->pHToken->subStem->nFindPos = strlen(pStemWord_02) - 1;
			}
			else if (bStemFind_01 == true)
			{
				IsExistStemDic(pNode, pStemWord_01);
				pNode->pHToken->subStem->nFindPos = strlen(pStemWord_01) - 1;
			}
		}
	}
	else
	{
		nABLen = 0;
		nLen = strlen(pNode->pHToken->strSource);
		if (bEndingFind_05 == true) nABLen = strlen(pEndingWord_05);
		else if (bEndingFind_04 == true) nABLen = strlen(pEndingWord_04);
		else if (bEndingFind_03 == true) nABLen = strlen(pEndingWord_03);
		else if (bEndingFind_02 == true) nABLen = strlen(pEndingWord_02);
		else if (bEndingFind_01 == true) nABLen = strlen(pEndingWord_01);
		//전제어절과 B가 같을 경우 B를 전체어절로 본다
		if (nLen == nABLen)
		{
			if (bEndingFind_05 == true)
			{
				IsExistEndingDic(pNode, pEndingWord_05);
				pNode->pHToken->subEnding->nFindPos = strlen(pNode->pHToken->strSource) - strlen(pEndingWord_05);
			}
			else if (bEndingFind_04 == true)
			{
				IsExistEndingDic(pNode, pEndingWord_04);
				pNode->pHToken->subEnding->nFindPos = strlen(pNode->pHToken->strSource) - strlen(pEndingWord_04);
			}
			else if (bEndingFind_03 == true)
			{
				IsExistEndingDic(pNode, pEndingWord_03);
				pNode->pHToken->subEnding->nFindPos = strlen(pNode->pHToken->strSource) - strlen(pEndingWord_03);
			}
			else if (bEndingFind_02 == true)
			{
				IsExistEndingDic(pNode, pEndingWord_02);
				pNode->pHToken->subEnding->nFindPos = strlen(pNode->pHToken->strSource) - strlen(pEndingWord_02);
			}
			else if (bEndingFind_01 == true)
			{
				IsExistEndingDic(pNode, pEndingWord_01);
				pNode->pHToken->subEnding->nFindPos = strlen(pNode->pHToken->strSource) - strlen(pEndingWord_01);
			}
		}
		else
		{
			if (bStemFind_01 == false && bStemFind_02 == false && bStemFind_03 == false && bStemFind_04 == false && bStemFind_05 == false)
			{
				if (bEndingFind_05 == true)
				{
					IsExistEndingDic(pNode, pEndingWord_05);
					pNode->pHToken->subEnding->nFindPos = strlen(pNode->pHToken->strSource) - strlen(pEndingWord_05);
				}
				else if (bEndingFind_04 == true)
				{
					IsExistEndingDic(pNode, pEndingWord_04);
					pNode->pHToken->subEnding->nFindPos = strlen(pNode->pHToken->strSource) - strlen(pEndingWord_04);
				}
				else if (bEndingFind_03 == true)
				{
					IsExistEndingDic(pNode, pEndingWord_03);
					pNode->pHToken->subEnding->nFindPos = strlen(pNode->pHToken->strSource) - strlen(pEndingWord_03);
				}
				else if (bEndingFind_02 == true)
				{
					IsExistEndingDic(pNode, pEndingWord_02);
					pNode->pHToken->subEnding->nFindPos = strlen(pNode->pHToken->strSource) - strlen(pEndingWord_02);
				}
				else if (bEndingFind_01 == true)
				{
					IsExistEndingDic(pNode, pEndingWord_01);
					pNode->pHToken->subEnding->nFindPos = strlen(pNode->pHToken->strSource) - strlen(pEndingWord_01);
				}
			}
			else
			{
				nABLen = 0;
				//'맛사지도' 최장분석 결과 : B의 후보군 : 도,지도 A의 후보군: 맛, 맛사지
				//B의 최장과 A의 최장 비교 서로 겹칠경우 B의 최장일치를 바꿔야 한다.
				if (bStemFind_05 == true)nABLen += strlen(pStemWord_05);
				else if (bStemFind_04 == true)nABLen += strlen(pStemWord_04);
				else if (bStemFind_03 == true)nABLen += strlen(pStemWord_03);
				else if (bStemFind_02 == true)nABLen += strlen(pStemWord_02);
				else if (bStemFind_01 == true)nABLen += strlen(pStemWord_01);

				if (bEndingFind_05 == true)nABLen += strlen(pEndingWord_05);
				else if (bEndingFind_04 == true)nABLen += strlen(pEndingWord_04);
				else if (bEndingFind_03 == true)nABLen += strlen(pEndingWord_03);
				else if (bEndingFind_02 == true)nABLen += strlen(pEndingWord_02);
				else if (bEndingFind_01 == true)nABLen += strlen(pEndingWord_01);


				if (bStemFind_05 == true)
				{
					IsExistStemDic(pNode, pStemWord_05);
					pNode->pHToken->subStem->nFindPos = strlen(pStemWord_05) - 1;
				}
				else if (bStemFind_04 == true)
				{
					IsExistStemDic(pNode, pStemWord_04);
					pNode->pHToken->subStem->nFindPos = strlen(pStemWord_04) - 1;
				}
				else if (bStemFind_03 == true)
				{
					IsExistStemDic(pNode, pStemWord_03);
					pNode->pHToken->subStem->nFindPos = strlen(pStemWord_03) - 1;
				}
				else if (bStemFind_02 == true)
				{
					IsExistStemDic(pNode, pStemWord_02);
					pNode->pHToken->subStem->nFindPos = strlen(pStemWord_02) - 1;
				}
				else if (bStemFind_01 == true)
				{
					IsExistStemDic(pNode, pStemWord_01);
					pNode->pHToken->subStem->nFindPos = strlen(pStemWord_01) - 1;
				}

				if (bEndingFind_05 == true)
				{
					IsExistEndingDic(pNode, pEndingWord_05);
					pNode->pHToken->subEnding->nFindPos = strlen(pNode->pHToken->strSource) - strlen(pEndingWord_05);
				}
				else if (bEndingFind_04 == true)
				{
					IsExistEndingDic(pNode, pEndingWord_04);
					pNode->pHToken->subEnding->nFindPos = strlen(pNode->pHToken->strSource) - strlen(pEndingWord_04);
				}
				else if (bEndingFind_03 == true)
				{
					IsExistEndingDic(pNode, pEndingWord_03);
					pNode->pHToken->subEnding->nFindPos = strlen(pNode->pHToken->strSource) - strlen(pEndingWord_03);
				}
				else if (bEndingFind_02 == true)
				{
					IsExistEndingDic(pNode, pEndingWord_02);
					pNode->pHToken->subEnding->nFindPos = strlen(pNode->pHToken->strSource) - strlen(pEndingWord_02);
				}
				else if (bEndingFind_01 == true)
				{
					IsExistEndingDic(pNode, pEndingWord_01);
					pNode->pHToken->subEnding->nFindPos = strlen(pNode->pHToken->strSource) - strlen(pEndingWord_01);
				}

				//A+B가 LEN(STR) 보다 큰 경우
				if (nABLen > nLen)
				{
					pFirstWord = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->subEnding->strResult, 0, DIC_STEM);
					pLastWord = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->subStem->strResult, strlen(pNode->pHToken->subStem->strResult) - 1, DIC_ENDING);
					if (strcmp(pLastWord, pFirstWord) == 0)
					{
						nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->subEnding->strResult, 0, DIC_STEM);
						nPos += 1;
						pEndingWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->subEnding->strResult, nPos, strlen(pNode->pHToken->subEnding->strResult) - 1);
						IsExistEndingDic(pNode, pEndingWord);
						pNode->pHToken->subEnding->nFindPos = strlen(pNode->pHToken->strSource) - strlen(pEndingWord_02);
					}
				}
			}
		}
	}
	CHAR_DELETE(pStemWord);
	CHAR_DELETE(pEndingWord);
	CHAR_DELETE(pLastWord);
	CHAR_DELETE(pFirstWord);
	CHAR_DELETE(pStemWord_01);
	CHAR_DELETE(pStemWord_02);
	CHAR_DELETE(pStemWord_03);
	CHAR_DELETE(pStemWord_04);
	CHAR_DELETE(pStemWord_05);
	CHAR_DELETE(pEndingWord_01);
	CHAR_DELETE(pEndingWord_02);
	CHAR_DELETE(pEndingWord_03);
	CHAR_DELETE(pEndingWord_04);
	CHAR_DELETE(pEndingWord_05);

	//어간,어미가 모두 존재하는 경우
	if (pNode->pHToken->subStem->strResult != NULL && pNode->pHToken->subEnding->strResult != NULL)
	{
		//(최장일치를 다시 호출 시에 처음함수와 다른 함수를 호출할 경우 처리)
		if (strcmp(m_AnalyzeWhatCallFunction, "") != 0 && strcmp(m_AnalyzeWhatCallFunction, "AB") != 0)
		{
			m_AnalyzeAgainFromTheBeginning = false;
		}
		m_AnalyzeWhatCallFunction = "AB";
		OfTheFourLogic_AB(pNode);
	}
	//어간만 존재하는 경우
	else if (pNode->pHToken->subStem->strResult != NULL && pNode->pHToken->subEnding->strResult == NULL)
	{
		//(최장일치를 다시 호출 시에 처음함수와 다른 함수를 호출할 경우 처리)
		if (strcmp(m_AnalyzeWhatCallFunction, "") != 0 && strcmp(m_AnalyzeWhatCallFunction, "AX") != 0)
		{
			m_AnalyzeAgainFromTheBeginning = false;
		}
		m_AnalyzeWhatCallFunction = "AX";
		OfTheFourLogic_AX(pNode);
	}
	//어미만 존재하는 경우(불규칙에 대한 처리)
	else if (pNode->pHToken->subStem->strResult == NULL && pNode->pHToken->subEnding->strResult != NULL)
	{
		//(최장일치를 다시 호출 시에 처음함수와 다른 함수를 호출할 경우 처리)
		if (strcmp(m_AnalyzeWhatCallFunction, "") != 0 && strcmp(m_AnalyzeWhatCallFunction, "XB") != 0)
		{
			m_AnalyzeAgainFromTheBeginning = false;
		}
		m_AnalyzeWhatCallFunction = "XB";
		OfTheFourLogic_XB(pNode);
	}
	else
	{
		//(최장일치를 다시 호출 시에 처음함수와 다른 함수를 호출할 경우 처리)
		if (strcmp(m_AnalyzeWhatCallFunction, "") != 0 && strcmp(m_AnalyzeWhatCallFunction, "FF") != 0)
		{
			m_AnalyzeAgainFromTheBeginning = false;
		}
		//분석실패
		m_AnalyzeWhatCallFunction = "FF";
		OfTheFourLogic_FF(pNode);
	}
	
	return bOK;
}



//최장일치의 결과에 따른 4가지 타입 발생
//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
	//A,B 모두 분석 실패
	//예)딥러닝, 챗봇, 밋밋한, 레시피
	//어절의 마지막 음절의 종성 처리 : (ㄴ, ㄹ, ㅁ, ㅂ, ㅆ)
	//어절의 마지막 음절 처리 : (ㄹ, ㅅ, 러, ㅎ, 르, ㄷ, 드, ㅜ, ㅂ불규칙처리) : 0 - B - 1(초중성), B - 1의 종성~B 어간 및 어미 사전 확인
	//어절의 마지막 음절 '케' 확인 및 처리
	
	//전체어절의 끝 음절을  1모듈 적용 예)할
	//전체어절 4모듈 적용 예)막가려운
	//전체어절 5모듈 적용
	//모두 실패시 미등록어로 본다.
//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
void CMorphoLogicalAnalysis::OfTheFourLogic_FF(H_NODE* pNode)
{
	//분석 실패한 어절의 마지막 음절의 위치
	int nLen = strlen(pNode->pHToken->strSource) -1;

	//(0모듈) : 어절의 마지막 음절변경처리모듈(케->하게)
	if (m_AnalyzeAgainFromTheBeginning == false)ExceptionModule_00(pNode);
	//(1모듈) : 어절의 마지막 음절의 종성처리모듈(ㄴ,ㄹ,ㅁ,ㅂ,ㅆ)
	if (m_AnalyzeAgainFromTheBeginning == false)ExceptionModule_01(pNode);
	//(3모듈) : 불규칙처리 모듈(ㄷ,ㅂ,ㅅ,르,우,러,여,오,ㅎ,으,ㄹ,거라,너라불규칙처리)
	if (m_AnalyzeAgainFromTheBeginning == false)ExceptionModule_03(pNode);
}



//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
//A,B모두 존재 STRLEN(A) + STRLEN(B) == STRLEN(전체어절)가 같은 경우만 처리
// 불규칙의 경우 B의 V 자질 있을 경우에만 확인에서 무조건 확인으로 변경함
//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
void CMorphoLogicalAnalysis::OfTheFourLogic_AB(H_NODE* pNode)
{
	int nFromAnalysisLocation = 0;
	int nToAnalysisLocation = 0;
	char* pStartToEndChar = NULL;
	char* pAddTwoWord = NULL;

	nToAnalysisLocation =  pNode->pHToken->subStem->nFindPos -1;
	nFromAnalysisLocation = pNode->pHToken->subEnding->nFindPos - 1;
	
	//A,B사이의 예외상황 
	//하,되,당하,시키가 미분서음절로 남아 있을 경우 A에 해당 미분석음절을 합친 후 종료한다.
	pStartToEndChar  = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, pNode->pHToken->subStem->nFindPos + 1, pNode->pHToken->subEnding->nFindPos - 1);
	if (pStartToEndChar != NULL && (strcmp(pStartToEndChar, "하") == 0 || strcmp(pStartToEndChar, "되") == 0 || strcmp(pStartToEndChar, "당하") == 0 || strcmp(pStartToEndChar, "시키") == 0))
	{
		pAddTwoWord = m_pCNAHangulUtil->GetCombineTwoStrings(pNode->pHToken->subStem->strResult, pStartToEndChar);
	 	CHAR_DELETE(pNode->pHToken->subStem->strResult);
		pNode->pHToken->subStem->strResult = new char[strlen(pAddTwoWord) + 1];
		memset(pNode->pHToken->subStem->strResult, 0x00, sizeof(strlen(pAddTwoWord) + 1));
		strcpy(pNode->pHToken->subStem->strResult, pAddTwoWord);
	}
	else
	{
		//미분석음절이 하나일 경우 : 었,겠... 
		if (m_AnalyzeAgainFromTheBeginning == true && pStartToEndChar != NULL 
			&& strlen(pStartToEndChar) == 2 && IsExistEndingDic(pStartToEndChar, 1) == true)
		{
			pAddTwoWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar,pNode->pHToken->subEnding->strResult);
			CHAR_DELETE(pNode->pHToken->subEnding->strResult);
			pNode->pHToken->subEnding->strResult = new char[strlen(pAddTwoWord) + 1];
			memset(pNode->pHToken->subEnding->strResult, 0x00, sizeof(strlen(pAddTwoWord) + 1));
			strcpy(pNode->pHToken->subEnding->strResult, pAddTwoWord);
		}

		if (nFromAnalysisLocation - nToAnalysisLocation > 1)
		{
			//전처리 어미 분석 후 B-1에 대한 처리
			if (m_AnalyzeAgainFromTheBeginning == false)BeforeModule_01(pNode);
			//(2모듈) : 선어말(복합)어미처리 모듈 //V, AMB + (V or N)일때....선어말어미 체크
			if (m_AnalyzeAgainFromTheBeginning == false)ExceptionModule_02(pNode);
			//(5모듈) : 선어말 처리 후 기타 조사(B)의 정보 학인 및 체크
			if (m_AnalyzeAgainFromTheBeginning == false)ExceptionModule_05(pNode);
			
			//(1모듈) : 어절의 마지막 음절의 종성처리모듈(ㄴ, ㄹ, ㅁ, ㅂ, ㅆ) : 마을사랑한들
			if (m_AnalyzeAgainFromTheBeginning == false)ExceptionModule_01(pNode);
		}
		//해당 음절들이 모두 A,B로 모두 분석이 된 경우
		//불규칙 처리 모듈(3모듈)
		if (m_AnalyzeAgainFromTheBeginning == false)ExceptionModule_03(pNode);
	}
	CHAR_DELETE(pStartToEndChar);
	CHAR_DELETE(pAddTwoWord);
}


//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
//B만 존재
//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
void CMorphoLogicalAnalysis::OfTheFourLogic_XB(H_NODE* pNode)
{
	int nFromAnalysisLocation = 0;
	int nToAnalysisLocation = 0;
	nToAnalysisLocation = 0;
	nFromAnalysisLocation = pNode->pHToken->subEnding->nFindPos - 1;

	//B이외의 다른 음절이 존재할 경우에
	if (pNode->pHToken->subEnding->nFindPos > 0)
	{
		//전처리 어미 분석 후 B-1에 대한 처리
		if (m_AnalyzeAgainFromTheBeginning == false)BeforeModule_01(pNode);
		//(2모듈) : 선어말(복합)어미처리 모듈 //V, AMB + (V or N)일때....선어말어미 체크
		if (nFromAnalysisLocation - nToAnalysisLocation > 1 && m_AnalyzeAgainFromTheBeginning == false)ExceptionModule_02(pNode);
		//(5모듈) : 선어말 처리 후 기타 조사(B)의 정보 학인 및 체크
		if (nFromAnalysisLocation - nToAnalysisLocation > 1 && m_AnalyzeAgainFromTheBeginning == false)ExceptionModule_05(pNode);
		

		//(1모듈) : 어절의 마지막 음절의 종성처리모듈(ㄴ, ㄹ, ㅁ, ㅂ, ㅆ)
		if (m_AnalyzeAgainFromTheBeginning == false)ExceptionModule_01(pNode);
	}

	if (m_AnalyzeAgainFromTheBeginning == false)ExceptionModule_03(pNode);
}




//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
//A만 존재
//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
void CMorphoLogicalAnalysis::OfTheFourLogic_AX(H_NODE* pNode)
{
	int nLen = strlen(pNode->pHToken->strSource) - 1;
	int nFromAnalysisLocation = 0;
	if(nLen > pNode->pHToken->subStem->nFindPos)
	{
		nFromAnalysisLocation = nLen;
		//(0모듈) : 어절의 마지막 음절변경처리모듈(케->하게)
		if (m_AnalyzeAgainFromTheBeginning == false)ExceptionModule_00(pNode);
		//(1모듈) : 어절의 마지막 음절의 종성처리모듈(ㄴ, ㄹ, ㅁ, ㅂ, ㅆ)
		if (m_AnalyzeAgainFromTheBeginning == false)ExceptionModule_01(pNode);
	}

	if (m_AnalyzeAgainFromTheBeginning == false)ExceptionModule_03(pNode);
}



//예외처리로직
//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
//(0모듈) : 어절의 마지막 음절변경처리모듈(케->하게)
//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
void CMorphoLogicalAnalysis::ExceptionModule_00(H_NODE* pNode)
{
	int nPos;
	int nAnalysePosition = 0;
	char* pLastSyllable = NULL;
	char* pChangeLastChar = NULL;

	nAnalysePosition = strlen(pNode->pHToken->strSource) - 1;

	//어미분석이 없을 경우 처리 축약형 처리
	if (pNode->pHToken->subEnding->strResult == NULL)
	{
		nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, nAnalysePosition, DIC_ENDING);
		pLastSyllable = m_pCNAHangulUtil->GetRightToLeftWord(pNode->pHToken->strSource, nPos);

		if (strcmp(pLastSyllable, "케") == 0)
			pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->strSource, "하게", CHANGE_CHAR_SYLLABLE_LAST);
		else if (strcmp(pLastSyllable, "해") == 0)
			pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->strSource, "하어", CHANGE_CHAR_SYLLABLE_LAST);
		else if (strcmp(pLastSyllable, "돼") == 0)
			pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->strSource, "되어", CHANGE_CHAR_SYLLABLE_LAST);
		else if (strcmp(pLastSyllable, "새") == 0)//어느새
			pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->strSource, "사이", CHANGE_CHAR_SYLLABLE_LAST);
		else if (strcmp(pLastSyllable, "려") == 0)//가려
			pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->strSource, "리어", CHANGE_CHAR_SYLLABLE_LAST);
		else if (strcmp(pLastSyllable, "겨") == 0)
			pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->strSource, "기어", CHANGE_CHAR_SYLLABLE_LAST);
		else if (strcmp(pLastSyllable, "져") == 0)
			pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->strSource, "지어", CHANGE_CHAR_SYLLABLE_LAST);
		else if (strcmp(pLastSyllable, "죠") == 0)
			pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->strSource, "지오", CHANGE_CHAR_SYLLABLE_LAST);
		else if (strcmp(pLastSyllable, "춰") == 0)
			pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->strSource, "추어", CHANGE_CHAR_SYLLABLE_LAST);
		else if (strcmp(pLastSyllable, "여") == 0)
			pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->strSource, "이어", CHANGE_CHAR_SYLLABLE_LAST);
		
		if (pChangeLastChar != NULL)
		{
			CHAR_DELETE(pNode->pHToken->strSource);
			pNode->pHToken->strSource = new char[strlen(pChangeLastChar) + 1];
			memset(pNode->pHToken->strSource, 0x00, strlen(pChangeLastChar) + 1);
			strcpy(pNode->pHToken->strSource, pChangeLastChar);

			CHAR_DELETE(pLastSyllable);
			CHAR_DELETE(pChangeLastChar);

			m_AnalyzeAgainFromTheBeginning = true;
			AnalysisModule_01(pNode);
		}
		else
		{
			CHAR_DELETE(pLastSyllable);
			CHAR_DELETE(pChangeLastChar);
		}
	}
}

//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
//(1모듈) : 어절의 마지막 음절의 종성처리모듈(ㄴ,ㄹ,ㅁ,ㅂ,ㅆ)
//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
void CMorphoLogicalAnalysis::ExceptionModule_01(H_NODE* pNode)
{
	int nAnalysePosition = 0;
	char* pOneChar = NULL;
	char* pChoJungChar = NULL;
	char* pJongChar = NULL;
	char* pStartToEndChar = NULL;
	char* pCandidateStemSyllableWord = NULL;
	char* pAddTwoWord = NULL;
	char* pAddTwoCandidateStemSyllableWord = NULL;
	char* pAddTwoCandidateStemEndingSyllableWord = NULL;
	int nStart = 0;
	int nEnd = strlen(pNode->pHToken->strSource) - 1;
	int nABLen  = 0;
	bool bFinalWordPos = false; //해당 음절의 nEnd 위치가 전체 음절의 마지막 음절인지 확인
	
	//예외상황 처리 : 커질 -> 커지 + ㄹ A:X B:질로 분석된 경우 B를 마지막 음절로 보고 종성처리해야 한다.
	if (pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strResult != NULL)
	{
		//1음절의 B로 분석 된경우 B의 종성유무를 파악한 후 있으면 해당 음절부터 해당 로직을 보게 처리한다.
		if (strlen(pNode->pHToken->subEnding->strResult) == 2)
		{
			pOneChar = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nEnd, DIC_ENDING);//해당 위치의 음절 값
			pJongChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pOneChar, CHAR_JONGSUNG);
			if (pJongChar != NULL && strcmp(pJongChar, "") != 0)
			{
				nEnd = strlen(pNode->pHToken->strSource) - 1;
				bFinalWordPos = true;
			}
			else
			{
				nEnd = pNode->pHToken->subEnding->nFindPos - 1;
			}
			CHAR_DELETE(pOneChar);
			CHAR_DELETE(pJongChar);
		}
		else
		{
			nEnd = pNode->pHToken->subEnding->nFindPos - 1;
		}
	}
	if (pNode->pHToken->subStem != NULL && pNode->pHToken->subStem->strResult != NULL)
	{
		nStart = pNode->pHToken->subStem->nFindPos + 1;
	}
	
	//예외상황 발생시 리턴한다.
	if (nStart > nEnd) return;

	//예외상황 1
	//B가 '은'/'는'일 경우에는 음절의 종성을 확인하지 않는다
	if(pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strResult != NULL)
	{
		if(strcmp(pNode->pHToken->subEnding->strResult,"은") == 0 || strcmp(pNode->pHToken->subEnding->strResult, "는") == 0 || 
		   strcmp(pNode->pHToken->subEnding->strResult, "을") == 0 || strcmp(pNode->pHToken->subEnding->strResult, "를") == 0)
		{
			return;
		}
	}
	
	//예외상황 2
	//A가 존재하고 B가 없을 경우 A+1~LEN(STR)이 스템사전에 존재할 경우 종성으로 보지 않는다.
	if (pNode->pHToken->subStem != NULL && pNode->pHToken->subStem->strResult != NULL
		&& pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strResult == NULL)
	{
		pStartToEndChar = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, strlen(pNode->pHToken->subStem->strResult), strlen(pNode->pHToken->strSource)-1);
		if(pStartToEndChar != NULL && strlen(pStartToEndChar) > 2 &&  IsExistStemDic(pStartToEndChar) == true)
		{ 
			CHAR_DELETE(pStartToEndChar);
			return;
		}
		CHAR_DELETE(pStartToEndChar);
	}

	pOneChar = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nEnd, DIC_ENDING);//해당 위치의 음절 값
	pChoJungChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pOneChar, CHAR_CHOJUNGSUNG);
	pJongChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pOneChar, CHAR_JONGSUNG);

	//예외상황 3
	//마지막 음절의 종성이 ㅁ일 경우 마지막음절 포함 2,3음절의 정보를 스템 사전에서 확인한다.
	//스템 사전에 조재 할 경우 아래 로직은 스킵한다.
	//새창열림 : (A)새  B:NULL
	if (pJongChar != NULL && strcmp(pJongChar, "ㅁ") == 0 && pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strResult == NULL)
	{
		char* pWord2 = NULL;
		char* pWord3 = NULL;
		bool bFind2 = false;
		bool bFind3 = false;
		int nPos = 0;
		int nAFindPos = 0;
		if (pNode->pHToken->subStem != NULL && pNode->pHToken->subStem->strResult != NULL)
		{
			nAFindPos = pNode->pHToken->subStem->nFindPos;
		}
		int nEndPosition = strlen(pNode->pHToken->strSource) - 1;
		nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, nEndPosition, DIC_ENDING);
		nPos = nPos - 1;
		//A 존재시 nPos는 A+1까지만 탐색
		if ((nAFindPos < nPos) && nPos >= 0)
		{
			nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, nPos, DIC_ENDING);
			pWord2 = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, nPos, nEndPosition);
			if (pWord2 != NULL && strcmp(pWord2, "") != 0) bFind2 = IsExistStemDic(pWord2);
			if (nPos == 0)nPos = -1;
		}
		//A 존재시 nPos는 A+1까지만 탐색
		if (nPos >= 0 && pWord2 != NULL && strlen(pWord2) == 4) nPos = nPos - 1;
		if ((nAFindPos < nPos) && nPos >= 0)
		{
			nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, nPos, DIC_ENDING);
			pWord3 = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, nPos, nEndPosition);
			if (pWord3 != NULL && strcmp(pWord3, "") != 0) bFind3 = IsExistStemDic(pWord3);
			if (nPos == 0)nPos = -1;
		}
		CHAR_DELETE(pWord2);
		CHAR_DELETE(pWord3);

		if (bFind2 == true || bFind3 == true) 
		{ 
			CHAR_DELETE(pOneChar);
			CHAR_DELETE(pChoJungChar);
			CHAR_DELETE(pJongChar);

			return; 
		}
	}

	//든다 --> 들 + ㄴ다 2017-12-04 추가
	//예정입니다.
	if(strcmp(pJongChar,"ㄴ") == 0 || strcmp(pJongChar, "ㄹ") == 0 || strcmp(pJongChar, "ㅁ") == 0 || strcmp(pJongChar, "ㅂ") == 0 || strcmp(pJongChar, "ㅆ") == 0)
	{
		if (strcmp(pChoJungChar, "하") == 0 || strcmp(pChoJungChar, "해") == 0 || strcmp(pChoJungChar, "되") == 0 || strcmp(pChoJungChar, "돼") == 0 || strcmp(pChoJungChar, "키") == 0 || 
			strcmp(pChoJungChar, "나") == 0 || strcmp(pChoJungChar, "혀") == 0 || strcmp(pChoJungChar, "져") == 0 || strcmp(pChoJungChar, "드") == 0 || strcmp(pChoJungChar, "이") == 0 || 
			strcmp(pChoJungChar, "시") == 0 || strcmp(pChoJungChar, "줘") == 0 || strcmp(pChoJungChar, "지") == 0 || strcmp(pChoJungChar, "리") == 0 || strcmp(pChoJungChar, "가") == 0 ||
			strcmp(pChoJungChar, "뀌") == 0 || strcmp(pChoJungChar, "오") == 0 || strcmp(pChoJungChar, "내") == 0)
		{
			//사랑한들,됐다
			nEnd = nEnd - strlen(pOneChar);
			if (nEnd >= 0) 
			{
				pStartToEndChar = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, nEnd);
			}

			if(pStartToEndChar != NULL && strcmp(pChoJungChar,"하") ==0)
			{
				if(strcmp(pJongChar, "ㄴ") == 0 ) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "하ㄴ");
				else if (strcmp(pJongChar, "ㄹ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "하ㄹ");
				else if (strcmp(pJongChar, "ㅁ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "하ㅁ");
				else if (strcmp(pJongChar, "ㅂ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "하ㅂ");
				else if (strcmp(pJongChar, "ㅆ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "하ㅆ");
			}
			else if (pStartToEndChar != NULL && strcmp(pChoJungChar, "되") == 0)
			{
				if (strcmp(pJongChar, "ㄴ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "되ㄴ");
				else if (strcmp(pJongChar, "ㄹ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "되ㄹ");
				else if (strcmp(pJongChar, "ㅁ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "되ㅁ");
				else if (strcmp(pJongChar, "ㅂ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "되ㅂ");
				else if (strcmp(pJongChar, "ㅆ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "되ㅆ");
			}
			else if (pStartToEndChar != NULL && strcmp(pChoJungChar, "시") == 0)
			{
				if (strcmp(pJongChar, "ㄴ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "시ㄴ");
				else if (strcmp(pJongChar, "ㄹ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "시ㄹ");
				else if (strcmp(pJongChar, "ㅁ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "시ㅁ");
				else if (strcmp(pJongChar, "ㅂ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "시ㅂ");
				else if (strcmp(pJongChar, "ㅆ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "시ㅆ");
			}
			else if (pStartToEndChar != NULL && strcmp(pChoJungChar, "지") == 0)
			{
				if (strcmp(pJongChar, "ㄴ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "지ㄴ");
				else if (strcmp(pJongChar, "ㄹ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "지ㄹ");
				else if (strcmp(pJongChar, "ㅁ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "지ㅁ");
				else if (strcmp(pJongChar, "ㅂ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "지ㅂ");
				else if (strcmp(pJongChar, "ㅆ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "지ㅆ");
			}
			else if (pStartToEndChar != NULL && strcmp(pChoJungChar, "리") == 0)
			{
				if (strcmp(pJongChar, "ㄴ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "리ㄴ");
				else if (strcmp(pJongChar, "ㄹ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "리ㄹ");
				else if (strcmp(pJongChar, "ㅁ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "리ㅁ");
				else if (strcmp(pJongChar, "ㅂ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "리ㅂ");
				else if (strcmp(pJongChar, "ㅆ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "리ㅆ");
			}
			else if (pStartToEndChar != NULL && strcmp(pChoJungChar, "가") == 0)
			{
				if (strcmp(pJongChar, "ㄴ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "가ㄴ");
				else if (strcmp(pJongChar, "ㄹ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "가ㄹ");
				else if (strcmp(pJongChar, "ㅁ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "가ㅁ");
				else if (strcmp(pJongChar, "ㅂ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "가ㅂ");
				else if (strcmp(pJongChar, "ㅆ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "가ㅆ");
			}
			else if (pStartToEndChar != NULL && strcmp(pChoJungChar, "뀌") == 0)
			{
				if (strcmp(pJongChar, "ㄴ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "뀌ㄴ");
				else if (strcmp(pJongChar, "ㄹ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "뀌ㄹ");
				else if (strcmp(pJongChar, "ㅁ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "뀌ㅁ");
				else if (strcmp(pJongChar, "ㅂ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "뀌ㅂ");
				else if (strcmp(pJongChar, "ㅆ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "뀌ㅆ");
			}
			else if (pStartToEndChar != NULL && strcmp(pChoJungChar, "오") == 0)
			{
				if (strcmp(pJongChar, "ㄴ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "오ㄴ");
				else if (strcmp(pJongChar, "ㄹ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "오ㄹ");
				else if (strcmp(pJongChar, "ㅁ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "오ㅁ");
				else if (strcmp(pJongChar, "ㅂ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "오ㅂ");
				else if (strcmp(pJongChar, "ㅆ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "오ㅆ");
			}
			else if (pStartToEndChar != NULL && strcmp(pChoJungChar, "내") == 0)
			{
				if (strcmp(pJongChar, "ㄴ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "내ㄴ");
				else if (strcmp(pJongChar, "ㄹ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "내ㄹ");
				else if (strcmp(pJongChar, "ㅁ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "내ㅁ");
				else if (strcmp(pJongChar, "ㅂ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "내ㅂ");
				else if (strcmp(pJongChar, "ㅆ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "내ㅆ");
			}
			else if (strcmp(pChoJungChar, "해") == 0 && strcmp(pJongChar, "ㅆ") == 0)
			{
				//해 -- 하 + 었 
				//A,B사이에 한음절만 남아 있을 경우 A+하, 었+B로 분석 완료한다. 처음으로 다시 분석되게 처리하지 않는다.
				if (pNode->pHToken->subStem->strResult != NULL && pNode->pHToken->subEnding->strResult != NULL)
				{
					if (strlen(pNode->pHToken->strSource) == (strlen(pNode->pHToken->subStem->strResult) + strlen(pNode->pHToken->subEnding->strResult) + strlen(pOneChar)))
					{
						pAddTwoWord = m_pCNAHangulUtil->GetCombineTwoStrings(pNode->pHToken->subStem->strResult, "하");
						CHAR_DELETE(pNode->pHToken->subStem->strResult);
						pNode->pHToken->subStem->strResult = new char[strlen(pAddTwoWord) + 1];
						memset(pNode->pHToken->subStem->strResult, 0x00, sizeof(strlen(pAddTwoWord) + 1));
						strcpy(pNode->pHToken->subStem->strResult, pAddTwoWord);
						CHAR_DELETE(pAddTwoWord);
						pAddTwoWord = m_pCNAHangulUtil->GetCombineTwoStrings("었", pNode->pHToken->subEnding->strResult);
						CHAR_DELETE(pNode->pHToken->subEnding->strResult);
						pNode->pHToken->subEnding->strResult = new char[strlen(pAddTwoWord) + 1];
						memset(pNode->pHToken->subEnding->strResult, 0x00, sizeof(strlen(pAddTwoWord) + 1));
						strcpy(pNode->pHToken->subEnding->strResult, pAddTwoWord);
					}
				}
				else
				{
					pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "하었");
				}
			}
			else if (strcmp(pChoJungChar, "돼") == 0 && strcmp(pJongChar, "ㅆ") == 0)
			{
				//돼 -- 되 + 었 
				//A,B사이에 한음절만 남아 있을 경우 A+하, 었+B로 분석 완료한다. 처음으로 다시 분석되게 처리하지 않는다.
				if (pNode->pHToken->subStem->strResult != NULL && pNode->pHToken->subEnding->strResult != NULL)
				{
					if (strlen(pNode->pHToken->strSource) == (strlen(pNode->pHToken->subStem->strResult) + strlen(pNode->pHToken->subEnding->strResult) + strlen(pOneChar)))
					{
						pAddTwoWord = m_pCNAHangulUtil->GetCombineTwoStrings(pNode->pHToken->subStem->strResult, "되");
						CHAR_DELETE(pNode->pHToken->subStem->strResult);
						pNode->pHToken->subStem->strResult = new char[strlen(pAddTwoWord) + 1];
						memset(pNode->pHToken->subStem->strResult, 0x00, sizeof(strlen(pAddTwoWord) + 1));
						strcpy(pNode->pHToken->subStem->strResult, pAddTwoWord);
						CHAR_DELETE(pAddTwoWord);
						pAddTwoWord = m_pCNAHangulUtil->GetCombineTwoStrings("었", pNode->pHToken->subEnding->strResult);
						CHAR_DELETE(pNode->pHToken->subEnding->strResult);
						pNode->pHToken->subEnding->strResult = new char[strlen(pAddTwoWord) + 1];
						memset(pNode->pHToken->subEnding->strResult, 0x00, sizeof(strlen(pAddTwoWord) + 1));
						strcpy(pNode->pHToken->subEnding->strResult, pAddTwoWord);
					}
				}
				else
				{
					pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "되었");
				}
			}
			else if (strcmp(pChoJungChar, "키") == 0)
			{
				pAddTwoWord = m_pCNAHangulUtil->GetCombineTwoStrings(pChoJungChar, pJongChar);
				pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, pAddTwoWord);
			}
			else if (strcmp(pChoJungChar, "져") == 0 && strcmp(pJongChar, "ㅆ") == 0)
			{
				pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "지었");
			}
			else if (strcmp(pChoJungChar, "혀") == 0)
			{
				pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "히었");
			}
			else if (strcmp(pChoJungChar, "나") == 0 && strcmp(pJongChar, "ㅆ") == 0)
			{
				pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "나ㅆ");
			}
			else if (strcmp(pChoJungChar, "이") == 0)//입니다
			{
				if(strcmp(pJongChar, "ㅂ") == 0)
					pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "이ㅂ");
				else if (strcmp(pJongChar, "ㄴ") == 0)
					pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "이ㄴ");
			}
			else if (strcmp(pChoJungChar, "줘") == 0 && strcmp(pJongChar, "ㅆ") == 0)//입니다
			{
				pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "주었");
			}
			else if (strcmp(pChoJungChar, "드") == 0 && strcmp(pJongChar, "ㄴ") == 0)
			{
				//1음절일때 분석된 스템 A를 '들'로 바꾸어 버린다
				//if(pNode->pHToken->subStem->strResult != NULL && strlen(pNode->pHToken->subStem->strResult) <= 2)
				if(strlen(pNode->pHToken->strSource) == 2)
					pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(NULL, "들ㄴ");
				else
					pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "들ㄴ");
			}
			if (pAddTwoCandidateStemSyllableWord != NULL)
			{
				//해당 음절의 nEnd 위치가 전체 음절의 마지막 음절인지 확인 마지막 음절이 ㄴ,ㄹ,ㅁ,ㅂ,ㅆ으로 변경되었음으로 문자열을 더할 필요가 없다.
				if (pNode->pHToken->subEnding != NULL && bFinalWordPos == true)
				{
					CHAR_DELETE(pNode->pHToken->subEnding->strResult);
					CHAR_DELETE(pNode->pHToken->subEnding->strFeature);
				}

				//분석된 어미가 있을 경우 해당 어미를 strSource에 더해 다시 분석하게 한다.
				if (pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strResult != NULL)
				{
					pAddTwoCandidateStemEndingSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoCandidateStemSyllableWord, pNode->pHToken->subEnding->strResult);
					CHAR_DELETE(pNode->pHToken->strSource);
					pNode->pHToken->strSource = new char[strlen(pAddTwoCandidateStemEndingSyllableWord) + 1];
					memset(pNode->pHToken->strSource, 0x00, strlen(pAddTwoCandidateStemEndingSyllableWord) + 1);
					strcpy(pNode->pHToken->strSource, pAddTwoCandidateStemEndingSyllableWord);
				}
				else
				{
					if (pNode->pHToken->subEnding != NULL)
					{
						CHAR_DELETE(pNode->pHToken->subEnding->strResult);
						CHAR_DELETE(pNode->pHToken->subEnding->strFeature);
					}
					CHAR_DELETE(pNode->pHToken->strSource);
					pNode->pHToken->strSource = new char[strlen(pAddTwoCandidateStemSyllableWord) + 1];
					memset(pNode->pHToken->strSource, 0x00, strlen(pAddTwoCandidateStemSyllableWord) + 1);
					strcpy(pNode->pHToken->strSource, pAddTwoCandidateStemSyllableWord);
				}
				m_AnalyzeAgainFromTheBeginning = true;
			}
		}
	}
	else
	{
		if (pNode->pHToken->subStem->strResult != NULL) nABLen += strlen(pNode->pHToken->subStem->strResult);
		if (pNode->pHToken->subEnding->strResult != NULL) nABLen += strlen(pNode->pHToken->subEnding->strResult);
		if (pOneChar != NULL) nABLen += strlen(pOneChar);
		if (pNode->pHToken->subStem->strResult != NULL && pNode->pHToken->subEnding->strResult != NULL && strlen(pNode->pHToken->strSource) == nABLen)
		{
			if (strcmp(pOneChar, "봐") == 0)
			{
				pAddTwoWord = m_pCNAHangulUtil->GetCombineTwoStrings(pNode->pHToken->subStem->strResult, "보아");
				pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord, pNode->pHToken->subEnding->strResult);
				CHAR_DELETE(pNode->pHToken->strSource);
				pNode->pHToken->strSource = new char[strlen(pAddTwoCandidateStemSyllableWord) + 1];
				memset(pNode->pHToken->strSource, 0x00, strlen(pAddTwoCandidateStemSyllableWord) + 1);
				strcpy(pNode->pHToken->strSource, pAddTwoCandidateStemSyllableWord);
				
				m_AnalyzeAgainFromTheBeginning = true;
			}
			else
			{
				//B가 1음절 일 경우 B의 종성이 ㄴ,ㄹ,ㅁ,ㅂ,ㅆ인 경우 strSource의 값을 변경해서 다시 분석 시도
				//불거진 --> 불거지 + ㄴ 2017-12-27 추가 A:불 B:진 미분석어:거
				if (pNode->pHToken->subEnding->strResult != NULL && strlen(pNode->pHToken->subEnding->strResult) == 2)
				{
					CHAR_DELETE(pStartToEndChar);
					CHAR_DELETE(pOneChar);
					CHAR_DELETE(pChoJungChar);
					CHAR_DELETE(pJongChar);
					pOneChar = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->subEnding->strResult, 1, DIC_ENDING);//해당 위치의 음절 값
					pChoJungChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pOneChar, CHAR_CHOJUNGSUNG);
					pJongChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pOneChar, CHAR_JONGSUNG);
					if (strcmp(pJongChar, "ㄴ") == 0 || strcmp(pJongChar, "ㄹ") == 0 || strcmp(pJongChar, "ㅁ") == 0 || strcmp(pJongChar, "ㅂ") == 0 || strcmp(pJongChar, "ㅆ") == 0)
					{
						if (strcmp(pChoJungChar, "하") == 0)
						{
							pStartToEndChar = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, strlen(pNode->pHToken->strSource) - strlen(pNode->pHToken->subEnding->strResult) - 1);
							if (pStartToEndChar != NULL && strcmp(pJongChar, "ㄴ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "하ㄴ");
							else if (pStartToEndChar != NULL && strcmp(pJongChar, "ㄹ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "하ㄹ");
							else if (pStartToEndChar != NULL && strcmp(pJongChar, "ㅁ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "하ㅁ");
							else if (pStartToEndChar != NULL && strcmp(pJongChar, "ㅂ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "하ㅂ");
						}
						else if (strcmp(pChoJungChar, "되") == 0)
						{
							pStartToEndChar = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, strlen(pNode->pHToken->strSource) - strlen(pNode->pHToken->subEnding->strResult) - 1);
							if (pStartToEndChar != NULL && strcmp(pJongChar, "ㄴ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "되ㄴ");
							else if (pStartToEndChar != NULL && strcmp(pJongChar, "ㄹ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "되ㄹ");
							else if (pStartToEndChar != NULL && strcmp(pJongChar, "ㅁ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "되ㅁ");
							else if (pStartToEndChar != NULL && strcmp(pJongChar, "ㅂ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "되ㅂ");
						}
						else if (strcmp(pChoJungChar, "시") == 0)
						{
							pStartToEndChar = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, strlen(pNode->pHToken->strSource) - strlen(pNode->pHToken->subEnding->strResult) - 1);
							if (pStartToEndChar != NULL && strcmp(pJongChar, "ㄴ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "시ㄴ");
							else if (pStartToEndChar != NULL && strcmp(pJongChar, "ㄹ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "시ㄹ");
							else if (pStartToEndChar != NULL && strcmp(pJongChar, "ㅁ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "시ㅁ");
							else if (pStartToEndChar != NULL && strcmp(pJongChar, "ㅂ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "시ㅂ");
						}
						else if (strcmp(pChoJungChar, "지") == 0)
						{
							pStartToEndChar = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, strlen(pNode->pHToken->strSource) - strlen(pNode->pHToken->subEnding->strResult) - 1);
							if (pStartToEndChar != NULL && strcmp(pJongChar, "ㄴ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "지ㄴ");
							else if (pStartToEndChar != NULL && strcmp(pJongChar, "ㄹ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "지ㄹ");
							else if (pStartToEndChar != NULL && strcmp(pJongChar, "ㅁ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "지ㅁ");
							else if (pStartToEndChar != NULL && strcmp(pJongChar, "ㅂ") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "지ㅂ");
						}
						if (pAddTwoCandidateStemSyllableWord != NULL)
						{
							//해당 음절의 nEnd 위치가 전체 음절의 마지막 음절인지 확인 마지막 음절이 ㄴ,ㄹ,ㅁ,ㅂ,ㅆ으로 변경되었음으로 문자열을 더할 필요가 없다.
							if (pNode->pHToken->subEnding != NULL && bFinalWordPos == true)
							{
								CHAR_DELETE(pNode->pHToken->subEnding->strResult);
								CHAR_DELETE(pNode->pHToken->subEnding->strFeature);
							}

							CHAR_DELETE(pNode->pHToken->strSource);
							pNode->pHToken->strSource = new char[strlen(pAddTwoCandidateStemSyllableWord) + 1];
							memset(pNode->pHToken->strSource, 0x00, strlen(pAddTwoCandidateStemSyllableWord) + 1);
							strcpy(pNode->pHToken->strSource, pAddTwoCandidateStemSyllableWord);

							m_AnalyzeAgainFromTheBeginning = true;
						}
					}
				}
			}
		}
	}

	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pChoJungChar);
	CHAR_DELETE(pJongChar);
	CHAR_DELETE(pStartToEndChar);
	CHAR_DELETE(pAddTwoWord);
	CHAR_DELETE(pCandidateStemSyllableWord);
	CHAR_DELETE(pAddTwoCandidateStemSyllableWord);
	CHAR_DELETE(pAddTwoCandidateStemEndingSyllableWord);
	
	if(m_AnalyzeAgainFromTheBeginning == true)
	{
		AnalysisModule_01(pNode);
	}
}




//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
//복합어미(선어말어미 등등) 처리 모듈(우->좌)
//1) 주체높임법 : -시 -
//2) 시제 : -는 - <현재>, -었 - <과거>, -겠 - <미래>
//3) 공손 : -옵 -
//4) 상대높임법의 합쇼체 : -ㅂ -
//5) 서법 : -느 - , -더 - , -리 -
//6) 강조법 : -것 - , -니 -
//7) '들' B가 N,Aff이면서 1음절일 경우 B-1이 '들'인 경우 복합어미로 본다
//8) B가 'V'자질이 없을 경우에는 SKIP 한다.
//9) 했,됐에 대한 처리 축약형을 원형복원하고 처음부터 다시 분석하게 해야 하나? 됐->되었 했->하었  
//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
void CMorphoLogicalAnalysis::ExceptionModule_02(H_NODE* pNode)
{
	H_SUBNODE* pStartNode = NULL;
	char* pAddEndingResultWords = NULL;
	char* pOneSyllable = NULL;
	bool bCompoundEndSyllable = false;
	int nFromPosition = 0;
	int nToPosition = 0;
	bool bNextEndingCheck = true;

	
	//B가 다음과 같은 경우 선어말 어미를 분석하지 않는다.
	/*
	if (pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strResult != NULL && strlen(pNode->pHToken->subEnding->strResult) != 2)
	{
		//B가 1음절 이상일 경우 예외 상황 처리 : B-1의 종성에 'ㅆ'이 존재할 경우 skip하지 않고 처리한다.
		if (pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strResult != NULL)nFromPosition = pNode->pHToken->subEnding->nFindPos - 1;
		pOneSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nFromPosition, DIC_ENDING);
		if (pOneSyllable != NULL)
		{
			bNextEndingCheck = false;
			char* pJongChar = NULL;
			pJongChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pOneSyllable, CHAR_JONGSUNG);
			if (strcmp(pJongChar, "ㅆ") == 0)
			{
				bNextEndingCheck = true;
			}
			CHAR_DELETE(pJongChar);
		}
		CHAR_DELETE(pOneSyllable);
		if(bNextEndingCheck == false)
		{
			return;
		}
	}
	*/

	if (strcmp(pNode->pHToken->subEnding->strResult, "가") == 0 || strcmp(pNode->pHToken->subEnding->strResult, "을") == 0 || strcmp(pNode->pHToken->subEnding->strResult, "를") == 0 || strcmp(pNode->pHToken->subEnding->strResult, "의") == 0) return;
	
	
	
	nFromPosition = strlen(pNode->pHToken->strSource) - 1;
	if(pNode->pHToken->subStem != NULL && pNode->pHToken->subStem->strResult != NULL)nToPosition = pNode->pHToken->subStem->nFindPos + 1;
	if (pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strResult != NULL)nFromPosition = pNode->pHToken->subEnding->nFindPos - 1;

	//도시어부들이
	//A가 N의 자질이고 A+1이 '를'일 시 아래의 B의 자질 체크보다 우선순위가 높다.
	if (pNode->pHToken->subStem->strResult != NULL && pNode->pHToken->subEnding->strFeature[STEM_DIC_N] == '1')
	{
		pOneSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nFromPosition, DIC_ENDING);
		if(strcmp(pOneSyllable, "들") == 0)
		{
			bNextEndingCheck = false;
		}
		CHAR_DELETE(pOneSyllable);
	}
	if(bNextEndingCheck == true)
	{
		//예외 상황
		//마지막 음절이 ㄴ,ㄹ,ㅁ인 경우
		//'추진해오ㄴ'으로 변경 된 후 최장일치 함수를 재 호출한 경우
		if (pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strResult != NULL && (strcmp(pNode->pHToken->subEnding->strResult, "ㄴ") == 0 || strcmp(pNode->pHToken->subEnding->strResult, "ㄹ") == 0 || strcmp(pNode->pHToken->subEnding->strResult, "ㅁ") == 0))
		{
			//A+1~B-1까지 어미 사전에서 찾는다.
			char* pEndingWord = NULL;
			if (pNode->pHToken->subStem != NULL &&pNode->pHToken->subStem->strResult != NULL && strcmp(pNode->pHToken->subStem->strResult, "") != 0)
			{
				pEndingWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, strlen(pNode->pHToken->subStem->strResult), pNode->pHToken->subEnding->nFindPos - 1);
			}
			else if (pNode->pHToken->subStem == NULL)
			{
				pEndingWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, pNode->pHToken->subEnding->nFindPos - 1);
			}
			if (pEndingWord != NULL)
			{
				if (IsExistEndingDic(pEndingWord, FIND_NOT_REVERSE) == true)
				{
					if (pNode->pHToken->bSubEndingCompCreate == false)
					{
						m_LinkedList->CreateCompoundSubNode(pNode, "ENDING");
						if (pNode->pHToken->subEnding->strResult != NULL)
						{
							InsertAddCompoundSubTail(pNode, pNode->pHToken->subEnding->strResult, DIC_ENDING, INSERT_RIGHT_TO_LEFT);//복합어미정보 
						}
						InsertAddCompoundSubTail(pNode, pEndingWord, DIC_ENDING, INSERT_RIGHT_TO_LEFT);//복합어미정보 
					}
					else
					{
						InsertAddCompoundSubTail(pNode, pEndingWord, DIC_ENDING, INSERT_RIGHT_TO_LEFT);//복합어미정보 
					}
					bCompoundEndSyllable = true;
					pNode->pHToken->subEnding->nFindPos = strlen(pNode->pHToken->strSource) - strlen(pEndingWord) - strlen(pNode->pHToken->subEnding->strResult);//최종적으로 찾은 어미의 위치값으로 변경한다.
					nFromPosition = 0;
				}
			}
			CHAR_DELETE(pEndingWord);
		}
		else
		{
			if (pNode->pHToken->subEnding->strFeature[ENDING_DIC_V] == '0')
			{
				nFromPosition = 0; 
				return;
			}
			if (pNode->pHToken->subEnding->strFeature[ENDING_DIC_AMB] == '1' && (pNode->pHToken->subEnding->strFeature[ENDING_DIC_V] == '0' || pNode->pHToken->subEnding->strFeature[ENDING_DIC_N] == 'N'))
			{
				nFromPosition = 0;
				return;
			}
		}
	}
	

	if (nFromPosition > 0)
	{
		for (nFromPosition; nToPosition <= nFromPosition; nFromPosition--)
		{
			//예외상황
			//신규사업부터는에서 A:신규 B:는 '는'에 amb n v aff 부여. 1음절의 선어말 어미 못찾았을 경우, 2음절 이상의 선어말 어미를 살펴봐야 한다.
			pOneSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nFromPosition, DIC_ENDING);
			if (strlen(pOneSyllable) == 2) nFromPosition = nFromPosition - 1;//한글일 경우
			if (
				(strcmp(pOneSyllable, "시") == 0 || strcmp(pOneSyllable, "은") == 0
					|| strcmp(pOneSyllable, "는") == 0 || strcmp(pOneSyllable, "았") == 0
					|| strcmp(pOneSyllable, "었") == 0 || strcmp(pOneSyllable, "겠") == 0
					|| strcmp(pOneSyllable, "겄") == 0 || strcmp(pOneSyllable, "옵") == 0
					|| strcmp(pOneSyllable, "느") == 0 || strcmp(pOneSyllable, "더") == 0
					|| strcmp(pOneSyllable, "리") == 0 || strcmp(pOneSyllable, "니") == 0
					|| strcmp(pOneSyllable, "졌") == 0
					//B가 N,Aff이면서 1음절일 경우 B-1이 '들'인 경우 복합어미로 본다
					|| strcmp(pOneSyllable, "들") == 0
					|| strcmp(pOneSyllable, "더") == 0
					)
				&&
				IsExistEndingDic(pOneSyllable, FIND_REVERSE) == true
				)
			{
				if (bCompoundEndSyllable == false) bCompoundEndSyllable = true;
				if (pNode->pHToken->bSubEndingCompCreate == false)
				{
					m_LinkedList->CreateCompoundSubNode(pNode, "ENDING");
					if (pNode->pHToken->subEnding->strResult != NULL)
					{
						InsertAddCompoundSubTail(pNode, pNode->pHToken->subEnding->strResult, DIC_ENDING, INSERT_RIGHT_TO_LEFT);//복합어미정보 
					}
					InsertAddCompoundSubTail(pNode, pOneSyllable, DIC_ENDING, INSERT_RIGHT_TO_LEFT);//복합어미정보 
				}
				else
				{
					InsertAddCompoundSubTail(pNode, pOneSyllable, DIC_ENDING, INSERT_RIGHT_TO_LEFT);//복합어미정보 
				}
				pNode->pHToken->subEnding->nFindPos = nFromPosition;//최종적으로 찾은 어미의 위치값으로 변경한다.
			}
			else
			{
				CHAR_DELETE(pOneSyllable);
				break;
			}
			CHAR_DELETE(pOneSyllable);
		}
	}
	//복합어미분석인 경우 복합으로 분석된 모든 정보를 하나로 합쳐 subEnding->strResult에 다시 저장한다.
	if (bCompoundEndSyllable == true)
	{
		pAddEndingResultWords = new char[strlen(pNode->pHToken->strSource)+1];
		memset(pAddEndingResultWords,0x00,sizeof(pAddEndingResultWords));
		
		if (pNode->pHToken->pSubEndingHeadNode != NULL && pNode->pHToken->pSubEndingHeadNode->pNext != NULL)
		{
			pStartNode = pNode->pHToken->pSubEndingHeadNode->pNext;
			while (pStartNode != pNode->pHToken->pSubEndingTailNode)
			{
				if(pStartNode->pCSubToken != NULL && pStartNode->pCSubToken->strResult != NULL)
				{
					strcat(pAddEndingResultWords,pStartNode->pCSubToken->strResult);
				}
				pStartNode = pStartNode->pNext;
			}
			CHAR_DELETE(pNode->pHToken->subEnding->strResult);
			pNode->pHToken->subEnding->strResult = new char[strlen(pAddEndingResultWords)+1];
			memset(pNode->pHToken->subEnding->strResult,0x00,sizeof(pNode->pHToken->subEnding->strResult));
			strcpy(pNode->pHToken->subEnding->strResult, pAddEndingResultWords);
		}
	}
	else if (bCompoundEndSyllable == false)
	{
		CHAR_DELETE(pOneSyllable);
		nToPosition = 0;
		nFromPosition = 0;
		nFromPosition = strlen(pNode->pHToken->strSource) - 1;
		if (pNode->pHToken->subStem != NULL && pNode->pHToken->subStem->strResult != NULL)nToPosition = pNode->pHToken->subStem->nFindPos + 1;
		if (pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strResult != NULL)nFromPosition = pNode->pHToken->subEnding->nFindPos - 1;
		pOneSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nFromPosition, DIC_ENDING);
		//됐->되었, 했->하었 으로 변경
		if (pOneSyllable != NULL && (strcmp(pOneSyllable, "됐") == 0 || strcmp(pOneSyllable, "했") == 0 || strcmp(pOneSyllable, "해") == 0))
		{
			char* pAddTwoWord_01 = NULL;
			char* pAddTwoWord_02 = NULL;
			char* pAddTwoWord_03 = NULL;
			char* pAddTwoWord_04 = NULL;
			//A,B 존재
			if (pNode->pHToken->subStem != NULL && pNode->pHToken->subStem->strResult != NULL && pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strResult != NULL)  
			{
				pAddTwoWord_01 = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, pNode->pHToken->subStem->nFindPos);
				if(strcmp(pOneSyllable, "됐") == 0) pAddTwoWord_02 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_01, "되었");
				if (strcmp(pOneSyllable, "했") == 0) pAddTwoWord_02 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_01, "하었");
				if (strcmp(pOneSyllable, "해") == 0) pAddTwoWord_02 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_01, "하어");
				pAddTwoWord_03 = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, pNode->pHToken->subEnding->nFindPos, strlen(pNode->pHToken->strSource) - 1);
				pAddTwoWord_04 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_02, pAddTwoWord_03);

				CHAR_DELETE(pNode->pHToken->strSource);
				pNode->pHToken->strSource = new char[strlen(pAddTwoWord_04) + 1];
				memset(pNode->pHToken->strSource, 0x00, strlen(pAddTwoWord_04) + 1);
				strcpy(pNode->pHToken->strSource, pAddTwoWord_04);
			}
			//A만 존재
			else if (pNode->pHToken->subStem != NULL && pNode->pHToken->subStem->strResult != NULL && pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strResult == NULL)
			{
				pAddTwoWord_01 = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, pNode->pHToken->subStem->nFindPos);
				if (strcmp(pOneSyllable, "됐") == 0) pAddTwoWord_02 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_01, "되었");
				if (strcmp(pOneSyllable, "했") == 0) pAddTwoWord_02 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_01, "하었");
				if (strcmp(pOneSyllable, "해") == 0) pAddTwoWord_02 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_01, "하어");
				pAddTwoWord_03 = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, pNode->pHToken->subStem->nFindPos + 1, strlen(pNode->pHToken->strSource) - 1);
				pAddTwoWord_04 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_02, pAddTwoWord_03);

				CHAR_DELETE(pNode->pHToken->strSource);
				pNode->pHToken->strSource = new char[strlen(pAddTwoWord_04) + 1];
				memset(pNode->pHToken->strSource, 0x00, strlen(pAddTwoWord_04) + 1);
				strcpy(pNode->pHToken->strSource, pAddTwoWord_04);
			}
			//B만 존재
			else if (pNode->pHToken->subStem != NULL && pNode->pHToken->subStem->strResult == NULL && pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strResult == NULL)
			{
				pAddTwoWord_01 = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, pNode->pHToken->subEnding->nFindPos -1);
				if (strcmp(pOneSyllable, "됐") == 0) pAddTwoWord_02 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_01, "되었");
				if (strcmp(pOneSyllable, "했") == 0) pAddTwoWord_02 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_01, "하었");
				if (strcmp(pOneSyllable, "해") == 0) pAddTwoWord_02 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_01, "하어");
				pAddTwoWord_03 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_01, pAddTwoWord_02);

				CHAR_DELETE(pNode->pHToken->strSource);
				pNode->pHToken->strSource = new char[strlen(pAddTwoWord_03) + 1];
				memset(pNode->pHToken->strSource, 0x00, strlen(pAddTwoWord_03) + 1);
				strcpy(pNode->pHToken->strSource, pAddTwoWord_03);
			}
			//모두 없을 경우
			else
			{
				//
				int nEndPosition = strlen(pNode->pHToken->strSource) - 1;
				int nLen = strlen(pNode->pHToken->strSource);
				int nPos = 0;
				nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, nEndPosition, DIC_ENDING);
				
				pAddTwoWord_01 = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, nPos - 1);
				if (strcmp(pOneSyllable, "됐") == 0) pAddTwoWord_02 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_01, "되었");
				if (strcmp(pOneSyllable, "했") == 0) pAddTwoWord_02 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_01, "하었");
				if (strcmp(pOneSyllable, "해") == 0) pAddTwoWord_02 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_01, "하어");

				CHAR_DELETE(pNode->pHToken->strSource);
				pNode->pHToken->strSource = new char[strlen(pAddTwoWord_02) + 1];
				memset(pNode->pHToken->strSource, 0x00, strlen(pAddTwoWord_02) + 1);
				strcpy(pNode->pHToken->strSource, pAddTwoWord_02);
			}

			CHAR_DELETE(pAddTwoWord_01);
			CHAR_DELETE(pAddTwoWord_02);
			CHAR_DELETE(pAddTwoWord_03);
			CHAR_DELETE(pAddTwoWord_04);

			m_AnalyzeAgainFromTheBeginning = true;
		}
	}
	CHAR_DELETE(pAddEndingResultWords);
	CHAR_DELETE(pOneSyllable);

	//됐,했의 경우 재분석하게 처리한다.
	if (m_AnalyzeAgainFromTheBeginning == true)
	{
		//기존에 분석된 정보 모두 무시하고 처음부터 분석하게 처리한다.
		m_AnalyzeAgainFromTheBeginning = false;
		AnalysisModule_01(pNode);
	}
}



//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
//불규칙처리 모듈
//(ㄷ,ㅂ,ㅅ,르,우,러,여,오,ㅎ,으,ㄹ,거라,너라불규칙처리)
//A,B[V자질일경우] 사이에 미분석 1음절 존재 시 : A+1~B-1의 마지막 음절, B의 첫 음절(B가 복합인 경우 복합어미의 첫 음절 비교) 비교 예)음악들으니 음악듣 + 으니
//A,B[V자질일경우] 사이에 미분석 1음절 이상 존재 시 : A+1~B-1의 마지막 음절, B의 첫 음절(B가 복합인 경우 복합어미의 첫 음절 비교) 비교 예)음악들으니 음악듣 + 으니
//A,B[V자질일경우] 사이에 미분석 음절 없을 시 : A의 마지막 음절 + B의 첫 음절(B가 복합인 경우 복합어미의 첫 음절 비교) 비교 예)걸어 ->걷 + 어

//A만 존재 시(마지막 음절이 2이상만 분석) :  A+A+1~MAX(LEN)-1), MAX(LEN) 예)가벼워-->가볍 + 어 
//B만[V자질일경우]만 존재 시((B 이전 음절이 2이상만 분석)) :  B-2,B-1의 음절 비교 예)쑥스러워진들-->쑥스럽 + 어 + 진 + 들
//A,B 모두 없을 시(마지막 음절이 2 이상만 분석) :  MAX(LEN)-1, MAX(LEN) 비교  예)껄끄러워-->껄끄럽 + 어
//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
void CMorphoLogicalAnalysis::ExceptionModule_03(H_NODE* pNode)
{
	char* pUnTranslatedSyllable = NULL;//미분석된음절(A~B사이의 음절)
	char* pA_EndSyllable = NULL;//A의마지막음절 또는 (우->좌)의2번째음절
	char* pB_FirstSyllable = NULL;//B의첫음절 또는 (우->좌)의1번째음절
	char* pIrregularRecoverySyllable = NULL;//불규칙복구

	char* pOneChar = NULL;

	char* pA_CharChoJung = NULL;
	char* pA_CharJongChar = NULL;
	char* pA_ChoChar = NULL;//한음절의 초성
	char* pA_JungChar = NULL;//한음절의 중성
	char* pA_JongChar = NULL;//한음절의 종성

	char* pB_CharChoJung = NULL;
	char* pB_CharJongChar = NULL;
	char* pB_ChoChar = NULL;//한음절의 초성
	char* pB_JungChar = NULL;//한음절의 중성
	char* pB_JongChar = NULL;//한음절의 종성


	char* pIrregularRestoreLocationSyllableWord = NULL;//불규칙복원어절(pNode->pHToken->strSource)
	char* pAddTwoWord_01 = NULL;
	char* pAddTwoWord_02 = NULL;
	char* pAddTwoWord_03 = NULL;
	char* pAddTwoWord_04 = NULL;
	char* pAddTwoWord_05 = NULL;

	char* pGrammer = NULL;
	int nA_IrregularRestoreLocation = -1;//불규칙 해당 음절의 복원 위치
	int nB_IrregularRestoreLocation = -1;//불규칙 해당 음절의 복원 위치
	int nPos = 0;
	int nFirstPos =  - 1;
	int nEndPos = -1;
	int nLen = 0;
	int nABLen = 0;

	nLen = strlen(pNode->pHToken->strSource);
	
	//불규칙의 경우 동사/형용사의 용언에 대한 내용임. 
	//만약 기 분석된 스템 A가 N의 자질이 있을 경우... 불규칙 처리를 무시한다.
	if (pNode->pHToken->subStem != NULL && pNode->pHToken->subStem->strResult != NULL)
	{
		if (pNode->pHToken->subStem->strFeature[STEM_DIC_N] == '1')
			return;
		if (pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strResult != NULL)
		{	
			//어미가 2음절 이상으로 분석된 경우 A,B의 분석값이 전체어절과 같을 경우
			if (strlen(pNode->pHToken->subEnding->strResult) >= 4 && strlen(pNode->pHToken->strSource) == (strlen(pNode->pHToken->subStem->strResult) + strlen(pNode->pHToken->subEnding->strResult)))
				return;
		}
	}

	//A,B의 값이 LEN(STR)과 같은 경우 SKIP 한다.
	if (pNode->pHToken->subStem->strResult != NULL) nABLen += strlen(pNode->pHToken->subStem->strResult);
	if (pNode->pHToken->subEnding->strResult != NULL) nABLen += strlen(pNode->pHToken->subEnding->strResult);
	if (nLen == nABLen) return;

	//A,B사이의 미분석 음절이 2음절 이상이라면 SKIP한다.
	if(nLen > nABLen+4) return;
		

	//A,B존재시
	if (pNode->pHToken->subStem != NULL && pNode->pHToken->subStem->strResult != NULL && pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strResult != NULL)
	{
		//A와B사이에 음절이 존재하지 않을 경우
		if (pNode->pHToken->subEnding->nFindPos - pNode->pHToken->subStem->nFindPos ==  1)
		{
			//A의 마지막 음절
			pA_EndSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->subStem->strResult, pNode->pHToken->subStem->nFindPos, DIC_ENDING);
			//B의 첫 음절
			pB_FirstSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->subEnding->strResult, 0, DIC_STEM);
			nB_IrregularRestoreLocation = pNode->pHToken->subEnding->nFindPos;

			//불규칙 해당 음절의 복원 위치
			nA_IrregularRestoreLocation = pNode->pHToken->subStem->nFindPos - 1;
		}
		else
		{
			//A~B사이의 음절
			pUnTranslatedSyllable = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, pNode->pHToken->subStem->nFindPos + 1, pNode->pHToken->subEnding->nFindPos - 1);
			if (pUnTranslatedSyllable != NULL)
			{
				//A~B사이의 음절이 스템사전에 존재할 경우
				if (IsExistStemDic(pUnTranslatedSyllable) == true)
				{
					CHAR_DELETE(pUnTranslatedSyllable);
					return;
				}

				//A의 마지막 음절
				//pA_EndSyllable = m_pCNAHangulUtil->GetPositionOneChar(pUnTranslatedSyllable, strlen(pUnTranslatedSyllable) -1 , DIC_ENDING);
				pA_EndSyllable = m_pCNAHangulUtil->GetPositionOneChar(pUnTranslatedSyllable, 0, DIC_STEM);

				//B의 첫 음절
				//pB_FirstSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->subEnding->strResult,0, DIC_STEM);
				//nB_IrregularRestoreLocation = pNode->pHToken->subEnding->nFindPos;
				pB_FirstSyllable = m_pCNAHangulUtil->GetPositionOneChar(pUnTranslatedSyllable, strlen(pA_EndSyllable), DIC_STEM);
				nB_IrregularRestoreLocation = pNode->pHToken->subStem->nFindPos + strlen(pA_EndSyllable) + strlen(pB_FirstSyllable) - 1;

				//불규칙 해당 음절의 복원 위치
				//nA_IrregularRestoreLocation = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, pNode->pHToken->subEnding->nFindPos - 1, DIC_ENDING);
				nA_IrregularRestoreLocation = pNode->pHToken->subStem->nFindPos + 1;
			}
		}
	}
	//A만 존재시
	else if (pNode->pHToken->subStem != NULL && pNode->pHToken->subStem->strResult != NULL && (pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strResult == NULL))
	{
		//B의 첫 음절
		nPos = strlen(pNode->pHToken->strSource) - 1;
		pB_FirstSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nPos, DIC_ENDING);
		nPos = nPos - 1;
		nB_IrregularRestoreLocation = nPos;
		if(strlen(pB_FirstSyllable) == 2)
		{
			nPos = nPos - 1;
		}
		//A의 마지막 음절
		pA_EndSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nPos,DIC_ENDING);
		nPos = nPos - 1;
		nA_IrregularRestoreLocation = nPos;
	}
	//B만 존재시	
	else if ((pNode->pHToken->subStem != NULL && pNode->pHToken->subStem->strResult == NULL) && pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strResult != NULL)
	{
		//미분석이 2음절 이상일 경우 B를 기준으로 왼쪽의 두 음절을 비교함.
		if(pNode->pHToken->subEnding->nFindPos > 2)
		{
			//B의 첫 음절
			nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, pNode->pHToken->subEnding->nFindPos - 1, DIC_ENDING);
			pB_FirstSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nPos, DIC_STEM);
			nB_IrregularRestoreLocation = nPos;

			if (strlen(pB_FirstSyllable) == 2)
				nPos = nPos - 1;
			//A의 마지막 음절
			pA_EndSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nPos, DIC_ENDING);
		}
		else
		{
			//B의 첫 음절
			pB_FirstSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->subEnding->strResult, 0, DIC_STEM);
			nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, pNode->pHToken->subEnding->nFindPos - 1, DIC_ENDING);
			//A의 마지막 음절
			pA_EndSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nPos, DIC_STEM);
		}
		//불규칙 해당 음절의 복원 위치
		if (pB_FirstSyllable != NULL && pA_EndSyllable != NULL)
		{
			nA_IrregularRestoreLocation = nPos;
			if (strlen(pA_EndSyllable) == 2)
			{
				nA_IrregularRestoreLocation = nA_IrregularRestoreLocation - 1;
			}
		}
	}
	//A,B 모두 분석 실패 시
	else
	{
		//B의 첫 음절
		nPos = strlen(pNode->pHToken->strSource) - 1;
		pB_FirstSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nPos, DIC_ENDING);
		nPos = nPos - 1;
		nB_IrregularRestoreLocation = nPos;
		if (strlen(pB_FirstSyllable) == 2)
		{
			nPos = nPos - 1;
		}
		//(미분석어절)A의 첫 음절
		if(nPos >= 0)
		{
			pA_EndSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nPos, DIC_ENDING);
			nPos = nPos - 1;
			if (nPos < 0) nPos = 0;
			nA_IrregularRestoreLocation = nPos;
		}
		else
		{
			//1음절로 된 어절이 A,B 모두 분석이 되지 않은 경우에 대한 처리 
			//A가 없을 경우 B의 첫음절로 대체한다.
			nPos = strlen(pNode->pHToken->strSource) - 1;
			pA_EndSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nPos, DIC_ENDING);
			nPos = nPos - 1;
			nA_IrregularRestoreLocation = nPos;
		}
	}
	
	if (nA_IrregularRestoreLocation < 0) nA_IrregularRestoreLocation = 0;

	if (pA_EndSyllable != NULL && pB_FirstSyllable != NULL)
	{
		pA_CharChoJung = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pA_EndSyllable, CHAR_CHOJUNGSUNG);
		pA_CharJongChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pA_EndSyllable, CHAR_JONGSUNG);
		pA_ChoChar = new char[3]; memset(pA_ChoChar, 0x00, 3);
		pA_JungChar = new char[3]; memset(pA_JungChar, 0x00, 3);
		pA_JongChar = new char[3]; memset(pA_JongChar, 0x00, 3);
		m_pCNAHangulUtil->GetWordsChoJungJongSyllable(pA_EndSyllable, pA_ChoChar, pA_JungChar, pA_JongChar);

		pB_CharChoJung = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pB_FirstSyllable, CHAR_CHOJUNGSUNG);
		pB_CharJongChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pB_FirstSyllable, CHAR_JONGSUNG);
		pB_ChoChar = new char[3]; memset(pB_ChoChar, 0x00, 3);
		pB_JungChar = new char[3]; memset(pB_JungChar, 0x00, 3);
		pB_JongChar = new char[3]; memset(pB_JongChar, 0x00, 3);
		m_pCNAHangulUtil->GetWordsChoJungJongSyllable(pB_FirstSyllable, pB_ChoChar, pB_JungChar, pB_JongChar);

		//====================================================================================================================================
		//불규칙 선행 조건
		//불규칙 처리를 위한 초중성 값이 기준음절과 기준음절-1의 음절이 존재하지 않을 경우 불규칙 처리를 하지 않는다.
		//====================================================================================================================================
		if (strcmp(pA_CharChoJung, "") != 0 && strcmp(pB_CharChoJung, "") != 0)
		{
			//====================================================================================================================================
			//ㄷ 불규칙
			//검사방법 : "걸", "결", "길", "눌", "달", "들", "물", "불", "실", "컬" 로 끝나는 어간 + 어미첫음절 첫소리가 ㅇ인 경우
			//====================================================================================================================================
			if (pGrammer == NULL)
			{
				if (strcmp(pA_EndSyllable, "걸") == 0 || strcmp(pA_EndSyllable, "걸") == 0 || strcmp(pA_EndSyllable, "결") == 0
					|| strcmp(pA_EndSyllable, "길") == 0 || strcmp(pA_EndSyllable, "눌") == 0 || strcmp(pA_EndSyllable, "달") == 0
					|| strcmp(pA_EndSyllable, "들") == 0 || strcmp(pA_EndSyllable, "물") == 0 || strcmp(pA_EndSyllable, "불") == 0
					|| strcmp(pA_EndSyllable, "실") == 0 || strcmp(pA_EndSyllable, "컬") == 0)
				{
					if (strcmp(pB_ChoChar, "ㅇ") == 0)
					{
						pIrregularRecoverySyllable = m_pCNAHangulUtil->ChangeCompulsionJongSyllable(pA_EndSyllable, "ㄷ");//ㄷ복원
						//노래실어 -> 노래싣어
						pIrregularRestoreLocationSyllableWord = m_pCNAHangulUtil->SetIrregularRestoreLocationSyllableWord(pNode->pHToken->strSource, pIrregularRecoverySyllable, nA_IrregularRestoreLocation);
						pGrammer = "ㄷ";
					}
				}
			}
			//====================================================================================================================================
			//ㅂ 불규칙
			//어간 끝소리 "ㅂ"이 "우"로 바뀌는 활용 형식이다.도와, 고와의 경우 "오" 형태로 변경됨
			//검사방법 : "가", "거", "겨", "고", "구", "기", "까", "꺼", "꼬", "나", "내", "누", "다", "더", "도", "두", "따", "떠", "라", "러", "려", "로", "리", "마", "매", "미", "벼", "서", "쉬", "스", "쑤", "어", "여", "오", "자", "저", "주", "짜", "쩌", "쪼", "쭈", "추", "타", "터", "허" 또는 "설"
			//로 끝나는 어간 + 어미첫음절 초성이 'ㅇ' 이고 중성이 'ㅗ', 'ㅘ', 'ㅜ', 'ㅝ' 인(음절로는 오, 와, 왔, 우, 워, 웠) 경우
			//====================================================================================================================================
			if (pGrammer == NULL)
			{
				if (strcmp(pA_EndSyllable, "가") == 0 || strcmp(pA_EndSyllable, "거") == 0 || strcmp(pA_EndSyllable, "겨") == 0 || strcmp(pA_EndSyllable, "고") == 0 || strcmp(pA_EndSyllable, "구") == 0
					|| strcmp(pA_EndSyllable, "기") == 0 || strcmp(pA_EndSyllable, "까") == 0 || strcmp(pA_EndSyllable, "꺼") == 0 || strcmp(pA_EndSyllable, "꼬") == 0 || strcmp(pA_EndSyllable, "나") == 0
					|| strcmp(pA_EndSyllable, "내") == 0 || strcmp(pA_EndSyllable, "누") == 0 || strcmp(pA_EndSyllable, "다") == 0 || strcmp(pA_EndSyllable, "더") == 0 || strcmp(pA_EndSyllable, "도") == 0
					|| strcmp(pA_EndSyllable, "두") == 0 || strcmp(pA_EndSyllable, "따") == 0 || strcmp(pA_EndSyllable, "떠") == 0 || strcmp(pA_EndSyllable, "라") == 0 || strcmp(pA_EndSyllable, "러") == 0
					|| strcmp(pA_EndSyllable, "려") == 0 || strcmp(pA_EndSyllable, "로") == 0 || strcmp(pA_EndSyllable, "리") == 0 || strcmp(pA_EndSyllable, "마") == 0 || strcmp(pA_EndSyllable, "매") == 0
					|| strcmp(pA_EndSyllable, "미") == 0 || strcmp(pA_EndSyllable, "벼") == 0 || strcmp(pA_EndSyllable, "서") == 0 || strcmp(pA_EndSyllable, "쉬") == 0 || strcmp(pA_EndSyllable, "스") == 0
					|| strcmp(pA_EndSyllable, "쑤") == 0 || strcmp(pA_EndSyllable, "어") == 0 || strcmp(pA_EndSyllable, "여") == 0 || strcmp(pA_EndSyllable, "오") == 0 || strcmp(pA_EndSyllable, "자") == 0
					|| strcmp(pA_EndSyllable, "저") == 0 || strcmp(pA_EndSyllable, "주") == 0 || strcmp(pA_EndSyllable, "짜") == 0 || strcmp(pA_EndSyllable, "쩌") == 0 || strcmp(pA_EndSyllable, "쪼") == 0
					|| strcmp(pA_EndSyllable, "쭈") == 0 || strcmp(pA_EndSyllable, "추") == 0 || strcmp(pA_EndSyllable, "타") == 0 || strcmp(pA_EndSyllable, "터") == 0 || strcmp(pA_EndSyllable, "허") == 0
					|| strcmp(pA_EndSyllable, "설") == 0)
				{
					if (strcmp(pB_ChoChar, "ㅇ") == 0 && (strcmp(pB_JungChar, "오") == 0 || strcmp(pB_JungChar, "와") == 0 || strcmp(pB_JungChar, "우") == 0 || strcmp(pB_JungChar, "워") == 0))
					{
						pIrregularRecoverySyllable = m_pCNAHangulUtil->ChangeCompulsionJongSyllable(pA_EndSyllable, "ㅂ");//ㅂ복원
						pAddTwoWord_01 = m_pCNAHangulUtil->SetIrregularRestoreLocationSyllableWord(pNode->pHToken->strSource, pIrregularRecoverySyllable, nA_IrregularRestoreLocation);
						if (strcmp(pB_JungChar, "오") == 0 || strcmp(pB_JungChar, "와") == 0)
						{
							pIrregularRestoreLocationSyllableWord = m_pCNAHangulUtil->SetIrregularRestoreLocationSyllableWord(pAddTwoWord_01, "아", nB_IrregularRestoreLocation);
						}
						//가까운 -->가깝+ㄴ
						else if (strcmp(pB_JungChar, "우") == 0 || strcmp(pB_JongChar, "ㄴ") == 0)
						{
							pIrregularRestoreLocationSyllableWord = m_pCNAHangulUtil->SetIrregularRestoreLocationSyllableWord(pAddTwoWord_01, "ㄴ", nB_IrregularRestoreLocation);
						}
						else
						{
							pIrregularRestoreLocationSyllableWord = m_pCNAHangulUtil->SetIrregularRestoreLocationSyllableWord(pAddTwoWord_01, "어", nB_IrregularRestoreLocation);
						}
						pGrammer = "ㅂ";
					}
				}
				else
				{
					//껄끄러우니(껄끄럽+니),부끄러우니(부끄럽+니),쑥스러우니(쑥스럽+니)
					//추우면(춥+면),무거우면(무겁+면),매우면(맵+면),더우면(덥+면),가벼우면(가볍+면),아름다우면(아름답+면)
					if (strcmp(pA_EndSyllable, "우") == 0)
					{
						if (strcmp(pB_FirstSyllable, "면") == 0 || strcmp(pB_FirstSyllable, "니") == 0)
						{
							pOneChar = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nA_IrregularRestoreLocation - 1, DIC_ENDING);
							nA_IrregularRestoreLocation = nA_IrregularRestoreLocation - 1;
							if (nA_IrregularRestoreLocation > 0)
							{
								if (strlen(pOneChar) == 2)
								{
									nA_IrregularRestoreLocation = nA_IrregularRestoreLocation - 1;
								}
								pIrregularRecoverySyllable = m_pCNAHangulUtil->ChangeCompulsionJongSyllable(pOneChar, "ㅂ");//ㅂ복원

								pAddTwoWord_01 = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, nA_IrregularRestoreLocation - 1);
								pAddTwoWord_02 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_01, pIrregularRecoverySyllable);
								pIrregularRestoreLocationSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_02, pB_FirstSyllable);
								pGrammer = "ㅂ";
							}
						}

					}
				}
			}
			//====================================================================================================================================
			//ㅅ 불규칙
			//어간 끝소리 "ㅅ"이 홀소리로 시작하는 어미 앞에서 사라지는 활용
			//ex) 긋다 = >그어, 그으니 낫다 = > 나아, 나으니
			//검사방법 : "그", "끄", "나", "무", "부", "이", "자", "저", "지" 로 끝나는 어간과 어미 첫음절 첫소리가 'ㅇ'인 경우
			//====================================================================================================================================
			if (pGrammer == NULL)
			{
				if (strcmp(pA_EndSyllable, "그") == 0 || strcmp(pA_EndSyllable, "끄") == 0 || strcmp(pA_EndSyllable, "나") == 0
					|| strcmp(pA_EndSyllable, "무") == 0 || strcmp(pA_EndSyllable, "부") == 0 || strcmp(pA_EndSyllable, "이") == 0
					|| strcmp(pA_EndSyllable, "자") == 0 || strcmp(pA_EndSyllable, "저") == 0 || strcmp(pA_EndSyllable, "지") == 0)
				{
					if (strcmp(pB_ChoChar, "ㅇ") == 0)
					{
						pIrregularRecoverySyllable = m_pCNAHangulUtil->ChangeCompulsionJongSyllable(pA_EndSyllable, "ㅅ");//ㅅ복원
						pIrregularRestoreLocationSyllableWord = m_pCNAHangulUtil->SetIrregularRestoreLocationSyllableWord(pNode->pHToken->strSource, pIrregularRecoverySyllable, nA_IrregularRestoreLocation);
						pGrammer = "ㅅ";
					}
				}
			}
			//====================================================================================================================================
			//ㄹ 불규칙
			//어간 끝소리 'ㄹ'이 'ㄴ', 'ㄹ', 'ㅂ', '오', '시' 앞에서 사라지는 활용 형식
			//검사방법 : 어간 끝 음절이 "가", "거", "고", "구", "그", "기", "까", "꼬", "끄", "나", "너", "노", "느", "니", "다", "더", "도", "두", "드", "따", "떠", "뚜", "마", "머", "며", "모", "무", "미", "바", "벌", "부", "빌", "빠", "사", "서", "소", "스", "써", "쏘", "쓰", "아", "어", "여", "우", "으", "이", "자", "저", "조", "주", "지", "치", "크", "터", "투", "트", "파", "푸", "허" 이고
			//어미의 첫음절 첫소리가 ㄴ, ㄹ, ㅂ이거나 첫음절이 오, 시, 셨 인 경우
			//====================================================================================================================================
			if (pGrammer == NULL)
			{
				if (strcmp(pA_EndSyllable, "가") == 0 || strcmp(pA_EndSyllable, "거") == 0 || strcmp(pA_EndSyllable, "고") == 0 || strcmp(pA_EndSyllable, "구") == 0 || strcmp(pA_EndSyllable, "그") == 0 ||
					strcmp(pA_EndSyllable, "기") == 0 || strcmp(pA_EndSyllable, "까") == 0 || strcmp(pA_EndSyllable, "꼬") == 0 || strcmp(pA_EndSyllable, "끄") == 0 || strcmp(pA_EndSyllable, "나") == 0 ||
					strcmp(pA_EndSyllable, "너") == 0 || strcmp(pA_EndSyllable, "노") == 0 || strcmp(pA_EndSyllable, "느") == 0 || strcmp(pA_EndSyllable, "니") == 0 || strcmp(pA_EndSyllable, "다") == 0 ||
					strcmp(pA_EndSyllable, "더") == 0 || strcmp(pA_EndSyllable, "도") == 0 || strcmp(pA_EndSyllable, "두") == 0 || strcmp(pA_EndSyllable, "드") == 0 || strcmp(pA_EndSyllable, "따") == 0 ||
					strcmp(pA_EndSyllable, "떠") == 0 || strcmp(pA_EndSyllable, "뚜") == 0 || strcmp(pA_EndSyllable, "마") == 0 || strcmp(pA_EndSyllable, "머") == 0 || strcmp(pA_EndSyllable, "며") == 0 ||
					strcmp(pA_EndSyllable, "모") == 0 || strcmp(pA_EndSyllable, "무") == 0 || strcmp(pA_EndSyllable, "미") == 0 || strcmp(pA_EndSyllable, "바") == 0 || strcmp(pA_EndSyllable, "벌") == 0 ||
					strcmp(pA_EndSyllable, "부") == 0 || strcmp(pA_EndSyllable, "빌") == 0 || strcmp(pA_EndSyllable, "빠") == 0 || strcmp(pA_EndSyllable, "사") == 0 || strcmp(pA_EndSyllable, "서") == 0 ||
					strcmp(pA_EndSyllable, "소") == 0 || strcmp(pA_EndSyllable, "스") == 0 || strcmp(pA_EndSyllable, "써") == 0 || strcmp(pA_EndSyllable, "쓰") == 0 || strcmp(pA_EndSyllable, "아") == 0 ||
					strcmp(pA_EndSyllable, "어") == 0 || strcmp(pA_EndSyllable, "여") == 0 || strcmp(pA_EndSyllable, "우") == 0 || strcmp(pA_EndSyllable, "으") == 0 || strcmp(pA_EndSyllable, "이") == 0 ||
					strcmp(pA_EndSyllable, "자") == 0 || strcmp(pA_EndSyllable, "저") == 0 || strcmp(pA_EndSyllable, "조") == 0 || strcmp(pA_EndSyllable, "주") == 0 || strcmp(pA_EndSyllable, "지") == 0 ||
					strcmp(pA_EndSyllable, "치") == 0 || strcmp(pA_EndSyllable, "크") == 0 || strcmp(pA_EndSyllable, "터") == 0 || strcmp(pA_EndSyllable, "투") == 0 || strcmp(pA_EndSyllable, "트") == 0 ||
					strcmp(pA_EndSyllable, "파") == 0 || strcmp(pA_EndSyllable, "푸") == 0 || strcmp(pA_EndSyllable, "허") == 0)
				{
					if (strcmp(pB_ChoChar, "ㄴ") == 0 || strcmp(pB_ChoChar, "ㄹ") == 0 || strcmp(pB_ChoChar, "ㅂ") == 0
						|| strcmp(pB_FirstSyllable, "오") == 0 || strcmp(pB_FirstSyllable, "시") == 0 || strcmp(pB_FirstSyllable, "셨") == 0)
					{
						pIrregularRecoverySyllable = m_pCNAHangulUtil->ChangeCompulsionJongSyllable(pA_EndSyllable, "ㄹ");
						pIrregularRestoreLocationSyllableWord = m_pCNAHangulUtil->SetIrregularRestoreLocationSyllableWord(pNode->pHToken->strSource, pIrregularRecoverySyllable, nA_IrregularRestoreLocation);
						pGrammer = "ㄹ";
					}
				}
				//삽니다. 팝니다 갑니다 등등..
				else if (strcmp(pA_JongChar, "ㅂ") == 0 &&
					(strcmp(pA_CharChoJung, "가") == 0 || strcmp(pA_CharChoJung, "거") == 0 || strcmp(pA_CharChoJung, "고") == 0 || strcmp(pA_CharChoJung, "구") == 0 || strcmp(pA_CharChoJung, "그") == 0 ||
						strcmp(pA_CharChoJung, "기") == 0 || strcmp(pA_CharChoJung, "까") == 0 || strcmp(pA_CharChoJung, "꼬") == 0 || strcmp(pA_CharChoJung, "끄") == 0 || strcmp(pA_CharChoJung, "나") == 0 ||
						strcmp(pA_CharChoJung, "너") == 0 || strcmp(pA_CharChoJung, "노") == 0 || strcmp(pA_CharChoJung, "느") == 0 || strcmp(pA_CharChoJung, "니") == 0 || strcmp(pA_CharChoJung, "다") == 0 ||
						strcmp(pA_CharChoJung, "더") == 0 || strcmp(pA_CharChoJung, "도") == 0 || strcmp(pA_CharChoJung, "두") == 0 || strcmp(pA_CharChoJung, "드") == 0 || strcmp(pA_CharChoJung, "따") == 0 ||
						strcmp(pA_CharChoJung, "떠") == 0 || strcmp(pA_CharChoJung, "뚜") == 0 || strcmp(pA_CharChoJung, "마") == 0 || strcmp(pA_CharChoJung, "머") == 0 || strcmp(pA_CharChoJung, "며") == 0 ||
						strcmp(pA_CharChoJung, "모") == 0 || strcmp(pA_CharChoJung, "무") == 0 || strcmp(pA_CharChoJung, "미") == 0 || strcmp(pA_CharChoJung, "바") == 0 || strcmp(pA_CharChoJung, "벌") == 0 ||
						strcmp(pA_CharChoJung, "부") == 0 || strcmp(pA_CharChoJung, "빌") == 0 || strcmp(pA_CharChoJung, "빠") == 0 || strcmp(pA_CharChoJung, "사") == 0 || strcmp(pA_CharChoJung, "서") == 0 ||
						strcmp(pA_CharChoJung, "소") == 0 || strcmp(pA_CharChoJung, "스") == 0 || strcmp(pA_CharChoJung, "써") == 0 || strcmp(pA_CharChoJung, "쓰") == 0 || strcmp(pA_CharChoJung, "아") == 0 ||
						strcmp(pA_CharChoJung, "어") == 0 || strcmp(pA_CharChoJung, "여") == 0 || strcmp(pA_CharChoJung, "우") == 0 || strcmp(pA_CharChoJung, "으") == 0 || strcmp(pA_CharChoJung, "이") == 0 ||
						strcmp(pA_CharChoJung, "자") == 0 || strcmp(pA_CharChoJung, "저") == 0 || strcmp(pA_CharChoJung, "조") == 0 || strcmp(pA_CharChoJung, "주") == 0 || strcmp(pA_CharChoJung, "지") == 0 ||
						strcmp(pA_CharChoJung, "치") == 0 || strcmp(pA_CharChoJung, "크") == 0 || strcmp(pA_CharChoJung, "터") == 0 || strcmp(pA_CharChoJung, "투") == 0 || strcmp(pA_CharChoJung, "트") == 0 ||
						strcmp(pA_CharChoJung, "파") == 0 || strcmp(pA_CharChoJung, "푸") == 0 || strcmp(pA_CharChoJung, "허") == 0))
				{
					if (strcmp(pB_ChoChar, "ㄴ") == 0 || strcmp(pB_ChoChar, "ㄹ") == 0 || strcmp(pB_ChoChar, "ㅂ") == 0
						|| strcmp(pB_FirstSyllable, "오") == 0 || strcmp(pB_FirstSyllable, "시") == 0 || strcmp(pB_FirstSyllable, "셨") == 0)
					{
						pIrregularRecoverySyllable = m_pCNAHangulUtil->ChangeCompulsionJongSyllable(pA_CharChoJung, "ㄹ");
						pIrregularRestoreLocationSyllableWord = m_pCNAHangulUtil->SetIrregularRestoreLocationSyllableWord(pNode->pHToken->strSource, pIrregularRecoverySyllable, nA_IrregularRestoreLocation);
						pGrammer = "ㄹ";
					}
				}
			}
			//====================================================================================================================================
			//ㅎ 불규칙 1
			//어간 끝 'ㅎ'이 어미 '-ㄴ'이나 '-ㅁ' 앞에서 사라지는 활용
			//ex) 까만 = > 까맣 / VA + ㄴ / ETM, 가느다람 = > 가느다랗 / VA + ㅁ / ETM
			//검사방법 : 어절끝이 "간", "건", "단", "떤", "란", "런", "만", "먼", "얀", "연" 인 경우(빨간)
			//어쩔끝이 "감", "검", "담", "떰", "람", "럼", "맘", "멈", "얌", "염" 인 경우(빨감)
			//어미끝이 "가", "거", "다", "떠', "라", "러", "마", "머", "야", "여" 이고 어미 첫음절 초성이ㅁ 인 경우 (빨가면)
			//====================================================================================================================================
			if (pGrammer == NULL)
			{
				if (strcmp(pB_FirstSyllable, "간") == 0 || strcmp(pB_FirstSyllable, "건") == 0 || strcmp(pB_FirstSyllable, "단") == 0 || strcmp(pB_FirstSyllable, "떤") == 0 ||
					strcmp(pB_FirstSyllable, "란") == 0 || strcmp(pB_FirstSyllable, "런") == 0 || strcmp(pB_FirstSyllable, "만") == 0 || strcmp(pB_FirstSyllable, "먼") == 0 ||
					strcmp(pB_FirstSyllable, "얀") == 0 || strcmp(pB_FirstSyllable, "연") == 0 ||
					strcmp(pB_FirstSyllable, "감") == 0 || strcmp(pB_FirstSyllable, "검") == 0 || strcmp(pB_FirstSyllable, "담") == 0 || strcmp(pB_FirstSyllable, "떰") == 0 ||
					strcmp(pB_FirstSyllable, "람") == 0 || strcmp(pB_FirstSyllable, "럼") == 0 || strcmp(pB_FirstSyllable, "맘") == 0 || strcmp(pB_FirstSyllable, "멈") == 0 ||
					strcmp(pB_FirstSyllable, "얌") == 0 || strcmp(pB_FirstSyllable, "염") == 0)
				{
					pIrregularRecoverySyllable = m_pCNAHangulUtil->ChangeCompulsionJongSyllable(pB_FirstSyllable, "ㅎ");
					pIrregularRestoreLocationSyllableWord = m_pCNAHangulUtil->SetIrregularRestoreLocationSyllableWord(pNode->pHToken->strSource, pIrregularRecoverySyllable, nB_IrregularRestoreLocation);
					pGrammer = "ㅎ";
				}
				else if (strcmp(pB_ChoChar, "ㅁ") == 0)
				{
					pOneChar = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nB_IrregularRestoreLocation - 1, DIC_ENDING);
					if (strcmp(pOneChar, "가") == 0 || strcmp(pOneChar, "거") == 0 || strcmp(pOneChar, "다") == 0 || strcmp(pOneChar, "떠") == 0 ||
						strcmp(pOneChar, "라") == 0 || strcmp(pOneChar, "러") == 0 || strcmp(pOneChar, "마") == 0 || strcmp(pOneChar, "머") == 0 ||
						strcmp(pOneChar, "야") == 0 || strcmp(pOneChar, "여") == 0)
					{
						nB_IrregularRestoreLocation = nB_IrregularRestoreLocation - 1;
						if (nB_IrregularRestoreLocation > 0)
						{
							if (strlen(pOneChar) == 2)
							{
								nB_IrregularRestoreLocation = nB_IrregularRestoreLocation - 1;
							}
							pIrregularRecoverySyllable = m_pCNAHangulUtil->ChangeCompulsionJongSyllable(pOneChar, "ㅎ");//ㅎ복원
							pIrregularRestoreLocationSyllableWord = m_pCNAHangulUtil->SetIrregularRestoreLocationSyllableWord(pNode->pHToken->strSource, pIrregularRecoverySyllable, nB_IrregularRestoreLocation);
							pGrammer = "ㅎ";
						}
					}
				}
			}
			//====================================================================================================================================
			//ㅎ 불규칙 2
			//어간 끝 'ㅎ'이 어미 '-아/-어' 앞에서 ㅣ로 바뀌어 합쳐지는 활용
			//ex) 까매 = > 까맣 / VA + 어 / EC,
			//검사방법 어간 끝이 "개", "대", "때", "래", "매", "애", "갰", "댔", "랬", "멨", "앴", "게", "레", "메" "겠", "렜", "멨" 인 경우
			//====================================================================================================================================
			if (pGrammer == NULL)
			{
				if (strcmp(pB_FirstSyllable, "개") == 0 || strcmp(pB_FirstSyllable, "대") == 0 || strcmp(pB_FirstSyllable, "때") == 0 || strcmp(pB_FirstSyllable, "래") == 0 ||
					strcmp(pB_FirstSyllable, "매") == 0 || strcmp(pB_FirstSyllable, "애") == 0 || strcmp(pB_FirstSyllable, "갰") == 0 || strcmp(pB_FirstSyllable, "댔") == 0 ||
					strcmp(pB_FirstSyllable, "랬") == 0 || strcmp(pB_FirstSyllable, "멨") == 0 || strcmp(pB_FirstSyllable, "앴") == 0 || strcmp(pB_FirstSyllable, "게") == 0)
				{
					m_pCNAHangulUtil->ChangeCompulsionChoJungJongSyllable(pIrregularRecoverySyllable, pB_ChoChar, "ㅏ", "ㅎ");
					pIrregularRestoreLocationSyllableWord = m_pCNAHangulUtil->SetIrregularRestoreLocationSyllableWord(pNode->pHToken->strSource, pIrregularRecoverySyllable, nB_IrregularRestoreLocation);
					pGrammer = "ㅎ";

				}
				else if (strcmp(pB_FirstSyllable, "멨") == 0 || strcmp(pB_FirstSyllable, "앴") == 0 || strcmp(pB_FirstSyllable, "게") == 0 || strcmp(pB_FirstSyllable, "레") == 0 ||
					strcmp(pB_FirstSyllable, "메") == 0 || strcmp(pB_FirstSyllable, "겠") == 0 || strcmp(pB_FirstSyllable, "렜") == 0 || strcmp(pB_FirstSyllable, "멨") == 0)
				{
					m_pCNAHangulUtil->ChangeCompulsionChoJungJongSyllable(pIrregularRecoverySyllable, pB_ChoChar, "ㅓ", "ㅎ");
					pIrregularRestoreLocationSyllableWord = m_pCNAHangulUtil->SetIrregularRestoreLocationSyllableWord(pNode->pHToken->strSource, pIrregularRecoverySyllable, nB_IrregularRestoreLocation);
					pGrammer = "ㅎ";
				}
			}
			//====================================================================================================================================
			//으 불규칙
			//용언 어간 '으'가 어미 '-아/-어' 앞에서 사라지는 활용 형식
			//ex) 기뻐 = > 기쁘 / VA + 어 / EC, 슬퍼 = > 슬프 / VA + 어 / EC
			//검사방법 : 어간 끝이  "가", "거", "까", "나", "떠", "빠", "뻐", "써", "아", "커", "터", "파", "퍼", "갔", "겄", "깠", "났", "떳", "빴", "뻣", "썻", "앗", "컷", "텃", "팠", "펐"
			//====================================================================================================================================
			if (pGrammer == NULL)
			{
				if (strcmp(pB_FirstSyllable, "가") == 0 || strcmp(pB_FirstSyllable, "거") == 0 || strcmp(pB_FirstSyllable, "까") == 0 || strcmp(pB_FirstSyllable, "나") == 0 ||
					strcmp(pB_FirstSyllable, "떠") == 0 || strcmp(pB_FirstSyllable, "빠") == 0 || strcmp(pB_FirstSyllable, "뻐") == 0 || strcmp(pB_FirstSyllable, "써") == 0 ||
					strcmp(pB_FirstSyllable, "아") == 0 || strcmp(pB_FirstSyllable, "커") == 0 || strcmp(pB_FirstSyllable, "터") == 0 || strcmp(pB_FirstSyllable, "파") == 0 ||
					strcmp(pB_FirstSyllable, "퍼") == 0 || strcmp(pB_FirstSyllable, "갔") == 0 || strcmp(pB_FirstSyllable, "겄") == 0 || strcmp(pB_FirstSyllable, "깠") == 0 ||
					strcmp(pB_FirstSyllable, "났") == 0 || strcmp(pB_FirstSyllable, "떳") == 0 || strcmp(pB_FirstSyllable, "빴") == 0 || strcmp(pB_FirstSyllable, "뻣") == 0 ||
					strcmp(pB_FirstSyllable, "썻") == 0 || strcmp(pB_FirstSyllable, "앗") == 0 || strcmp(pB_FirstSyllable, "컷") == 0 || strcmp(pB_FirstSyllable, "텃") == 0 ||
					strcmp(pB_FirstSyllable, "팠") == 0 || strcmp(pB_FirstSyllable, "펐") == 0)
				{
					pIrregularRecoverySyllable = new char[4];
					memset(pIrregularRecoverySyllable, 0x00, sizeof(pIrregularRecoverySyllable));
					m_pCNAHangulUtil->ChangeCompulsionChoJungJongSyllable(pIrregularRecoverySyllable, pB_ChoChar, "ㅡ", "");
					pIrregularRestoreLocationSyllableWord = m_pCNAHangulUtil->SetIrregularRestoreLocationSyllableWord(pNode->pHToken->strSource, pIrregularRecoverySyllable, nB_IrregularRestoreLocation);
					pGrammer = "으";
				}
			}
			//====================================================================================================================================
			//러 불규칙
			//어미 '-어/-어서'의 '-어'가 '-러'로 바뀌는 활용 형식
			//ex) 검푸르다 = >검푸르러,
			//검사방법 : 전수조사, 대략 10가지 종류이다.그런데 파생어가 만들어지기 나름이라 더 늘어날 수 있다.
			//어간이 "노르", "푸르", "누르", "바르", "이르" 로 끝난다. (심지바르다 같은 경우도 러 불규칙인데, "바르"로 끝난다.)
			//====================================================================================================================================
			if (pGrammer == NULL)
			{
				if (strcmp(pA_EndSyllable, "르") == 0 && strcmp(pB_FirstSyllable, "러") == 0)
				{
					CHAR_DELETE(pNode->pHToken->subEnding->strResult);
					pNode->pHToken->subEnding->strResult = new char[strlen("어") + 1];
					memset(pNode->pHToken->subEnding->strResult, 0x00, strlen("어") + 1);
					strcpy(pNode->pHToken->subEnding->strResult, "어");
					pGrammer = "러";
				}
			}
			//====================================================================================================================================
			//르 불규칙
			//어간의 끝 음절 '르'가 'ㄹ'로 줄고, 어미 '-아/-어'가 '-라/-러'로 바뀌는 활용
			//ex) 몰라 = > 모르 / VA + 아 / EC
			//검사방법 : "으"불규칙, "러" 불규칙이 아니고 어간 끝 음절이 ㄹ라 / ㄹ러 로 끝나는 경우
			//====================================================================================================================================
			if (pGrammer == NULL)
			{
				if (pA_CharJongChar != NULL && strcmp(pA_CharJongChar, "ㄹ") == 0)
				{
					if (strcmp(pB_FirstSyllable, "라") == 0 || strcmp(pB_FirstSyllable, "러") == 0)
					{
						pAddTwoWord_01 = m_pCNAHangulUtil->GetCombineTwoStrings(pA_CharChoJung, "르");
						if (strcmp(pB_FirstSyllable, "라") == 0)
							pAddTwoWord_02 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_01, "아");
						else
							pAddTwoWord_02 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_01, "어");
						pIrregularRestoreLocationSyllableWord = new char[strlen(pAddTwoWord_02) + 1];
						memset(pIrregularRestoreLocationSyllableWord, 0x00, strlen(pAddTwoWord_02) + 1);
						strcpy(pIrregularRestoreLocationSyllableWord, pAddTwoWord_02);
						pGrammer = "르";
					}
				}
			}
			//====================================================================================================================================
			//우 불규칙
			//어간 끝 '우'가 어미 '-어' 앞에서 사라지는 활용 형식
			//ex) 퍼 = > 푸 / VV + 어 / EC 가 유일하다.
			//검사방법 : 퍼가 유일하다고 해서 퍼만 검사하면 안된다.과거형도 고려해야 한다. "퍼", "펐" 인 경우
			//====================================================================================================================================
			if (pGrammer == NULL)
			{
				if (strcmp(pA_JungChar, "어") == 0)
				{
					pIrregularRecoverySyllable = new char[4];
					memset(pIrregularRecoverySyllable, 0x00, 4);
					if (pA_ChoChar != NULL && strcmp(pA_ChoChar, "ㅍ") == 0)
					{
						//'퍼'->'푸'로 변경
						m_pCNAHangulUtil->ChangeCompulsionChoJungJongSyllable(pIrregularRecoverySyllable, "ㅍ", "ㅜ", "");
					}
					if (IsExistStemDic(pIrregularRecoverySyllable) == true)
					{
						pIrregularRestoreLocationSyllableWord = m_pCNAHangulUtil->SetIrregularRestoreLocationSyllableWord(pNode->pHToken->strSource, pIrregularRecoverySyllable, nA_IrregularRestoreLocation);
						pGrammer = "우";
					}
				}
			}
		}
	}


	if (pIrregularRestoreLocationSyllableWord != NULL)
	{
		CHAR_DELETE(pNode->pHToken->strSource);
		pNode->pHToken->strSource = new char[strlen(pIrregularRestoreLocationSyllableWord) + 1];
		memset(pNode->pHToken->strSource, 0x00, strlen(pIrregularRestoreLocationSyllableWord) + 1);
		strcpy(pNode->pHToken->strSource, pIrregularRestoreLocationSyllableWord);
		m_AnalyzeAgainFromTheBeginning = true;
		m_ApplyIrregularAlgorithm = true; 
	}

	
	CHAR_DELETE(pUnTranslatedSyllable);
	CHAR_DELETE(pA_EndSyllable);
	CHAR_DELETE(pB_FirstSyllable);
	CHAR_DELETE(pIrregularRecoverySyllable);
	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pA_CharChoJung);
	CHAR_DELETE(pA_CharJongChar);
	CHAR_DELETE(pA_ChoChar);
	CHAR_DELETE(pA_JungChar);
	CHAR_DELETE(pA_JongChar);
	CHAR_DELETE(pB_CharChoJung);
	CHAR_DELETE(pB_CharJongChar);
	CHAR_DELETE(pB_ChoChar);
	CHAR_DELETE(pB_JungChar);
	CHAR_DELETE(pB_JongChar);
	CHAR_DELETE(pIrregularRestoreLocationSyllableWord);
	CHAR_DELETE(pAddTwoWord_01);
	CHAR_DELETE(pAddTwoWord_02);
	CHAR_DELETE(pAddTwoWord_03);
	CHAR_DELETE(pAddTwoWord_04);
	CHAR_DELETE(pAddTwoWord_05);

	
	if(m_AnalyzeAgainFromTheBeginning == true)
	{
		AnalysisModule_01(pNode);
	}
}




//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
//복합어처리(복합어간) 모듈
//A,B 존재시 A,B사이의 음절 TYPE=1
//A,B모두 없을 시 TYPE=2
//B만 존재시, 0~B-1 음절 확인 TYPE=3
//A만 존재시, A+1 확인 TYPE=4
//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
void CMorphoLogicalAnalysis::ExceptionModule_04(H_NODE* pNode)
{
	int nCompoundType = 0;
	int nStartPosition = 0;
	int nEndPosition = 0;
	if(pNode->pHToken->subStem->strResult != NULL && pNode->pHToken->subEnding->strResult != NULL)
	{
		if (strlen(pNode->pHToken->strSource) > strlen(pNode->pHToken->subStem->strResult) + strlen(pNode->pHToken->subEnding->strResult))
		{
			nCompoundType = 1;
			nStartPosition = pNode->pHToken->subStem->nFindPos + 1;
			nEndPosition = pNode->pHToken->subEnding->nFindPos - 1;
			//B가 복합어절로 된 경우
			if (pNode->pHToken->bSubEndingCompCreate == true) 
			{
				nEndPosition = strlen(pNode->pHToken->strSource) - strlen(pNode->pHToken->subEnding->strResult) - 1;
			}
		}
		//A와B가 전체어절과 같을 경우 A+B == 전체어절
		else if (strlen(pNode->pHToken->strSource) == strlen(pNode->pHToken->subStem->strResult) + strlen(pNode->pHToken->subEnding->strResult))
		{
			//nCompoundType = 2;
			//nStartPosition = 0;
			//nEndPosition = strlen(pNode->pHToken->strSource) - 1;

			//A+B 정보를 어간사전에 찾아본다. 없을 경우 SKIP 더이상 분석 안한다.
			//A+B가 있을 경우 전체를 A로 본다.
			char* pAddTwoWord = NULL;
			pAddTwoWord = m_pCNAHangulUtil->GetCombineTwoStrings(pNode->pHToken->subStem->strResult, pNode->pHToken->subEnding->strResult);
			if(IsExistStemDic(pAddTwoWord) == true)
			{
				IsExistStemDic(pNode, pAddTwoWord);
				pNode->pHToken->subStem->nFindPos = strlen(pAddTwoWord) - 1;
				CHAR_DELETE(pNode->pHToken->subEnding->strResult);
			}
			/*
			else
			{
				nCompoundType = 2;
				nStartPosition = 0;
				nEndPosition = strlen(pNode->pHToken->strSource) - 1;
			}
			*/
			CHAR_DELETE(pAddTwoWord);
			
		}
	}
	else if (pNode->pHToken->subStem->strResult == NULL && pNode->pHToken->subEnding->strResult == NULL)
	{
		nCompoundType = 3;
		nStartPosition = 0;
		nEndPosition = strlen(pNode->pHToken->strSource)  - 1;
	}
	else if (pNode->pHToken->subStem->strResult == NULL && pNode->pHToken->subEnding->strResult != NULL 
			&& strlen(pNode->pHToken->strSource) != strlen(pNode->pHToken->subEnding->strResult))
	{
		nCompoundType = 4;
		nStartPosition = 0;
		nEndPosition = pNode->pHToken->subEnding->nFindPos - 1;
	}
	else if (pNode->pHToken->subStem->strResult != NULL && pNode->pHToken->subEnding->strResult == NULL 
		&& strlen(pNode->pHToken->strSource) != strlen(pNode->pHToken->subStem->strResult))
	{
		nCompoundType = 5;
		nStartPosition = pNode->pHToken->subStem->nFindPos + 1;
		nEndPosition = strlen(pNode->pHToken->strSource) - 1;
	}
	if(nCompoundType > 0)
	{
		//복합명사로 추정하여 최장정보에 해당하는 정보를 보관한다.
		m_LinkedList->CreateCompoundSubNode(pNode, "STEM");
		CompoundNounDecomposition(pNode, nStartPosition, nEndPosition, nCompoundType);//복합명사분해
	}
}



//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
//조사처리 모듈(선어말 외의 B에 해당하는 정보 찾는 모듈)
//선어말 어미 처리 후 기타 어미 후보들에 대한 처리(2음절,3음절,4음절까지 확인한다 :74%에 해당함)
//1음절:123 2음절:590 3음절:1034 4음절:918 5음절:512 6음절:208 7음절:35 8음절:8 9음절:2
//신규사업부터는  A:신규 B:는 '부터'가 B로 분석이 되어야 한다.
//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
void CMorphoLogicalAnalysis::ExceptionModule_05(H_NODE* pNode)
{
	int nCompoundType = 0;
	int nStartPosition = 0;
	int nEndPosition = 0;
	if (pNode->pHToken->subStem->strResult != NULL && pNode->pHToken->subEnding->strResult != NULL)
	{
		if (strlen(pNode->pHToken->strSource) > strlen(pNode->pHToken->subStem->strResult) + strlen(pNode->pHToken->subEnding->strResult))
		{
			nCompoundType = 1;
			nStartPosition = pNode->pHToken->subStem->nFindPos + 1;
			nEndPosition = pNode->pHToken->subEnding->nFindPos - 1;
		}
	}
	if (nCompoundType > 0)
	{
		//복합조사로 추정하여
		CompoundPostPositionDecomposition(pNode, nStartPosition, nEndPosition, nCompoundType);//복합조사분해
	}
}


//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
//B-1의 1음절 처리
//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
void CMorphoLogicalAnalysis::BeforeModule_01(H_NODE* pNode)
{
	char* pOneChar = NULL;
	char* pAddTwoWord = NULL;
	int nLen = 0;
	if (pNode->pHToken->subEnding->strResult != NULL)
	{
		//이동통신업체들이, 제과점영업이거나
		if(pNode->pHToken->subEnding->nFindPos > 0)
		{
			pOneChar = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, pNode->pHToken->subEnding->nFindPos - 1, DIC_ENDING);
			if(strcmp(pOneChar,"들") == 0 || strcmp(pOneChar, "이") == 0 || strcmp(pOneChar, "에") == 0 || strcmp(pOneChar, "와") == 0)
			{
				pNode->pHToken->subEnding->nFindPos = pNode->pHToken->subEnding->nFindPos - 2;
				pAddTwoWord = m_pCNAHangulUtil->GetCombineTwoStrings(pOneChar,pNode->pHToken->subEnding->strResult);
				nLen = strlen(pAddTwoWord) + 1;
				CHAR_DELETE(pNode->pHToken->subEnding->strResult);
				pNode->pHToken->subEnding->strResult = new char[nLen];
				memset(pNode->pHToken->subEnding->strResult, 0x00, sizeof(nLen));
				strcpy(pNode->pHToken->subEnding->strResult, pAddTwoWord);
			}
		}
	}
	CHAR_DELETE(pOneChar);
	CHAR_DELETE(pAddTwoWord);
	
}

//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
//복합명사 미분해 음절 복합명사의 리스트로 추가 및 A로 미리 분석된 어절 정보 복합명사 Head에 추가
//우->좌 복합명사 분석
//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
void CMorphoLogicalAnalysis::AfterModule_01(H_NODE* pNode)
{
	int nArrayLen = 0;
	int nSourceLength = 0;
	int nCompoundLength = 0;
	int nWhileCompoundLength = 0;
	bool bCompoundAnalyse = false;
	H_SUBNODE* pStartNode = NULL;
	int nEndPosition = -1;
	char* strSource = NULL;
	char* pUnAnalyseCompondWords = NULL;
	char* pAddStemResultWords = NULL;

	//불규칙 모듈 적용 후 원음절이 변경 되었을 경우 원래 문장으로 원복하여 비교한다.
	if(m_ApplyIrregularAlgorithm == true)
	{
		if (strcmp(pNode->pHToken->strOriginalSource, pNode->pHToken->strSource) != 0)
		{
			CHAR_DELETE(pNode->pHToken->strSource);
			pNode->pHToken->strSource = new char[strlen(pNode->pHToken->strOriginalSource) + 1];
			memset(pNode->pHToken->strSource, 0x00, sizeof(pNode->pHToken->strSource));
			strcpy(pNode->pHToken->strSource, pNode->pHToken->strOriginalSource);
		}
	}

	if(pNode->pHToken->subStem->strResult != NULL)
		nCompoundLength = strlen(pNode->pHToken->subStem->strResult);
	if(pNode->pHToken->subEnding->strResult == NULL)
	{
		nEndPosition = strlen(pNode->pHToken->strSource) - 1;
	}
	else
	{
		nEndPosition = pNode->pHToken->subEnding->nFindPos - 1;
	}
	strSource = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, nEndPosition);
	if (strSource != NULL)
	{
		nSourceLength = strlen(strSource);
	}
	if (pNode->pHToken->pSubHeadNode != NULL && pNode->pHToken->pSubHeadNode->pNext != NULL)
	{
		nArrayLen = strlen(pNode->pHToken->strSource) + 1;
		pAddStemResultWords = new char[nArrayLen];
		memset(pAddStemResultWords, 0x00, nArrayLen);

		pStartNode = pNode->pHToken->pSubHeadNode->pNext;
		while (pStartNode != pNode->pHToken->pSubTailNode)
		{
			nCompoundLength += strlen(pStartNode->pCSubToken->strResult);
			nWhileCompoundLength += strlen(pStartNode->pCSubToken->strResult);
			pStartNode = pStartNode->pNext;
			if(bCompoundAnalyse == false)
				bCompoundAnalyse = true;
		}
		if(bCompoundAnalyse == true)
		{
			//미분석된 어절 중 복합명사 분석에 실패된 값이 존재할 경우 - 모든 정보를 미등록어로 보고 A로 대체한다.
			if(nSourceLength > nCompoundLength)
			{
				if(pNode->pHToken->subStem->strResult != NULL)
				{
					pUnAnalyseCompondWords = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, strlen(pNode->pHToken->subStem->strResult), pNode->pHToken->nLastFindCompoundNoun + strlen(pNode->pHToken->subStem->strResult) - 1);
				}
				else
				{
					//nLastFindCompoundNoun 찾은 어절의 첫위치값
					pUnAnalyseCompondWords = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, pNode->pHToken->nLastFindCompoundNoun - 1);
				}
				InsertAddCompoundSubTail(pNode, pUnAnalyseCompondWords, DIC_STEM, INSERT_RIGHT_TO_LEFT);
			}
			if (pNode->pHToken->subStem->strResult != NULL)
			{
				InsertAddCompoundSubTail(pNode, pNode->pHToken->subStem->strResult, DIC_STEM, INSERT_RIGHT_TO_LEFT);
			}
			pStartNode = pNode->pHToken->pSubHeadNode->pNext;
			while (pStartNode != pNode->pHToken->pSubTailNode)
			{
				strcat(pAddStemResultWords, pStartNode->pCSubToken->strResult);
				pStartNode = pStartNode->pNext;
			}
			//미등록어전부를 스템의 결과값을 변경한다.
			if (pAddStemResultWords != NULL)
			{
				CHAR_DELETE(pNode->pHToken->subStem->strResult);
				CHAR_DELETE(pNode->pHToken->subStem->strFeature);
				nArrayLen = strlen(pAddStemResultWords) + 1;
				pNode->pHToken->subStem->strResult = new char[nArrayLen];
				memset(pNode->pHToken->subStem->strResult, 0x00, nArrayLen);
				strcpy(pNode->pHToken->subStem->strResult, pAddStemResultWords);
				pNode->pHToken->subStem->strFeature = new char[STEM_DIC_ALL_FEATURE_NUMBER + 1];
				memset(pNode->pHToken->subStem->strFeature, 0x00, sizeof(pNode->pHToken->subStem->strFeature));
				for (int i = 0; i < STEM_DIC_ALL_FEATURE_NUMBER + 1; i++) {
					pNode->pHToken->subStem->strFeature[i] = '0';
				}
				pNode->pHToken->subStem->strFeature[STEM_DIC_N] = '1';//N명사자질부여
			}
		}
	}
	CHAR_DELETE(strSource);
	CHAR_DELETE(pUnAnalyseCompondWords);
	CHAR_DELETE(pAddStemResultWords);
}

//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
//A, B 사의의 분석되지 않는 결과 음절 처리 : A + 1~B 전체를 어미로 보고 처리
//어미 사전 실패시 - 전체어절의 마지막 음절의 
//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
void CMorphoLogicalAnalysis::AfterModule_02(H_NODE* pNode)
{
	char* strSource = NULL;
	char* pStartToEndChar = NULL;
	char* pAddTwoCandidateStemSyllableWord = NULL;
	char* pAddStemResultWords = NULL;

	H_SUBNODE* pStartNode = NULL;
	int nArrayLen = 0;
	int nStrLength = 0;
	int nAnanlyzeLength = 0;
	int nStartPosition = 0;
	int nEndPosition = 0;
	int nPos = 0;
	int nCompoundLen = 0;
	bool bFind = false;
	bool bOk = false;

	
	nStrLength = strlen(pNode->pHToken->strSource);
	if(pNode->pHToken->subStem->strResult != NULL) nAnanlyzeLength += strlen(pNode->pHToken->subStem->strResult);
	if (pNode->pHToken->subEnding->strResult != NULL) nAnanlyzeLength += strlen(pNode->pHToken->subEnding->strResult);

	//입력어절을 모두 분석한경우 SKIP 한다.
	if (pNode->pHToken->subStem->strResult != NULL && pNode->pHToken->subEnding->strResult != NULL)
	{
		if (strlen(pNode->pHToken->strSource) <= strlen(pNode->pHToken->subStem->strResult) + strlen(pNode->pHToken->subEnding->strResult))
			return;
	}

	if (pNode->pHToken->bSubCompCreate == true)
	{
		//nEndPosition을 재 설정한다.
		pStartNode = pNode->pHToken->pSubHeadNode->pNext;
		while (pStartNode != pNode->pHToken->pSubTailNode)
		{
			nCompoundLen += strlen(pStartNode->pCSubToken->strResult);
			pStartNode = pStartNode->pNext;
		}
	}

	if(pNode->pHToken->subStem->strResult != NULL && nStrLength > nAnanlyzeLength && nAnanlyzeLength > 0)
	{
		nStartPosition = strlen(pNode->pHToken->subStem->strResult);
		nEndPosition = strlen(pNode->pHToken->strSource) - 1;
		strSource = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, nStartPosition, nEndPosition);
		if (strSource != NULL && strcmp(strSource, "") != 0)
		{
			bFind = IsExistEndingDic(pNode, strSource);
			//어미로 분석 되지 않는 경우 미분석 복합어 본다.
			if (bFind == false)
			{
				if (pNode->pHToken->subStem->strResult != NULL && pNode->pHToken->subEnding->strResult != NULL)
				{
					CHAR_DELETE(strSource);
					strSource = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, strlen(pNode->pHToken->subStem->strResult), (strlen(pNode->pHToken->strSource) - (nCompoundLen + strlen(pNode->pHToken->subEnding->strResult))) - 1);
					if(strSource != NULL)
					{
						if (pNode->pHToken->bSubCompCreate == false)
						{
							m_LinkedList->CreateCompoundSubNode(pNode, "STEM");
							if (pNode->pHToken->subStem->strResult != NULL)
							{
								//INSERT_RIGHT_TO_LEFT 맨앞에 추가
								InsertAddCompoundSubTail(pNode, strSource, DIC_STEM, INSERT_RIGHT_TO_LEFT);
								InsertAddCompoundSubTail(pNode, pNode->pHToken->subStem->strResult, DIC_STEM, INSERT_RIGHT_TO_LEFT);
							}
							else
							{
								InsertAddCompoundSubTail(pNode, strSource, DIC_STEM, INSERT_RIGHT_TO_LEFT);
							}
						}
						else
						{
							//INSERT_RIGHT_TO_LEFT 맨앞에 추가
							InsertAddCompoundSubTail(pNode, strSource, DIC_STEM, INSERT_RIGHT_TO_LEFT);
							//복합명사 모듈에서 중간에 실패시 A와 미분석된 음절에 대한 처리를 하지 않으므로 해당 조건문에서 처리한다.
							if (pNode->pHToken->subStem->strResult != NULL)
							{
								//INSERT_RIGHT_TO_LEFT 맨앞에 추가
								InsertAddCompoundSubTail(pNode, pNode->pHToken->subStem->strResult, DIC_STEM, INSERT_RIGHT_TO_LEFT);
							}
							//INSERT_LEFT_TO_RIGHT 맨뒤에 추가
							//InsertAddCompoundSubTail(pNode, strSource, DIC_STEM, INSERT_LEFT_TO_RIGHT);
						}
						nArrayLen = strlen(pNode->pHToken->strSource) + 1;
						pAddStemResultWords = new char[nArrayLen];
						memset(pAddStemResultWords, 0x00, nArrayLen);
						pStartNode = pNode->pHToken->pSubHeadNode->pNext;
						while (pStartNode != pNode->pHToken->pSubTailNode)
						{
							strcat(pAddStemResultWords, pStartNode->pCSubToken->strResult);
							pStartNode = pStartNode->pNext;
						}
						//미등록어전부를 스템의 결과값을 변경한다.
						if (pAddStemResultWords != NULL)
						{
							CHAR_DELETE(pNode->pHToken->subStem->strResult);
							CHAR_DELETE(pNode->pHToken->subStem->strFeature);
							nArrayLen = strlen(pAddStemResultWords) + 1;
							pNode->pHToken->subStem->strResult = new char[nArrayLen];
							memset(pNode->pHToken->subStem->strResult, 0x00, nArrayLen);
							strcpy(pNode->pHToken->subStem->strResult, pAddStemResultWords);
							pNode->pHToken->subStem->strFeature = new char[STEM_DIC_ALL_FEATURE_NUMBER + 1];
							memset(pNode->pHToken->subStem->strFeature, 0x00, sizeof(pNode->pHToken->subStem->strFeature));
							for (int i = 0; i < STEM_DIC_ALL_FEATURE_NUMBER + 1; i++) {
								pNode->pHToken->subStem->strFeature[i] = '0';
							}
							pNode->pHToken->subStem->strFeature[STEM_DIC_N] = '1';//N명사자질부여
							bOk = true;
						}
					}
				}
			}
		}
	}

	if(bOk == false)
	{
		//분석 실패시(A,B 미 존재시 처리) 
		if (pNode->pHToken->subStem->strResult == NULL && pNode->pHToken->subEnding->strResult == NULL)
		{
			nStartPosition = 0;
			nEndPosition = strlen(pNode->pHToken->strSource) - 1;
		}
		else if (pNode->pHToken->subStem->strResult == NULL && pNode->pHToken->subEnding->strResult != NULL && (strlen(pNode->pHToken->strSource) > strlen(pNode->pHToken->subEnding->strResult)))
		{
			nStartPosition = 0;
			nEndPosition = strlen(pNode->pHToken->strSource) - strlen(pNode->pHToken->subEnding->strResult) - 1;
		}
		else if (pNode->pHToken->subStem->strResult != NULL && pNode->pHToken->subEnding->strResult == NULL && (strlen(pNode->pHToken->strSource) > strlen(pNode->pHToken->subStem->strResult)))
		{
			nStartPosition = strlen(pNode->pHToken->subStem->strResult);

			if(nCompoundLen > 0)
				nEndPosition = strlen(pNode->pHToken->strSource) - 1;
			else
				nEndPosition = strlen(pNode->pHToken->strSource) - 1;
		}
		
		if (nEndPosition >= 0)
		{
			if (nCompoundLen > 0)
			{
				nEndPosition = nEndPosition - nCompoundLen;
			}
			CHAR_DELETE(strSource);
			strSource = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, nStartPosition, nEndPosition);
			if (strSource != NULL && strcmp(strSource, "") != 0)
			{
				if(pNode->pHToken->subStem->strResult != NULL)
				{
					if (pNode->pHToken->bSubCompCreate == false)
					{
						m_LinkedList->CreateCompoundSubNode(pNode, "STEM");
						if (pNode->pHToken->subStem->strResult != NULL)
						{
							//INSERT_RIGHT_TO_LEFT 맨앞에 추가
							InsertAddCompoundSubTail(pNode, strSource, DIC_STEM, INSERT_RIGHT_TO_LEFT);
							InsertAddCompoundSubTail(pNode, pNode->pHToken->subStem->strResult, DIC_STEM, INSERT_RIGHT_TO_LEFT);
						}
						else
						{
							InsertAddCompoundSubTail(pNode, strSource, DIC_STEM, INSERT_RIGHT_TO_LEFT);
						}
					}
					else
					{
						//INSERT_RIGHT_TO_LEFT 맨앞에 추가
						InsertAddCompoundSubTail(pNode, strSource, DIC_STEM, INSERT_RIGHT_TO_LEFT);
						//복합명사 모듈에서 중간에 실패시 A와 미분석된 음절에 대한 처리를 하지 않으므로 해당 조건문에서 처리한다.
						if (pNode->pHToken->subStem->strResult != NULL)
						{
							//INSERT_RIGHT_TO_LEFT 맨앞에 추가
							InsertAddCompoundSubTail(pNode, pNode->pHToken->subStem->strResult, DIC_STEM, INSERT_RIGHT_TO_LEFT);
						}
					}
				}
				else
				{
					if (pNode->pHToken->bSubCompCreate == true)
					{
						//INSERT_RIGHT_TO_LEFT 맨앞에 추가
						InsertAddCompoundSubTail(pNode, strSource, DIC_STEM, INSERT_RIGHT_TO_LEFT);
					}
				}
				if (pNode->pHToken->bSubCompCreate == true)
				{
					nArrayLen = strlen(pNode->pHToken->strSource) + 1;
					pAddStemResultWords = new char[nArrayLen];
					memset(pAddStemResultWords, 0x00, nArrayLen);
					pStartNode = pNode->pHToken->pSubHeadNode->pNext;
					while (pStartNode != pNode->pHToken->pSubTailNode)
					{
						strcat(pAddStemResultWords, pStartNode->pCSubToken->strResult);
						pStartNode = pStartNode->pNext;
					}
					if(strcmp(pAddStemResultWords,"") == 0)
					{
						strcat(pAddStemResultWords, strSource);
					}
					//미등록어전부를 스템의 결과값을 변경한다.
					if (pAddStemResultWords != NULL)
					{
						CHAR_DELETE(pNode->pHToken->subStem->strResult);
						CHAR_DELETE(pNode->pHToken->subStem->strFeature);
						nArrayLen = strlen(pAddStemResultWords) + 1;
						pNode->pHToken->subStem->strResult = new char[nArrayLen];
						memset(pNode->pHToken->subStem->strResult, 0x00, nArrayLen);
						strcpy(pNode->pHToken->subStem->strResult, pAddStemResultWords);
						pNode->pHToken->subStem->strFeature = new char[STEM_DIC_ALL_FEATURE_NUMBER + 1];
						memset(pNode->pHToken->subStem->strFeature, 0x00, sizeof(pNode->pHToken->subStem->strFeature));
						for (int i = 0; i < STEM_DIC_ALL_FEATURE_NUMBER + 1; i++) {
							pNode->pHToken->subStem->strFeature[i] = '0';
						}
						pNode->pHToken->subStem->strFeature[STEM_DIC_N] = '1';//N명사자질부여
					}
				}
				else
				{
					CHAR_DELETE(pNode->pHToken->subStem->strResult);
					CHAR_DELETE(pNode->pHToken->subStem->strFeature);
					nArrayLen = strlen(strSource) + 1;
					pNode->pHToken->subStem->strResult = new char[nArrayLen];
					memset(pNode->pHToken->subStem->strResult, 0x00, nArrayLen);
					strcpy(pNode->pHToken->subStem->strResult, strSource);
					pNode->pHToken->subStem->strFeature = new char[STEM_DIC_ALL_FEATURE_NUMBER + 1];
					memset(pNode->pHToken->subStem->strFeature, 0x00, sizeof(pNode->pHToken->subStem->strFeature));
					for (int i = 0; i < STEM_DIC_ALL_FEATURE_NUMBER + 1; i++) {
						pNode->pHToken->subStem->strFeature[i] = '0';
					}
					pNode->pHToken->subStem->strFeature[STEM_DIC_N] = '1';//N명사자질부여
				}
			}
		}
	}
	CHAR_DELETE(strSource);
	CHAR_DELETE(pStartToEndChar);
	CHAR_DELETE(pAddTwoCandidateStemSyllableWord);
	CHAR_DELETE(pAddStemResultWords);
}

void CMorphoLogicalAnalysis::AfterModule_03(H_NODE* pNode)
{
}

void CMorphoLogicalAnalysis::AfterModule_04(H_NODE* pNode)
{
}

void CMorphoLogicalAnalysis::AfterModule_05(H_NODE* pNode)
{
}












//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
//Linked List에 SubTail 추가(복합어간 또는 복합어미)
//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
void CMorphoLogicalAnalysis::InsertAddCompoundSubTail(H_NODE *pNode, char *pString, int nFlag, int nInsertTyep)
{
	if (pString == NULL) return;

	H_SUBNODE* pInsertNode;
	pInsertNode = new H_SUBNODE;
	bool bFind = false;
	int nSize = 0;
	char Feature_Stem[STEM_DIC_ALL_FEATURE_NUMBER + 1];
	memset(Feature_Stem, '0', sizeof(Feature_Stem));
	char IS_CEHCK_FEATURE = '1';
	if (pInsertNode != NULL)
	{
		pInsertNode->pCSubToken = new COMP_SUB_TOKEN;
		if (pInsertNode->pCSubToken != NULL)
		{
			pInsertNode->pCSubToken->strResult = NULL;
			pInsertNode->pCSubToken->strFeature = NULL;
			if (nFlag == DIC_STEM)
			{
				//ClassfyKindChar(pInsertNode, pString);
				bFind = IsExistStemDic(pInsertNode, pString);
				if (bFind == false)
				{
					CHAR_DELETE(pInsertNode->pCSubToken->strResult);
					CHAR_DELETE(pInsertNode->pCSubToken->strFeature);
					int nLen = strlen(pString);
					pInsertNode->pCSubToken->strResult = new char[nLen + 1];
					memset(pInsertNode->pCSubToken->strResult, 0x00, nLen + 1);
					strcpy(pInsertNode->pCSubToken->strResult, pString);
					pInsertNode->pCSubToken->strFeature = new char[STEM_DIC_ALL_FEATURE_NUMBER + 1];
					memset(pInsertNode->pCSubToken->strFeature, 0x00, sizeof(pInsertNode->pCSubToken->strFeature));
					for (int i = 0; i < STEM_DIC_ALL_FEATURE_NUMBER + 1; i++) {
						pInsertNode->pCSubToken->strFeature[i] = '0';
					}
					pInsertNode->pCSubToken->strFeature[STEM_DIC_N] = '1';//N명사자질부여
				}
				//복합명사 저장 방향(추가할 노드를 리스트의 맨 앞에 저장)
				if (nInsertTyep == INSERT_RIGHT_TO_LEFT)
				{
					pInsertNode->pPrev = pNode->pHToken->pSubHeadNode;
					pInsertNode->pNext = pNode->pHToken->pSubHeadNode->pNext;
					if (pNode->pHToken->pSubHeadNode->pNext == pNode->pHToken->pSubTailNode)
					{
						pNode->pHToken->pSubTailNode->pPrev = pInsertNode;
					}
					pNode->pHToken->pSubHeadNode->pNext = pInsertNode;
				}
				//복합명사 저장 방향(추가할 노드를 리스트의 맨 뒤에 저장)
				else if (nInsertTyep == INSERT_LEFT_TO_RIGHT)
				{
					pInsertNode->pPrev = pNode->pHToken->pSubTailNode->pPrev;
					pInsertNode->pNext = pNode->pHToken->pSubTailNode;
					pNode->pHToken->pSubTailNode->pPrev->pNext = pInsertNode;
					pNode->pHToken->pSubTailNode->pPrev = pInsertNode;
				}
			}
			else if (nFlag == DIC_ENDING)
			{
				bFind = IsExistEndingDic(pInsertNode, pString);
				if (bFind == false)
				{
					CHAR_DELETE(pInsertNode->pCSubToken->strResult);
					CHAR_DELETE(pInsertNode->pCSubToken->strFeature);
					int nLen = strlen(pString);
					pInsertNode->pCSubToken->strResult = new char[nLen + 1];
					memset(pInsertNode->pCSubToken->strResult, 0x00, nLen + 1);
					strcpy(pInsertNode->pCSubToken->strResult, pString);
					pInsertNode->pCSubToken->strFeature = new char[ENDING_DIC_BASIC_FEATURE_NUMBER + 1];
					memset(pInsertNode->pCSubToken->strFeature, 0x00, sizeof(pInsertNode->pCSubToken->strFeature));
					for (int i = 0; i < ENDING_DIC_BASIC_FEATURE_NUMBER + 1; i++) {
						pInsertNode->pCSubToken->strFeature[i] = '0';
					}
					pInsertNode->pCSubToken->strFeature[ENDING_DIC_N] = '1';//N 조사자질부여
				}
				//복합명사 저장 방향(추가할 노드를 리스트의 맨 앞에 저장)
				if(nInsertTyep == INSERT_RIGHT_TO_LEFT)
				{
					pInsertNode->pPrev = pNode->pHToken->pSubEndingHeadNode;
					pInsertNode->pNext = pNode->pHToken->pSubEndingHeadNode->pNext;
					if (pNode->pHToken->pSubEndingHeadNode->pNext == pNode->pHToken->pSubEndingTailNode)
					{
						pNode->pHToken->pSubEndingTailNode->pPrev = pInsertNode;
					}
					pNode->pHToken->pSubEndingHeadNode->pNext = pInsertNode;
				}
				//복합명사 저장 방향(추가할 노드를 리스트의 맨 뒤에 저장)
				else if (nInsertTyep == INSERT_LEFT_TO_RIGHT)
				{
					pInsertNode->pPrev = pNode->pHToken->pSubEndingTailNode->pPrev;
					pInsertNode->pNext = pNode->pHToken->pSubEndingTailNode;
					pNode->pHToken->pSubEndingTailNode->pPrev->pNext = pInsertNode;
					pNode->pHToken->pSubEndingTailNode->pPrev = pInsertNode;
				}
			}
		}
	}
}



//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
//복합조사 추가
//엔딩사전의 2,3,4음절을 대상으로 찾는다.
//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
void CMorphoLogicalAnalysis::CompoundPostPositionDecomposition(H_NODE *pNode, int nStartPosition, int nEndPosition, int nCompoundType)
{
	H_SUBNODE* pStartNode = NULL;
	char* pAddEndingResultWords = NULL;

	bool bCompoundEndSyllable = false;
	int nLength = 0;
	int i = 0;
	int nPos = 0;
	char* strSource = NULL;
	
	char* pWord1 = NULL;
	char* pWord2 = NULL;
	char* pWord3 = NULL;
	char* pWord4 = NULL;
	
	bool bFind2 = false;
	bool bFind3 = false;
	bool bFind4 = false;
	
	int nFindPos2 = -1;
	int nFindPos3 = -1;
	int nFindPos4 = -1;
	
	bool isExist = false;
	char Feature_Stem[STEM_DIC_ALL_FEATURE_NUMBER + 1];
	char IS_CEHCK_FEATURE = '1';
	memset(Feature_Stem, 0x00, sizeof(Feature_Stem));

	//현엔딩사전구성정보 : 1음절:123 2음절:590 3음절:1034 4음절:918 5음절:512 6음절:208 7음절:35 8음절:8 9음절:2
	//2, 3, 4음절이 대부분의 사전을 차지함으로 복합조사 미등록어 분석 기준을 2,3,4음절만 대상으로 한다.
	if (nEndPosition < 0) return;

	//그외 우->좌 최장일치 형태로 복합명사를 분리한다.
	//신규사업부터는 A:신규 B:는 --> 신규사업 + 부터는으로 분석
	

	strSource = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, nStartPosition, nEndPosition);
	if (strSource != NULL)
	{
		nEndPosition = strlen(strSource) - 1;
	}
	//우->좌 최장일치의 어간 사전 확인
	//nStartPosition는 이미 정해져 있음. 해당 함수를 호출하는 함수에서 이미 start와end가 정해짐
	for (i = nEndPosition; i >= 0; i--)
	{
		CHAR_DELETE(pWord1);
		CHAR_DELETE(pWord2);
		CHAR_DELETE(pWord3);
		CHAR_DELETE(pWord4);
		
		bFind2 = false; bFind3 = false; bFind4 = false;
		nEndPosition = i;
		nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(strSource, nEndPosition, DIC_ENDING);
		if (nPos >= 0)
		{
			pWord1 = m_pCNAHangulUtil->GetStartToEndWord(strSource, nPos, nEndPosition);
			if (nPos == 0)nPos = -1;
		}

		if (nPos >= 0 && pWord1 != NULL && strlen(pWord1) == 2) nPos = nPos - 1;
		if (nPos >= 0)
		{
			nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(strSource, nPos, DIC_ENDING);
			pWord2 = m_pCNAHangulUtil->GetStartToEndWord(strSource, nPos, nEndPosition);
			if (pWord2 != NULL && strcmp(pWord2, "") != 0) bFind2 = IsExistEndingDic(pWord2, FIND_NOT_REVERSE);
			if (nPos == 0)nPos = -1;
		}

		if (nPos >= 0 && pWord2 != NULL && strlen(pWord2) == 4) nPos = nPos - 1;
		if (nPos >= 0)
		{
			nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(strSource, nPos, DIC_ENDING);
			pWord3 = m_pCNAHangulUtil->GetStartToEndWord(strSource, nPos, nEndPosition);
			if (pWord3 != NULL && strcmp(pWord3, "") != 0) bFind3 = IsExistEndingDic(pWord3, FIND_NOT_REVERSE);
			if (nPos == 0)nPos = -1;
		}

		if (nPos >= 0 && pWord3 != NULL && strlen(pWord3) == 6) nPos = nPos - 1;
		if (nPos >= 0)
		{
			nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(strSource, nPos, DIC_ENDING);
			pWord4 = m_pCNAHangulUtil->GetStartToEndWord(strSource, nPos, nEndPosition);
			if (pWord4 != NULL && strcmp(pWord4, "") != 0) bFind4 = IsExistEndingDic(pWord4, FIND_NOT_REVERSE);
			if (nPos == 0)nPos = -1;
		}

		if (bFind4 == true || bFind3 == true || bFind2 == true)
		{
			if (pNode->pHToken->bSubEndingCompCreate == false)
			{
				m_LinkedList->CreateCompoundSubNode(pNode, "ENDING");
				if (pNode->pHToken->subEnding->strResult != NULL)
				{
					InsertAddCompoundSubTail(pNode, pNode->pHToken->subEnding->strResult, DIC_ENDING, INSERT_RIGHT_TO_LEFT);//복합어미정보 
				}
			}
			
			if (bFind4 == true)
			{
				InsertAddCompoundSubTail(pNode, pWord4, DIC_ENDING, INSERT_RIGHT_TO_LEFT);//복합어미정보 
				i -= (strlen(pWord4) - 1);
				pNode->pHToken->nLastFindEndingSyllable = i;//복합조사찾은위치
				if (bCompoundEndSyllable == false) bCompoundEndSyllable = true;
			}
			else if (bFind3 == true)
			{
				InsertAddCompoundSubTail(pNode, pWord3, DIC_ENDING, INSERT_RIGHT_TO_LEFT);//복합어미정보 
				i -= (strlen(pWord3) - 1);
				pNode->pHToken->nLastFindEndingSyllable = i;//복합조사찾은위치
				if (bCompoundEndSyllable == false) bCompoundEndSyllable = true;
			}
			else if (bFind2 == true)
			{
				InsertAddCompoundSubTail(pNode, pWord2, DIC_ENDING, INSERT_RIGHT_TO_LEFT);//복합어미정보 
				i -= (strlen(pWord2) - 1);
				pNode->pHToken->nLastFindEndingSyllable = i;//복합조사찾은위치
				if (bCompoundEndSyllable == false) bCompoundEndSyllable = true;
			}
		}
		if (bFind4 == false && bFind3 == false && bFind2 == false)
		{
			break;
		}
	}


	//복합어미분석인 경우 복합으로 분석된 모든 정보를 하나로 합쳐 subEnding->strResult에 다시 저장한다.
	if (bCompoundEndSyllable == true)
	{
		pAddEndingResultWords = new char[strlen(pNode->pHToken->strSource) + 1];
		memset(pAddEndingResultWords, 0x00, sizeof(pAddEndingResultWords));

		if (pNode->pHToken->pSubEndingHeadNode != NULL && pNode->pHToken->pSubEndingHeadNode->pNext != NULL)
		{
			pStartNode = pNode->pHToken->pSubEndingHeadNode->pNext;
			while (pStartNode != pNode->pHToken->pSubEndingTailNode)
			{
				if (pStartNode->pCSubToken != NULL && pStartNode->pCSubToken->strResult != NULL)
				{
					strcat(pAddEndingResultWords, pStartNode->pCSubToken->strResult);
				}
				pStartNode = pStartNode->pNext;
			}
			CHAR_DELETE(pNode->pHToken->subEnding->strResult);
			pNode->pHToken->subEnding->strResult = new char[strlen(pAddEndingResultWords) + 1];
			memset(pNode->pHToken->subEnding->strResult, 0x00, sizeof(pNode->pHToken->subEnding->strResult));
			strcpy(pNode->pHToken->subEnding->strResult, pAddEndingResultWords);
		}
	}

	
	CHAR_DELETE(pAddEndingResultWords);
	CHAR_DELETE(strSource);
	CHAR_DELETE(pWord1);
	CHAR_DELETE(pWord2);
	CHAR_DELETE(pWord3);
	CHAR_DELETE(pWord4);
}



//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
//복합어간정보 추가
//복합명사의 경우 좌->우 로직을 적용한다.
//도시어부들이 --> 도시어부+들이
//복합명사 분석 시, B를 포함한 복합명사를 찾을 때 해당 복합 명사와 기 분석된 A가 전체 어절일 때 B를 포함한 복합명사가 고유명사라면(Prp:고유명사체크) 
//B를 포함한 어절에서 B는 어미가 되어야 한다.(Prp:고유명사체크로직 추가 2018-01-11)
//테스트베드로 --> 테스트 + 로 --> 테스트 + 베드로(고유명사) : 고유명사가 마지막일 경우에는 앞에 어절이 올 수 없다. Prp:고유명사체크
//테스트베드로성당 --> 테스트 --> 테스트 + 베드로 + 성당 : 베드로는 마지막에 있지 않음으로 베드로로 분석되어야 한다.(고유명사가 마지막에 있지 않음) :
//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
void CMorphoLogicalAnalysis::CompoundNounDecomposition(H_NODE *pNode, int nStartPosition, int nEndPosition, int nCompoundType)
{
	bool bInsertCompoundNoun = false;
	bool bMaternalInclusionAnalysis = false;//어미가 1음절일 경우 어미포함해서 복합명사를 분해한다.
	int nLength = 0;
	int i = 0;
	int nPos = 0;
	char* strSource = NULL;
	char* pWord1 = NULL;
	char* pWord2 = NULL;
	char* pWord3 = NULL;
	char* pWord4 = NULL;
	char* pWord5 = NULL;
	bool bFind1 = false;
	bool bFind2 = false;
	bool bFind3 = false;
	bool bFind4 = false;
	bool bFind5 = false;
	int nFindPos1 = -1;
	int nFindPos2 = -1;
	int nFindPos3 = -1;
	int nFindPos4 = -1;
	int nFindPos5 = -1;

	bool isExist = false;
	char Feature_Stem[STEM_DIC_ALL_FEATURE_NUMBER + 1];
	char IS_CEHCK_FEATURE = '1';
	memset(Feature_Stem, 0x00, sizeof(Feature_Stem));
	
	//현사전구성정보 : 1음절 : 970 2음절 : 22, 720 3음절 : 29, 132 4음절 : 9, 941 5음절 : 2, 750 
	//2, 3, 4음절이 대부분의 사전을 차지함으로 복합명사 미등록어 분석 기준을 2,3,4음절만 대상으로 한다.
	if (nEndPosition < 0) return;

	
	//A+B가 전체 어절이면서, B가 2음절 이상일 경우에는 복합명사를 분석하지 않는다.	
	if (pNode->pHToken->subStem->strResult != NULL && pNode->pHToken->subEnding->strResult != NULL && strlen(pNode->pHToken->subEnding->strResult) > 2)
	{
		if (strlen(pNode->pHToken->strSource) == strlen(pNode->pHToken->subStem->strResult) + strlen(pNode->pHToken->subEnding->strResult))
			return;
	}
	
	
	//어미가 1음절 일 경우, 어미 전체를 포함하여 좌->우 최장일치 형태로 복합명사를 분리한다.
	//그외 우->좌 최장일치 형태로 복합명사를 분리한다.
	//법정소송이(B:이 송이(복합명사대상), 소송(복합명사대상)),방통위원장과(B:과 장과(복합명사대상),원장(복합명사대상)) 으로 연속된 스템 존재 시 B-1부터 분석한다)
	if (pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strResult != NULL && strlen(pNode->pHToken->subEnding->strResult) == 2)
	{
		bool bFind_1 = false;
		bool bFind_2 = false;
		char* pFromMinusOneToToBWord = NULL;//B-1+B
		char* pFromBMinuseTwoToBMinusOneWord = NULL;//B-2+B-1
		int nPos = 0;

		nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource) - strlen(pNode->pHToken->subEnding->strResult) -1, DIC_ENDING);
		if (nPos >= 0)
		{
			//B-1+B
			pFromMinusOneToToBWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, nPos, strlen(pNode->pHToken->strSource) - 1);
			if (pFromMinusOneToToBWord != NULL && strcmp(pFromMinusOneToToBWord, "") != 0)bFind_1 = IsExistStemDic(pFromMinusOneToToBWord);
			if (nPos == 0)nPos = -1;
		}
		if (nPos >= 0 && pFromMinusOneToToBWord != NULL && strlen(pFromMinusOneToToBWord) == 4) nPos = nPos - 1; 
		if (nPos >= 0)
		{
			//B-2+B-1
			nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, nPos, DIC_ENDING);
			pFromBMinuseTwoToBMinusOneWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, nPos, strlen(pNode->pHToken->strSource) - strlen(pNode->pHToken->subEnding->strResult) -1);
			if (pFromBMinuseTwoToBMinusOneWord != NULL && strcmp(pFromBMinuseTwoToBMinusOneWord, "") != 0) bFind_2 = IsExistStemDic(pFromBMinuseTwoToBMinusOneWord);
			if (nPos == 0)nPos = -1;
		}
		CHAR_DELETE(pFromMinusOneToToBWord);
		CHAR_DELETE(pFromBMinuseTwoToBMinusOneWord);

		if(bFind_1 == true && bFind_2 == true)
		{
			nEndPosition = strlen(pNode->pHToken->strSource) - strlen(pNode->pHToken->subEnding->strResult) - 1;
		}
		else
		{
			nEndPosition = strlen(pNode->pHToken->strSource) - 1;
		}
		bMaternalInclusionAnalysis = true;
	}

	strSource = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, nStartPosition, nEndPosition);
	if(strSource != NULL)
	{
		nEndPosition = strlen(strSource) - 1;
	}
	//우->좌 최장일치의 어간 사전 확인
	//nStartPosition는 이미 정해져 있음. 해당 함수를 호출하는 함수에서 이미 start와end가 정해짐
	for (i = nEndPosition; i >= 0; i--)
	{
		CHAR_DELETE(pWord1);
		CHAR_DELETE(pWord2);
		CHAR_DELETE(pWord3);
		CHAR_DELETE(pWord4);
		CHAR_DELETE(pWord5);
		bFind1 = false;bFind2 = false;bFind3 = false;bFind4 = false;bFind5 = false;
		nEndPosition = i;
		nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(strSource, nEndPosition, DIC_ENDING);
		if(nPos >= 0)
		{
			pWord1 = m_pCNAHangulUtil->GetStartToEndWord(strSource, nPos, nEndPosition);
			if(pWord1 != NULL && strcmp(pWord1,"") != 0)bFind1 = IsExistStemDic(pWord1);
			if(nPos == 0)nPos = -1;
		}
			
		if(nPos >= 0 && pWord1 != NULL && strlen(pWord1) == 2) nPos = nPos - 1;
		if (nPos >= 0)
		{
			nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(strSource, nPos, DIC_ENDING);
			pWord2 = m_pCNAHangulUtil->GetStartToEndWord(strSource, nPos, nEndPosition);
			if (pWord2 != NULL && strcmp(pWord2, "") != 0) bFind2 = IsExistStemDic(pWord2);
			if (nPos == 0)nPos = -1;
		}
			
		if (nPos >= 0 && pWord2 != NULL && strlen(pWord2) == 4) nPos = nPos - 1;
		if (nPos >= 0)
		{
			nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(strSource, nPos, DIC_ENDING);
			pWord3 = m_pCNAHangulUtil->GetStartToEndWord(strSource, nPos, nEndPosition);
			if (pWord3 != NULL && strcmp(pWord3, "") != 0) bFind3 = IsExistStemDic(pWord3);
			if (nPos == 0)nPos = -1;
		}
			
		if (nPos >= 0 && pWord3 != NULL && strlen(pWord3) == 6) nPos = nPos - 1;
		if (nPos >= 0)
		{
			nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(strSource, nPos, DIC_ENDING);
			pWord4 = m_pCNAHangulUtil->GetStartToEndWord(strSource, nPos, nEndPosition);
			if (pWord4 != NULL && strcmp(pWord4, "") != 0) bFind4 = IsExistStemDic(pWord4);
			if (nPos == 0)nPos = -1;
		}

		if (nPos >= 0 && pWord4 != NULL && strlen(pWord4) == 8) nPos = nPos - 1;
		if (nPos >= 0 && pWord4 != NULL) nPos = nPos - 1;
		if (nPos >= 0) {
			nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(strSource, nPos, DIC_ENDING);
			pWord5 = m_pCNAHangulUtil->GetStartToEndWord(strSource, nPos, nEndPosition);
			if (pWord5 != NULL && strcmp(pWord5, "") != 0) bFind5 = IsExistStemDic(pWord5);
			if (nPos == 0)nPos = -1;
		}
		if(bFind5 == true)
		{
			//미분석 된 정보와 기 분석된 A의 길이가 전체문자열과 같을 경우 미분석전체어절의 자질을 확인한다.
			if (pNode->pHToken->subStem != NULL && pNode->pHToken->subStem->strResult != NULL && (strlen(pNode->pHToken->strSource) == strlen(pNode->pHToken->subStem->strResult) + strlen(pWord5)))
			{
				m_pStemEndDic->StemGetRecord(pWord5, (char *)Feature_Stem);
				//미분석 음절이 고유명사 자질 일 경우 앞에 다른 어절이나 음절이 올 수 없다.
				if (Feature_Stem[STEM_DIC_N] == '1' && Feature_Stem[STEM_DIC_PRP] == '1')
				{
					if (bInsertCompoundNoun == false) bInsertCompoundNoun = true;
					break;
				}
			}
			InsertAddCompoundSubTail(pNode, pWord5, DIC_STEM, INSERT_RIGHT_TO_LEFT);
			i -= (strlen(pWord5) - 1);
			pNode->pHToken->nLastFindCompoundNoun = i;//복합명사찾은위치
			if (bInsertCompoundNoun == false) bInsertCompoundNoun = true;
		}
		else if (bFind4 == true)
		{
			//미분석 된 정보와 기 분석된 A의 길이가 전체문자열과 같을 경우 미분석전체어절의 자질을 확인한다.
			if (pNode->pHToken->subStem != NULL && pNode->pHToken->subStem->strResult != NULL && (strlen(pNode->pHToken->strSource) == strlen(pNode->pHToken->subStem->strResult) + strlen(pWord4)))
			{
				m_pStemEndDic->StemGetRecord(pWord4, (char *)Feature_Stem);
				//미분석 음절이 고유명사 자질 일 경우 앞에 다른 어절이나 음절이 올 수 없다.
				if (Feature_Stem[STEM_DIC_N] == '1' && Feature_Stem[STEM_DIC_PRP] == '1')
				{
					if (bInsertCompoundNoun == false) bInsertCompoundNoun = true;
					break;
				}
			}
			InsertAddCompoundSubTail(pNode, pWord4, DIC_STEM, INSERT_RIGHT_TO_LEFT);
			i -= (strlen(pWord4) - 1);
			pNode->pHToken->nLastFindCompoundNoun = i;//복합명사찾은위치
			if (bInsertCompoundNoun == false) bInsertCompoundNoun = true;
		}
		else if (bFind3 == true)
		{
			//미분석 된 정보와 기 분석된 A의 길이가 전체문자열과 같을 경우 미분석전체어절의 자질을 확인한다.
			if (pNode->pHToken->subStem != NULL && pNode->pHToken->subStem->strResult  != NULL && (strlen(pNode->pHToken->strSource) == strlen(pNode->pHToken->subStem->strResult) + strlen(pWord3)))
			{
				m_pStemEndDic->StemGetRecord(pWord3, (char *)Feature_Stem);
				//미분석 음절이 고유명사 자질 일 경우 앞에 다른 어절이나 음절이 올 수 없다.
				if (Feature_Stem[STEM_DIC_N] == '1' && Feature_Stem[STEM_DIC_PRP] == '1')
				{
					if (bInsertCompoundNoun == false) bInsertCompoundNoun = true;
					break;
				}
			}
			InsertAddCompoundSubTail(pNode, pWord3, DIC_STEM, INSERT_RIGHT_TO_LEFT);
			i -= (strlen(pWord3) - 1);
			pNode->pHToken->nLastFindCompoundNoun = i;//복합명사찾은위치
			if(bInsertCompoundNoun == false) bInsertCompoundNoun = true;
		}
		else if (bFind2 == true)
		{
			//미분석 된 정보와 기 분석된 A의 길이가 전체문자열과 같을 경우 미분석전체어절의 자질을 확인한다.
			if (pNode->pHToken->subStem != NULL && pNode->pHToken->subStem->strResult != NULL && (strlen(pNode->pHToken->strSource) == strlen(pNode->pHToken->subStem->strResult) + strlen(pWord2)))
			{
				m_pStemEndDic->StemGetRecord(pWord2, (char *)Feature_Stem);
				//미분석 음절이 고유명사 자질 일 경우 앞에 다른 어절이나 음절이 올 수 없다.
				if (Feature_Stem[STEM_DIC_N] == '1' && Feature_Stem[STEM_DIC_PRP] == '1')
				{
					if (bInsertCompoundNoun == false) bInsertCompoundNoun = true;
					break;
				}
			}
			InsertAddCompoundSubTail(pNode, pWord2, DIC_STEM, INSERT_RIGHT_TO_LEFT);
			i -= (strlen(pWord2) - 1);
			pNode->pHToken->nLastFindCompoundNoun = i;//복합명사찾은위치
			if (bInsertCompoundNoun == false) bInsertCompoundNoun = true;
		}
		else if (bFind1 == true){
			//첫 LOOP문에서 bFind5,bFind4,bFind3,bFind2가 스템 사전에 없을 시에는 B를 제외하고 다시 복합어를 분석한다.
			//1음절에 한해서만 스템에 있을 경우
			if (bMaternalInclusionAnalysis == true)
			{	
				//nCompoundType == 2 , A와B가 전체어절과 같을 경우 A+B == 전체어절
				//복합명사분석을 더이상 하지 않는다.
				if(nCompoundType == 2)
				{
					if (pNode->pHToken->bSubCompCreate == true)
					{
						DeleteSubHTokenLinkedList(pNode->pHToken, "STEM");
						pNode->pHToken->bSubCompCreate = false;
					}
					break;
				}
			}
			else
			{
				InsertAddCompoundSubTail(pNode, pWord1, DIC_STEM, INSERT_RIGHT_TO_LEFT);
				i -= (strlen(pWord1) - 1);
				pNode->pHToken->nLastFindCompoundNoun = i;//복합명사찾은위치
				if (bInsertCompoundNoun == false) bInsertCompoundNoun = true;
			}
		}
				
		//B가 1음절일 때 B를 포함하여 5,4,3,2음절의 어절 중 스템사전에 존재할 때 B를 삭제 처리한다.
		//스템사전에 찾은 정보가 N자질이 없을 경우 명사로 보지 않는다.

		if (bMaternalInclusionAnalysis == true)
		{
			if(bFind2 == true || bFind3 == true || bFind4 == true || bFind5 == true)
			{
				memset(Feature_Stem, 0x00, sizeof(Feature_Stem));
				if(bFind5 == true && pWord5 != NULL)
					isExist = m_pStemEndDic->StemGetRecord(pWord5, (char *)Feature_Stem);
				else if (bFind4 == true && pWord4 != NULL)
					isExist = m_pStemEndDic->StemGetRecord(pWord4, (char *)Feature_Stem);
				else if (bFind3 == true && pWord3 != NULL)
					isExist = m_pStemEndDic->StemGetRecord(pWord3, (char *)Feature_Stem);
				else if (bFind2 == true && pWord2 != NULL)
					isExist = m_pStemEndDic->StemGetRecord(pWord2, (char *)Feature_Stem);
				//어미를 포함한 정보가 스템사전에 존재하기 때문에 명사자질을 보지 않고 바로 어미정보를 삭제 처리한다.
				//if(Feature_Stem[STEM_DIC_N] == '1')
				if(isExist == true && pNode->pHToken->subEnding != NULL)
				{
					//어미B로 분석되 1음절을 삭제 처리한다.
					DeleteHTokenResult(pNode, DIC_ENDING);
				}
				else
				{
					//B를 포함해서 찾지 못한 경우.. A+1~B-1에  해당하는 복합명사를 분석한다.
					nEndPosition = pNode->pHToken->subEnding->nFindPos - 1;
					CHAR_DELETE(strSource);
					strSource = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, nStartPosition, nEndPosition);
					if (strSource != NULL)
					{
						//B포함 어절을 원상복구 후, 해당 음절과 A비교 같은 경우 더이상 분석하지 않는다.
						if (pNode->pHToken->subStem->strResult != NULL && strcmp(strSource, pNode->pHToken->subStem->strResult) == 0)
						{
							break;
						}
						nEndPosition = strlen(strSource);
						i = nEndPosition;
					}
				}
			}
			else
			{
				//B를 포함해서 찾지 못한 경우.. A+1~B-1에  해당하는 복합명사를 분석한다.
				nEndPosition = pNode->pHToken->subEnding->nFindPos - 1;
				CHAR_DELETE(strSource);
				strSource = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, nStartPosition, nEndPosition);
				if (strSource != NULL)
				{
					//B포함 어절을 원상복구 후, 해당 음절과 A비교 같은 경우 더이상 분석하지 않는다.
					if (pNode->pHToken->subStem->strResult != NULL && strcmp(strSource, pNode->pHToken->subStem->strResult) == 0)
					{
						break;
					}
					nEndPosition = strlen(strSource);
					i = nEndPosition;
				}
			}
			bMaternalInclusionAnalysis = false;
		}
		else
		{
			if (bFind5 == false && bFind4 == false && bFind3 == false && bFind2 == false && bFind1 == false)
			{
				bInsertCompoundNoun = false;
				break;
			}
		}
	}
	//우->좌 복합명사 분석
	if (bInsertCompoundNoun == true)
	{
		AfterModule_01(pNode);
	}
	else
	{
		//A,B사이에 복합명사 형태로 분석이 안된 경우
		//B-1의 어절의 마지막 음절의 종성처리모듈(ㄴ,ㄹ,ㅁ,ㅂ,ㅆ)을 다시 한번 확인한다.
		ExceptionModule_01(pNode);
	}
	
	CHAR_DELETE(strSource);
	CHAR_DELETE(pWord1);
	CHAR_DELETE(pWord2);
	CHAR_DELETE(pWord3);
	CHAR_DELETE(pWord4);
	CHAR_DELETE(pWord5);
}


bool CMorphoLogicalAnalysis::IsExistStemDic(H_SUBNODE* pNode, char *pStemWord)
{
	bool isExist = false;
	char Feature_Stem[STEM_DIC_ALL_FEATURE_NUMBER + 1];
	char IS_CEHCK_FEATURE = '1';
	memset(Feature_Stem, 0x00, sizeof(Feature_Stem));
	isExist = m_pStemEndDic->StemGetRecord(pStemWord, (char *)Feature_Stem);
	if (isExist == true)
	{
		CHAR_DELETE(pNode->pCSubToken->strResult);
		CHAR_DELETE(pNode->pCSubToken->strFeature);
		int nLen = strlen(pStemWord);
		pNode->pCSubToken->strResult = new char[nLen + 1];
		memset(pNode->pCSubToken->strResult, 0x00, nLen + 1);
		strcpy(pNode->pCSubToken->strResult, pStemWord);
		pNode->pCSubToken->strFeature = new char[STEM_DIC_ALL_FEATURE_NUMBER + 1];
		memset(pNode->pCSubToken->strFeature, 0x00, sizeof(pNode->pCSubToken->strFeature));
		strcpy(pNode->pCSubToken->strFeature, Feature_Stem);
	}
	return isExist;
}

//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
//복합어미정보 추가
//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
bool CMorphoLogicalAnalysis::IsExistEndingDic(H_SUBNODE* pNode, char *pEndingOneSyllable)
{
	bool isExist = false;
	char Feature_End[ENDING_DIC_BASIC_FEATURE_NUMBER + 1];
	char IS_CEHCK_FEATURE = '1';
	memset(Feature_End, 0x00, sizeof(Feature_End));
	isExist = m_pStemEndDic->EndGetRecord(pEndingOneSyllable, (char *)Feature_End);
	if (isExist == true)
	{
		CHAR_DELETE(pNode->pCSubToken->strResult);
		CHAR_DELETE(pNode->pCSubToken->strFeature);
		int nLen = strlen(pEndingOneSyllable);
		pNode->pCSubToken->strResult = new char[nLen + 1];
		memset(pNode->pCSubToken->strResult, 0x00, nLen + 1);
		strcpy(pNode->pCSubToken->strResult, pEndingOneSyllable);
		pNode->pCSubToken->strFeature = new char[ENDING_DIC_BASIC_FEATURE_NUMBER + 1];
		memset(pNode->pCSubToken->strFeature, 0x00, sizeof(pNode->pCSubToken->strFeature));
		strcpy(pNode->pCSubToken->strFeature, Feature_End);
	}
	return isExist;
}



//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
//해당노드의 정보를 모두 초기화 한다	
//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
void CMorphoLogicalAnalysis::AnalyzeNodeInitialization(H_NODE *pNode)
{
	if (pNode != NULL)
	{
		if (pNode->pHToken->bSubCompCreate == true)
		{
			DeleteSubHTokenLinkedList(pNode->pHToken, "STEM");
			pNode->pHToken->bSubCompCreate = false;
		}
		//복합어미 삭제
		if (pNode->pHToken->bSubEndingCompCreate == true)
		{
			DeleteSubHTokenLinkedList(pNode->pHToken, "ENDING");
			pNode->pHToken->bSubEndingCompCreate = false;
		}
		if(pNode->pHToken->subStem->strResult != NULL)
		{
			CHAR_DELETE(pNode->pHToken->subStem->strResult);
			CHAR_DELETE(pNode->pHToken->subStem->strFeature);
		}
		if(pNode->pHToken->subEnding->strResult != NULL)
		{
			CHAR_DELETE(pNode->pHToken->subEnding->strResult);
			CHAR_DELETE(pNode->pHToken->subEnding->strFeature);
		}
	}
}


void CMorphoLogicalAnalysis::DeleteSubHTokenLinkedList(H_TOKEN *pDelHSubToken, char* strHFlag)
{
	H_SUBNODE* pStartNode;
	H_SUBNODE* pDeleteNode;
	//복합스템삭제
	if (strcmp(strHFlag, "STEM") == 0)
	{
		pStartNode = pDelHSubToken->pSubHeadNode->pNext;
		while (pStartNode != pDelHSubToken->pSubTailNode)
		{
			pDeleteNode = pStartNode;
			pStartNode = pStartNode->pNext;

			if (pDeleteNode->pCSubToken != NULL)
			{
				if (pDeleteNode->pCSubToken->strResult != NULL)
				{
					delete[]pDeleteNode->pCSubToken->strResult;
					pDeleteNode->pCSubToken->strResult = NULL;
				}
				if (pDeleteNode->pCSubToken->strFeature != NULL)
				{
					delete[]pDeleteNode->pCSubToken->strFeature;
					pDeleteNode->pCSubToken->strFeature = NULL;
				}
				if (pDeleteNode->pCSubToken != NULL)
				{
					delete[]pDeleteNode->pCSubToken;
					pDeleteNode->pCSubToken = NULL;
				}
			}
			if (pDeleteNode != NULL)
			{
				delete pDeleteNode;
				pDeleteNode = NULL;
			}
			if (pStartNode == pStartNode->pNext)
				break;
		}
		if (pDelHSubToken->pSubHeadNode->pNext != NULL)
			pDelHSubToken->pSubHeadNode->pNext = NULL;//link free
		if (pDelHSubToken->pSubTailNode->pPrev != NULL)
			pDelHSubToken->pSubTailNode->pPrev = NULL; //link free


		if (pDelHSubToken->pSubHeadNode != NULL)
		{
			delete pDelHSubToken->pSubHeadNode;
			pDelHSubToken->pSubHeadNode = NULL;
		}

		if (pDelHSubToken->pSubTailNode != NULL)
		{
			delete pDelHSubToken->pSubTailNode;
			pDelHSubToken->pSubTailNode = NULL;
		}
	}
	//복합엔딩삭제
	else if (strcmp(strHFlag, "ENDING") == 0)
	{
		pStartNode = pDelHSubToken->pSubEndingHeadNode->pNext;
		while (pStartNode != pDelHSubToken->pSubEndingTailNode)
		{
			pDeleteNode = pStartNode;
			pStartNode = pStartNode->pNext;

			if (pDeleteNode->pCSubToken != NULL)
			{
				if (pDeleteNode->pCSubToken->strResult != NULL)
				{
					delete[]pDeleteNode->pCSubToken->strResult;
					pDeleteNode->pCSubToken->strResult = NULL;
				}
				if (pDeleteNode->pCSubToken->strFeature != NULL)
				{
					delete[]pDeleteNode->pCSubToken->strFeature;
					pDeleteNode->pCSubToken->strFeature = NULL;
				}
				if (pDeleteNode->pCSubToken != NULL)
				{
					delete[]pDeleteNode->pCSubToken;
					pDeleteNode->pCSubToken = NULL;
				}
			}
			if (pDeleteNode != NULL)
			{
				delete pDeleteNode;
				pDeleteNode = NULL;
			}
			if (pStartNode == pStartNode->pNext)
				break;
		}
		if (pDelHSubToken->pSubEndingHeadNode->pNext != NULL)
			pDelHSubToken->pSubEndingHeadNode->pNext = NULL;//link free
		if (pDelHSubToken->pSubEndingTailNode->pPrev != NULL)
			pDelHSubToken->pSubEndingTailNode->pPrev = NULL; //link free


		if (pDelHSubToken->pSubEndingHeadNode != NULL)
		{
			delete pDelHSubToken->pSubEndingHeadNode;
			pDelHSubToken->pSubEndingHeadNode = NULL;
		}

		if (pDelHSubToken->pSubEndingTailNode != NULL)
		{
			delete pDelHSubToken->pSubEndingTailNode;
			pDelHSubToken->pSubEndingTailNode = NULL;
		}
	}
}


void CMorphoLogicalAnalysis::DeleteHTokenResult(H_NODE* pNode, int nDeleteKind)
{
	if (nDeleteKind == DIC_STEM)
	{
		CHAR_DELETE(pNode->pHToken->subStem->strResult);
		CHAR_DELETE(pNode->pHToken->subStem->strFeature);
	}
	else if (nDeleteKind == DIC_ENDING)
	{
		CHAR_DELETE(pNode->pHToken->subEnding->strResult);
		CHAR_DELETE(pNode->pHToken->subEnding->strFeature);
	}

}

//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
//중의성 처리
//♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣♣
bool CMorphoLogicalAnalysis::CreateAmbiScript()//중의성분석
{
	bool bCreate = false;
	H_NODE* pNode;
	pNode = m_LinkedList->m_pHeadNode->pNext;
	m_nListPosition = m_LinkedList->m_nTotalCount;
	bool isStemAmbCheck = false;
	bool isEndingAmbCheck = false;
	while (m_nListPosition > 0)
	{
		isStemAmbCheck = false;
		isEndingAmbCheck = false;
		if (pNode->nSkip == 0 && pNode->pHToken != NULL)
		{
			if (pNode->pHToken->subStem != NULL && pNode->pHToken->subStem->strFeature != NULL)
			{
				if (pNode->pHToken->subStem->strFeature[1] == '1')
					isStemAmbCheck = true;
				else
					isStemAmbCheck = false;
			}
			if (pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strFeature != NULL)
			{
				if (pNode->pHToken->subEnding->strFeature[0] == '1')
					isEndingAmbCheck = true;
				else
					isEndingAmbCheck = false;
			}
			if (isStemAmbCheck == true && isEndingAmbCheck == true)
			{
				m_pAmbiguity->GoAmbiguityFunction(this, pNode, 3);

				//임시적으로 주석 처리함.(오류처리를 복원하는 작업인 듯...)
				//RestorIrrigual(m_pNode);
				pNode = pNode->pNext;
				m_nListPosition -= 1;
			}
			else if (isStemAmbCheck == true && isEndingAmbCheck == false)
			{
				m_pAmbiguity->GoAmbiguityFunction(this, pNode, 2);
				//임시적으로 주석 처리함.(오류처리를 복원하는 작업인 듯...)
				//RestorIrrigual(m_pNode);
				pNode = pNode->pNext;
				m_nListPosition -= 1;
			}
			else if (isStemAmbCheck == false && isEndingAmbCheck == true)
			{
				m_pAmbiguity->GoAmbiguityFunction(this, pNode, 1);

				//임시적으로 주석 처리함.(오류처리를 복원하는 작업인 듯...)
				//RestorIrrigual(m_pNode);
				pNode = pNode->pNext;
				m_nListPosition -= 1;
			}
			else
			{
				//임시적으로 주석 처리함.(오류처리를 복원하는 작업인 듯...)
				//RestorIrrigual(m_pNode);
				pNode = pNode->pNext;
				m_nListPosition -= 1;
			}
		}
		else
		{
			pNode = pNode->pNext;
			m_nListPosition -= 1;
		}
	}
	m_nListPosition = 0;
	return bCreate;
}




void CMorphoLogicalAnalysis::SaveLogs(H_NODE *pNode, char* pCallFunc)
{
}