/*
||
|| Some <conio.h> functions for BREXX
|| Fully compatible with ansi.r by bnv
||
|| 13-Feb-1999 by Generoso Martello (generoso@martello.com)
||
*/

//#include <conio.h>

#include "rexx.h"
#include "rxdefs.h"
#include "rxconio.h"

/* -------------------------------------------------------------- */
/*  ANSICLS()                                                     */
/* -------------------------------------------------------------- */
/*  ANSIERASEEOL()                                                */
/* -------------------------------------------------------------- */
/*  ANSIDELLINE()                                                 */
/* -------------------------------------------------------------- */
/*  ANSIINSLINE()                                                 */
/* -------------------------------------------------------------- */
/*  ANSISAVECURSOR()                                              */
/* -------------------------------------------------------------- */
/*  ANSILOADCURSOR()                                              */
/* -------------------------------------------------------------- */
/*  GETCH()                                                       */
/* -------------------------------------------------------------- */
/*  KBHIT()                                                       */
/* -------------------------------------------------------------- */
/*  WHEREX()                                                      */
/* -------------------------------------------------------------- */
/*  WHEREY()                                                      */
/* -------------------------------------------------------------- */
void R_conio_O( const int func ) {
	if (ARGN) Lerror(ERR_INCORRECT_CALL,0);
	switch (func) {
		case f_ansicls:
			ANSI_CLEAR_SCREEN();
			break;
		case f_ansieraseeol:
			ANSI_ERASE_EOL();
			break;
		case f_ansidelline:
			delline();
			break;
		case f_ansiinsline:
			insline();
			break;
		case f_ansisavecursor:
			ANSI_SAVE_CURSOR();
			break;
		case f_ansiloadcursor:
			ANSI_LOAD_CURSOR();
			break;
		case f_getch:
			LTYPE(*ARGR) = LSTRING_TY;
			LLEN(*ARGR) = 1;
			LSTR(*ARGR)[0] = getch();
			break;
		case f_kbhit:
			LTYPE(*ARGR) = LSTRING_TY;
			LLEN(*ARGR) = 1;
			Licpy(ARGR,kbhit());
			break;
		case f_wherex:
			LTYPE(*ARGR) = LINTEGER_TY;
			LLEN(*ARGR) = 1;
			Licpy(ARGR,wherex());
			break;
		case f_wherey:
			LTYPE(*ARGR) = LINTEGER_TY;
			LLEN(*ARGR) = 1;
			Licpy(ARGR,wherey());
			break;
		default:
			Lerror(ERR_INTERPRETER_FAILURE,0);
	}
}

/* -------------------------------------------------------------- */
/*  ANSIMODE((mode))                                              */
/* -------------------------------------------------------------- */
/*  ANSIATTR((attr))                                              */
/* -------------------------------------------------------------- */
/*  [ANSI]SETCURSOR((type))                                       */
/* -------------------------------------------------------------- */
void R_conio_C( const int func ) {
	int i;
	if (ARGN!=1) Lerror(ERR_INCORRECT_CALL,0);
	L2STR(ARG1);
	switch (func) {
		case f_ansimode:
			i=LASTMODE;
			if      (Lcmp(ARG1,"LASTMODE")==0) i=LASTMODE;
			else if (Lcmp(ARG1,"BW40")    ==0) i=BW40;
			else if (Lcmp(ARG1,"C40")     ==0) i=C40;
			else if (Lcmp(ARG1,"BW80")    ==0) i=BW80;
			else if (Lcmp(ARG1,"C80")     ==0) i=C80;
			else if (Lcmp(ARG1,"MONO")    ==0) i=MONO;
			else if (Lcmp(ARG1,"C4350")   ==0) i=C4350;
			else if (Lcmp(ARG1,"0")       ==0) i=C40;
			else if (Lcmp(ARG1,"1")       ==0) i=BW40;
			else if (Lcmp(ARG1,"3")       ==0) i=C80;
			ANSI_MODE(i);
			break;
		case f_ansiattr:
			if      (Lcmp(ARG1,"NORMAL")    ==0) i=0;
			else if (Lcmp(ARG1,"BOLD")      ==0) i=1;
			else if (Lcmp(ARG1,"UNDERLINED")==0) i=2;
			else if (Lcmp(ARG1,"BLINK")     ==0) i=3;
			else if (Lcmp(ARG1,"REVERSE")   ==0) i=4;
			else if (Lcmp(ARG1,"UNVISIBLE") ==0) i=5;
			else                                 i=0;
			ANSI_ATTR(i);
			break;
		case f_setcursor:
			i=-1;
			if      (Lcmp(ARG1,"OFF")   ==0) i=_NOCURSOR;
			else if (Lcmp(ARG1,"SOLID") ==0) i=_SOLIDCURSOR;
			else if (Lcmp(ARG1,"NORMAL")==0) i=_NORMALCURSOR;
			if (i!=-1) _setcursortype(i);
			break;
		default:
			Lerror(ERR_INTERPRETER_FAILURE,0);
	}
}

