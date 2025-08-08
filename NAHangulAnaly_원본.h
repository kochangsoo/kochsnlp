#pragma once




#include "StemEndDic.h"
#include "ds_list.h"
#include "ds_htoken.h"
#include "na_convert.h"
#include "NAAmbiguity.h"
#include "NADocvector.h"

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

	//Attribute
public:
	unsigned int m_nListPosition;
	unsigned int m_nListCount;

	int m_nUWRFlag;		//전처리에서 쓰이는 변수( 0 : FALSE, 1 : TRUE, 2 : Default)

	char* m_pPrevOldBuffer;
	char *m_pOldBuffer;
	char *m_pNewBuffer;
	char *m_pRealOriginalString;

	char m_ArrayOldBuffer[48];
	char m_ArrayNewBuffer[48];
	char m_ArrayStem[235]; // X

	bool m_bIsFirstWord;
	bool m_bOneCharMask;

	int m_nTableNumber;
	int m_nTableIndex;
	int m_nTableString;
	int m_nInsertNodeCount;

	
	
	//한글 관련 연산 함수 
public:

	char* ReplaceString(char *pOneString, int nPos, int nCase);
	char* AddTwoStr(char *pOneString, char *pTwoString);
	char* MinusTwoStr(char *pOneString, char *pTwoString, int nPos);

	char* GetChojungJongsungChar(char *pOneChar, int nKind);
	char* GetMiddleCountChar(char *pString, int nStartPos, int nEndPos, int nStartCase, int nEndCase);
	char* GetEndCountChar(char *pString, int nPos);
	char* GetOneChar(char *pStr, int ns_Pos, int ne_Pos);
	char* GetStartCountChar(char *pStr, int nMoveCount);
	int ReturnNextPrevCount(char *pOriginalStr, int nStartPos, int nCount, int nCase);
	int ReturnNumberMoveLeftWordCount(char *pString, int &nCount);
	int Restoration(char *pJongChar);
	bool IsThisWordHangulAscii(char *pThisChar, int nPos);
	void MoveLeftWordCount(char *pString, int &nCount); //다음 문자로 이동
	void ReplaceHangulJA(char* pStr, int nWord);

	//이전 및 이후의 단어의 상태 
public:

	bool CalculateWordCount(char *pString, int nCount, int nMove, int nCase);
	bool CalculateOneWordCount(char *pString, int nPos);

	//기타 한글에 대한 처리들(한글 변경 및 삭제) 
public:

	bool IsSameWordPos(H_NODE *pNode, int nSecondAlkong);
	void ChangePosAnyAlkong(H_NODE *pNode, int &nStart, int &nAlkong, int &nSecondAlkong, int nCase);
	void JasoRestoration(H_NODE *pNode, int nPos, int nCase, int nWord);
	bool DeleteHgngulTokenSource(H_NODE *pNode, int nPos, int nCase);
	bool ReplaceHgngulTokenSource(H_NODE *pNode, int nPos, int nCase, int nWord);
	bool IsExisteTableReplaceJaso(H_NODE *pNode, int nPos, int nCase, int nWord);
	bool IsExistStrReplaceJaso(H_NODE *pNode, int nPos, int nWord, char* pStr);
	void ReplaceHangulJongsungNull(H_NODE *pNode, int nPos);


	//스템 및 엔딩 자질 추가 (버퍼내용의 추가 포함)
