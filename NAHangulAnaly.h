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

//���� �� ���� ���� Ž�� 
public:
	bool IsExistStemDic(H_NODE* pNode, char *pStemWord);
	bool IsExistStemDic(H_SUBNODE* pNode, char *pStemWord);//���վ ���� �������� Ȯ��
	bool IsExistEndingDic(H_NODE* pNode, char *pEndingWord);
	bool IsExistEndingDic(H_SUBNODE* pNode, char *pEndingOneSyllable);//���վ���� ���� �������� Ȯ��
	bool IsExistEndingDic(char *pEndingOneSyllable);
	
	//���Ǽ����� ����ϴ� �Լ�(���̻� ��� ����.. ���Ǽ� Ŭ�������� �ش� �Լ��� ���� �κ� ã�� ��� �ּ�ó���Ѵ�. ���� �Լ��� ����� �� �ְ� �����Ѵ�)
	bool IsExistStemDic(char *IsExistStemDic, char *pSaveBuffer);
	bool IsExistStemDic(char *pStemSeveralSyllables);
	
	
	
//����� ���� �Լ� ���� ���� ����
public:
	CString m_strLog;
	void SaveLogs(H_NODE *pNode, char* pCallFunc);
	void WriteAnalysisError(H_NODE *pNode,char *pStemWord, char *pEndingWord);

//�ѱۺм� ����
public:
	void StartHangulAnaly();//�ѱۺм�
	bool CreateAmbiScript();//���Ǽ��м�
	

public:
	bool FindTheWholeWord(H_NODE* pNode);//��ü����
	void LongestMatchStemAndEndOfWordAnalysis(H_NODE* pNode);//������ġ�� �,�������

	bool ExistAllEtcWordEndOfWordAnalysis(H_NODE* pNode);//A~B������ ��� ���� ��̻��� ���� ���� Ȯ��


	void IsStemEndingExistSyllable(H_NODE* pNode);//���̸�� �����ϴ� ���
	void IsOnlyStemExistSyllable(H_NODE* pNode);//��� �����ϴ� ���
	void IsEndingExistSyllable(H_NODE* pNode);//��̸� �����ϴ� ���

	void DeleteSubTokenResult(H_NODE* pNode,int nResult);//��׾���� ��� ���� ����
	void DeleteSubTokenFeature(H_NODE* pNode, int nResult);//��׾���� ���� ���� ����
	
	

public:
	void InsertAddCompoundSubTail(H_NODE *pNode, char *pString, int strFlag); //Linked List�� SubTail �߰�(���վ �Ǵ� ���վ��)
	void CompoundNounDecomposition(H_NODE *pNode, int nEndPosition);//���ո�����
	void UnRegisteredCompoundNounWord(H_NODE *pNode);//N��°�� ������ ������  N+1��°�� ù������ ���� ��, N+1��° ù ���� ���� ó��

public:
	bool IrregularGrammarCheck(H_NODE *pNode,char* pGrammer);//�ұ�Ģó��(��,��,��,��,��,��,��,��,��)


public:
	void NAHangulUtilTesting(H_NODE* pNode);
};

