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
	// 문서 정보
	DOC* m_pDOC;

	// 문서의 문장 Head, Tail
	SENTENCE* m_pHeadSentence;
	SENTENCE* m_pTailSentence;

	//현재 문장 위치
	SENTENCE* m_pCurrentSentence;

	//현재 문장의 Head, Tail, Count
	H_NODE* m_pHeadNode;
	H_NODE* m_pTailNode;
	int m_nTotalCount;

public:
	void SetTitle(char* pTitle);
	void SetFilename(char* pFilename);
	void SetDocID(char* pDocID);
	void SetAuthor(char* pAuthor);

	//초기화, 삭제
public:
	void InitDoc();//Doc, Meta, Sentence 정보 초기화
	void RemoveTailSentence();//m_pCurrentSentence(현재문장) 삭제
	void RemoveAllSentence();//문장들 초기화
	void RemoveAllNode(SENTENCE* pDeleteSen);//문장의 H_NODE		초기화
	void DeleteNode(H_NODE* pDeleteNode);
	void DeleteSubNode(H_SUBNODE* pDeleteNode);
	void DeleteSubToken(SUB_TOKEN* pSubToken);
	void DeleteSubCopyNode(H_NODE* pDelSubNode);
	void DeleteHToken(H_TOKEN* pDelHToken);
	//복합명사 또는 복합어미 삭제 처리 함수
	void DeleteSubHToken(H_TOKEN *pDelHSubToken,char* strHFlag);//strHFlag:STEM /ENDING
	

public:
	int AddSentenceHead(char* pSentence, int nOffset);//첫번째 문장 위치에 문장(SENTENCE)추가, 문장 위치 m_pCurrentSentence로 설정
	int AddSentenceTail(char* pSentence, int nOffset);// 마지막 문장 위치에 문장(SENTENCE)추가, 문장 위치 m_pCurrentSentence로 설정
	void ReplaceSentenceStr(char* pSentence);

	SENTENCE* GoSentenceFirst();//m_pCurrentSentence를 첫번째 문장 위치로 이동
	SENTENCE* GoSentenceLast();//m_pCurrentSentence를 마자믹 문장 위치로 이동
	SENTENCE* GoSentenceNext();//다음 문장 위치 이동
	SENTENCE* GoSentencePrev();//전 문장 위치 이동
	SENTENCE* GetSentence();//현재 문장 위치

	void SetSentenceNode(H_NODE* pFirst, H_NODE* pLast);//현재 문장(SENTENCE)에 첫번째 Node 연결

// Linked List API
public:
	void InitMorphList();
	int AddTail(char *pInputStr, int nOffset);
	int AddTail(char *pInputStr, char* pFeature, int nOffset);
	int AddHead(char *pInputStr, int nOffset);
	H_NODE* GetLastNode();
	int AddBeforeNode(char *pInputStr, H_NODE *pNode, int nOffset);
	H_NODE* CreateNode(char *pInputStr, int nOffset);//생성된 H_NODE를 Return만 수행
	int AddLinkTail(H_NODE* pLink); //H_NODE를 Tail에 Link
	
	//복합명사 또는 복합 어미를 관리하기 위해 	H_SUBNODE를 만든다.
	void CreateCompoundSubNode(H_NODE *pNode, char* strHFlag);//strHFlag:STEM /ENDING
	
};

