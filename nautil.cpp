#include	"nautil.h"


int	CNAByteCmp(char *sr1, char *sr2)
{
	register	unsigned char	*s1= (unsigned char	*)sr1;
	register	unsigned char	*s2= (unsigned char	*)sr2;

	for(;*s1 != 0 && *s2 != 0; s1++, s2++){
		if(*s1 - *s2 != 0)	return (int)(*s1-*s2);
	}
	return (int)(*s1-*s2);
}
