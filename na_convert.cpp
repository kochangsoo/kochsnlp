//
//ns_convert.cpp
//

#include "na_convert.h"

CConvert::CConvert()
{
}

CConvert::~CConvert()
{
}

//한 음절의 초중,종성 분리
void CConvert::EvalueLastChar(char* InData, char* pleadChar, char* plastChar)
{
	unsigned char head[3], tail[3];

	int loc, dist=0;
	int indx;

	unsigned char leadCode;
 	unsigned char tailCode;

	unsigned int cho_middle;
	unsigned int last;
	
	unsigned int AssCode;

	//한자이면 처리
	if(GetCharCode(InData)==1) return;

	leadCode = InData[0];
 	tailCode = InData[1];

	//완성형을 조합형으로....(코드번호를 얻어내는 부분)
	loc=(leadCode-0xb0)*94+(tailCode-0xa1);
	AssCode=g_waKssmTable2350[loc];
	

	//종성부분을 삭제한다.
	cho_middle=AssCode&0xffe0;
	cho_middle=cho_middle|0x8001;

	dist = GetTableInx(cho_middle);

	//다시 조합형을 완성형으로 변환....초.중성을 얻어낸다.
	head[0]=0xb0+dist/94;
	head[1]=0xa1+dist%94;
	head[2]=0;
	strcpy(pleadChar, (char*)head);

	//종성을 분리해내는 부분.
	last=AssCode&0x801f;
	last=last|0x8440;

	for(indx=0; indx<69; indx++)
	{
		if(g_waKssmTableETC[indx]==last)
		{
			last=g_waKscTableETC[indx];
			break;
		}
	}

	//종성을 얻어낸다.
	tail[0]=(unsigned char)(last/256);
	tail[1]=(unsigned char)(last&0x00ff);
	tail[2]=0;
	strcpy(plastChar, (char*)tail);
	if(plastChar[0] == -92 && plastChar[1] == -44)
		plastChar[0] = '\0';
}

//한 음절의 초성,중성,종성 분리
void CConvert::EvaluateDevChar(char* InData, char* pHead, char* pMiddle, char* pTail)
{
	unsigned char head[3], middle[3], tail[3];

	int loc, dist=0;
	int indx;

	unsigned char hiWord;
 	unsigned char lowWord;

	unsigned int cho;
	unsigned int jung;
	unsigned int last;
	
	unsigned int AssCode;

	hiWord = InData[0];
 	lowWord = InData[1];

	//완성형을 조합형으로....(코드번호를 얻어내는 부분)
	loc=(hiWord-0xb0)*94+(lowWord-0xa1);
	
	//조합형 코드
	AssCode=g_waKssmTable2350[loc];

	//초성만 구한다.
	cho = AssCode&0xfc00;
	cho = cho|0x8041;

	for(indx=0; indx<69; indx++)
	{
		if(g_waKssmTableETC[indx]==cho)
		{
			cho=g_waKscTableETC[indx];
			break;
		}
	}
	head[0]=(unsigned char)(cho/256);
	head[1]=(unsigned char)(cho&0x00ff);
	head[2]=0;
	strcpy(pHead, (char*)head);

	//중성을 구한다.
	jung=AssCode&0x83e0;
	jung=jung|0xb401;

	dist = GetTableInx(jung);

	//다시 조합형을 완성형으로 변환....중성을 얻어낸다.
	middle[0]=0xb0+dist/94;
	middle[1]=0xa1+dist%94;
	middle[2]=0;
	strcpy(pMiddle, (char*)middle);

	//종성을 분리해내는 부분.
	last=AssCode&0x801f;
	last=last|0x8440;

	for(indx=0; indx<69; indx++)
	{
		if(g_waKssmTableETC[indx]==last)
		{
			last=g_waKscTableETC[indx];
			break;
		}
	}

	//종성을 얻어낸다.
	tail[0]=(unsigned char)(last/256);
	tail[1]=(unsigned char)(last&0x00ff);
	tail[2]=0;
	strcpy(pTail, (char*)tail);
}

//초중성 + 종성의 결과 
void CConvert::EvaluateHapChar(char *OutData, char *chChoJung, char *chJong)
{
	int loc, dist=0;
	int indx = 0;

	unsigned char leadCode;
 	unsigned char tailCode;

	unsigned int ChoJungCode;
	unsigned int JongCode;
	unsigned int AccCode;

	leadCode = chChoJung[0];
 	tailCode = chChoJung[1];

	loc=(leadCode-0xb0)*94+(tailCode-0xa0)-1;
	ChoJungCode=g_waKssmTable2350[loc];

	ChoJungCode=ChoJungCode&0xffe0;
	ChoJungCode=ChoJungCode|0x8001;
	ChoJungCode = ChoJungCode & 0xFFE0;

	for(int nCnt=0; nCnt < 27; nCnt++)
	{
		if(strcmp(g_JongChar[nCnt], chJong)==0)
		{
			JongCode = g_waKssmTbJong[nCnt];
			JongCode=JongCode & 0x001F;

			AccCode = ChoJungCode | JongCode;
			
			dist = GetTableInx(AccCode);

			if(dist == -1)
			{
				OutData[0] = '\0';
				return;
			}
			OutData[0]=0xb0+dist/94;
			OutData[1]=0xa0+dist%94+1;
			OutData[2]=0;
			return;
		}
	}
	OutData[0] = '\0';
}


