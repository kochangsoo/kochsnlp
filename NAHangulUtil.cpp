#include "stdafx.h"
#include "NAHangulUtil.h"


CNAHangulUtil::CNAHangulUtil()
{
	m_pConvert = new CConvert;
}


CNAHangulUtil::~CNAHangulUtil()
{
	if (m_pConvert != NULL)
	{
		delete m_pConvert;
		m_pConvert = NULL;
	}
}

//�ش� ��ġ�� �ִ� ������ Byte�� Ȯ��(�ѱ�,����,����,��ȣ,Ư������,��Ÿ���Ȯ��...)
//nDirection:1(���ʿ���������), 2(�����ʿ�������)
//���ʿ��� ������ : 0���� ����
//�����ʿ��� ���� : ���ڿ� ��ü����-1���� ����
int CNAHangulUtil::GetCheckNnumberBytesCharacter(char* pWords, int nPos, int nDirection)
{
	int nByteCheck = 0;
	int nLength = 0;
	if(pWords != NULL)
	{
		nLength = strlen(pWords);
		//�ϼ��� ��������Ʈ üũ(0xB0~0xC8) 25����, ��������Ʈ üũ(0xA1~0xFE) 94����
		unsigned char ch = *(pWords + nPos);
		nByteCheck = nPos;
		if (nDirection == DIC_STEM)
		{	
			//�ƽ�Ű�ڵ尪���� ū ��� 2Byte�� ��
			if ((ch >=0xB0 && ch <= 0xC8) || ch > 255)
			{
				nByteCheck = nPos + 1;
			}
		}
		else//DIC_ENDING
		{
			if ((ch >= 0xA1 && ch <= 0xFE) || ch > 255)
			{
				nByteCheck = nPos - 1;
			}
		}
	}
	return nByteCheck;
}

//����->������ ���� ��ŭ ���ڿ� ���
char* CNAHangulUtil::GetLeftToRightWord(char* pWords, int nPos)
{
	char* pReturnStr = NULL;
	if(pWords != NULL)
	{
		pReturnStr = new char[nPos + 2];
		memset(pReturnStr, 0x00, nPos + 2);
		for (int i = 0; i <= nPos; i++) {
			pReturnStr[i] = pWords[i];
		}
	}
	return pReturnStr;
}

//������->���� ���� ��ŭ ���ڿ� ���
char* CNAHangulUtil::GetRightToLeftWord(char* pWords, int nPos)
{
	char* pReturnStr = NULL;
	int nWordLength = 0;
	if(pWords != NULL)
	{ 	
		nWordLength = strlen(pWords);
		pReturnStr = new char[nWordLength - nPos + 2];
		memset(pReturnStr, 0x00, nWordLength - nPos + 2);
		for (int i = nPos; i <= nWordLength-1; i++) {
			pReturnStr[i- nPos] = pWords[i];
		}
	}
	return pReturnStr;
}

//�ش� ��ġ�� �� ����
//nPos �迭�� ÷�ڰ�(0~1,2,3)
char* CNAHangulUtil::GetPositionOneChar(char* pWords, int nPos, int nDirection)
{
	char* pReturnStr = NULL;
	int nLength = 0;
	if (pWords != NULL)
	{
		nLength = strlen(pWords);
		unsigned char ch = *(pWords + nPos);
		if (nDirection == DIC_STEM)
		{
			//if (*(pWord + nPos) & 0x80)
			if ((ch >= 0xB0 && ch <= 0xC8) || ch > 255)
			{
				pReturnStr = new char[3];
				memset(pReturnStr, 0x00, 3);
				pReturnStr[0] = *(pWords + nPos);
				pReturnStr[1] = *(pWords + nPos + 1);
			}
			else
			{
				pReturnStr = new char[2];
				memset(pReturnStr, 0x00, 2);
				pReturnStr[0] = *(pWords + nPos);
			}
		}
		else if (nDirection == DIC_ENDING)
		{
			//if (*(pWord - nPos - 1) & 0x80)
			if ((ch >= 0xA1 && ch <= 0xFE) || ch > 255)
			{
				pReturnStr = new char[3];
				memset(pReturnStr, 0x00, 3);
				pReturnStr[0] = *(pWords + nPos - 1);
				pReturnStr[1] = *(pWords + nPos);
			}
			else
			{
				pReturnStr = new char[2];
				memset(pReturnStr, 0x00, 2);
				pReturnStr[0] = *(pWords + nPos);
			}
			
		}
	}
	return pReturnStr;
}


