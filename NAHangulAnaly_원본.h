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

	int m_nUWRFlag;		//��ó������ ���̴� ����( 0 : FALSE, 1 : TRUE, 2 : Default)

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

	
	
	//�ѱ� ���� ���� �Լ� 
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
	void MoveLeftWordCount(char *pString, int &nCount); //���� ���ڷ� �̵�
	void ReplaceHangulJA(char* pStr, int nWord);

	//���� �� ������ �ܾ��� ���� 
public:

	bool CalculateWordCount(char *pString, int nCount, int nMove, int nCase);
	bool CalculateOneWordCount(char *pString, int nPos);

	//��Ÿ �ѱۿ� ���� ó����(�ѱ� ���� �� ����) 
public:

	bool IsSameWordPos(H_NODE *pNode, int nSecondAlkong);
	void ChangePosAnyAlkong(H_NODE *pNode, int &nStart, int &nAlkong, int &nSecondAlkong, int nCase);
	void JasoRestoration(H_NODE *pNode, int nPos, int nCase, int nWord);
	bool DeleteHgngulTokenSource(H_NODE *pNode, int nPos, int nCase);
	bool ReplaceHgngulTokenSource(H_NODE *pNode, int nPos, int nCase, int nWord);
	bool IsExisteTableReplaceJaso(H_NODE *pNode, int nPos, int nCase, int nWord);
	bool IsExistStrReplaceJaso(H_NODE *pNode, int nPos, int nWord, char* pStr);
	void ReplaceHangulJongsungNull(H_NODE *pNode, int nPos);


	//���� �� ���� ���� �߰� (���۳����� �߰� ����)
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

	void InsertAddSubTail(H_NODE *pNode, char *pString); //Linked List�� SubTail �߰�
	void SubNodeUnity(H_NODE *pNode);		//���ո�� ��ġ��(Stem �̵�, ���� �ʱ�ȭ)
	H_SUBNODE* SubNodeSectionUnity(H_NODE *pNode);		//���ո�� �κ� ��ġ��("��", "��")

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

	//���� �� ���� ���� Ž�� 
public:

	bool IsExistNFeature(char *pString, int nStart, int nFeature);
	bool IsExistFeature(char *pString, int nKindDic, int nFeature);
	bool IsExistStemDic(char *pStem, char *pSaveBuffer);
	bool IsExistStemDic(char *pStem);
	bool IsExistEndDic(char *pEnd, char *pSaveBuffer);
	bool IsExistEndDic(char *pEnd);
	bool IsWildEndingDic(char *pEndingStr);
	bool IsExistWildEndingDic(char *pString);

	//���̺� ����(����� �� �ұ�Ģ ó��) 	
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

	//�ʱ�ȭ �Լ�
public:

	void InitOldNewBuffer();
	void AllInitMemberVariable();
	void InintMemberVariable(); // X

	void InitTableNumber();
	void InitBuffer();
	void InitOneWordMarsk();

	void AllAnalyString();


	

	


	//���¼� �м� �Լ� ��� 
	//��Ģó�� ���(MAR1, MAR2, MAR3) : �ѱ���� �������� � ����� �������� �̷���� ���� �м� ���
