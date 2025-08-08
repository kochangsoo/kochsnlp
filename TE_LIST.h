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
	PUMSA_STEM_N = 1,//���
	PUMSA_STEM_NUM,//����
	PUMSA_STEM_PRO,//����
	PUMSA_STEM_V,//����
	PUMSA_STEM_STA,//�����
	PUMSA_STEM_UNC,//������
	PUMSA_STEM_ADV,//�λ�
	PUMSA_STEM_IC,//��ź��
	PUMSA_STEM_SYMBOL,//��ȣ
	PUMSA_STEM_NUMB,//����
	PUMSA_STEM_FOREIGN,//�ܱ���
	PUMSA_STEM_UNREGISTE//�̵�Ͼ�
};
enum _PUMSA_END
{
	PUMSA_END_N = 1,//����
	PUMSA_END_V,//���
	PUMSA_END_C//�����̺���ü
};

enum _VIEW_OUT
{
	VIEW_OUT_ALL,//������
	VIEW_OUT_STEM,//����¸����
	VIEW_OUT_END,//������¸����

	VIEW_OUT_N,//���
	VIEW_OUT_NUM,//����
	VIEW_OUT_PRO,//����
	VIEW_OUT_V,//����
	VIEW_OUT_STA,//�����
	VIEW_OUT_UNC,//������
	VIEW_OUT_ADV,//�λ�
	VIEW_OUT_IC,//��ź��
	VIEW_OUT_SYMBOL,//��ȣ
	VIEW_OUT_NUMB,//����
	VIEW_OUT_FOREIGN,//�ܱ���
	VIEW_OUT_UNREGISTE,//�̵�Ͼ�

	VIEW_OUT_NEND,//����
	VIEW_OUT_VEND,//���
	VIEW_OUT_CEND//�����̺���ü
};
enum _VIEW_STATISTIC
{
	VIEW_STATISTIC_ALL, VIEW_STATISTIC_NORMAL, VIEW_STATISTIC_PUMSA, VIEW_STATISTIC_PLURAL
};

static char *pumsa_stem[] =
{
	" ","���","����","����","����","�����","������","�λ�","��ź��","��ȣ","����","�ܱ�����","�̵�Ͼ�"
};
static char *pumsa_end[] =
{
	" ","����","���","�����̺���ü"
};

//stem list
typedef struct _TE_STEM_TOKEN
{
	int _sentence_offset;//sentence_absolute_offset
	int _sentence_len;//sentence strlen()
	int _token_offset;
	int _pumsa;
	int _kind;//�ѱ�/����/�Ͼ�, ����/���� ���� ��ȣ
	int _same_count;//�ߺ��Ȱ���(�󵵼�)
	BOOL _same_deputy;//�ߺ���������� ������ ǥ���� ��ǥ(�븮)��

	char _szfeature[STEM_FEATURE_COUNT + 16];//������
	char *_szresult;//���ڿ���

}TE_STEM_TOKEN;

//end list.
typedef struct _TE_END_TOKEN
{
	int _sentence_offset;//sentence_absolute_offset
	int _sentence_len;//sentence strlen()
	int _token_offset;
	int _pumsa;
	int _kind;//�ѱ�/����/�Ͼ�, ����/���� ���� ��ȣ
	int _same_count;//�ߺ��Ȱ���(�󵵼�)
	BOOL _same_deputy;//�ߺ���������� ������ ǥ���� ��ǥ(�븮)��
	BOOL _iscomp;//���վ�� �и��Ⱦ��(����,����)

	char _szfeature[END_FEATURE_COUNT + 16];//������
	char *_szresult;//���ڿ���

}TE_END_TOKEN;

//���ո�� ���۸���Ʈ.
typedef struct _TE_COMP_TOKEN
{
	int _sentence_offset;//sentence_absolute_offset
	int _sentence_len;//sentence strlen()
	int _token_offset;
	int _pumsa;
	int _kind;//�ѱ�/����/�Ͼ�, ����/���� ���� ��ȣ.
	int _self_id;//0,1,2.....
	int _same_count;//�ߺ��Ȱ���(�󵵼�)
	BOOL _same_deputy;//�ߺ���������� ������ ǥ���� ��ǥ(�븮)��

	char _szfeature[STEM_FEATURE_COUNT + 16];//������
	char *_szresult;//���ڿ� ��

	struct _TE_COMP_TOKEN *_prev;
	struct _TE_COMP_TOKEN *_next;
}TE_COMP_TOKEN;

