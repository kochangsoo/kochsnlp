#include "stdafx.h"
#include "NADocVector.h"


CNADocVector::CNADocVector()
{
	m_pDOC = new DOC;

	m_pHeadSentence = new SENTENCE;
	m_pTailSentence = new SENTENCE;

	m_pCurrentSentence = NULL;

	m_pHeadNode = NULL;
	m_pTailNode = NULL;
	m_nTotalCount = 0;

	//�ʱ�ȭ
	InitDoc();
}


CNADocVector::~CNADocVector()
{
	RemoveAllSentence();

	if (m_pDOC != NULL) {
		delete m_pDOC;
		m_pDOC = NULL;
	}
	if (m_pHeadSentence != NULL)
	{
		delete m_pHeadSentence;
		m_pHeadSentence = NULL;
	}
	if (m_pTailSentence != NULL)
	{
		delete m_pTailSentence;
		m_pTailSentence = NULL;
	}
}

// Doc, Sentence ���� �ʱ�ȭ
void CNADocVector::InitDoc()
{
	m_pDOC->nCount = 0;
	m_pDOC->sentence = NULL;
	memset(m_pDOC->szDocID, 0x00, sizeof(m_pDOC->szDocID));
	memset(m_pDOC->szFilename, 0x00, sizeof(m_pDOC->szFilename));

	//m_pDOC->nDate = 0;
	memset(m_pDOC->szAuthor, 0x00, sizeof(m_pDOC->szAuthor));
	memset(m_pDOC->szDate, 0x00, sizeof(m_pDOC->szDate));
	memset(m_pDOC->szTitle, 0x00, sizeof(m_pDOC->szTitle));

	m_pHeadSentence->pNext = m_pTailSentence;
	m_pHeadSentence->pPrev = NULL;

	m_pTailSentence->pNext = NULL;
	m_pTailSentence->pPrev = m_pHeadSentence;

	m_pCurrentSentence = NULL;

	m_pHeadNode = NULL;
	m_pTailNode = NULL;
	m_nTotalCount = 0;
}

void CNADocVector::SetTitle(char* pTitle)
{
}

void CNADocVector::SetFilename(char* pFilename)
{
}

void CNADocVector::SetDocID(char* pDocID)
{
}

void CNADocVector::SetAuthor(char* pAuthor)
{
}



// ����� �ʱ�ȭ
void CNADocVector::RemoveTailSentence()
{
	SENTENCE* pBeforeSen = NULL;
	SENTENCE* pDeleteSen = m_pTailSentence->pPrev;

	if (pDeleteSen != m_pHeadSentence)
	{
		pBeforeSen = pDeleteSen->pPrev;

		m_pTailSentence->pPrev = pBeforeSen;
		pBeforeSen->pNext = m_pTailSentence;

		pDeleteSen->pNext = NULL;
		pDeleteSen->pPrev = NULL;

		RemoveAllNode(pDeleteSen);

		m_pCurrentSentence = pBeforeSen;
		m_pHeadNode = m_pCurrentSentence->pHead;
		m_pTailNode = m_pCurrentSentence->pTail;
	}
}

// ����� �ʱ�ȭ
void CNADocVector::RemoveAllSentence()
{
	SENTENCE* pDeleteSen = NULL;

	pDeleteSen = m_pTailSentence->pPrev;

	while (pDeleteSen != m_pHeadSentence)
	{
		m_pTailSentence->pPrev = pDeleteSen->pPrev;
		pDeleteSen->pPrev->pNext = m_pTailSentence;

		pDeleteSen->pNext = NULL;
		pDeleteSen->pPrev = NULL;

		RemoveAllNode(pDeleteSen);

		pDeleteSen = m_pTailSentence->pPrev;
	}

	InitDoc();
}

// ������ H_NODE �ʱ�ȭ Start////////////////////////////////////////////////////////////
void CNADocVector::RemoveAllNode(SENTENCE* pDeleteSen)
{
	H_NODE* pStartNode = NULL;
	H_NODE* pDeleteNode = NULL;

	if (pDeleteSen != NULL)
	{
		pStartNode = pDeleteSen->pHead->pNext;

		while (pStartNode != pDeleteSen->pTail)
		{
			pDeleteNode = pStartNode;
			pStartNode = pStartNode->pNext;

			DeleteNode(pDeleteNode);
		}

		if (pDeleteSen->pHead != NULL)
			delete pDeleteSen->pHead;
		if (pDeleteSen->pTail != NULL)
			delete pDeleteSen->pTail;

		delete pDeleteSen;
		pDeleteSen = NULL;
	}
}

