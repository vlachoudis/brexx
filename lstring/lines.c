/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/lines.c,v 1.1 1998/07/02 17:18:00 bnv Exp $
 * $Log: lines.c,v $
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <stdio.h>
#include <lstring.h>

/* ---------------- Llines ------------------- */
long
Llines( FILE *f )
{
	long	pos,l;
	int	ch,prev;

	pos = ftell(f);		/* read current position */
	l = 0;
	prev = ' ';
	while (1) {
		ch = fgetc(f);
		if (ch==-1) {
			if (prev!='\n') l++;
			break;
		}
		if (ch=='\n') l++;
		prev = ch;
	}
	fseek(f,pos,SEEK_SET);
	return l;
} /* Llines */
