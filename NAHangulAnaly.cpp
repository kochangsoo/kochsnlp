#include "NAHangulAnaly.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNAHangulAnaly::CNAHangulAnaly()
{
	m_LinkedList = NULL;
	m_pConvert = new CConvert;
	m_pAmbiguity = new CNAAmbiguity;
	m_pCNAHangulUtil = new CNAHangulUtil();
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
	if (m_pCNAHangulUtil != NULL)
	{
		delete m_pCNAHangulUtil;
		m_pCNAHangulUtil = NULL;
	}
}

void CNAHangulAnaly::SaveLogs(H_NODE *pNode,char* pCallFunc)
{
	#ifdef _DEBUG
		if(pNode != NULL){
			strcat(pNode->pHToken->strFlowChartDebug, pCallFunc);
		}
	#endif
}

bool CNAHangulAnaly::IsExistStemDic(H_NODE* pNode, char *pStemWord)
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

//���վ���� �߰�
bool CNAHangulAnaly::IsExistStemDic(H_SUBNODE* pNode, char *pStemWord)
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

bool CNAHangulAnaly::IsExistEndingDic(H_NODE* pNode, char *pEndingWord)
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

//���վ������ �߰�
bool CNAHangulAnaly::IsExistEndingDic(H_SUBNODE* pNode, char *pEndingOneSyllable)
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

bool CNAHangulAnaly::IsExistEndingDic(char *pEndingOneSyllable)
{
	bool isExist = false;
	isExist = m_pStemEndDic->EndFindIdx(pEndingOneSyllable, FIND_REVERSE);
	return isExist;
}


bool CNAHangulAnaly::IsExistStemDic(char *IsExistStemDic, char *pSaveBuffer)
{
	bool isExist = false;
	return isExist;
}

bool CNAHangulAnaly::IsExistStemDic(char *pStemSeveralSyllables)
{
	bool isExist = false;
	isExist = m_pStemEndDic->StemFindIdx(pStemSeveralSyllables);
	return isExist;
}


void CNAHangulAnaly::StartHangulAnaly()
{
	H_NODE* pNode;
	char* pFeature = NULL;
	pNode = m_LinkedList->m_pHeadNode->pNext;
	

	while (pNode != m_LinkedList->m_pTailNode)
	{
		if (pNode->pHToken != NULL && pNode->pHToken->strSource != NULL)
		{
			//NAHangulUtilTesting(pNode);//�ѱ���ƿ�׽�Ʈ �Լ�
			//---------------------------------------------------------------------------------------------------------------------------
			//�ѱ��� ��� ���ڿ��� 2Byte�̹Ƿ� 15���� ���ϸ� �м��Ѵ�.
			//---------------------------------------------------------------------------------------------------------------------------
			//if(strlen(pNode->pHToken->strSource) <= 20)
			if(m_pCNAHangulUtil->GetFirstToEndSyllableCount(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource) - 1) <= 15)
			{
				#ifdef _DEBUG
					memset(pNode->pHToken->strFlowChartDebug, 0, 1024);
					memset(pNode->pHToken->strAnalysisError, 0, 1024);
				#endif
					
				//---------------------------------------------------------------------------------------------------------------------------
				//��ü������ �������� ã�´�.
				//---------------------------------------------------------------------------------------------------------------------------
				m_strLog = "��ü���� '";
				m_strLog += pNode->pHToken->strSource;
				m_strLog += "' ����Ž��";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
				
				if(FindTheWholeWord(pNode))
				{
					pNode->pHToken->nKindError = 0;
					//�м��Ϸ�
					m_strLog = "\r\n��ü���� ������ ����. �м��Ϸ�(����).";
					SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
				}
				else
				{
					pNode->pHToken->nKindError = -999;
					m_strLog = "\r\n��ü���� ����� Ž�� ����";
					SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
					
					//---------------------------------------------------------------------------------------------------------------------------
					//������ġ�� �,��� ã��
					//---------------------------------------------------------------------------------------------------------------------------
					m_strLog = "\r\nA(�),B(���)�� ������ġ ���� ã��";
					SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
					LongestMatchStemAndEndOfWordAnalysis(pNode);
				}
			}
			else
			{
				//�м�����
				//�м���� ������ 15���� �̻�
				pNode->pHToken->nKindError = -999;
			}
		}

		pNode = pNode->pNext;
	}
	// ������ �м�
	CreateAmbiScript();
}




