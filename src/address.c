/*
 * $Header: /home/bnv/tmp/brexx/src/RCS/address.c,v 1.1 1998/07/02 17:34:50 bnv Exp $
 * $Log: address.c,v $
 * Revision 1.1  1998/07/02 17:34:50  bnv
 * Initial revision
 *
 */

#include <bnv.h>
#include <string.h>
#include <stdlib.h>
#include <lstring.h>

#include <rexx.h>
#include <trace.h>
#include <stack.h>
#include <compile.h>
#include <interpre.h>

#if defined(MSDOS) || defined(__WIN32__)
#	include <io.h>
#	include <dir.h>
#	include <process.h>
#	if defined(__BORLANDC__) && !defined(__WIN32__)
#		include <systemx.h>
#	endif
#elif defined(__MPW__)
#else
#	include <unistd.h>
#endif

#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#ifndef S_IREAD
#	define S_IREAD 0
#	define S_IWRITE 1
#endif

#define NOSTACK		0
#define FIFO		1
#define LIFO		2
#define STACK		3

#define STDIN		0
#define STDOUT		1

/* ---------------------- chkcmd4stack ---------------------- */
static void
chkcmd4stack(PLstr cmd, int *in, int *out )
{
	Lstr Ucmd;

	*in = *out = 0;
	if (LLEN(*cmd)<7) return;

	LINITSTR(Ucmd);

	/* Search for string "STACK>" in front of command
	or for strings    "(STACK", "(FIFO", "(LIFO"
	                  ">STACK", ">FIFO", ">LIFO" at the end */

	if (LLEN(*cmd)<=5) return;

	Lstrcpy(&Ucmd,cmd); Lupper(&Ucmd);

	if (!MEMCMP(LSTR(Ucmd),"STACK>",6)) *in=FIFO;
	if (!MEMCMP(LSTR(Ucmd)+LLEN(Ucmd)-5,"STACK",5)) *out = STACK;
	if (!MEMCMP(LSTR(Ucmd)+LLEN(Ucmd)-4,"FIFO",4)) *out = FIFO;
	if (!MEMCMP(LSTR(Ucmd)+LLEN(Ucmd)-4,"LIFO",4)) *out = LIFO;
	if (*out)
		if (LSTR(Ucmd)[LLEN(Ucmd)-((*out==STACK)?6:5)]!='(' &&
		    LSTR(Ucmd)[LLEN(Ucmd)-((*out==STACK)?6:5)]!='>')   *out = 0;
	LFREESTR(Ucmd);

	if (*in) {
		MEMMOVE(LSTR(*cmd),LSTR(*cmd)+6,LLEN(*cmd)-6);
		LLEN(*cmd) -= 6;
	}
	if (*out)
		LLEN(*cmd) -= (*out==STACK)?6:5;

	if (*out==STACK)
		*out = FIFO;
} /* chkcmd4stack */

