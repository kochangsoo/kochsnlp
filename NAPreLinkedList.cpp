#include "NAPreLinkedList.h"


CNAPreLinkedList::CNAPreLinkedList()
{
}


CNAPreLinkedList::~CNAPreLinkedList()
{
	delete m_pHeadNode;
	m_pHeadNode = NULL;

	delete m_pTailNode;
	m_pTailNode = NULL;
}

void CNAPreLinkedList::AddPreTail(char *pInputStr, int nOffset, int nEndCheck, int nFirstParent, int nLastParent)
{
	if (strlen(pInputStr) > 0)
	{
		PRE_NODE* pInsertNode;
		pInsertNode = new PRE_NODE;
		int nLen = 0;
		int i;

		if (pInsertNode != NULL)
		{
			pInsertNode->pPreToken = new PRE_TOKEN;
			if (pInsertNode->pPreToken != NULL)
			{
				nLen = strlen(pInputStr);
				pInsertNode->pPreToken->strOriginalSource = new char[nLen + 1];
				memset(pInsertNode->pPreToken->strOriginalSource, 0x00, nLen + 1);
				strcpy(pInsertNode->pPreToken->strOriginalSource, pInputStr);
				pInsertNode->pPreToken->nOffset = nOffset;
				pInsertNode->pPreToken->nEndCheck = nEndCheck;
				pInsertNode->pPreToken->nFirstParent = nFirstParent;
				pInsertNode->pPreToken->nLastParent = nLastParent;

				memset(pInsertNode->pPreToken->pFeature, 0x00, PRE_FEATURE + 1);
				for (i = 0; i<PRE_FEATURE; i++)
					pInsertNode->pPreToken->pFeature[i] = '0';

				pInsertNode->pPrev = m_pTailNode->pPrev;
				pInsertNode->pNext = m_pTailNode;

				m_pTailNode->pPrev->pNext = pInsertNode;
				m_pTailNode->pPrev = pInsertNode;
			}
		}
	}
}

int CNAPreLinkedList::AddPreNode(PRE_NODE* pPreNode, char* pInputStr, int nOffset)
{
	if (strlen(pInputStr) > 0)
	{
		int nLen = 0;
		PRE_NODE* pInsertNode;
		PRE_NODE* pNextNode = pPreNode->pNext;
		pInsertNode = new PRE_NODE;

		int i;

		if (pInsertNode != NULL)
		{
			pInsertNode->pPreToken = new PRE_TOKEN;
			if (pInsertNode->pPreToken != NULL)
			{
				nLen = strlen(pInputStr);
				pInsertNode->pPreToken->strOriginalSource = new char[nLen + 1];
				memset(pInsertNode->pPreToken->strOriginalSource, 0x00, nLen + 1);
				strcpy(pInsertNode->pPreToken->strOriginalSource, pInputStr);
				pInsertNode->pPreToken->nOffset = nOffset;
				pInsertNode->pPreToken->nEndCheck = 0;
				pInsertNode->pPreToken->nFirstParent = 0;
				pInsertNode->pPreToken->nLastParent = 0;

				memset(pInsertNode->pPreToken->pFeature, 0x00, PRE_FEATURE + 1);
				for (i = 0; i<PRE_FEATURE; i++)
					pInsertNode->pPreToken->pFeature[i] = '0';

				pInsertNode->pPrev = pPreNode;
				pInsertNode->pNext = pNextNode;

				pPreNode->pNext = pInsertNode;
				pNextNode->pPrev = pInsertNode;
				return 1;
			}
		}
	}
	return -1;
}

void CNAPreLinkedList::RemovePreNode(PRE_NODE* pPreNode)
{
	PRE_NODE* pBeforeNode = pPreNode->pPrev;
	PRE_NODE* pNextNode = pPreNode->pNext;

	if (pPreNode != NULL)
	{
		pBeforeNode->pNext = pNextNode;
		pNextNode->pPrev = pBeforeNode;

		if (pPreNode->pPreToken->strOriginalSource != NULL)
		{
			delete[]pPreNode->pPreToken->strOriginalSource;
			pPreNode->pPreToken->strOriginalSource = NULL;
		}

		if (pPreNode->pPreToken != NULL)
		{
			delete[]pPreNode->pPreToken;
			pPreNode->pPreToken = NULL;
		}
		if (pPreNode != NULL)
		{
			delete pPreNode;
			pPreNode = NULL;
		}
	}
}