//�,��̸�� �����ϴ� ���
void CNAHangulAnaly::IsStemEndingExistSyllable(H_NODE* pNode)
{
	char* pOneSyllable = NULL;
	char* pCombineAllString = NULL;
	char* pChoJungChar = NULL;
	char* pJongChar = NULL;
	char* pCandidateSyllableEndOfWord = NULL;
	char* pAddTwoEndOfWord = NULL;
	char* pCandidateStemSyllableWord = NULL;
	char* pChangeLastCharSyllable = NULL;
	char* pChangeLastChar = NULL;
	char* pAddTwoStmeEndOfWord = NULL;//�+���
	char* pFirstOrLastOneChar = NULL;//���ڿ��� ù �Ǵ� ������ ����
	char* pStartToEndWord = NULL;

	bool pAllFindSyllable = false;
	bool bFindStem = false;
	bool bFindEnding = false;

	int nPos = 0;
	int nStart = 0;
	int nEnd = 0;

	m_strLog = "�,��̸�� �����ϴ� ���";
	m_strLog += "\r\nA+1~B-1 ������ ������ ��̻�����ȸ";
	SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
	//A~B������ ��� ���� ��̻��� ���� ���� Ȯ��
	pAllFindSyllable = ExistAllEtcWordEndOfWordAnalysis(pNode);
	//---------------------------------------------------------------------------------------------------------------------------
	//��� ������ ��� ������ ���� ��� �ش� ������+B(������ġ���)�� ���� ��̷� �м��Ѵ�.
	//���������� ���������� �����ؾ� �Ѵ�.
	//---------------------------------------------------------------------------------------------------------------------------
	if (pAllFindSyllable == true)
	{
		m_strLog = "\r\n�� ���� ��̻����� ��� ����";
		SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
		//---------------------------------------------------------------------------------------------------------------------------
		//A+1~B-1�� ���� + B(������ġ���)�� ���� �ϳ��� ��̷� �����Ѵ�.
		//---------------------------------------------------------------------------------------------------------------------------
		//pCombineAllString = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, pNode->pHToken->subStem->nFindPos + 1, pNode->pHToken->subEnding->nFindPos - 1);
		pCombineAllString = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, pNode->pHToken->subStem->nFindPos + 1, strlen(pNode->pHToken->strSource));
		if(pCombineAllString != NULL && strcmp(pCombineAllString,"") != 0)
		{
			m_strLog = "\r\n(A+1)~B�� ��̷� ����";
			DeleteSubTokenResult(pNode, DIC_ENDING);//���� ��� ��� ���� ����
			pNode->pHToken->subEnding->strResult = new char[strlen(pCombineAllString) + 1];
			memset(pNode->pHToken->subEnding->strResult, 0x00, strlen(pCombineAllString) + 1);
			strcpy(pNode->pHToken->subEnding->strResult, pCombineAllString);
			CHAR_DELETE(pCombineAllString);

			m_strLog = "\r\nA : ";
			m_strLog += pNode->pHToken->subStem->strResult;
			m_strLog += "\r\nB : ";
			m_strLog += pNode->pHToken->subEnding->strResult;
			SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

			m_LinkedList->CreateCompoundSubNode(pNode, "ENDING");
			/*
			nStart = pNode->pHToken->subEnding->nFindPos - 1;
			nEnd = pNode->pHToken->subStem->nFindPos;
			*/
			nStart = strlen(pNode->pHToken->strSource)-1;
			nEnd = pNode->pHToken->subStem->nFindPos;
			
			m_strLog = "\r\n(A+1)~B ������ ������ ������ ���վ�̷� �߰�";
			SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
			
			for (nStart; nStart>nEnd; nStart--)
			{
				pOneSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nStart, DIC_ENDING);
				m_strLog = "\r\n�������� : '";
				m_strLog += pOneSyllable;
				m_strLog += "'";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
				if (strlen(pOneSyllable) == 2) nStart = nStart - 1;//�ѱ��� ���
				//---------------------------------------------------------------------------------------------------------------------------
				//A~B������  �� �������� ������ ���ε��� ��������� �����Ѵ�.
				//---------------------------------------------------------------------------------------------------------------------------
				InsertAddCompoundSubTail(pNode, pOneSyllable, DIC_ENDING);//���վ������ 
				CHAR_DELETE(pOneSyllable);
			}
			m_strLog = "\r\n�м��Ϸ�(����).";
			SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
		}
		else
		{
			pNode->pHToken->nKindError = -999;
			m_strLog = "\r\n(A+1)~B������ �� ���� ��̻����� ������";
			m_strLog = "\r\n�м��Ϸ�(����).";
			SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

			pNode->pHToken->nKindError = 0;
		}
	}
	else
	{
		//---------------------------------------------------------------------------------------------------------------------------
		//(A+1~B)���� � �������� Ž��
		//������ ��쿡 ���վ�� ó���Ѵ�.
		//---------------------------------------------------------------------------------------------------------------------------
		bFindStem = false;
		//�ӵ���--> �� + ���� : �ӵ� + ���� ���·� �м��Ǿ����.
		if (pNode->pHToken->subEnding->nFindPos - pNode->pHToken->subStem->nFindPos == 1)
		{
			nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, pNode->pHToken->subEnding->nFindPos, DIC_STEM);
			pStartToEndWord = m_pCNAHangulUtil->GetLeftToRightWord(pNode->pHToken->strSource, nPos);
			bFindStem = IsExistStemDic(pNode, pStartToEndWord);
			if(bFindStem == true)
			{
				CHAR_DELETE(pStartToEndWord);
				pStartToEndWord= m_pCNAHangulUtil->GetRightToLeftWord(pNode->pHToken->strSource, nPos+1);
				bFindEnding = IsExistEndingDic(pNode, pStartToEndWord);
			}
		}
		else
		{
			//���þ�ε��� �:���� + ���:�� 
			if(pNode->pHToken->subEnding->nFindPos - pNode->pHToken->subStem->nFindPos >= 2)
			{
				pStartToEndWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, pNode->pHToken->subStem->nFindPos + 1, strlen(pNode->pHToken->strSource)-1);
				m_strLog = "\r\n(A+1~B) ���� ���� Ž�� : '";
				m_strLog += pStartToEndWord;
				m_strLog += "'";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
				bFindStem = IsExistStemDic(pStartToEndWord);
				CHAR_DELETE(pStartToEndWord);
			}
			if(bFindStem == true)
			{
				m_strLog = "\r\n���� ������ ���� - ���ո��� ������";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
				//���ո��� �����Ͽ� ���������� �ش��ϴ� ������ �����Ѵ�.
				CHAR_DELETE(pNode->pHToken->subEnding->strResult);
				CHAR_DELETE(pNode->pHToken->subEnding->strFeature);
				m_LinkedList->CreateCompoundSubNode(pNode, "STEM");
				CompoundNounDecomposition(pNode, strlen(pNode->pHToken->strSource)-1);//���ո�����
				//����Ʈ���� �� �����Ѵ�. N��°�� ������ ������  N+1��°�� ù������ ���� ��, N+1��° ù ���� ���� ó��
				UnRegisteredCompoundNounWord(pNode);
			}
			else
			{
				//---------------------------------------------------------------------------------------------------------------------------
				//(A+1)�� ��̻����� ���� ���
				//---------------------------------------------------------------------------------------------------------------------------
				//�����
				//A+1~B-1������ ���� �� �ľ�(B���� A+1~B-1������ ������ ��� ������ �������� ���� ��쵵 ������: '�����'���� '��'(�) '��'(���)�� �м� �� '��'�� ��̻����� ���� )
				if(pNode->pHToken->nLastFindEndingSyllable == 0)
				{
					pNode->pHToken->nLastFindEndingSyllable = pNode->pHToken->subEnding->nFindPos;
				}
				//�������� �ǹ�
				if ((pNode->pHToken->nLastFindEndingSyllable - 1) - pNode->pHToken->subStem->nFindPos > 0 && (pNode->pHToken->nLastFindEndingSyllable -1) - pNode->pHToken->subStem->nFindPos <= 2)
				{
					m_strLog = "\r\n(A+1)�� ��̻����� ���� ���";
					SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

					pOneSyllable = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, pNode->pHToken->subStem->nFindPos + 1, pNode->pHToken->nLastFindEndingSyllable - 1);
			
					m_strLog = "\r\n(A+1)~(B-1)���̿� �Ѱ��� ������ ���� : ";
					m_strLog += pOneSyllable;
					SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
	
					//�ϳ��� ������ ��̻��� �м� �ÿ� �������� �ʾ��� ���
					if (pOneSyllable  != NULL && strlen(pOneSyllable) <= 2)
					{	
						//�ش�����(A+1)�� ����,���� �и�
						pChoJungChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pOneSyllable, CHAR_CHOJUNGSUNG);
						pJongChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pOneSyllable, CHAR_JONGSUNG);

						m_strLog = "\r\n(A+1)������ ��,��,�����и�";
						m_strLog += "\r\n���߼� : '";
						m_strLog += pChoJungChar;
						m_strLog += "'";
						m_strLog += "\r\n���� : '";
						m_strLog += pJongChar;
						m_strLog += "'";
						SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

						//��,��,��,��,���� ���
						if (strcmp(pJongChar, "��") == 0 || strcmp(pJongChar, "��") == 0 || strcmp(pJongChar, "��") == 0 || strcmp(pJongChar, "��") == 0 || strcmp(pJongChar, "��") == 0)
						{
							m_strLog = "\r\n(A+1)������ ������ '��','��','��','��','��'�� ���";
							SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

							//���� ��� - ���ܻ�Ȳ1
							if (strcmp(pChoJungChar, "��") == 0 || strcmp(pChoJungChar, "��") == 0)
							{
								m_strLog = "\r\n������ '��' �Ǵ� '��'�� ����  ���ܻ�Ȳ(������ '��'�� ���)";
								SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

								if (strcmp(pChoJungChar, "��") == 0)
								{
									//pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->subStem->strResult, "��", CHANGE_CHAR_SYLLABLE_LAST);
									pChangeLastChar = m_pCNAHangulUtil->GetCombineTwoStrings( pNode->pHToken->subStem->strResult, "��");
								}
								else if (strcmp(pChoJungChar, "��") == 0)
								{
									//pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->subStem->strResult, "��", CHANGE_CHAR_SYLLABLE_LAST);
									pChangeLastChar = m_pCNAHangulUtil->GetCombineTwoStrings(pNode->pHToken->subStem->strResult, "��");
								}
								pAddTwoEndOfWord = m_pCNAHangulUtil->GetCombineTwoStrings("��", pNode->pHToken->subEnding->strResult);
								pAddTwoStmeEndOfWord = m_pCNAHangulUtil->GetCombineTwoStrings(pChangeLastChar, pAddTwoEndOfWord);
								
								//���ߴ� �:�� ���:�� ���Ͼ��� ���·� �ٽ� �м��� ���� �Ѵ�.
								CHAR_DELETE(pNode->pHToken->strSource);
								pNode->pHToken->strSource = new char[strlen(pAddTwoStmeEndOfWord) + 1];
								memset(pNode->pHToken->strSource, 0x00, strlen(pAddTwoStmeEndOfWord) + 1);
								strcpy(pNode->pHToken->strSource, pAddTwoStmeEndOfWord);
								
								CHAR_DELETE(pAddTwoEndOfWord);
								CHAR_DELETE(pAddTwoStmeEndOfWord);

								LongestMatchStemAndEndOfWordAnalysis(pNode);
							}
							//���� ��� - ���ܻ�Ȳ2
							else	if (strcmp(pChoJungChar, "��") == 0 || strcmp(pChoJungChar, "��") == 0)
							{
								m_strLog = "\r\n������ '��' �Ǵ� '��'�� ����  ���ܻ�Ȳ(������ '��'�� ���)";
								SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

								CHAR_DELETE(pChoJungChar);
								CHAR_DELETE(pJongChar);
								pFirstOrLastOneChar = m_pCNAHangulUtil->GetFirstOrLastOneChar(pNode->pHToken->subStem->strResult, DIC_STEM);
								pChoJungChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pFirstOrLastOneChar, CHAR_CHOJUNGSUNG);
								pJongChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pFirstOrLastOneChar, CHAR_JONGSUNG);
								pChangeLastChar = m_pCNAHangulUtil->ChangeCompulsionJongSyllable(pChoJungChar, "��");
								pChangeLastCharSyllable = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->subStem->strResult, pChangeLastChar, CHANGE_CHAR_SYLLABLE_LAST);//�ش� ��ġ�� ���� ���� //nPos(CHANGE_CHAR_SYLLABLE_FIRST,CHANGE_CHAR_SYLLABLE_LAST)�� ���� ������ ó�� �Ǵ� �������� ����
								pAddTwoEndOfWord = m_pCNAHangulUtil->GetCombineTwoStrings("��", pNode->pHToken->subEnding->strResult);

								pAddTwoStmeEndOfWord = m_pCNAHangulUtil->GetCombineTwoStrings(pChangeLastCharSyllable, pAddTwoEndOfWord);

								m_strLog = "\r\n�����ڿ��� ������ ������ ���� ���� :";
								m_strLog += "'";
								m_strLog += pNode->pHToken->strSource;
								m_strLog += "' ���� ";
								m_strLog += "'";
								m_strLog += pAddTwoStmeEndOfWord;
								m_strLog += "'���� ���� �� ������ġ �����Լ��� �ٽ� ȣ���Ѵ�.";
								SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
						
								CHAR_DELETE(pNode->pHToken->strSource);
								pNode->pHToken->strSource = new char[strlen(pAddTwoStmeEndOfWord) + 1];
								memset(pNode->pHToken->strSource, 0x00, strlen(pAddTwoStmeEndOfWord) + 1);
								strcpy(pNode->pHToken->strSource, pAddTwoStmeEndOfWord);

								LongestMatchStemAndEndOfWordAnalysis(pNode);
							}
							else
							{
								//(A+2)~(B-1) + B(������ġ���)
								pCandidateSyllableEndOfWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, pNode->pHToken->nLastFindEndingSyllable, strlen(pNode->pHToken->strSource) - 1);
								pAddTwoEndOfWord = m_pCNAHangulUtil->GetCombineTwoStrings(pJongChar, pCandidateSyllableEndOfWord);

								m_strLog = "\r\n(A+1)�� ���� + B = '";
								m_strLog += pAddTwoEndOfWord;
								m_strLog += "'";
								SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

								//(0~(A+1))������ ����(�����Ȯ��)
								pCandidateStemSyllableWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, pNode->pHToken->nLastFindEndingSyllable - 1);
								//���ڿ��� ������ ������ ���߼����κ���(A + (A+1)) :A+1�� ��� ���߼���
								pChangeLastCharSyllable = m_pCNAHangulUtil->SetChangeSyllableWord(pCandidateStemSyllableWord, pChoJungChar, CHANGE_CHAR_SYLLABLE_LAST);

								m_strLog = "\r\nA~(A+1)�� ���߼� = '";
								m_strLog += pChangeLastCharSyllable;
								m_strLog += "'";
								m_strLog += "\r\nA~(A+1)�� ���߼��� �ش��ϴ� �,(A+1)�� ����+B�� �ش��ϴ� ��� ���� ��ȸ";
								SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

								bFindStem = IsExistStemDic(pNode, pChangeLastCharSyllable);
								bFindEnding = IsExistEndingDic(pNode, pAddTwoEndOfWord);

								if (bFindStem == true && bFindEnding == true)
								{
									m_strLog = "\r\n��� ����(����)";
									SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
								}
								else
								{
									//A~A+1�� ���߼��� �ش��ϴ� ��� ������ �ְ�, ������ '��'�� ��� ����ó���Ѵ�.
									if (strcmp(pJongChar, "��") == 0)
									{
										m_strLog = "\r\nA~(A+1�� ���߼�) ������� ";
										if (bFindStem == false){m_strLog += "����";}else{m_strLog += "������"; }
										m_strLog += ",(A+1������)+B ��̻����� ";
										if (bFindEnding == false) { m_strLog += "����"; }else { m_strLog += "������"; }
										m_strLog += ",������ '��' �� ��� ���� ó��";
										SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

										m_LinkedList->CreateCompoundSubNode(pNode, "ENDING");
								
										DeleteSubTokenResult(pNode, DIC_STEM);//���� � ��� ���� ����
										pNode->pHToken->subStem->strResult = new char[strlen(pChangeLastCharSyllable) + 1];
										memset(pNode->pHToken->subStem->strResult, 0x00, strlen(pChangeLastCharSyllable) + 1);
										strcpy(pNode->pHToken->subStem->strResult, pChangeLastCharSyllable);

										//��м��� ��̸� ���վ�̿� �ִ´�.
										InsertAddCompoundSubTail(pNode, pNode->pHToken->subEnding->strResult, DIC_ENDING);//���վ������ 
										InsertAddCompoundSubTail(pNode, pJongChar, DIC_ENDING);//���վ������ 
										m_strLog = "\r\n�������� : '";
										m_strLog += pNode->pHToken->subEnding->strResult;
										m_strLog = "'";
										m_strLog += "\r\n�������� : '";
										m_strLog += pJongChar;
										m_strLog = "'";
										SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
								
										//'��'+��� ������ ��������� ������ �����.
										DeleteSubTokenResult(pNode, DIC_ENDING);//���� ��� ��� ���� ����
										pNode->pHToken->subEnding->strResult = new char[strlen(pAddTwoEndOfWord) + 1];
										memset(pNode->pHToken->subEnding->strResult, 0x00, strlen(pAddTwoEndOfWord) + 1);
										strcpy(pNode->pHToken->subEnding->strResult, pAddTwoEndOfWord);
								
										pNode->pHToken->nKindError = 0;
										//�м��Ϸ�
										m_strLog = "\r\n�м��Ϸ�(����).";
										SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
									}
									else
									{
										//Ǯ�̵ȴ� : Ǯ�̵� + ����
										DeleteSubTokenResult(pNode, DIC_STEM);//���� � ��� ���� ����
										pNode->pHToken->subStem->strResult = new char[strlen(pChangeLastCharSyllable) + 1];
										memset(pNode->pHToken->subStem->strResult, 0x00, strlen(pChangeLastCharSyllable) + 1);
										strcpy(pNode->pHToken->subStem->strResult, pChangeLastCharSyllable);

										pNode->pHToken->nKindError = -999;
										m_strLog = "\r\n��� ����(����)";
										SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
										//����ó�� : ���վ�ǥ��  �:�м�����:(���) �м�����:(�����) ���:��(�м�����) ������(�м�����)
										WriteAnalysisError(pNode, pChangeLastCharSyllable, pAddTwoEndOfWord);
									}
								}
							}
						}
						else
						{
							//���ո��� �����Ͽ� ���������� �ش��ϴ� ������ �����Ѵ�.
							m_LinkedList->CreateCompoundSubNode(pNode, "STEM");
							CompoundNounDecomposition(pNode, pNode->pHToken->subEnding->nFindPos - 1);//���ո�����
							 //����Ʈ���� �� �����Ѵ�. N��°�� ������ ������  N+1��°�� ù������ ���� ��, N+1��° ù ���� ���� ó��
							UnRegisteredCompoundNounWord(pNode);
							//���վ�(�̵�Ͼ�)�� ���� �м��� ���� ���� �ܿ�
							if (pNode->pHToken->pSubHeadNode != NULL && pNode->pHToken->pSubHeadNode->pNext == NULL)
							{
								pNode->pHToken->nKindError = -999;
								m_strLog = "\r\n���վ� �Ǵ� �̵�Ͼ ���� �м�(����).";
								SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
							}
							else
							{
								m_strLog = "\r\n���վ� �Ǵ� �̵�Ͼ ���� �м�(����).";
								H_SUBNODE* pStartNode;
								pStartNode = pNode->pHToken->pSubHeadNode->pNext;
								while (pStartNode != pNode->pHToken->pSubTailNode)
								{
									m_strLog += "\r\n ���վ� �Ǵ¹̵�Ͼ� : '";
									m_strLog += pStartNode->pCSubToken->strResult;
									m_strLog += "'";
									SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

									pStartNode = pStartNode->pNext;
								}
							}
						}
					}
				}
				else
				{
					//---------------------------------------------------------------------------------------------------------------------------
					//A~B������ ���� �� A+1�� ��쿡�� ��̻����� ������ ��� �׿ܴ� ���� ó���Ѵ�.
					//'���þ�ε���' ���� ��� '����'+'��'�� �м���('���'�� ���ո��� �м��Ǿ� '���þ��'�� �Ǿ�� �ϸ� '��'�� ��̷� ���ļ� '����'�� �Ǿ�� ��
					//---------------------------------------------------------------------------------------------------------------------------
					//��̺м� ����
					if ((pNode->pHToken->nLastFindEndingSyllable - 1) - pNode->pHToken->subStem->nFindPos > 0)
					{
						//���վ�� �м�
						m_LinkedList->CreateCompoundSubNode(pNode, "ENDING");
						nStart = pNode->pHToken->nLastFindEndingSyllable - 1;
						nEnd = pNode->pHToken->subStem->nFindPos;
						for (nStart; nStart>nEnd; nStart--)
						{
							pOneSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nStart, DIC_ENDING);
							if(IsExistEndingDic(pOneSyllable) == true)
							{
								m_strLog = "\r\n�������� : '";
								m_strLog += pOneSyllable;
								m_strLog += "'";
								SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
								if (strlen(pOneSyllable) == 2) nStart = nStart - 1;//�ѱ��� ���
								pNode->pHToken->subEnding->nFindPos = nStart;
								pNode->pHToken->nLastFindEndingSyllable = nStart;
								//---------------------------------------------------------------------------------------------------------------------------
								//A~B������  �� �������� ������ ���ε��� ��������� �����Ѵ�.
								//---------------------------------------------------------------------------------------------------------------------------
								InsertAddCompoundSubTail(pNode, pOneSyllable, DIC_ENDING);//���վ������ 

								pCandidateSyllableEndOfWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, pNode->pHToken->nLastFindEndingSyllable, strlen(pNode->pHToken->strSource) - 1);
								DeleteSubTokenResult(pNode, DIC_ENDING);//���� ��� ��� ���� ����
								pNode->pHToken->subEnding->strResult = new char[strlen(pCandidateSyllableEndOfWord) + 1];
								memset(pNode->pHToken->subEnding->strResult, 0x00, strlen(pCandidateSyllableEndOfWord) + 1);
								strcpy(pNode->pHToken->subEnding->strResult, pCandidateSyllableEndOfWord);
								CHAR_DELETE(pCandidateSyllableEndOfWord);
							}
							else
							{
								nEnd = nStart;
							}
							CHAR_DELETE(pOneSyllable);
						}

						//���ո��� �����Ͽ� ���������� �ش��ϴ� ������ �����Ѵ�.
						m_LinkedList->CreateCompoundSubNode(pNode, "STEM");
						CompoundNounDecomposition(pNode, pNode->pHToken->subEnding->nFindPos-1);//���ո�����
						//����Ʈ���� �� �����Ѵ�. N��°�� ������ ������  N+1��°�� ù������ ���� ��, N+1��° ù ���� ���� ó��
						UnRegisteredCompoundNounWord(pNode);

						//���վ�(�̵�Ͼ�)�� ���� �м��� ���� ���� �ܿ�
						if (pNode->pHToken->pSubHeadNode != NULL && pNode->pHToken->pSubHeadNode->pNext == NULL)
						{
							pNode->pHToken->nKindError = -999;
							m_strLog = "\r\n���վ� �Ǵ� �̵�Ͼ ���� �м�(����).";
							SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
						}
						else
						{
							m_strLog = "\r\n���վ� �Ǵ� �̵�Ͼ ���� �м�(����).";
							H_SUBNODE* pStartNode;
							pStartNode = pNode->pHToken->pSubHeadNode->pNext;
							while (pStartNode != pNode->pHToken->pSubTailNode)
							{
								m_strLog += "\r\n ���վ� �Ǵ¹̵�Ͼ� : '";
								m_strLog += pStartNode->pCSubToken->strResult;
								m_strLog += "'";
								SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

								pStartNode = pStartNode->pNext;
							}
						}
					}
					else
					{
						m_strLog = "\r\n(A+1)~(B-1)���̿� ������ ����";
						m_strLog += "\r\n�м��Ϸ�(����)";
						SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
					 }
				}
			}
		}
	}
	CHAR_DELETE(pOneSyllable);
	CHAR_DELETE(pCombineAllString);
	CHAR_DELETE(pChoJungChar);
	CHAR_DELETE(pJongChar);
	CHAR_DELETE(pCandidateSyllableEndOfWord);
	CHAR_DELETE(pAddTwoEndOfWord);
	CHAR_DELETE(pCandidateStemSyllableWord);
	CHAR_DELETE(pChangeLastCharSyllable);
	CHAR_DELETE(pChangeLastChar);
	CHAR_DELETE(pAddTwoStmeEndOfWord);
	CHAR_DELETE(pFirstOrLastOneChar);
	CHAR_DELETE(pStartToEndWord);
}

