/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/read.c,v 1.1 1998/07/02 17:18:00 bnv Exp $
 * $Log: read.c,v $
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <stdio.h>
#include <lstring.h>

/* ---------------- Lread ------------------- */
void
Lread( FILE *f, const PLstr line, long size )
{
	long	l;
	char	*c;
	int	ci;

	if (size>0) {
		Lfx(line,(size_t)size);
		c = LSTR(*line);
		for (l=0; l<size; l++) {
			ci = fgetc(f);
			if (ci==-1)
				break;
			*c++ = ci;
		}
	} else
	if (size==0) {			/* Read a single line */
		Lfx(line,LREADINCSIZE);
		l = 0;
		while ((ci=fgetc(f))!='\n') {
			if (ci==EOF) break;
			c = LSTR(*line) + l;
			*c = ci;
			if (++l >= LMAXLEN(*line))
				Lfx(line, (size_t)l+LREADINCSIZE);
		}
	} else {			/* Read entire file */
		l = ftell(f);
		fseek(f,0L,SEEK_END);
		size = ftell(f) - l;
		fseek(f,l,SEEK_SET);
		if (size>0) { 
			Lfx(line,(size_t)size);
			c = LSTR(*line);
			l = 0;
			while (!feof(f)) {
				*c++ = fgetc(f);
				l++;
			}
			l--;c--;
			if (*c=='\n') l--;
		} else {	/* probably STDIN */
			Lfx(line,LREADINCSIZE);
			l = 0;
			while ((ci=fgetc(f))!=EOF) {
				c = LSTR(*line) + l;
				*c = ci;
				if (++l >= LMAXLEN(*line))
					Lfx(line, (size_t)l+LREADINCSIZE);
			}
		}
	}
	LLEN(*line) = l;
	LTYPE(*line) = LSTRING_TY;
} /* Lread */
