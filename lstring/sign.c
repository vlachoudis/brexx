/*
 * $Id: sign.c,v 1.2 2001/06/25 18:49:48 bnv Exp $
 * $Log: sign.c,v $
 * Revision 1.2  2001/06/25 18:49:48  bnv
 * Header changed to Id
 *
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <lstring.h>

/* ------------------ Lsign --------------------- */
int
Lsign( const PLstr num )
{
	L2NUM(num);

	switch (LTYPE(*num)) {
		case LINTEGER_TY:
			if (LINT(*num)<0)
				return -1;
			else
			if (LINT(*num)>0)
				return  1;
			else
				return  0;

		case LREAL_TY:
			if (LREAL(*num)<0)
				return -1;
			else
			if (LREAL(*num)>0)
				return  1;
			else
				return  0;
	}
	return 0;
} /* Lsign */
