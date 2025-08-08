#ifndef	NAENCDICTIONARY_H
#define	NAENCDICTIONARY_H


#include	"nadictionary.h"

class	CNAEncDictionary: public CNADictionary
{
public:
	virtual	int	readLine(FILE *fp, char *buf, int len);
	virtual	int	writeLine(FILE *fp, char *buf, int len);
};

#endif