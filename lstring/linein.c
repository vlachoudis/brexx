/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/linein.c,v 1.1 1998/07/02 17:18:00 bnv Exp $
 * $Log: linein.c,v $
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <stdio.h>
#include <lstring.h>

/* ---------------- Llinein ------------------- */
void
Llinein( FILE *f, const PLstr line, long *curline, long start, long length )
{
	int	ch,prev;
	Lstr	aux;

	/* initialise line */
	LZEROSTR(*line);

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
		if (start > *curline) return;
	}

	if (length<=0) return;

	if (length==1) {
		Lread(f,line,LREADLINE);
		(*curline)++;
	} else {
		LINITSTR(aux);
		while (length) {
			Lread(f,&aux,LREADLINE);
			Lstrcat(line,&aux);
			if (length>1)
				Lcat(line,"\n");
			(*curline)++;
			length--;
		}
		LFREESTR(aux);
	}
} /* Llinein */
