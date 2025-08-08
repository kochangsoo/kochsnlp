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

//�ѱۺм��� ����
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
			//�ѱ��� ��� ���ڿ��� 2Byte�̹Ƿ� 10���� ���ϸ� �м��Ѵ�.
			//---------------------------------------------------------------------------------------------------------------------------
			if (m_pCNAHangulUtil->GetFirstToEndSyllableCount(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource) - 1) <= 20)
			{
				#ifdef _DEBUG
					memset(pNode->pHToken->strFlowChartDebug, 0, 1024);
					memset(pNode->pHToken->strAnalysisError, 0, 1024);
				#endif

				//��ü���� ����� Ž�� ���
				if (AnalysisModule_00(pNode))
				{
					pNode->pHToken->nKindError = 0;
				}
				else
				{
					pNode->pHToken->nKindError = -999;
					//������ġ(� �� ��� �м� �ĺ� ����) Ž�� ���
					AnalysisModule_01(pNode);

					//���ո�����
					ExceptionModule_04(pNode);
					//A,B������ �м����� �ʴ� ���� ó��
					AfterModule_02(pNode);
				}
			}
			else
			{
				//�м�����
				//�м���� ������ 10���� �̻�
				pNode->pHToken->nKindError = -999;
			}
		}
		m_AnalyzeAgainFromTheBeginning = false;
		m_ApplyIrregularAlgorithm = false;
		m_AnalyzeWhatCallFunction = "";
		pNode = pNode->pNext;
	}

	// ������ �м�
	CreateAmbiScript();
	
}



//��ü���� ����� Ž�� ���
bool CMorphoLogicalAnalysis::AnalysisModule_00(H_NODE* pNode)
{
	return IsExistStemDic(pNode, pNode->pHToken->strSource);
}

//������ġ(� �� ��� �м� �ĺ� ����) Ž�� ���
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


	//�� �Լ����� ������ġ�Լ��� �� ȣ�� ��, ����(�,���) ������ �ʱ�ȭ �Ѵ�.
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
		//���������� B�� ���� ��� B�� ��ü������ ����
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
				//'��������' ����м� ��� : B�� �ĺ��� : ��,���� A�� �ĺ���: ��, ������
				//B�� ����� A�� ���� �� ���� ��ĥ��� B�� ������ġ�� �ٲ�� �Ѵ�.
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

				//A+B�� LEN(STR) ���� ū ���
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

	//�,��̰� ��� �����ϴ� ���
	if (pNode->pHToken->subStem->strResult != NULL && pNode->pHToken->subEnding->strResult != NULL)
	{
		//(������ġ�� �ٽ� ȣ�� �ÿ� ó���Լ��� �ٸ� �Լ��� ȣ���� ��� ó��)
		if (strcmp(m_AnalyzeWhatCallFunction, "") != 0 && strcmp(m_AnalyzeWhatCallFunction, "AB") != 0)
		{
			m_AnalyzeAgainFromTheBeginning = false;
		}
		m_AnalyzeWhatCallFunction = "AB";
		OfTheFourLogic_AB(pNode);
	}
	//��� �����ϴ� ���
	else if (pNode->pHToken->subStem->strResult != NULL && pNode->pHToken->subEnding->strResult == NULL)
	{
		//(������ġ�� �ٽ� ȣ�� �ÿ� ó���Լ��� �ٸ� �Լ��� ȣ���� ��� ó��)
		if (strcmp(m_AnalyzeWhatCallFunction, "") != 0 && strcmp(m_AnalyzeWhatCallFunction, "AX") != 0)
		{
			m_AnalyzeAgainFromTheBeginning = false;
		}
		m_AnalyzeWhatCallFunction = "AX";
		OfTheFourLogic_AX(pNode);
	}
	//��̸� �����ϴ� ���(�ұ�Ģ�� ���� ó��)
	else if (pNode->pHToken->subStem->strResult == NULL && pNode->pHToken->subEnding->strResult != NULL)
	{
		//(������ġ�� �ٽ� ȣ�� �ÿ� ó���Լ��� �ٸ� �Լ��� ȣ���� ��� ó��)
		if (strcmp(m_AnalyzeWhatCallFunction, "") != 0 && strcmp(m_AnalyzeWhatCallFunction, "XB") != 0)
		{
			m_AnalyzeAgainFromTheBeginning = false;
		}
		m_AnalyzeWhatCallFunction = "XB";
		OfTheFourLogic_XB(pNode);
	}
	else
	{
		//(������ġ�� �ٽ� ȣ�� �ÿ� ó���Լ��� �ٸ� �Լ��� ȣ���� ��� ó��)
		if (strcmp(m_AnalyzeWhatCallFunction, "") != 0 && strcmp(m_AnalyzeWhatCallFunction, "FF") != 0)
		{
			m_AnalyzeAgainFromTheBeginning = false;
		}
		//�м�����
		m_AnalyzeWhatCallFunction = "FF";
		OfTheFourLogic_FF(pNode);
	}
	
	return bOK;
}



//������ġ�� ����� ���� 4���� Ÿ�� �߻�
//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
	//A,B ��� �м� ����
	//��)������, ê��, �Թ���, ������
	//������ ������ ������ ���� ó�� : (��, ��, ��, ��, ��)
	//������ ������ ���� ó�� : (��, ��, ��, ��, ��, ��, ��, ��, ���ұ�Ģó��) : 0 - B - 1(���߼�), B - 1�� ����~B � �� ��� ���� Ȯ��
	//������ ������ ���� '��' Ȯ�� �� ó��
	
	//��ü������ �� ������  1��� ���� ��)��
	//��ü���� 4��� ���� ��)��������
	//��ü���� 5��� ����
	//��� ���н� �̵�Ͼ�� ����.
//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
void CMorphoLogicalAnalysis::OfTheFourLogic_FF(H_NODE* pNode)
{
	//�м� ������ ������ ������ ������ ��ġ
	int nLen = strlen(pNode->pHToken->strSource) -1;

	//(0���) : ������ ������ ��������ó�����(��->�ϰ�)
	if (m_AnalyzeAgainFromTheBeginning == false)ExceptionModule_00(pNode);
	//(1���) : ������ ������ ������ ����ó�����(��,��,��,��,��)
	if (m_AnalyzeAgainFromTheBeginning == false)ExceptionModule_01(pNode);
	//(3���) : �ұ�Ģó�� ���(��,��,��,��,��,��,��,��,��,��,��,�Ŷ�,�ʶ�ұ�Ģó��)
	if (m_AnalyzeAgainFromTheBeginning == false)ExceptionModule_03(pNode);
}



//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
//A,B��� ���� STRLEN(A) + STRLEN(B) == STRLEN(��ü����)�� ���� ��츸 ó��
// �ұ�Ģ�� ��� B�� V ���� ���� ��쿡�� Ȯ�ο��� ������ Ȯ������ ������
//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
void CMorphoLogicalAnalysis::OfTheFourLogic_AB(H_NODE* pNode)
{
	int nFromAnalysisLocation = 0;
	int nToAnalysisLocation = 0;
	char* pStartToEndChar = NULL;
	char* pAddTwoWord = NULL;

	nToAnalysisLocation =  pNode->pHToken->subStem->nFindPos -1;
	nFromAnalysisLocation = pNode->pHToken->subEnding->nFindPos - 1;
	
	//A,B������ ���ܻ�Ȳ 
	//��,��,����,��Ű�� �̺м������� ���� ���� ��� A�� �ش� �̺м������� ��ģ �� �����Ѵ�.
	pStartToEndChar  = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, pNode->pHToken->subStem->nFindPos + 1, pNode->pHToken->subEnding->nFindPos - 1);
	if (pStartToEndChar != NULL && (strcmp(pStartToEndChar, "��") == 0 || strcmp(pStartToEndChar, "��") == 0 || strcmp(pStartToEndChar, "����") == 0 || strcmp(pStartToEndChar, "��Ű") == 0))
	{
		pAddTwoWord = m_pCNAHangulUtil->GetCombineTwoStrings(pNode->pHToken->subStem->strResult, pStartToEndChar);
	 	CHAR_DELETE(pNode->pHToken->subStem->strResult);
		pNode->pHToken->subStem->strResult = new char[strlen(pAddTwoWord) + 1];
		memset(pNode->pHToken->subStem->strResult, 0x00, sizeof(strlen(pAddTwoWord) + 1));
		strcpy(pNode->pHToken->subStem->strResult, pAddTwoWord);
	}
	else
	{
		//�̺м������� �ϳ��� ��� : ��,��... 
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
			//��ó�� ��� �м� �� B-1�� ���� ó��
			if (m_AnalyzeAgainFromTheBeginning == false)BeforeModule_01(pNode);
			//(2���) : ���(����)���ó�� ��� //V, AMB + (V or N)�϶�....������ üũ
			if (m_AnalyzeAgainFromTheBeginning == false)ExceptionModule_02(pNode);
			//(5���) : ��� ó�� �� ��Ÿ ����(B)�� ���� ���� �� üũ
			if (m_AnalyzeAgainFromTheBeginning == false)ExceptionModule_05(pNode);
			
			//(1���) : ������ ������ ������ ����ó�����(��, ��, ��, ��, ��) : ��������ѵ�
			if (m_AnalyzeAgainFromTheBeginning == false)ExceptionModule_01(pNode);
		}
		//�ش� �������� ��� A,B�� ��� �м��� �� ���
		//�ұ�Ģ ó�� ���(3���)
		if (m_AnalyzeAgainFromTheBeginning == false)ExceptionModule_03(pNode);
	}
	CHAR_DELETE(pStartToEndChar);
	CHAR_DELETE(pAddTwoWord);
}


//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
//B�� ����
//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
void CMorphoLogicalAnalysis::OfTheFourLogic_XB(H_NODE* pNode)
{
	int nFromAnalysisLocation = 0;
	int nToAnalysisLocation = 0;
	nToAnalysisLocation = 0;
	nFromAnalysisLocation = pNode->pHToken->subEnding->nFindPos - 1;

	//B�̿��� �ٸ� ������ ������ ��쿡
	if (pNode->pHToken->subEnding->nFindPos > 0)
	{
		//��ó�� ��� �м� �� B-1�� ���� ó��
		if (m_AnalyzeAgainFromTheBeginning == false)BeforeModule_01(pNode);
		//(2���) : ���(����)���ó�� ��� //V, AMB + (V or N)�϶�....������ üũ
		if (nFromAnalysisLocation - nToAnalysisLocation > 1 && m_AnalyzeAgainFromTheBeginning == false)ExceptionModule_02(pNode);
		//(5���) : ��� ó�� �� ��Ÿ ����(B)�� ���� ���� �� üũ
		if (nFromAnalysisLocation - nToAnalysisLocation > 1 && m_AnalyzeAgainFromTheBeginning == false)ExceptionModule_05(pNode);
		

		//(1���) : ������ ������ ������ ����ó�����(��, ��, ��, ��, ��)
		if (m_AnalyzeAgainFromTheBeginning == false)ExceptionModule_01(pNode);
	}

	if (m_AnalyzeAgainFromTheBeginning == false)ExceptionModule_03(pNode);
}




//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
//A�� ����
//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
void CMorphoLogicalAnalysis::OfTheFourLogic_AX(H_NODE* pNode)
{
	int nLen = strlen(pNode->pHToken->strSource) - 1;
	int nFromAnalysisLocation = 0;
	if(nLen > pNode->pHToken->subStem->nFindPos)
	{
		nFromAnalysisLocation = nLen;
		//(0���) : ������ ������ ��������ó�����(��->�ϰ�)
		if (m_AnalyzeAgainFromTheBeginning == false)ExceptionModule_00(pNode);
		//(1���) : ������ ������ ������ ����ó�����(��, ��, ��, ��, ��)
		if (m_AnalyzeAgainFromTheBeginning == false)ExceptionModule_01(pNode);
	}

	if (m_AnalyzeAgainFromTheBeginning == false)ExceptionModule_03(pNode);
}



