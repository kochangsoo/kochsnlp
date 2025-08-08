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

//복합어간정보 추가
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

//복합어미정보 추가
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
			//NAHangulUtilTesting(pNode);//한글유틸테스트 함수
			//---------------------------------------------------------------------------------------------------------------------------
			//한글인 경우 문자열이 2Byte이므로 15음절 이하만 분석한다.
			//---------------------------------------------------------------------------------------------------------------------------
			//if(strlen(pNode->pHToken->strSource) <= 20)
			if(m_pCNAHangulUtil->GetFirstToEndSyllableCount(pNode->pHToken->strSource, strlen(pNode->pHToken->strSource) - 1) <= 15)
			{
				#ifdef _DEBUG
					memset(pNode->pHToken->strFlowChartDebug, 0, 1024);
					memset(pNode->pHToken->strAnalysisError, 0, 1024);
				#endif
					
				//---------------------------------------------------------------------------------------------------------------------------
				//전체어절을 사전에서 찾는다.
				//---------------------------------------------------------------------------------------------------------------------------
				m_strLog = "전체어절 '";
				m_strLog += pNode->pHToken->strSource;
				m_strLog += "' 사전탐색";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
				
				if(FindTheWholeWord(pNode))
				{
					pNode->pHToken->nKindError = 0;
					//분석완료
					m_strLog = "\r\n전체어절 사전에 존재. 분석완료(성공).";
					SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
				}
				else
				{
					pNode->pHToken->nKindError = -999;
					m_strLog = "\r\n전체어절 어간사전 탐색 실패";
					SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
					
					//---------------------------------------------------------------------------------------------------------------------------
					//최장일치의 어간,어미 찾기
					//---------------------------------------------------------------------------------------------------------------------------
					m_strLog = "\r\nA(어간),B(어미)의 최장일치 정보 찾기";
					SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
					LongestMatchStemAndEndOfWordAnalysis(pNode);
				}
			}
			else
			{
				//분석실패
				//분석대상 어절이 15음절 이상
				pNode->pHToken->nKindError = -999;
			}
		}

		pNode = pNode->pNext;
	}
	// 중위성 분석
	CreateAmbiScript();
}




