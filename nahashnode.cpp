#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	"nautil.h"
#include	"naobject.h"
#include	"naentry.h"
#include	"nahashnode.h"


/*
	This variable is used to ehance the performance of setting bit and getting bit
	This is a kind of look-up table.
	This is only one in the program
*/
int	*CNAHashNode::m_bitMask= NULL;

/*
	Construction
*/
CNAHashNode::CNAHashNode(int childSize, int bitSize)
{
	int	i;

	// if m_bitMask is not initialized, initialize it
	if(m_bitMask == NULL){
		int	mask= 0x1;
		m_bitMask= new int[DEFAULT_INT_BIT_SIZE];	// 0b1, 0b10, 0b100, ....
		for(i=0;i<DEFAULT_INT_BIT_SIZE;i++){
			m_bitMask[i]= mask;
			mask <<= 1;
		}
	}

	// initialize bitMap, 
	m_bitMap= NULL;
	m_bitSize= bitSize;
	if(m_bitSize != 0){
		int	len= (m_bitSize-1)/DEFAULT_INT_BIT_SIZE+1;	// calculate its size of int unit
		m_bitMap= new unsigned int[len];			// allocate memory
		for(i=0;i<len;i++){
			m_bitMap[i]= 0x00000000;				// initialize
		}
	}

	// initialize pointers for childrens
	// minimum number of children is garenteed as DEFAULT_CHILDREN_SIZE
	m_keyLen= 0;
	m_childSize= (childSize!=0)? childSize: DEFAULT_CHILDREN_SIZE;
	m_children= new CNAHashNode*[m_childSize];
	for(i=0;i<m_childSize;i++)
		m_children[i]= NULL;						// initialize as NULL

	m_entryArray= NULL;
	m_entrySize= 0;
	m_entryLen= 0;
}

CNAHashNode::~CNAHashNode()
{
	if(m_children != NULL){
		for(int i=0;i<m_childSize;i++){
			if(m_children[i] != NULL)
				delete m_children[i];
		}
		delete[] m_children;
	}
	if(m_bitMap != NULL)	delete[] m_bitMap;
	if(m_bitMask != NULL)
	{
		delete[] m_bitMask;
		m_bitMask = NULL;
	}
	if(m_entryArray!= NULL) delete[] m_entryArray;
}

/*
	set key level which is the depth of hash tree
*/
void CNAHashNode::setKeyLen(int keyLevel)
{
	m_keyLen= keyLevel*2;
}

/* 
	Extract the proper value from key
	Now, we extract 2bytes and they are converted to a short
*/
unsigned int	CNAHashNode::getValue(int keyLevel, char *key)
{
	unsigned	short	val= ((unsigned short *)key)[keyLevel];
	return (unsigned int)val;
}

/*
	Set bitMap in order to note existence of value
*/
void CNAHashNode::setBitMap(int keyLevel, char *key)
{
	if(m_bitSize == 0)	return;

	unsigned int	value= getValue(keyLevel, key);
	value= value%m_bitSize;					// convert value to address
	int	intLoc= value/DEFAULT_INT_BIT_SIZE;	// location in integer array
	int	bitLoc= value%DEFAULT_INT_BIT_SIZE;	// location in integer bit

	m_bitMap[intLoc]= m_bitMap[intLoc] | m_bitMask[bitLoc];

}

/*
	get bitMap in order to check existence of value.
	if return 1, there is value
	Otherwise, there is not value
*/
int	CNAHashNode::getBitMap(int keyLevel, char *key)
{
	// if m_bitSize is zero, it means that there are any values
	if(m_bitSize == 0)	return 1;

	unsigned int	value= getValue(keyLevel, key);
	value= value%m_bitSize;					// convert value to address
	int	intLoc= value/DEFAULT_INT_BIT_SIZE;	// location in integer array
	int	bitLoc= value%DEFAULT_INT_BIT_SIZE;	// location in integer bit

	return m_bitMap[intLoc] & m_bitMask[bitLoc];
}

CNAHashNode*&	CNAHashNode::child(int index)
{
	return m_children[index];
}