//����ó������
//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
//(0���) : ������ ������ ��������ó�����(��->�ϰ�)
//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
void CMorphoLogicalAnalysis::ExceptionModule_00(H_NODE* pNode)
{
	int nPos;
	int nAnalysePosition = 0;
	char* pLastSyllable = NULL;
	char* pChangeLastChar = NULL;

	nAnalysePosition = strlen(pNode->pHToken->strSource) - 1;

	//��̺м��� ���� ��� ó�� ����� ó��
	if (pNode->pHToken->subEnding->strResult == NULL)
	{
		nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, nAnalysePosition, DIC_ENDING);
		pLastSyllable = m_pCNAHangulUtil->GetRightToLeftWord(pNode->pHToken->strSource, nPos);

		if (strcmp(pLastSyllable, "��") == 0)
			pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->strSource, "�ϰ�", CHANGE_CHAR_SYLLABLE_LAST);
		else if (strcmp(pLastSyllable, "��") == 0)
			pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->strSource, "�Ͼ�", CHANGE_CHAR_SYLLABLE_LAST);
		else if (strcmp(pLastSyllable, "��") == 0)
			pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->strSource, "�Ǿ�", CHANGE_CHAR_SYLLABLE_LAST);
		else if (strcmp(pLastSyllable, "��") == 0)//�����
			pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->strSource, "����", CHANGE_CHAR_SYLLABLE_LAST);
		else if (strcmp(pLastSyllable, "��") == 0)//����
			pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->strSource, "����", CHANGE_CHAR_SYLLABLE_LAST);
		else if (strcmp(pLastSyllable, "��") == 0)
			pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->strSource, "���", CHANGE_CHAR_SYLLABLE_LAST);
		else if (strcmp(pLastSyllable, "��") == 0)
			pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->strSource, "����", CHANGE_CHAR_SYLLABLE_LAST);
		else if (strcmp(pLastSyllable, "��") == 0)
			pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->strSource, "����", CHANGE_CHAR_SYLLABLE_LAST);
		else if (strcmp(pLastSyllable, "��") == 0)
			pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->strSource, "�߾�", CHANGE_CHAR_SYLLABLE_LAST);
		else if (strcmp(pLastSyllable, "��") == 0)
			pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->strSource, "�̾�", CHANGE_CHAR_SYLLABLE_LAST);
		
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

