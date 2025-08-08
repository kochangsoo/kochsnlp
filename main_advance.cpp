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

void main_()
{
	int	i;

	CNAEntry		*entry;
	CNADictionary	*dictionary= new CNADictionary();

	// 전부 로딩한다.
	dictionary->loadDictionary("All_Stem.txt");

	printf("starting binary search and hash search\n");
	// 순차적으로 모든 엔트리에 대하여
	// 이진탐색과 해쉬탐색을 하여 동일한 것이 찾아졌는지 비교
	dictionary->getFirstEntry();
	while((entry=dictionary->getNextEntry())!=NULL){
		char	*key= entry->getKey();
		CNAEntry	*bentry= dictionary->searchEntryBs(key);
		CNAEntry	*hentry= dictionary->searchEntryHs(key);
		if(bentry != hentry){
			if(bentry != NULL)	bentry->print();
			if(hentry != NULL)	hentry->print();
			printf("erro\n");
			exit(0);

		}
	}
	printf("OK: binary search and hash search\n");
	
	printf("starting binary search and hash search again\n");

	// 키의 값을 역으로 하여 다시한번 테스트
	dictionary->getLastEntry();
	while((entry=dictionary->getPrevEntry())!=NULL){
		char	*key= entry->getKey();
		CNAEntry	*bentry= dictionary->searchEntryBs(key);
		CNAEntry	*hentry= dictionary->searchEntryHs(key);
		if(bentry != hentry){
			if(bentry != NULL)	bentry->print();
			if(hentry != NULL)	hentry->print();
			printf("erro\n");
			exit(0);

		}
	}
	printf("OK: binary search and hash search again\n");


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

	// 변조된 키에 대하여 검색
	// 따라서 없는 키 이므로 모든 탐색은 NULL이 되어야 함
	printf("starting binary search and hash search with failure\n");

	// 키의 값을 역으로 하여 다시한번 테스트
	for(i=0;i<index;i++){
		char	*key= keys[i];
		CNAEntry	*bentry= dictionary->searchEntryBs(key);
		CNAEntry	*hentry= dictionary->searchEntryHs(key);
		if(bentry != NULL){
			printf("error in BS\n");
			exit(0);
		}
		if(hentry != NULL){
			printf("error in HS\n");
			exit(0);
		}
	}
	printf("OK: binary search and hash search with failure\n");


	// 변조된 모든 키를 새로운 엔트리로 삽입
	char	buf[1024];
	for(i=0;i<index;i++){
		CNAEntry	*newEntry= new CNAEntry();
		sprintf(buf, "\"%s\"	0	1	0	1\n", keys[i]);
		newEntry->setString(buf);

		dictionary->addEntry(newEntry);
	}

	// 변조된 키만 검색
	printf("starting binary search and hash search with modified keys\n");
	for(i=0;i<index;i++){
		char	*key= keys[i];

		CNAEntry	*bentry= dictionary->searchEntryBs(key);
		CNAEntry	*hentry= dictionary->searchEntryHs(key);
		if(bentry != hentry){
			printf("%s\n", key);
			if(bentry != NULL)	bentry->print();
			if(hentry != NULL)	hentry->print();
			printf("erro\n");
			exit(0);

		}
	}
	printf("OK: binary search and hash search with modified keys\n");

	// 변조된 키를 전부를 지운다.
	for(i=0;i<index;i++){
		char	*key= keys[i];

		CNAEntry	*bentry= dictionary->searchEntryBs(key);
		if(bentry != NULL){
			dictionary->removeEntry(bentry);
			delete bentry;
		}
	}

	// 다시 변조된 키를 전부 삽입한다.
	for(i=0;i<index;i++){
		CNAEntry	*newEntry= new CNAEntry();
		sprintf(buf, "\"%s\"	0	1	0	1\n", keys[i]);
		newEntry->setString(buf);

		dictionary->addEntry(newEntry);
	}

	printf("starting binary search and hash search with modified keys again\n");
	// 다시 검색한다.
	for(i=0;i<index;i++){
		char	*key= keys[i];

		CNAEntry	*bentry= dictionary->searchEntryBs(key);
		CNAEntry	*hentry= dictionary->searchEntryHs(key);
		if(bentry != hentry){
			printf("%s\n", key);
			if(bentry != NULL)	bentry->print();
			if(hentry != NULL)	hentry->print();
			printf("erro\n");
			exit(0);

		}
	}
	printf("OK: binary search and hash search with modified keys again\n");

	dictionary->refineDictionary();

	printf("starting binary search and hash search with refinement\n");
	// 다시 검색한다.
	for(i=0;i<index;i++){
		char	*key= keys[i];
		CNAEntry	*bentry= dictionary->searchEntryBs(key);
		CNAEntry	*hentry= dictionary->searchEntryHs(key);
		if(bentry != hentry){
			printf("%s\n", key);
			if(bentry != NULL)	bentry->print();
			if(hentry != NULL)	hentry->print();
			printf("erro\n");
			exit(0);

		}
	}
	printf("OK: binary search and hash search with refinement\n");

	delete dictionary;
}
