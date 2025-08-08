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

	CNAEntry		*entry= new CNAEntry();
	CNADictionary	*dictionary= new CNADictionary();
	CNADictionary	*dictionary1= new CNADictionary();

	dictionary->loadDictionary("testdata.txt");

	// ����
	entry->setString("\"���繮\" 1 1 1 1");
	dictionary->addEntry(entry);

	// ����2
	entry= new CNAEntry();
	entry->setString("\"���繮\" 1 000");
	dictionary->addEntry(entry);

	dictionary->getPrevEntry();
	entry= dictionary->getEntry();
	entry->print();
	entry= dictionary->removeEntry();
	dictionary->addEntry(entry);
	
	int	flag;
	entry= dictionary->searchEntryBs("B", &flag);
	entry->print();		// B ���

	// B�� �����ϴ� ��Ʈ���� ����� ����
	dictionary->setEntry(entry);
	entry= dictionary->getEntry();
	entry->print();	// ���� B�� ���


	// B�� �����ϴ� ���� 7���ε� ��� ����
	CNAEntry	*bufEntry[7];
	for(i=0;i<7;i++){
		bufEntry[i]= dictionary->removeEntry();
	}

	// ���� �� ���Ͽ��� B*�� ��� ����� ��
	dictionary->storeDictionary("testdataExceptB.txt");

	// �ٽ� ������ 7���� ����
	for(i=0;i<7;i++){
		dictionary->addEntry(bufEntry[i]);
	}
	// ���� �� ������ ������ �����Ͽ��� ��
	dictionary->storeDictionary("testdataExceptBB.txt");

	// �̰��� �޸𸮸� ���Ѵ�. ���� ���� ����� ��ʺ��� �����Ͽ��� ��
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