void CNADocVector::DeleteHToken(H_TOKEN* pDelHToken)
{
	if (pDelHToken->strSource != NULL)
	{
		delete[]pDelHToken->strSource;
		pDelHToken->strSource = NULL;
	}
	if (pDelHToken->strOriginalSource != NULL)
	{
		delete[]pDelHToken->strOriginalSource;
		pDelHToken->strOriginalSource = NULL;
	}
	if (pDelHToken->strAmbEndingSource != NULL)
	{
		delete[]pDelHToken->strAmbEndingSource;
		pDelHToken->strAmbEndingSource = NULL;
	}

	DeleteSubToken(pDelHToken->subStem);
	DeleteSubToken(pDelHToken->subEnding);

	if (pDelHToken != NULL)
	{
		delete pDelHToken;
		pDelHToken = NULL;
	}
}

void CNADocVector::DeleteSubCopyNode(H_NODE* pDelSubNode)
{
	if (pDelSubNode->pHToken->bSubCompCreate == true)
		DeleteSubHToken(pDelSubNode->pHToken,"STEM");
	if (pDelSubNode->pHToken->bSubEndingCompCreate == true)
		DeleteSubHToken(pDelSubNode->pHToken, "ENDING");

	bool bFind = false;
	H_NODE* pNext;
	H_NODE* pMoveNext;
	pNext = pDelSubNode;
	pMoveNext = pDelSubNode;
	for (;;)
	{
		DeleteHToken(pMoveNext->pHToken); //Token & SubToke Delete!
		pNext = pNext->pNext;
		pMoveNext->pNext = NULL;
		pMoveNext->pPrev = NULL;
		if (pNext == pMoveNext)
			bFind = true;
		delete pMoveNext;
		pMoveNext = NULL;
		pMoveNext = pNext;
		if (bFind)
			break;
		if (pMoveNext == pMoveNext->pNext)
			bFind = true;
	}
}

void CNADocVector::DeleteSubToken(SUB_TOKEN* pSubToken)
{
	if (pSubToken != NULL)
	{
		if (pSubToken->strResult != NULL)
		{
			delete[]pSubToken->strResult;
			pSubToken->strResult = NULL;
		}

		if (pSubToken->strFeature != NULL)
		{
			delete[]pSubToken->strFeature;
			pSubToken->strFeature = NULL;
		}

		if (pSubToken != NULL)
		{
			delete pSubToken;
			pSubToken = NULL;
		}
	}
}

