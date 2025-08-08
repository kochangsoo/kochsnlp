//warning C4996 : 'strcpy' : This function or variable may be unsafe.
#pragma warning ( disable : 4996 )

#pragma once

#include "ds_list.h"

class CNADocVector
{
public:
	CNADocVector();
	~CNADocVector();

public:
	// ���� ����
	DOC* m_pDOC;

	// ������ ���� Head, Tail
	SENTENCE* m_pHeadSentence;
	SENTENCE* m_pTailSentence;

	//���� ���� ��ġ
	SENTENCE* m_pCurrentSentence;

	//���� ������ Head, Tail, Count
	H_NODE* m_pHeadNode;
	H_NODE* m_pTailNode;
	int m_nTotalCount;

public:
	void SetTitle(char* pTitle);
	void SetFilename(char* pFilename);
	void SetDocID(char* pDocID);
	void SetAuthor(char* pAuthor);

	//�ʱ�ȭ, ����
public:
	void InitDoc();//Doc, Meta, Sentence ���� �ʱ�ȭ
	void RemoveTailSentence();//m_pCurrentSentence(���繮��) ����
	void RemoveAllSentence();//����� �ʱ�ȭ
	void RemoveAllNode(SENTENCE* pDeleteSen);//������ H_NODE		�ʱ�ȭ
	void DeleteNode(H_NODE* pDeleteNode);
	void DeleteSubNode(H_SUBNODE* pDeleteNode);
	void DeleteSubToken(SUB_TOKEN* pSubToken);
	void DeleteSubCopyNode(H_NODE* pDelSubNode);
	void DeleteHToken(H_TOKEN* pDelHToken);
	//���ո�� �Ǵ� ���վ�� ���� ó�� �Լ�
	void DeleteSubHToken(H_TOKEN *pDelHSubToken,char* strHFlag);//strHFlag:STEM /ENDING
	

public:
	int AddSentenceHead(char* pSentence, int nOffset);//ù��° ���� ��ġ�� ����(SENTENCE)�߰�, ���� ��ġ m_pCurrentSentence�� ����
	int AddSentenceTail(char* pSentence, int nOffset);// ������ ���� ��ġ�� ����(SENTENCE)�߰�, ���� ��ġ m_pCurrentSentence�� ����
	void ReplaceSentenceStr(char* pSentence);

	SENTENCE* GoSentenceFirst();//m_pCurrentSentence�� ù��° ���� ��ġ�� �̵�
	SENTENCE* GoSentenceLast();//m_pCurrentSentence�� ���ڹ� ���� ��ġ�� �̵�
	SENTENCE* GoSentenceNext();//���� ���� ��ġ �̵�
	SENTENCE* GoSentencePrev();//�� ���� ��ġ �̵�
	SENTENCE* GetSentence();//���� ���� ��ġ

	void SetSentenceNode(H_NODE* pFirst, H_NODE* pLast);//���� ����(SENTENCE)�� ù��° Node ����

// Linked List API
public:
	void InitMorphList();
	int AddTail(char *pInputStr, int nOffset);
	int AddTail(char *pInputStr, char* pFeature, int nOffset);
	int AddHead(char *pInputStr, int nOffset);
	H_NODE* GetLastNode();
	int AddBeforeNode(char *pInputStr, H_NODE *pNode, int nOffset);
	H_NODE* CreateNode(char *pInputStr, int nOffset);//������ H_NODE�� Return�� ����
	int AddLinkTail(H_NODE* pLink); //H_NODE�� Tail�� Link
	
	//���ո�� �Ǵ� ���� ��̸� �����ϱ� ���� 	H_SUBNODE�� �����.
	void CreateCompoundSubNode(H_NODE *pNode, char* strHFlag);//strHFlag:STEM /ENDING
	
};

