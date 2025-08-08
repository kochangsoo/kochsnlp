#pragma once
//
//ds_list.h
//

#ifndef __DS_LIST_H__
#define __DS_LIST_H__


////////////////////////////////
//�����м� 
////////////////////////////////

//Blocking of Information
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>


#define S_E_V 0 //Setenece Start - End Flag Value
#define S_P_B 1 //Start_P_Bloking
#define E_P_B 2 //End_P_Blocking
#define S_N_B 3
#define E_N_B 4
#define S_G_B 5
#define E_G_B 6
#define S_O_B 7
#define E_O_B 8
#define S_C_B 9
#define E_C_B 10
#define S_S_B 11
#define E_S_B 12

//���� ���̺�
#define SUBJ_VOC 0 //ȣ��
#define SUBJ_CONJ 1 //��
#define SUBJ_PA 2 //�λ�
#define SUBJ_S 3 //�־�
#define SUBJ_P 4 //�μ�
#define SUBJ_SD 5 //�μ�
#define SUBJ_IO 6 //����
#define SUBJ_DO 7 //������
#define SUBJ_P1 8 //�μ�
#define SUBJ_C 9 //����
#define SUBJ_V 10 //���� 
#define SUBJ_AUX 11 //������ 

//���� ���̺� 
#define MODE_HON 0 //��Ī 
#define MODE_PRS 1 //����(����) 
#define MODE_PST 2
#define MODE_FUT 3
#define MODE_RE 4 //��(����)
#define MODE_IG 5
#define MODE_PP 6
#define MODE_NEG 7 //����
#define MODE_DCL 8 //��������(����) 
#define MODE_WHQ 9
#define MODE_YQ 10
#define MODE_IMPR 11
#define MODE_PPO 12
#define MODE_EXCL 13


//Sentence of Information 
#define F_N 0 //Flag_Start
#define F_S 1 //Flag_Start
#define F_E 2 //Flag_End

#define B_NN 0 //is Not Block
#define B_NB 1 //N-Block
#define B_OB 2 //O-Block
#define B_GB 3 //G-Block

#define TABLE_NOTCHECK 0 //Table Not Check
#define TABLE_CHECK    1    //Table Check

#define MOVE_L 0 //Left Move
#define MOVE_R 1 //Right Move

#define DISTICT_ONE 0 //One(N) Only Block Info Search
#define DISTICT_ALL 1 //All Block Info Search

////////////////////////////////
//���ü� �м� 
////////////////////////////////

//define Token Word Divide 
#define WORD_SPACE ' '
#define WORD_CARRAGERETURN_1 '\r'
#define WORD_CARRAGERETURN_2 '\n'

#define CHAR_CHOJUNGSUNG 1//���߼� �ڵ�
#define CHAR_JONGSUNG    2//���� �ڵ�

#define CHANGE_CHAR_SYLLABLE_FIRST 1//ó�� ���� ����
#define CHANGE_CHAR_SYLLABLE_LAST    2//������ ���� ����

#define HANGUL_CHAR_WORD   1// �ѱ�
#define CHINESE_CHAR_WORD  2// ����  
#define KATAGANA_CHAR_WORD 3// ��ī���� 
#define HIRAGANA_CHAR_WORD 4// ���󰡳�
#define ENGLISH_CHAR_WORD  5// ���� 
#define NUMBER_CHAR_WORD   6// ���� 
#define SINGLE_CHAR_WORD   7// ���Ϲ����ȣ 
#define TWIN_CHAR_WORD     8// ���սֹ����ȣ 
#define ASCII_CHAR_WORD    9// ASCII �ڵ� 
#define SPECIAL_CHAR_WORD  10// Ư������

#define SYMBOL_CHAR_1  1 //'<' 
#define SYMBOL_CHAR_2  2 //'('
#define SYMBOL_CHAR_3  3 //'['
#define SYMBOL_CHAR_4  4 //'>'
#define SYMBOL_CHAR_5  5 //')'
#define SYMBOL_CHAR_6  6 //']'
#define SYMBOL_CHAR_7  7 //'\''
#define SYMBOL_CHAR_8  8 //'"'