public:

	void AddHTokenEnding(H_NODE *pEndingNode, int nCase);
	void DeleteHTokenEnding(H_NODE *pEndingNode, int nCase);
	void SaveHTokenEnding(H_NODE *pEndingNode);

	void AddHTokenStem(H_NODE *pStemNode, int nCase);
	void DeleteHTokenStem(H_NODE *pStemNode, int nCase);
	void SaveHTokenStem(H_NODE *pStemNode);
	void SaveHTokenStemFeature(H_NODE *pStemNode, char *pStem);

	void InsertStemString(H_NODE *pNode, char *pStem);
	void InsertEndingString(H_NODE *pNode, char *pEnding);

	void InsertAddSubTail(H_NODE *pNode, char *pString); //Linked List에 SubTail 추가
	void SubNodeUnity(H_NODE *pNode);		//복합명사 합치기(Stem 이동, 자질 초기화)
	H_SUBNODE* SubNodeSectionUnity(H_NODE *pNode);		//복합명사 부분 합치기("씨", "님")

	void SaveHTokenEndingOldFeature(H_NODE *pNode);
	void SaveHTokenEndingNewFeature(H_NODE *pNode);
	void AddEndingFeature(H_NODE *pEndingNode, char *pString);
	void MakeNewEndingFeature(H_NODE *pEndingNode);
	void InsertArrayFeatureNew(char *pString);
	void InsertArrayFeatureOld(char *pString);
	void AddOldNewBuffer(H_NODE *pNode);

	void SaveRealOriginalStr(char* pSaveStr);

	//New API
	void ReplaceSubNodeTailString(H_NODE *pNode, char* pStem);
	void AddSubNodeTailFeature(H_NODE *pNode, char* pStem);
	void AddSubNodeTailFeature(H_NODE *pNode, int nCase);
	void DeleteSubNodeTailFeature(H_NODE *pNode, int nCase);
	void DeleteAllSubNode(H_NODE *pNode);
	void DeleteSubNodeTail(H_NODE *pNode);
	int SubNodeCount(H_NODE *pNode);

	//스템 및 엔딩 사전 탐색 
public:

	bool IsExistNFeature(char *pString, int nStart, int nFeature);
	bool IsExistFeature(char *pString, int nKindDic, int nFeature);
	bool IsExistStemDic(char *pStem, char *pSaveBuffer);
	bool IsExistStemDic(char *pStem);
	bool IsExistEndDic(char *pEnd, char *pSaveBuffer);
	bool IsExistEndDic(char *pEnd);
	bool IsWildEndingDic(char *pEndingStr);
	bool IsExistWildEndingDic(char *pString);

	//테이블 관련(축약형 및 불규칙 처리) 	
public:

	bool CompareVariousTable01(char *pOneChar, int nIndex, bool isAll);
	bool CompareVariousTable02(char *pOneChar, int nIndex, bool isAll);
	bool CompareVariousTable03(char *pOneChar, int nIndex, bool isAll);
	bool CompareVariousTable04(char *pOneChar, int nIndex, bool isAll);
	bool CompareVariousTable05(char *pOneChar, int nIndex, bool isAll);
	bool CompareVariousTable06(char *pOneChar, int nIndex, bool isAll);
	bool CompareVariousTable07(char *pOneChar, int nIndex, bool isAll);
	bool CompareVariousTable08(char *pOneChar, int nIndex, bool isAll);
	bool CompareVariousTable09(char *pOneChar, int nIndex, bool isAll);
	bool CompareVariousTable10(char *pOneChar, int nIndex, bool isAll);

	char* GetAnyTableValue(); // X

	void CheckFirstWord();
	void SetOneWordMarsk();

	void GoErrorFunction(H_NODE *pNode);

	//초기화 함수
public:

	void InitOldNewBuffer();
	void AllInitMemberVariable();
	void InintMemberVariable(); // X

	void InitTableNumber();
	void InitBuffer();
	void InitOneWordMarsk();

	void AllAnalyString();


	

	


	//형태소 분석 함수 목록 
	//규칙처리 모듈(MAR1, MAR2, MAR3) : 한국어에서 정상적인 어간 어미의 결합으로 이루어진 어절 분석 모듈
