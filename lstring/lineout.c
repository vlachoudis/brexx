/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/lineout.c,v 1.2 1999/03/15 15:25:53 bnv Exp $
 * $Log: lineout.c,v $
 * Revision 1.2  1999/03/15 15:25:53  bnv
 * Corrected: initial value to prev
 *
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <stdio.h>
#include <lstring.h>

/* ---------------- Llineout ------------------- */
int
Llineout( FILE *f, const PLstr line, long *curline, long start )
{
	int	ch,prev='\n';

	/* find current line */
	if (start>=0) {
		if (*curline>start) {
			*curline = 1;
			fseek(f,0,SEEK_SET);
		}
		while (start>*curline) {
			ch = fgetc(f);
			if (ch==-1) {
				if (prev!='\n') (*curline)++;
				break;
			}
			if (ch=='\n') (*curline)++;
			prev = ch;
		}
		while (start > *curline) {
			fputc('\n',f);
			(*curline)++;
		}
	}

	Lwrite(f,line,TRUE);
	(*curline)++;
	return 0;		/* if everything ok */
} /* Llineout */
