#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	"naobject.h"
#include	"naentry.h"

#pragma warning(disable:4996)

/*
	data members are initialized
	and if key is not NULL, build Key
	and if bitVector is not NULL, add it
	Because memory management of bitVector is up to this object, 
	bitVector must be not deallocated at external.
*/
CNAEntry::CNAEntry(char *key, char *bitVector)
{	
	// for key, initialize
	m_key= NULL;
	m_keyLen= 0;


	m_prevAnalysisResult = NULL;

	// set bitVector, do'nt care the format of bitVector
	// we only store it and if want it, return it.
	m_sizeBitVectors= DEFAULT_BITVECTOR_SIZE;

	m_bitVectors = new char*[m_sizeBitVectors];	// allocate default size

	if(m_bitVectors == NULL){	// Memory is not sufficient
		setMessage(-1, "Memory is not sufficient in CNAEntry");
		return;
	}

	// initialize
	m_numberBitVectors= 0;

	if(key == NULL)	return;		// this is empty entry	
	
	setKey(key);			// build key
	
	if(bitVector != NULL)
		addBitVector(bitVector);
}

/*
	distruction
*/
CNAEntry::~CNAEntry()
{
	if(m_bitVectors != NULL)
	{
		for(int i=0;i<m_numberBitVectors;i++)
		{
			delete m_bitVectors[i];
		}
		delete[] m_bitVectors;
	}
	if(m_key != NULL)			delete[] m_key;

	if (m_prevAnalysisResult != NULL)			delete[] m_prevAnalysisResult;
	
}


/*
	key is a word which is a string in C++
	we store it to unsigned short array in order to enhance efficiency;
*/
int	CNAEntry::setKey(char *key)
{

	if(key == NULL){		// this case is not allowed
		setMessage(-1, "key is not allowed as NULL in CNAEntry");
		return -1;
	}
	
	m_keyLen= strlen(key);	// it is length of string
	m_keyLen += m_keyLen%2;	// if it is odd, incerase it as one

	/*
	if(m_keyLen == 0){
		setMessage(-1, "key is NULL in CNAEntry");
		return -1;
	}
	*/

	// allocate memory, it will be deallocated at destructor
	if(m_key != NULL)	delete[] m_key;
	m_key= new char[m_keyLen+1];

	if(m_key == NULL){	// Memory is not sufficient
		setMessage(-1, "Memory is not sufficient in CNAEntry");
		return -1;
	}
	
	// copy key to m_key
	int	i;
	for(i=0;i<m_keyLen;i++){
		m_key[i]= *key;
		if(*key != 0)	key++;
	}
	m_key[i]= 0;

	return 0;
}


/*
	copy & return
*/
char	*CNAEntry::getKey(char *buf)
{
	if(buf == NULL)	return m_key;

	int	i;

	for(i=0;m_key[i] != 0; i++)
		buf[i]= m_key[i];
	buf[i]= 0;

	return buf;
}



/*
key is a word which is a string in C++
we store it to unsigned short array in order to enhance efficiency;
*/
int	CNAEntry::setPrevAnalysisResult(char *key)
{

	if (key == NULL) {		// this case is not allowed
		setMessage(-1, "key is not allowed as NULL in CNAEntry");
		return -1;
	}

	m_prevAnalysisResultLen = strlen(key);	// it is length of string
	m_prevAnalysisResultLen += m_prevAnalysisResultLen % 2;	// if it is odd, incerase it as one

								

	// allocate memory, it will be deallocated at destructor
	if (m_prevAnalysisResult != NULL)	delete[] m_prevAnalysisResult;
	m_prevAnalysisResult = new char[m_prevAnalysisResultLen + 1];

	if (m_prevAnalysisResult == NULL) {	// Memory is not sufficient
		setMessage(-1, "Memory is not sufficient in CNAEntry");
		return -1;
	}

	// copy key to m_key
	int	i;
	for (i = 0; i<m_prevAnalysisResultLen; i++) {
		m_prevAnalysisResult[i] = *key;
		if (*key != 0)	key++;
	}
	m_prevAnalysisResult[i] = 0;

	return 0;
}