void CNADocVector::DeleteSubHToken(H_TOKEN *pDelHSubToken, char* strHFlag)
{
	H_SUBNODE* pStartNode;
	H_SUBNODE* pDeleteNode;
	//���ս��ۻ���
	if(strcmp(strHFlag,"STEM") == 0)
	{
		pStartNode = pDelHSubToken->pSubHeadNode->pNext;
		while (pStartNode != pDelHSubToken->pSubTailNode)
		{
			pDeleteNode = pStartNode;
			pStartNode = pStartNode->pNext;

			if (pDeleteNode->pCSubToken != NULL)
			{
				if (pDeleteNode->pCSubToken->strResult != NULL)
				{
					delete[]pDeleteNode->pCSubToken->strResult;
					pDeleteNode->pCSubToken->strResult = NULL;
				}
				if (pDeleteNode->pCSubToken->strFeature != NULL)
				{
					delete[]pDeleteNode->pCSubToken->strFeature;
					pDeleteNode->pCSubToken->strFeature = NULL;
				}
				if (pDeleteNode->pCSubToken != NULL)
				{
					delete[]pDeleteNode->pCSubToken;
					pDeleteNode->pCSubToken = NULL;
				}
			}
			if (pDeleteNode != NULL)
			{
				delete pDeleteNode;
				pDeleteNode = NULL;
			}
			if (pStartNode == pStartNode->pNext)
				break;
		}
		if (pDelHSubToken->pSubHeadNode->pNext != NULL)
			pDelHSubToken->pSubHeadNode->pNext = NULL;//link free
		if (pDelHSubToken->pSubTailNode->pPrev != NULL)
			pDelHSubToken->pSubTailNode->pPrev = NULL; //link free


		if (pDelHSubToken->pSubHeadNode != NULL)
		{
			delete pDelHSubToken->pSubHeadNode;
			pDelHSubToken->pSubHeadNode = NULL;
		}

		if (pDelHSubToken->pSubTailNode != NULL)
		{
			delete pDelHSubToken->pSubTailNode;
			pDelHSubToken->pSubTailNode = NULL;
		}
	}
	//���տ�������
	else if (strcmp(strHFlag, "ENDING") == 0)
	{
		pStartNode = pDelHSubToken->pSubEndingHeadNode->pNext;
		while (pStartNode != pDelHSubToken->pSubEndingTailNode)
		{
			pDeleteNode = pStartNode;
			pStartNode = pStartNode->pNext;

			if (pDeleteNode->pCSubToken != NULL)
			{
				if (pDeleteNode->pCSubToken->strResult != NULL)
				{
					delete[]pDeleteNode->pCSubToken->strResult;
					pDeleteNode->pCSubToken->strResult = NULL;
				}
				if (pDeleteNode->pCSubToken->strFeature != NULL)
				{
					delete[]pDeleteNode->pCSubToken->strFeature;
					pDeleteNode->pCSubToken->strFeature = NULL;
				}
				if (pDeleteNode->pCSubToken != NULL)
				{
					delete[]pDeleteNode->pCSubToken;
					pDeleteNode->pCSubToken = NULL;
				}
			}
			if (pDeleteNode != NULL)
			{
				delete pDeleteNode;
				pDeleteNode = NULL;
			}
			if (pStartNode == pStartNode->pNext)
				break;
		}
		if (pDelHSubToken->pSubEndingHeadNode->pNext != NULL)
			pDelHSubToken->pSubEndingHeadNode->pNext = NULL;//link free
		if (pDelHSubToken->pSubEndingTailNode->pPrev != NULL)
			pDelHSubToken->pSubEndingTailNode->pPrev = NULL; //link free


		if (pDelHSubToken->pSubEndingHeadNode != NULL)
		{
			delete pDelHSubToken->pSubEndingHeadNode;
			pDelHSubToken->pSubEndingHeadNode = NULL;
		}

		if (pDelHSubToken->pSubEndingTailNode != NULL)
		{
			delete pDelHSubToken->pSubEndingTailNode;
			pDelHSubToken->pSubEndingTailNode = NULL;
		}
	}
}


void CNADocVector::DeleteSubNode(H_SUBNODE* pDeleteNode)
{
	if (pDeleteNode->pCSubToken != NULL)
	{
		if (pDeleteNode->pCSubToken->strResult != NULL)
		{
			delete[]pDeleteNode->pCSubToken->strResult;
			pDeleteNode->pCSubToken->strResult = NULL;
		}
		if (pDeleteNode->pCSubToken->strFeature != NULL)
		{
			delete[]pDeleteNode->pCSubToken->strFeature;
			pDeleteNode->pCSubToken->strFeature = NULL;
		}
		if (pDeleteNode->pCSubToken != NULL)
		{
			delete[]pDeleteNode->pCSubToken;
			pDeleteNode->pCSubToken = NULL;
		}
	}
	if (pDeleteNode != NULL)
	{
		delete pDeleteNode;
		pDeleteNode = NULL;
	}
}

void CNADocVector::DeleteNode(H_NODE* pDeleteNode)
{
	if (pDeleteNode != NULL)
	{
		if (pDeleteNode->pHToken->bSubCompCreate == true)
		{
			DeleteSubHToken(pDeleteNode->pHToken,"STEM");
			pDeleteNode->pHToken->bSubCompCreate = false;
		}
		//���վ�� ����
		if (pDeleteNode->pHToken->bSubEndingCompCreate == true)
		{
			DeleteSubHToken(pDeleteNode->pHToken, "ENDING");
			pDeleteNode->pHToken->bSubEndingCompCreate = false;
		}


		if (pDeleteNode->pHToken != NULL)
			DeleteHToken(pDeleteNode->pHToken); 

		if (pDeleteNode != NULL)
		{
			pDeleteNode->pNext = NULL;
			pDeleteNode->pPrev = NULL;
			delete pDeleteNode;
			pDeleteNode = NULL;
		}
	}
}


// ������ H_NODE End ////////////////////////////////////////////////////////////

