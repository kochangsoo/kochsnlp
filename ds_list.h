#pragma once
//
//ds_list.h
//

#ifndef __DS_LIST_H__
#define __DS_LIST_H__


////////////////////////////////
//구문분석 
////////////////////////////////

//Blocking of Information
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>


#define S_E_V 0 //Setenece Start - End Flag Value
#define S_P_B 1 //Start_P_Bloking
#define E_P_B 2 //End_P_Blocking
#define S_N_B 3
#define E_N_B 4
#define S_G_B 5
#define E_G_B 6
#define S_O_B 7
#define E_O_B 8
#define S_C_B 9
#define E_C_B 10
#define S_S_B 11
#define E_S_B 12

//명제 테이블
#define SUBJ_VOC 0 //호격
#define SUBJ_CONJ 1 //절
#define SUBJ_PA 2 //부사
#define SUBJ_S 3 //주어
#define SUBJ_P 4 //부속
#define SUBJ_SD 5 //부속
#define SUBJ_IO 6 //여격
#define SUBJ_DO 7 //묵적어
#define SUBJ_P1 8 //부속
#define SUBJ_C 9 //보어
#define SUBJ_V 10 //동사 
#define SUBJ_AUX 11 //조동사 

//양태 테이블 
#define MODE_HON 0 //존칭 
#define MODE_PRS 1 //시제(이하) 
#define MODE_PST 2
#define MODE_FUT 3
#define MODE_RE 4 //상(이하)
#define MODE_IG 5
#define MODE_PP 6
#define MODE_NEG 7 //부정
#define MODE_DCL 8 //문장유형(이하) 
#define MODE_WHQ 9
#define MODE_YQ 10
#define MODE_IMPR 11
#define MODE_PPO 12
#define MODE_EXCL 13


//Sentence of Information 
#define F_N 0 //Flag_Start
#define F_S 1 //Flag_Start
#define F_E 2 //Flag_End

#define B_NN 0 //is Not Block
#define B_NB 1 //N-Block
#define B_OB 2 //O-Block
#define B_GB 3 //G-Block

#define TABLE_NOTCHECK 0 //Table Not Check
#define TABLE_CHECK    1    //Table Check

#define MOVE_L 0 //Left Move
#define MOVE_R 1 //Right Move

#define DISTICT_ONE 0 //One(N) Only Block Info Search
#define DISTICT_ALL 1 //All Block Info Search

////////////////////////////////
//형택소 분석 
////////////////////////////////

//define Token Word Divide 
#define WORD_SPACE ' '
#define WORD_CARRAGERETURN_1 '\r'
#define WORD_CARRAGERETURN_2 '\n'

#define CHAR_CHOJUNGSUNG 1//초중성 코드
#define CHAR_JONGSUNG    2//종성 코드

#define CHANGE_CHAR_SYLLABLE_FIRST 1//처음 음절 변경
#define CHANGE_CHAR_SYLLABLE_LAST    2//마지막 음절 변경

#define HANGUL_CHAR_WORD   1// 한글
#define CHINESE_CHAR_WORD  2// 한자  
#define KATAGANA_CHAR_WORD 3// 가카가나 
#define HIRAGANA_CHAR_WORD 4// 히라가나
#define ENGLISH_CHAR_WORD  5// 영어 
#define NUMBER_CHAR_WORD   6// 숫자 
#define SINGLE_CHAR_WORD   7// 단일문장부호 
#define TWIN_CHAR_WORD     8// 결합쌍문장부호 
#define ASCII_CHAR_WORD    9// ASCII 코드 
#define SPECIAL_CHAR_WORD  10// 특수문자

#define SYMBOL_CHAR_1  1 //'<' 
#define SYMBOL_CHAR_2  2 //'('
#define SYMBOL_CHAR_3  3 //'['
#define SYMBOL_CHAR_4  4 //'>'
#define SYMBOL_CHAR_5  5 //')'
#define SYMBOL_CHAR_6  6 //']'
#define SYMBOL_CHAR_7  7 //'\''
#define SYMBOL_CHAR_8  8 //'"'