public:

	/////////////////////////////////////////////////////////
	//MAR1
	//������ ���ų� �����Ǿ� ���۸����� �̷���� ���� ó��
	//ü��� ����, ��Ģ���� ��� ó��(������Ż�� ����)
	//��) ����, �����, ����, �����
	/////////////////////////////////////////////////////////
	void GoMAR1(H_NODE *pNode);
	void GoMAR1_1(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong);
	void GoMAR1_2(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong);
	void GoMAR1_3(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong);
	void GoMAR1_4(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong);
	void GoMAR1_5(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong);

	//////////////////////////////////////////////////////////////////////////
	//MAR2
	//���յ� ������ ������ ���� ���
	//��) ���ڷμ���
	//////////////////////////////////////////////////////////////////////////
	void GoMAR2(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong);
	void GoMAR2_1(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong);
	void GoMAR2_2(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong);
	void GoMAR2_3(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong);
	void GoMAR2_4(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong);
	void GoMAR2_5(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong);


	/////////////////////////////////////////////////////////
	//MAR3
	//�󵵼� ���� ���� ó��(��, ��, ��, ��, ��, ��, ��, ��, ��, ��, ŭ), ��Ż�� ��� ó��
	//��) ���ٰ�, ����
	/////////////////////////////////////////////////////////
	void GoMAR3(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong);
	void GoMAR3_1(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong);
	void GoMAR3_2(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong);
	void GoMAR3_3(H_NODE *pNode, int &nStart, int &mAlkong, int &mSecondAlkong);

	//�ұ�Ģ ó�� ���(IRM1~IRM4) : �ѱ����� �ұ�Ģ ������ ó��
	/////////////////////////////////////////////////////////////////
	//IMR1
	//�ұ�Ģ ������ ó��(������� ó���� ���� �κ� - ��, ġ, ���, ��, Ű ���� ó��)
	//��) ������
	/////////////////////////////////////////////////////////////////
	void GoIMR1(H_NODE *pNode, int &nFirstOlive, int &mSecondOlive, int &mSecondAlkong);
	void GoIMR1_1(H_NODE *pNode, int &nFirstOlive, int &mSecondOlive, int &mSecondAlkong);
	void GoIMR1_2(H_NODE *pNode, int &nFirstOlive, int &mSecondOlive, int &mSecondAlkong);
	void GoIMR1_3(H_NODE *pNode, int &nFirstOlive, int &mSecondOlive, int &mSecondAlkong);


	/////////////////////////////////////////////////////////////////
	//IMR2
	//�� ó�� �˰���(��, ��, ��, ��, ��, �� ���� ó��)
	//��) ����ϼ̴�
	/////////////////////////////////////////////////////////////////
	void GoIMR2(H_NODE* pNode, int& nFirstOlive, int& mSecondOlive, int &mSecondAlkong);
	void GoIMR2_1(H_NODE* pNode, int& nFirstOlive, int& mSecondOlive, int &mSecondAlkong);

	/////////////////////////////////////////////////////////////////
	//IMR3
	//��, �� ó�� �˰���(��, ��, ��, ��, ��, ��, �� ���� ó��)
	//��) �¾��
	/////////////////////////////////////////////////////////////////
	void GoIMR3(H_NODE* pNode, int& nFirstOlive, int& mSecondOlive, int &mSecondAlkong);
	void GoIMR3_1(H_NODE* pNode, int& nFirstOlive, int& mSecondOlive, int &mSecondAlkong);

	/////////////////////////////////////////////////////////////////
	//IMR4
	//����� �ұ�Ģ ó��(������, ������, ���졯, ������, ������, ������, �������ұ�Ģ)
	//���ϴ�, �Ǵ١� ó��
	//��) �����������
	/////////////////////////////////////////////////////////////////
	void GoIMR4(H_NODE* pNode, int& nFirstOlive, int& mSecondOlive, int &mSecondAlkong);
	void GoIMR4_1(H_NODE* pNode, int& nFirstOlive, int& mSecondOlive, int &mSecondAlkong);
	void GoIMR4_2(H_NODE* pNode, int& nFirstOlive, int& mSecondOlive, int &mSecondAlkong);

	/////////////////////////////////////////////////////////////////
	//NER (��ü�� �ν�)
	/////////////////////////////////////////////////////////////////
	bool GoNER01(H_NODE* pNode);
	bool GoNER02(H_NODE* pNode);
	void GoNER03(H_NODE* pNode);

	// ��ó�� ��籸 ��ġ��
	void GoUnitNode(H_NODE* pNode);

	bool IsExistNameTabe(char *pString);

public://�̵�Ͼ� �� ���� ó�� ���
	char* GetUnRegisterMiddleCountChar(char *pString, int nStartPos, int nEndPos, int nStartCase, int nEndCase);
	int GetUnRegisterMoveRightCount(char* pString, int nCount);
	void UnRegisteMoveRight(char *pString, int &nCount);
	char* GetUnRegisterOneChar(char *pString, int nCount);
	void UnRegisterWord(H_NODE *pNode);

	//���� ó�� ���
	void GoEND(H_NODE *pNode);
	void GoEND1_1(H_NODE *pNode, int& nStart, int& mAlkong);

	//���� ���� ����
	void GoSVA(H_NODE* pNode);
	void GoSVA1_1(H_NODE* pNode);
	void GoSVA1_2(H_NODE *pNode);
	void GoSVA1_3(H_NODE *pNode);

	//�̵�Ͼ� �� ���� 
	void GoUWR(H_NODE *pNode);
	void GoUWR1_1(H_NODE* pNode, int &nStart, int &mAlkong);
	void GoUWR1_2(H_NODE* pNode, int &nStart, int &mAlkong);
	void GoUWR1_3(H_NODE* pNode, int &nStart, int &mAlkong);
	void GoUWR1_4(H_NODE *pNode, int &nStart, int &mAlkong);

	//����ó�� ��� 
	void GoFNM1(H_NODE *pNode);
	void GoFNM1_1(H_NODE *pNode, int &nStart);
	void GoFNM1_2(H_NODE *pNode, int &nStart);
	void GoFNM1_3(H_NODE *pNode, int &nStart);
	void GoFNM1_4(H_NODE *pNode, int &nStart);

	void UniMakeNewEndingFeature(H_NODE *pEndingNode);
	void UniAddEndingFeature(H_NODE *pEndingNode, char *pString);
	void SaveAmbstrEnding(H_NODE *pNode, char *pEndStr);

public://���Ǽ� ó��
	bool CreateAmbiScript();

public://FNM1_5���� Ư������ ó���� ���� �Լ�
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