void CNAPreLinkedList::RemovePreAll()
{
	PRE_NODE* pStartNode;
	PRE_NODE* pDeleteNode;

	pStartNode = m_pHeadNode->pNext;

	while (pStartNode != m_pTailNode)
	{
		pDeleteNode = pStartNode;
		pStartNode = pStartNode->pNext;

		if (pDeleteNode->pPreToken->strOriginalSource != NULL)
		{
			delete[]pDeleteNode->pPreToken->strOriginalSource;
			pDeleteNode->pPreToken->strOriginalSource = NULL;
		}
		
		if (pDeleteNode->pPreToken != NULL)
		{
			delete[]pDeleteNode->pPreToken;
			pDeleteNode->pPreToken = NULL;
		}
		
		//after free pDelete
		if (pDeleteNode != NULL)
		{
			delete pDeleteNode;
			pDeleteNode = NULL;
		}
	}

	m_pHeadNode->pNext = NULL;//link free
	m_pTailNode->pPrev = NULL;//link free

	if (m_pHeadNode != NULL)
	{
		delete m_pHeadNode;
		m_pHeadNode = NULL;
	}

	if (m_pTailNode != NULL)
	{
		delete m_pTailNode;
		m_pTailNode = NULL;
	}
}

void CNAPreLinkedList::InitHeadTailNode()
{
	m_pHeadNode = new PRE_NODE;
	m_pTailNode = new PRE_NODE;

	m_pHeadNode->pNext = m_pTailNode;
	m_pHeadNode->pPrev = m_pHeadNode;

	m_pTailNode->pNext = m_pTailNode;
	m_pTailNode->pPrev = m_pHeadNode;

	m_pHeadNode->pPreToken = NULL;
	m_pTailNode->pPreToken = NULL;

	m_pCurrentPreNode = NULL;
}

PRE_NODE* CNAPreLinkedList::GoPreNodeFirst()
{
	if (m_pHeadNode->pNext != m_pTailNode)
	{
		m_pCurrentPreNode = m_pHeadNode->pNext;
	}
	else
		return NULL;

	return m_pCurrentPreNode;
}

PRE_NODE* CNAPreLinkedList::GoPreNodeLast()
{
	if (m_pTailNode->pPrev != m_pHeadNode)
	{
		m_pCurrentPreNode = m_pTailNode->pPrev;
	}
	else
		return NULL;

	return m_pCurrentPreNode;
}

PRE_NODE* CNAPreLinkedList::GoPreNodeNext()
{
	if (m_pCurrentPreNode != NULL && m_pCurrentPreNode->pNext != m_pTailNode)
	{
		m_pCurrentPreNode = m_pCurrentPreNode->pNext;
	}
	else
		return NULL;

	return m_pCurrentPreNode;
}

PRE_NODE* CNAPreLinkedList::GoPreNodePrev()
{
	if (m_pCurrentPreNode != NULL && m_pCurrentPreNode->pPrev != m_pHeadNode)
	{
		m_pCurrentPreNode = m_pCurrentPreNode->pPrev;
	}
	else
		return NULL;

	return m_pCurrentPreNode;
}

PRE_NODE* CNAPreLinkedList::GetPreNode()
{
	return m_pCurrentPreNode;
}