//�ش� ���ڿ��� ������ ���� ��
char* CNAHangulUtil::GetFirstOrLastOneChar(char* pWords, int nDirection)
{
	char* pReturnStr = NULL;
	int nLength = 0;
	if (pWords != NULL)
	{
		nLength = strlen(pWords);
		
		if (nDirection == DIC_STEM)
		{
			unsigned char ch = *(pWords + (nLength - 1));
			if ((ch >= 0xA1 && ch <= 0xFE) || ch > 255)
			{
				pReturnStr = new char[3];
				memset(pReturnStr, 0x00, 3);
				pReturnStr[0] = *(pWords + nLength - 2);
				pReturnStr[1] = *(pWords + nLength-1);
			}
			else
			{
				pReturnStr = new char[2];
				memset(pReturnStr, 0x00, 2);
				pReturnStr[0] = *(pWords + nLength - 1);
			}
		}
		else if (nDirection == DIC_ENDING)
		{
			unsigned char ch = *(pWords);
			if ((ch >= 0xB0 && ch <= 0xC8) || ch > 255)
			{
				pReturnStr = new char[3];
				memset(pReturnStr, 0x00, 3);
				pReturnStr[0] = *(pWords);
				pReturnStr[1] = *(pWords + 1);
			}
			else
			{
				pReturnStr = new char[2];
				memset(pReturnStr, 0x00, 2);
				pReturnStr[0] = *(pWords);
			}
		}
	}
	return pReturnStr;
}

//�ش� ������ ���߼� �Ǵ� ���� �� Ȯ��
char* CNAHangulUtil::GetWordsChoJungORJongSyllable(char* pOneChar, int nKind)
{
	char* pOriginalChar = NULL;
	char* pReturnChar = NULL;
	int nTotalCount = 0;
	int nCount = 0;
	int nLen = 0;
	char chChoJung[3];
	char chJong[3];
	memset(chChoJung, 0x00, 3);
	memset(chJong, 0x00, 3);
	
	unsigned char ch = *(pOneChar);
	//if (pOneChar[0] & 0x80)
	if ((ch >= 0xB0 && ch <= 0xC8) || ch > 255)
	{
		if (m_pConvert != NULL)
		{
			m_pConvert->EvalueLastChar(pOneChar, chChoJung, chJong);
		}
		if (nKind == CHAR_CHOJUNGSUNG)
		{
			if (chChoJung != NULL && strlen(chChoJung) > 0)
			{
				nLen = strlen(chChoJung);
				nCount = 0;
				pReturnChar = new char[nLen + 1];
				memset(pReturnChar, 0x00, nLen + 1);
				while (nLen)
				{
					pReturnChar[nCount] = chChoJung[nCount];
					nLen -= 1;
					nCount += 1;
				}
			}
		}

		if (nKind == CHAR_JONGSUNG)
		{
			nLen = strlen(chJong);
			nCount = 0;
			pReturnChar = new char[nLen + 1];
			memset(pReturnChar, 0x00, nLen + 1);
			while (nLen)
			{
				pReturnChar[nCount] = chJong[nCount];
				nLen -= 1;
				nCount += 1;
			}
		}
	}
	else
	{
		pReturnChar = new char[2];
		memset(pReturnChar, 0x00, 2);
		strcpy(pReturnChar, "");
	}
	return pReturnChar;
}