public:

	/////////////////////////////////////////////////////////
	//MAR1
	//엔딩이 없거나 생략되어 스템만으로 이루어진 어휘 처리
	//체언과 조사, 규칙용언과 어미 처리(‘ㄹ’탈락 포함)
	//예) 저물, 사람이, 정들어서, 만드는
	/////////////////////////////////////////////////////////
	void GoMAR1(H_NODE *pNode);
	void GoMAR1_1(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong);
	void GoMAR1_2(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong);
	void GoMAR1_3(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong);
	void GoMAR1_4(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong);
	void GoMAR1_5(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong);

	//////////////////////////////////////////////////////////////////////////
	//MAR2
	//결합된 엔딩이 사전에 없는 경우
	//예) 학자로서의
	//////////////////////////////////////////////////////////////////////////
	void GoMAR2(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong);
	void GoMAR2_1(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong);
	void GoMAR2_2(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong);
	void GoMAR2_3(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong);
	void GoMAR2_4(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong);
	void GoMAR2_5(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong);


	/////////////////////////////////////////////////////////
	//MAR3
	//빈도수 높은 음절 처리(여, 다, 야, 여, 이, 가, 터, ㄴ, ㅂ, 려, 큼), ㄹ탈락 용언 처리
	//예) 가다가, 빌려
	/////////////////////////////////////////////////////////
	void GoMAR3(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong);
	void GoMAR3_1(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong);
	void GoMAR3_2(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong);
	void GoMAR3_3(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong);

	//불규칙 처리 모듈(IRM1~IRM4) : 한국어의 불규칙 현상을 처리
	/////////////////////////////////////////////////////////////////
	//IMR1
	//불규칙 개별항 처리(축약형의 처리를 위한 부분 - 케, 치, 토록, 셔, 키 등의 처리)
	//예) 적응케
	/////////////////////////////////////////////////////////////////
	void GoIMR1(H_NODE *pNode, int &nFirstOlive, int &mSecondOlive, int &mSecondAlkong);
	void GoIMR1_1(H_NODE *pNode, int &nFirstOlive, int &mSecondOlive, int &mSecondAlkong);
	void GoIMR1_2(H_NODE *pNode, int &nFirstOlive, int &mSecondOlive, int &mSecondAlkong);
	void GoIMR1_3(H_NODE *pNode, int &nFirstOlive, int &mSecondOlive, int &mSecondAlkong);


	/////////////////////////////////////////////////////////////////
	//IMR2
	//ㅆ 처리 알고리듬(었, 았, 겠, 였, 셨, 갔 등의 처리)
	//예) 편애하셨던
	/////////////////////////////////////////////////////////////////
	void GoIMR2(H_NODE* pNode, int& nFirstOlive, int& mSecondOlive, int &mSecondAlkong);
	void GoIMR2_1(H_NODE* pNode, int& nFirstOlive, int& mSecondOlive, int &mSecondAlkong);

	/////////////////////////////////////////////////////////////////
	//IMR3
	//ㅁ, ㄻ 처리 알고리듬(ㄻ, 임, 음, 기, 함, 됨, 심 등의 처리)
	//예) 태어나심
	/////////////////////////////////////////////////////////////////
	void GoIMR3(H_NODE* pNode, int& nFirstOlive, int& mSecondOlive, int &mSecondAlkong);
	void GoIMR3_1(H_NODE* pNode, int& nFirstOlive, int& mSecondOlive, int &mSecondAlkong);

	/////////////////////////////////////////////////////////////////
	//IMR4
	//용언의 불규칙 처리(‘ㄷ’, ‘ㅅ’, ‘우’, ‘으’, ‘르’, ‘러’, ‘ㅎ’불규칙)
	//‘하다, 되다’ 처리
	//예) 마무리지어야
	/////////////////////////////////////////////////////////////////
	void GoIMR4(H_NODE* pNode, int& nFirstOlive, int& mSecondOlive, int &mSecondAlkong);
	void GoIMR4_1(H_NODE* pNode, int& nFirstOlive, int& mSecondOlive, int &mSecondAlkong);
	void GoIMR4_2(H_NODE* pNode, int& nFirstOlive, int& mSecondOlive, int &mSecondAlkong);

	/////////////////////////////////////////////////////////////////
	//NER (개체명 인식)
	/////////////////////////////////////////////////////////////////
	bool GoNER01(H_NODE* pNode);
	bool GoNER02(H_NODE* pNode);
	void GoNER03(H_NODE* pNode);

	// 미처리 명사구 합치기
	void GoUnitNode(H_NODE* pNode);

	bool IsExistNameTabe(char *pString);

