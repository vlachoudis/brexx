/*
 * $Id: inc.c,v 1.2 2001/06/25 18:49:48 bnv Exp $
 * $Log: inc.c,v $
 * Revision 1.2  2001/06/25 18:49:48  bnv
 * Header changed to Id
 *
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <lstring.h>

/* ------------------- Linc ------------------ */
void
Linc( const PLstr num )
{
	L2NUM(num);

	if (LTYPE(*num)==LINTEGER_TY)
		LINT(*num) += 1;
	else
		LREAL(*num) += 1.0;
} /* Linc */
