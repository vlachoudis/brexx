/*
 * $Id: main.c,v 1.7 2002/06/11 12:37:38 bnv Exp $
 * $Log: main.c,v $
 * Revision 1.7  2002/06/11 12:37:38  bnv
 * Added: CDECL
 *
 * Revision 1.6  2002/06/06 08:24:02  bnv
 * Corrected: READLINE support when using redirected input
 *
 * Revision 1.5  2001/06/25 18:51:48  bnv
 * Header -> Id
 *
 * Revision 1.4  1999/11/26 13:13:47  bnv
 * Added: A filter option.
 *
 * Revision 1.3  1999/05/26 16:48:06  bnv
 * Gene corrections in RXCONIO
 *
 * Revision 1.2  1999/02/10 15:43:16  bnv
 * Additions from Generoso Martello
 *
 * Revision 1.1  1998/07/02 17:34:50  bnv
 * Initial revision
 *
 */
#include <stdio.h>
#include <string.h>
#include <lstring.h>

#include <rexx.h>
#include <rxdefs.h>
#include <sys/stat.h>

/* ------- Includes for any other external library ------- */
#ifdef RXCONIO
extern void __CDECL RxConIOInitialize();
#endif
#ifdef RXMYSQL
extern void __CDECL RxMySQLInitialize();
extern void __CDECL RxMySQLFinalize();
#endif

/* --------------------- main ---------------------- */
int __CDECL
main(int ac, char *av[])
{
	Lstr	args, tracestr, file;
	int	ia,ir;
	bool	input, loop_over_stdin;
#if defined(USE_READLINE)
	Lstr	line;
	LINITSTR(line);
#endif

	input = loop_over_stdin = FALSE;
	LINITSTR(args);
	LINITSTR(tracestr);
	LINITSTR(file);

	if (ac<2) {
		puts("\nsyntax: rexx [-[trace]|-F] <filename> <args>...\n\trexx -\tto use stdin\n\trexx -F\tloop over standard input\n\t\t\'linein\' contains each line from stdin.\n");
		puts(VERSION);
		puts("Author: "AUTHOR);
		puts("Please report bugs, errors or comments to the above address.\n");
		return 0;
	}
#ifdef __DEBUG__
	__debug__ = FALSE;
#endif

	/* --- Initialise --- */
	RxInitialize(av[0]);

	/* --- Register functions of external libraries --- */
#ifdef RXCONIO
	RxConIOInitialize();
#endif
#ifdef RXMYSQL
	RxMySQLInitialize();
#endif


	/* --- scan arguments --- */
	ia = 1;
	if (av[ia][0]=='-') {
		if (av[ia][1]==0)
			input = TRUE;
		else
		if (av[ia][1]=='F')
			loop_over_stdin = input = TRUE;
		else
			Lscpy(&tracestr,av[ia]+1);
		ia++;
	} else
	if (av[ia][0]=='?' || av[ia][0]=='!') {
		Lscpy(&tracestr,av[ia]);
		ia++;
	}

	/* --- let's read a normal file --- */
	if (!input && ia<ac) {
		/* prepare arguments for program */
		for (ir=ia+1; ir<ac; ir++) {
			Lcat(&args,av[ir]);
			if (ir<ac-1) Lcat(&args," ");
		}
		RxRun(av[ia],NULL,&args,&tracestr,NULL);
	} else {
		if (ia>=ac) {
#if !defined(USE_READLINE)
			Lread(STDIN,&file,LREADFILE);
#else
			struct stat buf;
			fstat(0,&buf);
			if (S_ISCHR(buf.st_mode)) {
				printf("End with \";\" on a line by itself.\n");
				while (1) {
					Lread(STDIN,&line,LREADLINE);
					if (!Lcmp(&line,";"))
						break;
					Lstrcat(&file,&line);
					Lcat(&file,"\n");
				}
			} else
				Lread(STDIN,&file,LREADFILE);
#endif
		} else {
			/* Copy a small header */
			if (loop_over_stdin)
				Lcat(&file,"do forever;"
					"linein=read();"
					"if eof(0) then exit;");
			for (;ia<ac; ia++) {
				Lcat(&file,av[ia]);
				if (ia<ac-1) Lcat(&file," ");
			}
			/* and a footer */
			if (loop_over_stdin)
				Lcat(&file,";end");
		}
		RxRun(NULL,&file,&args,&tracestr,NULL);
	}

	/* --- Free everything --- */
	RxFinalize();
	LFREESTR(args);
	LFREESTR(tracestr);
	LFREESTR(file);
#if defined(USE_READLINE)
	LFREESTR(line);
#endif
#ifdef RXMYSQL
	RxMySQLFinalize();
#endif

#ifdef __DEBUG__
	if (mem_allocated()!=0) {
		fprintf(STDERR,"\nMemory left allocated: %ld\n",mem_allocated());
		mem_list();
	}
#endif

	return RxReturnCode;
} /* main */
