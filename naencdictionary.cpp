
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
#include	"naencdictionary.h"

#pragma warning(disable:4996)



int	CNAEncDictionary::readLine(FILE *fp, char *buf, int len)
{
	if(fread(&len, 1, sizeof(int), fp)<sizeof(int))	return -1;
	if((int)fread(buf, 1, len, fp) < len)	return -1;
	for (int i = 0; i < len; i++) {
		buf[i] = ~buf[i];
		buf[i] = 0;
	}
	return 0;
}

int	CNAEncDictionary::writeLine(FILE *fp, char *buf, int len)
{
	for(int i=0;i<len;i++)
		buf[i]= ~buf[i];
	fwrite(&len, 1, sizeof(int), fp);
	fwrite(buf, 1, len, fp);
	return 0;
}