/*
copy & return
*/
char	*CNAEntry::getPrevAnalysisResult(char *buf)
{
	if (buf == NULL)	return m_prevAnalysisResult;

	int	i;

	for (i = 0; m_prevAnalysisResult[i] != 0; i++)
		buf[i] = m_prevAnalysisResult[i];
	buf[i] = 0;

	return buf;
}


/*
	Receive a string which consists of key("XXXX") and 0101010
	and resolve string as key and bitVector
	so that they are stored
	기분석 사전의 경우 prevanalysis의 값 1로 세팅
*/
void	CNAEntry::setString(char *string, int prevanalysis)
{
	// string: "I am a key"   0   1  0111 00 0
	
	// start of extracting key
	// find the first "
	while(*string != 0 && *string != '"')	string++;
	if(*string == 0)	return;

	char	*startPtr= ++string;	// this is start of key
	while(*string != 0 && *string != '"')	string++;
	char	*endPtr= string++;		// this is the second "

	int	i, keyLen= endPtr-startPtr;
	char	key[MAX_KEY_LEN*sizeof(short)];
	for(i=0;i<keyLen;i++)			// copy key value
		key[i]= *startPtr++;
	key[i]= 0;						// mark end of key
	// end of extracting key


	setKey(key);// build key

	// start of extracting bitVector
	int	j, bitLen = strlen(string);
	if (prevanalysis == 0)
	{
		char	bitVector[MAX_FIELD_LEN];
		for (i = 0, j = 0; i < bitLen; i++, string++)
		{
			if (*string != '0' && *string != '1')	continue;	// skip if not 0 or 1
			bitVector[j++] = *string;
		}
		bitVector[j++] = 0;
		// end of extracting bitVector
		addBitVector(bitVector);	// add bitVector
	}
	else
	{
		char	prev_result_word[MAX_KEY_LEN * sizeof(int)];
		for (i = 0, j = 0; i < bitLen; i++, string++)
		{
			if (*string == '\t' || *string == '\r' || *string == '\n')	continue;	// skip if not \t or \r or \n
			prev_result_word[j++] = *string;
		}
		prev_result_word[j++] = 0;
		setPrevAnalysisResult(prev_result_word);//기분석 사전의 결과 저장
	}

	
}



void CNAEntry::setString(char *stem, char *pField)
{
	if (stem == 0) return;
	int i, keyLen = strlen(stem);
	char	key[MAX_KEY_LEN * sizeof(short)];
	for (i = 0; i<keyLen; i++)			// copy key value
		key[i] = *stem++;
	key[i] = 0;						// mark end of key

									// start of extracting bitVector
	int		j, bitLen = strlen(pField);
	char	bitVector[MAX_FIELD_LEN];
	for (i = 0, j = 0; i<bitLen; i++, pField++) {
		if (*pField != '0' && *pField != '1')	continue;	// skip if not 0 or 1
		bitVector[j++] = *pField;
	}
	bitVector[j++] = 0;
	// end of extracting bitVector

	setKey(key);			// build key
	addBitVector(bitVector);	// add bitVector
}

/*
	Make the shortized key to string and append bitVector to it
*/
char	*CNAEntry::getString(int index, char *string)
{
	if(index < 0 || index >= m_numberBitVectors)	return NULL;

	// copy key as "XXXXX"
	char	*p= string;
	*p++= '"';
	for(int i=0;m_key[i] != 0;i++)
		*p++ = m_key[i];
	*p++= '"';
	*p++= 0;
	
//	string= getKey(string);		// get string of key

	// make bitVector to be string with tab separator
	char	*bitVector= getBitVector(index);
	if(bitVector == NULL)	return string;
	p= string+strlen(string);
	while(*bitVector != 0){
		*p++ = '\t';
		*p++ = *bitVector++;
	}
	*p++= '\n';
	*p++= 0;
	return string;
}

/*
	Add bitVector simply
*/
char	*CNAEntry::addBitVector(char *bitVector)
{
	int	i;

	// check available room
	if(m_sizeBitVectors == m_numberBitVectors){		// need more room
		m_sizeBitVectors += DEFAULT_BITVECTOR_SIZE;
		char **bitVectors= new char*[m_sizeBitVectors];	// allocate default size
		for(int i=0;i<m_numberBitVectors;i++)
			bitVectors[i]= m_bitVectors[i];
	}

	// allocate and copy
	char	*p= new char[strlen(bitVector)+1];
	for(i=0;bitVector[i] != 0;i++){
		p[i]= bitVector[i];
	}
	p[i]= 0;

	return 	m_bitVectors[m_numberBitVectors++]= p;
}


