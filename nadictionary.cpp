#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	"nautil.h"
#include	"naobject.h"
#include	"naentry.h"
#include	"nalink.h"
#include	"nahashnode.h"
#include	"nahashtree.h"
#include	"nadictionary.h"

//visual studio 2005 이상부터 'scanf', fopen' , 'strcpy' 등을 사용하면  Warning C4996 경고가 발생하게 된다. 아래 추가하여 해결.
#pragma warning(disable:4996)

CNADictionary::CNADictionary(char *fname)
{
	// initialize
	m_firstLink = NULL;
	m_lastLink = NULL;
	m_currentLink = NULL;


	m_linkArray = NULL;
	m_linkFree = NULL;

	m_hashTree = NULL;


	m_loadingComplete = 0;

	// load dictionary
	if (fname != NULL)
		loadDictionary(fname, 1, 0);



}
CNADictionary::~CNADictionary()
{
	if (m_hashTree != NULL)	delete m_hashTree;
	CNALink	*t = m_firstLink;
	while (t != NULL) {
		CNAEntry* cn = NULL;
		cn = t->m_element;
		t = t->m_next;
		delete cn;
	}

	while (m_linkFree != NULL) {
		t = m_linkFree;
		m_linkFree = t->m_next;
		delete t;
	}

	CNALink	*s = m_firstLink;
	while (s != NULL) {
		t = s;
		s = s->m_next;
		if (t->m_type == LINKTYPE_SINGLE)
			delete t;
	}

	if (m_linkArray != NULL)
		delete[] m_linkArray;
}

void	CNADictionary::cleanRoom()
{
	m_firstLink = NULL;
	m_lastLink = NULL;
	m_currentLink = NULL;

	// initialize memory
	if (m_linkArray != NULL)	delete[] m_linkArray;
	while (m_linkFree != NULL) {
		CNALink	*t = m_linkFree;
		m_linkFree = t->m_next;
		delete t;
	}
	m_linkArray = NULL;
	m_linkFree = NULL;
}

/*
this function is divided into two parts
The first is to sort
The second is to construct a duoble linked list
*/
int	CNADictionary::buildDictionary(CNAEntry *entryArray[], int numberOfEntry)
{
	int	i, j;

	// if two entry have the same key, we merge both
	int	lastEntry = 0;
	for (i = 1; i<numberOfEntry; i++) {
		if (CNAEntry::compare((void*)&entryArray[lastEntry], (void*)&entryArray[i]) == 0) {
			// this is case which two entry have the same key
			char	*bitVector = entryArray[i]->getBitVector(0);	// get bitVector
			entryArray[i]->removeBitVector(0);					// for destruction of entryArray[i]
			entryArray[lastEntry]->addBitVector(bitVector);		// in this case, entryArray[lastEntry] has multiple-bitVector
			delete entryArray[i];	// need no more
			continue;
		}
		entryArray[++lastEntry] = entryArray[i];
	}
	m_sizeArray = lastEntry + 1;


	cleanRoom();

	// make a double linked list
	// allocate memory for constructing a double linked list
	m_linkArray = new CNALink[m_sizeArray];

	// make linking with bidirection
	for (i = 0, j = m_sizeArray - 1; i<m_sizeArray - 1; i++, j--) {
		m_linkArray[i].m_type = LINKTYPE_ARRAY;
		m_linkArray[i].m_status = LINKSTATUS_ALIVE;
		m_linkArray[i].m_next = &m_linkArray[i + 1];
		m_linkArray[j].m_prev = &m_linkArray[j - 1];
	}
	m_linkArray[m_sizeArray - 1].m_type = LINKTYPE_ARRAY;
	m_linkArray[m_sizeArray - 1].m_status = LINKSTATUS_ALIVE;
	m_linkArray[m_sizeArray - 1].m_next = NULL;		// next of the last is NULL
	m_linkArray[0].m_prev = NULL;					// prev of the first also is NULL

	m_firstLink = &m_linkArray[0];					// assign first link
	m_lastLink = &m_linkArray[m_sizeArray - 1];	// assign last link

												// connect node of linked list to its entry
												// and entry to node of linked list
	for (i = 0; i<m_sizeArray; i++) {
		m_linkArray[i].m_element = (CNAEntry *)entryArray[i];
		entryArray[i]->m_myLink = (void *)&m_linkArray[i];
	}
	return 0;
}

