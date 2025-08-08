#pragma once

#include "ds_list.h"
#include "common.h"
#include "na_convert.h"

class CNAHangulUtil
{
public:
	CNAHangulUtil();
	virtual ~CNAHangulUtil();

public:
	CConvert *m_pConvert;

public:
	//nPos�� ��� �迭�� �ε��� 0���� ������. ��ü���̴� strlen(����) -1 ��	
	int GetCheckNnumberBytesCharacter(char* pWords, int nPos, int nDirection);//�ش� ��ġ�� �ִ� ����(charset) Ȯ�� nDirection:1(DIC_STEM:���ʿ���������),2(DIC_ENDING:�����ʿ�������)
	char* GetLeftToRightWord(char* pWords, int nPos);//���ʿ��������� ���� ��ŭ ���ڿ� ���(2Byte���� : 0~nPos+1, 1Byte���� :  0~nPos)
	char* GetRightToLeftWord(char* pWords, int nPos);//�����ʿ������� ���� ��ŭ ���ڿ� ���(2Byte���� : nPos-1~��ü����, 1Byte���� :  nPos~��ü����)
	char* GetPositionOneChar(char* pWords, int nPos, int nDirection);//�ش� ��ġ�� ���� ��
	char* GetFirstOrLastOneChar(char* pWords,int nDirection);//�ش� ���ڿ��� ������ ���� ��

	char* GetWordsChoJungORJongSyllable(char* pOneChar, int nKind);//�ش� ������ ����,������(CHAR_CHOJUNGSUNG,CHAR_JONGSUNG) 
	void GetWordsChoJungJongSyllable(char* pOneChar, char* pCho, char* pJung, char* pJong);//�ش� ������ ����,�߼�,������
	char* SetChangeSyllableWord(char* pWords, char* pOneWord,int nPos);//�ش� ��ġ�� ���� ���� //nPos(CHANGE_CHAR_SYLLABLE_FIRST,CHANGE_CHAR_SYLLABLE_LAST)�� ���� ������ ó�� �Ǵ� �������� ����
	//CHANGE_CHAR_SYLLABLE_LAST  pWords : �����, pOneWord : ��, --> �����
	//CHANGE_CHAR_SYLLABLE_FIRST pWords : �����ϴ�, pOneWord : ��, -->�����ϴ�
	char* GetCombineTwoStrings(char* pOneWords, char* pTwoWords);//�� ���ڿ� ����
	char* ChangeCompulsionJongSyllable(char* pChoJung, char* pJong);//����+���� ����
	
	void ChangeCompulsionChoJungJongSyllable(char* pOneChar, char* pCho, char* pJung, char* pJong);//�ʼ� + �߼�+���� ����

	char* GetStartToEndWord(char* pWords, int nStart, int nEnd);//nStart~nEnd������ ���ڿ� �� ��������
	int  GetFirstToEndSyllableCount(char* pWords, int nEnd);//nStart~nEnd������ ���ڿ� ����

	

	//�ұ�Ģ ó�� : �ش� ��ġ�� ���� ��ü
	char* SetIrregularRestoreLocationSyllableWord(char* pWords, char* pOneWord, int nPos);


};

