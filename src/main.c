/*
 * $Header: /home/bnv/tmp/brexx/src/RCS/main.c,v 1.4 1999/11/26 13:13:47 bnv Exp $
 * $Log: main.c,v $
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

/* ------- Includes for any other external library ------- */
#ifdef RXCONIO
extern RxConIOInitialize();
#endif

/* --------------------- main ---------------------- */
int
main(int ac, char *av[])
{
	Lstr	args, tracestr, file;
	int	ia,ir;
	bool	input, loop_over_stdin;

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
		if (ia>=ac)
			Lread(STDIN,&file,LREADFILE);
		else {
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

#ifdef __DEBUG__
	if (mem_allocated()!=0) {
		fprintf(STDERR,"\nMemory left allocated: %ld\n",mem_allocated());
		mem_list();
	}
#endif

	return RxReturnCode;
} /* main */