// ���� �߰� Start //////////////////////////////////////////////////////////////
int CNADocVector::AddSentenceHead(char* pSentence, int nOffset)
{
	int nReturn = -1;
	SENTENCE* pNewSentence = NULL;

	if (pSentence != NULL && strlen(pSentence) > 0)
	{
		pNewSentence = new SENTENCE;
		InitMorphList();

		// ù��° ���� Link
		if (m_pDOC->sentence == NULL)
			m_pDOC->sentence = pNewSentence;

		pNewSentence->nOffset = nOffset;
		pNewSentence->pHead = m_pHeadNode;
		pNewSentence->pTail = m_pTailNode;
		memset(pNewSentence->szSentence, 0x00, sizeof(pNewSentence->szSentence));
		strcpy(pNewSentence->szSentence, pSentence);

		//ù��° ����
		if (m_pCurrentSentence == NULL)
		{
			m_pHeadSentence->pNext = pNewSentence;
			m_pTailSentence->pPrev = pNewSentence;

			pNewSentence->pNext = m_pTailSentence;
			pNewSentence->pPrev = m_pHeadSentence;
		}
		else
		{
			pNewSentence->pPrev = m_pHeadSentence;
			pNewSentence->pNext = m_pHeadSentence->pNext;

			m_pHeadSentence->pNext->pPrev = pNewSentence;

			m_pHeadSentence->pNext = pNewSentence;
		}

		m_pCurrentSentence = pNewSentence;
		m_pDOC->nCount++;
		nReturn = 1;
	}

	return nReturn;
}

int CNADocVector::AddSentenceTail(char* pSentence, int nOffset)
{
	int nReturn = -1;
	SENTENCE* pNewSentence = NULL;
	
	if (pSentence != NULL)
	{
		pNewSentence = new SENTENCE;
		InitMorphList();

		// ù��° ���� Link
		if (m_pDOC->sentence == NULL)
			m_pDOC->sentence = pNewSentence;

		pNewSentence->nOffset = nOffset;
		pNewSentence->pHead = m_pHeadNode;
		pNewSentence->pTail = m_pTailNode;

		
		memset(pNewSentence->szSentence, 0x00, sizeof(pNewSentence->szSentence));
		strcpy(pNewSentence->szSentence, pSentence);

		//ù��° ����
		if (m_pCurrentSentence == NULL)
		{
			m_pHeadSentence->pNext = pNewSentence;
			m_pTailSentence->pPrev = pNewSentence;

			pNewSentence->pNext = m_pTailSentence;
			pNewSentence->pPrev = m_pHeadSentence;
		}
		else
		{
			pNewSentence->pNext = m_pTailSentence;
			pNewSentence->pPrev = m_pTailSentence->pPrev;

			m_pTailSentence->pPrev->pNext = pNewSentence;

			m_pTailSentence->pPrev = pNewSentence;
		}

		m_pCurrentSentence = pNewSentence;
		m_pDOC->nCount++;
		nReturn = 1;
	}
	return nReturn;
}

void CNADocVector::ReplaceSentenceStr(char* pSentence)
{
	memset(m_pCurrentSentence->szSentence, 0x00, sizeof(m_pCurrentSentence->szSentence));
	strcpy(m_pCurrentSentence->szSentence, pSentence);
}
// ���� �߰� End //////////////////////////////////////////////////////////////

// ���� ����(SENTENCE)�� ù��° Node ����
void CNADocVector::SetSentenceNode(H_NODE* pFirst, H_NODE* pLast)
{
	m_pCurrentSentence->pHead->pNext = pFirst;
	pFirst->pPrev = m_pCurrentSentence->pHead;
	m_pCurrentSentence->pTail->pPrev = pLast;
	pLast->pNext = m_pCurrentSentence->pTail;
}

// m_pCurrentSentence�� ù��° ���� ��ġ�� �̵�
SENTENCE* CNADocVector::GoSentenceFirst()
{
	if (m_pHeadSentence->pNext != m_pTailSentence)
	{
		m_pCurrentSentence = m_pHeadSentence->pNext;
	}
	else
		return NULL;

	return m_pCurrentSentence;
}

// m_pCurrentSentence�� ������ ���� ��ġ�� �̵�
SENTENCE* CNADocVector::GoSentenceLast()
{
	if (m_pTailSentence->pPrev != m_pHeadSentence)
	{
		m_pCurrentSentence = m_pTailSentence->pPrev;
	}
	else
		return NULL;

	return m_pCurrentSentence;
}