//��� �Ϻθ� �����ϴ� ���
void CNAHangulAnaly::IsOnlyStemExistSyllable(H_NODE* pNode)
{
	//���̻� �м����� �ʰ� �������� ���ո��� ����. 
	char* pOneSyllable = NULL;
	char* pFirstOrLastOneChar = NULL;
	char* pCandidateSyllableEndOfWord = NULL;
	char* pChangeLastChar = NULL;
	char* pAddTwoStmeEndOfWord = NULL;

	int nLength = 0;
	nLength = strlen(pNode->pHToken->strSource);
	nLength = nLength - 1;
	//�м������ ������ ���� ��ȸ
	pFirstOrLastOneChar = m_pCNAHangulUtil->GetFirstOrLastOneChar(pNode->pHToken->strSource, DIC_STEM);
	
	m_strLog = "\r\n��� �Ϻθ� �����ϴ� ���[��̾���]";
	m_strLog += "\r\n�м������ ������ ���� ��ȸ";
	m_strLog += "\r\n������������ '��'���� Ȯ��";
	m_strLog += "\r\n���������� : '";
	m_strLog += pFirstOrLastOneChar;
	m_strLog += "'";
	SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
	
	m_strLog = "\r\n������������ '��'���� Ȯ�� : ";
	m_strLog += "'";
	m_strLog += pFirstOrLastOneChar;
	m_strLog += "'";
	SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

	//�̺м������� 2 �̻��̶�� ����ó��
	if (strcmp(pFirstOrLastOneChar, "��") == 0 && (nLength - (pNode->pHToken->subStem->nFindPos+1) < 2))
	{
		m_strLog = "\r\n'��' �߰� �� '��'�� '�ϰ�'�� �����ϰ� �ٽ� ������ġ�Լ��� ȣ���Ѵ�.";
		//'��'�� '�ϰ�'�� �����Ѵ�
		pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->strSource, "�ϰ�", CHANGE_CHAR_SYLLABLE_LAST);
		CHAR_DELETE(pNode->pHToken->strSource);
		pNode->pHToken->strSource = new char[strlen(pAddTwoStmeEndOfWord) + 1];
		memset(pNode->pHToken->strSource, 0x00, strlen(pAddTwoStmeEndOfWord) + 1);
		strcpy(pNode->pHToken->strSource, pAddTwoStmeEndOfWord);

		LongestMatchStemAndEndOfWordAnalysis(pNode);
	}
	else
	{
		//���ո��� �����Ͽ� ���������� �ش��ϴ� ������ �����Ѵ�.
		m_LinkedList->CreateCompoundSubNode(pNode, "STEM");
		CompoundNounDecomposition(pNode, strlen(pNode->pHToken->strSource)-1);//���ո�����
		//����Ʈ���� �� �����Ѵ�. N��°�� ������ ������  N+1��°�� ù������ ���� ��, N+1��° ù ���� ���� ó��
		UnRegisteredCompoundNounWord(pNode);

		//���վ�(�̵�Ͼ�)�� ���� �м��� ���� ���� �ܿ�
		if (pNode->pHToken->pSubHeadNode != NULL && pNode->pHToken->pSubHeadNode->pNext == NULL)
		{
			pNode->pHToken->nKindError = -999;
			m_strLog = "\r\n���վ� �Ǵ� �̵�Ͼ ���� �м�(����).";
			SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
		}
		else
		{
			m_strLog = "\r\n���վ� �Ǵ� �̵�Ͼ ���� �м�(����).";
			H_SUBNODE* pStartNode;
			pStartNode = pNode->pHToken->pSubHeadNode->pNext;
			while (pStartNode != pNode->pHToken->pSubTailNode)
			{
				m_strLog += "\r\n ���վ� �Ǵ¹̵�Ͼ� : '";
				m_strLog += pStartNode->pCSubToken->strResult;
				m_strLog += "'";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

				pStartNode = pStartNode->pNext;
			}
		}
		/*
		m_strLog = "\r\n'��'�� �ƴϰų� �̺м������� 2 �̻��̶�� ����ó��";
		SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

		pCandidateSyllableEndOfWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, pNode->pHToken->subStem->nFindPos + 1, nLength);
		//�м�����
		//�м���� ���� �� �Ϻθ� ������ ������(���ۺм�����,��̹� ����)
		WriteAnalysisError(pNode, pCandidateSyllableEndOfWord, NULL);
		pNode->pHToken->nKindError = -997; //�м���� ������ ������ �Ϻθ� ����, ��̾��� ���
		*/
	}
	
	CHAR_DELETE(pOneSyllable);
	CHAR_DELETE(pFirstOrLastOneChar);
	CHAR_DELETE(pCandidateSyllableEndOfWord);
	CHAR_DELETE(pChangeLastChar);
	CHAR_DELETE(pAddTwoStmeEndOfWord);
}