/*
	remove bitVector simply
	First find a given bitVector in array, and if exist, delete
*/
char	*CNAEntry::removeBitVector(char *bitVector)
{
	// find bitVector
	for(int i=0;i<m_numberBitVectors;i++){
		if(m_bitVectors[i] == bitVector){
			// found, shift so that the there is no hole in array
			for(i= i+1;i<m_numberBitVectors;i++){
				m_bitVectors[i-1]= m_bitVectors[i];
			}

			// decrease number of fields
			m_numberBitVectors--;

			return bitVector;
		}
	}

	// can not find a given bitVector in array
	return NULL;
}

/*
	remove bitVector simply
	First check that a given index  is correct or not
*/
char	*CNAEntry::removeBitVector(int index)
{
	if(index < 0 || index >= m_numberBitVectors){
		// this is invalid index
		return NULL;
	}

	char	*bitVector= m_bitVectors[index];	// save for return

	// found, shift so that the there is no hole in array
	for(int i=index+1;i<m_numberBitVectors;i++){
		m_bitVectors[i-1]= m_bitVectors[i];
	}

	// decrease number of fields
	m_numberBitVectors--;

	return bitVector;
}

/*
	return number of bitVector
*/
int	CNAEntry::getNumberBitVectors()
{
	return m_numberBitVectors;
}

/*
	return bitVector stored in index th array
	First check that a given index  is correct or not
*/
char	*CNAEntry::getBitVector(int index)
{
	if(index < 0 || index >= m_numberBitVectors){
		// this is invalid index
		return NULL;
	}
	return m_bitVectors[index];
}

/*
	compare two CNAEntry elem1, elem2 wrt their key
	return negative integer, 0, positive integer
*/
int	CNAEntry::compare(const void *elem1, const void *elem2)
{
	CNAEntry	*e1= *(CNAEntry**)elem1;
	CNAEntry	*e2= *(CNAEntry**)elem2;

	// compare key values
	int	keyLen= (e1->m_keyLen < e2->m_keyLen)? e1->m_keyLen: e2->m_keyLen;

//	keyLen /=2;
	unsigned char	*key1= (unsigned char *)e1->m_key;
	unsigned char	*key2= (unsigned char *)e2->m_key;

	for(int i=0;i<keyLen; i++){
		if(key1[i] != key2[i])
			return (int)(key1[i] - key2[i]);
	}

	// compare key length, since their key values are same to each other
	return e1->m_keyLen - e2->m_keyLen;
}

void	CNAEntry::print()
{
	//printf("<%s>:%d\n", m_key, m_numberBitVectors);
	printf("<%s>\n", m_key);
	for(int i=0;i<m_numberBitVectors; i++)
		printf("%s\n", m_bitVectors[i]);
}

/*
	copy an entry and return it
*/
CNAEntry	*CNAEntry::clone()
{
	char		buf[4096];
	CNAEntry	*entry= new CNAEntry();
	for(int i=0;i<this->getNumberBitVectors();i++){
		char *xx= this->getString(i, buf);
		entry->setString(xx,0);
	}
	return entry;
}

int CNAEntry::SaveDic(FILE *fpout)
{
	if (!fpout) return 0;
			// WRITE HEADER
	fwrite((void *)&m_keyLen, sizeof(int), 1, fpout);
	fwrite((void *)&m_numberBitVectors, sizeof(int), 1 , fpout);
	//fwrite((void *)&m_sizeBitVectors, sizeof(int), 1, fpout);
	//fwrite((void *)&n, sizeof(int), 1, fpout);	//임시

	// WRITE ENTRY
	Encoding(m_key);
	fwrite((void *)m_key, sizeof(char), m_keyLen, fpout);
	unsigned char *bit;
	bit = new unsigned char [MAX_FIELD_LEN/8];
	memset(bit, 0x00, MAX_FIELD_LEN/8);

	for(int i=0;i<m_numberBitVectors;i++){
		//Encoding(m_bitVectors[i]);
		str2bit(m_bitVectors[i], bit);
		fwrite((void *)bit, sizeof(char), MAX_FIELD_LEN/8, fpout);
	}
	
	delete[] bit;
	bit = NULL;
	
	return 1;
}

