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
	//nPos의 경우 배열의 인덱스 0부터 시작함. 전체길이는 strlen(어절) -1 임	
	int GetCheckNnumberBytesCharacter(char* pWords, int nPos, int nDirection);//해당 위치에 있는 문자(charset) 확인 nDirection:1(DIC_STEM:왼쪽에서오른쪽),2(DIC_ENDING:오른쪽에서왼쪽)
	char* GetLeftToRightWord(char* pWords, int nPos);//왼쪽에서오른쪽 길이 만큼 문자열 얻기(2Byte문자 : 0~nPos+1, 1Byte문자 :  0~nPos)
	char* GetRightToLeftWord(char* pWords, int nPos);//오른쪽에서왼쪽 길이 만큼 문자열 얻기(2Byte문자 : nPos-1~전체길이, 1Byte문자 :  nPos~전체길이)
	char* GetPositionOneChar(char* pWords, int nPos, int nDirection);//해당 위치의 음절 값
	char* GetFirstOrLastOneChar(char* pWords,int nDirection);//해당 문자열의 마지막 음절 값

	char* GetWordsChoJungORJongSyllable(char* pOneChar, int nKind);//해당 음절의 초중,종성값(CHAR_CHOJUNGSUNG,CHAR_JONGSUNG) 
	void GetWordsChoJungJongSyllable(char* pOneChar, char* pCho, char* pJung, char* pJong);//해당 음절의 초중,중성,종성값
	char* SetChangeSyllableWord(char* pWords, char* pOneWord,int nPos);//해당 위치의 음절 변경 //nPos(CHANGE_CHAR_SYLLABLE_FIRST,CHANGE_CHAR_SYLLABLE_LAST)에 따라 음절의 처음 또는 끝음절만 변경
	//CHANGE_CHAR_SYLLABLE_LAST  pWords : 사랑학, pOneWord : 하, --> 사랑하
	//CHANGE_CHAR_SYLLABLE_FIRST pWords : 였습니다, pOneWord : ㄴ, -->ㄴ습니다
	char* GetCombineTwoStrings(char* pOneWords, char* pTwoWords);//두 문자열 결합
	char* ChangeCompulsionJongSyllable(char* pChoJung, char* pJong);//초중+종성 결합
	
	void ChangeCompulsionChoJungJongSyllable(char* pOneChar, char* pCho, char* pJung, char* pJong);//초성 + 중성+종성 결합

	char* GetStartToEndWord(char* pWords, int nStart, int nEnd);//nStart~nEnd까지의 문자열 값 가져오기
	int  GetFirstToEndSyllableCount(char* pWords, int nEnd);//nStart~nEnd까지의 문자열 길이

	

	//불규칙 처리 : 해당 위치의 음절 대체
	char* SetIrregularRestoreLocationSyllableWord(char* pWords, char* pOneWord, int nPos);


};