//��̸� �����ϴ� ���(�̵�Ͼ� ó�� �� )
void CNAHangulAnaly::IsEndingExistSyllable(H_NODE* pNode)
{
	//���ո��� �����Ͽ� ��̸� ������ ���� B-1���� ������->�������� ������ġ�� �Ǵ� ������� ã�� �� Ž������. ��������
	//0.B-1 �м��� �ȵ� ������ 1������ ���
	//1.����� ���� ���� Ȯ��(N,V�� ���� �ٸ��� ó���Ѵ�)
	char* pOneSyllable = NULL;
	char* pChoJungChar = NULL;
	char* pJongChar = NULL;
	char* pCandidateSyllableEndOfWord = NULL;
	char* pAddTwoEndOfWord = NULL;
	char* pCandidateStemSyllableWord = NULL;
	char* pChangeLastCharSyllable = NULL;
	char* pChangeLastChar = NULL;
	char* pAddTwoStmeEndOfWord = NULL;//�+���
	char* pFirstOrLastOneChar = NULL;//���ڿ��� ù �Ǵ� ������ ����

	int nLen = 0;
	bool bFindStem = false;
	bool bFindEnding = false;

	//����� ���� Ȯ��
	if (pNode->pHToken->subEnding->strFeature[ENDING_DIC_N] == '1' || pNode->pHToken->subEnding->strFeature[ENDING_DIC_AMB] == '1')
	{	
		//���ո��� �����Ͽ� ���������� �ش��ϴ� ������ �����Ѵ�.
		m_LinkedList->CreateCompoundSubNode(pNode, "STEM");
		CompoundNounDecomposition(pNode, pNode->pHToken->subEnding->nFindPos - 1);//���ո�����
		//����Ʈ���� �� �����Ѵ�. N��°�� ������ ������  N+1��°�� ù������ ���� ��, N+1��° ù ���� ���� ó��
		UnRegisteredCompoundNounWord(pNode);

		//���վ�(�̵�Ͼ�)�� ���� �м��� ���� ���� �ܿ�
		if (pNode->pHToken->pSubHeadNode != NULL && pNode->pHToken->pSubHeadNode->pNext == NULL)
		{
			pNode->pHToken->nKindError = -999;
			m_strLog = "���վ� �Ǵ� �̵�Ͼ ���� �м�(����).";
			SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
		}
		else
		{
			m_strLog = "���վ� �Ǵ� �̵�Ͼ ���� �м�(����).";
			H_SUBNODE* pStartNode;
			pStartNode = pNode->pHToken->pSubHeadNode->pNext;
			while (pStartNode != pNode->pHToken->pSubTailNode)
			{
				m_strLog += "\r\n ���վ�Ǵ¹̵�Ͼ� : '";
				m_strLog += pStartNode->pCSubToken->strResult;
				m_strLog += "'";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

				pStartNode = pStartNode->pNext;
			}
		}
	}
	//�ұ�Ģ ���� Ȯ��
	else if (pNode->pHToken->subEnding->strFeature[ENDING_DIC_V] == '1')
	{
		nLen = strlen(pNode->pHToken ->strSource) - strlen(pNode->pHToken->subEnding->strResult);
		//�������� �ǹ�
		if (nLen == 1 || nLen == 2)
		{
			pOneSyllable = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, pNode->pHToken->subEnding->nFindPos - 1);
			m_strLog = "\r\n��� �м� �� �Ѱ��� ������ ���� : ";
			m_strLog += pOneSyllable;
			SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
			//�ϳ��� ������ ��̻��� �м� �ÿ� �������� �ʾ��� ���
			if (pOneSyllable != NULL && strlen(pOneSyllable) <= 2)
			{
				//�ش�����(A+1)�� ����,���� �и�
				pChoJungChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pOneSyllable, CHAR_CHOJUNGSUNG);
				pJongChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pOneSyllable, CHAR_JONGSUNG);

				m_strLog = "\r\n(A+1)������ ��,��,�����и�";
				m_strLog += "\r\n���߼� : '";
				m_strLog += pChoJungChar;
				m_strLog += "'";
				m_strLog += "\r\n���� : '";
				m_strLog += pJongChar;
				m_strLog += "'";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

				//��,��,��,��,���� ���
				if (strcmp(pJongChar, "��") == 0 || strcmp(pJongChar, "��") == 0 || strcmp(pJongChar, "��") == 0 || strcmp(pJongChar, "��") == 0 || strcmp(pJongChar, "��") == 0)
				{
					m_strLog = "\r\n(A+1)������ ������ '��','��','��','��','��'�� ���";
					SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

					//���� ��� - ���ܻ�Ȳ1
					if (strcmp(pChoJungChar, "��") == 0 || strcmp(pChoJungChar, "��") == 0 || strcmp(pChoJungChar, "��") == 0)
					{
						m_strLog = "\r\n������ '��' �Ǵ� '��'�� ����  ���ܻ�Ȳ(������ '��'�� ���)";
						SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

						if (strcmp(pChoJungChar, "��") == 0)
							pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->subStem->strResult, "��", CHANGE_CHAR_SYLLABLE_LAST);
						else if (strcmp(pChoJungChar, "��") == 0)
							pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->subStem->strResult, "��", CHANGE_CHAR_SYLLABLE_LAST);
						else if (strcmp(pChoJungChar, "��") == 0)
							pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->subStem->strResult, "����", CHANGE_CHAR_SYLLABLE_LAST);

						pAddTwoEndOfWord = m_pCNAHangulUtil->GetCombineTwoStrings("��", pNode->pHToken->subEnding->strResult);
						pAddTwoStmeEndOfWord = m_pCNAHangulUtil->GetCombineTwoStrings(pChangeLastChar, pAddTwoEndOfWord);

						CHAR_DELETE(pNode->pHToken->strSource);
						pNode->pHToken->strSource = new char[strlen(pAddTwoStmeEndOfWord) + 1];
						memset(pNode->pHToken->strSource, 0x00, strlen(pAddTwoStmeEndOfWord) + 1);
						strcpy(pNode->pHToken->strSource, pAddTwoStmeEndOfWord);

						LongestMatchStemAndEndOfWordAnalysis(pNode);
					}
					//���� ��� - ���ܻ�Ȳ2
					else	if (strcmp(pChoJungChar, "��") == 0 || strcmp(pChoJungChar, "��") == 0)
					{
						m_strLog = "\r\n������ '��' �Ǵ� '��'�� ����  ���ܻ�Ȳ(������ '��'�� ���)";
						SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

						CHAR_DELETE(pChoJungChar);
						CHAR_DELETE(pJongChar);
						pFirstOrLastOneChar = m_pCNAHangulUtil->GetFirstOrLastOneChar(pNode->pHToken->subStem->strResult, DIC_STEM);
						pChoJungChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pFirstOrLastOneChar, CHAR_CHOJUNGSUNG);
						pJongChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pFirstOrLastOneChar, CHAR_JONGSUNG);
						pChangeLastChar = m_pCNAHangulUtil->ChangeCompulsionJongSyllable(pChoJungChar, "��");
						pChangeLastCharSyllable = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->subStem->strResult, pChangeLastChar, CHANGE_CHAR_SYLLABLE_LAST);//�ش� ��ġ�� ���� ���� //nPos(CHANGE_CHAR_SYLLABLE_FIRST,CHANGE_CHAR_SYLLABLE_LAST)�� ���� ������ ó�� �Ǵ� �������� ����
						pAddTwoEndOfWord = m_pCNAHangulUtil->GetCombineTwoStrings("��", pNode->pHToken->subEnding->strResult);

						pAddTwoStmeEndOfWord = m_pCNAHangulUtil->GetCombineTwoStrings(pChangeLastCharSyllable, pAddTwoEndOfWord);

						m_strLog = "\r\n�����ڿ��� ������ ������ ���� ���� :";
						m_strLog += "'";
						m_strLog += pNode->pHToken->strSource;
						m_strLog += "' ���� ";
						m_strLog += "'";
						m_strLog += pAddTwoStmeEndOfWord;
						m_strLog += "'���� ���� �� ������ġ �����Լ��� �ٽ� ȣ���Ѵ�.";
						SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

						CHAR_DELETE(pNode->pHToken->strSource);
						pNode->pHToken->strSource = new char[strlen(pAddTwoStmeEndOfWord) + 1];
						memset(pNode->pHToken->strSource, 0x00, strlen(pAddTwoStmeEndOfWord) + 1);
						strcpy(pNode->pHToken->strSource, pAddTwoStmeEndOfWord);

						CHAR_DELETE(pOneSyllable);
						CHAR_DELETE(pChoJungChar);
						CHAR_DELETE(pJongChar);
						CHAR_DELETE(pCandidateSyllableEndOfWord);
						CHAR_DELETE(pAddTwoEndOfWord);
						CHAR_DELETE(pCandidateStemSyllableWord);
						CHAR_DELETE(pChangeLastCharSyllable);
						CHAR_DELETE(pChangeLastChar);
						CHAR_DELETE(pAddTwoStmeEndOfWord);
						CHAR_DELETE(pFirstOrLastOneChar);

						LongestMatchStemAndEndOfWordAnalysis(pNode);
					}
					else
					{
						//(A+2)~(B-1) + B(������ġ���)
						pCandidateSyllableEndOfWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, pNode->pHToken->nLastFindEndingSyllable, strlen(pNode->pHToken->strSource) - 1);
						pAddTwoEndOfWord = m_pCNAHangulUtil->GetCombineTwoStrings(pJongChar, pCandidateSyllableEndOfWord);

						m_strLog = "\r\n(A+1)�� ���� + B = '";
						m_strLog += pAddTwoEndOfWord;
						m_strLog += "'";
						SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

						//(0~(A+1))������ ����(�����Ȯ��)
						pCandidateStemSyllableWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, pNode->pHToken->nLastFindEndingSyllable - 1);
						//���ڿ��� ������ ������ ���߼����κ���(A + (A+1)) :A+1�� ��� ���߼���
						pChangeLastCharSyllable = m_pCNAHangulUtil->SetChangeSyllableWord(pCandidateStemSyllableWord, pChoJungChar, CHANGE_CHAR_SYLLABLE_LAST);

						m_strLog = "\r\nA~(A+1)�� ���߼� = '";
						m_strLog += pChangeLastCharSyllable;
						m_strLog += "'";
						m_strLog += "\r\nA~(A+1)�� ���߼��� �ش��ϴ� �,(A+1)�� ����+B�� �ش��ϴ� ��� ���� ��ȸ";
						SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

						bFindStem = IsExistStemDic(pNode, pChangeLastCharSyllable);
						bFindEnding = IsExistEndingDic(pNode, pAddTwoEndOfWord);

						if (bFindStem == true && bFindEnding == true)
						{
							m_strLog = "\r\n��� ����(����)";
							SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
						}
						else
						{
							//A~A+1�� ���߼��� �ش��ϴ� ��� ������ �ְ�, ������ '��'�� ��� ����ó���Ѵ�.
							if (strcmp(pJongChar, "��") == 0)
							{
								m_strLog = "\r\nA~(A+1�� ���߼�) ������� ";
								if (bFindStem == false) { m_strLog += "����"; }
								else { m_strLog += "������"; }
								m_strLog += ",(A+1������)+B ��̻����� ";
								if (bFindEnding == false) { m_strLog += "����"; }
								else { m_strLog += "������"; }
								m_strLog += ",������ '��' �� ��� ���� ó��";
								SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

								m_LinkedList->CreateCompoundSubNode(pNode, "ENDING");

								DeleteSubTokenResult(pNode, DIC_STEM);//���� � ��� ���� ����
								pNode->pHToken->subStem->strResult = new char[strlen(pChangeLastCharSyllable) + 1];
								memset(pNode->pHToken->subStem->strResult, 0x00, strlen(pChangeLastCharSyllable) + 1);
								strcpy(pNode->pHToken->subStem->strResult, pChangeLastCharSyllable);

								//��м��� ��̸� ���վ�̿� �ִ´�.
								InsertAddCompoundSubTail(pNode, pNode->pHToken->subEnding->strResult, DIC_ENDING);//���վ������ 
								InsertAddCompoundSubTail(pNode, pJongChar, DIC_ENDING);//���վ������ 
								m_strLog = "\r\n�������� : '";
								m_strLog += pNode->pHToken->subEnding->strResult;
								m_strLog = "'";
								m_strLog += "\r\n�������� : '";
								m_strLog += pJongChar;
								m_strLog = "'";
								SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

								//'��'+��� ������ ��������� ������ �����.
								DeleteSubTokenResult(pNode, DIC_ENDING);//���� ��� ��� ���� ����
								pNode->pHToken->subEnding->strResult = new char[strlen(pAddTwoEndOfWord) + 1];
								memset(pNode->pHToken->subEnding->strResult, 0x00, strlen(pAddTwoEndOfWord) + 1);
								strcpy(pNode->pHToken->subEnding->strResult, pAddTwoEndOfWord);

								pNode->pHToken->nKindError = 0;
								//�м��Ϸ�
								m_strLog = "\r\n�м��Ϸ�(����).";
								SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
							}
							else
							{
								pNode->pHToken->nKindError = -999;
								m_strLog = "\r\n��� ����(����)";
								SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
								//����ó�� : ���վ�ǥ��  �:�м�����:(���) �м�����:(�����) ���:��(�м�����) ������(�м�����)
								WriteAnalysisError(pNode, pChangeLastCharSyllable, pAddTwoEndOfWord);
							}
						}
					}
				}
			}
		}
		else
		{
			if (IrregularGrammarCheck(pNode, "��") == true)
			{
				m_strLog = "���ұ�Ģ ó�� �Ϸ�";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
			}
			else if (IrregularGrammarCheck(pNode, "��") == true)
			{
				m_strLog = "���ұ�Ģ ó�� �Ϸ�";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
			}
			else if (IrregularGrammarCheck(pNode, "��") == true)
			{
				m_strLog = "���ұ�Ģ ó�� �Ϸ�";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
			}
			else if (IrregularGrammarCheck(pNode, "��") == true)
			{
				m_strLog = "���ұ�Ģ ó�� �Ϸ�";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
			}
			else if (IrregularGrammarCheck(pNode, "��") == true)
			{
				m_strLog = "���ұ�Ģ ó�� �Ϸ�";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
			}
			else if (IrregularGrammarCheck(pNode, "��") == true)
			{
				m_strLog = "���ұ�Ģ ó�� �Ϸ�";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
			}
			else if (IrregularGrammarCheck(pNode, "��") == true)
			{
				m_strLog = "�̺ұ�Ģ ó�� �Ϸ�";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
			}
			else if (IrregularGrammarCheck(pNode, "��") == true)
			{
				m_strLog = "���ұ�Ģ ó�� �Ϸ�";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
			}
			
			else
			{
				pNode->pHToken->nKindError = -999;
				m_strLog = "�ұ�Ģ ���� ����";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
			}
		}
	}
	else
	{
		//����
		pNode->pHToken->nKindError = -999;
		m_strLog = "��̸� �����ϴ� ���(�̵�Ͼ� ó��) ��Ÿ ������ ����";
		SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
	}

	CHAR_DELETE(pOneSyllable);
	CHAR_DELETE(pChoJungChar);
	CHAR_DELETE(pJongChar);
	CHAR_DELETE(pCandidateSyllableEndOfWord);
	CHAR_DELETE(pAddTwoEndOfWord);
	CHAR_DELETE(pCandidateStemSyllableWord);
	CHAR_DELETE(pChangeLastCharSyllable);
	CHAR_DELETE(pChangeLastChar);
	CHAR_DELETE(pAddTwoStmeEndOfWord);
	CHAR_DELETE(pFirstOrLastOneChar);
}