//초성 + 중성  + 종성의 결과 
//초중종성을 유니코드로 변경한 후, 유니코드의 해당하는 조합형 코드의 초중종성을 조합하여 완성형값을 리턴한다
void CConvert::EvaluateChoJungJongHapChar(char *OutData, char *chCho, char *chJung,char *chJong)
{
	int  dist = 0;
	unsigned int ChoCode = 0;
	unsigned int JungCode = 0;
	unsigned int JongCode = 0;
	unsigned int AccCode = 0;
	unsigned short kssm, u;
	unsigned short cho, jung, jong;
	//유니코드 초중종성 시작 위치 확인	
	for (int nCnt = 0; nCnt < 19; nCnt++)
	{
		if (strcmp(g_unicodeCho[nCnt], chCho) == 0){ChoCode = nCnt;break;}
	}
	for (int nCnt = 0; nCnt < 21; nCnt++)
	{
		if (strcmp(g_unicodeJung[nCnt], chJung) == 0){JungCode = nCnt; break;}
	}
	for (int nCnt = 0; nCnt < 28; nCnt++)
	{
		if (strcmp(g_unicodeJong[nCnt], chJong) == 0){JongCode = nCnt; break;}
	}
	//AccCode  = 0xAC00(유니코드 한글 시작점) + [초성번호] * 588 + [중성번호] * 28 + [종성번호]
	/*
	AccCode = 0xAC00 + (28*21* ChoCode) + (28* JungCode) + JongCode;
	// unicode 한글영역 
	AccCode -= 0xac00;
	//유니코드의 초중종성 
	jong = AccCode % 28;
	jung = (AccCode % (28 * 21)) / 28;
	cho = AccCode / (28 * 21);
	*/
	// make kssm code
	//유니코드의 초중종성에 대응되는 조합형 초중종성 코드를 찾는다
	cho = _u2j_cho[ChoCode];
	jung = _u2j_jung[JungCode];
	jong = _u2j_jong[JongCode];
	kssm = 0x8000 | (cho << 10) | (jung << 5) | (jong);
	

	//완성형 코드에 대응되는 조합형 코드값을 찾은 후 최종 결과값은 완성형 결과값을 저장한다.
	dist = GetTableInx(kssm);
	if (dist > 0 && dist > 94)
	{
		OutData[0] = 0xb0 + dist / 94;
		OutData[1] = 0xa0 + dist % 94 + 1;
		OutData[2] = 0;
	}
}


int CConvert::GetTableInx(unsigned int nInput)
{
	int Low, High, Mid, CompRet, CompVal;

	Low = 0;
	High = 2350;

	while(Low <= High)
	{
		Mid = (Low + High) >> 1;
		CompVal = g_waKssmTable2350[Mid];
		CompRet = nInput - CompVal;
		if(CompRet == 0)
			return Mid;
		else if(CompRet < 0)
			High = Mid - 1;
		else
			Low = Mid + 1;
	}

	return -1;
}

int CConvert::GetCharCode(char *pKindCode)
{
	int syl = 0;
	char *p = pKindCode;

	syl = (*p << 8) & 0x0000FF00;
	syl |= *(p+1) & 0x000000FF;

	return GetHighLow(syl);
}

int CConvert::GetHighLow(int nKindCode)
{
	int ret = 0;
	int high,low;

	high = (nKindCode & 0xFF00) >> 8;
	low = (nKindCode & 0x00FF);

	ret = IsKindCode(high, low);
	return ret;
}

//한자 인지 판단
int CConvert::IsKindCode(int high, int low)
{
	if((high >= 0xCA && high <= 0xCF) && (low >= 0xA1 && low <= 0xFE) //hanja area 1
		   || (high >= 0xD0 && high <= 0xDF) && (low >= 0xA1 && low <= 0xFE) //hanja area 2
		   || (high >= 0xE0 && high <= 0xEF) && (low >= 0xA1 && low <= 0xFE) //hanja area 3 
		   || (high >= 0xF0 && high <= 0xFD) && (low >= 0xA1 && low <= 0xFE))//hanja area 4 
	{
		return 1;
	}
	else
		return -1;
}

/*
조합 방법 예제 - OR연산(| )을 사용한다. (0x는 16진수라는 C언어에서의 표기법임)
가 : ㄱ + ㅏ + <채움> : 0x8800 | 0x8060 | 0x8001 = 0x8861
각 : ㄱ + ㅏ + ㄱ : 0x8800 | 0x8060 | 0x8002 = 0x8862
훤 : ㅎ + ㅝ + ㄴ : 0xD000 | 0x82A0 | 0x8005 = 0xD2A5

분해 방법 예제 - AND연산(&)으로 마스킹 하는 방법을 쓴다.
각 - 초성 : 0x8862 & 0xFC00 = 0x8800 : ㄱ
각 - 중성 : 0x8862 & 0x83E0 = 0x8060 : ㅏ
각 - 종성 : 0x8862 & 0x801F = 0x8002 : ㄱ

분해된 코드는 단독으로 출력할 수 없고, <채움>코드와 다시 조합되어야 출력된다.
ㄱ : ㄱ + <채움>+<채움> : 0x8800 | 0x8040 | 0x8001 = 0x8841
*/
