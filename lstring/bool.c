/*
 * $Id: bool.c,v 1.4 2002/06/11 12:37:15 bnv Exp $
 * $Log: bool.c,v $
 * Revision 1.4  2002/06/11 12:37:15  bnv
 * Added: CDECL
 *
 * Revision 1.3  2001/06/25 18:49:48  bnv
 * Header changed to Id
 *
 * Revision 1.2  1999/11/26 09:52:45  bnv
 * Changed: To make use of the LastScannedNumber
 *
 * Revision 1.1  1998/07/02 17:16:35  bnv
 * Initial revision
 *
 */

#include <lerror.h>
#include <lstring.h>

/* ------------------- Lbool ------------------ */
int __CDECL
Lbool( const PLstr num )
{
	long	i;

	switch (LTYPE(*num)) {
		case LSTRING_TY:
			if (_Lisnum(num)==LSTRING_TY)
				Lerror(ERR_UNLOGICAL_VALUE,0);
			i = (long)lLastScannedNumber;
			if ((double)i != lLastScannedNumber)
				Lerror(ERR_UNLOGICAL_VALUE,0);
			break;
		case LREAL_TY:
			i = (long)LREAL(*num);
			if ((double)i != LREAL(*num))
				Lerror(ERR_UNLOGICAL_VALUE,0);
			break;
		case LINTEGER_TY:
			i = LINT(*num);
			break;
	}
	if (i == 0 || i == 1) return i;
	Lerror(ERR_UNLOGICAL_VALUE,0);
	return -1;	/* Never gets here but keep compiler happy :-) */
} /* Lbool */
