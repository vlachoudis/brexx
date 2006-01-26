/*
 * $Id: address.c,v 1.12 2006/01/26 10:24:27 bnv Exp $
 * $Log: address.c,v $
 * Revision 1.12  2006/01/26 10:24:27  bnv
 * Spaces...
 *
 * Revision 1.11  2004/08/16 15:27:37  bnv
 * Include fcntl added
 *
 * Revision 1.9  2003/11/04 09:48:00  bnv
 * Corrected for the mkstemp
 *
 * Revision 1.8  2003/10/30 13:14:55  bnv
 * Some corrections
 *
 * Revision 1.7  2002/06/11 12:37:38  bnv
 * Added: CDECL
 *
 * Revision 1.6  2002/06/06 08:23:46  bnv
 * Changed: mkstemp to MKTEMP
 *
 * Revision 1.5  2001/11/29 13:12:52  bnv
 * Corrected: The error handling when trace is off
 *
 * Revision 1.4  2001/06/25 18:51:48  bnv
 * Header -> Id
 *
 * Revision 1.3  1999/11/26 13:13:02  bnv
 * Added: Windows CE support.
 *
 * Revision 1.2  1999/03/10 16:53:32  bnv
 * Added MSC support
 *
 * Revision 1.1  1998/07/02 17:34:50  bnv
 * Initial revision
 *
 */

#include <string.h>
#include <stdlib.h>
#include <lstring.h>

#include <rexx.h>
#include <trace.h>
#include <stack.h>
#include <compile.h>
#include <interpre.h>

#ifndef WIN
#if defined(MSDOS) || defined(__WIN32__)
#	include <io.h>
#	include <fcntl.h>
#ifndef _MSC_VER
#	include <dir.h>
#endif
#	include <process.h>
#	if defined(__BORLANDC__) && !defined(__WIN32__)
#		include <systemx.h>
#	endif
#elif defined(__MPW__)
#elif defined(_MSC_VER)
#else
#	include <fcntl.h>
#	include <unistd.h>
#endif

#include <sys/stat.h>
#include <string.h>

#ifndef S_IREAD
#	define S_IREAD 0
#	define S_IWRITE 1
#endif

#define NOSTACK		0
#define FIFO		1
#define LIFO		2
#define STACK		3

#define LOW_STDIN	0
#define LOW_STDOUT	1

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

/* -------------------- mkfntemp -------------------- */
static void
mkfntemp(char *fn, size_t length)
{
	int	l;
	char	*c;


	fn[0] = '\0'; c = getenv("TEMP");
	if (c) STRCPY(fn,c);
	l = STRLEN(fn);
	if (l)
		if (fn[l-1] != FILESEP) {
			fn[l] = FILESEP;
			fn[l+1] = '\0';
		}
	STRCAT(fn,"OXXXXXX");
	MKTEMP(fn);
} /* mkfntemp */

