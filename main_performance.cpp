//C++에서 fopen 함수 사용시 C4996 에러 #define _CRT_SECURE_NO_WARNINGS  가장 상단에 추가 또는 #include 아래에 추가 #pragma warning(disable:4996)
#define _CRT_SECURE_NO_WARNINGS 

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<windows.h>
#include	<winbase.h>
#include	"naobject.h"
#include	"naentry.h"
#include	"nalink.h"
#include	"nahashnode.h"
#include	"nahashtree.h"
#include	"nadictionary.h"
#include <conio.h>

void main_()
{
	CNAEntry		*entry;
	CNADictionary	*dictionary= new CNADictionary();

	// 전부 로딩한다.
	
	int n = 0;
	printf("Loading:0 / Dic Loading:1 / Search:2 / Dic Search:3 = ");
	scanf("%d", &n);

	DWORD	stime=GetTickCount();
	if(n == 1 || n == 3)
	{
		printf("test.txt Loding...\n");
		dictionary->DicLoadDictionary("test.txt");
	}
	else{
		printf("All_Stem.txt Loding...\n");
	//	dictionary->loadDictionary("사본-All_Stem.txt");
		dictionary->loadDictionary("All_Stem.txt");
	}
	DWORD	etime=GetTickCount();
	printf("total time of Load= %d\n", etime-stime);

	/*
	CNAEntry* addCn = new CNAEntry();
	addCn->setString("\"2002\"	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0");
	dictionary->addEntry(addCn);
	*/
	
	if(n == 2 || n == 3)
	{
		while(1)
		{
			char* s = new char[30];
			printf("검색단어(종료:q): ");
			scanf("%s", s);
			
			if(strcmp(s, "q") == 0)
			{
				printf("종료합니다.\n");
				break;
			}
			CNAEntry* cn = NULL;
			cn = dictionary->searchEntryHs(s);
			if(cn != NULL)
			{
				printf("EntryHashTree = ");
				cn->print();
				cn = NULL;
			}
			cn = dictionary->searchEntryBs(s);
			if(cn != NULL)
			{
				printf("EntryLinkedBS = ");
				cn->print();
				//cn = NULL;
			}
			delete[] s;
			s = NULL;
			int z = 0;
			printf("삭제하겠습니까? (0:1) : ");
			scanf("%d", &z);
			if(z == 1)
				dictionary->removeEntry(cn);
		}
	}
	else
	{
	// 모든 키를 추출하여 키를 변조한다.
	int	numberOfEntry=0;
	dictionary->getFirstEntry();
	while((entry=dictionary->getNextEntry())!=NULL){
		numberOfEntry++;
	}

	
	int	index=0;
	char	**keys= new char*[numberOfEntry];
	dictionary->getFirstEntry();
	while((entry=dictionary->getNextEntry())!=NULL){
		int	keyLen= entry->getKeyLen()+100;
		keys[index]= new char[keyLen];
		entry->getKey(keys[index]);
		strcat(keys[index],"            A");
		index++;
	}
	
	/// finding case
	printf("number of keys=%d\n", index);
	int i;
	/*DWORD*/	
	stime=GetTickCount();
	for(i=0;i<index;i++){
		dictionary->searchEntryBs(keys[i]);	
	}
	/*DWORD*/	
	etime=GetTickCount();

	printf("total time of BS= %d\n", etime-stime);

	stime=GetTickCount();
	for(i=0;i<index;i++){
		dictionary->searchEntryHs(keys[i]);	
	}
	etime=GetTickCount();

	printf("total time of HS= %d\n", etime-stime);

	for(i=0;i<index;i++){
		char *key= keys[i];
		for(int j=0;j < (int)strlen(key);j++)
			key[j]++;
	}

	// not finding case
	printf("number of keys=%d\n", index);
	stime=GetTickCount();
	for(i=0;i<index;i++){
		dictionary->searchEntryBs(keys[i]);	
	}
	etime=GetTickCount();

	printf("total time of BS= %d\n", etime-stime);

	stime=GetTickCount();
	for(i=0;i<index;i++){
		dictionary->searchEntryHs(keys[i]);	
	}
	etime=GetTickCount();

	printf("total time of HS= %d\n", etime-stime); 

	}

	printf("Dic 파일로 저장하겠습니까?(N:0/Y:1) : ");
	int l = 0;
	scanf("%d", &l);
	if(l == 1)
	{
	stime=GetTickCount();
	printf("test.txt Saving...\n");
	if(dictionary->DicStoreDictionary("test1234.txt") == 0)// store a dictionary to a given file
		printf("파일저장 성공\n");
	//dictionary->refineDictionary();				// reconstruct a dictionary
	etime=GetTickCount();
	printf("total time of Save= %d\n", etime-stime);
	}
	
	delete dictionary;
	//getch();
}
