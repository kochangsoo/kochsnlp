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

	CNAEntry		*entry= new CNAEntry();
	CNADictionary	*dictionary= new CNADictionary();
	CNADictionary	*dictionary1= new CNADictionary();

	dictionary->loadDictionary("testdata.txt");

	// 삽입
	entry->setString("\"이재문\" 1 1 1 1");
	dictionary->addEntry(entry);

	// 삽입2
	entry= new CNAEntry();
	entry->setString("\"이재문\" 1 000");
	dictionary->addEntry(entry);

	dictionary->getPrevEntry();
	entry= dictionary->getEntry();
	entry->print();
	entry= dictionary->removeEntry();
	dictionary->addEntry(entry);
	
	int	flag;
	entry= dictionary->searchEntryBs("B", &flag);
	entry->print();		// B 출력

	// B로 시작하는 엔트리를 현재로 설정
	dictionary->setEntry(entry);
	entry= dictionary->getEntry();
	entry->print();	// 따라서 B가 출력


	// B로 시작하는 것이 7개인데 모두 삭제
	CNAEntry	*bufEntry[7];
	for(i=0;i<7;i++){
		bufEntry[i]= dictionary->removeEntry();
	}

	// 따라서 이 파일에는 B*는 모두 없어야 함
	dictionary->storeDictionary("testdataExceptB.txt");

	// 다시 삭제된 7개를 삽입
	for(i=0;i<7;i++){
		dictionary->addEntry(bufEntry[i]);
	}
	// 따라서 이 파일은 원본과 동일하여야 함
	dictionary->storeDictionary("testdataExceptBB.txt");

	// 이것은 메모리를 정한다. 따라서 다음 결과는 우너본과 동일하여야 함
	dictionary->refineDictionary();
	dictionary->storeDictionary("testdataExceptBBB.txt");

	
	dictionary->getFirstEntry();
	while(1){
		entry= dictionary->removeEntry();
		if(entry == NULL)	break;
		dictionary1->addEntry(entry);
	}
	dictionary1->storeDictionary("testdata1.txt");
	dictionary->storeDictionary("testdata2.txt");

}