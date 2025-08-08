#pragma once
//
//ds_pretoken.h
//

#ifndef __DS_PRETOKEN_H__
#define __DS_PRETOKEN_H__

#include "ds_list.h"

#define PRE_FEATURE 234

#define HANGUL_CHAR_WORD   1// 한글
#define CHINESE_CHAR_WORD  2// 한자  
#define KATAGANA_CHAR_WORD 3// 가카가나 
#define HIRAGANA_CHAR_WORD 4// 히라가나
#define ENGLISH_CHAR_WORD  5// 영어 
#define NUMBER_CHAR_WORD   6// 숫자 
#define SINGLE_CHAR_WORD   7// 단일문장부호 
#define TWIN_CHAR_WORD     8// 결합쌍문장부호 
#define ASCII_CHAR_WORD    9// ASCII 코드 
#define SPECIAL_CHAR_WORD  10// 특수문자
//#define ROMA_CHAR_WORD  11// 로마문자

#define FIRST_CHAR 0
#define LAST_CHAR 1
#define TRUE_ASCII 0
#define FALSE_ASCII 1
#define CHAR_ASCII 0
#define CHAR_NOTASCII 1 

#define SYMBOL_CHAR_1  1 //'<' 
#define SYMBOL_CHAR_2  2 //'('
#define SYMBOL_CHAR_3  3 //'['
#define SYMBOL_CHAR_4  4 //'{'

#define SYMBOL_CHAR_5  5 //'>'
#define SYMBOL_CHAR_6  6 //')'
#define SYMBOL_CHAR_7  7 //']'
#define SYMBOL_CHAR_8  8 //'}'
#define SYMBOL_CHAR_9  9 //'\''
#define SYMBOL_CHAR_10  10 //'"'

#define SYMBOL_CHAR_11  11 //『
#define SYMBOL_CHAR_12  12 //「
#define SYMBOL_CHAR_13  13 //【
#define SYMBOL_CHAR_14  14 //』
#define SYMBOL_CHAR_15  15 //」
#define SYMBOL_CHAR_16  16 //】

#define PRENODE_CHAR_1 1 //' '
#define PRENODE_CHAR_2 2 //'\r','\n'
#define PRENODE_CHAR_3 3 //'?', '!', '.'
#define PRENODE_CHAR_4 4 //','
#define PRENODE_CHAR_5 5 //('?', '!', '.') + ('\r', '\n')
#define PRENODE_CHAR_6 6 //('\r', '\n' )*2

#define SYMBOL_TS 211	// 여는 따음표 자질
#define SYMBOL_TE 212	// 닫는 따옴표 자질
#define SYMBOL_NS 213
#define SYMBOL_NE 214
#define SYMBOL_COMMA 215
#define SYMBOL_CLAUSE 216

#define LONG_TOKEN 40

#define SYMBOL_MAX 3

typedef struct _PRE_TOKEN
{
	char* strOriginalSource;
	char pFeature[PRE_FEATURE + 1];
	int nOffset;
	int nEndCheck; //default:0, ' ':1, '\r'/'\n':2, '?'/'!'/'.'/:3 ',':4
	int nFirstParent;	//default :0
	int nLastParent;	//default :0
}PRE_TOKEN;

typedef struct CPreNode
{
	PRE_TOKEN* pPreToken;
	struct CPreNode* pNext;
	struct CPreNode* pPrev;
}PRE_NODE;

#endif