public://미등록어 및 숫자 처리 모듈
	char* GetUnRegisterMiddleCountChar(char *pString, int nStartPos, int nEndPos, int nStartCase, int nEndCase);
	int GetUnRegisterMoveRightCount(char* pString, int nCount);
	void UnRegisteMoveRight(char *pString, int &nCount);
	char* GetUnRegisterOneChar(char *pString, int nCount);
	void UnRegisterWord(H_NODE *pNode);

	//에러 처리 모듈
	void GoEND(H_NODE *pNode);
	void GoEND1_1(H_NODE *pNode, int& nStart, int& mAlkong);

	//동사 연결 구성
	void GoSVA(H_NODE* pNode);
	void GoSVA1_1(H_NODE* pNode);
	void GoSVA1_2(H_NODE *pNode);
	void GoSVA1_3(H_NODE *pNode);

	//미등록어 및 띄어쓰기 
	void GoUWR(H_NODE *pNode);
	void GoUWR1_1(H_NODE* pNode, int &nStart, int &mAlkong);
	void GoUWR1_2(H_NODE* pNode, int &nStart, int &mAlkong);
	void GoUWR1_3(H_NODE* pNode, int &nStart, int &mAlkong);
	void GoUWR1_4(H_NODE *pNode, int &nStart, int &mAlkong);

	//숫자처리 모듈 
	void GoFNM1(H_NODE *pNode);
	void GoFNM1_1(H_NODE *pNode, int &nStart);
	void GoFNM1_2(H_NODE *pNode, int &nStart);
	void GoFNM1_3(H_NODE *pNode, int &nStart);
	void GoFNM1_4(H_NODE *pNode, int &nStart);

	void UniMakeNewEndingFeature(H_NODE *pEndingNode);
	void UniAddEndingFeature(H_NODE *pEndingNode, char *pString);
	void SaveAmbstrEnding(H_NODE *pNode, char *pEndStr);

public://중의성 처리
	bool CreateAmbiScript();

public://FNM1_5에서 특수문자 처리를 위한 함수
	bool IsNextOneCharSpecial(char *pOneChar);

public:
	void GoPFM(H_NODE* pNode, int &nStart, int &mAlkong);
	bool IsAalphabet(char* pOneString);
	void GoFNM1_5(H_NODE *pNode, int &nStart);
	bool IsExistTabel009(char* pString);
	bool IsExistTabel008(char* pString);
	bool IsExistTabel007(char* pString);
	bool IsExistTabel006(char* pString);
	bool IsExistTabel005(char* pString);

	void RepeatCreate();
	void RestorIrrigual(H_NODE* pNode);
	void ClassfyKindChar(H_NODE* pNode, char* pString);
	void ClassfyKindChar(H_SUBNODE* pNode, char *pString);
	int GetHighLow(int nKindCode, int *high, int *low);
	int GetSylCode(char *pKindCode);
	int IsKindCode(int high, int low);
	void SaveCompEndDic(H_NODE* pNode, char *pString);
	void InitRealOriginalString();
	void Insert_Str_LinkedList(char *pStr, int nOffset);
	void Insert_Str_LinkedList(char *pStr, H_NODE *pNode, int nOffset);
	void Insert_Str_LinkedList(H_NODE *pNode);
	void StartHangulAnaly();
	void DeleteLinkedList();
};