// ���� ���� ��ġ �̵�
SENTENCE* CNADocVector::GoSentenceNext()
{
	if (m_pCurrentSentence != NULL && m_pCurrentSentence->pNext != m_pTailSentence)
	{
		m_pCurrentSentence = m_pCurrentSentence->pNext;
	}
	else
		return NULL;

	return m_pCurrentSentence;
}

// �� ���� ��ġ �̵�
SENTENCE* CNADocVector::GoSentencePrev()
{
	if (m_pCurrentSentence != NULL && m_pCurrentSentence->pPrev != m_pHeadSentence)
	{
		m_pCurrentSentence = m_pCurrentSentence->pPrev;
	}
	else
		return NULL;

	return m_pCurrentSentence;
}

SENTENCE* CNADocVector::GetSentence()
{
	return m_pCurrentSentence;
}

// Linked List API Start ///////////////////////////////////////////////////////////
void CNADocVector::InitMorphList()
{
	m_pHeadNode = new H_NODE;
	m_pTailNode = new H_NODE;

	m_pHeadNode->pNext = m_pTailNode;
	m_pHeadNode->pPrev = m_pHeadNode;//NULL;

	m_pTailNode->pNext = m_pTailNode;//NULL;
	m_pTailNode->pPrev = m_pHeadNode;

	m_pHeadNode->pHToken = NULL;
	m_pTailNode->pHToken = NULL;

	m_nTotalCount = 0;
}

int CNADocVector::AddTail(char *pInputStr, char* pFeature, int nOffset)
{
	int ReturnValue = -1;
	int i;
	if (AddTail(pInputStr, nOffset) != -1)
	{
		//������ �߰�
		m_pTailNode->pPrev->pHToken->subStem->strFeature = new char[235];
		memset(m_pTailNode->pPrev->pHToken->subStem->strFeature, 0x00, 235);
		for (i = 0; i<234; i++)
			m_pTailNode->pPrev->pHToken->subStem->strFeature[i] = pFeature[i];
	}
	return ReturnValue;
}

//Insert a Value to Tail 
int CNADocVector::AddTail(char *pInputStr, int nOffset)
{
	H_NODE* pInsertNode;
	int ReturnValue = -1;
	int nLen = 0;

	pInsertNode = new H_NODE;
	if (pInsertNode != NULL)
	{
		pInsertNode->pHToken = new H_TOKEN;
		if (pInsertNode->pHToken != NULL)
		{
			m_nTotalCount += 1;
			nLen = strlen(pInputStr);
			//���ڿ����� ����		
			//2017-09-28 �ű� �߰�
			pInsertNode->pHToken->strSourceLen = nLen;//�����ڿ��� ����
			pInsertNode->pHToken->nKindError = 0;//�м����
			pInsertNode->pHToken->nLastFindEndingSyllable = 0;//A~B������ ������ �����ϴ� ����� ���� ������ ��ġ
			pInsertNode->pHToken->nLastFindCompoundNoun = 0;
			nLen += 1;

			pInsertNode->pHToken->strSource = new char[nLen];
			memset(pInsertNode->pHToken->strSource, 0x00, nLen);

			pInsertNode->pHToken->strOriginalSource = new char[nLen];
			memset(pInsertNode->pHToken->strOriginalSource, 0x00, nLen);

			strcpy(pInsertNode->pHToken->strSource, pInputStr);
			strcpy(pInsertNode->pHToken->strOriginalSource, pInputStr);


			//������ ��尡 ������ ���� ��� Skip�� �Ѵ�. ^^;
			pInsertNode->nSkip = 0;
			pInsertNode->bMarsk = false;
			pInsertNode->pHToken->bSubCompCreate = false;
			pInsertNode->pHToken->nOffset = nOffset;

			pInsertNode->pHToken->pSubHeadNode = NULL;
			pInsertNode->pHToken->pSubTailNode = NULL;

			pInsertNode->pHToken->strAmbEndingSource = NULL;

			pInsertNode->pHToken->subStem = new SUB_TOKEN;
			pInsertNode->pHToken->subEnding = new SUB_TOKEN;

			pInsertNode->pHToken->subStem->strResult = NULL;
			pInsertNode->pHToken->subStem->strFeature = NULL;
			pInsertNode->pHToken->subStem->nFindPos = 0;

			pInsertNode->pHToken->subEnding->strResult = NULL;
			pInsertNode->pHToken->subEnding->strFeature = NULL;
			pInsertNode->pHToken->subEnding->nFindPos = 0;

			pInsertNode->pHToken->bSubEndingCompCreate = false;//���� ��� ���� �÷���
			pInsertNode->pHToken->pSubEndingHeadNode = NULL;
			pInsertNode->pHToken->pSubEndingTailNode = NULL;

			pInsertNode->pPrev = m_pTailNode->pPrev;
			pInsertNode->pNext = m_pTailNode;

			m_pTailNode->pPrev->pNext = pInsertNode;
			m_pTailNode->pPrev = pInsertNode;

			ReturnValue = 1;
		}
	}
	return ReturnValue;
}