int	CNADictionary::readLine(FILE *fp, char *buf, int len)
{
	char *ret = fgets(buf, len - 1, fp);
	return (ret != NULL) ? 0 : -1;
}

int	CNADictionary::writeLine(FILE *fp, char *buf, int len)
{
	fwrite(buf, 1, len - 1, fp);
	fwrite("\n", 1, 1, fp);
	return 0;
}


/*
This function is to load data from a given file
and make a linked list by using loading data
Then call buildDictionary which builds a double linked list actually.
prevanalysis : 기분석 사전의 경우 1의 값으로 세팅
*/
int	CNADictionary::loadDictionary(char *fname, int encryption,int prevanalysis)
{
	FILE	*fp = fopen(fname, "rb");
	if (fp == NULL) {
		setMessage(-1, "can not find a given file");
		return -1;
	}

	// read line by line, and a line is converted to CNAEntry
	// make a list of CNAEntry, so that all Entries are loaded
	int			numberOfEntry = 0;
	CNAEntry	*eList = NULL;		// a list of CNAEntry
	char	buf[4096];				// buffer of a line

									//while(fgets(buf, 4095, fp)!=NULL){
	while (1) {
		int ret = (encryption == 0) ? CNADictionary::readLine(fp, buf, 4096) : readLine(fp, buf, 4096);
		if (ret < 0)	break;
		CNAEntry	*entry = new CNAEntry("");	// create entry with dummy key
		entry->setString(buf, prevanalysis);	// a line is converted to CNAEntry

								// make a list of CNAEntry
		entry->m_nextEntry = eList;
		eList = entry;
		numberOfEntry++;
	}
	fclose(fp);

	if (numberOfEntry == 0)	// entry is nothing
	{
		m_loadingComplete = 1;//2017-08-21 해당 사전이 존재하지 않을 경우에도 로딩이 완료 된것으로 본다
		return 0;
	}
	// end of loading


	// we must sort a list of CNAEntry
	// first, we allocate array memory and store entries to it
	// so that we use qsort
	CNAEntry	**entryArray = (CNAEntry **)new CNAEntry*[numberOfEntry];
	// store 
	for (int i = 0; i<numberOfEntry; i++) {
		entryArray[i] = eList;
		eList = eList->m_nextEntry;
	}

	// start to sort
	qsort(entryArray, numberOfEntry, sizeof(CNAEntry*), CNAEntry::compare);
	// end of sorting

	buildDictionary(entryArray, numberOfEntry);

	// make hashtree
	// we need a linked list
	eList = NULL;
	CNALink	*linkPtr = m_lastLink;
	while (linkPtr != NULL) {
		linkPtr->m_element->m_nextEntry = eList;
		eList = linkPtr->m_element;
		linkPtr = linkPtr->m_prev;
	}

	// start to make hashtree
	m_hashTree = new CNAHashTree();
	m_hashTree->createTree(eList, numberOfEntry);


	delete[] entryArray;

	//로딩완료
	m_loadingComplete = 1;

	return 0;
}