//�ش� ������ ����,�߼�,������
void CNAHangulUtil::GetWordsChoJungJongSyllable(char* pOneChar, char* pCho, char* pJung, char* pJong)
{
	unsigned char ch = *(pOneChar);
	if ((ch >= 0xB0 && ch <= 0xC8) || ch > 255)
	{
		if (m_pConvert != NULL)
		{
			m_pConvert->EvaluateDevChar(pOneChar, pCho, pJung, pJong);
		}
	}
}

//�ʼ� + �߼�+���� ����
void CNAHangulUtil::ChangeCompulsionChoJungJongSyllable(char* pOneChar, char* pCho, char* pJung, char* pJong)
{
	m_pConvert->EvaluateChoJungJongHapChar(pOneChar, pCho, pJung, pJong);
}

//�ش� ��ġ�� ���� ���� //nPos�� ��� ������ ó�� �Ǵ� ���� ���� �����ϰ� �Ѵ�.
char* CNAHangulUtil::SetChangeSyllableWord(char* pWords, char* pOneWord,int nPos)
{
	char* pReturnStr = NULL;
	int nLength = 0;
	int pWordsLen = 0;
	int pOneWordLen = 0;
	unsigned char ch = NULL;
	int nStart = 0;
	int nEnd = 0;
	if(pWords != NULL  && pOneWord != NULL && strcmp(pWords,"") != 0)
	{
		pWordsLen = strlen(pWords);
		pOneWordLen = strlen(pOneWord);
		nLength = pWordsLen + pOneWordLen;
		pReturnStr = new char[nLength];
		memset(pReturnStr,0x00, nLength);
		//�迭�� �ε��� ������ �ٲ۴�.
		if(pWordsLen > 0){pWordsLen -= 1;}
		if (pOneWordLen > 0){pOneWordLen -= 1;}
		nLength = 0;
		//ó�� ���� ����
		if(nPos == CHANGE_CHAR_SYLLABLE_FIRST)
		{
			if(pOneWord != NULL && pOneWord != "")
			{
				for (int i = 0; i <= pOneWordLen; i++)
				{
					pReturnStr[nLength] = pOneWord[i];
					nLength += 1;
				}
			}
			nStart = 1;//�迭�� �ε����� 0���� �����ϹǷ� ���� ���� 1���� ���۵ȴ�
			ch = *(pWords);
			if ((ch >= 0xB0 && ch <= 0xC8) || ch > 255)
			{
				nStart = 2;//�ѱ��ϰ�� ������ġ
			}
			for (int j = nStart; j <= pWordsLen; j++)
			{
				pReturnStr[nLength] = pWords[j];
				nLength += 1;
			}
		}
		//������ ���� ����
		else if (nPos == CHANGE_CHAR_SYLLABLE_LAST)
		{
			ch = *(pWords+ pWordsLen);
			nEnd = pWordsLen;//�迭�� �ε��������� ������ ����Ѵ�.pWordsLen�� ��� ���ڿ� ��ü ���̿��� -1�� �� ����.
			if ((ch >= 0xA1 && ch <= 0xFE) || ch > 255)
			{
				nEnd = pWordsLen - 2;
			}
			for (int i = 0; i <= nEnd; i++)
			{
				pReturnStr[nLength] = pWords[i];
				nLength += 1;
			}
			if (pOneWord != NULL && pOneWord != "")
			{
				for (int i = 0; i <= pOneWordLen; i++)
				{
					pReturnStr[nLength] = pOneWord[i];
					nLength += 1;
				}
			}
		}
	}
	else
	{
		nLength = strlen(pOneWord) + 1;
		pReturnStr = new char[nLength];
		memset(pReturnStr, 0x00, nLength);
		strcpy(pReturnStr, pOneWord);
	}
	return pReturnStr;
}