void CNAHangulAnaly::DeleteSubTokenResult(H_NODE* pNode, int nResult)//��׾���� ��� ���� ����
{
	if(nResult == DIC_STEM)
	{
		CHAR_DELETE(pNode->pHToken->subStem->strResult);
	}
	else if (nResult == DIC_ENDING)
	{
		CHAR_DELETE(pNode->pHToken->subEnding->strResult);
	}
}

void CNAHangulAnaly::DeleteSubTokenFeature(H_NODE* pNode, int nResult)//��׾���� ���� ���� ����
{
	if (nResult == DIC_STEM)
	{
		CHAR_DELETE(pNode->pHToken->subStem->strFeature);
	}
	else if (nResult == DIC_ENDING)
	{
		CHAR_DELETE(pNode->pHToken->subEnding->strFeature);
	}
}


void CNAHangulAnaly::WriteAnalysisError(H_NODE *pNode, char *pStemWord, char *pEndingWord)
{
	//����ó�� : ���վ�ǥ��  �:�м�����:(���) �м�����:(�����) ���:��(�м�����) ������(�м�����)
	pNode->pHToken->nKindError = -999;
	if(pStemWord != NULL)
	{
		#ifdef _DEBUG
		strcat(pNode->pHToken->strAnalysisError, "���� �[���վ�] �:�м�����:(");
		strcat(pNode->pHToken->strAnalysisError, pNode->pHToken->subStem->strResult);
		strcat(pNode->pHToken->strAnalysisError, ")");
		strcat(pNode->pHToken->strAnalysisError, "�м�����:(");
		strcat(pNode->pHToken->strAnalysisError, pStemWord);
		strcat(pNode->pHToken->strAnalysisError, ")");
		#endif
	}
	if (pEndingWord != NULL)
	{
		#ifdef _DEBUG
		strcat(pNode->pHToken->strAnalysisError, "���� ���[����] ���:�м�����:(");
		strcat(pNode->pHToken->strAnalysisError, pNode->pHToken->subEnding->strResult);
		strcat(pNode->pHToken->strAnalysisError, ")");
		strcat(pNode->pHToken->strAnalysisError, "�м�����:(");
		strcat(pNode->pHToken->strAnalysisError, pEndingWord);
		strcat(pNode->pHToken->strAnalysisError, ")");
		#endif
	}
}

bool CNAHangulAnaly::FindTheWholeWord(H_NODE* pNode)
{
	return IsExistStemDic(pNode, pNode->pHToken->strSource);
}

void CNAHangulAnaly::LongestMatchStemAndEndOfWordAnalysis(H_NODE* pNode)
{
	CString m_strLog;
	char* pStemWord = NULL;
	char* pEndingWord = NULL;
	int nLen = strlen(pNode->pHToken->strSource) - 1;
	int nPos = 0;
	
	for (int i = nLen; i >= 0; i--)
	{
		nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, i, DIC_ENDING);
		pEndingWord = m_pCNAHangulUtil->GetRightToLeftWord(pNode->pHToken->strSource, nPos);
		//�ش� �������� ���� ��Ī 
		if(IsExistEndingDic(pNode, pEndingWord) == false)
		{
			CHAR_DELETE(pEndingWord);
			break;
		}
		else
		{
			pNode->pHToken->subEnding->nFindPos = nPos;
			nLen = nPos;//����� ã�� ���� ��ġ�� ������ �����Ѵ�.
			CHAR_DELETE(pEndingWord);
		}
		i = nPos;
	}
	
	//��������� ������ �� 0~B-1(��̸���������ü����)���� ��������� �׼��� �Ѵ�.
	if(pNode->pHToken->subEnding->strResult != NULL)
	{
		pStemWord = m_pCNAHangulUtil->GetLeftToRightWord(pNode->pHToken->strSource, pNode->pHToken->subEnding->nFindPos - 1);
		if(IsExistStemDic(pNode, pStemWord) == true)
		{
			pNode->pHToken->subStem->nFindPos = pNode->pHToken->subEnding->nFindPos - 1;
		}
		CHAR_DELETE(pStemWord);
	}
	if(pNode->pHToken->subStem->strResult == NULL)
	{
		//������������ ã���� ���.. ��ü���ڿ����� ������ ã�� �ʿ䰡 ����. ã�� ��� ������������ ������� �׼����Ѵ�.
		if(pNode->pHToken->subEnding->strResult != NULL)
		{
			nLen = pNode->pHToken->subEnding->nFindPos;
		}
		int nNotFindNumber = 0;
		for (int i = 0; i < nLen - 1; i++)
		{
			nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, i, DIC_STEM);
			pStemWord = m_pCNAHangulUtil->GetLeftToRightWord(pNode->pHToken->strSource, nPos);
			//�ش� ���ۻ��� ���� ��Ī 
			if (IsExistStemDic(pNode, pStemWord) == true) 
			{
				nNotFindNumber += 1;
				pNode->pHToken->subStem->nFindPos = nPos;
			}
			else
			{
				nNotFindNumber += 1;
				if(nNotFindNumber > 5)//5���� �̻����� �м��Ǵ� ������� ���´�.
				{
					CHAR_DELETE(pStemWord);
					break;
				}
			}
			CHAR_DELETE(pStemWord);
			i = nPos;
		}
	}

	//�,��̰� ��� �����ϴ� ���
	if (pNode->pHToken->subStem->strResult != NULL && pNode->pHToken->subEnding->strResult != NULL)
	{
		m_strLog = "\r\nA,B ��� �/��̻����� ���� ";
		m_strLog += "A : '";
		m_strLog += pNode->pHToken->subStem->strResult;
		m_strLog += "'";
		m_strLog += ", B : '";
		m_strLog += pNode->pHToken->subEnding->strResult;
		m_strLog += "'";
		SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

		IsStemEndingExistSyllable(pNode);
	}
	//��� �����ϴ� ���
	else if (pNode->pHToken->subStem->strResult != NULL && pNode->pHToken->subEnding->strResult == NULL)
	{
		m_strLog = "\r\nA�� ������� ����, B(��̻���)������ ";
		m_strLog += "\r\n A : '";
		m_strLog += pNode->pHToken->subStem->strResult;
		m_strLog += "'";
		SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
		IsOnlyStemExistSyllable(pNode);
	}
	//��̸� �����ϴ� ���(�ұ�Ģ�� ���� ó��)
	else if (pNode->pHToken->subStem->strResult == NULL && pNode->pHToken->subEnding->strResult != NULL)
	{
		m_strLog = "\r\nB�� ��̻����� ����, A(�����)������";
		m_strLog += "B : '";
		m_strLog += pNode->pHToken->subEnding->strResult;
		m_strLog += "'";
		SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
		IsEndingExistSyllable(pNode);
	}
	else
	{
		//�м�����
		//�м���� ���� ��� ������ �� ����(����,������ ����)
		//pNode->pHToken->nKindError = -998;
		//���ո��� �����Ͽ� ���������� �ش��ϴ� ������ �����Ѵ�.
		m_LinkedList->CreateCompoundSubNode(pNode, "STEM");
		CompoundNounDecomposition(pNode, strlen(pNode->pHToken->strSource)-1);//���ո�����
		//����Ʈ���� �� �����Ѵ�. N��°�� ������ ������  N+1��°�� ù������ ���� ��, N+1��° ù ���� ���� ó��
		UnRegisteredCompoundNounWord(pNode);
	}

}


