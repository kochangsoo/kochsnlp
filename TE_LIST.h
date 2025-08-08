#pragma once

#include <io.h>
#include "ds_common.h"
#include "Preprocessor.h"

#define TE_STEM_HEAD_SIZE (sizeof(4)*7+STEM_FEATURE_COUNT+16)
#define TE_END_HEAD_SIZE (sizeof(4)*8+END_FEATURE_COUNT+16)
#define TE_COMP_HEAD_SIZE (sizeof(4)*8+STEM_FEATURE_COUNT+16)
#define TE_TOKEN_HEAD_SIZE (sizeof(4)*6)
#define TE_SENTENCE_HEAD_SIZE (sizeof(4)*2)
#define TE_DATA_HEAD_SIZE (sizeof(4)*32)

#define SAVE_LOAD_SEPARATE 0x0a

enum _TYPE
{
	TYPE_TE_STEM = 1,
	TYPE_TE_END,
	TYPE_TE_COMP,
	TYPE_TE_TOKEN,
	TYPE_TE_SENTENCE,
	TYPE_TE_DATA,
	TYPE_TE_ALL
};

enum _IAM
{
	IAM_STEM,
	IAM_END
};

enum _PUMSA_STEM
{
	PUMSA_STEM_N = 1,//명사
	PUMSA_STEM_NUM,//수사
	PUMSA_STEM_PRO,//대명사
	PUMSA_STEM_V,//동사
	PUMSA_STEM_STA,//형용사
	PUMSA_STEM_UNC,//관형사
	PUMSA_STEM_ADV,//부사
	PUMSA_STEM_IC,//감탄사
	PUMSA_STEM_SYMBOL,//기호
	PUMSA_STEM_NUMB,//숫자
	PUMSA_STEM_FOREIGN,//외국어
	PUMSA_STEM_UNREGISTE//미등록어
};
enum _PUMSA_END
{
	PUMSA_END_N = 1,//조사
	PUMSA_END_V,//어미
	PUMSA_END_C//조사어미복합체
};

enum _VIEW_OUT
{
	VIEW_OUT_ALL,//모두출력
	VIEW_OUT_STEM,//어간형태만출력
	VIEW_OUT_END,//어미형태만출력

	VIEW_OUT_N,//명사
	VIEW_OUT_NUM,//수사
	VIEW_OUT_PRO,//대명사
	VIEW_OUT_V,//동사
	VIEW_OUT_STA,//형용사
	VIEW_OUT_UNC,//관형사
	VIEW_OUT_ADV,//부사
	VIEW_OUT_IC,//감탄사
	VIEW_OUT_SYMBOL,//기호
	VIEW_OUT_NUMB,//숫자
	VIEW_OUT_FOREIGN,//외국어
	VIEW_OUT_UNREGISTE,//미등록어

	VIEW_OUT_NEND,//조사
	VIEW_OUT_VEND,//어미
	VIEW_OUT_CEND//조사어미복합체
};
enum _VIEW_STATISTIC
{
	VIEW_STATISTIC_ALL, VIEW_STATISTIC_NORMAL, VIEW_STATISTIC_PUMSA, VIEW_STATISTIC_PLURAL
};

static char *pumsa_stem[] =
{
	" ","명사","수사","대명사","동사","형용사","관형사","부사","감탄사","기호","숫자","외국문자","미등록어"
};
static char *pumsa_end[] =
{
	" ","조사","어미","조사어미복합체"
};

//stem list
typedef struct _TE_STEM_TOKEN
{
	int _sentence_offset;//sentence_absolute_offset
	int _sentence_len;//sentence strlen()
	int _token_offset;
	int _pumsa;
	int _kind;//한글/한자/일어, 숫자/영문 구별 기호
	int _same_count;//중복된갯수(빈도수)
	BOOL _same_deputy;//중복이있을경우 정보를 표시할 대표(대리)자

	char _szfeature[STEM_FEATURE_COUNT + 16];//자질값
	char *_szresult;//문자열값

}TE_STEM_TOKEN;

//end list.
typedef struct _TE_END_TOKEN
{
	int _sentence_offset;//sentence_absolute_offset
	int _sentence_len;//sentence strlen()
	int _token_offset;
	int _pumsa;
	int _kind;//한글/한자/일어, 숫자/영문 구별 기호
	int _same_count;//중복된갯수(빈도수)
	BOOL _same_deputy;//중복이있을경우 정보를 표시할 대표(대리)자
	BOOL _iscomp;//복합어에서 분리된어미(조사,엔딩)

	char _szfeature[END_FEATURE_COUNT + 16];//자질값
	char *_szresult;//문자열값

}TE_END_TOKEN;

//복합명사 스템리스트.
typedef struct _TE_COMP_TOKEN
{
	int _sentence_offset;//sentence_absolute_offset
	int _sentence_len;//sentence strlen()
	int _token_offset;
	int _pumsa;
	int _kind;//한글/한자/일어, 숫자/영문 구별 기호.
	int _self_id;//0,1,2.....
	int _same_count;//중복된갯수(빈도수)
	BOOL _same_deputy;//중복이있을경우 정보를 표시할 대표(대리)자

	char _szfeature[STEM_FEATURE_COUNT + 16];//자질값
	char *_szresult;//문자열 값

	struct _TE_COMP_TOKEN *_prev;
	struct _TE_COMP_TOKEN *_next;
}TE_COMP_TOKEN;