//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
//(1���) : ������ ������ ������ ����ó�����(��,��,��,��,��)
//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
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
	bool bFinalWordPos = false; //�ش� ������ nEnd ��ġ�� ��ü ������ ������ �������� Ȯ��
	
	//���ܻ�Ȳ ó�� : Ŀ�� -> Ŀ�� + �� A:X B:���� �м��� ��� B�� ������ ������ ���� ����ó���ؾ� �Ѵ�.
	if (pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strResult != NULL)
	{
		//1������ B�� �м� �Ȱ�� B�� ���������� �ľ��� �� ������ �ش� �������� �ش� ������ ���� ó���Ѵ�.
		if (strlen(pNode->pHToken->subEnding->strResult) == 2)
		{
			pOneChar = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nEnd, DIC_ENDING);//�ش� ��ġ�� ���� ��
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
	
	//���ܻ�Ȳ �߻��� �����Ѵ�.
	if (nStart > nEnd) return;

	//���ܻ�Ȳ 1
	//B�� '��'/'��'�� ��쿡�� ������ ������ Ȯ������ �ʴ´�
	if(pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strResult != NULL)
	{
		if(strcmp(pNode->pHToken->subEnding->strResult,"��") == 0 || strcmp(pNode->pHToken->subEnding->strResult, "��") == 0 || 
		   strcmp(pNode->pHToken->subEnding->strResult, "��") == 0 || strcmp(pNode->pHToken->subEnding->strResult, "��") == 0)
		{
			return;
		}
	}
	
	//���ܻ�Ȳ 2
	//A�� �����ϰ� B�� ���� ��� A+1~LEN(STR)�� ���ۻ����� ������ ��� �������� ���� �ʴ´�.
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

	pOneChar = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nEnd, DIC_ENDING);//�ش� ��ġ�� ���� ��
	pChoJungChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pOneChar, CHAR_CHOJUNGSUNG);
	pJongChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pOneChar, CHAR_JONGSUNG);

	//���ܻ�Ȳ 3
	//������ ������ ������ ���� ��� ���������� ���� 2,3������ ������ ���� �������� Ȯ���Ѵ�.
	//���� ������ ���� �� ��� �Ʒ� ������ ��ŵ�Ѵ�.
	//��â���� : (A)��  B:NULL
	if (pJongChar != NULL && strcmp(pJongChar, "��") == 0 && pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strResult == NULL)
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
		//A ����� nPos�� A+1������ Ž��
		if ((nAFindPos < nPos) && nPos >= 0)
		{
			nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, nPos, DIC_ENDING);
			pWord2 = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, nPos, nEndPosition);
			if (pWord2 != NULL && strcmp(pWord2, "") != 0) bFind2 = IsExistStemDic(pWord2);
			if (nPos == 0)nPos = -1;
		}
		//A ����� nPos�� A+1������ Ž��
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

	//��� --> �� + ���� 2017-12-04 �߰�
	//�����Դϴ�.
	if(strcmp(pJongChar,"��") == 0 || strcmp(pJongChar, "��") == 0 || strcmp(pJongChar, "��") == 0 || strcmp(pJongChar, "��") == 0 || strcmp(pJongChar, "��") == 0)
	{
		if (strcmp(pChoJungChar, "��") == 0 || strcmp(pChoJungChar, "��") == 0 || strcmp(pChoJungChar, "��") == 0 || strcmp(pChoJungChar, "��") == 0 || strcmp(pChoJungChar, "Ű") == 0 || 
			strcmp(pChoJungChar, "��") == 0 || strcmp(pChoJungChar, "��") == 0 || strcmp(pChoJungChar, "��") == 0 || strcmp(pChoJungChar, "��") == 0 || strcmp(pChoJungChar, "��") == 0 || 
			strcmp(pChoJungChar, "��") == 0 || strcmp(pChoJungChar, "��") == 0 || strcmp(pChoJungChar, "��") == 0 || strcmp(pChoJungChar, "��") == 0 || strcmp(pChoJungChar, "��") == 0 ||
			strcmp(pChoJungChar, "��") == 0 || strcmp(pChoJungChar, "��") == 0 || strcmp(pChoJungChar, "��") == 0)
		{
			//����ѵ�,�ƴ�
			nEnd = nEnd - strlen(pOneChar);
			if (nEnd >= 0) 
			{
				pStartToEndChar = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, nEnd);
			}

			if(pStartToEndChar != NULL && strcmp(pChoJungChar,"��") ==0)
			{
				if(strcmp(pJongChar, "��") == 0 ) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�Ϥ�");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�Ϥ�");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�Ϥ�");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�Ϥ�");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�Ϥ�");
			}
			else if (pStartToEndChar != NULL && strcmp(pChoJungChar, "��") == 0)
			{
				if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�Ǥ�");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�Ǥ�");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�Ǥ�");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�Ǥ�");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�Ǥ�");
			}
			else if (pStartToEndChar != NULL && strcmp(pChoJungChar, "��") == 0)
			{
				if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�ä�");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�ä�");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�ä�");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�ä�");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�ä�");
			}
			else if (pStartToEndChar != NULL && strcmp(pChoJungChar, "��") == 0)
			{
				if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "����");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "����");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "����");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "����");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "����");
			}
			else if (pStartToEndChar != NULL && strcmp(pChoJungChar, "��") == 0)
			{
				if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "����");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "����");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "����");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "����");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "����");
			}
			else if (pStartToEndChar != NULL && strcmp(pChoJungChar, "��") == 0)
			{
				if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "����");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "����");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "����");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "����");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "����");
			}
			else if (pStartToEndChar != NULL && strcmp(pChoJungChar, "��") == 0)
			{
				if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�");
			}
			else if (pStartToEndChar != NULL && strcmp(pChoJungChar, "��") == 0)
			{
				if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "����");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "����");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "����");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "����");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "����");
			}
			else if (pStartToEndChar != NULL && strcmp(pChoJungChar, "��") == 0)
			{
				if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "����");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "����");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "����");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "����");
				else if (strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "����");
			}
			else if (strcmp(pChoJungChar, "��") == 0 && strcmp(pJongChar, "��") == 0)
			{
				//�� -- �� + �� 
				//A,B���̿� �������� ���� ���� ��� A+��, ��+B�� �м� �Ϸ��Ѵ�. ó������ �ٽ� �м��ǰ� ó������ �ʴ´�.
				if (pNode->pHToken->subStem->strResult != NULL && pNode->pHToken->subEnding->strResult != NULL)
				{
					if (strlen(pNode->pHToken->strSource) == (strlen(pNode->pHToken->subStem->strResult) + strlen(pNode->pHToken->subEnding->strResult) + strlen(pOneChar)))
					{
						pAddTwoWord = m_pCNAHangulUtil->GetCombineTwoStrings(pNode->pHToken->subStem->strResult, "��");
						CHAR_DELETE(pNode->pHToken->subStem->strResult);
						pNode->pHToken->subStem->strResult = new char[strlen(pAddTwoWord) + 1];
						memset(pNode->pHToken->subStem->strResult, 0x00, sizeof(strlen(pAddTwoWord) + 1));
						strcpy(pNode->pHToken->subStem->strResult, pAddTwoWord);
						CHAR_DELETE(pAddTwoWord);
						pAddTwoWord = m_pCNAHangulUtil->GetCombineTwoStrings("��", pNode->pHToken->subEnding->strResult);
						CHAR_DELETE(pNode->pHToken->subEnding->strResult);
						pNode->pHToken->subEnding->strResult = new char[strlen(pAddTwoWord) + 1];
						memset(pNode->pHToken->subEnding->strResult, 0x00, sizeof(strlen(pAddTwoWord) + 1));
						strcpy(pNode->pHToken->subEnding->strResult, pAddTwoWord);
					}
				}
				else
				{
					pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�Ͼ�");
				}
			}
			else if (strcmp(pChoJungChar, "��") == 0 && strcmp(pJongChar, "��") == 0)
			{
				//�� -- �� + �� 
				//A,B���̿� �������� ���� ���� ��� A+��, ��+B�� �м� �Ϸ��Ѵ�. ó������ �ٽ� �м��ǰ� ó������ �ʴ´�.
				if (pNode->pHToken->subStem->strResult != NULL && pNode->pHToken->subEnding->strResult != NULL)
				{
					if (strlen(pNode->pHToken->strSource) == (strlen(pNode->pHToken->subStem->strResult) + strlen(pNode->pHToken->subEnding->strResult) + strlen(pOneChar)))
					{
						pAddTwoWord = m_pCNAHangulUtil->GetCombineTwoStrings(pNode->pHToken->subStem->strResult, "��");
						CHAR_DELETE(pNode->pHToken->subStem->strResult);
						pNode->pHToken->subStem->strResult = new char[strlen(pAddTwoWord) + 1];
						memset(pNode->pHToken->subStem->strResult, 0x00, sizeof(strlen(pAddTwoWord) + 1));
						strcpy(pNode->pHToken->subStem->strResult, pAddTwoWord);
						CHAR_DELETE(pAddTwoWord);
						pAddTwoWord = m_pCNAHangulUtil->GetCombineTwoStrings("��", pNode->pHToken->subEnding->strResult);
						CHAR_DELETE(pNode->pHToken->subEnding->strResult);
						pNode->pHToken->subEnding->strResult = new char[strlen(pAddTwoWord) + 1];
						memset(pNode->pHToken->subEnding->strResult, 0x00, sizeof(strlen(pAddTwoWord) + 1));
						strcpy(pNode->pHToken->subEnding->strResult, pAddTwoWord);
					}
				}
				else
				{
					pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�Ǿ�");
				}
			}
			else if (strcmp(pChoJungChar, "Ű") == 0)
			{
				pAddTwoWord = m_pCNAHangulUtil->GetCombineTwoStrings(pChoJungChar, pJongChar);
				pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, pAddTwoWord);
			}
			else if (strcmp(pChoJungChar, "��") == 0 && strcmp(pJongChar, "��") == 0)
			{
				pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "����");
			}
			else if (strcmp(pChoJungChar, "��") == 0)
			{
				pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "����");
			}
			else if (strcmp(pChoJungChar, "��") == 0 && strcmp(pJongChar, "��") == 0)
			{
				pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "����");
			}
			else if (strcmp(pChoJungChar, "��") == 0)//�Դϴ�
			{
				if(strcmp(pJongChar, "��") == 0)
					pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�̤�");
				else if (strcmp(pJongChar, "��") == 0)
					pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�̤�");
			}
			else if (strcmp(pChoJungChar, "��") == 0 && strcmp(pJongChar, "��") == 0)//�Դϴ�
			{
				pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�־�");
			}
			else if (strcmp(pChoJungChar, "��") == 0 && strcmp(pJongChar, "��") == 0)
			{
				//1�����϶� �м��� ���� A�� '��'�� �ٲپ� ������
				//if(pNode->pHToken->subStem->strResult != NULL && strlen(pNode->pHToken->subStem->strResult) <= 2)
				if(strlen(pNode->pHToken->strSource) == 2)
					pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(NULL, "�餤");
				else
					pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�餤");
			}
			if (pAddTwoCandidateStemSyllableWord != NULL)
			{
				//�ش� ������ nEnd ��ġ�� ��ü ������ ������ �������� Ȯ�� ������ ������ ��,��,��,��,������ ����Ǿ������� ���ڿ��� ���� �ʿ䰡 ����.
				if (pNode->pHToken->subEnding != NULL && bFinalWordPos == true)
				{
					CHAR_DELETE(pNode->pHToken->subEnding->strResult);
					CHAR_DELETE(pNode->pHToken->subEnding->strFeature);
				}

				//�м��� ��̰� ���� ��� �ش� ��̸� strSource�� ���� �ٽ� �м��ϰ� �Ѵ�.
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
			if (strcmp(pOneChar, "��") == 0)
			{
				pAddTwoWord = m_pCNAHangulUtil->GetCombineTwoStrings(pNode->pHToken->subStem->strResult, "����");
				pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord, pNode->pHToken->subEnding->strResult);
				CHAR_DELETE(pNode->pHToken->strSource);
				pNode->pHToken->strSource = new char[strlen(pAddTwoCandidateStemSyllableWord) + 1];
				memset(pNode->pHToken->strSource, 0x00, strlen(pAddTwoCandidateStemSyllableWord) + 1);
				strcpy(pNode->pHToken->strSource, pAddTwoCandidateStemSyllableWord);
				
				m_AnalyzeAgainFromTheBeginning = true;
			}
			else
			{
				//B�� 1���� �� ��� B�� ������ ��,��,��,��,���� ��� strSource�� ���� �����ؼ� �ٽ� �м� �õ�
				//�Ұ��� --> �Ұ��� + �� 2017-12-27 �߰� A:�� B:�� �̺м���:��
				if (pNode->pHToken->subEnding->strResult != NULL && strlen(pNode->pHToken->subEnding->strResult) == 2)
				{
					CHAR_DELETE(pStartToEndChar);
					CHAR_DELETE(pOneChar);
					CHAR_DELETE(pChoJungChar);
					CHAR_DELETE(pJongChar);
					pOneChar = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->subEnding->strResult, 1, DIC_ENDING);//�ش� ��ġ�� ���� ��
					pChoJungChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pOneChar, CHAR_CHOJUNGSUNG);
					pJongChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pOneChar, CHAR_JONGSUNG);
					if (strcmp(pJongChar, "��") == 0 || strcmp(pJongChar, "��") == 0 || strcmp(pJongChar, "��") == 0 || strcmp(pJongChar, "��") == 0 || strcmp(pJongChar, "��") == 0)
					{
						if (strcmp(pChoJungChar, "��") == 0)
						{
							pStartToEndChar = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, strlen(pNode->pHToken->strSource) - strlen(pNode->pHToken->subEnding->strResult) - 1);
							if (pStartToEndChar != NULL && strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�Ϥ�");
							else if (pStartToEndChar != NULL && strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�Ϥ�");
							else if (pStartToEndChar != NULL && strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�Ϥ�");
							else if (pStartToEndChar != NULL && strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�Ϥ�");
						}
						else if (strcmp(pChoJungChar, "��") == 0)
						{
							pStartToEndChar = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, strlen(pNode->pHToken->strSource) - strlen(pNode->pHToken->subEnding->strResult) - 1);
							if (pStartToEndChar != NULL && strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�Ǥ�");
							else if (pStartToEndChar != NULL && strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�Ǥ�");
							else if (pStartToEndChar != NULL && strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�Ǥ�");
							else if (pStartToEndChar != NULL && strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�Ǥ�");
						}
						else if (strcmp(pChoJungChar, "��") == 0)
						{
							pStartToEndChar = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, strlen(pNode->pHToken->strSource) - strlen(pNode->pHToken->subEnding->strResult) - 1);
							if (pStartToEndChar != NULL && strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�ä�");
							else if (pStartToEndChar != NULL && strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�ä�");
							else if (pStartToEndChar != NULL && strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�ä�");
							else if (pStartToEndChar != NULL && strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "�ä�");
						}
						else if (strcmp(pChoJungChar, "��") == 0)
						{
							pStartToEndChar = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, strlen(pNode->pHToken->strSource) - strlen(pNode->pHToken->subEnding->strResult) - 1);
							if (pStartToEndChar != NULL && strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "����");
							else if (pStartToEndChar != NULL && strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "����");
							else if (pStartToEndChar != NULL && strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "����");
							else if (pStartToEndChar != NULL && strcmp(pJongChar, "��") == 0) pAddTwoCandidateStemSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pStartToEndChar, "����");
						}
						if (pAddTwoCandidateStemSyllableWord != NULL)
						{
							//�ش� ������ nEnd ��ġ�� ��ü ������ ������ �������� Ȯ�� ������ ������ ��,��,��,��,������ ����Ǿ������� ���ڿ��� ���� �ʿ䰡 ����.
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




//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
//���վ��(������ ���) ó�� ���(��->��)
//1) ��ü���ӹ� : -�� -
//2) ���� : -�� - <����>, -�� - <����>, -�� - <�̷�>
//3) ���� : -�� -
//4) �����ӹ��� �ռ�ü : -�� -
//5) ���� : -�� - , -�� - , -�� -
//6) ������ : -�� - , -�� -
//7) '��' B�� N,Aff�̸鼭 1������ ��� B-1�� '��'�� ��� ���վ�̷� ����
//8) B�� 'V'������ ���� ��쿡�� SKIP �Ѵ�.
//9) ��,�ƿ� ���� ó�� ������� ���������ϰ� ó������ �ٽ� �м��ϰ� �ؾ� �ϳ�? ��->�Ǿ� ��->�Ͼ�  
//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
void CMorphoLogicalAnalysis::ExceptionModule_02(H_NODE* pNode)
{
	H_SUBNODE* pStartNode = NULL;
	char* pAddEndingResultWords = NULL;
	char* pOneSyllable = NULL;
	bool bCompoundEndSyllable = false;
	int nFromPosition = 0;
	int nToPosition = 0;
	bool bNextEndingCheck = true;

	
	//B�� ������ ���� ��� ��� ��̸� �м����� �ʴ´�.
	/*
	if (pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strResult != NULL && strlen(pNode->pHToken->subEnding->strResult) != 2)
	{
		//B�� 1���� �̻��� ��� ���� ��Ȳ ó�� : B-1�� ������ '��'�� ������ ��� skip���� �ʰ� ó���Ѵ�.
		if (pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strResult != NULL)nFromPosition = pNode->pHToken->subEnding->nFindPos - 1;
		pOneSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nFromPosition, DIC_ENDING);
		if (pOneSyllable != NULL)
		{
			bNextEndingCheck = false;
			char* pJongChar = NULL;
			pJongChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pOneSyllable, CHAR_JONGSUNG);
			if (strcmp(pJongChar, "��") == 0)
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

	if (strcmp(pNode->pHToken->subEnding->strResult, "��") == 0 || strcmp(pNode->pHToken->subEnding->strResult, "��") == 0 || strcmp(pNode->pHToken->subEnding->strResult, "��") == 0 || strcmp(pNode->pHToken->subEnding->strResult, "��") == 0) return;
	
	
	
	nFromPosition = strlen(pNode->pHToken->strSource) - 1;
	if(pNode->pHToken->subStem != NULL && pNode->pHToken->subStem->strResult != NULL)nToPosition = pNode->pHToken->subStem->nFindPos + 1;
	if (pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strResult != NULL)nFromPosition = pNode->pHToken->subEnding->nFindPos - 1;

	//���þ�ε���
	//A�� N�� �����̰� A+1�� '��'�� �� �Ʒ��� B�� ���� üũ���� �켱������ ����.
	if (pNode->pHToken->subStem->strResult != NULL && pNode->pHToken->subEnding->strFeature[STEM_DIC_N] == '1')
	{
		pOneSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nFromPosition, DIC_ENDING);
		if(strcmp(pOneSyllable, "��") == 0)
		{
			bNextEndingCheck = false;
		}
		CHAR_DELETE(pOneSyllable);
	}
	if(bNextEndingCheck == true)
	{
		//���� ��Ȳ
		//������ ������ ��,��,���� ���
		//'�����ؿ���'���� ���� �� �� ������ġ �Լ��� �� ȣ���� ���
		if (pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strResult != NULL && (strcmp(pNode->pHToken->subEnding->strResult, "��") == 0 || strcmp(pNode->pHToken->subEnding->strResult, "��") == 0 || strcmp(pNode->pHToken->subEnding->strResult, "��") == 0))
		{
			//A+1~B-1���� ��� �������� ã�´�.
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
							InsertAddCompoundSubTail(pNode, pNode->pHToken->subEnding->strResult, DIC_ENDING, INSERT_RIGHT_TO_LEFT);//���վ������ 
						}
						InsertAddCompoundSubTail(pNode, pEndingWord, DIC_ENDING, INSERT_RIGHT_TO_LEFT);//���վ������ 
					}
					else
					{
						InsertAddCompoundSubTail(pNode, pEndingWord, DIC_ENDING, INSERT_RIGHT_TO_LEFT);//���վ������ 
					}
					bCompoundEndSyllable = true;
					pNode->pHToken->subEnding->nFindPos = strlen(pNode->pHToken->strSource) - strlen(pEndingWord) - strlen(pNode->pHToken->subEnding->strResult);//���������� ã�� ����� ��ġ������ �����Ѵ�.
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
			//���ܻ�Ȳ
			//�űԻ�����ʹ¿��� A:�ű� B:�� '��'�� amb n v aff �ο�. 1������ ��� ��� ��ã���� ���, 2���� �̻��� ��� ��̸� ������� �Ѵ�.
			pOneSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nFromPosition, DIC_ENDING);
			if (strlen(pOneSyllable) == 2) nFromPosition = nFromPosition - 1;//�ѱ��� ���
			if (
				(strcmp(pOneSyllable, "��") == 0 || strcmp(pOneSyllable, "��") == 0
					|| strcmp(pOneSyllable, "��") == 0 || strcmp(pOneSyllable, "��") == 0
					|| strcmp(pOneSyllable, "��") == 0 || strcmp(pOneSyllable, "��") == 0
					|| strcmp(pOneSyllable, "��") == 0 || strcmp(pOneSyllable, "��") == 0
					|| strcmp(pOneSyllable, "��") == 0 || strcmp(pOneSyllable, "��") == 0
					|| strcmp(pOneSyllable, "��") == 0 || strcmp(pOneSyllable, "��") == 0
					|| strcmp(pOneSyllable, "��") == 0
					//B�� N,Aff�̸鼭 1������ ��� B-1�� '��'�� ��� ���վ�̷� ����
					|| strcmp(pOneSyllable, "��") == 0
					|| strcmp(pOneSyllable, "��") == 0
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
						InsertAddCompoundSubTail(pNode, pNode->pHToken->subEnding->strResult, DIC_ENDING, INSERT_RIGHT_TO_LEFT);//���վ������ 
					}
					InsertAddCompoundSubTail(pNode, pOneSyllable, DIC_ENDING, INSERT_RIGHT_TO_LEFT);//���վ������ 
				}
				else
				{
					InsertAddCompoundSubTail(pNode, pOneSyllable, DIC_ENDING, INSERT_RIGHT_TO_LEFT);//���վ������ 
				}
				pNode->pHToken->subEnding->nFindPos = nFromPosition;//���������� ã�� ����� ��ġ������ �����Ѵ�.
			}
			else
			{
				CHAR_DELETE(pOneSyllable);
				break;
			}
			CHAR_DELETE(pOneSyllable);
		}
	}
	//���վ�̺м��� ��� �������� �м��� ��� ������ �ϳ��� ���� subEnding->strResult�� �ٽ� �����Ѵ�.
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
		//��->�Ǿ�, ��->�Ͼ� ���� ����
		if (pOneSyllable != NULL && (strcmp(pOneSyllable, "��") == 0 || strcmp(pOneSyllable, "��") == 0 || strcmp(pOneSyllable, "��") == 0))
		{
			char* pAddTwoWord_01 = NULL;
			char* pAddTwoWord_02 = NULL;
			char* pAddTwoWord_03 = NULL;
			char* pAddTwoWord_04 = NULL;
			//A,B ����
			if (pNode->pHToken->subStem != NULL && pNode->pHToken->subStem->strResult != NULL && pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strResult != NULL)  
			{
				pAddTwoWord_01 = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, pNode->pHToken->subStem->nFindPos);
				if(strcmp(pOneSyllable, "��") == 0) pAddTwoWord_02 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_01, "�Ǿ�");
				if (strcmp(pOneSyllable, "��") == 0) pAddTwoWord_02 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_01, "�Ͼ�");
				if (strcmp(pOneSyllable, "��") == 0) pAddTwoWord_02 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_01, "�Ͼ�");
				pAddTwoWord_03 = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, pNode->pHToken->subEnding->nFindPos, strlen(pNode->pHToken->strSource) - 1);
				pAddTwoWord_04 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_02, pAddTwoWord_03);

				CHAR_DELETE(pNode->pHToken->strSource);
				pNode->pHToken->strSource = new char[strlen(pAddTwoWord_04) + 1];
				memset(pNode->pHToken->strSource, 0x00, strlen(pAddTwoWord_04) + 1);
				strcpy(pNode->pHToken->strSource, pAddTwoWord_04);
			}
			//A�� ����
			else if (pNode->pHToken->subStem != NULL && pNode->pHToken->subStem->strResult != NULL && pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strResult == NULL)
			{
				pAddTwoWord_01 = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, pNode->pHToken->subStem->nFindPos);
				if (strcmp(pOneSyllable, "��") == 0) pAddTwoWord_02 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_01, "�Ǿ�");
				if (strcmp(pOneSyllable, "��") == 0) pAddTwoWord_02 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_01, "�Ͼ�");
				if (strcmp(pOneSyllable, "��") == 0) pAddTwoWord_02 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_01, "�Ͼ�");
				pAddTwoWord_03 = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, pNode->pHToken->subStem->nFindPos + 1, strlen(pNode->pHToken->strSource) - 1);
				pAddTwoWord_04 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_02, pAddTwoWord_03);

				CHAR_DELETE(pNode->pHToken->strSource);
				pNode->pHToken->strSource = new char[strlen(pAddTwoWord_04) + 1];
				memset(pNode->pHToken->strSource, 0x00, strlen(pAddTwoWord_04) + 1);
				strcpy(pNode->pHToken->strSource, pAddTwoWord_04);
			}
			//B�� ����
			else if (pNode->pHToken->subStem != NULL && pNode->pHToken->subStem->strResult == NULL && pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strResult == NULL)
			{
				pAddTwoWord_01 = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, pNode->pHToken->subEnding->nFindPos -1);
				if (strcmp(pOneSyllable, "��") == 0) pAddTwoWord_02 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_01, "�Ǿ�");
				if (strcmp(pOneSyllable, "��") == 0) pAddTwoWord_02 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_01, "�Ͼ�");
				if (strcmp(pOneSyllable, "��") == 0) pAddTwoWord_02 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_01, "�Ͼ�");
				pAddTwoWord_03 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_01, pAddTwoWord_02);

				CHAR_DELETE(pNode->pHToken->strSource);
				pNode->pHToken->strSource = new char[strlen(pAddTwoWord_03) + 1];
				memset(pNode->pHToken->strSource, 0x00, strlen(pAddTwoWord_03) + 1);
				strcpy(pNode->pHToken->strSource, pAddTwoWord_03);
			}
			//��� ���� ���
			else
			{
				//
				int nEndPosition = strlen(pNode->pHToken->strSource) - 1;
				int nLen = strlen(pNode->pHToken->strSource);
				int nPos = 0;
				nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, nEndPosition, DIC_ENDING);
				
				pAddTwoWord_01 = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, nPos - 1);
				if (strcmp(pOneSyllable, "��") == 0) pAddTwoWord_02 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_01, "�Ǿ�");
				if (strcmp(pOneSyllable, "��") == 0) pAddTwoWord_02 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_01, "�Ͼ�");
				if (strcmp(pOneSyllable, "��") == 0) pAddTwoWord_02 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_01, "�Ͼ�");

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

	//��,���� ��� ��м��ϰ� ó���Ѵ�.
	if (m_AnalyzeAgainFromTheBeginning == true)
	{
		//������ �м��� ���� ��� �����ϰ� ó������ �м��ϰ� ó���Ѵ�.
		m_AnalyzeAgainFromTheBeginning = false;
		AnalysisModule_01(pNode);
	}
}



//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
//�ұ�Ģó�� ���
//(��,��,��,��,��,��,��,��,��,��,��,�Ŷ�,�ʶ�ұ�Ģó��)
//A,B[V�����ϰ��] ���̿� �̺м� 1���� ���� �� : A+1~B-1�� ������ ����, B�� ù ����(B�� ������ ��� ���վ���� ù ���� ��) �� ��)���ǵ����� ���ǵ� + ����
//A,B[V�����ϰ��] ���̿� �̺м� 1���� �̻� ���� �� : A+1~B-1�� ������ ����, B�� ù ����(B�� ������ ��� ���վ���� ù ���� ��) �� ��)���ǵ����� ���ǵ� + ����
//A,B[V�����ϰ��] ���̿� �̺м� ���� ���� �� : A�� ������ ���� + B�� ù ����(B�� ������ ��� ���վ���� ù ���� ��) �� ��)�ɾ� ->�� + ��

//A�� ���� ��(������ ������ 2�̻� �м�) :  A+A+1~MAX(LEN)-1), MAX(LEN) ��)������-->���� + �� 
//B��[V�����ϰ��]�� ���� ��((B ���� ������ 2�̻� �м�)) :  B-2,B-1�� ���� �� ��)������������-->������ + �� + �� + ��
//A,B ��� ���� ��(������ ������ 2 �̻� �м�) :  MAX(LEN)-1, MAX(LEN) ��  ��)��������-->������ + ��
//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
void CMorphoLogicalAnalysis::ExceptionModule_03(H_NODE* pNode)
{
	char* pUnTranslatedSyllable = NULL;//�̺м�������(A~B������ ����)
	char* pA_EndSyllable = NULL;//A�Ǹ��������� �Ǵ� (��->��)��2��°����
	char* pB_FirstSyllable = NULL;//B��ù���� �Ǵ� (��->��)��1��°����
	char* pIrregularRecoverySyllable = NULL;//�ұ�Ģ����

	char* pOneChar = NULL;

	char* pA_CharChoJung = NULL;
	char* pA_CharJongChar = NULL;
	char* pA_ChoChar = NULL;//�������� �ʼ�
	char* pA_JungChar = NULL;//�������� �߼�
	char* pA_JongChar = NULL;//�������� ����

	char* pB_CharChoJung = NULL;
	char* pB_CharJongChar = NULL;
	char* pB_ChoChar = NULL;//�������� �ʼ�
	char* pB_JungChar = NULL;//�������� �߼�
	char* pB_JongChar = NULL;//�������� ����


	char* pIrregularRestoreLocationSyllableWord = NULL;//�ұ�Ģ��������(pNode->pHToken->strSource)
	char* pAddTwoWord_01 = NULL;
	char* pAddTwoWord_02 = NULL;
	char* pAddTwoWord_03 = NULL;
	char* pAddTwoWord_04 = NULL;
	char* pAddTwoWord_05 = NULL;

	char* pGrammer = NULL;
	int nA_IrregularRestoreLocation = -1;//�ұ�Ģ �ش� ������ ���� ��ġ
	int nB_IrregularRestoreLocation = -1;//�ұ�Ģ �ش� ������ ���� ��ġ
	int nPos = 0;
	int nFirstPos =  - 1;
	int nEndPos = -1;
	int nLen = 0;
	int nABLen = 0;

	nLen = strlen(pNode->pHToken->strSource);
	
	//�ұ�Ģ�� ��� ����/������� ��� ���� ������. 
	//���� �� �м��� ���� A�� N�� ������ ���� ���... �ұ�Ģ ó���� �����Ѵ�.
	if (pNode->pHToken->subStem != NULL && pNode->pHToken->subStem->strResult != NULL)
	{
		if (pNode->pHToken->subStem->strFeature[STEM_DIC_N] == '1')
			return;
		if (pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strResult != NULL)
		{	
			//��̰� 2���� �̻����� �м��� ��� A,B�� �м����� ��ü������ ���� ���
			if (strlen(pNode->pHToken->subEnding->strResult) >= 4 && strlen(pNode->pHToken->strSource) == (strlen(pNode->pHToken->subStem->strResult) + strlen(pNode->pHToken->subEnding->strResult)))
				return;
		}
	}

	//A,B�� ���� LEN(STR)�� ���� ��� SKIP �Ѵ�.
	if (pNode->pHToken->subStem->strResult != NULL) nABLen += strlen(pNode->pHToken->subStem->strResult);
	if (pNode->pHToken->subEnding->strResult != NULL) nABLen += strlen(pNode->pHToken->subEnding->strResult);
	if (nLen == nABLen) return;

	//A,B������ �̺м� ������ 2���� �̻��̶�� SKIP�Ѵ�.
	if(nLen > nABLen+4) return;
		

	//A,B�����
	if (pNode->pHToken->subStem != NULL && pNode->pHToken->subStem->strResult != NULL && pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strResult != NULL)
	{
		//A��B���̿� ������ �������� ���� ���
		if (pNode->pHToken->subEnding->nFindPos - pNode->pHToken->subStem->nFindPos ==  1)
		{
			//A�� ������ ����
			pA_EndSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->subStem->strResult, pNode->pHToken->subStem->nFindPos, DIC_ENDING);
			//B�� ù ����
			pB_FirstSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->subEnding->strResult, 0, DIC_STEM);
			nB_IrregularRestoreLocation = pNode->pHToken->subEnding->nFindPos;

			//�ұ�Ģ �ش� ������ ���� ��ġ
			nA_IrregularRestoreLocation = pNode->pHToken->subStem->nFindPos - 1;
		}
		else
		{
			//A~B������ ����
			pUnTranslatedSyllable = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, pNode->pHToken->subStem->nFindPos + 1, pNode->pHToken->subEnding->nFindPos - 1);
			if (pUnTranslatedSyllable != NULL)
			{
				//A~B������ ������ ���ۻ����� ������ ���
				if (IsExistStemDic(pUnTranslatedSyllable) == true)
				{
					CHAR_DELETE(pUnTranslatedSyllable);
					return;
				}

				//A�� ������ ����
				//pA_EndSyllable = m_pCNAHangulUtil->GetPositionOneChar(pUnTranslatedSyllable, strlen(pUnTranslatedSyllable) -1 , DIC_ENDING);
				pA_EndSyllable = m_pCNAHangulUtil->GetPositionOneChar(pUnTranslatedSyllable, 0, DIC_STEM);

				//B�� ù ����
				//pB_FirstSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->subEnding->strResult,0, DIC_STEM);
				//nB_IrregularRestoreLocation = pNode->pHToken->subEnding->nFindPos;
				pB_FirstSyllable = m_pCNAHangulUtil->GetPositionOneChar(pUnTranslatedSyllable, strlen(pA_EndSyllable), DIC_STEM);
				nB_IrregularRestoreLocation = pNode->pHToken->subStem->nFindPos + strlen(pA_EndSyllable) + strlen(pB_FirstSyllable) - 1;

				//�ұ�Ģ �ش� ������ ���� ��ġ
				//nA_IrregularRestoreLocation = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, pNode->pHToken->subEnding->nFindPos - 1, DIC_ENDING);
				nA_IrregularRestoreLocation = pNode->pHToken->subStem->nFindPos + 1;
			}
		}
	}
	//A�� �����
	else if (pNode->pHToken->subStem != NULL && pNode->pHToken->subStem->strResult != NULL && (pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strResult == NULL))
	{
		//B�� ù ����
		nPos = strlen(pNode->pHToken->strSource) - 1;
		pB_FirstSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nPos, DIC_ENDING);
		nPos = nPos - 1;
		nB_IrregularRestoreLocation = nPos;
		if(strlen(pB_FirstSyllable) == 2)
		{
			nPos = nPos - 1;
		}
		//A�� ������ ����
		pA_EndSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nPos,DIC_ENDING);
		nPos = nPos - 1;
		nA_IrregularRestoreLocation = nPos;
	}
	//B�� �����	
	else if ((pNode->pHToken->subStem != NULL && pNode->pHToken->subStem->strResult == NULL) && pNode->pHToken->subEnding != NULL && pNode->pHToken->subEnding->strResult != NULL)
	{
		//�̺м��� 2���� �̻��� ��� B�� �������� ������ �� ������ ����.
		if(pNode->pHToken->subEnding->nFindPos > 2)
		{
			//B�� ù ����
			nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, pNode->pHToken->subEnding->nFindPos - 1, DIC_ENDING);
			pB_FirstSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nPos, DIC_STEM);
			nB_IrregularRestoreLocation = nPos;

			if (strlen(pB_FirstSyllable) == 2)
				nPos = nPos - 1;
			//A�� ������ ����
			pA_EndSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nPos, DIC_ENDING);
		}
		else
		{
			//B�� ù ����
			pB_FirstSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->subEnding->strResult, 0, DIC_STEM);
			nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, pNode->pHToken->subEnding->nFindPos - 1, DIC_ENDING);
			//A�� ������ ����
			pA_EndSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nPos, DIC_STEM);
		}
		//�ұ�Ģ �ش� ������ ���� ��ġ
		if (pB_FirstSyllable != NULL && pA_EndSyllable != NULL)
		{
			nA_IrregularRestoreLocation = nPos;
			if (strlen(pA_EndSyllable) == 2)
			{
				nA_IrregularRestoreLocation = nA_IrregularRestoreLocation - 1;
			}
		}
	}
	//A,B ��� �м� ���� ��
	else
	{
		//B�� ù ����
		nPos = strlen(pNode->pHToken->strSource) - 1;
		pB_FirstSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nPos, DIC_ENDING);
		nPos = nPos - 1;
		nB_IrregularRestoreLocation = nPos;
		if (strlen(pB_FirstSyllable) == 2)
		{
			nPos = nPos - 1;
		}
		//(�̺м�����)A�� ù ����
		if(nPos >= 0)
		{
			pA_EndSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nPos, DIC_ENDING);
			nPos = nPos - 1;
			if (nPos < 0) nPos = 0;
			nA_IrregularRestoreLocation = nPos;
		}
		else
		{
			//1������ �� ������ A,B ��� �м��� ���� ���� ��쿡 ���� ó�� 
			//A�� ���� ��� B�� ù������ ��ü�Ѵ�.
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
		//�ұ�Ģ ���� ����
		//�ұ�Ģ ó���� ���� ���߼� ���� ���������� ��������-1�� ������ �������� ���� ��� �ұ�Ģ ó���� ���� �ʴ´�.
		//====================================================================================================================================
		if (strcmp(pA_CharChoJung, "") != 0 && strcmp(pB_CharChoJung, "") != 0)
		{
			//====================================================================================================================================
			//�� �ұ�Ģ
			//�˻��� : "��", "��", "��", "��", "��", "��", "��", "��", "��", "��" �� ������ � + ���ù���� ù�Ҹ��� ���� ���
			//====================================================================================================================================
			if (pGrammer == NULL)
			{
				if (strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0
					|| strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0
					|| strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0
					|| strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0)
				{
					if (strcmp(pB_ChoChar, "��") == 0)
					{
						pIrregularRecoverySyllable = m_pCNAHangulUtil->ChangeCompulsionJongSyllable(pA_EndSyllable, "��");//������
						//�뷡�Ǿ� -> �뷡�ƾ�
						pIrregularRestoreLocationSyllableWord = m_pCNAHangulUtil->SetIrregularRestoreLocationSyllableWord(pNode->pHToken->strSource, pIrregularRecoverySyllable, nA_IrregularRestoreLocation);
						pGrammer = "��";
					}
				}
			}
			//====================================================================================================================================
			//�� �ұ�Ģ
			//� ���Ҹ� "��"�� "��"�� �ٲ�� Ȱ�� �����̴�.����, ����� ��� "��" ���·� �����
			//�˻��� : "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "¥", "¼", "��", "��", "��", "Ÿ", "��", "��" �Ǵ� "��"
			//�� ������ � + ���ù���� �ʼ��� '��' �̰� �߼��� '��', '��', '��', '��' ��(�����δ� ��, ��, ��, ��, ��, ��) ���
			//====================================================================================================================================
			if (pGrammer == NULL)
			{
				if (strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0
					|| strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0
					|| strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0
					|| strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0
					|| strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0
					|| strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0
					|| strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0
					|| strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "¥") == 0 || strcmp(pA_EndSyllable, "¼") == 0 || strcmp(pA_EndSyllable, "��") == 0
					|| strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "Ÿ") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0
					|| strcmp(pA_EndSyllable, "��") == 0)
				{
					if (strcmp(pB_ChoChar, "��") == 0 && (strcmp(pB_JungChar, "��") == 0 || strcmp(pB_JungChar, "��") == 0 || strcmp(pB_JungChar, "��") == 0 || strcmp(pB_JungChar, "��") == 0))
					{
						pIrregularRecoverySyllable = m_pCNAHangulUtil->ChangeCompulsionJongSyllable(pA_EndSyllable, "��");//������
						pAddTwoWord_01 = m_pCNAHangulUtil->SetIrregularRestoreLocationSyllableWord(pNode->pHToken->strSource, pIrregularRecoverySyllable, nA_IrregularRestoreLocation);
						if (strcmp(pB_JungChar, "��") == 0 || strcmp(pB_JungChar, "��") == 0)
						{
							pIrregularRestoreLocationSyllableWord = m_pCNAHangulUtil->SetIrregularRestoreLocationSyllableWord(pAddTwoWord_01, "��", nB_IrregularRestoreLocation);
						}
						//����� -->����+��
						else if (strcmp(pB_JungChar, "��") == 0 || strcmp(pB_JongChar, "��") == 0)
						{
							pIrregularRestoreLocationSyllableWord = m_pCNAHangulUtil->SetIrregularRestoreLocationSyllableWord(pAddTwoWord_01, "��", nB_IrregularRestoreLocation);
						}
						else
						{
							pIrregularRestoreLocationSyllableWord = m_pCNAHangulUtil->SetIrregularRestoreLocationSyllableWord(pAddTwoWord_01, "��", nB_IrregularRestoreLocation);
						}
						pGrammer = "��";
					}
				}
				else
				{
					//���������(������+��),�β������(�β���+��),���������(������+��)
					//�߿��(��+��),���ſ��(����+��),�ſ��(��+��),�����(��+��),�������(����+��),�Ƹ��ٿ��(�Ƹ���+��)
					if (strcmp(pA_EndSyllable, "��") == 0)
					{
						if (strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0)
						{
							pOneChar = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nA_IrregularRestoreLocation - 1, DIC_ENDING);
							nA_IrregularRestoreLocation = nA_IrregularRestoreLocation - 1;
							if (nA_IrregularRestoreLocation > 0)
							{
								if (strlen(pOneChar) == 2)
								{
									nA_IrregularRestoreLocation = nA_IrregularRestoreLocation - 1;
								}
								pIrregularRecoverySyllable = m_pCNAHangulUtil->ChangeCompulsionJongSyllable(pOneChar, "��");//������

								pAddTwoWord_01 = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, nA_IrregularRestoreLocation - 1);
								pAddTwoWord_02 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_01, pIrregularRecoverySyllable);
								pIrregularRestoreLocationSyllableWord = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_02, pB_FirstSyllable);
								pGrammer = "��";
							}
						}

					}
				}
			}
			//====================================================================================================================================
			//�� �ұ�Ģ
			//� ���Ҹ� "��"�� Ȧ�Ҹ��� �����ϴ� ��� �տ��� ������� Ȱ��
			//ex) �ߴ� = >�׾�, ������ ���� = > ����, ������
			//�˻��� : "��", "��", "��", "��", "��", "��", "��", "��", "��" �� ������ ��� ��� ù���� ù�Ҹ��� '��'�� ���
			//====================================================================================================================================
			if (pGrammer == NULL)
			{
				if (strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0
					|| strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0
					|| strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0)
				{
					if (strcmp(pB_ChoChar, "��") == 0)
					{
						pIrregularRecoverySyllable = m_pCNAHangulUtil->ChangeCompulsionJongSyllable(pA_EndSyllable, "��");//������
						pIrregularRestoreLocationSyllableWord = m_pCNAHangulUtil->SetIrregularRestoreLocationSyllableWord(pNode->pHToken->strSource, pIrregularRecoverySyllable, nA_IrregularRestoreLocation);
						pGrammer = "��";
					}
				}
			}
			//====================================================================================================================================
			//�� �ұ�Ģ
			//� ���Ҹ� '��'�� '��', '��', '��', '��', '��' �տ��� ������� Ȱ�� ����
			//�˻��� : � �� ������ "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "ġ", "ũ", "��", "��", "Ʈ", "��", "Ǫ", "��" �̰�
			//����� ù���� ù�Ҹ��� ��, ��, ���̰ų� ù������ ��, ��, �� �� ���
			//====================================================================================================================================
			if (pGrammer == NULL)
			{
				if (strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 ||
					strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 ||
					strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 ||
					strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 ||
					strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 ||
					strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 ||
					strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 ||
					strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 ||
					strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 ||
					strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 ||
					strcmp(pA_EndSyllable, "ġ") == 0 || strcmp(pA_EndSyllable, "ũ") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "Ʈ") == 0 ||
					strcmp(pA_EndSyllable, "��") == 0 || strcmp(pA_EndSyllable, "Ǫ") == 0 || strcmp(pA_EndSyllable, "��") == 0)
				{
					if (strcmp(pB_ChoChar, "��") == 0 || strcmp(pB_ChoChar, "��") == 0 || strcmp(pB_ChoChar, "��") == 0
						|| strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0)
					{
						pIrregularRecoverySyllable = m_pCNAHangulUtil->ChangeCompulsionJongSyllable(pA_EndSyllable, "��");
						pIrregularRestoreLocationSyllableWord = m_pCNAHangulUtil->SetIrregularRestoreLocationSyllableWord(pNode->pHToken->strSource, pIrregularRecoverySyllable, nA_IrregularRestoreLocation);
						pGrammer = "��";
					}
				}
				//��ϴ�. �˴ϴ� ���ϴ� ���..
				else if (strcmp(pA_JongChar, "��") == 0 &&
					(strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 ||
						strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 ||
						strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 ||
						strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 ||
						strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 ||
						strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 ||
						strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 ||
						strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 ||
						strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 ||
						strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 ||
						strcmp(pA_CharChoJung, "ġ") == 0 || strcmp(pA_CharChoJung, "ũ") == 0 || strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "Ʈ") == 0 ||
						strcmp(pA_CharChoJung, "��") == 0 || strcmp(pA_CharChoJung, "Ǫ") == 0 || strcmp(pA_CharChoJung, "��") == 0))
				{
					if (strcmp(pB_ChoChar, "��") == 0 || strcmp(pB_ChoChar, "��") == 0 || strcmp(pB_ChoChar, "��") == 0
						|| strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0)
					{
						pIrregularRecoverySyllable = m_pCNAHangulUtil->ChangeCompulsionJongSyllable(pA_CharChoJung, "��");
						pIrregularRestoreLocationSyllableWord = m_pCNAHangulUtil->SetIrregularRestoreLocationSyllableWord(pNode->pHToken->strSource, pIrregularRecoverySyllable, nA_IrregularRestoreLocation);
						pGrammer = "��";
					}
				}
			}
			//====================================================================================================================================
			//�� �ұ�Ģ 1
			//� �� '��'�� ��� '-��'�̳� '-��' �տ��� ������� Ȱ��
			//ex) � = > ��� / VA + �� / ETM, �����ٶ� = > �����ٶ� / VA + �� / ETM
			//�˻��� : �������� "��", "��", "��", "��", "��", "��", "��", "��", "��", "��" �� ���(����)
			//��¿���� "��", "��", "��", "��", "��", "��", "��", "��", "��", "��" �� ���(����)
			//��̳��� "��", "��", "��", "��', "��", "��", "��", "��", "��", "��" �̰� ��� ù���� �ʼ��̤� �� ��� (������)
			//====================================================================================================================================
			if (pGrammer == NULL)
			{
				if (strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 ||
					strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 ||
					strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 ||
					strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 ||
					strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 ||
					strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0)
				{
					pIrregularRecoverySyllable = m_pCNAHangulUtil->ChangeCompulsionJongSyllable(pB_FirstSyllable, "��");
					pIrregularRestoreLocationSyllableWord = m_pCNAHangulUtil->SetIrregularRestoreLocationSyllableWord(pNode->pHToken->strSource, pIrregularRecoverySyllable, nB_IrregularRestoreLocation);
					pGrammer = "��";
				}
				else if (strcmp(pB_ChoChar, "��") == 0)
				{
					pOneChar = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nB_IrregularRestoreLocation - 1, DIC_ENDING);
					if (strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 ||
						strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 ||
						strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0)
					{
						nB_IrregularRestoreLocation = nB_IrregularRestoreLocation - 1;
						if (nB_IrregularRestoreLocation > 0)
						{
							if (strlen(pOneChar) == 2)
							{
								nB_IrregularRestoreLocation = nB_IrregularRestoreLocation - 1;
							}
							pIrregularRecoverySyllable = m_pCNAHangulUtil->ChangeCompulsionJongSyllable(pOneChar, "��");//������
							pIrregularRestoreLocationSyllableWord = m_pCNAHangulUtil->SetIrregularRestoreLocationSyllableWord(pNode->pHToken->strSource, pIrregularRecoverySyllable, nB_IrregularRestoreLocation);
							pGrammer = "��";
						}
					}
				}
			}
			//====================================================================================================================================
			//�� �ұ�Ģ 2
			//� �� '��'�� ��� '-��/-��' �տ��� �ӷ� �ٲ�� �������� Ȱ��
			//ex) ��� = > ��� / VA + �� / EC,
			//�˻��� � ���� "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��" "��", "��", "��" �� ���
			//====================================================================================================================================
			if (pGrammer == NULL)
			{
				if (strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 ||
					strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 ||
					strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0)
				{
					m_pCNAHangulUtil->ChangeCompulsionChoJungJongSyllable(pIrregularRecoverySyllable, pB_ChoChar, "��", "��");
					pIrregularRestoreLocationSyllableWord = m_pCNAHangulUtil->SetIrregularRestoreLocationSyllableWord(pNode->pHToken->strSource, pIrregularRecoverySyllable, nB_IrregularRestoreLocation);
					pGrammer = "��";

				}
				else if (strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 ||
					strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0)
				{
					m_pCNAHangulUtil->ChangeCompulsionChoJungJongSyllable(pIrregularRecoverySyllable, pB_ChoChar, "��", "��");
					pIrregularRestoreLocationSyllableWord = m_pCNAHangulUtil->SetIrregularRestoreLocationSyllableWord(pNode->pHToken->strSource, pIrregularRecoverySyllable, nB_IrregularRestoreLocation);
					pGrammer = "��";
				}
			}
			//====================================================================================================================================
			//�� �ұ�Ģ
			//��� � '��'�� ��� '-��/-��' �տ��� ������� Ȱ�� ����
			//ex) �⻵ = > ��� / VA + �� / EC, ���� = > ���� / VA + �� / EC
			//�˻��� : � ����  "��", "��", "��", "��", "��", "��", "��", "��", "��", "Ŀ", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��"
			//====================================================================================================================================
			if (pGrammer == NULL)
			{
				if (strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 ||
					strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 ||
					strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "Ŀ") == 0 || strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 ||
					strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 ||
					strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 ||
					strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0 ||
					strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0)
				{
					pIrregularRecoverySyllable = new char[4];
					memset(pIrregularRecoverySyllable, 0x00, sizeof(pIrregularRecoverySyllable));
					m_pCNAHangulUtil->ChangeCompulsionChoJungJongSyllable(pIrregularRecoverySyllable, pB_ChoChar, "��", "");
					pIrregularRestoreLocationSyllableWord = m_pCNAHangulUtil->SetIrregularRestoreLocationSyllableWord(pNode->pHToken->strSource, pIrregularRecoverySyllable, nB_IrregularRestoreLocation);
					pGrammer = "��";
				}
			}
			//====================================================================================================================================
			//�� �ұ�Ģ
			//��� '-��/-�'�� '-��'�� '-��'�� �ٲ�� Ȱ�� ����
			//ex) ��Ǫ���� = >��Ǫ����,
			//�˻��� : ��������, �뷫 10���� �����̴�.�׷��� �Ļ�� ��������� �����̶� �� �þ �� �ִ�.
			//��� "�븣", "Ǫ��", "����", "�ٸ�", "�̸�" �� ������. (�����ٸ��� ���� ��쵵 �� �ұ�Ģ�ε�, "�ٸ�"�� ������.)
			//====================================================================================================================================
			if (pGrammer == NULL)
			{
				if (strcmp(pA_EndSyllable, "��") == 0 && strcmp(pB_FirstSyllable, "��") == 0)
				{
					CHAR_DELETE(pNode->pHToken->subEnding->strResult);
					pNode->pHToken->subEnding->strResult = new char[strlen("��") + 1];
					memset(pNode->pHToken->subEnding->strResult, 0x00, strlen("��") + 1);
					strcpy(pNode->pHToken->subEnding->strResult, "��");
					pGrammer = "��";
				}
			}
			//====================================================================================================================================
			//�� �ұ�Ģ
			//��� �� ���� '��'�� '��'�� �ٰ�, ��� '-��/-��'�� '-��/-��'�� �ٲ�� Ȱ��
			//ex) ���� = > �� / VA + �� / EC
			//�˻��� : "��"�ұ�Ģ, "��" �ұ�Ģ�� �ƴϰ� � �� ������ ���� / ���� �� ������ ���
			//====================================================================================================================================
			if (pGrammer == NULL)
			{
				if (pA_CharJongChar != NULL && strcmp(pA_CharJongChar, "��") == 0)
				{
					if (strcmp(pB_FirstSyllable, "��") == 0 || strcmp(pB_FirstSyllable, "��") == 0)
					{
						pAddTwoWord_01 = m_pCNAHangulUtil->GetCombineTwoStrings(pA_CharChoJung, "��");
						if (strcmp(pB_FirstSyllable, "��") == 0)
							pAddTwoWord_02 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_01, "��");
						else
							pAddTwoWord_02 = m_pCNAHangulUtil->GetCombineTwoStrings(pAddTwoWord_01, "��");
						pIrregularRestoreLocationSyllableWord = new char[strlen(pAddTwoWord_02) + 1];
						memset(pIrregularRestoreLocationSyllableWord, 0x00, strlen(pAddTwoWord_02) + 1);
						strcpy(pIrregularRestoreLocationSyllableWord, pAddTwoWord_02);
						pGrammer = "��";
					}
				}
			}
			//====================================================================================================================================
			//�� �ұ�Ģ
			//� �� '��'�� ��� '-��' �տ��� ������� Ȱ�� ����
			//ex) �� = > Ǫ / VV + �� / EC �� �����ϴ�.
			//�˻��� : �۰� �����ϴٰ� �ؼ� �۸� �˻��ϸ� �ȵȴ�.�������� ����ؾ� �Ѵ�. "��", "��" �� ���
			//====================================================================================================================================
			if (pGrammer == NULL)
			{
				if (strcmp(pA_JungChar, "��") == 0)
				{
					pIrregularRecoverySyllable = new char[4];
					memset(pIrregularRecoverySyllable, 0x00, 4);
					if (pA_ChoChar != NULL && strcmp(pA_ChoChar, "��") == 0)
					{
						//'��'->'Ǫ'�� ����
						m_pCNAHangulUtil->ChangeCompulsionChoJungJongSyllable(pIrregularRecoverySyllable, "��", "��", "");
					}
					if (IsExistStemDic(pIrregularRecoverySyllable) == true)
					{
						pIrregularRestoreLocationSyllableWord = m_pCNAHangulUtil->SetIrregularRestoreLocationSyllableWord(pNode->pHToken->strSource, pIrregularRecoverySyllable, nA_IrregularRestoreLocation);
						pGrammer = "��";
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




//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
//���վ�ó��(���վ) ���
//A,B ����� A,B������ ���� TYPE=1
//A,B��� ���� �� TYPE=2
//B�� �����, 0~B-1 ���� Ȯ�� TYPE=3
//A�� �����, A+1 Ȯ�� TYPE=4
//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
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
			//B�� ���վ����� �� ���
			if (pNode->pHToken->bSubEndingCompCreate == true) 
			{
				nEndPosition = strlen(pNode->pHToken->strSource) - strlen(pNode->pHToken->subEnding->strResult) - 1;
			}
		}
		//A��B�� ��ü������ ���� ��� A+B == ��ü����
		else if (strlen(pNode->pHToken->strSource) == strlen(pNode->pHToken->subStem->strResult) + strlen(pNode->pHToken->subEnding->strResult))
		{
			//nCompoundType = 2;
			//nStartPosition = 0;
			//nEndPosition = strlen(pNode->pHToken->strSource) - 1;

			//A+B ������ ������� ã�ƺ���. ���� ��� SKIP ���̻� �м� ���Ѵ�.
			//A+B�� ���� ��� ��ü�� A�� ����.
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
		//���ո��� �����Ͽ� ���������� �ش��ϴ� ������ �����Ѵ�.
		m_LinkedList->CreateCompoundSubNode(pNode, "STEM");
		CompoundNounDecomposition(pNode, nStartPosition, nEndPosition, nCompoundType);//���ո�����
	}
}



//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
//����ó�� ���(��� ���� B�� �ش��ϴ� ���� ã�� ���)
//��� ��� ó�� �� ��Ÿ ��� �ĺ��鿡 ���� ó��(2����,3����,4�������� Ȯ���Ѵ� :74%�� �ش���)
//1����:123 2����:590 3����:1034 4����:918 5����:512 6����:208 7����:35 8����:8 9����:2
//�űԻ�����ʹ�  A:�ű� B:�� '����'�� B�� �м��� �Ǿ�� �Ѵ�.
//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
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
		//��������� �����Ͽ�
		CompoundPostPositionDecomposition(pNode, nStartPosition, nEndPosition, nCompoundType);//�����������
	}
}


//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
//B-1�� 1���� ó��
//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
void CMorphoLogicalAnalysis::BeforeModule_01(H_NODE* pNode)
{
	char* pOneChar = NULL;
	char* pAddTwoWord = NULL;
	int nLen = 0;
	if (pNode->pHToken->subEnding->strResult != NULL)
	{
		//�̵���ž�ü����, �����������̰ų�
		if(pNode->pHToken->subEnding->nFindPos > 0)
		{
			pOneChar = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, pNode->pHToken->subEnding->nFindPos - 1, DIC_ENDING);
			if(strcmp(pOneChar,"��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0 || strcmp(pOneChar, "��") == 0)
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

//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
//���ո�� �̺��� ���� ���ո���� ����Ʈ�� �߰� �� A�� �̸� �м��� ���� ���� ���ո�� Head�� �߰�
//��->�� ���ո�� �м�
//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
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

	//�ұ�Ģ ��� ���� �� �������� ���� �Ǿ��� ��� ���� �������� �����Ͽ� ���Ѵ�.
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
			//�̺м��� ���� �� ���ո�� �м��� ���е� ���� ������ ��� - ��� ������ �̵�Ͼ�� ���� A�� ��ü�Ѵ�.
			if(nSourceLength > nCompoundLength)
			{
				if(pNode->pHToken->subStem->strResult != NULL)
				{
					pUnAnalyseCompondWords = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, strlen(pNode->pHToken->subStem->strResult), pNode->pHToken->nLastFindCompoundNoun + strlen(pNode->pHToken->subStem->strResult) - 1);
				}
				else
				{
					//nLastFindCompoundNoun ã�� ������ ù��ġ��
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
			//�̵�Ͼ����θ� ������ ������� �����Ѵ�.
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
				pNode->pHToken->subStem->strFeature[STEM_DIC_N] = '1';//N��������ο�
			}
		}
	}
	CHAR_DELETE(strSource);
	CHAR_DELETE(pUnAnalyseCompondWords);
	CHAR_DELETE(pAddStemResultWords);
}

//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
//A, B ������ �м����� �ʴ� ��� ���� ó�� : A + 1~B ��ü�� ��̷� ���� ó��
//��� ���� ���н� - ��ü������ ������ ������ 
//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
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

	//�Է¾����� ��� �м��Ѱ�� SKIP �Ѵ�.
	if (pNode->pHToken->subStem->strResult != NULL && pNode->pHToken->subEnding->strResult != NULL)
	{
		if (strlen(pNode->pHToken->strSource) <= strlen(pNode->pHToken->subStem->strResult) + strlen(pNode->pHToken->subEnding->strResult))
			return;
	}

	if (pNode->pHToken->bSubCompCreate == true)
	{
		//nEndPosition�� �� �����Ѵ�.
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
			//��̷� �м� ���� �ʴ� ��� �̺м� ���վ� ����.
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
								//INSERT_RIGHT_TO_LEFT �Ǿտ� �߰�
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
							//INSERT_RIGHT_TO_LEFT �Ǿտ� �߰�
							InsertAddCompoundSubTail(pNode, strSource, DIC_STEM, INSERT_RIGHT_TO_LEFT);
							//���ո�� ��⿡�� �߰��� ���н� A�� �̺м��� ������ ���� ó���� ���� �����Ƿ� �ش� ���ǹ����� ó���Ѵ�.
							if (pNode->pHToken->subStem->strResult != NULL)
							{
								//INSERT_RIGHT_TO_LEFT �Ǿտ� �߰�
								InsertAddCompoundSubTail(pNode, pNode->pHToken->subStem->strResult, DIC_STEM, INSERT_RIGHT_TO_LEFT);
							}
							//INSERT_LEFT_TO_RIGHT �ǵڿ� �߰�
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
						//�̵�Ͼ����θ� ������ ������� �����Ѵ�.
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
							pNode->pHToken->subStem->strFeature[STEM_DIC_N] = '1';//N��������ο�
							bOk = true;
						}
					}
				}
			}
		}
	}

	if(bOk == false)
	{
		//�м� ���н�(A,B �� ����� ó��) 
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
							//INSERT_RIGHT_TO_LEFT �Ǿտ� �߰�
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
						//INSERT_RIGHT_TO_LEFT �Ǿտ� �߰�
						InsertAddCompoundSubTail(pNode, strSource, DIC_STEM, INSERT_RIGHT_TO_LEFT);
						//���ո�� ��⿡�� �߰��� ���н� A�� �̺м��� ������ ���� ó���� ���� �����Ƿ� �ش� ���ǹ����� ó���Ѵ�.
						if (pNode->pHToken->subStem->strResult != NULL)
						{
							//INSERT_RIGHT_TO_LEFT �Ǿտ� �߰�
							InsertAddCompoundSubTail(pNode, pNode->pHToken->subStem->strResult, DIC_STEM, INSERT_RIGHT_TO_LEFT);
						}
					}
				}
				else
				{
					if (pNode->pHToken->bSubCompCreate == true)
					{
						//INSERT_RIGHT_TO_LEFT �Ǿտ� �߰�
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
					//�̵�Ͼ����θ� ������ ������� �����Ѵ�.
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
						pNode->pHToken->subStem->strFeature[STEM_DIC_N] = '1';//N��������ο�
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
					pNode->pHToken->subStem->strFeature[STEM_DIC_N] = '1';//N��������ο�
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












//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
//Linked List�� SubTail �߰�(���վ �Ǵ� ���վ��)
//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
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
					pInsertNode->pCSubToken->strFeature[STEM_DIC_N] = '1';//N��������ο�
				}
				//���ո�� ���� ����(�߰��� ��带 ����Ʈ�� �� �տ� ����)
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
				//���ո�� ���� ����(�߰��� ��带 ����Ʈ�� �� �ڿ� ����)
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
					pInsertNode->pCSubToken->strFeature[ENDING_DIC_N] = '1';//N ���������ο�
				}
				//���ո�� ���� ����(�߰��� ��带 ����Ʈ�� �� �տ� ����)
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
				//���ո�� ���� ����(�߰��� ��带 ����Ʈ�� �� �ڿ� ����)
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



//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
//�������� �߰�
//���������� 2,3,4������ ������� ã�´�.
//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
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

	//������������������ : 1����:123 2����:590 3����:1034 4����:918 5����:512 6����:208 7����:35 8����:8 9����:2
	//2, 3, 4������ ��κ��� ������ ���������� �������� �̵�Ͼ� �м� ������ 2,3,4������ ������� �Ѵ�.
	if (nEndPosition < 0) return;

	//�׿� ��->�� ������ġ ���·� ���ո�縦 �и��Ѵ�.
	//�űԻ�����ʹ� A:�ű� B:�� --> �űԻ�� + ���ʹ����� �м�
	

	strSource = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, nStartPosition, nEndPosition);
	if (strSource != NULL)
	{
		nEndPosition = strlen(strSource) - 1;
	}
	//��->�� ������ġ�� � ���� Ȯ��
	//nStartPosition�� �̹� ������ ����. �ش� �Լ��� ȣ���ϴ� �Լ����� �̹� start��end�� ������
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
					InsertAddCompoundSubTail(pNode, pNode->pHToken->subEnding->strResult, DIC_ENDING, INSERT_RIGHT_TO_LEFT);//���վ������ 
				}
			}
			
			if (bFind4 == true)
			{
				InsertAddCompoundSubTail(pNode, pWord4, DIC_ENDING, INSERT_RIGHT_TO_LEFT);//���վ������ 
				i -= (strlen(pWord4) - 1);
				pNode->pHToken->nLastFindEndingSyllable = i;//��������ã����ġ
				if (bCompoundEndSyllable == false) bCompoundEndSyllable = true;
			}
			else if (bFind3 == true)
			{
				InsertAddCompoundSubTail(pNode, pWord3, DIC_ENDING, INSERT_RIGHT_TO_LEFT);//���վ������ 
				i -= (strlen(pWord3) - 1);
				pNode->pHToken->nLastFindEndingSyllable = i;//��������ã����ġ
				if (bCompoundEndSyllable == false) bCompoundEndSyllable = true;
			}
			else if (bFind2 == true)
			{
				InsertAddCompoundSubTail(pNode, pWord2, DIC_ENDING, INSERT_RIGHT_TO_LEFT);//���վ������ 
				i -= (strlen(pWord2) - 1);
				pNode->pHToken->nLastFindEndingSyllable = i;//��������ã����ġ
				if (bCompoundEndSyllable == false) bCompoundEndSyllable = true;
			}
		}
		if (bFind4 == false && bFind3 == false && bFind2 == false)
		{
			break;
		}
	}


	//���վ�̺м��� ��� �������� �м��� ��� ������ �ϳ��� ���� subEnding->strResult�� �ٽ� �����Ѵ�.
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



//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
//���վ���� �߰�
//���ո���� ��� ��->�� ������ �����Ѵ�.
//���þ�ε��� --> ���þ��+����
//���ո�� �м� ��, B�� ������ ���ո�縦 ã�� �� �ش� ���� ���� �� �м��� A�� ��ü ������ �� B�� ������ ���ո�簡 ���������(Prp:�������üũ) 
//B�� ������ �������� B�� ��̰� �Ǿ�� �Ѵ�.(Prp:�������üũ���� �߰� 2018-01-11)
//�׽�Ʈ����� --> �׽�Ʈ + �� --> �׽�Ʈ + �����(�������) : ������簡 �������� ��쿡�� �տ� ������ �� �� ����. Prp:�������üũ
//�׽�Ʈ����μ��� --> �׽�Ʈ --> �׽�Ʈ + ����� + ���� : ����δ� �������� ���� �������� ����η� �м��Ǿ�� �Ѵ�.(������簡 �������� ���� ����) :
//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
void CMorphoLogicalAnalysis::CompoundNounDecomposition(H_NODE *pNode, int nStartPosition, int nEndPosition, int nCompoundType)
{
	bool bInsertCompoundNoun = false;
	bool bMaternalInclusionAnalysis = false;//��̰� 1������ ��� ��������ؼ� ���ո�縦 �����Ѵ�.
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
	
	//�������������� : 1���� : 970 2���� : 22, 720 3���� : 29, 132 4���� : 9, 941 5���� : 2, 750 
	//2, 3, 4������ ��κ��� ������ ���������� ���ո�� �̵�Ͼ� �м� ������ 2,3,4������ ������� �Ѵ�.
	if (nEndPosition < 0) return;

	
	//A+B�� ��ü �����̸鼭, B�� 2���� �̻��� ��쿡�� ���ո�縦 �м����� �ʴ´�.	
	if (pNode->pHToken->subStem->strResult != NULL && pNode->pHToken->subEnding->strResult != NULL && strlen(pNode->pHToken->subEnding->strResult) > 2)
	{
		if (strlen(pNode->pHToken->strSource) == strlen(pNode->pHToken->subStem->strResult) + strlen(pNode->pHToken->subEnding->strResult))
			return;
	}
	
	
	//��̰� 1���� �� ���, ��� ��ü�� �����Ͽ� ��->�� ������ġ ���·� ���ո�縦 �и��Ѵ�.
	//�׿� ��->�� ������ġ ���·� ���ո�縦 �и��Ѵ�.
	//�����Ҽ���(B:�� ����(���ո����), �Ҽ�(���ո����)),�����������(B:�� ���(���ո����),����(���ո����)) ���� ���ӵ� ���� ���� �� B-1���� �м��Ѵ�)
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
	//��->�� ������ġ�� � ���� Ȯ��
	//nStartPosition�� �̹� ������ ����. �ش� �Լ��� ȣ���ϴ� �Լ����� �̹� start��end�� ������
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
			//�̺м� �� ������ �� �м��� A�� ���̰� ��ü���ڿ��� ���� ��� �̺м���ü������ ������ Ȯ���Ѵ�.
			if (pNode->pHToken->subStem != NULL && pNode->pHToken->subStem->strResult != NULL && (strlen(pNode->pHToken->strSource) == strlen(pNode->pHToken->subStem->strResult) + strlen(pWord5)))
			{
				m_pStemEndDic->StemGetRecord(pWord5, (char *)Feature_Stem);
				//�̺м� ������ ������� ���� �� ��� �տ� �ٸ� �����̳� ������ �� �� ����.
				if (Feature_Stem[STEM_DIC_N] == '1' && Feature_Stem[STEM_DIC_PRP] == '1')
				{
					if (bInsertCompoundNoun == false) bInsertCompoundNoun = true;
					break;
				}
			}
			InsertAddCompoundSubTail(pNode, pWord5, DIC_STEM, INSERT_RIGHT_TO_LEFT);
			i -= (strlen(pWord5) - 1);
			pNode->pHToken->nLastFindCompoundNoun = i;//���ո��ã����ġ
			if (bInsertCompoundNoun == false) bInsertCompoundNoun = true;
		}
		else if (bFind4 == true)
		{
			//�̺м� �� ������ �� �м��� A�� ���̰� ��ü���ڿ��� ���� ��� �̺м���ü������ ������ Ȯ���Ѵ�.
			if (pNode->pHToken->subStem != NULL && pNode->pHToken->subStem->strResult != NULL && (strlen(pNode->pHToken->strSource) == strlen(pNode->pHToken->subStem->strResult) + strlen(pWord4)))
			{
				m_pStemEndDic->StemGetRecord(pWord4, (char *)Feature_Stem);
				//�̺м� ������ ������� ���� �� ��� �տ� �ٸ� �����̳� ������ �� �� ����.
				if (Feature_Stem[STEM_DIC_N] == '1' && Feature_Stem[STEM_DIC_PRP] == '1')
				{
					if (bInsertCompoundNoun == false) bInsertCompoundNoun = true;
					break;
				}
			}
			InsertAddCompoundSubTail(pNode, pWord4, DIC_STEM, INSERT_RIGHT_TO_LEFT);
			i -= (strlen(pWord4) - 1);
			pNode->pHToken->nLastFindCompoundNoun = i;//���ո��ã����ġ
			if (bInsertCompoundNoun == false) bInsertCompoundNoun = true;
		}
		else if (bFind3 == true)
		{
			//�̺м� �� ������ �� �м��� A�� ���̰� ��ü���ڿ��� ���� ��� �̺м���ü������ ������ Ȯ���Ѵ�.
			if (pNode->pHToken->subStem != NULL && pNode->pHToken->subStem->strResult  != NULL && (strlen(pNode->pHToken->strSource) == strlen(pNode->pHToken->subStem->strResult) + strlen(pWord3)))
			{
				m_pStemEndDic->StemGetRecord(pWord3, (char *)Feature_Stem);
				//�̺м� ������ ������� ���� �� ��� �տ� �ٸ� �����̳� ������ �� �� ����.
				if (Feature_Stem[STEM_DIC_N] == '1' && Feature_Stem[STEM_DIC_PRP] == '1')
				{
					if (bInsertCompoundNoun == false) bInsertCompoundNoun = true;
					break;
				}
			}
			InsertAddCompoundSubTail(pNode, pWord3, DIC_STEM, INSERT_RIGHT_TO_LEFT);
			i -= (strlen(pWord3) - 1);
			pNode->pHToken->nLastFindCompoundNoun = i;//���ո��ã����ġ
			if(bInsertCompoundNoun == false) bInsertCompoundNoun = true;
		}
		else if (bFind2 == true)
		{
			//�̺м� �� ������ �� �м��� A�� ���̰� ��ü���ڿ��� ���� ��� �̺м���ü������ ������ Ȯ���Ѵ�.
			if (pNode->pHToken->subStem != NULL && pNode->pHToken->subStem->strResult != NULL && (strlen(pNode->pHToken->strSource) == strlen(pNode->pHToken->subStem->strResult) + strlen(pWord2)))
			{
				m_pStemEndDic->StemGetRecord(pWord2, (char *)Feature_Stem);
				//�̺м� ������ ������� ���� �� ��� �տ� �ٸ� �����̳� ������ �� �� ����.
				if (Feature_Stem[STEM_DIC_N] == '1' && Feature_Stem[STEM_DIC_PRP] == '1')
				{
					if (bInsertCompoundNoun == false) bInsertCompoundNoun = true;
					break;
				}
			}
			InsertAddCompoundSubTail(pNode, pWord2, DIC_STEM, INSERT_RIGHT_TO_LEFT);
			i -= (strlen(pWord2) - 1);
			pNode->pHToken->nLastFindCompoundNoun = i;//���ո��ã����ġ
			if (bInsertCompoundNoun == false) bInsertCompoundNoun = true;
		}
		else if (bFind1 == true){
			//ù LOOP������ bFind5,bFind4,bFind3,bFind2�� ���� ������ ���� �ÿ��� B�� �����ϰ� �ٽ� ���վ �м��Ѵ�.
			//1������ ���ؼ��� ���ۿ� ���� ���
			if (bMaternalInclusionAnalysis == true)
			{	
				//nCompoundType == 2 , A��B�� ��ü������ ���� ��� A+B == ��ü����
				//���ո��м��� ���̻� ���� �ʴ´�.
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
				pNode->pHToken->nLastFindCompoundNoun = i;//���ո��ã����ġ
				if (bInsertCompoundNoun == false) bInsertCompoundNoun = true;
			}
		}
				
		//B�� 1������ �� B�� �����Ͽ� 5,4,3,2������ ���� �� ���ۻ����� ������ �� B�� ���� ó���Ѵ�.
		//���ۻ����� ã�� ������ N������ ���� ��� ���� ���� �ʴ´�.

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
				//��̸� ������ ������ ���ۻ����� �����ϱ� ������ ��������� ���� �ʰ� �ٷ� ��������� ���� ó���Ѵ�.
				//if(Feature_Stem[STEM_DIC_N] == '1')
				if(isExist == true && pNode->pHToken->subEnding != NULL)
				{
					//���B�� �м��� 1������ ���� ó���Ѵ�.
					DeleteHTokenResult(pNode, DIC_ENDING);
				}
				else
				{
					//B�� �����ؼ� ã�� ���� ���.. A+1~B-1��  �ش��ϴ� ���ո�縦 �м��Ѵ�.
					nEndPosition = pNode->pHToken->subEnding->nFindPos - 1;
					CHAR_DELETE(strSource);
					strSource = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, nStartPosition, nEndPosition);
					if (strSource != NULL)
					{
						//B���� ������ ���󺹱� ��, �ش� ������ A�� ���� ��� ���̻� �м����� �ʴ´�.
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
				//B�� �����ؼ� ã�� ���� ���.. A+1~B-1��  �ش��ϴ� ���ո�縦 �м��Ѵ�.
				nEndPosition = pNode->pHToken->subEnding->nFindPos - 1;
				CHAR_DELETE(strSource);
				strSource = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, nStartPosition, nEndPosition);
				if (strSource != NULL)
				{
					//B���� ������ ���󺹱� ��, �ش� ������ A�� ���� ��� ���̻� �м����� �ʴ´�.
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
	//��->�� ���ո�� �м�
	if (bInsertCompoundNoun == true)
	{
		AfterModule_01(pNode);
	}
	else
	{
		//A,B���̿� ���ո�� ���·� �м��� �ȵ� ���
		//B-1�� ������ ������ ������ ����ó�����(��,��,��,��,��)�� �ٽ� �ѹ� Ȯ���Ѵ�.
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

//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
//���վ������ �߰�
//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
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



//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
//�ش����� ������ ��� �ʱ�ȭ �Ѵ�	
//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
void CMorphoLogicalAnalysis::AnalyzeNodeInitialization(H_NODE *pNode)
{
	if (pNode != NULL)
	{
		if (pNode->pHToken->bSubCompCreate == true)
		{
			DeleteSubHTokenLinkedList(pNode->pHToken, "STEM");
			pNode->pHToken->bSubCompCreate = false;
		}
		//���վ�� ����
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
	//���ս��ۻ���
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
	//���տ�������
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

//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
//���Ǽ� ó��
//��������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
bool CMorphoLogicalAnalysis::CreateAmbiScript()//���Ǽ��м�
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

				//�ӽ������� �ּ� ó����.(����ó���� �����ϴ� �۾��� ��...)
				//RestorIrrigual(m_pNode);
				pNode = pNode->pNext;
				m_nListPosition -= 1;
			}
			else if (isStemAmbCheck == true && isEndingAmbCheck == false)
			{
				m_pAmbiguity->GoAmbiguityFunction(this, pNode, 2);
				//�ӽ������� �ּ� ó����.(����ó���� �����ϴ� �۾��� ��...)
				//RestorIrrigual(m_pNode);
				pNode = pNode->pNext;
				m_nListPosition -= 1;
			}
			else if (isStemAmbCheck == false && isEndingAmbCheck == true)
			{
				m_pAmbiguity->GoAmbiguityFunction(this, pNode, 1);

				//�ӽ������� �ּ� ó����.(����ó���� �����ϴ� �۾��� ��...)
				//RestorIrrigual(m_pNode);
				pNode = pNode->pNext;
				m_nListPosition -= 1;
			}
			else
			{
				//�ӽ������� �ּ� ó����.(����ó���� �����ϴ� �۾��� ��...)
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