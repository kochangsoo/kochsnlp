//C++���� fopen �Լ� ���� C4996 ���� #define _CRT_SECURE_NO_WARNINGS  ���� ��ܿ� �߰� �Ǵ� #include �Ʒ��� �߰� #pragma warning(disable:4996)
#define _CRT_SECURE_NO_WARNINGS 

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	"naobject.h"
#include	"naentry.h"
#include	"nalink.h"
#include	"nahashnode.h"
#include	"nahashtree.h"
#include	"nadictionary.h"
#include	"naencdictionary.h"

void main_()
{
	CNAEncDictionary	*dictionary= new CNAEncDictionary();

	// ���� �ε��Ѵ�.
	dictionary->loadDictionary("All_Stem.txt");

	// ���� �����Ѵ�.
	dictionary->storeDictionary("All_Stem1.txt");
	delete dictionary;

	// ���� �� ������ ��Ȯ�� ���ƾ� �Ѵ�.
}