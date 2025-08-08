#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	"nautil.h"
#include	"naobject.h"
#include	"naentry.h"
//#include	"nalink.h"
//#include	"nadictionary.h"
#include	"nahashnode.h"
#include	"nahashtree.h"

CNAHashTree::CNAHashTree()
{
	m_rootNode= NULL;
}
CNAHashTree::~CNAHashTree()
{
	if(m_rootNode != NULL)	delete m_rootNode;
}

int	CNAHashTree::createTree(CNAEntry *entryList, int entryCount)
{
	m_rootNode= insertBulkEntry(0, entryList, entryCount);
	return 0;
}

/*
	Construct a node in tree
	1. Make CNAHashNode with the proper size
	2. Divide entryList into the some entryLists
	3. For each subentryList, make CNAhashNode recursively
*/
CNAHashNode	*CNAHashTree::insertBulkEntry(int keyLevel, CNAEntry *entryList, int entryCount)
{
	int				i;
	int				bitSize;
	int				childSize;
	CNAEntry		*forme, *t;
	//재귀호출 탈출 제어문
	if(entryList == NULL)	return NULL;
	if(entryCount == 0)		return NULL;

	// recalcuate childSize so that it is greater than entryCount
	// and it is 2 power x
	childSize= 1;
	while(childSize<entryCount)	childSize <<= 1;

	// calculate bitMap Size: childSize*DEFAULT_BITSIZE_TIMES
	bitSize= (keyLevel != 0)? childSize*DEFAULT_BITSIZE_TIMES: 0;

	// create new node
	CNAHashNode	*node= new CNAHashNode(childSize, bitSize);

	// this represents the depth in tree
	node->setKeyLen(keyLevel);

	// prepare memory to divide all entry into some children
	forme= NULL;	
	int	formeCount=0;
	CNAEntry	**entryLists= new CNAEntry*[childSize];
	int			*entryCounts= new int[childSize];
	for(i=0;i<childSize;i++){
		entryLists[i]= NULL;
		entryCounts[i]= 0;
	}

	// divide entrys into childSize subentrys
	while(entryList != NULL){
		// extract one entry, pointed by t
		t= entryList;
		entryList= entryList->m_nextEntry;

		// if its keyLen is keyLevel, it need not to be divided 
		if(t->getKeyLen() == (int)(keyLevel*DEFAULT_SHORT_SIZE)){
			//KeyLevel에 해당하는 것들만 서로 그룹핑한다.
			//예: 한단어에 해당하는 것들, 두단어에 해당하는 것들, 세단어에 해당하는 것들의 그룹핑 정보
			t->m_nextEntry= forme;
			forme= t;
			formeCount++;
			continue;
		}
		//아래 로직은 해당 KeyLevel에 해당하는  Entry가 아닌 경우 따로 entryLists에 담아서 다시 재귀함수를 호출하여
		//적당한 HashNode에 삽입되게 처리한다.
		char	*key= t->getKey();

		// mark that there is value in children
		node->setBitMap(keyLevel, key);

		// calculate which child is proper
		int	hashValue= node->getHashValue(keyLevel, key);

		t->m_nextEntry= entryLists[hashValue];
		entryLists[hashValue]= t;
		entryCounts[hashValue]++;
	}
	// for forme
	//KeyLevel에 해당하는 Entry를 해당 노드에 생성한다.(내림또는 오름차순 기준으로 생성)
	node->setForme(forme, formeCount, (keyLevel+1)%2);

	// for all sub entry, call insertBulkEntry
	for(i=0;i<childSize;i++){
		node->child(i)= insertBulkEntry(keyLevel+1, entryLists[i], entryCounts[i]);
	}
	delete[] entryLists;
	delete[] entryCounts;

	return node;
}

CNAEntry	*CNAHashTree::searchEntry(char *orderedKey)
{
	int	level=0;
	int	keyLen= strlen(orderedKey);

	keyLen += keyLen%2;

	CNAEntry	*entry= NULL;
	CNAHashNode	*node= getRootNode();
	if(node == NULL)	return NULL;
	while(1){
		node= node->getHashChild(level++, keyLen, orderedKey, &entry);
		if(node == NULL)	break;
	}
	return entry;


}

void	CNAHashTree::addEntry(CNAEntry *entry)
{
	char		*key= entry->getKey();

	CNAHashNode	*node= m_rootNode;
	int	depth= entry->getKeyLen()/2;	// depth of tree for this key
	int	keyLevel= 0;
	while(keyLevel < depth){
		// get hash value
		int	hashValue= node->getHashValue(keyLevel, key);

		// set bit map so that we can know that there is the key
		node->setBitMap(keyLevel, key);

		// get pointer of child node
		CNAHashNode	**child= &(node->getHashChild(hashValue));
		if(*child == NULL){	// there is no child node
			// need new child
			CNAHashNode	*newNode= new CNAHashNode(MIN_CHILD_SIZE, MIN_BITMAP_SIZE);

			// this represents the depth in tree
			newNode->setKeyLen(keyLevel+1);

			// enroll new child node
			*child= newNode;	
		}

		// proceed the next child
		node= *child;
		keyLevel++;
	}

	// this is the final node, add forme
	node->addForme(entry);
}

void	CNAHashTree::removeEntry(CNAEntry *entry)
{
	char		*key= entry->getKey();

	CNAHashNode	*node= m_rootNode;
	int	depth= entry->getKeyLen()/2;	// depth of tree for this key
	int	keyLevel= 0;
	while(keyLevel < depth){

		// check if the key exists
		if(node->getBitMap(keyLevel, key) == 0)	return;

		// get hash value
		int	hashValue= node->getHashValue(keyLevel, key);

		// get pointer of child node
		CNAHashNode	**child= &(node->getHashChild(hashValue));
		if(*child == NULL)	return;

		// proceed the next child
		node= *child;
		keyLevel++;
	}

	// this is the final node, add forme
	node->removeForme(entry);
}
