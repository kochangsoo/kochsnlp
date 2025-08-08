#pragma once
//warning C4996 : 'strcpy' : This function or variable may be unsafe.
#pragma warning ( disable : 4996 )

#include "ds_pretoken.h"

class CNAPreLinkedList
{
public:
	CNAPreLinkedList();
	virtual ~CNAPreLinkedList();

public:
	PRE_NODE* m_pHeadNode;
	PRE_NODE* m_pTailNode;

	PRE_NODE* m_pCurrentPreNode;

public:
	void InitHeadTailNode();
	void AddPreTail(char* pInputStr, int nOffset, int nEndCheck = 0, int nFirstParent = 0, int nLastParent = 0);
	int AddPreNode(PRE_NODE* pPreNode, char* pInputStr, int nOffset);
	void RemovePreAll();
	void RemovePreNode(PRE_NODE* pPreNode);
	PRE_NODE* PreNodeUnite(PRE_NODE* pPreNode1, PRE_NODE* pPreNode2);	// 두노드를 합치기
	char* PreNodeUniteStr(PRE_NODE* pPreNode1, PRE_NODE* pPreNode2);
	PRE_NODE* PreNodeDivide(PRE_NODE* pPreNode, int nPos, int nCase);

	PRE_NODE* GoPreNodeFirst();	// m_pCurrentPreNode를 첫번째 Node 위치로 이동
	PRE_NODE* GoPreNodeLast();  // m_pCurrentPreNode를 마자믹 Node 위치로 이동
	PRE_NODE* GoPreNodeNext();  // 다음 Node 위치 이동
	PRE_NODE* GoPreNodePrev();  // 전 Node 위치 이동
	PRE_NODE* GetPreNode();  // 현재 Node 위치
};

