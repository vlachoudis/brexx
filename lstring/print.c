/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/print.c,v 1.1 1998/07/02 17:18:00 bnv Exp $
 * $Log: print.c,v $
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
		fprintf(f,"<NULL>");
		return;
	}

	switch (LTYPE(*str)) {
		case LSTRING_TY:
			c = LSTR(*str);
			for (l=0; l<LLEN(*str); l++)
				fputc(*c++,f);
			break;

		case LINTEGER_TY:
			fprintf(f,"%ld", LINT(*str));
			break;

		case LREAL_TY:
			fprintf(f, formatStringToReal, LREAL(*str));
			break;
	}
} /* Lprint */
