#pragma once

#include "ds_hanja.h"
#include "ds_list.h"
#include "ds_pretoken.h"
#include "ds_htoken.h"

#include "stddef.h"//NULL 정의 헤더 파일
#include "StemEndDic.h"
#include "NADocVector.h"
#include "NAHangulAnaly.h"
#include "MorphoLogicalAnalysis.h"
#include "NAPreLinkedList.h"

#include "time.h"//분석시간체크


class CPreProcessor
{
public:
	CPreProcessor();
	CPreProcessor(int readType, char* strExePath);
	virtual ~CPreProcessor();

public:
	CStemEndDic* m_pStemEndDic;//사전탐색
	CNADocVector* m_pDocVector;// Documnet Vector
	CNAHangulAnaly* m_wndNAHangulAnaly;
	CMorphoLogicalAnalysis* m_wndMorphoLogicalAnalysis;

public:
	char* m_pAllString;//전체문자열
	double m_nAnalysisResultTime;//형태소 분석 및 중의성 처리 시간

public:
	bool IsDot(char* pString, int nPos, int isAscii);
	bool IsInternetChar(char* pString, int nPos, int isAscii);
	bool IsCurrency(char* pString, int nPos, int isAscii);
	bool IsOperation(char* pString, int nPos, int isAscii);

	bool InitHangulAnalyClass();
	void InsertLinkedList(char *pstrInsert, int nOffset);
	void InsertLinkedList(H_NODE* pLink);
	int GetSylCode(char *pKindCode);
	int GetHighLow(int nKindCode, int *high, int *low);
	int IsKindCode(int high, int low);
	void RemoveDocVectorData(); // Documnet Vector Data Remove All

public:
	int GetCharCode(char *pKindCode);
	int GetHighLow(int nKindCode);
	void DivideDocSpace(char *pAllString);
	void PreLinkedNode(CNAPreLinkedList* pPreLinkedList);//문장 추가, Pre Linked List를 H_NODE로 변환
	int CharFindSymbol(PRE_NODE* m_pNode, int* nPos);	// 특수문자 찾기
	int CharFindSymbol(PRE_NODE* m_pNode, char* pSearch);
	int CharSymbolDelete(PRE_NODE* m_pNode); // 입력되는 특수 문자외 삭제 처리
	bool NextCharPos(char* pChar, int& nPos);		//다음 char의 Pos를 리턴 및 '\r', '\n' 있는지 판단
	void SentenceAddNode(PRE_NODE* pSentenceFirst, PRE_NODE* pSentenceLast); // Pre Node의 First와 Last를 입력받아 문장 추가

	bool NodeStrCmp(PRE_NODE* m_pNode, char* pCmp, int nCase = 0); //pCmp 문자 각각 m_pNode의 Str에 있는 검사
	void NodeOneCharRemove(PRE_NODE* m_pNode, char* pCmp); //pCmp에 비교된 문자는 제거
	void NodeTwoCharRemove(PRE_NODE* m_pNode, char* pCmp); //pCmp 문자를 제거

	int GetParentSymbol(char pChar, int nCase = 0); //괄호검사
	int ParentNodeTotal(CNAPreLinkedList* pPreLinkedList, PRE_NODE* m_pNode);
	int SetParentNode1(CNAPreLinkedList* pPreLinkedList, PRE_NODE* m_pNode, int nCase1, int nCase2); // '(', '{', '[', '<' 처리
	bool SetParentNode2(CNAPreLinkedList* pPreLinkedList, PRE_NODE* m_pNode); // '"' 처리
	int IsSlachDot(char *pString, int nPos, int isAscii); // '/', '·' 처리

	int Hanja2Hangul(PRE_NODE* m_pNode); // 특수기호 (한자->한글변환)

//Hanja Api
public:
	int hanja_bsearch(unsigned int item);
	unsigned int hanja_conv(char hbyte, char lbyte);

public:
	double GetAnalysisResultTime();
};