int	CNADictionary::refineDictionary()
{
	int			numberOfEntry = 0;
	CNALink		*link = m_firstLink;

	// count number of Entry
	while (link != NULL) {
		numberOfEntry++;
		link = link->m_next;
	}

	// if there is no entry, clean room and return
	if (numberOfEntry == 0) {
		cleanRoom();
		return 0;
	}

	// allocate memory
	CNAEntry	**entryArray = (CNAEntry **)new CNAEntry*[numberOfEntry];

	// store entry sequentially so that their order can be maintained
	numberOfEntry = 0;
	link = m_firstLink;
	while (link != NULL) {
		entryArray[numberOfEntry++] = (CNAEntry*)link->m_element;
		link = link->m_next;
	}

	buildDictionary(entryArray, numberOfEntry);

	// make hashtree
	// we need a linked list
	CNAEntry	*eList = NULL;
	CNALink	*linkPtr = m_lastLink;
	while (linkPtr != NULL) {
		linkPtr->m_element->m_nextEntry = eList;
		eList = linkPtr->m_element;
		linkPtr = linkPtr->m_prev;
	}

	// start to make hashtree
	if (m_hashTree != NULL)	delete m_hashTree;

	m_hashTree = new CNAHashTree();
	m_hashTree->createTree(eList, numberOfEntry);

	delete[] entryArray;
	return 0;
}

int	CNADictionary::storeDictionary(char *fname, int kinddic, int encryption)
{
	FILE	*fp = fopen(fname, "wb");
	if (fp == NULL) {
		setMessage(-1, "can not find a given file");
		return -1;
	}

	// for all enrty in the double linked list
	// we convert it with a string of a line
	// and store it to a given file
	char	buf[4096];
	CNALink	*listPtr = m_firstLink;
	while (listPtr != NULL) {
		CNAEntry	*entry = (CNAEntry *)(listPtr->m_element);	// get entry
		for (int i = 0; i<entry->getNumberBitVectors(); i++) {
			entry->getString(i, buf);				// get string data
			if (encryption == 0)
				CNADictionary::writeLine(fp, buf, strlen(buf));
			else
				writeLine(fp, buf, strlen(buf));
		}
		listPtr = listPtr->m_next;				// for next entry
	}
	fclose(fp);
	return 0;
}

/*
this is routine of insert in duoble linked list
if there is entry with same key, then add bit vectors only
Otherwise, insert in duoble linked list
*/
int	CNADictionary::addEntry(CNAEntry *entry)
{
	//char	buf[MAX_KEY_LEN];

	CNALink	*targetLink = NULL, *beforeLink, *afterLink;

	// search the nearest entry or equal to entry
	beforeLink = searchEntry(entry->getKey(), targetLink);

	if (targetLink != NULL) {	// there exists a entry with same key
								// we must add bitVector
		CNAEntry	*targetEntry = (CNAEntry	*)(targetLink->m_element);

		// Because entry may have multiple bitVectors, copy all bitVectors
		for (int i = 0; i<entry->getNumberBitVectors(); i++) {
			targetEntry->addBitVector(entry->getBitVector(i));
		}

		// move current location
		m_currentLink = targetLink;
		return 0;
	}

	// make new link and fill out it
	if (m_linkFree != NULL) {
		targetLink = m_linkFree;
		m_linkFree = m_linkFree->m_next;
	}
	else {
		targetLink = new CNALink();
		targetLink->m_type = LINKTYPE_SINGLE;
	}

	// initialize new member
	targetLink->m_status = LINKSTATUS_ALIVE;
	targetLink->m_element = (CNAEntry*)entry;

	entry->m_myLink = (void*)targetLink;

	if (beforeLink == NULL) {		// entry will be the first
		targetLink->m_next = m_firstLink;
		targetLink->m_prev = NULL;
		if (m_firstLink != NULL) {
			m_firstLink->m_prev = targetLink;
		}
		else {					// entry is alone in list
			m_lastLink = targetLink;
		}
		m_firstLink = targetLink;
	}
	else	if (beforeLink == m_lastLink) {	// entry will be the last
		targetLink->m_next = NULL;
		targetLink->m_prev = beforeLink;
		beforeLink->m_next = targetLink;
		m_lastLink = targetLink;

	}
	else {
		afterLink = beforeLink->m_next;
		targetLink->m_next = afterLink;
		targetLink->m_prev = beforeLink;
		beforeLink->m_next = targetLink;
		afterLink->m_prev = targetLink;
	}

	// move current location
	m_currentLink = targetLink;

	// add it to hash hree
	if (m_hashTree != NULL) {
		m_hashTree->addEntry(entry);
	}
	else
	{
		//2017-05-31
		//사전에 데이터가 존재하지 않을 경우 HashTree가 만들어 지지 않는다.
		//데이터가 존재하지 않을 경우 최초 추가되는 Entry를 이용하여 HashTree를 만든다.
		m_hashTree = new CNAHashTree();
		entry->m_nextEntry = NULL;
		m_hashTree->createTree(entry, 1);
	}
	return 1;
}