// 토큰(단어)리스트.
typedef struct _TE_TOKEN
{
	int _sentence_offset;//sentence_absolute_offset
	int _sentence_len;//sentence strlen()
	int _token_offset;//token_absolute_offset
	int _same_count;//중복된갯수(빈도수)
	BOOL _same_deputy;//중복이있을경우 정보를 표시할 대표(대리)자
	BOOL _iscomp;//복합 명사 생성 플래그

	char *_sztoken;//token_word
	char *_szambend;////중의성이 발생한 엔딩 문자열 보관
	char *_szcomp_token;//_token에서 조사(어미)를 뺀 순수복합명사

	TE_STEM_TOKEN *_pstem;
	TE_END_TOKEN *_pend;

	struct _TE_TOKEN *_prev;
	struct _TE_TOKEN *_next;

}TE_TOKEN;

//문장정보(시작오프셋,길이)리스트.
typedef struct _TE_SENTENCE
{
	int _sentence_offset;//sentence_absolute_offset
	int _sentence_len;//sentence strlen()

	struct _TE_SENTENCE *_prev;
	struct _TE_SENTENCE *_next;
}TE_SENTENCE;

// 분석된 데이타의 전체적통계 및 문장,토큰,스템,엔딩리스트 정보 포함.
typedef struct _TE_DATA
{
	//스템,엔딩,복합어,문장,토큰(단어),
	int _cnt_sentence;//문장갯수
	int _cnt_token;//토큰(단어)갯수
	int _cnt_stem;//스템갯수
	int _cnt_end;//엔딩갯수
	int _cnt_comp;//복합어 갯수
	int _cnt_comp_stem;//복합어 스템갯수
	int _cnt_comp_end;//복합어 엔딩갯수
	int _cnt_ambend;////중의성이 발생한 엔딩 문자열 갯수

	int _cnt_same_stem;//중복된 스템갯수
	int _cnt_same_end;//중복된 엔딩갯수
	int _cnt_same_comp;//중복된 복합어 갯수

	int _cnt_corea_char;
	int _cnt_chinese_char;
	int _cnt_japan_char;
	int _cnt_english_char;
	int _cnt_number_char;
	int _cnt_etc_char;

	//stem
	int _cnt_noun; //명사갯수(자질:N)
	int _cnt_numeral;//수사(N && Num)
	int _cnt_pronoun;//대명사(N && Pro)
	int _cnt_actverb;//동사(V || Cmp)
	int _cnt_stateverb;//형용사((V && Sta) || (Cmp && Sta))
	int _cnt_adv;//부사(Adv)
	int _cnt_unc;//관형사(Unc)
	int _cnt_interjection;//감탄사(IC)
	int _cnt_numb;//숫자(Numb)
	int _cnt_foreign;//외국어(미정:부가기능=>전문용어)

	int _cnt_unregiste;//미등록명사

					   //ending
	int _cnt_nend;//조사갯수(자질:N)
	int _cnt_vend;//어미(V)
	int _cnt_cend;//조사어미복합체(N && V)

	int _self_id;//0,1,2.....

	TE_COMP_TOKEN *_pte_comp_head;//복합어구성 스템
	TE_COMP_TOKEN *_pte_comp_tail;//복합어구성 스템

	TE_TOKEN *_pte_token_head;//토큰(단어)
	TE_TOKEN *_pte_token_tail;//토큰(단어)

	TE_SENTENCE *_pte_sentence_head;//문장정보(시작idx,길이)
	TE_SENTENCE *_pte_sentence_tail;//문장정보(시작idx,길이)

}TE_DATA;
//자료구조 최종 관리리스트
class TE_LIST
{
public:
	TE_LIST();
	~TE_LIST();

private:
	TE_DATA *_pte_data;

public:
	TE_DATA *f_get_te_data();

public:
	bool f_init();//TE_DAAT *_pte_data 자료구조 초기화(메모리할당,내부멤버 리스트연결

public:
	bool f_add(void *node, int add_type);//sentence,token(stem,end)포함,comp리스트에 노드 추가

public:
	bool f_del(bool del_self);//TE_DATA 내의 모든메모리 사제 자신포함(선택).
	bool f_del(int del_type, bool del_head_tail);//TE_DATA내의 리스트(sentence,token,comp) 삭제
	bool f_del(TE_TOKEN *pte_token);//TE_TOKEN내의 문자열,스템(내부문자열포함),엔딩(내부문자열포함)삭제,<자신=노드>도 삭제
	bool f_del(TE_COMP_TOKEN *pte_comp);//TE_COMP내의 문자열삭제, <자신=노드>도 삭제

public:
	void f_migration(CPreProcessor *ppreprocessor);//형태소분석이끝난 데이타를 TE_DATA구조로 새롭게 구성
	void f_migration(SENTENCE *psentence);//문장 정보재취합
	void f_migration(H_TOKEN *ph_token, int sentence_offset, int sentence_len);//토큰정보 재취합

public:
	int f_pumsa_cnt(int iam_stem_end, int kind, char *feature);//품사별(스템,엔딩) 갯수 증가 도우미
	void f_kind_cnt(int kind);//종류
	void f_vindosu_statistic();//빈도수 통계

public:
	bool f_save(char *path);
	bool f_load(char *path);
};

