/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/print.c,v 1.2 1999/02/10 15:45:16 bnv Exp $
 * $Log: print.c,v $
 * Revision 1.2  1999/02/10 15:45:16  bnv
 * RXANSI support by Generoso Martello
 *
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <stdio.h>
#include <lstring.h>

/* ---------------- Lprint ------------------- */
void
Lprint( FILE *f, const PLstr str )
{
	size_t	l;
	char	*c;

	if (str==NULL) {
#ifdef RXANSI
		if (f==stdout) cprintf("<NULL>");
		else
#endif
		fprintf(f,"<NULL>");
		return;
	}

	switch (LTYPE(*str)) {
		case LSTRING_TY:
			c = LSTR(*str);
			for (l=0; l<LLEN(*str); l++)
#ifdef RXANSI
				if (f==stdout) putch(*c++);
				else
#endif
				fputc(*c++,f);
			break;

		case LINTEGER_TY:
#ifdef RXANSI
			if (f==stdout) cprintf("%ld", LINT(*str));
			else
#endif
			fprintf(f,"%ld", LINT(*str));
			break;

		case LREAL_TY:
#ifdef RXANSI
			if (f==stdout) cprintf(formatStringToReal, LREAL(*str));
			else
#endif
			fprintf(f, formatStringToReal, LREAL(*str));
			break;
	}
} /* Lprint */