//Insert a Value to Head
int CNADocVector::AddHead(char *pInputStr, int nOffset)
{
	H_NODE* pInsertNode;
	int ReturnValue = -1;
	int nLen = 0;
	int nStart = 0;

	pInsertNode = new H_NODE;
	if (pInsertNode != NULL)
	{
		pInsertNode->pHToken = new H_TOKEN;
		if (pInsertNode->pHToken != NULL)
		{
			m_nTotalCount += 1;

			nLen = strlen(pInputStr);
			nStart = 0;
			pInsertNode->pHToken->strSource = new char[nLen + 1];
			memset(pInsertNode->pHToken->strSource, 0x00, nLen + 1);
			while (nLen)
			{
				pInsertNode->pHToken->strSource[nStart] = *pInputStr;

				nStart += 1;
				nLen -= 1;
				pInputStr++;
			}
			pInsertNode->pHToken->subStem = new SUB_TOKEN;
			pInsertNode->pHToken->subEnding = new SUB_TOKEN;

			pInsertNode->pNext = m_pHeadNode->pNext;
			pInsertNode->pPrev = m_pHeadNode;

			m_pHeadNode->pNext->pPrev = pInsertNode;
			m_pHeadNode->pNext = pInsertNode;

			ReturnValue = 1;
		}
	}
	return ReturnValue;
}

int CNADocVector::AddBeforeNode(char *pInputStr, H_NODE *pNode, int nOffset)
{
	H_NODE* pInsertNode;
	int ReturnValue = -1;
	int nLen = 0;

	pInsertNode = new H_NODE;
	if (pInsertNode != NULL)
	{
		pInsertNode->pHToken = new H_TOKEN;
		if (pInsertNode->pHToken != NULL)
		{
			m_nTotalCount += 1;

			nLen = strlen(pInputStr);

			nLen += 1;
			pInsertNode->pHToken->strSource = new char[nLen];
			memset(pInsertNode->pHToken->strSource, 0x00, nLen);

			pInsertNode->pHToken->strOriginalSource = new char[nLen];
			memset(pInsertNode->pHToken->strOriginalSource, 0x00, nLen);

			strcpy(pInsertNode->pHToken->strSource, pInputStr);
			strcpy(pInsertNode->pHToken->strOriginalSource, pInputStr);


			//������ ��尡 ������ ���� ��� Skip�� �Ѵ�. ^^;
			pInsertNode->nSkip = 0;
			pInsertNode->bMarsk = false;
			pInsertNode->pHToken->bSubCompCreate = false;
			pInsertNode->pHToken->nOffset = nOffset;

			pInsertNode->pHToken->pSubHeadNode = NULL;
			pInsertNode->pHToken->pSubTailNode = NULL;

			pInsertNode->pHToken->strAmbEndingSource = NULL;

			pInsertNode->pHToken->subStem = new SUB_TOKEN;
			pInsertNode->pHToken->subEnding = new SUB_TOKEN;

			pInsertNode->pHToken->subStem->strResult = NULL;
			pInsertNode->pHToken->subStem->strFeature = NULL;

			pInsertNode->pHToken->subEnding->strResult = NULL;
			pInsertNode->pHToken->subEnding->strFeature = NULL;

			pInsertNode->pNext = pNode->pNext;
			pInsertNode->pPrev = pNode;

			pNode->pNext->pPrev = pInsertNode;
			pNode->pNext = pInsertNode;

			ReturnValue = 1;
		}
	}
	return ReturnValue;
}

int CNADocVector::AddLinkTail(H_NODE* pLink)
{
	int nReturn = -1;
	if (pLink != NULL)
	{
		pLink->pPrev = m_pTailNode->pPrev;
		pLink->pNext = m_pTailNode;

		m_pTailNode->pPrev->pNext = pLink;
		m_pTailNode->pPrev = pLink;

		nReturn = 1;
	}
	return nReturn;
}

