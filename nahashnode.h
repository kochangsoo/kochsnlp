#ifndef	NAHASHNODE_H
#define	NAHASHNODE_H


#define	DEFAULT_CHILDREN_SIZE	4
#define	DEFAULT_INT_SIZE		sizeof(int)
#define	DEFAULT_INT_BIT_SIZE	32
#define	DEFAULT_ENTRY_SIZE		4

class	CNAHashNode: public CNAObject
{
	static	int*	m_bitMask;
	unsigned int*	m_bitMap;
	int				m_bitSize;

	int				m_keyLen;
	CNAHashNode**	m_children;
	int				m_childSize;

	CNAEntry**		m_entryArray;
	int				m_entrySize;
	int				m_entryLen;

	unsigned int	getValue(int keyLevel, char *key);
public:

	CNAHashNode(int childSize, int bitSize);
	~CNAHashNode();

	CNAHashNode*&	child(int index);
	void			setForme(CNAEntry *entryList, int entryCount, int needReverse); 
	void			addForme(CNAEntry *entry);
	void			removeForme(CNAEntry *entry);

	CNAEntry*		findForme(char *key);
	int				getLevelValue(int keyLevel, char *key);

	void			setKeyLen(int keyLen);
	void			setBitMap(int keyLevel, char *key);
	int				getBitMap(int keyLevel, char *key);
	int				getHashValue(int keyLevel, char *key);
	CNAHashNode*	getHashChild(int keyLevel, int keyLen, char *key, CNAEntry **entry);
	CNAHashNode*&	getHashChild(int index);

	//void			addEntry(CNAEntry *entry);
	void			removeEntry(CNAEntry *entry);

};

#endif