//�� ���ڿ� ����
char* CNAHangulUtil::GetCombineTwoStrings(char* pOneWords, char* pTwoWords)
{
	char* pReturnStr = NULL;
	int nLength = 0;
	int pOneWordsLen = 0;
	int pTwoWordsLen = 0;
	if(pOneWords == NULL) pOneWords = "";
	if (pTwoWords == NULL) pTwoWords = "";
	if(pOneWords != NULL && pTwoWords != NULL)
	{
		pOneWordsLen = strlen(pOneWords);
		pTwoWordsLen = strlen(pTwoWords);
		nLength = pOneWordsLen + pTwoWordsLen + 1;
		pReturnStr = new char[nLength];
		memset(pReturnStr, 0x00, nLength);
		pOneWordsLen -= 1;
		pTwoWordsLen -= 1;
		nLength = 0;
		for(int i = 0; i <= pOneWordsLen; i++)
		{
			pReturnStr[nLength] = pOneWords[i];
			nLength += 1;
		}
		for (int j = 0; j <= pTwoWordsLen; j++)
		{
			pReturnStr[nLength] = pTwoWords[j];
			nLength += 1;
		}
	}
	return pReturnStr;
}

//����+���� ����
char* CNAHangulUtil::ChangeCompulsionJongSyllable(char* pChoJung, char* pJong)
{
	char* pReturnStr = NULL;
	pReturnStr = new char[3];
	memset(pReturnStr, 0x00, 3);
	if(pChoJung != NULL &&  pJong != NULL)
	{
		if (m_pConvert != NULL)
			m_pConvert->EvaluateHapChar(pReturnStr, pChoJung, pJong);
	}
	else
	{
		strcpy(pReturnStr, "");
	}
	return pReturnStr;
}


char* CNAHangulUtil::GetStartToEndWord(char* pWords, int nStart, int nEnd)
{
	char* pReturnStr = NULL;
	int nLength = 0;
	int nInx = 0;
	if((nStart > 0 && nEnd > 0 && (nStart == nEnd)) || (nStart > nEnd)) return pReturnStr;
	
	if(pWords != NULL && pWords != "")
	{
		nLength = strlen(pWords);
		pReturnStr = new char[nLength + 1];
		memset(pReturnStr,0x00,nLength+1);
		for(nStart; nStart<= nEnd; nStart++)
		{
			pReturnStr[nInx] = pWords[nStart];
			nInx  = nInx +1;
		}
	}
	return pReturnStr;
}

//nEnd���� ù��° ���������� ���ڰ���
int  CNAHangulUtil::GetFirstToEndSyllableCount(char* pWords, int nEnd)
{
	int nSyllableCount = 0;
	int nPos = 0;
	for (int i = nEnd; i >= 0; i--)
	{
		nPos = GetCheckNnumberBytesCharacter(pWords, i, DIC_ENDING);
		if(nPos >= 0) nSyllableCount += 1;
		i = nPos;
	}
	return nSyllableCount;
}


//�ұ�Ģ ó�� : �ش� ��ġ�� ���� ��ü
char* CNAHangulUtil::SetIrregularRestoreLocationSyllableWord(char* pWords, char* pOneWord, int nPos)
{
	char* pRestoreSyllableWord = NULL;
	int nLength = 0;
	int nRestorePos = -1;
	int nStart = 0;
	int nEnd = 0;
	if (pWords != NULL  && pOneWord != NULL && strcmp(pWords, "") != 0)
	{
		nEnd = strlen(pWords) - 1;
		nLength = strlen(pWords) + strlen(pOneWord) + 1;
		pRestoreSyllableWord = new char[nLength];
		memset(pRestoreSyllableWord,0x00, nLength);

		for(nStart = 0; nStart <= nEnd; nStart++)
		{
			if(nStart == nPos)
			{
				for(int i = 0; i <= strlen(pOneWord) -1; i++)
				{
					pRestoreSyllableWord[nStart] = pOneWord[i];
					nStart += 1;
				}
				nStart -= 1;
			}
			else
			{
				pRestoreSyllableWord[nStart] = pWords[nStart];
			}
		}
	}
	return pRestoreSyllableWord;
}