/* ------------------ RxRedirectCmd ----------------- */
int __CDECL
RxRedirectCmd(PLstr cmd, int in, int out, PLstr outputstr)
{
	char	fnin[250], fnout[250];
	int	old_stdin, old_stdout;
	int	filein, fileout;
	FILE	*f;
	PLstr	str;

	/* --- redirect input --- */
	if (in) {
		mkfntemp(fnin,sizeof(fnin));
		if ((f=fopen(fnin,"w"))!=NULL) {
			while (StackQueued()>0) {
				str = PullFromStack();
				L2STR(str); LASCIIZ(*str);
				fputs(LSTR(*str),f); fputc('\n',f);
				LPFREE(str);
			}
			fclose(f);

			old_stdin = dup(LOW_STDIN);
			filein = open(fnin,S_IREAD);
			dup2(filein,LOW_STDIN);
			close(filein);
		} else
			in = FALSE;
	}

	/* --- redirect output --- */
	if (out) {
		mkfntemp(fnout,sizeof(fnout));
		old_stdout = dup(LOW_STDOUT);
		fileout = open(fnout,O_WRONLY|O_CREAT,0600);
		dup2(fileout,LOW_STDOUT);
		close(fileout);
	}

	/* --- Execute the command --- */
	LASCIIZ(*cmd);
#if defined(__BORLANDC__) && !defined(__WIN32__)
	rxReturnCode = systemx(LSTR(*cmd));
#else
	rxReturnCode = system(LSTR(*cmd));
#endif

	/* --- restore input --- */
	if (in) {
		close(LOW_STDIN);
		dup2(old_stdin,LOW_STDIN);
		close(old_stdin);
		remove(fnin);
	}

	/* --- restore output --- */
	if (out) {
		close(LOW_STDOUT);
		dup2(old_stdout,LOW_STDOUT);  /* restore stdout */
		close(old_stdout);
#ifndef MSDOS
		chmod(fnout,0666);
#endif
		if ((f=fopen(fnout,"r"))!=NULL) {
			if (outputstr) {
				Lread(f,outputstr,LREADFILE);
#ifdef RMLAST
				if (LSTR(*outputstr)[LLEN(*outputstr)-1]=='\n')
					LLEN(*outputstr)--;
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

	return rxReturnCode;
} /* RxRedirectCmd */
#endif

/* ------------------ RxExecuteCmd ----------------- */
int __CDECL
RxExecuteCmd( PLstr cmd, PLstr env )
{
#ifndef WIN
	int	in,out;
	Lstr	cmdN;

	LINITSTR(cmdN); Lfx(&cmdN,1);
	Lstrcpy(&cmdN,cmd);
	L2STR(&cmdN);

	LASCIIZ(cmdN);
	if (env==NULL) {
		chkcmd4stack(&cmdN,&in,&out);
		rxReturnCode = RxRedirectCmd(&cmdN,in,out,FALSE);
	} else
	if (	!Lcmp(env,"COMMAND") ||
		!Lcmp(env,"DOS")     ||
		!Lcmp(env,"CMS")     ||
		!Lstrcmp(env,&(systemStr->key))) {
			chkcmd4stack(&cmdN,&in,&out);
			rxReturnCode = RxRedirectCmd(&cmdN,in,out,FALSE);
	}
#if defined(__BORLANDC__) && !defined(__WIN32__)
	else
	if (!Lcmp(env,"INT2E"))
		int2e(LSTR(cmdN));
#endif
	else
		if (!Lcmp(env,"EXEC"))  ; /*execl(...); */
	else
		rxReturnCode = -3;

	/* free string */
	LFREESTR(cmdN);

	RxSetSpecialVar(RCVAR,rxReturnCode);
	if (rxReturnCode && !(_proc[_rx_proc].trace & off_trace)) {
		if (_proc[_rx_proc].trace & (error_trace | normal_trace)) {
			TraceCurline(NULL,TRUE);
			fprintf(STDERR,"       +++ RC(%d) +++\n",rxReturnCode);
			if (_proc[_rx_proc].interactive_trace)
				TraceInteractive(FALSE);
		}
		if (_proc[_rx_proc].condition & SC_ERROR)
			RxSignalCondition(SC_ERROR);
	}
#else
	size_t	len;
	char	*ch;
	Lstr	file, args;
	TCHAR	*uFile, *uArgs;
	PROCESS_INFORMATION p;

	LINITSTR(file);
	LINITSTR(args); Lfx(&args,1);

	ch = LSTR(*cmd);
	if ((*ch=='\'') || (*ch=='\"')) {
		ch = STRCHR(ch+1,*ch);
		if (ch)
			len = (DWORD)ch - (DWORD)LSTR(*cmd);
		else
			len = LLEN(*cmd);
		Lsubstr(&file, cmd, 2, len-1, ' ');
		Lsubstr(&args, cmd, len+3, LREST, ' ');
	} else {
		Lword(&file, cmd, 1);
		Lsubword(&args, cmd, 2, LREST);
	}

	uFile = (TCHAR*)MALLOC(sizeof(TCHAR)*LLEN(file)+2,NULL);
	uArgs = (TCHAR*)MALLOC(sizeof(TCHAR)*LLEN(args)+2,NULL);
	mbstowcs(uFile,LSTR(file),LLEN(file));	uFile[LLEN(file)] = (TCHAR)0;
	mbstowcs(uArgs,LSTR(args),LLEN(args));	uArgs[LLEN(args)] = (TCHAR)0;

	CreateProcess(uFile, uArgs, NULL, NULL, FALSE, 0, NULL, NULL, NULL, &p);
	CloseHandle(p.hProcess);
	CloseHandle(p.hThread);

	FREE(uFile);
	FREE(uArgs);
	LFREESTR(file);
	LFREESTR(args);
#endif
	return rxReturnCode;
} /* RxExecuteCmd */