// ��ū(�ܾ�)����Ʈ.
typedef struct _TE_TOKEN
{
	int _sentence_offset;//sentence_absolute_offset
	int _sentence_len;//sentence strlen()
	int _token_offset;//token_absolute_offset
	int _same_count;//�ߺ��Ȱ���(�󵵼�)
	BOOL _same_deputy;//�ߺ���������� ������ ǥ���� ��ǥ(�븮)��
	BOOL _iscomp;//���� ��� ���� �÷���

	char *_sztoken;//token_word
	char *_szambend;////���Ǽ��� �߻��� ���� ���ڿ� ����
	char *_szcomp_token;//_token���� ����(���)�� �� �������ո��

	TE_STEM_TOKEN *_pstem;
	TE_END_TOKEN *_pend;

	struct _TE_TOKEN *_prev;
	struct _TE_TOKEN *_next;

}TE_TOKEN;

//��������(���ۿ�����,����)����Ʈ.
typedef struct _TE_SENTENCE
{
	int _sentence_offset;//sentence_absolute_offset
	int _sentence_len;//sentence strlen()

	struct _TE_SENTENCE *_prev;
	struct _TE_SENTENCE *_next;
}TE_SENTENCE;

// �м��� ����Ÿ�� ��ü����� �� ����,��ū,����,��������Ʈ ���� ����.
typedef struct _TE_DATA
{
	//����,����,���վ�,����,��ū(�ܾ�),
	int _cnt_sentence;//���尹��
	int _cnt_token;//��ū(�ܾ�)����
	int _cnt_stem;//���۰���
	int _cnt_end;//��������
	int _cnt_comp;//���վ� ����
	int _cnt_comp_stem;//���վ� ���۰���
	int _cnt_comp_end;//���վ� ��������
	int _cnt_ambend;////���Ǽ��� �߻��� ���� ���ڿ� ����

	int _cnt_same_stem;//�ߺ��� ���۰���
	int _cnt_same_end;//�ߺ��� ��������
	int _cnt_same_comp;//�ߺ��� ���վ� ����

	int _cnt_corea_char;
	int _cnt_chinese_char;
	int _cnt_japan_char;
	int _cnt_english_char;
	int _cnt_number_char;
	int _cnt_etc_char;

	//stem
	int _cnt_noun; //��簹��(����:N)
	int _cnt_numeral;//����(N && Num)
	int _cnt_pronoun;//����(N && Pro)
	int _cnt_actverb;//����(V || Cmp)
	int _cnt_stateverb;//�����((V && Sta) || (Cmp && Sta))
	int _cnt_adv;//�λ�(Adv)
	int _cnt_unc;//������(Unc)
	int _cnt_interjection;//��ź��(IC)
	int _cnt_numb;//����(Numb)
	int _cnt_foreign;//�ܱ���(����:�ΰ����=>�������)

	int _cnt_unregiste;//�̵�ϸ��

					   //ending
	int _cnt_nend;//���簹��(����:N)
	int _cnt_vend;//���(V)
	int _cnt_cend;//�����̺���ü(N && V)

	int _self_id;//0,1,2.....

	TE_COMP_TOKEN *_pte_comp_head;//���վ�� ����
	TE_COMP_TOKEN *_pte_comp_tail;//���վ�� ����

	TE_TOKEN *_pte_token_head;//��ū(�ܾ�)
	TE_TOKEN *_pte_token_tail;//��ū(�ܾ�)

	TE_SENTENCE *_pte_sentence_head;//��������(����idx,����)
	TE_SENTENCE *_pte_sentence_tail;//��������(����idx,����)

}TE_DATA;
//�ڷᱸ�� ���� ��������Ʈ
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
	bool f_init();//TE_DAAT *_pte_data �ڷᱸ�� �ʱ�ȭ(�޸��Ҵ�,���θ�� ����Ʈ����

public:
	bool f_add(void *node, int add_type);//sentence,token(stem,end)����,comp����Ʈ�� ��� �߰�

public:
	bool f_del(bool del_self);//TE_DATA ���� ���޸� ���� �ڽ�����(����).
	bool f_del(int del_type, bool del_head_tail);//TE_DATA���� ����Ʈ(sentence,token,comp) ����
	bool f_del(TE_TOKEN *pte_token);//TE_TOKEN���� ���ڿ�,����(���ι��ڿ�����),����(���ι��ڿ�����)����,<�ڽ�=���>�� ����
	bool f_del(TE_COMP_TOKEN *pte_comp);//TE_COMP���� ���ڿ�����, <�ڽ�=���>�� ����

public:
	void f_migration(CPreProcessor *ppreprocessor);//���¼Һм��̳��� ����Ÿ�� TE_DATA������ ���Ӱ� ����
	void f_migration(SENTENCE *psentence);//���� ����������
	void f_migration(H_TOKEN *ph_token, int sentence_offset, int sentence_len);//��ū���� ������

public:
	int f_pumsa_cnt(int iam_stem_end, int kind, char *feature);//ǰ�纰(����,����) ���� ���� �����
	void f_kind_cnt(int kind);//����
	void f_vindosu_statistic();//�󵵼� ���

public:
	bool f_save(char *path);
	bool f_load(char *path);
};

