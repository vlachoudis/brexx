/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/bool.c,v 1.1 1998/07/02 17:16:35 bnv Exp $
 * $Log: bool.c,v $
 * Revision 1.1  1998/07/02 17:16:35  bnv
 * Initial revision
 *
 */

#include <lerror.h>
#include <lstring.h>

/* ------------------- Lbool ------------------ */
int
Lbool( const PLstr num )
{
	int	t;
	long	i;
	double	r;

	switch (LTYPE(*num)) {
		case LSTRING_TY:
			t = _Lisnum(num);
			if (t==LSTRING_TY) Lerror(ERR_UNLOGICAL_VALUE,0);
			r = strtod(LSTR(*num),NULL);
			i = (long)r;
			if ((double)i != r)
				Lerror(ERR_UNLOGICAL_VALUE,0);
			break;
		case LREAL_TY:
			i = LREAL(*num);
			if ((double)i != LREAL(*num))
				Lerror(ERR_UNLOGICAL_VALUE,0);
			break;
		case LINTEGER_TY:
			i = LINT(*num);
			break;
	}
	if (i == 0 || i == 1) return i;
	Lerror(ERR_UNLOGICAL_VALUE,0);
	return -1;             /* Never gets here but keep compiler happy :-) */
} /* Lbool */
