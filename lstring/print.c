/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/print.c,v 1.4 1999/05/26 16:47:42 bnv Exp $
 * $Log: print.c,v $
 * Revision 1.4  1999/05/26 16:47:42  bnv
 * Gene corrections in RXCONIO
 *
 * Revision 1.3  1999/03/10 16:55:55  bnv
 * Correction for RXCONIO
 *
 * Revision 1.2  1999/02/10 15:45:16  bnv
 * RXCONIO support by Generoso Martello
 *
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <stdio.h>
#include <lstring.h>

#ifdef RXCONIO
#define ANSI_FPRINTF(fd,fs,ff)		if (fd==stdout) cprintf(fs,ff); else fprintf(fd,fs,ff)
#else
#define ANSI_FPRINTF(fd,fs,ff)		fprintf(fd,fs,ff)
#endif

/* ---------------- Lprint ------------------- */
void
Lprint( FILE *f, const PLstr str )
{
	size_t	l;
	char	*c;

	if (str==NULL) {
		ANSI_FPRINTF(f,"<NULL>",NULL);
		return;
	}

	switch (LTYPE(*str)) {
		case LSTRING_TY:
			c = LSTR(*str);
			for (l=0; l<LLEN(*str); l++)
#if defined(RXCONIO)
				if (f==stdout) {
					putch(*c++);
				} else
#endif
				fputc(*c++,f);
			break;

		case LINTEGER_TY:
			ANSI_FPRINTF(f,"%ld", LINT(*str));
			break;

		case LREAL_TY:
			ANSI_FPRINTF(f, formatStringToReal, LREAL(*str));
			break;
	}
} /* Lprint */