//축어 명령어
#define CLASS_DELETE(X)	if(X != NULL) { delete X; X = NULL; }
#define CHAR_DELETE(X)	if(X != NULL) { delete[] X; X = NULL; }

//자료구조
typedef struct _SUB_TOKEN //형태소 분석 후의 어간/어미 정보 및 자질 정보 
{
	//pSend(0) PBlock(1-2) NBlock(3-4) GBlock(5-6) OBlock(7-8) CBlock(9-10) SBlock(11-12)
	//unsigned short BlockInfo[13];//블럭킹 정보
	int nKindChar;//한글/한자/일어, 숫자/영문 구별 기호
	char* strResult;//문자열 값 
	char* strFeature;//자질값 
	//0을 포함한 위치값으로 보게 설계함
	int	nFindPos;//전체 어절에서 최장일치 어간 또는 어미의 발견위치(어간은 찾은 문자열의 끝위치, 어미는 찾은 문자열의 시작위치)
}SUB_TOKEN;

typedef struct _COMP_SUB_TOKEN
{
	int nKindChar;//한글/한자/일어, 숫자/영문 구별 기호
	char* strResult;//문자열 
	char* strFeature;//자질값 
}COMP_SUB_TOKEN;

//복합 명사 표현 구조체
typedef struct CSubNode
{
	COMP_SUB_TOKEN* pCSubToken;
	struct CSubNode* pNext;
	struct CSubNode* pPrev;
}H_SUBNODE;

typedef struct _HANGUL_TOKEN //전체 관리 
{
	char* strOriginalSource;
	char* strSource;//원문자열 정보 보관
	char* strAmbEndingSource;//중의성이 발생한 엔딩 문자열 보관
	int nOffset;

	SUB_TOKEN* subStem;//스템 값
	SUB_TOKEN* subEnding;//엔딩 값 

	bool bSubCompCreate;//복합 명사 생성 플래그
	H_SUBNODE* pSubHeadNode;
	H_SUBNODE* pSubTailNode;

	//2017-09-28 신규 추가
	bool bSubEndingCompCreate;//복합 어미 생성 플래그
	H_SUBNODE* pSubEndingHeadNode;
	H_SUBNODE* pSubEndingTailNode;
	
	//2017-08-31
	#ifdef _DEBUG
	char strFlowChartDebug[1024];//형태소 분석 알고리즘 분석 흐름도
	#endif

	//2017-10-01
	#ifdef _DEBUG
	char strAnalysisError[1024];//분석오류
	#endif

	//2017-09-28 신규 추가
	int strSourceLen;//원문자열의 길이
	int nLastFindEndingSyllable;//A~B사이의 사전에 존재하는 어미의 가장 마지막 위치
	int nLastFindCompoundNoun;//복합명사로 분석된 어절의 마지막 위치(링크드리스트의 제일 첫 위치)

	int nKindError;//0     : 에러 없음(분석성공) -999 : 분석실패
	
}H_TOKEN;

typedef struct CNode
{
	//현재 문장이 한번 분석 되었음을 의미함!!
	bool bMarsk;
	int nSkip;
	H_TOKEN* pHToken;
	struct CNode* pNext;
	struct CNode* pPrev;
}H_NODE;


typedef struct _SENTENCE
{
	struct CNode* pHead;	//문장 시작 위치
	struct CNode* pTail;	//문장 끝 위치
	char szSentence[8192 * 5];
	int nOffset;

	struct _SENTENCE* pNext;
	struct _SENTENCE* pPrev;
}SENTENCE;

typedef struct _DOC
{
	struct _SENTENCE* sentence;	//문장 첫번째 위치

	char szFilename[256];
	char szDocID[256];
	char szTitle[4096];
	char szAuthor[256];
	char szDate[64];

	time_t nDate;
	int nCount;	// 문장의 전체 수
}DOC;



typedef struct _HANGUL_ANALY_RESULT //전체 관리(형태소분석결과 임시사용) 
{
	char strSource[50];//원문자열 정보 보관
	int nRefer;//언급
	int bVerb;//동사여부

}H_ANALY_RESULT;

#endif
