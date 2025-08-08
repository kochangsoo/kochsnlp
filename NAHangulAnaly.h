#pragma once



#include "common.h"
#include "StemEndDic.h"
#include "ds_list.h"
#include "ds_htoken.h"
#include "na_convert.h"
#include "NAAmbiguity.h"
#include "NADocvector.h"
#include "NAHangulUtil.h"

#define MAX_BUFFER 50

class CStemEndDic;
class CNAAmbiguity;

class CNAHangulAnaly
{
public:
	CNAHangulAnaly();
	~CNAHangulAnaly();

	
public:
	CStemEndDic* m_pStemEndDic;
	CConvert *m_pConvert;
	CNAAmbiguity* m_pAmbiguity;
	CNADocVector* m_LinkedList;
	CNAHangulUtil* m_pCNAHangulUtil;

	unsigned int m_nListPosition;
	unsigned int m_nListCount;

//스템 및 엔딩 사전 탐색 
public:
	bool IsExistStemDic(H_NODE* pNode, char *pStemWord);
	bool IsExistStemDic(H_SUBNODE* pNode, char *pStemWord);//복합어간 정보 사전에서 확인
	bool IsExistEndingDic(H_NODE* pNode, char *pEndingWord);
	bool IsExistEndingDic(H_SUBNODE* pNode, char *pEndingOneSyllable);//복합어미의 정보 사전에서 확인
	bool IsExistEndingDic(char *pEndingOneSyllable);
	
	//중의성에서 사용하는 함수(더이상 사용 안함.. 중의성 클래스에서 해당 함수에 대한 부분 찾아 모두 주석처리한다. 위의 함수를 사용할 수 있게 변경한다)
	bool IsExistStemDic(char *IsExistStemDic, char *pSaveBuffer);
	bool IsExistStemDic(char *pStemSeveralSyllables);
	
	
	
//디버그 관련 함수 실행 로직 저장
public:
	CString m_strLog;
	void SaveLogs(H_NODE *pNode, char* pCallFunc);
	void WriteAnalysisError(H_NODE *pNode,char *pStemWord, char *pEndingWord);

//한글분석 시작
public:
	void StartHangulAnaly();//한글분석
	bool CreateAmbiScript();//중의성분석
	

public:
	bool FindTheWholeWord(H_NODE* pNode);//전체어절
	void LongestMatchStemAndEndOfWordAnalysis(H_NODE* pNode);//최장일치의 어간,어미정보

	bool ExistAllEtcWordEndOfWordAnalysis(H_NODE* pNode);//A~B사이의 모든 음절 어미사전 존재 여부 확인


	void IsStemEndingExistSyllable(H_NODE* pNode);//어간어미모두 존재하는 경우
	void IsOnlyStemExistSyllable(H_NODE* pNode);//어간만 존재하는 경우
	void IsEndingExistSyllable(H_NODE* pNode);//어미만 존재하는 경우

	void DeleteSubTokenResult(H_NODE* pNode,int nResult);//어간및어미의 결과 정보 삭제
	void DeleteSubTokenFeature(H_NODE* pNode, int nResult);//어간및어미의 자질 정보 삭제
	
	

public:
	void InsertAddCompoundSubTail(H_NODE *pNode, char *pString, int strFlag); //Linked List에 SubTail 추가(복합어간 또는 복합어미)
	void CompoundNounDecomposition(H_NODE *pNode, int nEndPosition);//복합명사분해
	void UnRegisteredCompoundNounWord(H_NODE *pNode);//N번째의 마지막 음절과  N+1번째의 첫음절이 같을 시, N+1번째 첫 음절 삭제 처리

public:
	bool IrregularGrammarCheck(H_NODE *pNode,char* pGrammer);//불규칙처리(ㄹ,ㅅ,러,ㅎ,ㅅ,르,ㄷ,ㅜ,ㅂ)


public:
	void NAHangulUtilTesting(H_NODE* pNode);
};

