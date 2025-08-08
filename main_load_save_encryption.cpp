//C++에서 fopen 함수 사용시 C4996 에러 #define _CRT_SECURE_NO_WARNINGS  가장 상단에 추가 또는 #include 아래에 추가 #pragma warning(disable:4996)
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

	// 암호화 되지 않는 파일을 로딩한다.
	dictionary->loadDictionary("All_Stem.txt");

	// 암호화하여 전부를 저장한다.
	dictionary->storeDictionary("All_Stem1.txt", 1);
	delete dictionary;

	// 따라서 두 파일은 같지 않다.

	dictionary= new CNAEncDictionary();

	// 암호화 된 파일을 로딩한다.
	dictionary->loadDictionary("All_Stem1.txt", 1);

	// 암호화 되지 않는 상태로 저장한다.
	dictionary->storeDictionary("All_Stem2.txt");

	// 따라서 All_Stem2와 All_Stem과는 정확히 같아야 한다.
	delete dictionary;
}