/*
 * $Id: stderr.c,v 1.4 2002/06/11 12:37:15 bnv Exp $
 * $Log: stderr.c,v $
 * Revision 1.4  2002/06/11 12:37:15  bnv
 * Added: CDECL
 *
 * Revision 1.3  2001/06/25 18:49:48  bnv
 * Header changed to Id
 *
 * Revision 1.2  1999/11/26 12:52:25  bnv
 * Changed: To use new macros
 *
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <lstring.h>

#ifdef WCE
#	error "Lstderr: should not be included in the CE version"
#endif
/* ------------------ Lstderr ------------------- */
void __CDECL
Lstderr( const int errno, const int subno, ... ) 
{
	Lstr	errmsg;
	va_list	ap;

	LINITSTR(errmsg);

	va_start(ap,subno);
	Lerrortext(&errmsg,errno,subno,&ap);
	va_end(ap);

	if (LLEN(errmsg)==0)
		fprintf(STDERR,"Ooops unknown error %d.%d!!!\n",errno,subno);
	else {
		LASCIIZ(errmsg);
		if (subno==0)
			fprintf(STDERR,"Error %d: %s\n",errno,LSTR(errmsg));
		else 
			fprintf(STDERR,"Error %d.%d: %s\n",errno,subno,LSTR(errmsg));
	}

	LFREESTR(errmsg);
	exit(errno);
} /* Lstderr */
