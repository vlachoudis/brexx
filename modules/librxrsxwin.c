/*
||
|| Some <windows.h> functions for BREXX
|| It's just a test...
|| 
|| 13-Feb-1999 by Generoso Martello (generoso@martello.com)
||
*/

#ifdef RSXWIN
#include <windows.h>

#include <rexx.h>
#include <rxdefs.h>

/* -------------------------------------------------------------- */
/*  WINMESSAGEBOX((text))                                         */
/* -------------------------------------------------------------- */
void R_rsxwin_msgbox() {
	if (ARGN!=2) Lerror(ERR_INCORRECT_CALL,0);
	L2STR(ARG1);
	L2STR(ARG2);
	MessageBox (NULL, LSTR(*ARG1), LSTR(*ARG2), MB_OK);
}

void RxRSXWinInitialize() {
	RxRegFunction("WINMESSAGEBOX",		R_rsxwin_msgbox,			0);
}
#endif