/* -------------------------------------------------------------- */
/*  ANSICURSORUP((n))                                             */
/* -------------------------------------------------------------- */
/*  ANSICURSORDOWN((n))                                           */
/* -------------------------------------------------------------- */
/*  ANSICURSORRIGHT((n))                                          */
/* -------------------------------------------------------------- */
/*  ANSICURSORLEFT((n))                                           */
/* -------------------------------------------------------------- */
void R_conio_I( const int func ) {
	if (ARGN!=1) Lerror(ERR_INCORRECT_CALL,0);
	L2INT(ARG1);
	switch (func) {
		case f_ansicursorup:
			ANSI_CURSOR_UP(LINT(*ARG1));
			break;
		case f_ansicursordown:
			ANSI_CURSOR_DOWN(LINT(*ARG1));
			break;
		case f_ansicursorright:
			ANSI_CURSOR_RIGHT(LINT(*ARG1));
			break;
		case f_ansicursorleft:
			ANSI_CURSOR_LEFT(LINT(*ARG1));
			break;
		default:
			Lerror(ERR_INTERPRETER_FAILURE,0);
	}
}

/* -------------------------------------------------------------- */
/*  ANSICOLOR((forecolor)[,(backcolor)])                          */
/* -------------------------------------------------------------- */
void R_conio_ansicolor() {
	int i=-1, j=-1;
	if ((ARGN<1) || (ARGN>2)) Lerror(ERR_INCORRECT_CALL,0);
	L2STR(ARG1);
	if      (Lcmp(ARG1,"BLACK")       ==0) i=BLACK;
	else if (Lcmp(ARG1,"BLUE")        ==0) i=BLUE;
	else if (Lcmp(ARG1,"GREEN")       ==0) i=GREEN;
	else if (Lcmp(ARG1,"CYAN")        ==0) i=CYAN;
	else if (Lcmp(ARG1,"RED")         ==0) i=RED;
	else if (Lcmp(ARG1,"MAGENTA")     ==0) i=MAGENTA;
	else if (Lcmp(ARG1,"YELLOW")      ==0) i=YELLOW;
	else if (Lcmp(ARG1,"WHITE")       ==0) i=WHITE;
	else if ((Lcmp(ARG1,"BROWN")       ==0) || (Lcmp(ARG1,"BOLDYELLOW") ==0)) i=BROWN;
	else if ((Lcmp(ARG1,"LIGHTGRAY")   ==0) || (Lcmp(ARG1,"BOLDWHITE")  ==0)) i=LIGHTGRAY;
	else if ((Lcmp(ARG1,"DARKGRAY")    ==0) || (Lcmp(ARG1,"BOLDBLACK")  ==0)) i=DARKGRAY;
	else if ((Lcmp(ARG1,"LIGHTBLUE")   ==0) || (Lcmp(ARG1,"BOLDBLUE")   ==0)) i=LIGHTBLUE;
	else if ((Lcmp(ARG1,"LIGHTGREEN")  ==0) || (Lcmp(ARG1,"BOLDGREEN")  ==0)) i=LIGHTGREEN;
	else if ((Lcmp(ARG1,"LIGHTCYAN")   ==0) || (Lcmp(ARG1,"BOLDCYAN")   ==0)) i=LIGHTCYAN;
	else if ((Lcmp(ARG1,"LIGHTRED")    ==0) || (Lcmp(ARG1,"BOLDRED")    ==0)) i=LIGHTRED;
	else if ((Lcmp(ARG1,"LIGHTMAGENTA")==0) || (Lcmp(ARG1,"BOLDMAGENTA")==0)) i=LIGHTMAGENTA;
	if (ARGN==2) {
		L2STR(ARG2);
		if      (Lcmp(ARG2,"BLACK")       ==0) j=BLACK;
		else if (Lcmp(ARG2,"BLUE")        ==0) j=BLUE;
		else if (Lcmp(ARG2,"GREEN")       ==0) j=GREEN;
		else if (Lcmp(ARG2,"CYAN")        ==0) j=CYAN;
		else if (Lcmp(ARG2,"RED")         ==0) j=RED;
		else if (Lcmp(ARG2,"MAGENTA")     ==0) j=MAGENTA;
		else if (Lcmp(ARG2,"YELLOW")      ==0) j=YELLOW;
		else if (Lcmp(ARG2,"WHITE")       ==0) j=WHITE;
	}
	if (j!=-1) { ANSI_COLOR(i,j); } else { ANSI_FG_LCOLOR(i); }
}