//어간,어미모두 존재하는 경우
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
	char* pAddTwoStmeEndOfWord = NULL;//어간+어미
	char* pFirstOrLastOneChar = NULL;//문자열의 첫 또는 마지막 음절
	char* pStartToEndWord = NULL;

	bool pAllFindSyllable = false;
	bool bFindStem = false;
	bool bFindEnding = false;

	int nPos = 0;
	int nStart = 0;
	int nEnd = 0;

	m_strLog = "어간,어미모두 존재하는 경우";
	m_strLog += "\r\nA+1~B-1 사이의 각음절 어미사전조회";
	SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
	//A~B사이의 모든 음절 어미사전 존재 여부 확인
	pAllFindSyllable = ExistAllEtcWordEndOfWordAnalysis(pNode);
	//---------------------------------------------------------------------------------------------------------------------------
	//모든 음절이 어미 사전에 있을 경우 해당 음절과+B(최장일치어미)의 값을 어미로 분석한다.
	//자질정보는 복합적으로 관리해야 한다.
	//---------------------------------------------------------------------------------------------------------------------------
	if (pAllFindSyllable == true)
	{
		m_strLog = "\r\n각 음절 어미사전에 모두 존재";
		SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
		//---------------------------------------------------------------------------------------------------------------------------
		//A+1~B-1의 음절 + B(최장일치어미)를 합쳐 하나의 어미로 간주한다.
		//---------------------------------------------------------------------------------------------------------------------------
		//pCombineAllString = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, pNode->pHToken->subStem->nFindPos + 1, pNode->pHToken->subEnding->nFindPos - 1);
		pCombineAllString = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, pNode->pHToken->subStem->nFindPos + 1, strlen(pNode->pHToken->strSource));
		if(pCombineAllString != NULL && strcmp(pCombineAllString,"") != 0)
		{
			m_strLog = "\r\n(A+1)~B를 어미로 간주";
			DeleteSubTokenResult(pNode, DIC_ENDING);//기존 어미 결과 정보 삭제
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
			
			m_strLog = "\r\n(A+1)~B 까지의 각음절 정보를 복합어미로 추가";
			SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
			
			for (nStart; nStart>nEnd; nStart--)
			{
				pOneSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nStart, DIC_ENDING);
				m_strLog = "\r\n복합음절 : '";
				m_strLog += pOneSyllable;
				m_strLog += "'";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
				if (strlen(pOneSyllable) == 2) nStart = nStart - 1;//한글일 경우
				//---------------------------------------------------------------------------------------------------------------------------
				//A~B사이의  각 음절들의 정보를 따로따로 어미정보로 관리한다.
				//---------------------------------------------------------------------------------------------------------------------------
				InsertAddCompoundSubTail(pNode, pOneSyllable, DIC_ENDING);//복합어미정보 
				CHAR_DELETE(pOneSyllable);
			}
			m_strLog = "\r\n분석완료(성공).";
			SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
		}
		else
		{
			pNode->pHToken->nKindError = -999;
			m_strLog = "\r\n(A+1)~B사이의 각 음절 어미사전에 미존재";
			m_strLog = "\r\n분석완료(실패).";
			SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

			pNode->pHToken->nKindError = 0;
		}
	}
	else
	{
		//---------------------------------------------------------------------------------------------------------------------------
		//(A+1~B)정보 어간 사전에서 탐색
		//존해할 경우에 복합어로 처리한다.
		//---------------------------------------------------------------------------------------------------------------------------
		bFindStem = false;
		//속도다--> 속 + 도다 : 속도 + 다의 형태로 분석되어야함.
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
			//도시어부들이 어간:도시 + 어미:이 
			if(pNode->pHToken->subEnding->nFindPos - pNode->pHToken->subStem->nFindPos >= 2)
			{
				pStartToEndWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, pNode->pHToken->subStem->nFindPos + 1, strlen(pNode->pHToken->strSource)-1);
				m_strLog = "\r\n(A+1~B) 스템 사전 탐색 : '";
				m_strLog += pStartToEndWord;
				m_strLog += "'";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
				bFindStem = IsExistStemDic(pStartToEndWord);
				CHAR_DELETE(pStartToEndWord);
			}
			if(bFindStem == true)
			{
				m_strLog = "\r\n스템 사전에 있음 - 복합명사로 추정함";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
				//복합명사로 추정하여 최장정보에 해당하는 정보를 보관한다.
				CHAR_DELETE(pNode->pHToken->subEnding->strResult);
				CHAR_DELETE(pNode->pHToken->subEnding->strFeature);
				m_LinkedList->CreateCompoundSubNode(pNode, "STEM");
				CompoundNounDecomposition(pNode, strlen(pNode->pHToken->strSource)-1);//복합명사분해
				//리스트내용 재 정의한다. N번째의 마지막 음절과  N+1번째의 첫음절이 같을 시, N+1번째 첫 음절 삭제 처리
				UnRegisteredCompoundNounWord(pNode);
			}
			else
			{
				//---------------------------------------------------------------------------------------------------------------------------
				//(A+1)만 어미사전에 없는 경우
				//---------------------------------------------------------------------------------------------------------------------------
				//배움이
				//A+1~B-1사이의 음절 수 파악(B이후 A+1~B-1까지의 음절이 어미 사전에 존재하지 않은 경우도 존재함: '배움이'에서 '배'(어간) '이'(어미)로 분석 후 '움'은 어미사전에 없음 )
				if(pNode->pHToken->nLastFindEndingSyllable == 0)
				{
					pNode->pHToken->nLastFindEndingSyllable = pNode->pHToken->subEnding->nFindPos;
				}
				//한음절의 의미
				if ((pNode->pHToken->nLastFindEndingSyllable - 1) - pNode->pHToken->subStem->nFindPos > 0 && (pNode->pHToken->nLastFindEndingSyllable -1) - pNode->pHToken->subStem->nFindPos <= 2)
				{
					m_strLog = "\r\n(A+1)만 어미사전에 없는 경우";
					SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

					pOneSyllable = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, pNode->pHToken->subStem->nFindPos + 1, pNode->pHToken->nLastFindEndingSyllable - 1);
			
					m_strLog = "\r\n(A+1)~(B-1)사이에 한개의 음절만 존재 : ";
					m_strLog += pOneSyllable;
					SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
	
					//하나의 음절만 어미사전 분석 시에 존재하지 않았을 경우
					if (pOneSyllable  != NULL && strlen(pOneSyllable) <= 2)
					{	
						//해당음절(A+1)의 초중,종성 분리
						pChoJungChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pOneSyllable, CHAR_CHOJUNGSUNG);
						pJongChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pOneSyllable, CHAR_JONGSUNG);

						m_strLog = "\r\n(A+1)음절의 초,중,종성분리";
						m_strLog += "\r\n초중성 : '";
						m_strLog += pChoJungChar;
						m_strLog += "'";
						m_strLog += "\r\n종성 : '";
						m_strLog += pJongChar;
						m_strLog += "'";
						SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

						//ㄴ,ㄹ,ㅁ,ㅂ,ㅆ일 경우
						if (strcmp(pJongChar, "ㄴ") == 0 || strcmp(pJongChar, "ㄹ") == 0 || strcmp(pJongChar, "ㅁ") == 0 || strcmp(pJongChar, "ㅂ") == 0 || strcmp(pJongChar, "ㅆ") == 0)
						{
							m_strLog = "\r\n(A+1)음절의 종성이 'ㄴ','ㄹ','ㅁ','ㅂ','ㅆ'일 경우";
							SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

							//ㅆ일 경우 - 예외상황1
							if (strcmp(pChoJungChar, "돼") == 0 || strcmp(pChoJungChar, "해") == 0)
							{
								m_strLog = "\r\n종성이 '돼' 또는 '해'에 대한  예외상황(종성이 'ㅆ'일 경우)";
								SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

								if (strcmp(pChoJungChar, "돼") == 0)
								{
									//pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->subStem->strResult, "되", CHANGE_CHAR_SYLLABLE_LAST);
									pChangeLastChar = m_pCNAHangulUtil->GetCombineTwoStrings( pNode->pHToken->subStem->strResult, "되");
								}
								else if (strcmp(pChoJungChar, "해") == 0)
								{
									//pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->subStem->strResult, "하", CHANGE_CHAR_SYLLABLE_LAST);
									pChangeLastChar = m_pCNAHangulUtil->GetCombineTwoStrings(pNode->pHToken->subStem->strResult, "하");
								}
								pAddTwoEndOfWord = m_pCNAHangulUtil->GetCombineTwoStrings("었", pNode->pHToken->subEnding->strResult);
								pAddTwoStmeEndOfWord = m_pCNAHangulUtil->GetCombineTwoStrings(pChangeLastChar, pAddTwoEndOfWord);
								
								//전했다 어간:전 어미:다 전하었다 형태로 다시 분석되 어어야 한다.
								CHAR_DELETE(pNode->pHToken->strSource);
								pNode->pHToken->strSource = new char[strlen(pAddTwoStmeEndOfWord) + 1];
								memset(pNode->pHToken->strSource, 0x00, strlen(pAddTwoStmeEndOfWord) + 1);
								strcpy(pNode->pHToken->strSource, pAddTwoStmeEndOfWord);
								
								CHAR_DELETE(pAddTwoEndOfWord);
								CHAR_DELETE(pAddTwoStmeEndOfWord);

								LongestMatchStemAndEndOfWordAnalysis(pNode);
							}
							//ㅆ일 경우 - 예외상황2
							else	if (strcmp(pChoJungChar, "워") == 0 || strcmp(pChoJungChar, "와") == 0)
							{
								m_strLog = "\r\n종성이 '워' 또는 '와'에 대한  예외상황(종성이 'ㅆ'일 경우)";
								SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

								CHAR_DELETE(pChoJungChar);
								CHAR_DELETE(pJongChar);
								pFirstOrLastOneChar = m_pCNAHangulUtil->GetFirstOrLastOneChar(pNode->pHToken->subStem->strResult, DIC_STEM);
								pChoJungChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pFirstOrLastOneChar, CHAR_CHOJUNGSUNG);
								pJongChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pFirstOrLastOneChar, CHAR_JONGSUNG);
								pChangeLastChar = m_pCNAHangulUtil->ChangeCompulsionJongSyllable(pChoJungChar, "ㅂ");
								pChangeLastCharSyllable = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->subStem->strResult, pChangeLastChar, CHANGE_CHAR_SYLLABLE_LAST);//해당 위치의 음절 변경 //nPos(CHANGE_CHAR_SYLLABLE_FIRST,CHANGE_CHAR_SYLLABLE_LAST)에 따라 음절의 처음 또는 끝음절만 변경
								pAddTwoEndOfWord = m_pCNAHangulUtil->GetCombineTwoStrings("았", pNode->pHToken->subEnding->strResult);

								pAddTwoStmeEndOfWord = m_pCNAHangulUtil->GetCombineTwoStrings(pChangeLastCharSyllable, pAddTwoEndOfWord);

								m_strLog = "\r\n원문자열을 정보를 다음과 같이 변경 :";
								m_strLog += "'";
								m_strLog += pNode->pHToken->strSource;
								m_strLog += "' 에서 ";
								m_strLog += "'";
								m_strLog += pAddTwoStmeEndOfWord;
								m_strLog += "'으로 변경 후 최장일치 로직함수를 다시 호출한다.";
								SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
						
								CHAR_DELETE(pNode->pHToken->strSource);
								pNode->pHToken->strSource = new char[strlen(pAddTwoStmeEndOfWord) + 1];
								memset(pNode->pHToken->strSource, 0x00, strlen(pAddTwoStmeEndOfWord) + 1);
								strcpy(pNode->pHToken->strSource, pAddTwoStmeEndOfWord);

								LongestMatchStemAndEndOfWordAnalysis(pNode);
							}
							else
							{
								//(A+2)~(B-1) + B(최장일치어미)
								pCandidateSyllableEndOfWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, pNode->pHToken->nLastFindEndingSyllable, strlen(pNode->pHToken->strSource) - 1);
								pAddTwoEndOfWord = m_pCNAHangulUtil->GetCombineTwoStrings(pJongChar, pCandidateSyllableEndOfWord);

								m_strLog = "\r\n(A+1)의 종성 + B = '";
								m_strLog += pAddTwoEndOfWord;
								m_strLog += "'";
								SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

								//(0~(A+1))사이의 음절(어간사전확인)
								pCandidateStemSyllableWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, pNode->pHToken->nLastFindEndingSyllable - 1);
								//문자열의 마지막 음절을 초중성으로변경(A + (A+1)) :A+1의 경우 초중성값
								pChangeLastCharSyllable = m_pCNAHangulUtil->SetChangeSyllableWord(pCandidateStemSyllableWord, pChoJungChar, CHANGE_CHAR_SYLLABLE_LAST);

								m_strLog = "\r\nA~(A+1)의 초중성 = '";
								m_strLog += pChangeLastCharSyllable;
								m_strLog += "'";
								m_strLog += "\r\nA~(A+1)의 초중성에 해당하는 어간,(A+1)의 종성+B에 해당하는 어미 사전 조회";
								SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

								bFindStem = IsExistStemDic(pNode, pChangeLastCharSyllable);
								bFindEnding = IsExistEndingDic(pNode, pAddTwoEndOfWord);

								if (bFindStem == true && bFindEnding == true)
								{
									m_strLog = "\r\n모두 존재(성공)";
									SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
								}
								else
								{
									//A~A+1의 초중성에 해당하는 어간이 사전에 있고, 종성이 'ㅁ'일 경우 예외처리한다.
									if (strcmp(pJongChar, "ㅁ") == 0)
									{
										m_strLog = "\r\nA~(A+1의 초중성) 어간사전에 ";
										if (bFindStem == false){m_strLog += "존재";}else{m_strLog += "미존재"; }
										m_strLog += ",(A+1의종성)+B 어미사전에 ";
										if (bFindEnding == false) { m_strLog += "존재"; }else { m_strLog += "미존재"; }
										m_strLog += ",종성이 'ㅁ' 일 경우 예외 처리";
										SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

										m_LinkedList->CreateCompoundSubNode(pNode, "ENDING");
								
										DeleteSubTokenResult(pNode, DIC_STEM);//기존 어간 결과 정보 삭제
										pNode->pHToken->subStem->strResult = new char[strlen(pChangeLastCharSyllable) + 1];
										memset(pNode->pHToken->subStem->strResult, 0x00, strlen(pChangeLastCharSyllable) + 1);
										strcpy(pNode->pHToken->subStem->strResult, pChangeLastCharSyllable);

										//기분석된 어미를 복합어미에 넣는다.
										InsertAddCompoundSubTail(pNode, pNode->pHToken->subEnding->strResult, DIC_ENDING);//복합어미정보 
										InsertAddCompoundSubTail(pNode, pJongChar, DIC_ENDING);//복합어미정보 
										m_strLog = "\r\n복합음절 : '";
										m_strLog += pNode->pHToken->subEnding->strResult;
										m_strLog = "'";
										m_strLog += "\r\n복합음절 : '";
										m_strLog += pJongChar;
										m_strLog = "'";
										SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
								
										//'ㅁ'+어미 정보를 어미정보로 강제로 만든다.
										DeleteSubTokenResult(pNode, DIC_ENDING);//기존 어미 결과 정보 삭제
										pNode->pHToken->subEnding->strResult = new char[strlen(pAddTwoEndOfWord) + 1];
										memset(pNode->pHToken->subEnding->strResult, 0x00, strlen(pAddTwoEndOfWord) + 1);
										strcpy(pNode->pHToken->subEnding->strResult, pAddTwoEndOfWord);
								
										pNode->pHToken->nKindError = 0;
										//분석완료
										m_strLog = "\r\n분석완료(성공).";
										SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
									}
									else
									{
										//풀이된다 : 풀이되 + ㄴ다
										DeleteSubTokenResult(pNode, DIC_STEM);//기존 어간 결과 정보 삭제
										pNode->pHToken->subStem->strResult = new char[strlen(pChangeLastCharSyllable) + 1];
										memset(pNode->pHToken->subStem->strResult, 0x00, strlen(pChangeLastCharSyllable) + 1);
										strcpy(pNode->pHToken->subStem->strResult, pChangeLastCharSyllable);

										pNode->pHToken->nKindError = -999;
										m_strLog = "\r\n모두 존재(실패)";
										SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
										//에러처리 : 복합어표시  어간:분석성공:(사랑) 분석실패:(사랑하) 어미:요(분석성공) ㄴ가요(분석실패)
										WriteAnalysisError(pNode, pChangeLastCharSyllable, pAddTwoEndOfWord);
									}
								}
							}
						}
						else
						{
							//복합명사로 추정하여 최장정보에 해당하는 정보를 보관한다.
							m_LinkedList->CreateCompoundSubNode(pNode, "STEM");
							CompoundNounDecomposition(pNode, pNode->pHToken->subEnding->nFindPos - 1);//복합명사분해
							 //리스트내용 재 정의한다. N번째의 마지막 음절과  N+1번째의 첫음절이 같을 시, N+1번째 첫 음절 삭제 처리
							UnRegisteredCompoundNounWord(pNode);
							//복합어(미등록어)에 대한 분석이 실패 했을 겨우
							if (pNode->pHToken->pSubHeadNode != NULL && pNode->pHToken->pSubHeadNode->pNext == NULL)
							{
								pNode->pHToken->nKindError = -999;
								m_strLog = "\r\n복합어 또는 미등록어에 대한 분석(실패).";
								SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
							}
							else
							{
								m_strLog = "\r\n복합어 또는 미등록어에 대한 분석(성공).";
								H_SUBNODE* pStartNode;
								pStartNode = pNode->pHToken->pSubHeadNode->pNext;
								while (pStartNode != pNode->pHToken->pSubTailNode)
								{
									m_strLog += "\r\n 복합어 또는미등록어 : '";
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
					//A~B사이의 음절 중 A+1의 경우에만 어미사전에 없었을 경우 그외는 에러 처리한다.
					//'도시어부들이' 같은 경우 '도시'+'이'로 분석됨('어부'는 복합명사로 분석되어 '도시어부'가 되어야 하며 '들'은 어미로 합쳐서 '들이'가 되어야 함
					//---------------------------------------------------------------------------------------------------------------------------
					//어미분석 실패
					if ((pNode->pHToken->nLastFindEndingSyllable - 1) - pNode->pHToken->subStem->nFindPos > 0)
					{
						//복합어미 분석
						m_LinkedList->CreateCompoundSubNode(pNode, "ENDING");
						nStart = pNode->pHToken->nLastFindEndingSyllable - 1;
						nEnd = pNode->pHToken->subStem->nFindPos;
						for (nStart; nStart>nEnd; nStart--)
						{
							pOneSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nStart, DIC_ENDING);
							if(IsExistEndingDic(pOneSyllable) == true)
							{
								m_strLog = "\r\n복합음절 : '";
								m_strLog += pOneSyllable;
								m_strLog += "'";
								SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
								if (strlen(pOneSyllable) == 2) nStart = nStart - 1;//한글일 경우
								pNode->pHToken->subEnding->nFindPos = nStart;
								pNode->pHToken->nLastFindEndingSyllable = nStart;
								//---------------------------------------------------------------------------------------------------------------------------
								//A~B사이의  각 음절들의 정보를 따로따로 어미정보로 관리한다.
								//---------------------------------------------------------------------------------------------------------------------------
								InsertAddCompoundSubTail(pNode, pOneSyllable, DIC_ENDING);//복합어미정보 

								pCandidateSyllableEndOfWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, pNode->pHToken->nLastFindEndingSyllable, strlen(pNode->pHToken->strSource) - 1);
								DeleteSubTokenResult(pNode, DIC_ENDING);//기존 어미 결과 정보 삭제
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

						//복합명사로 추정하여 최장정보에 해당하는 정보를 보관한다.
						m_LinkedList->CreateCompoundSubNode(pNode, "STEM");
						CompoundNounDecomposition(pNode, pNode->pHToken->subEnding->nFindPos-1);//복합명사분해
						//리스트내용 재 정의한다. N번째의 마지막 음절과  N+1번째의 첫음절이 같을 시, N+1번째 첫 음절 삭제 처리
						UnRegisteredCompoundNounWord(pNode);

						//복합어(미등록어)에 대한 분석이 실패 했을 겨우
						if (pNode->pHToken->pSubHeadNode != NULL && pNode->pHToken->pSubHeadNode->pNext == NULL)
						{
							pNode->pHToken->nKindError = -999;
							m_strLog = "\r\n복합어 또는 미등록어에 대한 분석(실패).";
							SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
						}
						else
						{
							m_strLog = "\r\n복합어 또는 미등록어에 대한 분석(성공).";
							H_SUBNODE* pStartNode;
							pStartNode = pNode->pHToken->pSubHeadNode->pNext;
							while (pStartNode != pNode->pHToken->pSubTailNode)
							{
								m_strLog += "\r\n 복합어 또는미등록어 : '";
								m_strLog += pStartNode->pCSubToken->strResult;
								m_strLog += "'";
								SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

								pStartNode = pStartNode->pNext;
							}
						}
					}
					else
					{
						m_strLog = "\r\n(A+1)~(B-1)사이에 음절이 없음";
						m_strLog += "\r\n분석완료(성공)";
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

//어간의 일부만 존재하는 경우
void CNAHangulAnaly::IsOnlyStemExistSyllable(H_NODE* pNode)
{
	//더이상 분석하지 않고 에러리턴 복합명사라 추정. 
	char* pOneSyllable = NULL;
	char* pFirstOrLastOneChar = NULL;
	char* pCandidateSyllableEndOfWord = NULL;
	char* pChangeLastChar = NULL;
	char* pAddTwoStmeEndOfWord = NULL;

	int nLength = 0;
	nLength = strlen(pNode->pHToken->strSource);
	nLength = nLength - 1;
	//분석대상의 마지막 음절 조회
	pFirstOrLastOneChar = m_pCNAHangulUtil->GetFirstOrLastOneChar(pNode->pHToken->strSource, DIC_STEM);
	
	m_strLog = "\r\n어간의 일부만 존재하는 경우[어미없음]";
	m_strLog += "\r\n분석대상의 마지막 음절 조회";
	m_strLog += "\r\n마지막음절이 '케'인지 확인";
	m_strLog += "\r\n마지막음절 : '";
	m_strLog += pFirstOrLastOneChar;
	m_strLog += "'";
	SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
	
	m_strLog = "\r\n마지막음절이 '케'인진 확인 : ";
	m_strLog += "'";
	m_strLog += pFirstOrLastOneChar;
	m_strLog += "'";
	SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

	//미분석음절이 2 이상이라면 에러처리
	if (strcmp(pFirstOrLastOneChar, "케") == 0 && (nLength - (pNode->pHToken->subStem->nFindPos+1) < 2))
	{
		m_strLog = "\r\n'케' 발견 후 '케'를 '하게'로 변경하고 다시 최장일치함수를 호출한다.";
		//'케'를 '하게'로 변경한다
		pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->strSource, "하게", CHANGE_CHAR_SYLLABLE_LAST);
		CHAR_DELETE(pNode->pHToken->strSource);
		pNode->pHToken->strSource = new char[strlen(pAddTwoStmeEndOfWord) + 1];
		memset(pNode->pHToken->strSource, 0x00, strlen(pAddTwoStmeEndOfWord) + 1);
		strcpy(pNode->pHToken->strSource, pAddTwoStmeEndOfWord);

		LongestMatchStemAndEndOfWordAnalysis(pNode);
	}
	else
	{
		//복합명사로 추정하여 최장정보에 해당하는 정보를 보관한다.
		m_LinkedList->CreateCompoundSubNode(pNode, "STEM");
		CompoundNounDecomposition(pNode, strlen(pNode->pHToken->strSource)-1);//복합명사분해
		//리스트내용 재 정의한다. N번째의 마지막 음절과  N+1번째의 첫음절이 같을 시, N+1번째 첫 음절 삭제 처리
		UnRegisteredCompoundNounWord(pNode);

		//복합어(미등록어)에 대한 분석이 실패 했을 겨우
		if (pNode->pHToken->pSubHeadNode != NULL && pNode->pHToken->pSubHeadNode->pNext == NULL)
		{
			pNode->pHToken->nKindError = -999;
			m_strLog = "\r\n복합어 또는 미등록어에 대한 분석(실패).";
			SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
		}
		else
		{
			m_strLog = "\r\n복합어 또는 미등록어에 대한 분석(성공).";
			H_SUBNODE* pStartNode;
			pStartNode = pNode->pHToken->pSubHeadNode->pNext;
			while (pStartNode != pNode->pHToken->pSubTailNode)
			{
				m_strLog += "\r\n 복합어 또는미등록어 : '";
				m_strLog += pStartNode->pCSubToken->strResult;
				m_strLog += "'";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

				pStartNode = pStartNode->pNext;
			}
		}
		/*
		m_strLog = "\r\n'케'가 아니거나 미분석음절이 2 이상이라면 에러처리";
		SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

		pCandidateSyllableEndOfWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, pNode->pHToken->subStem->nFindPos + 1, nLength);
		//분석실패
		//분석대상 어절 중 일부만 사전에 존재함(스템분석실패,어미미 존재)
		WriteAnalysisError(pNode, pCandidateSyllableEndOfWord, NULL);
		pNode->pHToken->nKindError = -997; //분석대상 어절이 스템의 일부만 존재, 어미없는 경우
		*/
	}
	
	CHAR_DELETE(pOneSyllable);
	CHAR_DELETE(pFirstOrLastOneChar);
	CHAR_DELETE(pCandidateSyllableEndOfWord);
	CHAR_DELETE(pChangeLastChar);
	CHAR_DELETE(pAddTwoStmeEndOfWord);
}

//어미만 존재하는 경우(미등록어 처리 및 )
void CNAHangulAnaly::IsEndingExistSyllable(H_NODE* pNode)
{
	//복합명사라 추정하여 어미를 제외한 다음 B-1부터 오른쪽->왼쪽으로 최장일치가 되는 어간정보를 찾은 후 탐색종료. 에러리턴
	//0.B-1 분석이 안된 음절이 1음절일 경우
	//1.어미의 자질 정보 확인(N,V에 따라 다르게 처리한다)
	char* pOneSyllable = NULL;
	char* pChoJungChar = NULL;
	char* pJongChar = NULL;
	char* pCandidateSyllableEndOfWord = NULL;
	char* pAddTwoEndOfWord = NULL;
	char* pCandidateStemSyllableWord = NULL;
	char* pChangeLastCharSyllable = NULL;
	char* pChangeLastChar = NULL;
	char* pAddTwoStmeEndOfWord = NULL;//어간+어미
	char* pFirstOrLastOneChar = NULL;//문자열의 첫 또는 마지막 음절

	int nLen = 0;
	bool bFindStem = false;
	bool bFindEnding = false;

	//어미의 자질 확인
	if (pNode->pHToken->subEnding->strFeature[ENDING_DIC_N] == '1' || pNode->pHToken->subEnding->strFeature[ENDING_DIC_AMB] == '1')
	{	
		//복합명사로 추정하여 최장정보에 해당하는 정보를 보관한다.
		m_LinkedList->CreateCompoundSubNode(pNode, "STEM");
		CompoundNounDecomposition(pNode, pNode->pHToken->subEnding->nFindPos - 1);//복합명사분해
		//리스트내용 재 정의한다. N번째의 마지막 음절과  N+1번째의 첫음절이 같을 시, N+1번째 첫 음절 삭제 처리
		UnRegisteredCompoundNounWord(pNode);

		//복합어(미등록어)에 대한 분석이 실패 했을 겨우
		if (pNode->pHToken->pSubHeadNode != NULL && pNode->pHToken->pSubHeadNode->pNext == NULL)
		{
			pNode->pHToken->nKindError = -999;
			m_strLog = "복합어 또는 미등록어에 대한 분석(실패).";
			SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
		}
		else
		{
			m_strLog = "복합어 또는 미등록어에 대한 분석(성공).";
			H_SUBNODE* pStartNode;
			pStartNode = pNode->pHToken->pSubHeadNode->pNext;
			while (pStartNode != pNode->pHToken->pSubTailNode)
			{
				m_strLog += "\r\n 복합어또는미등록어 : '";
				m_strLog += pStartNode->pCSubToken->strResult;
				m_strLog += "'";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

				pStartNode = pStartNode->pNext;
			}
		}
	}
	//불규칙 정보 확인
	else if (pNode->pHToken->subEnding->strFeature[ENDING_DIC_V] == '1')
	{
		nLen = strlen(pNode->pHToken ->strSource) - strlen(pNode->pHToken->subEnding->strResult);
		//한음절의 의미
		if (nLen == 1 || nLen == 2)
		{
			pOneSyllable = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, pNode->pHToken->subEnding->nFindPos - 1);
			m_strLog = "\r\n어미 분석 후 한개의 음절만 존재 : ";
			m_strLog += pOneSyllable;
			SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
			//하나의 음절만 어미사전 분석 시에 존재하지 않았을 경우
			if (pOneSyllable != NULL && strlen(pOneSyllable) <= 2)
			{
				//해당음절(A+1)의 초중,종성 분리
				pChoJungChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pOneSyllable, CHAR_CHOJUNGSUNG);
				pJongChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pOneSyllable, CHAR_JONGSUNG);

				m_strLog = "\r\n(A+1)음절의 초,중,종성분리";
				m_strLog += "\r\n초중성 : '";
				m_strLog += pChoJungChar;
				m_strLog += "'";
				m_strLog += "\r\n종성 : '";
				m_strLog += pJongChar;
				m_strLog += "'";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

				//ㄴ,ㄹ,ㅁ,ㅂ,ㅆ일 경우
				if (strcmp(pJongChar, "ㄴ") == 0 || strcmp(pJongChar, "ㄹ") == 0 || strcmp(pJongChar, "ㅁ") == 0 || strcmp(pJongChar, "ㅂ") == 0 || strcmp(pJongChar, "ㅆ") == 0)
				{
					m_strLog = "\r\n(A+1)음절의 종성이 'ㄴ','ㄹ','ㅁ','ㅂ','ㅆ'일 경우";
					SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

					//ㅆ일 경우 - 예외상황1
					if (strcmp(pChoJungChar, "돼") == 0 || strcmp(pChoJungChar, "해") == 0 || strcmp(pChoJungChar, "봐") == 0)
					{
						m_strLog = "\r\n종성이 '돼' 또는 '해'에 대한  예외상황(종성이 'ㅆ'일 경우)";
						SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

						if (strcmp(pChoJungChar, "돼") == 0)
							pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->subStem->strResult, "되", CHANGE_CHAR_SYLLABLE_LAST);
						else if (strcmp(pChoJungChar, "해") == 0)
							pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->subStem->strResult, "하", CHANGE_CHAR_SYLLABLE_LAST);
						else if (strcmp(pChoJungChar, "봐") == 0)
							pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->subStem->strResult, "보았", CHANGE_CHAR_SYLLABLE_LAST);

						pAddTwoEndOfWord = m_pCNAHangulUtil->GetCombineTwoStrings("었", pNode->pHToken->subEnding->strResult);
						pAddTwoStmeEndOfWord = m_pCNAHangulUtil->GetCombineTwoStrings(pChangeLastChar, pAddTwoEndOfWord);

						CHAR_DELETE(pNode->pHToken->strSource);
						pNode->pHToken->strSource = new char[strlen(pAddTwoStmeEndOfWord) + 1];
						memset(pNode->pHToken->strSource, 0x00, strlen(pAddTwoStmeEndOfWord) + 1);
						strcpy(pNode->pHToken->strSource, pAddTwoStmeEndOfWord);

						LongestMatchStemAndEndOfWordAnalysis(pNode);
					}
					//ㅆ일 경우 - 예외상황2
					else	if (strcmp(pChoJungChar, "워") == 0 || strcmp(pChoJungChar, "와") == 0)
					{
						m_strLog = "\r\n종성이 '워' 또는 '와'에 대한  예외상황(종성이 'ㅆ'일 경우)";
						SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

						CHAR_DELETE(pChoJungChar);
						CHAR_DELETE(pJongChar);
						pFirstOrLastOneChar = m_pCNAHangulUtil->GetFirstOrLastOneChar(pNode->pHToken->subStem->strResult, DIC_STEM);
						pChoJungChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pFirstOrLastOneChar, CHAR_CHOJUNGSUNG);
						pJongChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pFirstOrLastOneChar, CHAR_JONGSUNG);
						pChangeLastChar = m_pCNAHangulUtil->ChangeCompulsionJongSyllable(pChoJungChar, "ㅂ");
						pChangeLastCharSyllable = m_pCNAHangulUtil->SetChangeSyllableWord(pNode->pHToken->subStem->strResult, pChangeLastChar, CHANGE_CHAR_SYLLABLE_LAST);//해당 위치의 음절 변경 //nPos(CHANGE_CHAR_SYLLABLE_FIRST,CHANGE_CHAR_SYLLABLE_LAST)에 따라 음절의 처음 또는 끝음절만 변경
						pAddTwoEndOfWord = m_pCNAHangulUtil->GetCombineTwoStrings("았", pNode->pHToken->subEnding->strResult);

						pAddTwoStmeEndOfWord = m_pCNAHangulUtil->GetCombineTwoStrings(pChangeLastCharSyllable, pAddTwoEndOfWord);

						m_strLog = "\r\n원문자열을 정보를 다음과 같이 변경 :";
						m_strLog += "'";
						m_strLog += pNode->pHToken->strSource;
						m_strLog += "' 에서 ";
						m_strLog += "'";
						m_strLog += pAddTwoStmeEndOfWord;
						m_strLog += "'으로 변경 후 최장일치 로직함수를 다시 호출한다.";
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
						//(A+2)~(B-1) + B(최장일치어미)
						pCandidateSyllableEndOfWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, pNode->pHToken->nLastFindEndingSyllable, strlen(pNode->pHToken->strSource) - 1);
						pAddTwoEndOfWord = m_pCNAHangulUtil->GetCombineTwoStrings(pJongChar, pCandidateSyllableEndOfWord);

						m_strLog = "\r\n(A+1)의 종성 + B = '";
						m_strLog += pAddTwoEndOfWord;
						m_strLog += "'";
						SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

						//(0~(A+1))사이의 음절(어간사전확인)
						pCandidateStemSyllableWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, pNode->pHToken->nLastFindEndingSyllable - 1);
						//문자열의 마지막 음절을 초중성으로변경(A + (A+1)) :A+1의 경우 초중성값
						pChangeLastCharSyllable = m_pCNAHangulUtil->SetChangeSyllableWord(pCandidateStemSyllableWord, pChoJungChar, CHANGE_CHAR_SYLLABLE_LAST);

						m_strLog = "\r\nA~(A+1)의 초중성 = '";
						m_strLog += pChangeLastCharSyllable;
						m_strLog += "'";
						m_strLog += "\r\nA~(A+1)의 초중성에 해당하는 어간,(A+1)의 종성+B에 해당하는 어미 사전 조회";
						SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

						bFindStem = IsExistStemDic(pNode, pChangeLastCharSyllable);
						bFindEnding = IsExistEndingDic(pNode, pAddTwoEndOfWord);

						if (bFindStem == true && bFindEnding == true)
						{
							m_strLog = "\r\n모두 존재(성공)";
							SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
						}
						else
						{
							//A~A+1의 초중성에 해당하는 어간이 사전에 있고, 종성이 'ㅁ'일 경우 예외처리한다.
							if (strcmp(pJongChar, "ㅁ") == 0)
							{
								m_strLog = "\r\nA~(A+1의 초중성) 어간사전에 ";
								if (bFindStem == false) { m_strLog += "존재"; }
								else { m_strLog += "미존재"; }
								m_strLog += ",(A+1의종성)+B 어미사전에 ";
								if (bFindEnding == false) { m_strLog += "존재"; }
								else { m_strLog += "미존재"; }
								m_strLog += ",종성이 'ㅁ' 일 경우 예외 처리";
								SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

								m_LinkedList->CreateCompoundSubNode(pNode, "ENDING");

								DeleteSubTokenResult(pNode, DIC_STEM);//기존 어간 결과 정보 삭제
								pNode->pHToken->subStem->strResult = new char[strlen(pChangeLastCharSyllable) + 1];
								memset(pNode->pHToken->subStem->strResult, 0x00, strlen(pChangeLastCharSyllable) + 1);
								strcpy(pNode->pHToken->subStem->strResult, pChangeLastCharSyllable);

								//기분석된 어미를 복합어미에 넣는다.
								InsertAddCompoundSubTail(pNode, pNode->pHToken->subEnding->strResult, DIC_ENDING);//복합어미정보 
								InsertAddCompoundSubTail(pNode, pJongChar, DIC_ENDING);//복합어미정보 
								m_strLog = "\r\n복합음절 : '";
								m_strLog += pNode->pHToken->subEnding->strResult;
								m_strLog = "'";
								m_strLog += "\r\n복합음절 : '";
								m_strLog += pJongChar;
								m_strLog = "'";
								SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

								//'ㅁ'+어미 정보를 어미정보로 강제로 만든다.
								DeleteSubTokenResult(pNode, DIC_ENDING);//기존 어미 결과 정보 삭제
								pNode->pHToken->subEnding->strResult = new char[strlen(pAddTwoEndOfWord) + 1];
								memset(pNode->pHToken->subEnding->strResult, 0x00, strlen(pAddTwoEndOfWord) + 1);
								strcpy(pNode->pHToken->subEnding->strResult, pAddTwoEndOfWord);

								pNode->pHToken->nKindError = 0;
								//분석완료
								m_strLog = "\r\n분석완료(성공).";
								SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
							}
							else
							{
								pNode->pHToken->nKindError = -999;
								m_strLog = "\r\n모두 존재(실패)";
								SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
								//에러처리 : 복합어표시  어간:분석성공:(사랑) 분석실패:(사랑하) 어미:요(분석성공) ㄴ가요(분석실패)
								WriteAnalysisError(pNode, pChangeLastCharSyllable, pAddTwoEndOfWord);
							}
						}
					}
				}
			}
		}
		else
		{
			if (IrregularGrammarCheck(pNode, "ㄹ") == true)
			{
				m_strLog = "ㄹ불규칙 처리 완료";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
			}
			else if (IrregularGrammarCheck(pNode, "ㅅ") == true)
			{
				m_strLog = "ㅅ불규칙 처리 완료";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
			}
			else if (IrregularGrammarCheck(pNode, "ㅎ") == true)
			{
				m_strLog = "ㅎ불규칙 처리 완료";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
			}
			else if (IrregularGrammarCheck(pNode, "ㅂ") == true)
			{
				m_strLog = "ㅂ불규칙 처리 완료";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
			}
			else if (IrregularGrammarCheck(pNode, "ㄷ") == true)
			{
				m_strLog = "ㄷ불규칙 처리 완료";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
			}
			else if (IrregularGrammarCheck(pNode, "르") == true)
			{
				m_strLog = "르불규칙 처리 완료";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
			}
			else if (IrregularGrammarCheck(pNode, "ㅜ") == true)
			{
				m_strLog = "ㅜ불규칙 처리 완료";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
			}
			else if (IrregularGrammarCheck(pNode, "러") == true)
			{
				m_strLog = "러불규칙 처리 완료";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
			}
			
			else
			{
				pNode->pHToken->nKindError = -999;
				m_strLog = "불규칙 정보 실패";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
			}
		}
	}
	else
	{
		//에러
		pNode->pHToken->nKindError = -999;
		m_strLog = "어미만 존재하는 경우(미등록어 처리) 기타 이유로 실패";
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


void CNAHangulAnaly::DeleteSubTokenResult(H_NODE* pNode, int nResult)//어간및어미의 결과 정보 삭제
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

void CNAHangulAnaly::DeleteSubTokenFeature(H_NODE* pNode, int nResult)//어간및어미의 자질 정보 삭제
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
	//에러처리 : 복합어표시  어간:분석성공:(사랑) 분석실패:(사랑하) 어미:요(분석성공) ㄴ가요(분석실패)
	pNode->pHToken->nKindError = -999;
	if(pStemWord != NULL)
	{
		#ifdef _DEBUG
		strcat(pNode->pHToken->strAnalysisError, "에러 어간[복합어] 어간:분석성공:(");
		strcat(pNode->pHToken->strAnalysisError, pNode->pHToken->subStem->strResult);
		strcat(pNode->pHToken->strAnalysisError, ")");
		strcat(pNode->pHToken->strAnalysisError, "분석실패:(");
		strcat(pNode->pHToken->strAnalysisError, pStemWord);
		strcat(pNode->pHToken->strAnalysisError, ")");
		#endif
	}
	if (pEndingWord != NULL)
	{
		#ifdef _DEBUG
		strcat(pNode->pHToken->strAnalysisError, "에러 어미[결합] 어미:분석성공:(");
		strcat(pNode->pHToken->strAnalysisError, pNode->pHToken->subEnding->strResult);
		strcat(pNode->pHToken->strAnalysisError, ")");
		strcat(pNode->pHToken->strAnalysisError, "분석실패:(");
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
		//해당 엔딩사전 최장 매칭 
		if(IsExistEndingDic(pNode, pEndingWord) == false)
		{
			CHAR_DELETE(pEndingWord);
			break;
		}
		else
		{
			pNode->pHToken->subEnding->nFindPos = nPos;
			nLen = nPos;//어미의 찾은 최종 위치의 정보를 보관한다.
			CHAR_DELETE(pEndingWord);
		}
		i = nPos;
	}
	
	//어미정보가 존재할 시 0~B-1(어미를제외한전체어절)까지 어간사전에서 액세스 한다.
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
		//최장어미정보를 찾았을 경우.. 전체문자열에서 최장어간을 찾을 필요가 없음. 찾은 어미 전까지에서만 어간정보를 액세스한다.
		if(pNode->pHToken->subEnding->strResult != NULL)
		{
			nLen = pNode->pHToken->subEnding->nFindPos;
		}
		int nNotFindNumber = 0;
		for (int i = 0; i < nLen - 1; i++)
		{
			nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, i, DIC_STEM);
			pStemWord = m_pCNAHangulUtil->GetLeftToRightWord(pNode->pHToken->strSource, nPos);
			//해당 스템사전 최장 매칭 
			if (IsExistStemDic(pNode, pStemWord) == true) 
			{
				nNotFindNumber += 1;
				pNode->pHToken->subStem->nFindPos = nPos;
			}
			else
			{
				nNotFindNumber += 1;
				if(nNotFindNumber > 5)//5음절 이상으로 분석되는 어간정보를 막는다.
				{
					CHAR_DELETE(pStemWord);
					break;
				}
			}
			CHAR_DELETE(pStemWord);
			i = nPos;
		}
	}

	//어간,어미가 모두 존재하는 경우
	if (pNode->pHToken->subStem->strResult != NULL && pNode->pHToken->subEnding->strResult != NULL)
	{
		m_strLog = "\r\nA,B 모두 어간/어미사전에 존재 ";
		m_strLog += "A : '";
		m_strLog += pNode->pHToken->subStem->strResult;
		m_strLog += "'";
		m_strLog += ", B : '";
		m_strLog += pNode->pHToken->subEnding->strResult;
		m_strLog += "'";
		SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

		IsStemEndingExistSyllable(pNode);
	}
	//어간만 존재하는 경우
	else if (pNode->pHToken->subStem->strResult != NULL && pNode->pHToken->subEnding->strResult == NULL)
	{
		m_strLog = "\r\nA만 어간사전에 존재, B(어미사전)미존재 ";
		m_strLog += "\r\n A : '";
		m_strLog += pNode->pHToken->subStem->strResult;
		m_strLog += "'";
		SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
		IsOnlyStemExistSyllable(pNode);
	}
	//어미만 존재하는 경우(불규칙에 대한 처리)
	else if (pNode->pHToken->subStem->strResult == NULL && pNode->pHToken->subEnding->strResult != NULL)
	{
		m_strLog = "\r\nB만 어미사전에 존재, A(어간사전)미존재";
		m_strLog += "B : '";
		m_strLog += pNode->pHToken->subEnding->strResult;
		m_strLog += "'";
		SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
		IsEndingExistSyllable(pNode);
	}
	else
	{
		//분석실패
		//분석대상 어절 모두 사전에 미 존재(스템,엔딩에 없음)
		//pNode->pHToken->nKindError = -998;
		//복합명사로 추정하여 최장정보에 해당하는 정보를 보관한다.
		m_LinkedList->CreateCompoundSubNode(pNode, "STEM");
		CompoundNounDecomposition(pNode, strlen(pNode->pHToken->strSource)-1);//복합명사분해
		//리스트내용 재 정의한다. N번째의 마지막 음절과  N+1번째의 첫음절이 같을 시, N+1번째 첫 음절 삭제 처리
		UnRegisteredCompoundNounWord(pNode);
	}

}