CNAEntry	*CNADictionary::removeEntry(CNAEntry *entry)
{
	CNALink	*targetLink;

	if (entry == NULL) {	// means that we remove m_currentLink
		if (m_currentLink == NULL)	// can not find target Link
			return NULL;
	}
	targetLink = (entry != NULL) ? (CNALink*)entry->m_myLink : m_currentLink;
	entry = targetLink->m_element;

	// first, we remove it from hashTree
	if (m_hashTree != NULL) {
		m_hashTree->removeEntry(entry);
	}

	// update double linked list
	if (targetLink == m_firstLink) {	// target is the first node
		m_firstLink = m_firstLink->m_next;
		if (m_firstLink != NULL) {
			m_firstLink->m_prev = NULL;
		}
		else {				// this case is that remove makes list to be empty
			m_lastLink = NULL;
		}

	}
	else if (targetLink == m_lastLink) {	// target is the last node
		m_lastLink = m_lastLink->m_prev;
		if (m_lastLink != NULL) {
			m_lastLink->m_next = NULL;
		}
		else {				// this case is that remove makes list to be empty
			m_firstLink = NULL;
		}

	}
	else {
		targetLink->m_prev->m_next = targetLink->m_next;
		targetLink->m_next->m_prev = targetLink->m_prev;
	}


	// update m_currentLink
	if (targetLink == m_currentLink) {
		if (targetLink->m_next != NULL)	// if there is next one, it'll be current
			m_currentLink = targetLink->m_next;
		else	if (targetLink->m_prev != NULL) // if there is prev one, it'll be current
			m_currentLink = targetLink->m_prev;
		else
			m_currentLink = m_firstLink;
	}

	entry = (CNAEntry*)targetLink->m_element;

	// for next use, store the free list
	targetLink->m_status = LINKSTATUS_DIE;
	if (targetLink->m_type == LINKTYPE_SINGLE) {
		targetLink->m_next = m_linkFree;
		m_linkFree = targetLink;
	}


	return entry;

}

/*
This is an extension of binary search.
There are dummy elements(deleted element) in Array and
external elements which are not in Array

This function return targetLink if there is element with the same key
Otherwise, return the largest in smaller than orderedKey and then
targetLink must be NULL
*/
CNALink	*CNADictionary::searchEntry(char *orderedKey, CNALink *&targetLink)
{
	int	left = 0;
	int	right = m_sizeArray - 1;
	int	middle = -1;
	int	cmp;

	targetLink = NULL;	// initialize
	while (left <= right) {

		// for deleted elements, they are skipped
		while (m_linkArray[left].m_status == LINKSTATUS_DIE && left <= right) left++;
		while (m_linkArray[right].m_status == LINKSTATUS_DIE && right >= left) right--;
		if (left > right)	break;	// undeleted elements are NONE

		middle = (left + right) / 2;		// calculate middle point
											// if middle point was deleted, middle point is moved
		while (m_linkArray[middle].m_status == LINKSTATUS_DIE && middle >= left) middle--;

		// compare key
		char *mKey = ((CNAEntry*)(m_linkArray[middle].m_element))->getKey();
		cmp = CNAByteCmp(orderedKey, mKey);

		if (cmp == 0) {	// found same key
			targetLink = &m_linkArray[middle];
			return targetLink;
		}

		if (cmp < 0)	right = middle - 1;
		if (cmp > 0)	left = middle + 1;
	}

	// Now, we conclude that the orderedKey is not in Array
	// and we find the nearest key to the orderedKey, which is middle point
	// Thus, based on middle point, we find the equal key or the nearest key wrt
	// in Array and in External elements

	CNALink	*startList;
	if (middle != -1) {	// the nearest key exists
		if (cmp < 0) {	// middle key is gerate than orderedKey

						// Now, we find more near key than the middle point
						// Sometime, we can find same key in the external entry
			startList = m_linkArray[middle].m_prev;
			while (startList != NULL) {
				char *mKey = ((CNAEntry*)(startList->m_element))->getKey();
				cmp = CNAByteCmp(mKey, orderedKey);
				if (cmp <= 0) {	// find the nearest key
					if (cmp == 0)	targetLink = startList; // find the same key
					return startList;
				}
				startList = startList->m_prev;
			}

			// this means that all keyes are greater than the orderedKey
			return NULL;
		}

		// middle point is less than the orderedKey
		startList = m_linkArray[middle].m_next;
	}
	else {	// This case is empty in Array and we must find External elements
		startList = m_firstLink;
	}

	// This routine is similar to find the nearest key in the accending ordered list
	while (startList != NULL) {
		char *mKey = ((CNAEntry*)(startList->m_element))->getKey();
		cmp = CNAByteCmp(mKey, orderedKey);

		if (cmp >= 0) {
			if (cmp == 0)	targetLink = startList;
			return startList->m_prev;
		}
		startList = startList->m_next;
	}
	return m_lastLink;

}