/*
	entryList must be maintained as sorted in array so that we can search binary
*/
void	CNAHashNode::setForme(CNAEntry *entryList, int entryCount, int needReverse)
{
	int	i;

	// The list is stored to Array so that binary search can be performed
	m_entrySize= entryCount;
	m_entryLen= m_entrySize+ m_entrySize/2 +1;
	if(m_entryLen < DEFAULT_ENTRY_SIZE)	m_entryLen= DEFAULT_ENTRY_SIZE;

	m_entryArray= new CNAEntry*[m_entryLen];

	if(needReverse){
		for(i=m_entrySize-1;i>=0; i--){
			m_entryArray[i]= entryList;
			entryList= entryList->m_nextEntry;
		}

	}else{
		for(int i=0;i<m_entrySize; i++){
			m_entryArray[i]= entryList;
			entryList= entryList->m_nextEntry;
		}
	}

}

void	CNAHashNode::addForme(CNAEntry *entry)
{
	// for the first entry, we need space
	if(m_entryArray == NULL){
		m_entryLen= DEFAULT_ENTRY_SIZE;
		m_entryArray= new CNAEntry*[m_entryLen];
		m_entrySize= 0;
	}

	// overflow entry wrt array size
	if(m_entrySize == m_entryLen){
		m_entryLen += m_entryLen/2;
		CNAEntry**	tmpArray= new CNAEntry*[m_entryLen];
		for(int i=0;i<m_entrySize;i++)
			tmpArray[i]= m_entryArray[i];
		delete[] m_entryArray;
		m_entryArray= tmpArray;
	}

	// add it to array
	// As the keys were already sorted, inertion is best
	int	e= m_entrySize-1;
	while(e >= 0){
		if(CNAByteCmp(entry->getKey(), m_entryArray[e]->getKey())>=0) break;
		m_entryArray[e+1]=m_entryArray[e];
		e--;
	}
	m_entryArray[e+1]=entry;
	m_entrySize++;
}

void	CNAHashNode::removeForme(CNAEntry *entry)
{
	int	left= 0;
	int	right= m_entrySize-1;
	int	middle;

	while(left <= right){
		middle= (left+right)/2;
		CNAEntry	*entryMiddle= m_entryArray[middle];
		int	cmp= CNAByteCmp(entry->getKey(), entryMiddle->getKey());
		if(cmp == 0)	break;
		if(cmp < 0)	right= middle-1;
		else		left= middle+1;
	}
	// can not found
	if(left > right)	return;
	
	// shift in order to remove a hole
	for(int i= middle;i<m_entrySize-1;i++)
		m_entryArray[i]= m_entryArray[i+1];
	m_entrySize--;
}

/*
	find entry with given key by using binary search
*/
CNAEntry	*CNAHashNode::findForme(char *key)
{
	int	left= 0;
	int	right= m_entrySize-1;

	while(left <= right){
		int	middle= (left+right)/2;
		CNAEntry	*entry= m_entryArray[middle];
		int	cmp= CNAByteCmp(key, entry->getKey());
		if(cmp == 0)	return entry;
		if(cmp < 0)	right= middle-1;
		else		left= middle+1;
	}
	return NULL;
}

/*
	convert keyLevel and  key to index of child in children
*/
int	CNAHashNode::getHashValue(int keyLevel, char *key)
{
	int	value= getValue(keyLevel, key);
	return value%m_childSize;
}

/* 
	convert keyLevel and  key to index of child in children
	if there are no more children, find the maching entry
*/
CNAHashNode* CNAHashNode::getHashChild(int keyLevel, int keyLen, char *key, CNAEntry **entry)
{

	*entry= NULL;
	if(m_keyLen == keyLen){	// there are no more children
		*entry= findForme(key);
		return NULL;
	}

	// first, check bit map so that return NULL if bit map is reset
	int	value= getHashValue(keyLevel, key);
	if(getBitMap(keyLevel, key)==0)
		return NULL;
	return m_children[value];
//	return (getBitMap(keyLevel, key)!=0)? m_children[value]: NULL;
}

CNAHashNode*&	CNAHashNode::getHashChild(int index)
{
	if(index < 0 || index >= m_childSize)	index= 0;
	return m_children[index];
}

