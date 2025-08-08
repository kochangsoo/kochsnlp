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

class CMorphoLogicalAnalysis
{
public:
	CMorphoLogicalAnalysis();
	virtual ~CMorphoLogicalAnalysis();

	//사전
public:
	CStemEndDic* m_pStemEndDic;
	CConvert *m_pConvert;
	CNAAmbiguity* m_pAmbiguity;
	CNADocVector* m_LinkedList;
	CNAHangulUtil* m_pCNAHangulUtil;

	unsigned int m_nListPosition;
	unsigned int m_nListCount;


	//함수 내의 다음 모듈 적용 여부 결정
public:
	char* m_AnalyzeWhatCallFunction;//4가지 함수 중 어떤 함수 호출 정보 보관(최장일치를 다시 호출 시에 처음함수와 다른 함수를 호출할 경우 처리)
	bool m_AnalyzeAgainFromTheBeginning;//최장일치함수 재 호출
	bool m_ApplyIrregularAlgorithm;//불규칙알고리즘 적용 여부
	

	//스템 및 엔딩 사전 탐색 
public:
	bool IsExistStemDic(H_NODE* pNode, char *pStemWord);
	//중의성에서 사용하는 함수(더이상 사용 안함.. 중의성 클래스에서 해당 함수에 대한 부분 찾아 모두 주석처리한다. 위의 함수를 사용할 수 있게 변경한다)
	bool IsExistStemDic(char *IsExistStemDic, char *pSaveBuffer);
	bool IsExistStemDic(char *pStemSeveralSyllables);
	bool IsExistEndingDic(H_NODE* pNode, char *pEndingWord);
	bool IsExistEndingDic(char *pEndingOneSyllable, int nReverse);//0:우좌[해당 단어를 REVERSE해야 한다] 1:좌우
	bool IsExistStemDic(H_SUBNODE* pNode, char *pStemWord);//복합어간 정보 사전에서 확인
	bool IsExistEndingDic(H_SUBNODE* pNode, char *pEndingOneSyllable);//복합어미의 정보 사전에서 확인


	//한글분석 시작
public:
	void StartMorphoLogicalAnalysis();//한글분석의 시작
	bool CreateAmbiScript();//중의성분석


	//분석모듈 A:어간 B:어미 ,문자열 위치는 0부터 시작
public:
	bool AnalysisModule_00(H_NODE* pNode);//전체어절 어간사전 탐색 모듈
	bool AnalysisModule_01(H_NODE* pNode);//최장일치(어간 및 어미 분석 후보 결정) 탐색 모듈


	//최장일치의 결과에 따른 4가지 타입 발생
public:
	void OfTheFourLogic_FF(H_NODE* pNode); //A,B 모두 분석 실패
	void OfTheFourLogic_AB(H_NODE* pNode);//A,B모두 존재
	void OfTheFourLogic_XB(H_NODE* pNode);//B만 존재
	void OfTheFourLogic_AX(H_NODE* pNode);//A만 존재

	//전처리로직
public:
	void BeforeModule_01(H_NODE* pNode);//(1모듈)   : B가 존재 시 B-1에 대한 처리(를)처리

	//예외처리로직
public:
	void ExceptionModule_00(H_NODE* pNode);//(0모듈) : 어절의 마지막 음절변경처리모듈(케->하게)
	void ExceptionModule_01(H_NODE* pNode);//(1모듈) : 어절의 마지막 음절의 종성처리모듈(ㄴ,ㄹ,ㅁ,ㅂ,ㅆ)
	void ExceptionModule_02(H_NODE* pNode);//(2모듈) : 선어말 어미 처리 모듈(B가 V,AMB+(V or N)일 때 선어말어미 : '시','었','겠','더'등등)
	
	//불규칙처리의 경우 반드시 어미로 분석된 정보가 있어야 한다.(V자질확인)
	void ExceptionModule_03(H_NODE* pNode);//(3모듈) : 불규칙처리 모듈(ㄷ,ㅂ,ㅅ,르,우,러,여,오,ㅎ,으,ㄹ,거라,너라불규칙처리)
	void ExceptionModule_04(H_NODE* pNode);//(4모듈) : 복합어처리(복합어간) 모듈

	void ExceptionModule_05(H_NODE* pNode);//(5모듈) : 조사처리 모듈(선어말 외의 B에 해당하는 정보 찾는 모듈)
	
	
	//후처리로직
public:
	void AfterModule_01(H_NODE* pNode);//(5모듈)   : (A) 복합명사 후처리 - A의값 및 미분석음절 복합명사 리스트에 추가(우->좌분석)
	void AfterModule_02(H_NODE* pNode);//(6모듈)   : A,B 사의의 분석되지 않는 결과 음절 처리 : A+1~B 전체를 어미로 보고 처리
	void AfterModule_03(H_NODE* pNode);//(7모듈)   : (A) 숫자판단로직
	void AfterModule_04(H_NODE* pNode);//(8모듈)	   : (A) 알파벳판단로직
	void AfterModule_05(H_NODE* pNode);//(9모듈)	   : (A) 중국어(한자)변환로직


public:
	//Linked List에 SubTail 추가(복합어간 또는 복합어미)
	void InsertAddCompoundSubTail(H_NODE *pNode, char *pString, int nFlag, int nInsertTyep);
	void CompoundNounDecomposition(H_NODE *pNode, int nStartPosition, int nEndPosition, int nCompoundType);//복합명사분해
	void CompoundPostPositionDecomposition(H_NODE *pNode, int nStartPosition, int nEndPosition, int nCompoundType);//복합조사분해

	


public:
	//해당노드의 정보(복합어간 또는 복합어미)를 모두 초기화 한다	
	void AnalyzeNodeInitialization(H_NODE *pNode);
	void DeleteSubHTokenLinkedList(H_TOKEN *pDelHSubToken, char* strHFlag);
	void DeleteHTokenResult(H_NODE* pNode, int nDeleteKind);
	

	//디버그 관련 함수 실행 로직 저장
public:
	CString m_strLog;
	void SaveLogs(H_NODE *pNode, char* pCallFunc);




};


/*
테트스 URL : http://v.media.daum.net/v/20171205214533464
테트스 URL : http://v.media.daum.net/v/20171206142803881
*/