int CNAEntry::LoadDic(FILE *fpin)
{
	if (!fpin) return 0;
	
	fread((void *)&m_keyLen, sizeof(int), 1, fpin);
	fread((void *)&m_numberBitVectors, sizeof(int), 1, fpin);
	//fread((void *)&m_sizeBitVectors, sizeof(int), 1, fpin);
	m_sizeBitVectors = DEFAULT_BITVECTOR_SIZE;
	
	char* buf = new char[4096];
	memset(buf, 0x00, 4096);
	
	fread((void *)buf, sizeof(char), m_keyLen, fpin);
	buf[m_keyLen +1] = 0;
	if(m_key != NULL) delete[] m_key;
	m_key = new char[m_keyLen+1];
	memset(m_key, 0x00, m_keyLen+1);
	strcpy(m_key, buf);

	delete[] buf;
	buf = NULL;

	Decoding(m_key);
	for(int i=0;i<m_numberBitVectors;i++){
		unsigned char* s = new unsigned char[MAX_FIELD_LEN/8];
		memset(s, 0x00, MAX_FIELD_LEN/8);
		fread((void *)s, sizeof(char), MAX_FIELD_LEN/8, fpin);

		char* n = new char[MAX_FIELD_LEN];
		memset(n, 0x00, MAX_FIELD_LEN);
		bit2str(s, (char *)n);

		int k =0; 
		while(n[k] == '0' || n[k] == '1') k++;
		n[k+1] = 0;
 		
		char *p= new char[k+1];
		memset(p, 0x00, k+1);
		strcpy(p, n);
		//Decoding(p);
		m_bitVectors[i] = p;

		delete[] n;
		n = NULL;
		
		delete[] s;
		s = NULL;
	}
	return 1;
}

void CNAEntry::Encoding(char *stem) {
	char *p = stem;
	int leng = strlen(p);

	for (int i=0; i<leng; i++) {
		*p = (*p ^ ENCODE_BIT);
		p++;
	}
}

void CNAEntry::Decoding(char *stem) {
	Encoding(stem);
}

//
// record의 값을 unsigned char *bit로 바꾼다. 
// 8bytes의 내용을 1byte의 내용으로 변경.
////////////////////////////////////////////////////////////////////
void CNAEntry::str2bit(char *rbuf, unsigned char *bit) {
	char *p = rbuf;
	int byteoff, bitoff;
	int value;

	while(GetBitOff((p-rbuf), &byteoff, &bitoff) < RECORD) {
		value = *p ++ - '0'; //value == 0 or value == 1
		*(bit + byteoff) |= (value << bitoff);
	}
}

//
// 1byte의 값을 8bytes로 변경한다.
////////////////////////////////////////////////////////////////////
void CNAEntry::bit2str(unsigned char *bit, char *rbuf) {
	char *p = rbuf;
	unsigned char *q = bit;
	int byteoff, bitoff;
	int value;

	while(GetBitOff((p-rbuf), &byteoff, &bitoff) < RECORD) {
		value = ((*(q + byteoff) >> bitoff) & 0x00000001) + '0';
		*p++ = value;
	}
}

//
// idx의 값을 (74 필드) byteoff, bitoff로 구분한다.
////////////////////////////////////////////////////////////////////
int CNAEntry::GetBitOff(int idx, int *byteoff, int *bitoff) {
	*byteoff = idx / 8;
	*bitoff = idx % 8;
	return idx;
}


int	CNAEntry::SetBitVector(int index, char* bitVector)
{
	if (index < 0 || index >= m_numberBitVectors) {
		// this is invalid index
		return 0;
	}

	//2017-05-18
	//기존에 자질 정보가 존재하는 경우 해당 배열을 삭제하고 다시 만든다.
	if (m_bitVectors[index] != NULL) {
		delete m_bitVectors[index];
	}

	int i;
	char	*p = new char[strlen(bitVector) + 1];
	for (i = 0; bitVector[i] != 0; i++) {
		p[i] = bitVector[i];
	}
	p[i] = 0;

	m_bitVectors[index] = p;

	return 1;
}