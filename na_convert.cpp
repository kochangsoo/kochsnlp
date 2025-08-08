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

//�� ������ ����,���� �и�
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

	//�����̸� ó��
	if(GetCharCode(InData)==1) return;

	leadCode = InData[0];
 	tailCode = InData[1];

	//�ϼ����� ����������....(�ڵ��ȣ�� ���� �κ�)
	loc=(leadCode-0xb0)*94+(tailCode-0xa1);
	AssCode=g_waKssmTable2350[loc];
	

	//�����κ��� �����Ѵ�.
	cho_middle=AssCode&0xffe0;
	cho_middle=cho_middle|0x8001;

	dist = GetTableInx(cho_middle);

	//�ٽ� �������� �ϼ������� ��ȯ....��.�߼��� ����.
	head[0]=0xb0+dist/94;
	head[1]=0xa1+dist%94;
	head[2]=0;
	strcpy(pleadChar, (char*)head);

	//������ �и��س��� �κ�.
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

	//������ ����.
	tail[0]=(unsigned char)(last/256);
	tail[1]=(unsigned char)(last&0x00ff);
	tail[2]=0;
	strcpy(plastChar, (char*)tail);
	if(plastChar[0] == -92 && plastChar[1] == -44)
		plastChar[0] = '\0';
}

//�� ������ �ʼ�,�߼�,���� �и�
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

	//�ϼ����� ����������....(�ڵ��ȣ�� ���� �κ�)
	loc=(hiWord-0xb0)*94+(lowWord-0xa1);
	
	//������ �ڵ�
	AssCode=g_waKssmTable2350[loc];

	//�ʼ��� ���Ѵ�.
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

	//�߼��� ���Ѵ�.
	jung=AssCode&0x83e0;
	jung=jung|0xb401;

	dist = GetTableInx(jung);

	//�ٽ� �������� �ϼ������� ��ȯ....�߼��� ����.
	middle[0]=0xb0+dist/94;
	middle[1]=0xa1+dist%94;
	middle[2]=0;
	strcpy(pMiddle, (char*)middle);

	//������ �и��س��� �κ�.
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

	//������ ����.
	tail[0]=(unsigned char)(last/256);
	tail[1]=(unsigned char)(last&0x00ff);
	tail[2]=0;
	strcpy(pTail, (char*)tail);
}

//���߼� + ������ ��� 
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


//�ʼ� + �߼�  + ������ ��� 
//���������� �����ڵ�� ������ ��, �����ڵ��� �ش��ϴ� ������ �ڵ��� ���������� �����Ͽ� �ϼ������� �����Ѵ�
void CConvert::EvaluateChoJungJongHapChar(char *OutData, char *chCho, char *chJung,char *chJong)
{
	int  dist = 0;
	unsigned int ChoCode = 0;
	unsigned int JungCode = 0;
	unsigned int JongCode = 0;
	unsigned int AccCode = 0;
	unsigned short kssm, u;
	unsigned short cho, jung, jong;
	//�����ڵ� �������� ���� ��ġ Ȯ��	
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
	//AccCode  = 0xAC00(�����ڵ� �ѱ� ������) + [�ʼ���ȣ] * 588 + [�߼���ȣ] * 28 + [������ȣ]
	/*
	AccCode = 0xAC00 + (28*21* ChoCode) + (28* JungCode) + JongCode;
	// unicode �ѱۿ��� 
	AccCode -= 0xac00;
	//�����ڵ��� �������� 
	jong = AccCode % 28;
	jung = (AccCode % (28 * 21)) / 28;
	cho = AccCode / (28 * 21);
	*/
	// make kssm code
	//�����ڵ��� ���������� �����Ǵ� ������ �������� �ڵ带 ã�´�
	cho = _u2j_cho[ChoCode];
	jung = _u2j_jung[JungCode];
	jong = _u2j_jong[JongCode];
	kssm = 0x8000 | (cho << 10) | (jung << 5) | (jong);
	

	//�ϼ��� �ڵ忡 �����Ǵ� ������ �ڵ尪�� ã�� �� ���� ������� �ϼ��� ������� �����Ѵ�.
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

//���� ���� �Ǵ�
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
���� ��� ���� - OR����(| )�� ����Ѵ�. (0x�� 16������� C������ ǥ�����)
�� : �� + �� + <ä��> : 0x8800 | 0x8060 | 0x8001 = 0x8861
�� : �� + �� + �� : 0x8800 | 0x8060 | 0x8002 = 0x8862
�� : �� + �� + �� : 0xD000 | 0x82A0 | 0x8005 = 0xD2A5

���� ��� ���� - AND����(&)���� ����ŷ �ϴ� ����� ����.
�� - �ʼ� : 0x8862 & 0xFC00 = 0x8800 : ��
�� - �߼� : 0x8862 & 0x83E0 = 0x8060 : ��
�� - ���� : 0x8862 & 0x801F = 0x8002 : ��

���ص� �ڵ�� �ܵ����� ����� �� ����, <ä��>�ڵ�� �ٽ� ���յǾ�� ��µȴ�.
�� : �� + <ä��>+<ä��> : 0x8800 | 0x8040 | 0x8001 = 0x8841
*/