/* -------------------------------------------------------------- */
/*  ANSIGOTO((x),(y))                                             */
/* -------------------------------------------------------------- */
void R_conio_ansigoto() {
	if (ARGN!=2) Lerror(ERR_INCORRECT_CALL,0);
	L2INT(ARG1);
	L2INT(ARG2);
	ANSI_GOTOXY(LINT(*ARG1),LINT(*ARG2));
}

void RxConIOInitialize() {
	RxRegFunction("CLS",			R_conio_O,			f_ansicls);
	RxRegFunction("ANSICLS",		R_conio_O,			f_ansicls);
	RxRegFunction("ANSIERASEEOL",		R_conio_O,			f_ansieraseeol);
	RxRegFunction("ANSIDELLINE",		R_conio_O,			f_ansidelline);
	RxRegFunction("ANSIINSLINE",		R_conio_O,			f_ansiinsline);
	RxRegFunction("ANSISAVECURSOR",	R_conio_O,			f_ansisavecursor);
	RxRegFunction("ANSILOADCURSOR",	R_conio_O,			f_ansiloadcursor);
	RxRegFunction("GETCH",			R_conio_O,			f_getch);
	RxRegFunction("KBHIT",			R_conio_O,			f_kbhit);
	RxRegFunction("WHEREX",			R_conio_O,			f_wherex);
	RxRegFunction("WHEREY",			R_conio_O,			f_wherey);
	RxRegFunction("ANSICURSORUP",		R_conio_I,			f_ansicursorup);
	RxRegFunction("ANSICURSORDOWN",	R_conio_I,			f_ansicursordown);
	RxRegFunction("ANSICURSORRIGHT",	R_conio_I,			f_ansicursorright);
	RxRegFunction("ANSICURSORLEFT",	R_conio_I,			f_ansicursorleft);
	RxRegFunction("ANSIMODE",		R_conio_C,			f_ansimode);
	RxRegFunction("ANSIATTR",		R_conio_C,			f_ansiattr);
	RxRegFunction("SETCURSOR",		R_conio_C,			f_setcursor);
	RxRegFunction("ANSISETCURSOR",	R_conio_C,			f_setcursor);
	RxRegFunction("ANSIGOTO",		R_conio_ansigoto,		0);
	RxRegFunction("ANSICOLOR",		R_conio_ansicolor,	0);
}
