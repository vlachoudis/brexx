/*
 * $Id: extra.c,v 1.2 2017/01/12 11:09:28 bnv Exp $
 * $Log: extra.c,v $
 * Revision 1.2  2017/01/12 11:09:28  bnv
 * Updated gcvt
 *
 * Revision 1.1  2008/07/16 08:01:45  bnv
 * Initial revision
 *
 */

#include <stdio.h>
#include <stdlib.h>

/* -------------- putenv ---------------- */
int
putenv(char *envval){
	printf(" Env String = %s\n",envval);
	return 0;
} /* putenv */

/* -------------- gcvt ---------------- */
char *
gcvt(double num, size_t ndigits, char *result)
{
    sprintf(result, "%.*g", ndigits, num);
    return (result);
} /* gcvt */
