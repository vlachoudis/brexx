/*
 * $Id: read.c,v 1.11 2008/07/14 13:08:16 bnv Exp $
 * $Log: read.c,v $
 * Revision 1.11  2008/07/14 13:08:16  bnv
 * MVS,CMS support
 *
 * Revision 1.10  2004/07/22 13:00:17  bnv
 * Corrected: Reading from STDIN with readline when operating on a device
 *
 * Revision 1.9  2004/03/26 22:52:26  bnv
 * Added FIFO support
 *
 * Revision 1.8  2003/02/26 16:29:24  bnv
 * Changed: READLINE definitions
 *
 * Revision 1.7  2002/06/11 12:37:15  bnv
 * Added: CDECL
 *
 * Revision 1.6  2002/06/06 08:21:55  bnv
 * Changed: Directive for READLINE
 *
 * Revision 1.5  2001/07/20 14:28:37  bnv
 * Corrected: When pipeing the readline was echoing the line
 *
 * Revision 1.5  2001/07/19 16:33:43  bnv
 * Corrected the readline when a file was redirected
 *
 * Revision 1.4  2001/06/25 18:49:48  bnv
 * Header changed to Id
 *
 * Revision 1.3  2000/04/07 07:11:01  bnv
 * Corrected: When trying to read a complete file from STDIN, it was reporting
 * size=1, where it should be negative (unknown)
 *
 * Revision 1.2  1999/11/26 12:52:25  bnv
 * Added: Windows CE support
 *
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <lstring.h>

#ifdef HAVE_LIBREADLINE
#	include <sys/stat.h>
#	if defined(HAVE_READLINE_READLINE_H)
#		include <readline/readline.h>
#	elif defined(HAVE_READLINE_H)
#		include <readline.h>
#	else /* !defined(HAVE_READLINE_H) */
		extern char *readline ();
#	endif /* !defined(HAVE_READLINE_H) */
#else /* !defined(HAVE_READLINE_READLINE_H) */
	/* no readline */
#endif /* HAVE_LIBREADLINE */

#ifdef HAVE_READLINE_HISTORY
#	if defined(HAVE_READLINE_HISTORY_H)
#		include <readline/history.h>
#	elif defined(HAVE_HISTORY_H)
#		include <history.h>
#	else /* !defined(HAVE_HISTORY_H) */
		extern void add_history ();
		extern int write_history ();
		extern int read_history ();
#	endif /* defined(HAVE_READLINE_HISTORY_H) */
		/* no history */
#endif /* HAVE_READLINE_HISTORY */

/* ---------------- Lread ------------------- */
void __CDECL
Lread( FILEP f, const PLstr line, long size )
{
	long	l;
	char	*c;
	int	ci;

	/* We use the fgetc and not the fread to get rid of the 0x0D */
	if (size>0) {
		Lfx(line,(size_t)size);
		c = LSTR(*line);
		for (l=0; l<size; l++) {
			ci = FGETC(f);
			if (ci==-1)
				break;
			*c++ = ci;
		}
	} else
	if (size==0) {			/* Read a single line */
#ifdef HAVE_LIBREADLINE
		if (f==STDIN) {
			struct stat buf;
			fstat(0,&buf);
			if (S_ISCHR(buf.st_mode) || S_ISBLK(buf.st_mode)) {
				char *str = readline(NULL);
#ifdef HAVE_READLINE_HISTORY
				if (str && *str)
					add_history(str);
#endif
				Lscpy(line,str);
				free(str);
				return;
			}
		}
#endif
		Lfx(line,LREADINCSIZE);
		l = 0;
		while ((ci=FGETC(f))!='\n') {
			if (ci==EOF) break;
			c = LSTR(*line) + l;
			*c = ci;
			if (++l >= LMAXLEN(*line))
				Lfx(line, (size_t)l+LREADINCSIZE);
		}
	} else {			/* Read entire file */
#ifndef WCE
#	if defined(JCC) || defined(__CMS__) || defined(__MVS__)
		size = 0; /* Always do it the slow way: so no-seek (JCL inline) files work. */
		/*   printf(" JCC Rules invoked \n");   dw*/
#	else
		l = FTELL(f);
		if (l>=0) {
			FSEEK(f,0L,SEEK_END);
			size = FTELL(f) - l + 1;
			FSEEK(f,l,SEEK_SET);
		}
#endif
#elif defined(__BORLANDC__)
		l = FTELL(f);
                size = FSEEK(f,0L,SEEK_END) - l + 1;
		FSEEK(f,l,SEEK_SET);
#else
		size = GetFileSize(f->handle,NULL) - FTELL(f) + 1;
#endif
		if (size>0) {
			Lfx(line,(size_t)size);
			c = LSTR(*line);
			l = 0;
			while (1) {
				int ch = FGETC(f);
				if (ch==EOF) break;
				*c++ = ch;
				l++;
			}
			/*??? if (*c=='\n') l--; // If it is binary then wrong! */
		}
#ifndef WCE
		else {	/* probably STDIN */
			Lfx(line,LREADINCSIZE);
			l = 0;
			while ((ci=FGETC(f))!=EOF) {
				c = LSTR(*line) + l;
				*c = ci;
				if (++l >= LMAXLEN(*line))
					Lfx(line, (size_t)l+LREADINCSIZE);
			}
		}
#endif
	}
	LLEN(*line) = l;
	LTYPE(*line) = LSTRING_TY;
#if defined(JCC) || defined(__CMS__) || defined(__MVS__)
	LASCIIZ(*line);
#endif
} /* Lread */