/*
This is used in outside.
It is the same to searchEntry except on parameters and return value
*/
CNAEntry*	CNADictionary::searchEntryBs(char *orderedKey, int *matchFlag)
{
	CNALink	*beforeLink, *targetLink;

	// search the nearest entry or equal to entry
	beforeLink = searchEntry(orderedKey, targetLink);

	// if exact matching, return 1, else 0
	if (matchFlag != NULL)
		*matchFlag = (targetLink != NULL) ? 1 : 0;

	// This is the exact matching
	if (targetLink != NULL)
		return (CNAEntry *)targetLink->m_element;

	// This is not found but return the nearest value in the smaller than orderedKey
	if (matchFlag != NULL && beforeLink != NULL)
		return (CNAEntry *)beforeLink->m_element;

	return NULL;
}

/*
This is used in outside.
It is the same to searchEntry except on parameters and return value
*/
CNAEntry*	CNADictionary::searchEntryHs(char *orderedKey)
{
	//사전의 레코드가 존재하지 않을 경우
	if (m_hashTree == NULL) return NULL;
	return m_hashTree->searchEntry(orderedKey);
}

/*
A link to be point by entry become the current Link
If the link does not exist or it was already die, return -1;
*/
int CNADictionary::setEntry(CNAEntry *entry)
{
	// is it valid
	if (entry->m_myLink == NULL)	return -1;
	CNALink *tmpLink = (CNALink *)entry->m_myLink;

	// is it alive
	if (tmpLink->m_status == LINKSTATUS_DIE)	return -1;
	m_currentLink = tmpLink;
	return 0;
}

/*
return entry pointed by m_currentLink
*/
CNAEntry *CNADictionary::getEntry()
{
	if (m_currentLink == NULL)	return NULL;
	return (CNAEntry *)m_currentLink->m_element;
}


/*
move to the first and return its entry
*/
CNAEntry* CNADictionary::getFirstEntry()
{
	m_currentLink = m_firstLink;
	return (m_currentLink != NULL) ? m_currentLink->m_element : NULL;
}

/*
move to the last and return its entry
*/
CNAEntry* CNADictionary::getLastEntry()
{
	m_currentLink = m_lastLink;
	return (m_currentLink != NULL) ? m_currentLink->m_element : NULL;
}

/*
move to the next and return its entry
*/
CNAEntry* CNADictionary::getNextEntry()
{
	// if m_currentLink is NULL, return NULL
	if (m_currentLink == NULL)	return NULL;

	CNAEntry	*entry = m_currentLink->m_element;
	m_currentLink = m_currentLink->m_next;
	return entry;
}

