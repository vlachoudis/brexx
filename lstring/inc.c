/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/inc.c,v 1.1 1998/07/02 17:18:00 bnv Exp $
 * $Log: inc.c,v $
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
