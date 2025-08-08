#pragma once

#include "ds_hanja.h"
#include "ds_list.h"
#include "ds_pretoken.h"
#include "ds_htoken.h"

#include "stddef.h"//NULL ���� ��� ����
#include "StemEndDic.h"
#include "NADocVector.h"
#include "NAHangulAnaly.h"
#include "MorphoLogicalAnalysis.h"
#include "NAPreLinkedList.h"

#include "time.h"//�м��ð�üũ


class CPreProcessor
{
public:
	CPreProcessor();
	CPreProcessor(int readType, char* strExePath);
	virtual ~CPreProcessor();

public:
	CStemEndDic* m_pStemEndDic;//����Ž��
	CNADocVector* m_pDocVector;// Documnet Vector
	CNAHangulAnaly* m_wndNAHangulAnaly;
	CMorphoLogicalAnalysis* m_wndMorphoLogicalAnalysis;

public:
	char* m_pAllString;//��ü���ڿ�
	double m_nAnalysisResultTime;//���¼� �м� �� ���Ǽ� ó�� �ð�

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
	void PreLinkedNode(CNAPreLinkedList* pPreLinkedList);//���� �߰�, Pre Linked List�� H_NODE�� ��ȯ
	int CharFindSymbol(PRE_NODE* m_pNode, int* nPos);	// Ư������ ã��
	int CharFindSymbol(PRE_NODE* m_pNode, char* pSearch);
	int CharSymbolDelete(PRE_NODE* m_pNode); // �ԷµǴ� Ư�� ���ڿ� ���� ó��
	bool NextCharPos(char* pChar, int& nPos);		//���� char�� Pos�� ���� �� '\r', '\n' �ִ��� �Ǵ�
	void SentenceAddNode(PRE_NODE* pSentenceFirst, PRE_NODE* pSentenceLast); // Pre Node�� First�� Last�� �Է¹޾� ���� �߰�

	bool NodeStrCmp(PRE_NODE* m_pNode, char* pCmp, int nCase = 0); //pCmp ���� ���� m_pNode�� Str�� �ִ� �˻�
	void NodeOneCharRemove(PRE_NODE* m_pNode, char* pCmp); //pCmp�� �񱳵� ���ڴ� ����
	void NodeTwoCharRemove(PRE_NODE* m_pNode, char* pCmp); //pCmp ���ڸ� ����

	int GetParentSymbol(char pChar, int nCase = 0); //��ȣ�˻�
	int ParentNodeTotal(CNAPreLinkedList* pPreLinkedList, PRE_NODE* m_pNode);
	int SetParentNode1(CNAPreLinkedList* pPreLinkedList, PRE_NODE* m_pNode, int nCase1, int nCase2); // '(', '{', '[', '<' ó��
	bool SetParentNode2(CNAPreLinkedList* pPreLinkedList, PRE_NODE* m_pNode); // '"' ó��
	int IsSlachDot(char *pString, int nPos, int isAscii); // '/', '��' ó��

	int Hanja2Hangul(PRE_NODE* m_pNode); // Ư����ȣ (����->�ѱۺ�ȯ)

//Hanja Api
public:
	int hanja_bsearch(unsigned int item);
	unsigned int hanja_conv(char hbyte, char lbyte);

public:
	double GetAnalysisResultTime();
};