/*
move to the previous and return its entry
*/
CNAEntry* CNADictionary::getPrevEntry()
{
	// if m_currentLink is NULL, return NULL;
	if (m_currentLink == NULL)	return NULL;

	CNAEntry	*entry = m_currentLink->m_element;
	m_currentLink = m_currentLink->m_prev;
	return entry;
}

int	CNADictionary::encLoadDictionary(char *fname, int kinddic, int encryption)
{
	FILE	*fp = fopen(fname, "rb");
	if (fp == NULL) {
		setMessage(-1, "can not find a given file");
		return -1;
	}

	// read line by line, and a line is converted to CNAEntry
	// make a list of CNAEntry, so that all Entries are loaded
	int			numberOfEntry = 0;
	CNAEntry	*eList = NULL;		// a list of CNAEntry
									//char	buf[4096];				// buffer of a line

									//while(fgets(buf, 4095, fp)!=NULL){
	int nTotal = 0;
	//참고 URL : http://beej.us/guide/bgc/output/html/multipage/fread.html
	fread((void *)&nTotal, sizeof(int), 1, fp);
	while (nTotal-- > 0) {
		CNAEntry	*entry = new CNAEntry("");	// create entry with dummy key
		entry->LoadDic(fp);
		// make a list of CNAEntry
		entry->m_nextEntry = eList;
		eList = entry;
		numberOfEntry++;
	}
	fclose(fp);

	if (numberOfEntry == 0)	// entry is nothing
	{
		m_loadingComplete = 1;//2017-08-21 해당 사전이 존재하지 않을 경우에도 로딩이 완료 된것으로 본다
		return 0;
	}
	// end of loading

	// we must sort a list of CNAEntry
	// first, we allocate array memory and store entries to it
	// so that we use qsort
	CNAEntry	**entryArray = (CNAEntry **)new CNAEntry*[numberOfEntry];

	// store 
	for (int i = 0; i<numberOfEntry; i++) {
		entryArray[i] = eList;
		eList = eList->m_nextEntry;
	}

	// start to sort
	qsort(entryArray, numberOfEntry, sizeof(CNAEntry*), CNAEntry::compare);
	// end of sorting

	buildDictionary(entryArray, numberOfEntry);

	// make hashtree
	// we need a linked list
	eList = NULL;
	CNALink	*linkPtr = m_lastLink;
	while (linkPtr != NULL) {
		linkPtr->m_element->m_nextEntry = eList;
		eList = linkPtr->m_element;
		linkPtr = linkPtr->m_prev;
	}

	// start to make hashtree
	m_hashTree = new CNAHashTree();
	m_hashTree->createTree(eList, numberOfEntry);

	delete[] entryArray;

	//로딩완료
	m_loadingComplete = 1;

	return 0;

}

int CNADictionary::encStoreDictionary(char *fname, int kinddic, int encryption)
{
	FILE	*fp = fopen(fname, "wb");
	if (fp == NULL) {
		setMessage(-1, "can not find a given file");
		return -1;
	}

	//전체 Entry Count
	int nTotal = 0;
	m_currentLink = m_firstLink;
	while (m_currentLink != NULL)
	{
		m_currentLink = m_currentLink->m_next;
		nTotal++;
	}
	// for all enrty in the double linked list
	// we convert it with a string of a line
	// and store it to a given file
	//char	buf[4096];
	int n = 0;
	if (nTotal > 0)
	{
		CNALink	*listPtr = m_firstLink;
		fwrite((void *)&nTotal, sizeof(int), 1, fp);
		while (listPtr != NULL) {
			CNAEntry	*entry = (CNAEntry *)(listPtr->m_element);	// get entry

			if (entry->SaveDic(fp) != 1)
				return -1;
			listPtr = listPtr->m_next;				// for next entry
		}
	}
	fclose(fp);
	return 0;
}

int CNADictionary::getDicLoadingComplete()
{
	return m_loadingComplete;
}