//��� ��ɾ�
#define CLASS_DELETE(X)	if(X != NULL) { delete X; X = NULL; }
#define CHAR_DELETE(X)	if(X != NULL) { delete[] X; X = NULL; }

//�ڷᱸ��
typedef struct _SUB_TOKEN //���¼� �м� ���� �/��� ���� �� ���� ���� 
{
	//pSend(0) PBlock(1-2) NBlock(3-4) GBlock(5-6) OBlock(7-8) CBlock(9-10) SBlock(11-12)
	//unsigned short BlockInfo[13];//��ŷ ����
	int nKindChar;//�ѱ�/����/�Ͼ�, ����/���� ���� ��ȣ
	char* strResult;//���ڿ� �� 
	char* strFeature;//������ 
	//0�� ������ ��ġ������ ���� ������
	int	nFindPos;//��ü �������� ������ġ � �Ǵ� ����� �߰���ġ(��� ã�� ���ڿ��� ����ġ, ��̴� ã�� ���ڿ��� ������ġ)
}SUB_TOKEN;

typedef struct _COMP_SUB_TOKEN
{
	int nKindChar;//�ѱ�/����/�Ͼ�, ����/���� ���� ��ȣ
	char* strResult;//���ڿ� 
	char* strFeature;//������ 
}COMP_SUB_TOKEN;

//���� ��� ǥ�� ����ü
typedef struct CSubNode
{
	COMP_SUB_TOKEN* pCSubToken;
	struct CSubNode* pNext;
	struct CSubNode* pPrev;
}H_SUBNODE;

typedef struct _HANGUL_TOKEN //��ü ���� 
{
	char* strOriginalSource;
	char* strSource;//�����ڿ� ���� ����
	char* strAmbEndingSource;//���Ǽ��� �߻��� ���� ���ڿ� ����
	int nOffset;

	SUB_TOKEN* subStem;//���� ��
	SUB_TOKEN* subEnding;//���� �� 

	bool bSubCompCreate;//���� ��� ���� �÷���
	H_SUBNODE* pSubHeadNode;
	H_SUBNODE* pSubTailNode;

	//2017-09-28 �ű� �߰�
	bool bSubEndingCompCreate;//���� ��� ���� �÷���
	H_SUBNODE* pSubEndingHeadNode;
	H_SUBNODE* pSubEndingTailNode;
	
	//2017-08-31
	#ifdef _DEBUG
	char strFlowChartDebug[1024];//���¼� �м� �˰��� �м� �帧��
	#endif

	//2017-10-01
	#ifdef _DEBUG
	char strAnalysisError[1024];//�м�����
	#endif

	//2017-09-28 �ű� �߰�
	int strSourceLen;//�����ڿ��� ����
	int nLastFindEndingSyllable;//A~B������ ������ �����ϴ� ����� ���� ������ ��ġ
	int nLastFindCompoundNoun;//���ո��� �м��� ������ ������ ��ġ(��ũ�帮��Ʈ�� ���� ù ��ġ)

	int nKindError;//0     : ���� ����(�м�����) -999 : �м�����
	
}H_TOKEN;

typedef struct CNode
{
	//���� ������ �ѹ� �м� �Ǿ����� �ǹ���!!
	bool bMarsk;
	int nSkip;
	H_TOKEN* pHToken;
	struct CNode* pNext;
	struct CNode* pPrev;
}H_NODE;


typedef struct _SENTENCE
{
	struct CNode* pHead;	//���� ���� ��ġ
	struct CNode* pTail;	//���� �� ��ġ
	char szSentence[8192 * 5];
	int nOffset;

	struct _SENTENCE* pNext;
	struct _SENTENCE* pPrev;
}SENTENCE;

typedef struct _DOC
{
	struct _SENTENCE* sentence;	//���� ù��° ��ġ

	char szFilename[256];
	char szDocID[256];
	char szTitle[4096];
	char szAuthor[256];
	char szDate[64];

	time_t nDate;
	int nCount;	// ������ ��ü ��
}DOC;



typedef struct _HANGUL_ANALY_RESULT //��ü ����(���¼Һм���� �ӽû��) 
{
	char strSource[50];//�����ڿ� ���� ����
	int nRefer;//���
	int bVerb;//���翩��

}H_ANALY_RESULT;

#endif
