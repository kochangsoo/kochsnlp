#ifndef	NAHASHTREE_H
#define	NAHASHTREE_H

#define	DEFAULT_BITSIZE_TIMES	32
#define	DEFAULT_SHORT_SIZE		sizeof(unsigned short)
#define	MIN_CHILD_SIZE			16
#define	MIN_BITMAP_SIZE			(MIN_CHILD_SIZE*DEFAULT_BITSIZE_TIMES)

class	CNAHashTree: public CNAObject
{
	CNAHashNode*	m_rootNode;
	CNAHashNode*	insertBulkEntry(int keyLevel, CNAEntry *entryList, int entryCount);

public:
	CNAHashTree();
	~CNAHashTree();
	int				createTree(CNAEntry *entryList, int entryCount);
	CNAHashNode*	getRootNode(){	return m_rootNode; }

	CNAEntry*		searchEntry(char *key);
	void			addEntry(CNAEntry *entry);
	void			removeEntry(CNAEntry *entry);

};

#endif