/*
 * $Id: librxunix.c,v 1.1 2002/08/22 12:27:09 bnv Exp $
 * $Log: librxunix.c,v $
 * Revision 1.1  2002/08/22 12:27:09  bnv
 * Initial revision
 *
 */

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

#include <lerror.h>
#include <lstring.h>

#include <rexx.h>
#include <rxdefs.h>

/* --------------------------------------------------------------- */
/*  CHDIR([directory])                                             */
/*     with no arguments returns current directory                 */
/*     otherwise changes the default directory                     */
/* --------------------------------------------------------------- */
void __CDECL
R_chdir( const int func )
{
	if (ARGN>1) Lerror(ERR_INCORRECT_CALL,0);
	Lfx(ARGR,PATH_MAX);
	Lscpy(ARGR,getcwd(LSTR(*ARGR),LMAXLEN(*ARGR)));
	if (ARGN==1) {
		int	err;
		L2STR(ARG1);
		LASCIIZ(*ARG1);
		err = chdir(LSTR(*ARG1));
	}
} /* R_chdir */

/* --- Register functions --- */
void __CDECL
RxUnixInitialize()
{
	RxRegFunction("CHDIR",	R_chdir,  0);
}