char* CNAPreLinkedList::PreNodeUniteStr(PRE_NODE* pPreNode1, PRE_NODE* pPreNode2)
{
	char* pReturn = NULL;
	int nLen1, nLen2, i, j;

	if (pPreNode1 != NULL && pPreNode2 != NULL)
	{
		if (pPreNode1->pPreToken != NULL && pPreNode2->pPreToken != NULL)
		{
			nLen1 = strlen(pPreNode1->pPreToken->strOriginalSource);
			nLen2 = strlen(pPreNode2->pPreToken->strOriginalSource);

			pReturn = new char[nLen1 + nLen2 + 1];
			memset(pReturn, 0x00, nLen1 + nLen2 + 1);

			for (i = 0; i<nLen1; i++)
				pReturn[i] = pPreNode1->pPreToken->strOriginalSource[i];

			for (j = 0; j<nLen2; j++)
				pReturn[i++] = pPreNode2->pPreToken->strOriginalSource[j];
		}
	}

	return pReturn;
}

PRE_NODE* CNAPreLinkedList::PreNodeUnite(PRE_NODE* pPreNode1, PRE_NODE* pPreNode2)
{
	// 두노드의 String 합
	int nLen1, nLen2, i, j;
	char* pTmpOri = NULL;

	if (pPreNode1 != NULL && pPreNode2 != NULL)
	{
		if (pPreNode1->pPreToken != NULL && pPreNode2->pPreToken != NULL)
		{
			nLen1 = strlen(pPreNode1->pPreToken->strOriginalSource);
			nLen2 = strlen(pPreNode2->pPreToken->strOriginalSource);

			pTmpOri = new char[nLen1 + nLen2 + 1];
			memset(pTmpOri, 0x00, nLen1 + nLen2 + 1);

			for (i = 0; i<nLen1; i++)
				pTmpOri[i] = pPreNode1->pPreToken->strOriginalSource[i];

			for (j = 0; j<nLen2; j++)
				pTmpOri[i++] = pPreNode2->pPreToken->strOriginalSource[j];

			delete[] pPreNode1->pPreToken->strOriginalSource;
			pPreNode1->pPreToken->strOriginalSource = pTmpOri;	//Char 연결

			pPreNode1->pPreToken->nEndCheck = PRENODE_CHAR_1;

			for (i = 0; i<PRE_FEATURE; i++)
				pPreNode1->pPreToken->pFeature[i] = pPreNode2->pPreToken->pFeature[i];

			pPreNode1->pPreToken->nLastParent = pPreNode2->pPreToken->nLastParent;

			RemovePreNode(pPreNode2);
		}
	}
	return pPreNode1;
}

// Pre Node 분리하고 분리된 마지막 Node 리턴
// nPos + nCase 위치까지 분리
// nPos까지 String 변경
PRE_NODE* CNAPreLinkedList::PreNodeDivide(PRE_NODE* pPreNode, int nPos, int nCase)
{
	char tmp[1024];
	char tmpDivide[1024];
	int nLen, i, j = 0;

	if (pPreNode != NULL)
	{
		if (pPreNode->pPreToken != NULL)
		{
			memset(tmp, 0x00, 1024);
			memset(tmpDivide, 0x00, 1024);
			nLen = strlen(pPreNode->pPreToken->strOriginalSource);
			strcpy(tmp, pPreNode->pPreToken->strOriginalSource);

			delete[] pPreNode->pPreToken->strOriginalSource;
			pPreNode->pPreToken->strOriginalSource = new char[nPos + 1];
			memset(pPreNode->pPreToken->strOriginalSource, 0x00, nPos + 1);

			// String 변경
			for (i = 0; i<nPos; i++)
				pPreNode->pPreToken->strOriginalSource[i] = tmp[i];
			pPreNode->pPreToken->strOriginalSource[i] = 0;

			// 나머지 String 저장
			for (i = nPos + nCase; i<nLen; i++)
				tmpDivide[j++] = tmp[i];
			tmpDivide[j] = 0;

			if (AddPreNode(pPreNode, tmpDivide, pPreNode->pPreToken->nOffset + nPos + nCase) == 1)
			{
				pPreNode->pNext->pPreToken->nEndCheck = pPreNode->pPreToken->nEndCheck;
				pPreNode->pPreToken->nEndCheck = 1;
				pPreNode->pNext->pPreToken->nLastParent = pPreNode->pPreToken->nLastParent;
				pPreNode->pPreToken->nLastParent = 0;
			}
		}
	}
	return pPreNode->pNext;
}