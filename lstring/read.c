/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/read.c,v 1.3 2000/04/07 07:11:01 bnv Exp $
 * $Log: read.c,v $
 * Revision 1.3  2000/04/07 07:11:01  bnv
 * Corrected: When trying to read a complete file from STDIN, it was reporting
 * size=1, where it should be negative (unknown)
 *
 * Revision 1.2  1999/11/26 12:52:25  bnv
 * Added: Windows CE support
 *
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <lstring.h>

/* ---------------- Lread ------------------- */
void
Lread( FILEP f, const PLstr line, long size )
{
	long	l;
	char	*c;
	int	ci;

	/* We use the fgetc and not the fread to get rid of the 0x0D */
	if (size>0) {
		Lfx(line,(size_t)size);
		c = LSTR(*line);
		for (l=0; l<size; l++) {
			ci = FGETC(f);
			if (ci==-1)
				break;
			*c++ = ci;
		}
	} else
	if (size==0) {			/* Read a single line */
		Lfx(line,LREADINCSIZE);
		l = 0;
		while ((ci=FGETC(f))!='\n') {
			if (ci==EOF) break;
			c = LSTR(*line) + l;
			*c = ci;
			if (++l >= LMAXLEN(*line))
				Lfx(line, (size_t)l+LREADINCSIZE);
		}
	} else {			/* Read entire file */
#ifndef WCE
		l = FTELL(f);
		if (l>=0) {
			FSEEK(f,0L,SEEK_END);
			size = FTELL(f) - l + 1;
			FSEEK(f,l,SEEK_SET);
		}
#elif defined(__BORLANDC__)
		l = FTELL(f);
                size = FSEEK(f,0L,SEEK_END) - l + 1;
		FSEEK(f,l,SEEK_SET);
#else
		size = GetFileSize(f->handle,NULL) - FTELL(f) + 1;
#endif
		if (size>0) { 
			Lfx(line,(size_t)size);
			c = LSTR(*line);
			l = 0;
			while (1) {
				int ch = FGETC(f);
				if (ch==EOF) break;
				*c++ = ch;
				l++;
			}
			/*??? if (*c=='\n') l--; // If it is binary then wrong! */
		}
#ifndef WCE
		else {	/* probably STDIN */
			Lfx(line,LREADINCSIZE);
			l = 0;
			while ((ci=FGETC(f))!=EOF) {
				c = LSTR(*line) + l;
				*c = ci;
				if (++l >= LMAXLEN(*line))
					Lfx(line, (size_t)l+LREADINCSIZE);
			}
		}
#endif
	}
	LLEN(*line) = l;
	LTYPE(*line) = LSTRING_TY;
} /* Lread */