//Linked List에 SubTail 추가(복합어간 또는 복합어미)
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
					pInsertNode->pCSubToken->strFeature[STEM_DIC_N] = '1';//N명사자질부여
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
					pInsertNode->pCSubToken->strFeature[ENDING_DIC_N] = '1';//N 조사자질부여
				}
				pInsertNode->pPrev = pNode->pHToken->pSubEndingTailNode->pPrev;
				pInsertNode->pNext = pNode->pHToken->pSubEndingTailNode;
				pNode->pHToken->pSubEndingTailNode->pPrev->pNext = pInsertNode;
				pNode->pHToken->pSubEndingTailNode->pPrev = pInsertNode;
			}
			
		}	
	}
}

//A~B사이의 모든 음절 어미사전 존재 여부 확인
bool CNAHangulAnaly::ExistAllEtcWordEndOfWordAnalysis(H_NODE* pNode)
{
	//어간과 어미 사이의 모든 음절을 검사한다.
	bool pAllFindSyllable = false;
	int nStart = pNode->pHToken->subEnding->nFindPos - 1;
	int nEnd = pNode->pHToken->subStem->nFindPos;
	for (nStart; nStart>nEnd; nStart--)
	{
		char* pOneSyllable = NULL;
		//(A(어간의최장일치)->B(어미의최장일치) 사이의 값을 읽음.
		//왼쪽에서 오른쪽으로 값을 액세스 하므로 DIC_STEM을 사용함)
		pOneSyllable = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, nStart, DIC_ENDING);
		if (strlen(pOneSyllable) == 2) nStart = nStart - 1;//한글일 경우
		
		m_strLog = "\r\n음절 : ";
		
		if (IsExistEndingDic(pOneSyllable) == false)
		{
			m_strLog += "'";
			m_strLog += pOneSyllable;
			m_strLog += "'";
			m_strLog +=  "-없음";
			SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
			CHAR_DELETE(pOneSyllable);
			pAllFindSyllable = false;
			//최장음절 실패시 마지막 음절 위치를 원래 값으로 돌린다.
			pNode->pHToken->nLastFindEndingSyllable =  pNode->pHToken->subEnding->nFindPos;
			break;
		}
		else
		{
			m_strLog += "'";
			m_strLog += pOneSyllable;
			m_strLog += "'";
			m_strLog += "-있음";
			SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
			pAllFindSyllable = true;

			//A~B사이의 오른쪽에서 왼쪽으로 분석한 마지막 어미 음절 보관
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
				m_strLog = "\r\n중의성분석 전 ";
				m_strLog += "어간/어미 :'";
				m_strLog += "'";
				m_strLog += pNode->pHToken->subStem->strResult;
				m_strLog += "'/'";
				m_strLog += pNode->pHToken->subEnding->strResult;
				m_strLog += "'";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

				//m_pAmbiguity->GoAmbiguityFunction(this, pNode, 3);

				m_strLog = "\r\n중의성분석 완료 후 ";
				m_strLog += "어간/어미 :'";
				m_strLog += "'";
				m_strLog += pNode->pHToken->subStem->strResult;
				m_strLog += "'/'";
				m_strLog += pNode->pHToken->subEnding->strResult;
				m_strLog += "'";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

				//임시적으로 주석 처리함.(오류처리를 복원하는 작업인 듯...)
				//RestorIrrigual(m_pNode);
				pNode = pNode->pNext;
				m_nListPosition -= 1;
			}
			else if (isStemAmbCheck == true && isEndingAmbCheck == false)
			{
				m_strLog = "\r\n중의성분석 전 ";
				m_strLog += "어간/어미 :'";
				m_strLog += "'";
				m_strLog += pNode->pHToken->subStem->strResult;
				m_strLog += "'/'";
				m_strLog += pNode->pHToken->subEnding->strResult;
				m_strLog += "'";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

				//m_pAmbiguity->GoAmbiguityFunction(this, pNode, 2);
	
				m_strLog = "\r\n중의성분석 완료 후 ";
				m_strLog += "어간/어미 :'";
				m_strLog += "'";
				m_strLog += pNode->pHToken->subStem->strResult;
				m_strLog += "'/'";
				m_strLog += pNode->pHToken->subEnding->strResult;
				m_strLog += "'";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

				//임시적으로 주석 처리함.(오류처리를 복원하는 작업인 듯...)
				//RestorIrrigual(m_pNode);
				pNode = pNode->pNext;
				m_nListPosition -= 1;
			}
			else if (isStemAmbCheck == false && isEndingAmbCheck == true)
			{
				m_strLog = "\r\n중의성분석 전 ";
				m_strLog += "어간/어미 :'";
				m_strLog += "'";
				m_strLog += pNode->pHToken->subStem->strResult;
				m_strLog += "'/'";
				m_strLog += pNode->pHToken->subEnding->strResult;
				m_strLog += "'";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
				
				//m_pAmbiguity->GoAmbiguityFunction(this, pNode, 1);
				
				m_strLog = "\r\n중의성분석 완료 후 ";
				m_strLog += "어간/어미 :'";
				m_strLog += "'";
				m_strLog += pNode->pHToken->subStem->strResult;
				m_strLog += "'/'";
				m_strLog += pNode->pHToken->subEnding->strResult;
				m_strLog += "'";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);

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

//복합명사분해
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
	
	
	//현사전구성정보 : 1음절 : 970 2음절 : 22, 720 3음절 : 29, 132 4음절 : 9, 941 5음절 : 2, 750 
	//2, 3, 4음절이 대부분의 사전을 차지함으로 복합명사 미등록어 분석 기준을 2,3,4음절만 대상으로 한다.
	if (nEndPosition < 0) return;
	
	strSource = new char[nEndPosition+2];
	memset(strSource,0x00, nEndPosition + 2);
	strncpy(strSource, pNode->pHToken->strSource, nEndPosition+1);
	//0~nEndPosition까지의 글자수 확인한다.
	nSyllableCount = m_pCNAHangulUtil->GetFirstToEndSyllableCount(strSource, nEndPosition);
	
	//2음절 이하인 경우 분석로직에서 제외한다.(한음절이 남은 경우, 가장 최근에 분석한 미등록어에 해당 값을 합친다)
	if(nSyllableCount < 2)
	{ 
		if(pNode->pHToken->pSubHeadNode != NULL && pNode->pHToken->pSubHeadNode->pNext != NULL)
		{
			//한음절이 되었을시 해당 한음절과 비교 노드의 처음 음절비교 같은 경우 strSource의 값을 더하지 않고 로직을 처리한다.
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
					InsertAddCompoundSubTail(pNode, strSource, DIC_STEM);//분석에 실패한 죄초복합어간정보 
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
	//4음절 이상은 분석하지 않는다.(최장4음절까지만 분석)
	if(nSyllableCount > 4) nSyllableCount = 4;
	for (i = nEndPosition; i >= 0; i--)
	{
		if (nSyllableCount < 1) break;//2음절 이하의 경우 버린다.
		nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(strSource, i, DIC_ENDING);
		pStemWord = m_pCNAHangulUtil->GetRightToLeftWord(strSource, nPos);
		if(pStemWord != NULL)
		{
			nFewPositions += 1;//몇번째에 해당는 단어
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
	//2,3,4음절에 해당하는 정보까지 모두 사전에 없을 경우 미등록로 최장에 해당하는 정보를 미등록로 넣어 둔다.
	if(bFind4 == false && bFind3 == false && bFind2 == false)
	{
		nEndPosition = i;
		if(pWord4 != NULL)InsertAddCompoundSubTail(pNode, pWord4, DIC_STEM);//분석에 실패한 죄초복합어간정보 
		else if (pWord3 != NULL) InsertAddCompoundSubTail(pNode, pWord3, DIC_STEM);//분석에 실패한 죄초복합어간정보 
		else if (pWord2 != NULL) InsertAddCompoundSubTail(pNode, pWord2, DIC_STEM);//분석에 실패한 죄초복합어간정보 
		//분석 모두 실패한 경우,  실패한 어절 중 최장정보의 첫 음절부터 다시 분석되게 nEndPosition값을 재조정한다.
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
			InsertAddCompoundSubTail(pNode, pWord4, DIC_STEM);//복합어간정보 
		}
		else if (bFind3 == true)
		{
			InsertAddCompoundSubTail(pNode, pWord3, DIC_STEM);//복합어간정보 
		}
		else if (bFind2 == true)
		{
			InsertAddCompoundSubTail(pNode, pWord2, DIC_STEM);//복합어간정보 
		}
	}

	CHAR_DELETE(pAddTwoWord);
	CHAR_DELETE(pFirstSyllable);
	CHAR_DELETE(pStemWord);
	CHAR_DELETE(strSource);
	CHAR_DELETE(pWord2);
	CHAR_DELETE(pWord3);
	CHAR_DELETE(pWord4);
	
	//재귀호출
	CompoundNounDecomposition(pNode, nEndPosition);
	
}

//N번째의 마지막 음절과  N+1번째의 첫음절이 같을 시, N+1번째 첫 음절 삭제 처리
void CNAHangulAnaly::UnRegisteredCompoundNounWord(H_NODE *pNode)
{
	H_SUBNODE* pStartNode;
	H_SUBNODE* pNextNode;
	char* pLastSyllable = NULL; //기준음절
	char* pFirstSyllable = NULL;//비교음절
	char* pMuchSyllable = NULL;//해당음절로 대체
	char* pStemResultWords = NULL;//스템변경값보관
	char* pAddStemResultWords = NULL;
	int nWhile = 0;
	int nPos = 0;
	int nLen = 0;
	int nArrayLen = 0;
	//복합어 또는 미등록어에 대한 분석처리가 존재할 경우에만 처리함.
	if(pNode->pHToken->bSubCompCreate == true)
	{
		nArrayLen =  strlen(pNode->pHToken->strSource) + 1;
		pAddStemResultWords = new char[nArrayLen];
		memset(pAddStemResultWords,0x00, nArrayLen);
		
		//어간처리
		if (pNode->pHToken->pSubHeadNode != NULL && pNode->pHToken->pSubHeadNode->pNext != NULL)
		{
			pStartNode = pNode->pHToken->pSubHeadNode->pNext;
			while (pStartNode != pNode->pHToken->pSubTailNode)
			{
				//복합어에 대한 문자정보 계속 합친다.
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
					//기준 노드의 마지막 음절
					nLen = strlen(pStartNode->pCSubToken->strResult)-1;
					nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pStartNode->pCSubToken->strResult, nLen, DIC_ENDING);
					pLastSyllable = m_pCNAHangulUtil->GetRightToLeftWord(pStartNode->pCSubToken->strResult, nPos);
					//비교 노드의 처음 음절
					nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNextNode->pCSubToken->strResult, 0, DIC_STEM);
					pFirstSyllable = m_pCNAHangulUtil->GetLeftToRightWord(pNextNode->pCSubToken->strResult, nPos);
					//(N이'필리핀',N+1이'핀카지노' 일 경우	N의 마지막 음절과 N+1의 첫 음절이 같음으로 N+1의 첫 음절을 삭제처리한다.
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

		//미등록어전부를 스템의 결과값을 변경한다.각 분리한 값을 하나로 합쳐 하나의 스템으로 분석된 것처럼 보이게 처리한다.
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
			pNode->pHToken->subStem->strFeature[STEM_DIC_N] = '1';//N명사자질부여

			m_strLog = "\r\n복합명사로 분석된 정보를 하나로 합쳐 미등록어에 해당하는 어간으로 재 구성";
			SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
		}
	}

	CHAR_DELETE(pLastSyllable);
	CHAR_DELETE(pFirstSyllable);
	CHAR_DELETE(pMuchSyllable);
	CHAR_DELETE(pStemResultWords);
	CHAR_DELETE(pAddStemResultWords);
}

//불규칙처리(ㄹ,ㅅ,러,ㅎ,ㅅ,르,ㄷ,ㅜ,ㅂ)
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

	pFirstOrLastOneChar = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, pNode->pHToken->subEnding->nFindPos - 1, DIC_ENDING);//해당 위치의 음절 값
	pChoJungChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pFirstOrLastOneChar, CHAR_CHOJUNGSUNG);
	pJongChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pFirstOrLastOneChar, CHAR_JONGSUNG);

	if (pJongChar != NULL && strcmp(pJongChar, "") != 0)
	{
		if(strcmp(pGrammer,"ㄹ") == 0)//ㄹ불규칙
		{
			pChangeLastChar = m_pCNAHangulUtil->ChangeCompulsionJongSyllable(pChoJungChar, "ㄹ");//ㄹ불규칙
			pCandidateStemSyllableWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, pNode->pHToken->subEnding->nFindPos - 1);
			pFindStemSyllableWord = m_pCNAHangulUtil->SetChangeSyllableWord(pCandidateStemSyllableWord, pChangeLastChar, CHANGE_CHAR_SYLLABLE_LAST);//문자열의 마지막 음절을 초중성으로변경
			bFind = IsExistStemDic(pNode, pFindStemSyllableWord);
			
			if(bFind == true) m_strLog = "\r\n'ㄹ'불규칙 사전(있음) : '"; else m_strLog = "\r\n'ㄹ'불규칙 사전(없음) : '";
			m_strLog += pFindStemSyllableWord;
			m_strLog += "'";
			SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
		}
		else if (strcmp(pGrammer, "ㅅ") == 0)//ㅅ불규칙
		{
			pChangeLastChar = m_pCNAHangulUtil->ChangeCompulsionJongSyllable(pChoJungChar, "ㅅ");//ㅅ불규칙
			pCandidateStemSyllableWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, pNode->pHToken->subEnding->nFindPos - 1);
			pFindStemSyllableWord = m_pCNAHangulUtil->SetChangeSyllableWord(pCandidateStemSyllableWord, pChangeLastChar, CHANGE_CHAR_SYLLABLE_LAST);//문자열의 마지막 음절을 초중성으로변경
			bFind = IsExistStemDic(pNode, pFindStemSyllableWord);

			if (bFind == true) m_strLog = "\r\n'ㅅ'불규칙 사전(있음) : '"; else m_strLog = "\r\n'ㅅ'불규칙 사전(없음) : '";
			m_strLog += pFindStemSyllableWord;
			m_strLog += "'";
			SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
		 }
		else if (strcmp(pGrammer, "ㅎ") == 0)//ㅎ불규칙
		{
			pChangeLastChar = m_pCNAHangulUtil->ChangeCompulsionJongSyllable(pChoJungChar, "ㅎ");//ㅎ불규칙
			pCandidateStemSyllableWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, pNode->pHToken->subEnding->nFindPos - 1);
			pFindStemSyllableWord = m_pCNAHangulUtil->SetChangeSyllableWord(pCandidateStemSyllableWord, pChangeLastChar, CHANGE_CHAR_SYLLABLE_LAST);//문자열의 마지막 음절을 초중성으로변경
			bFind = IsExistStemDic(pNode, pFindStemSyllableWord);

			if (bFind == true) m_strLog = "\r\n'ㅎ'불규칙 사전(있음) : '"; else m_strLog = "\r\n'ㅎ'불규칙 사전(없음) : '";
			m_strLog += pFindStemSyllableWord;
			m_strLog += "'";
			SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
		}
		else if (strcmp(pGrammer, "ㅂ") == 0)//ㅂ불규칙
		{
			pChangeLastChar = m_pCNAHangulUtil->ChangeCompulsionJongSyllable(pChoJungChar, "ㅂ");//ㅂ불규칙
			pCandidateStemSyllableWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, pNode->pHToken->subEnding->nFindPos - 1);
			pFindStemSyllableWord = m_pCNAHangulUtil->SetChangeSyllableWord(pCandidateStemSyllableWord, pChangeLastChar, CHANGE_CHAR_SYLLABLE_LAST);//문자열의 마지막 음절을 초중성으로변경
			bFind = IsExistStemDic(pNode, pFindStemSyllableWord);

			if (bFind == true) m_strLog = "\r\n'ㅂ'불규칙 사전(있음) : '"; else m_strLog = "\r\n'ㅂ'불규칙 사전(없음) : '";
			m_strLog += pFindStemSyllableWord;
			m_strLog += "'";
			SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
		}
		else if (strcmp(pGrammer, "ㄷ") == 0)//ㄷ불규칙
		{
			pChangeLastChar = m_pCNAHangulUtil->ChangeCompulsionJongSyllable(pChoJungChar, "ㄷ");//ㄷ불규칙
			pCandidateStemSyllableWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, pNode->pHToken->subEnding->nFindPos - 1);
			pFindStemSyllableWord = m_pCNAHangulUtil->SetChangeSyllableWord(pCandidateStemSyllableWord, pChangeLastChar, CHANGE_CHAR_SYLLABLE_LAST);//문자열의 마지막 음절을 초중성으로변경
			bFind = IsExistStemDic(pNode, pFindStemSyllableWord);

			if (bFind == true) m_strLog = "\r\n'ㄷ'불규칙 사전(있음) : '"; else m_strLog = "\r\n'ㄷ'불규칙 사전(없음) : '";
			m_strLog += pFindStemSyllableWord;
			m_strLog += "'";
			SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
		}
		else if (strcmp(pGrammer, "르") == 0)//르불규칙('르'를 붙여 탐색 : 어간 말음이 '르'로 끝나는 많은 동사들이 '르불규칙' 동사에 해당)
		{
			pChangeLastChar = m_pCNAHangulUtil->ChangeCompulsionJongSyllable(pChoJungChar, "");//르불규칙
			pCandidateStemSyllableWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, pNode->pHToken->subEnding->nFindPos - 1);
			pFindStemSyllableWord = m_pCNAHangulUtil->SetChangeSyllableWord(pCandidateStemSyllableWord, pChangeLastChar, CHANGE_CHAR_SYLLABLE_LAST);//문자열의 마지막 음절을 초중성으로변경
			bFind = IsExistStemDic(pNode, pFindStemSyllableWord);

			if (bFind == true) m_strLog = "\r\n'르'불규칙 사전(있음) : '"; else m_strLog = "\r\n'르'불규칙 사전(없음) : '";
			m_strLog += pFindStemSyllableWord;
			m_strLog += "'";
			SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
		}
		else if (strcmp(pGrammer, "ㅜ") == 0)///ㅜ불규칙(종성에서 'ㅓ'를 떼고 초성이 'ㅍ'이면 '푸' 아니면 남은 초성에 'ㅡ'붙여 어간사전 탐색)
		{
			CHAR_DELETE(pChoChar);
			CHAR_DELETE(pJungChar);
			CHAR_DELETE(pJongChar);
			pChoChar = new char[3]; memset(pChoChar, 0x00, 3);
			pJungChar = new char[3]; memset(pJungChar, 0x00, 3);
			pJongChar = new char[3]; memset(pJongChar, 0x00, 3);
			m_pCNAHangulUtil->GetWordsChoJungJongSyllable(pFirstOrLastOneChar, pChoChar, pJungChar, pJongChar);
			if (strcmp(pJungChar, "어") == 0)
			{
				CHAR_DELETE(pOneChangeChar);
				pOneChangeChar = new char[3];
				memset(pOneChangeChar, 0x00, 3);
				if (pChoChar != NULL && strcmp(pChoChar, "ㅍ") == 0)
				{
					//'퍼'->'푸'로 변경
					m_pCNAHangulUtil->ChangeCompulsionChoJungJongSyllable(pOneChangeChar, pChoChar, "ㅜ", NULL);
				}
				else
				{
					//'껏'->'끄'로 변경
					m_pCNAHangulUtil->ChangeCompulsionChoJungJongSyllable(pOneChangeChar, pChoChar, "ㅡ", NULL);
				}
				pFindStemSyllableWord = m_pCNAHangulUtil->SetChangeSyllableWord(pCandidateStemSyllableWord, pOneChangeChar, CHANGE_CHAR_SYLLABLE_LAST);//문자열의 마지막 음절을 초중성으로변경
				bFind = IsExistStemDic(pNode, pFindStemSyllableWord);

				if (bFind == true) m_strLog = "\r\n'ㅜ'불규칙 사전(있음) : '"; else m_strLog = "\r\n'ㅜ'불규칙 사전(없음) : '";
				m_strLog += pFindStemSyllableWord;
				m_strLog += "'";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
			}
		}
		else if (strcmp(pGrammer, "러") == 0)//러불규칙(종성에서 'ㄹ'을 떼고 '르' :  동사중 이르다만 '러불규칙'이다 형용사 중에는 '푸르다','노르다','누르다')
		{
			if (pJongChar != NULL && strcmp(pJongChar, "ㄹ") == 0)
			{
				pCandidateStemSyllableWord = m_pCNAHangulUtil->GetStartToEndWord(pNode->pHToken->strSource, 0, pNode->pHToken->subEnding->nFindPos - 1);
				pFindStemSyllableWord = m_pCNAHangulUtil->SetChangeSyllableWord(pCandidateStemSyllableWord, "르", CHANGE_CHAR_SYLLABLE_LAST);//문자열의 마지막 음절을 초중성으로변경
				bFind = IsExistStemDic(pNode, pFindStemSyllableWord);

				if (bFind == true) m_strLog = "\r\n'러'불규칙 사전(있음) : '"; else m_strLog = "\r\n'러'불규칙 사전(없음) : '";
				m_strLog += pFindStemSyllableWord;
				m_strLog += "'";
				SaveLogs(pNode, (LPSTR)(LPCSTR)m_strLog);
			}
			else
			{
				m_strLog = "\r\n'러'불규칙 종성이 'ㄹ'이 아니므로 분석에서 제외함";
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
	
	//어절의 전체 음절갯수 파악
	//nPos = m_pCNAHangulUtil->GetFirstToEndSyllableCount(pNode->pHToken->strSource, nLen-1);
	
	//복합명사 분리 함수
	m_LinkedList->CreateCompoundSubNode(pNode, "STEM");
	CompoundNounDecomposition(pNode, strlen(pNode->pHToken->strSource) - 1);//복합명사분리 함수
	//리스트내용 재 정의한다. N번째의 마지막 음절과  N+1번째의 첫음절이 같을 시, N+1번째 첫 음절 삭제 처리
	UnRegisteredCompoundNounWord(pNode);
	
	

	//초중성+종성 결함
	pStemWord= m_pCNAHangulUtil->ChangeCompulsionJongSyllable("가", "ㄹ");
	CHAR_DELETE(pStemWord);

	for(int i = 0; i <= nLen-1; i++)
	{
		nPos = m_pCNAHangulUtil->GetCheckNnumberBytesCharacter(pNode->pHToken->strSource, i, DIC_STEM);
		pStemWord = m_pCNAHangulUtil->GetLeftToRightWord(pNode->pHToken->strSource, nPos);
		//해당 인덱스(배열)의 한음절 조회
		char* pOneStemChar = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, i, DIC_STEM);
		//해당 인덱스(배열)의 한음절의 초중성 조회
		char* pChoJungStemChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pOneStemChar, CHAR_CHOJUNGSUNG);
		//해당 인덱스(배열)의 한음절의 종성 조회
		char* pJongStemChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pOneStemChar, CHAR_JONGSUNG);
		//문자열의 마지막 음절을 초중성으로변경
		char* pChangeLastChar = m_pCNAHangulUtil->SetChangeSyllableWord(pStemWord, pChoJungStemChar, CHANGE_CHAR_SYLLABLE_LAST);		
		
		CHAR_DELETE(pOneStemChar);
		CHAR_DELETE(pChoJungStemChar);
		CHAR_DELETE(pJongStemChar);

		CHAR_DELETE(pChangeLastChar);
		//CHAR_DELETE(pChangeFirstChar);
		//해당 스템사전 최장 매칭 
		//최장값 선택 후 i+1의 초중성 확인 로직 필요
		//i의 값 i+1의 초중성 합친 후 마지막 확인 작업 필요
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
		//해당 인덱스(배열)의 한음절 조회
		char* pOneEndingChar = m_pCNAHangulUtil->GetPositionOneChar(pNode->pHToken->strSource, i, DIC_ENDING);
		//해당 인덱스(배열)의 한음절의 초중성 조회
		char* pChoJungEndingChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pOneEndingChar, CHAR_CHOJUNGSUNG);
		//해당 인덱스(배열)의 한음절의 종성 조회
		char* pJongEndingChar = m_pCNAHangulUtil->GetWordsChoJungORJongSyllable(pOneEndingChar, CHAR_JONGSUNG);
		//문자열의 첫 음절을  해당 종성값으로변경
		char* pChangeFirstChar = m_pCNAHangulUtil->SetChangeSyllableWord(pEndingWord, pJongEndingChar, CHANGE_CHAR_SYLLABLE_FIRST);

		CHAR_DELETE(pChangeFirstChar);
		CHAR_DELETE(pOneEndingChar);
		CHAR_DELETE(pChoJungEndingChar);
		CHAR_DELETE(pJongEndingChar);

		//해당 엔딩사전 최장 매칭 
		//최장값 선택 후 i-1의 종성 확인 로직 필요
		//i-1의 종성과 i의 값 합친 후 마지막 확인 작업 필요
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
