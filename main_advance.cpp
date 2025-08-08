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

void main_()
{
	int	i;

	CNAEntry		*entry;
	CNADictionary	*dictionary= new CNADictionary();

	// ���� �ε��Ѵ�.
	dictionary->loadDictionary("All_Stem.txt");

	printf("starting binary search and hash search\n");
	// ���������� ��� ��Ʈ���� ���Ͽ�
	// ����Ž���� �ؽ�Ž���� �Ͽ� ������ ���� ã�������� ��
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

	// Ű�� ���� ������ �Ͽ� �ٽ��ѹ� �׽�Ʈ
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


	// ��� Ű�� �����Ͽ� Ű�� �����Ѵ�.
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

	// ������ Ű�� ���Ͽ� �˻�
	// ���� ���� Ű �̹Ƿ� ��� Ž���� NULL�� �Ǿ�� ��
	printf("starting binary search and hash search with failure\n");

	// Ű�� ���� ������ �Ͽ� �ٽ��ѹ� �׽�Ʈ
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


	// ������ ��� Ű�� ���ο� ��Ʈ���� ����
	char	buf[1024];
	for(i=0;i<index;i++){
		CNAEntry	*newEntry= new CNAEntry();
		sprintf(buf, "\"%s\"	0	1	0	1\n", keys[i]);
		newEntry->setString(buf);

		dictionary->addEntry(newEntry);
	}

	// ������ Ű�� �˻�
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

	// ������ Ű�� ���θ� �����.
	for(i=0;i<index;i++){
		char	*key= keys[i];

		CNAEntry	*bentry= dictionary->searchEntryBs(key);
		if(bentry != NULL){
			dictionary->removeEntry(bentry);
			delete bentry;
		}
	}

	// �ٽ� ������ Ű�� ���� �����Ѵ�.
	for(i=0;i<index;i++){
		CNAEntry	*newEntry= new CNAEntry();
		sprintf(buf, "\"%s\"	0	1	0	1\n", keys[i]);
		newEntry->setString(buf);

		dictionary->addEntry(newEntry);
	}

	printf("starting binary search and hash search with modified keys again\n");
	// �ٽ� �˻��Ѵ�.
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
	// �ٽ� �˻��Ѵ�.
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
