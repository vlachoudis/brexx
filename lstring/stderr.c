/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/stderr.c,v 1.1 1998/07/02 17:18:00 bnv Exp $
 * $Log: stderr.c,v $
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <lstring.h>

/* ------------------ Lstderr ------------------- */
void
Lstderr( const int errno, const int subno, ... ) 
{
	Lstr	errmsg;
	va_list	ap;

	LINITSTR(errmsg);

	va_start(ap,subno);
	Lerrortext(&errmsg,errno,subno,ap);
	va_end(ap);

	if (LLEN(errmsg)==0)
		fprintf(stderr,"Ooops unknown error %d.%d!!!\n",errno,subno);
	else {
		LASCIIZ(errmsg);
		if (subno==0)
			fprintf(stderr,"Error %d: %s\n",errno,LSTR(errmsg));
		else 
			fprintf(stderr,"Error %d.%d: %s\n",errno,subno,LSTR(errmsg));
	}

	LFREESTR(errmsg);
	exit(errno);
} /* Lstderr */
