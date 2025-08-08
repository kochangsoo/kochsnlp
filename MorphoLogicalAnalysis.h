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

	//����
public:
	CStemEndDic* m_pStemEndDic;
	CConvert *m_pConvert;
	CNAAmbiguity* m_pAmbiguity;
	CNADocVector* m_LinkedList;
	CNAHangulUtil* m_pCNAHangulUtil;

	unsigned int m_nListPosition;
	unsigned int m_nListCount;


	//�Լ� ���� ���� ��� ���� ���� ����
public:
	char* m_AnalyzeWhatCallFunction;//4���� �Լ� �� � �Լ� ȣ�� ���� ����(������ġ�� �ٽ� ȣ�� �ÿ� ó���Լ��� �ٸ� �Լ��� ȣ���� ��� ó��)
	bool m_AnalyzeAgainFromTheBeginning;//������ġ�Լ� �� ȣ��
	bool m_ApplyIrregularAlgorithm;//�ұ�Ģ�˰��� ���� ����
	

	//���� �� ���� ���� Ž�� 
public:
	bool IsExistStemDic(H_NODE* pNode, char *pStemWord);
	//���Ǽ����� ����ϴ� �Լ�(���̻� ��� ����.. ���Ǽ� Ŭ�������� �ش� �Լ��� ���� �κ� ã�� ��� �ּ�ó���Ѵ�. ���� �Լ��� ����� �� �ְ� �����Ѵ�)
	bool IsExistStemDic(char *IsExistStemDic, char *pSaveBuffer);
	bool IsExistStemDic(char *pStemSeveralSyllables);
	bool IsExistEndingDic(H_NODE* pNode, char *pEndingWord);
	bool IsExistEndingDic(char *pEndingOneSyllable, int nReverse);//0:����[�ش� �ܾ REVERSE�ؾ� �Ѵ�] 1:�¿�
	bool IsExistStemDic(H_SUBNODE* pNode, char *pStemWord);//���վ ���� �������� Ȯ��
	bool IsExistEndingDic(H_SUBNODE* pNode, char *pEndingOneSyllable);//���վ���� ���� �������� Ȯ��


	//�ѱۺм� ����
public:
	void StartMorphoLogicalAnalysis();//�ѱۺм��� ����
	bool CreateAmbiScript();//���Ǽ��м�


	//�м���� A:� B:��� ,���ڿ� ��ġ�� 0���� ����
public:
	bool AnalysisModule_00(H_NODE* pNode);//��ü���� ����� Ž�� ���
	bool AnalysisModule_01(H_NODE* pNode);//������ġ(� �� ��� �м� �ĺ� ����) Ž�� ���


	//������ġ�� ����� ���� 4���� Ÿ�� �߻�
public:
	void OfTheFourLogic_FF(H_NODE* pNode); //A,B ��� �м� ����
	void OfTheFourLogic_AB(H_NODE* pNode);//A,B��� ����
	void OfTheFourLogic_XB(H_NODE* pNode);//B�� ����
	void OfTheFourLogic_AX(H_NODE* pNode);//A�� ����

	//��ó������
public:
	void BeforeModule_01(H_NODE* pNode);//(1���)   : B�� ���� �� B-1�� ���� ó��(��)ó��

	//����ó������
public:
	void ExceptionModule_00(H_NODE* pNode);//(0���) : ������ ������ ��������ó�����(��->�ϰ�)
	void ExceptionModule_01(H_NODE* pNode);//(1���) : ������ ������ ������ ����ó�����(��,��,��,��,��)
	void ExceptionModule_02(H_NODE* pNode);//(2���) : ��� ��� ó�� ���(B�� V,AMB+(V or N)�� �� ������ : '��','��','��','��'���)
	
	//�ұ�Ģó���� ��� �ݵ�� ��̷� �м��� ������ �־�� �Ѵ�.(V����Ȯ��)
	void ExceptionModule_03(H_NODE* pNode);//(3���) : �ұ�Ģó�� ���(��,��,��,��,��,��,��,��,��,��,��,�Ŷ�,�ʶ�ұ�Ģó��)
	void ExceptionModule_04(H_NODE* pNode);//(4���) : ���վ�ó��(���վ) ���

	void ExceptionModule_05(H_NODE* pNode);//(5���) : ����ó�� ���(��� ���� B�� �ش��ϴ� ���� ã�� ���)
	
	
	//��ó������
public:
	void AfterModule_01(H_NODE* pNode);//(5���)   : (A) ���ո�� ��ó�� - A�ǰ� �� �̺м����� ���ո�� ����Ʈ�� �߰�(��->�ºм�)
	void AfterModule_02(H_NODE* pNode);//(6���)   : A,B ������ �м����� �ʴ� ��� ���� ó�� : A+1~B ��ü�� ��̷� ���� ó��
	void AfterModule_03(H_NODE* pNode);//(7���)   : (A) �����Ǵܷ���
	void AfterModule_04(H_NODE* pNode);//(8���)	   : (A) ���ĺ��Ǵܷ���
	void AfterModule_05(H_NODE* pNode);//(9���)	   : (A) �߱���(����)��ȯ����


public:
	//Linked List�� SubTail �߰�(���վ �Ǵ� ���վ��)
	void InsertAddCompoundSubTail(H_NODE *pNode, char *pString, int nFlag, int nInsertTyep);
	void CompoundNounDecomposition(H_NODE *pNode, int nStartPosition, int nEndPosition, int nCompoundType);//���ո�����
	void CompoundPostPositionDecomposition(H_NODE *pNode, int nStartPosition, int nEndPosition, int nCompoundType);//�����������

	


public:
	//�ش����� ����(���վ �Ǵ� ���վ��)�� ��� �ʱ�ȭ �Ѵ�	
	void AnalyzeNodeInitialization(H_NODE *pNode);
	void DeleteSubHTokenLinkedList(H_TOKEN *pDelHSubToken, char* strHFlag);
	void DeleteHTokenResult(H_NODE* pNode, int nDeleteKind);
	

	//����� ���� �Լ� ���� ���� ����
public:
	CString m_strLog;
	void SaveLogs(H_NODE *pNode, char* pCallFunc);




};


/*
��Ʈ�� URL : http://v.media.daum.net/v/20171205214533464
��Ʈ�� URL : http://v.media.daum.net/v/20171206142803881
*/