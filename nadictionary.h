#ifndef	NADICTIONARY_H
#define	NADICTIONARY_H

#define STEM_FIELD 234
#define ENDING_FIELD 46

class	CNADictionary : public CNAObject
{
	CNALink*	m_firstLink;
	CNALink*	m_lastLink;
	CNALink*	m_currentLink;

	int			m_sizeArray;
	CNALink*	m_linkArray;
	CNALink*	m_linkFree;

	CNAHashTree*	m_hashTree;

	void		cleanRoom();
	CNALink*	searchEntry(char *orderedKey, CNALink *&targetLink);
	int			buildDictionary(CNAEntry *entryArray[], int numberOfEntry);

public:
	CNADictionary(char *fname = NULL);
	~CNADictionary();
	//kinddic 1:스템사전 2:엔딩사전 3:사용자사전 4:코퍼스사전
	int			loadDictionary(char *fname, int encryption, int prevanalysis);	// load a dictionary from a given file
	int			storeDictionary(char *fname, int kinddic, int encryption = 0);	// store a dictionary to a given file
	int			refineDictionary();				// reconstruct a dictionary

	int			addEntry(CNAEntry *entry);
	CNAEntry*	removeEntry(CNAEntry *entry = NULL);

	CNAEntry*	searchEntryBs(char *orderedKey, int *matchFlag = NULL);
	CNAEntry*	searchEntryHs(char *orderedKey);

	int			setEntry(CNAEntry *entry);
	CNAEntry*	getEntry();

	CNAEntry*	getFirstEntry();
	CNAEntry*	getLastEntry();
	CNAEntry*	getNextEntry();
	CNAEntry*	getPrevEntry();

	virtual	int	readLine(FILE *fp, char *buf, int len);
	virtual	int	writeLine(FILE *fp, char *buf, int len);
	//kinddic 1:스템사전 2:엔딩사전 3:사용자사전 4:코퍼스사전
	int	encLoadDictionary(char *fname, int kinddic, int encryption = 0);
	int encStoreDictionary(char *fname, int kinddic, int encryption = 0);

	int m_loadingComplete;//사전로딩완료여부
	int getDicLoadingComplete();
};

#endif