//Linked List�� SubTail �߰�(���վ �Ǵ� ���վ��)
void CNAHangulAnaly::InsertAddCompoundSubTail(H_NODE *pNode, char *pString, int strFlag)
{
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
			if(strFlag == DIC_STEM)
			{
				//ClassfyKindChar(pInsertNode, pString);
				bFind = IsExistStemDic(pInsertNode, pString);
				if(bFind == false)
				{
					CHAR_DELETE(pInsertNode->pCSubToken->strResult);
					CHAR_DELETE(pInsertNode->pCSubToken->strFeature);
					int nLen = strlen(pString);
					pInsertNode->pCSubToken->strResult = new char[nLen + 1];
					memset(pInsertNode->pCSubToken->strResult, 0x00, nLen + 1);
					strcpy(pInsertNode->pCSubToken->strResult, pString);
					pInsertNode->pCSubToken->strFeature = new char[STEM_DIC_ALL_FEATURE_NUMBER + 1];
					memset(pInsertNode->pCSubToken->strFeature,0x00, sizeof(pInsertNode->pCSubToken->strFeature));
					for (int i = 0; i < STEM_DIC_ALL_FEATURE_NUMBER + 1; i++) {
						pInsertNode->pCSubToken->strFeature[i] = '0';
					}
					pInsertNode->pCSubToken->strFeature[STEM_DIC_N] = '1';//N��������ο�
				 }
				pInsertNode->pPrev =  pNode->pHToken->pSubHeadNode;
				pInsertNode->pNext = pNode->pHToken->pSubHeadNode->pNext;
				pNode->pHToken->pSubHeadNode->pNext = pInsertNode;
				pNode->pHToken->pSubHeadNode->pPrev = pInsertNode;
			}
			else if (strFlag == DIC_ENDING)
			{
				bFind = IsExistEndingDic(pInsertNode, pString);
				if(bFind == false)
				{
					CHAR_DELETE(pInsertNode->pCSubToken->strResult);
					CHAR_DELETE(pInsertNode->pCSubToken->strFeature);
					int nLen = strlen(pString);
					pInsertNode->pCSubToken->strResult = new char[nLen + 1];
					memset(pInsertNode->pCSubToken->strResult, 0x00, nLen + 1);
					strcpy(pInsertNode->pCSubToken->strResult, pString);
					pInsertNode->pCSubToken->strFeature = new char[ENDING_DIC_BASIC_FEATURE_NUMBER + 1];
					memset(pInsertNode->pCSubToken->strFeature, 0x00, sizeof(pInsertNode->pCSubToken->strFeature));
					for(int i = 0; i < ENDING_DIC_BASIC_FEATURE_NUMBER + 1; i++){
						pInsertNode->pCSubToken->strFeature[i] = '0';
					}
					pInsertNode->pCSubToken->strFeature[ENDING_DIC_N] = '1';//N ���������ο�
				}
				pInsertNode->pPrev = pNode->pHToken->pSubEndingTailNode->pPrev;
				pInsertNode->pNext = pNode->pHToken->pSubEndingTailNode;
				pNode->pHToken->pSubEndingTailNode->pPrev->pNext = pInsertNode;
				pNode->pHToken->pSubEndingTailNode->pPrev = pInsertNode;
			}
			
		}	
	}
}

//A~B������ ��� ���� ��̻��� ���� ���� Ȯ��
bool CNAHangulAnaly::ExistAllEtcWordEndOfWordAnalysis(H_NODE* pNode)
{
	//��� ��� ������ ��� ������ �˻��Ѵ�.
	bool pAllFindSyllable = false;
	int nStart = pNode->pHToken->subEnding->nFindPos - 1;
	int nEnd = pNode->pHToken->subStem->nFindPos;
	for (nStart; nStart>nEnd; nStart--)
	{
		char* pOneSyllable = NULL;
		//(A(���������ġ)->B(�����������ġ) ������ ���� ����.
		//���ʿ��� ���������� ���� �׼��� �ϹǷ� DIC_STEM�� �����)
		pOneSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nStart, DIC_ENDING);
		if (strlen(pOneSyllable) == 2) nStart = nStart - 1;//�ѱ��� ���
		
		m_strLog = "\r\n���� : ";
		
		if (IsExistEndingDic(pOneSyllable) == false)
		{
			m_strLog += "'";
			m_strLog += pOneSyllable;
			m_strLog += "'";
			m_strLog +=  "-����";
			SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
			CHAR_DELETE(pOneSyllable);
			pAllFindSyllable = false;
			//�������� ���н� ������ ���� ��ġ�� ���� ������ ������.
			pNode->pHToken->nLastFindEndingSyllable =  pNode->pHToken->subEnding->nFindPos;
			break;
		}
		else
		{
			m_strLog += "'";
			m_strLog += pOneSyllable;
			m_strLog += "'";
			m_strLog += "-����";
			SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
			pAllFindSyllable = true;

			//A~B������ �����ʿ��� �������� �м��� ������ ��� ���� ����
			pNode->pHToken->nLastFindEndingSyllable = nStart;
			CHAR_DELETE(pOneSyllable);
		}
	}
	return pAllFindSyllable;
}


bool CNAHangulAnaly::CreateAmbiScript()
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
				m_strLog = "\r\n���Ǽ��м� �� ";
				m_strLog += "�/��� :'";
				m_strLog += "'";
				m_strLog += pNode->pHToken->subStem->strResult;
				m_strLog += "'/'";
				m_strLog += pNode->pHToken->subEnding->strResult;
				m_strLog += "'";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

				//m_pAmbiguity->GoAmbiguityFunction(this, pNode, 3);

				m_strLog = "\r\n���Ǽ��м� �Ϸ� �� ";
				m_strLog += "�/��� :'";
				m_strLog += "'";
				m_strLog += pNode->pHToken->subStem->strResult;
				m_strLog += "'/'";
				m_strLog += pNode->pHToken->subEnding->strResult;
				m_strLog += "'";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

				//�ӽ������� �ּ� ó����.(����ó���� �����ϴ� �۾��� ��...)
				//RestorIrrigual(m_pNode);
				pNode = pNode->pNext;
				m_nListPosition -= 1;
			}
			else if (isStemAmbCheck == true && isEndingAmbCheck == false)
			{
				m_strLog = "\r\n���Ǽ��м� �� ";
				m_strLog += "�/��� :'";
				m_strLog += "'";
				m_strLog += pNode->pHToken->subStem->strResult;
				m_strLog += "'/'";
				m_strLog += pNode->pHToken->subEnding->strResult;
				m_strLog += "'";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

				//m_pAmbiguity->GoAmbiguityFunction(this, pNode, 2);
	
				m_strLog = "\r\n���Ǽ��м� �Ϸ� �� ";
				m_strLog += "�/��� :'";
				m_strLog += "'";
				m_strLog += pNode->pHToken->subStem->strResult;
				m_strLog += "'/'";
				m_strLog += pNode->pHToken->subEnding->strResult;
				m_strLog += "'";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

				//�ӽ������� �ּ� ó����.(����ó���� �����ϴ� �۾��� ��...)
				//RestorIrrigual(m_pNode);
				pNode = pNode->pNext;
				m_nListPosition -= 1;
			}
			else if (isStemAmbCheck == false && isEndingAmbCheck == true)
			{
				m_strLog = "\r\n���Ǽ��м� �� ";
				m_strLog += "�/��� :'";
				m_strLog += "'";
				m_strLog += pNode->pHToken->subStem->strResult;
				m_strLog += "'/'";
				m_strLog += pNode->pHToken->subEnding->strResult;
				m_strLog += "'";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
				
				//m_pAmbiguity->GoAmbiguityFunction(this, pNode, 1);
				
				m_strLog = "\r\n���Ǽ��м� �Ϸ� �� ";
				m_strLog += "�/��� :'";
				m_strLog += "'";
				m_strLog += pNode->pHToken->subStem->strResult;
				m_strLog += "'/'";
				m_strLog += pNode->pHToken->subEnding->strResult;
				m_strLog += "'";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

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

//���ո�����
void CNAHangulAnaly::CompoundNounDecomposition(H_NODE *pNode, int nEndPosition)
{
	int nSyllableCount = 0;
	int i = 0;
	int nPos = 0;
	int nFewPositions = 0;
	char * pAddTwoWord = NULL;
	char * pFirstSyllable = NULL;
	char* pStemWord = NULL;
	char* strSource = NULL;
	char* pWord2 = NULL;
	char* pWord3 = NULL;
	char* pWord4 = NULL;
	bool bFindWord = false;
	bool bFind2 = false;
	bool bFind3 = false;
	bool bFind4 = false;
	int nFindPos2 = -1;
	int nFindPos3 = -1;
	int nFindPos4 = -1;
	
	
	//�������������� : 1���� : 970 2���� : 22, 720 3���� : 29, 132 4���� : 9, 941 5���� : 2, 750 
	//2, 3, 4������ ��κ��� ������ ���������� ���ո�� �̵�Ͼ� �м� ������ 2,3,4������ ������� �Ѵ�.
	if (nEndPosition < 0) return;
	
	strSource = new char[nEndPosition+2];
	memset(strSource,0x00, nEndPosition + 2);
	strncpy(strSource, pNode->pHToken->strSource, nEndPosition+1);
	//0~nEndPosition������ ���ڼ� Ȯ���Ѵ�.
	nSyllableCount = m_pCNAHangulUtil->GetFirstToEndSyllableCount(strSource, nEndPosition);
	
	//2���� ������ ��� �м��������� �����Ѵ�.(�������� ���� ���, ���� �ֱٿ� �м��� �̵�Ͼ �ش� ���� ��ģ��)
	if(nSyllableCount < 2)
	{ 
		if(pNode->pHToken->pSubHeadNode != NULL && pNode->pHToken->pSubHeadNode->pNext != NULL)
		{
			//�������� �Ǿ����� �ش� �������� �� ����� ó�� ������ ���� ��� strSource�� ���� ������ �ʰ� ������ ó���Ѵ�.
			if(pNode->pHToken->pSubHeadNode->pNext->pCSubToken != NULL && pNode->pHToken->pSubHeadNode->pNext->pCSubToken->strResult != NULL)
			{
				//nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, 0, DIC_STEM);
				//pFirstSyllable = m_pCNAHangulUtil->GetLeftToRightWord(pNode->pHToken->strSource, nPos);
				nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->pSubHeadNode->pNext->pCSubToken->strResult, 0, DIC_STEM);
				pFirstSyllable = m_pCNAHangulUtil->GetLeftToRightWord(pNode->pHToken->pSubHeadNode->pNext->pCSubToken->strResult, nPos);
				if(strcmp(strSource, pFirstSyllable) == 0)
				{
					pAddTwoWord = m_pCNAHangulUtil->GetCombineTwoStrings(NULL, pNode->pHToken->pSubHeadNode->pNext->pCSubToken->strResult);
				}
				else
				{
					pAddTwoWord = m_pCNAHangulUtil->GetCombineTwoStrings(strSource, pNode->pHToken->pSubHeadNode->pNext->pCSubToken->strResult);
				}
				CHAR_DELETE(pNode->pHToken->pSubHeadNode->pNext->pCSubToken->strResult);
				pNode->pHToken->pSubHeadNode->pNext->pCSubToken->strResult = new char[strlen(pAddTwoWord)+1];
				memset(pNode->pHToken->pSubHeadNode->pNext->pCSubToken->strResult,0x00,sizeof(pNode->pHToken->pSubHeadNode->pNext->pCSubToken->strResult));
				strcpy(pNode->pHToken->pSubHeadNode->pNext->pCSubToken->strResult, pAddTwoWord);
			}
			else
			{
				if(pNode->pHToken->pSubHeadNode->pNext->pCSubToken == NULL)
				{
					InsertAddCompoundSubTail(pNode, strSource, DIC_STEM);//�м��� ������ ���ʺ��վ���� 
				}
			}
		}
		nEndPosition = -1; 

		CHAR_DELETE(pAddTwoWord);
		CHAR_DELETE(pFirstSyllable);
		CHAR_DELETE(pStemWord);
		CHAR_DELETE(strSource);
		CHAR_DELETE(pWord2);
		CHAR_DELETE(pWord3);
		CHAR_DELETE(pWord4);

		return;
	}
	//4���� �̻��� �м����� �ʴ´�.(����4���������� �м�)
	if(nSyllableCount > 4) nSyllableCount = 4;
	for (i = nEndPosition; i >= 0; i--)
	{
		if (nSyllableCount < 1) break;//2���� ������ ��� ������.
		nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(strSource, i, DIC_ENDING);
		pStemWord = m_pCNAHangulUtil->GetRightToLeftWord(strSource, nPos);
		if(pStemWord != NULL)
		{
			nFewPositions += 1;//���°�� �ش�� �ܾ�
			if(nFewPositions > 1)
			{
				bFindWord = IsExistStemDic(pStemWord);
				if (nFewPositions == 2)
				{
					if (bFindWord == true){bFind2 = true; nEndPosition = nPos-1;}
					pWord2 = new char[strlen(pStemWord) + 1]; strcpy(pWord2, pStemWord);
				}
				if (nFewPositions == 3)
				{
					if (bFindWord == true) { bFind3 = true; nEndPosition = nPos - 1;}
					pWord3 = new char[strlen(pStemWord) + 1]; strcpy(pWord3, pStemWord);
				}
				if (nFewPositions == 4)
				{
					if (bFindWord == true) { bFind4 = true; nEndPosition = nPos - 1;}
					pWord4 = new char[strlen(pStemWord) + 1]; strcpy(pWord4, pStemWord);
				}
			}
		}
		CHAR_DELETE(pStemWord);
		i = nPos;
		nSyllableCount -= 1;
	}
	//2,3,4������ �ش��ϴ� �������� ��� ������ ���� ��� �̵�Ϸ� ���忡 �ش��ϴ� ������ �̵�Ϸ� �־� �д�.
	if(bFind4 == false && bFind3 == false && bFind2 == false)
	{
		nEndPosition = i;
		if(pWord4 != NULL)InsertAddCompoundSubTail(pNode, pWord4, DIC_STEM);//�м��� ������ ���ʺ��վ���� 
		else if (pWord3 != NULL) InsertAddCompoundSubTail(pNode, pWord3, DIC_STEM);//�м��� ������ ���ʺ��վ���� 
		else if (pWord2 != NULL) InsertAddCompoundSubTail(pNode, pWord2, DIC_STEM);//�м��� ������ ���ʺ��վ���� 
		//�м� ��� ������ ���,  ������ ���� �� ���������� ù �������� �ٽ� �м��ǰ� nEndPosition���� �������Ѵ�.
		if(nEndPosition >= 0)
		{
			if (strlen(strSource) - 1 >= nEndPosition + 1)
			{
				nEndPosition = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(strSource, nEndPosition + 1, DIC_STEM);
			}
		}
	}
	else
	{
		if(bFind4 == true)
		{
			InsertAddCompoundSubTail(pNode, pWord4, DIC_STEM);//���վ���� 
		}
		else if (bFind3 == true)
		{
			InsertAddCompoundSubTail(pNode, pWord3, DIC_STEM);//���վ���� 
		}
		else if (bFind2 == true)
		{
			InsertAddCompoundSubTail(pNode, pWord2, DIC_STEM);//���վ���� 
		}
	}

	CHAR_DELETE(pAddTwoWord);
	CHAR_DELETE(pFirstSyllable);
	CHAR_DELETE(pStemWord);
	CHAR_DELETE(strSource);
	CHAR_DELETE(pWord2);
	CHAR_DELETE(pWord3);
	CHAR_DELETE(pWord4);
	
	//���ȣ��
	CompoundNounDecomposition(pNode, nEndPosition);
	
}