H_NODE* CNADocVector::GetLastNode()
{
	if (m_pTailNode->pPrev == m_pHeadNode)
		return NULL;
	return m_pTailNode->pPrev;
}

H_NODE* CNADocVector::CreateNode(char *pInputStr, int nOffset)
{
	H_NODE* pInsertNode;
	int nLen = 0;

	if (pInputStr != NULL)
	{
		pInsertNode = new H_NODE;
		pInsertNode->pHToken = new H_TOKEN;
		if (pInsertNode->pHToken != NULL)
		{
			nLen = strlen(pInputStr);

			nLen += 1;
			pInsertNode->pHToken->strSource = new char[nLen];
			memset(pInsertNode->pHToken->strSource, 0x00, nLen);

			pInsertNode->pHToken->strOriginalSource = new char[nLen];
			memset(pInsertNode->pHToken->strOriginalSource, 0x00, nLen);

			strcpy(pInsertNode->pHToken->strSource, pInputStr);
			strcpy(pInsertNode->pHToken->strOriginalSource, pInputStr);


			//������ ��尡 ������ ���� ��� Skip�� �Ѵ�. ^^;
			pInsertNode->nSkip = 0;
			pInsertNode->bMarsk = false;
			pInsertNode->pHToken->bSubCompCreate = false;
			pInsertNode->pHToken->nOffset = nOffset;

			pInsertNode->pHToken->pSubHeadNode = NULL;
			pInsertNode->pHToken->pSubTailNode = NULL;

			pInsertNode->pHToken->strAmbEndingSource = NULL;

			pInsertNode->pHToken->subStem = new SUB_TOKEN;
			pInsertNode->pHToken->subEnding = new SUB_TOKEN;

			pInsertNode->pHToken->subStem->strResult = NULL;
			pInsertNode->pHToken->subStem->strFeature = NULL;

			pInsertNode->pHToken->subEnding->strResult = NULL;
			pInsertNode->pHToken->subEnding->strFeature = NULL;

			pInsertNode->pPrev = NULL;
			pInsertNode->pNext = NULL;
		}
	}
	return pInsertNode;
}

void CNADocVector::CreateCompoundSubNode(H_NODE *pNode, char* strHFlag)
{	
	if(strcmp(strHFlag,"STEM") == 0)
	{
		if (pNode->pHToken->bSubCompCreate == false)//FALSE�� ��츸 ó�� ��
		{
			pNode->pHToken->bSubCompCreate = true;

			pNode->pHToken->pSubHeadNode = new H_SUBNODE;
			pNode->pHToken->pSubTailNode = new H_SUBNODE;

			pNode->pHToken->pSubHeadNode->pCSubToken = NULL;
			pNode->pHToken->pSubTailNode->pCSubToken = NULL;

			pNode->pHToken->pSubHeadNode->pNext = pNode->pHToken->pSubTailNode;
			pNode->pHToken->pSubHeadNode->pPrev = pNode->pHToken->pSubHeadNode;
			pNode->pHToken->pSubTailNode->pNext = pNode->pHToken->pSubTailNode;
			pNode->pHToken->pSubTailNode->pPrev = pNode->pHToken->pSubHeadNode;
		}
	}
	else if (strcmp(strHFlag, "ENDING") == 0)
	{
		if (pNode->pHToken->bSubEndingCompCreate == false)//FALSE�� ��츸 ó�� ��
		{
			pNode->pHToken->bSubEndingCompCreate = true;

			pNode->pHToken->pSubEndingHeadNode = new H_SUBNODE;
			pNode->pHToken->pSubEndingTailNode = new H_SUBNODE;

			pNode->pHToken->pSubEndingHeadNode->pCSubToken = NULL;
			pNode->pHToken->pSubEndingTailNode->pCSubToken = NULL;

			pNode->pHToken->pSubEndingHeadNode->pNext = pNode->pHToken->pSubEndingTailNode;
			pNode->pHToken->pSubEndingHeadNode->pPrev = pNode->pHToken->pSubEndingHeadNode;
			pNode->pHToken->pSubEndingTailNode->pNext = pNode->pHToken->pSubEndingTailNode;
			pNode->pHToken->pSubEndingTailNode->pPrev = pNode->pHToken->pSubEndingHeadNode;
		}
	}
}


// Linked List API ENd   ///////////////////////////////////////////////////////////