/* ------------------ RxRedirectCmd ----------------- */
int
RxRedirectCmd(PLstr cmd, int in, int out, PLstr resultstr)
{
	char	fnin[250], fnout[250];
	int	old_stdin, old_stdout;
	int	filein, fileout;
	FILE	*f;
	int	l;
	char	*c;
	PLstr	str;

	/* --- redirect input --- */
	if (in) {
		fnin[0] = '\0'; c = getenv("TEMP");
		if (c) STRCPY(fnin,c);
		l = STRLEN(fnin);
		if (l)
			if (fnin[l-1] != FILESEP) {
				fnin[l] = FILESEP;
				fnin[l+1] = '\0';
			}
		STRCAT(fnin,"OXXXXXX");
		mktemp(fnin);

		if ((f=fopen(fnin,"w"))!=NULL) {
			while (StackQueued()>0) {
				str = PullFromStack();
				L2STR(str); LASCIIZ(*str);
				fputs(LSTR(*str),f); fputc('\n',f);
				LPFREE(str);
			}
			fclose(f);

			old_stdin = dup(STDIN);
			filein = open(fnin,S_IREAD);
			dup2(filein,STDIN);
			close(filein);
		} else
			in = FALSE;
	}

	/* --- redirect output --- */
	if (out) {
		fnout[0] = '\0'; c = getenv("TEMP");
		if (c) STRCPY(fnout,c);
		l = STRLEN(fnout);
		if (l)
			if (fnout[l-1] != FILESEP) {
				fnout[l] = FILESEP;
				fnout[l+1] = '\0';
			}
		STRCAT(fnout,"OXXXXXX");
		mktemp(fnout);
		old_stdout = dup(STDOUT);
		fileout = creat(fnout,S_IWRITE);
		dup2(fileout,STDOUT);
		close(fileout);
	}

	/* --- Execute the command --- */
	LASCIIZ(*cmd);
#if defined(__BORLANDC__) && !defined(__WIN32__)
	RxReturnCode = systemx(LSTR(*cmd));
#else
	RxReturnCode = system(LSTR(*cmd));
#endif

	/* --- restore input --- */
	if (in) {
		close(STDIN);
		dup2(old_stdin,STDIN);
		close(old_stdin);
		remove(fnin);
	}

	/* --- restore output --- */
	if (out) {
		close(STDOUT);
		dup2(old_stdout,STDOUT);  /* restore stdout */
		close(old_stdout);
#ifndef MSDOS
		chmod(fnout,0666);
#endif
		if ((f=fopen(fnout,"r"))!=NULL) {
			if (resultstr) {
				Lread(f,resultstr,LREADFILE);
#if defined(RMLAST)
				if (LSTR(*resultstr)[LLEN(*resultstr)-1]=='\n')
					LLEN(*resultstr)--;
#endif
			} else	/* push it to stack */
				while (!feof(f)) {
					LPMALLOC(str);
					Lread(f,str,LREADLINE);
					if (LLEN(*str)==0 && feof(f)) {
						LPFREE(str);
						break;
					}
					if (out==FIFO)
						Queue2Stack(str);
					else
						Push2Stack(str);
				}

			fclose(f);
			remove(fnout); 
		}
	}

	return RxReturnCode;
} /* RxRedirectCmd */

/* ------------------ RxExecuteCmd ----------------- */
int
RxExecuteCmd( PLstr cmd, PLstr env )
{
	int	in,out;
	Lstr	cmdN;

	LINITSTR(cmdN); Lfx(&cmdN,1);
	Lstrcpy(&cmdN,cmd);
	L2STR(&cmdN);

	LASCIIZ(cmdN);
	if (env==NULL) {
		chkcmd4stack(&cmdN,&in,&out);
		RxReturnCode = RxRedirectCmd(&cmdN,in,out,FALSE);
	} else
	if (	!Lcmp(env,"COMMAND") ||
		!Lcmp(env,"DOS")     ||
		!Lcmp(env,"CMS")     ||
		!Lstrcmp(env,&(SystemStr->key))) {
			chkcmd4stack(&cmdN,&in,&out);
			RxReturnCode = RxRedirectCmd(&cmdN,in,out,FALSE);
	}
#if defined(__BORLANDC__) && !defined(__WIN32__)
	else
	if (!Lcmp(env,"INT2E"))
		int2e(LSTR(cmdN));
#endif
	else
		if (!Lcmp(env,"EXEC"))  ; /*execl(...); */
	else
		RxReturnCode = -3;

	/* free string */
	LFREESTR(cmdN);

	RxSetSpecialVar(RCVAR,RxReturnCode);
	if (RxReturnCode) {
		if (_Proc[_rx_proc].trace & (error_trace | normal_trace)) {
			TraceCurline(NULL,TRUE);
			fprintf(stderr,"       +++ RC(%d) +++\n",RxReturnCode);
			if (_Proc[_rx_proc].interactive_trace)
				TraceInteractive(FALSE);
		}
		if (_Proc[_rx_proc].condition & SC_ERROR)
			RxSignalCondition(SC_ERROR);
	}
	return RxReturnCode;
} /* RxExecuteCmd */