//N��°�� ������ ������  N+1��°�� ù������ ���� ��, N+1��° ù ���� ���� ó��
void CNAHangulAnaly::UnRegisteredCompoundNounWord(H_NODE *pNode)
{
	H_SUBNODE* pStartNode;
	H_SUBNODE* pNextNode;
	char* pLastSyllable = NULL; //��������
	char* pFirstSyllable = NULL;//������
	char* pMuchSyllable = NULL;//�ش������� ��ü
	char* pStemResultWords = NULL;//���ۺ��氪����
	char* pAddStemResultWords = NULL;
	int nWhile = 0;
	int nPos = 0;
	int nLen = 0;
	int nArrayLen = 0;
	//���վ� �Ǵ� �̵�Ͼ ���� �м�ó���� ������ ��쿡�� ó����.
	if(pNode->pHToken->bSubCompCreate == true)
	{
		nArrayLen =  strlen(pNode->pHToken->strSource) + 1;
		pAddStemResultWords = new char[nArrayLen];
		memset(pAddStemResultWords,0x00, nArrayLen);
		
		//�ó��
		if (pNode->pHToken->pSubHeadNode != NULL && pNode->pHToken->pSubHeadNode->pNext != NULL)
		{
			pStartNode = pNode->pHToken->pSubHeadNode->pNext;
			while (pStartNode != pNode->pHToken->pSubTailNode)
			{
				//���վ ���� �������� ��� ��ģ��.
				if(pStartNode != NULL && pStartNode->pCSubToken != NULL && pStartNode->pCSubToken->strResult != NULL)
				{
					if (nWhile == 0)
					{
						pStemResultWords = m_pCNAHangulUtil->GetCombineTwoStrings("", pStartNode->pCSubToken->strResult);
						strcat(pAddStemResultWords, pStemResultWords);
						CHAR_DELETE(pStemResultWords);
					}
					else
					{
						pStemResultWords = m_pCNAHangulUtil->GetCombineTwoStrings(pAddStemResultWords, pStartNode->pCSubToken->strResult);
						CHAR_DELETE(pAddStemResultWords);
						nArrayLen = strlen(pStemResultWords) + 1;
						pAddStemResultWords = new char[nArrayLen];
						memset(pAddStemResultWords, 0x00, nArrayLen);
						strcat(pAddStemResultWords, pStemResultWords);
						CHAR_DELETE(pStemResultWords);
					}
				}
				pNextNode = pStartNode->pNext;
				if(pNextNode != NULL && pNextNode->pCSubToken != NULL)
				{
					//���� ����� ������ ����
					nLen = strlen(pStartNode->pCSubToken->strResult)-1;
					nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pStartNode->pCSubToken->strResult, nLen, DIC_ENDING);
					pLastSyllable = m_pCNAHangulUtil->GetRightToLeftWord(pStartNode->pCSubToken->strResult, nPos);
					//�� ����� ó�� ����
					nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNextNode->pCSubToken->strResult, 0, DIC_STEM);
					pFirstSyllable = m_pCNAHangulUtil->GetLeftToRightWord(pNextNode->pCSubToken->strResult, nPos);
					//(N��'�ʸ���',N+1��'��ī����' �� ���	N�� ������ ������ N+1�� ù ������ �������� N+1�� ù ������ ����ó���Ѵ�.
					if(strcmp(pLastSyllable, pFirstSyllable) == 0)
					{
						nLen = strlen(pNextNode->pCSubToken->strResult) - 1;
						pMuchSyllable = m_pCNAHangulUtil->GetStartToEndWord(pNextNode->pCSubToken->strResult,nPos+1, nLen);
						if(pMuchSyllable != NULL)
						{
							CHAR_DELETE(pNextNode->pCSubToken->strResult);
							nLen = strlen(pMuchSyllable) + 1;
							pNextNode->pCSubToken->strResult = new char[nLen +1];
							memset(pNextNode->pCSubToken->strResult,0x00, nLen+1);
							strcpy(pNextNode->pCSubToken->strResult, pMuchSyllable);
						}
					}
					
					CHAR_DELETE(pMuchSyllable);
					CHAR_DELETE(pLastSyllable);
					CHAR_DELETE(pFirstSyllable);
				}
				pStartNode = pStartNode->pNext;
				nWhile += 1;
			}
		}

		//�̵�Ͼ����θ� ������ ������� �����Ѵ�.�� �и��� ���� �ϳ��� ���� �ϳ��� �������� �м��� ��ó�� ���̰� ó���Ѵ�.
		if(pAddStemResultWords != NULL)
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

			m_strLog = "\r\n���ո��� �м��� ������ �ϳ��� ���� �̵�Ͼ �ش��ϴ� ����� �� ����";
			SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
		}
	}

	CHAR_DELETE(pLastSyllable);
	CHAR_DELETE(pFirstSyllable);
	CHAR_DELETE(pMuchSyllable);
	CHAR_DELETE(pStemResultWords);
	CHAR_DELETE(pAddStemResultWords);
}

