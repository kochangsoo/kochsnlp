#ifndef	NAENTRY_H
#define	NAENTRY_H

#define	MAX_KEY_LEN				50
#define	MAX_FIELD_LEN			256
#define	DEFAULT_BITVECTOR_SIZE	10

#define ENCODE_BIT 0x33
#define RECORD 234

class	CNAEntry: public CNAObject
{
	// for key
	char			*m_key;						// store a key with short format
	int				m_keyLen;					// length of short not char

	// for fields
	char			**m_bitVectors;				// really char vector
	int				m_numberBitVectors;			// number of vectors wrt data
	int				m_sizeBitVectors;			// size of vectors wrt memory 

	char			*m_prevAnalysisResult; //기분석사전의 형태소 분석 결과
	int				m_prevAnalysisResultLen;	// length of short not char

public:
	CNAEntry(char *key=NULL, char *bitVector=NULL);
	~CNAEntry();

	int				setKey(char *key);			// set key
	char			*getKey(char *buf=NULL);	// get key
	
	int				setPrevAnalysisResult(char *key);			//기분석사전 형태소 분석 결과 정보
	char			*getPrevAnalysisResult(char *buf = NULL);	//기분석사전 형태소 분석 결과 정보

	void			setString(char *string, int prevanalysis);			// convert string to key and bitVector
	void			setString(char *stem, char *pField);

	char			*getString(int index, char *string);	// convert key and bitVector to string

	char			*addBitVector(char *bitVector);		// add a bitVector
	char			*removeBitVector(char *bitVector);	// remove a bitVector
	char			*removeBitVector(int index);		// remove  a bitVector

	int				getNumberBitVectors();				// return number of bit vectors
	char			*getBitVector(int index);	
	
	int				getKeyLen(){	return m_keyLen;}
	unsigned short*	getShortKey(){	return (unsigned short*)m_key; }
	static	int		compare(const void *elem1, const void *elem2);		// compare two CNAEntry
public:
	CNAEntry		*m_nextEntry;						// for many other uses
	void			*m_myLink;							// use in dictionary
	void			print();
	CNAEntry*		clone();

	void Encoding(char *stem);
	void Decoding(char *stem);
	int LoadDic(FILE *fpout);
	int SaveDic(FILE *fpin);
	void str2bit(char *rbuf, unsigned char *bit);
	void bit2str(unsigned char *bit, char *rbuf);
	int GetBitOff(int idx, int *byteoff, int *bitoff);
	int	SetBitVector(int index, char* bitVector);
};


#endif