//�ұ�Ģó��(��,��,��,��,��,��,��,��,��)
bool CNAHangulAnaly::IrregularGrammarCheck(H_NODE *pNode, char* pGrammer)
{
	char* pChoJungChar = NULL;
	char* pChoChar = NULL;
	char* pJungChar = NULL;
	char* pJongChar = NULL;
	char* pChangeLastChar = NULL;
	char* pAddTwoEndOfWord = NULL;
	char* pAddTwoStmeEndOfWord = NULL;
	char* pFirstOrLastOneChar = NULL;
	char* pChangeLastCharSyllable = NULL;
	char* pCandidateSyllableEndOfWord = NULL;
	char* pCandidateStemSyllableWord = NULL;
	char* pOneEndingChar = NULL;
	char* pFindStemSyllableWord = NULL;
	char* pOneChangeChar = NULL;
	int nPos = 0;
	int nLastPos = 0;
	bool bFind = false;

	pFirstOrLastOneChar = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, pNode->pHToken->subEnding->nFindPos - 1, DIC_ENDING);//�ش� ��ġ�� ���� ��
	pChoJungChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pFirstOrLastOneChar, CHAR_CHOJUNGSUNG);
	pJongChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pFirstOrLastOneChar, CHAR_JONGSUNG);

	if (pJongChar != NULL && strcmp(pJongChar, "") != 0)
	{
		if(strcmp(pGrammer,"��") == 0)//���ұ�Ģ
		{
			pChangeLastChar = m_pCNAHangulUtil->ChangeCompulsionJongSyllable(pChoJungChar, "��");//���ұ�Ģ
			pCandidateStemSyllableWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, pNode->pHToken->subEnding->nFindPos - 1);
			pFindStemSyllableWord = m_pCNAHangulUtil->SetChangeSyllableWord(pCandidateStemSyllableWord, pChangeLastChar, CHANGE_CHAR_SYLLABLE_LAST);//���ڿ��� ������ ������ ���߼����κ���
			bFind = IsExistStemDic(pNode, pFindStemSyllableWord);
			
			if(bFind == true) m_strLog = "\r\n'��'�ұ�Ģ ����(����) : '"; else m_strLog = "\r\n'��'�ұ�Ģ ����(����) : '";
			m_strLog += pFindStemSyllableWord;
			m_strLog += "'";
			SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
		}
		else if (strcmp(pGrammer, "��") == 0)//���ұ�Ģ
		{
			pChangeLastChar = m_pCNAHangulUtil->ChangeCompulsionJongSyllable(pChoJungChar, "��");//���ұ�Ģ
			pCandidateStemSyllableWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, pNode->pHToken->subEnding->nFindPos - 1);
			pFindStemSyllableWord = m_pCNAHangulUtil->SetChangeSyllableWord(pCandidateStemSyllableWord, pChangeLastChar, CHANGE_CHAR_SYLLABLE_LAST);//���ڿ��� ������ ������ ���߼����κ���
			bFind = IsExistStemDic(pNode, pFindStemSyllableWord);

			if (bFind == true) m_strLog = "\r\n'��'�ұ�Ģ ����(����) : '"; else m_strLog = "\r\n'��'�ұ�Ģ ����(����) : '";
			m_strLog += pFindStemSyllableWord;
			m_strLog += "'";
			SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
		 }
		else if (strcmp(pGrammer, "��") == 0)//���ұ�Ģ
		{
			pChangeLastChar = m_pCNAHangulUtil->ChangeCompulsionJongSyllable(pChoJungChar, "��");//���ұ�Ģ
			pCandidateStemSyllableWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, pNode->pHToken->subEnding->nFindPos - 1);
			pFindStemSyllableWord = m_pCNAHangulUtil->SetChangeSyllableWord(pCandidateStemSyllableWord, pChangeLastChar, CHANGE_CHAR_SYLLABLE_LAST);//���ڿ��� ������ ������ ���߼����κ���
			bFind = IsExistStemDic(pNode, pFindStemSyllableWord);

			if (bFind == true) m_strLog = "\r\n'��'�ұ�Ģ ����(����) : '"; else m_strLog = "\r\n'��'�ұ�Ģ ����(����) : '";
			m_strLog += pFindStemSyllableWord;
			m_strLog += "'";
			SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
		}
		else if (strcmp(pGrammer, "��") == 0)//���ұ�Ģ
		{
			pChangeLastChar = m_pCNAHangulUtil->ChangeCompulsionJongSyllable(pChoJungChar, "��");//���ұ�Ģ
			pCandidateStemSyllableWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, pNode->pHToken->subEnding->nFindPos - 1);
			pFindStemSyllableWord = m_pCNAHangulUtil->SetChangeSyllableWord(pCandidateStemSyllableWord, pChangeLastChar, CHANGE_CHAR_SYLLABLE_LAST);//���ڿ��� ������ ������ ���߼����κ���
			bFind = IsExistStemDic(pNode, pFindStemSyllableWord);

			if (bFind == true) m_strLog = "\r\n'��'�ұ�Ģ ����(����) : '"; else m_strLog = "\r\n'��'�ұ�Ģ ����(����) : '";
			m_strLog += pFindStemSyllableWord;
			m_strLog += "'";
			SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
		}
		else if (strcmp(pGrammer, "��") == 0)//���ұ�Ģ
		{
			pChangeLastChar = m_pCNAHangulUtil->ChangeCompulsionJongSyllable(pChoJungChar, "��");//���ұ�Ģ
			pCandidateStemSyllableWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, pNode->pHToken->subEnding->nFindPos - 1);
			pFindStemSyllableWord = m_pCNAHangulUtil->SetChangeSyllableWord(pCandidateStemSyllableWord, pChangeLastChar, CHANGE_CHAR_SYLLABLE_LAST);//���ڿ��� ������ ������ ���߼����κ���
			bFind = IsExistStemDic(pNode, pFindStemSyllableWord);

			if (bFind == true) m_strLog = "\r\n'��'�ұ�Ģ ����(����) : '"; else m_strLog = "\r\n'��'�ұ�Ģ ����(����) : '";
			m_strLog += pFindStemSyllableWord;
			m_strLog += "'";
			SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
		}
		else if (strcmp(pGrammer, "��") == 0)//���ұ�Ģ('��'�� �ٿ� Ž�� : � ������ '��'�� ������ ���� ������� '���ұ�Ģ' ���翡 �ش�)
		{
			pChangeLastChar = m_pCNAHangulUtil->ChangeCompulsionJongSyllable(pChoJungChar, "");//���ұ�Ģ
			pCandidateStemSyllableWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, pNode->pHToken->subEnding->nFindPos - 1);
			pFindStemSyllableWord = m_pCNAHangulUtil->SetChangeSyllableWord(pCandidateStemSyllableWord, pChangeLastChar, CHANGE_CHAR_SYLLABLE_LAST);//���ڿ��� ������ ������ ���߼����κ���
			bFind = IsExistStemDic(pNode, pFindStemSyllableWord);

			if (bFind == true) m_strLog = "\r\n'��'�ұ�Ģ ����(����) : '"; else m_strLog = "\r\n'��'�ұ�Ģ ����(����) : '";
			m_strLog += pFindStemSyllableWord;
			m_strLog += "'";
			SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
		}
		else if (strcmp(pGrammer, "��") == 0)///�̺ұ�Ģ(�������� '��'�� ���� �ʼ��� '��'�̸� 'Ǫ' �ƴϸ� ���� �ʼ��� '��'�ٿ� ����� Ž��)
		{
			CHAR_DELETE(pChoChar);
			CHAR_DELETE(pJungChar);
			CHAR_DELETE(pJongChar);
			pChoChar = new char[3]; memset(pChoChar, 0x00, 3);
			pJungChar = new char[3]; memset(pJungChar, 0x00, 3);
			pJongChar = new char[3]; memset(pJongChar, 0x00, 3);
			m_pCNAHangulUtil->GetWordsChoJungJongSyllable(pFirstOrLastOneChar, pChoChar, pJungChar, pJongChar);
			if (strcmp(pJungChar, "��") == 0)
			{
				CHAR_DELETE(pOneChangeChar);
				pOneChangeChar = new char[3];
				memset(pOneChangeChar, 0x00, 3);
				if (pChoChar != NULL && strcmp(pChoChar, "��") == 0)
				{
					//'��'->'Ǫ'�� ����
					m_pCNAHangulUtil->ChangeCompulsionChoJungJongSyllable(pOneChangeChar, pChoChar, "��", NULL);
				}
				else
				{
					//'��'->'��'�� ����
					m_pCNAHangulUtil->ChangeCompulsionChoJungJongSyllable(pOneChangeChar, pChoChar, "��", NULL);
				}
				pFindStemSyllableWord = m_pCNAHangulUtil->SetChangeSyllableWord(pCandidateStemSyllableWord, pOneChangeChar, CHANGE_CHAR_SYLLABLE_LAST);//���ڿ��� ������ ������ ���߼����κ���
				bFind = IsExistStemDic(pNode, pFindStemSyllableWord);

				if (bFind == true) m_strLog = "\r\n'��'�ұ�Ģ ����(����) : '"; else m_strLog = "\r\n'��'�ұ�Ģ ����(����) : '";
				m_strLog += pFindStemSyllableWord;
				m_strLog += "'";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
			}
		}
		else if (strcmp(pGrammer, "��") == 0)//���ұ�Ģ(�������� '��'�� ���� '��' :  ������ �̸��ٸ� '���ұ�Ģ'�̴� ����� �߿��� 'Ǫ����','�븣��','������')
		{
			if (pJongChar != NULL && strcmp(pJongChar, "��") == 0)
			{
				pCandidateStemSyllableWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, pNode->pHToken->subEnding->nFindPos - 1);
				pFindStemSyllableWord = m_pCNAHangulUtil->SetChangeSyllableWord(pCandidateStemSyllableWord, "��", CHANGE_CHAR_SYLLABLE_LAST);//���ڿ��� ������ ������ ���߼����κ���
				bFind = IsExistStemDic(pNode, pFindStemSyllableWord);

				if (bFind == true) m_strLog = "\r\n'��'�ұ�Ģ ����(����) : '"; else m_strLog = "\r\n'��'�ұ�Ģ ����(����) : '";
				m_strLog += pFindStemSyllableWord;
				m_strLog += "'";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
			}
			else
			{
				m_strLog = "\r\n'��'�ұ�Ģ ������ '��'�� �ƴϹǷ� �м����� ������";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
			}
		}
	}
	CHAR_DELETE(pChoJungChar);
	CHAR_DELETE(pChoChar);
	CHAR_DELETE(pJungChar);
	CHAR_DELETE(pJongChar);
	CHAR_DELETE(pChangeLastChar);
	CHAR_DELETE(pAddTwoEndOfWord);
	CHAR_DELETE(pAddTwoStmeEndOfWord);
	CHAR_DELETE(pFirstOrLastOneChar);
	CHAR_DELETE(pChangeLastCharSyllable);
	CHAR_DELETE(pCandidateSyllableEndOfWord);
	CHAR_DELETE(pCandidateStemSyllableWord);
	CHAR_DELETE(pOneEndingChar);
	CHAR_DELETE(pFindStemSyllableWord);
	CHAR_DELETE(pOneChangeChar);

	return  bFind;
}



















void CNAHangulAnaly::NAHangulUtilTesting(H_NODE* pNode)
{
	m_pCNAHangulUtil = new CNAHangulUtil();
	char* pStemWord = NULL;
	char* pEndingWord = NULL;
	int nLen = strlen(pNode->pHToken->strSource);
	int nPos = 0;
	
	//������ ��ü �������� �ľ�
	//nPos = m_pCNAHangulUtil->GetFirstToEndSyllableCount(pNode->pHToken->strSource, nLen-1);
	
	//���ո�� �и� �Լ�
	m_LinkedList->CreateCompoundSubNode(pNode, "STEM");
	CompoundNounDecomposition(pNode, strlen(pNode->pHToken->strSource) - 1);//���ո��и� �Լ�
	//����Ʈ���� �� �����Ѵ�. N��°�� ������ ������  N+1��°�� ù������ ���� ��, N+1��° ù ���� ���� ó��
	UnRegisteredCompoundNounWord(pNode);
	
	

	//���߼�+���� ����
	pStemWord= m_pCNAHangulUtil->ChangeCompulsionJongSyllable("��", "��");
	CHAR_DELETE(pStemWord);

	for(int i = 0; i <= nLen-1; i++)
	{
		nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, i, DIC_STEM);
		pStemWord = m_pCNAHangulUtil->GetLeftToRightWord(pNode->pHToken->strSource, nPos);
		//�ش� �ε���(�迭)�� ������ ��ȸ
		char* pOneStemChar = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, i, DIC_STEM);
		//�ش� �ε���(�迭)�� �������� ���߼� ��ȸ
		char* pChoJungStemChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pOneStemChar, CHAR_CHOJUNGSUNG);
		//�ش� �ε���(�迭)�� �������� ���� ��ȸ
		char* pJongStemChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pOneStemChar, CHAR_JONGSUNG);
		//���ڿ��� ������ ������ ���߼����κ���
		char* pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pStemWord, pChoJungStemChar, CHANGE_CHAR_SYLLABLE_LAST);		
		
		CHAR_DELETE(pOneStemChar);
		CHAR_DELETE(pChoJungStemChar);
		CHAR_DELETE(pJongStemChar);

		CHAR_DELETE(pChangeLastChar);
		//CHAR_DELETE(pChangeFirstChar);
		//�ش� ���ۻ��� ���� ��Ī 
		//���尪 ���� �� i+1�� ���߼� Ȯ�� ���� �ʿ�
		//i�� �� i+1�� ���߼� ��ģ �� ������ Ȯ�� �۾� �ʿ�
		i = nPos;
		if(pStemWord != NULL)
		{
			delete pStemWord;
			pStemWord = NULL;
		}
	}
	for (int i = nLen-1; i >= 0; i--)
	{
		nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, i, DIC_ENDING);
		pEndingWord = m_pCNAHangulUtil->GetRightToLeftWord(pNode->pHToken->strSource, nPos);
		//�ش� �ε���(�迭)�� ������ ��ȸ
		char* pOneEndingChar = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, i, DIC_ENDING);
		//�ش� �ε���(�迭)�� �������� ���߼� ��ȸ
		char* pChoJungEndingChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pOneEndingChar, CHAR_CHOJUNGSUNG);
		//�ش� �ε���(�迭)�� �������� ���� ��ȸ
		char* pJongEndingChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pOneEndingChar, CHAR_JONGSUNG);
		//���ڿ��� ù ������  �ش� ���������κ���
		char* pChangeFirstChar = m_pCNAHangulUtil->SetChangeSyllableWord(pEndingWord, pJongEndingChar, CHANGE_CHAR_SYLLABLE_FIRST);

		CHAR_DELETE(pChangeFirstChar);
		CHAR_DELETE(pOneEndingChar);
		CHAR_DELETE(pChoJungEndingChar);
		CHAR_DELETE(pJongEndingChar);

		//�ش� �������� ���� ��Ī 
		//���尪 ���� �� i-1�� ���� Ȯ�� ���� �ʿ�
		//i-1�� ������ i�� �� ��ģ �� ������ Ȯ�� �۾� �ʿ�
		i = nPos;
		if (pEndingWord != NULL)
		{
			delete pEndingWord;
			pEndingWord = NULL;
		}
	}
	delete m_pCNAHangulUtil;
	m_pCNAHangulUtil = NULL;
}
