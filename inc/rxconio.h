/*
||
|| Header file for rxconio.c ...
|| Some <conio.h> functions for BREXX
|| Fully compatible with ansi.r by bnv
||
|| 13-Feb-1999 by Generoso Martello (generoso@martello.com)
||
*/

#ifdef RSXWIN

#define BLACK   0
#define RED     1
#define GREEN   2
#define YELLOW  3
#define BLUE    4
#define MAGENTA 5
#define CYAN    6
#define WHITE   7

#define ANSI_GOTOXY(z,s)		printf("\033[%d;%dH", (s), (z))
#define ANSI_CLEAR_SCREEN()		printf("\033[2J")
#define ANSI_COLOR(f,b)			printf("\033[%d;%dm", (f) + 30, (b) + 40)
#define ANSI_FG_LCOLOR(f)		printf("\033[0;%dm", (f) + 30)
#define ANSI_FG_HCOLOR(f)		printf("\033[1;%dm", (f) + 30)
#define ANSI_ERASE_EOL()		printf("\033[K")
#define ANSI_MODE(m)			printf("\033[=%dh", (m))
#define ANSI_PRINTF			printf

enum text_modes { LASTMODE=-1, BW40=0, C40, BW80, C80, MONO=7, C4350=64 };

#else

#define ANSI_GOTOXY(z,s)		gotoxy(z,s)
#define ANSI_CLEAR_SCREEN()		clrscr()
#define ANSI_COLOR(f,b)			textcolor(f) ; textbackground(b)
#define ANSI_FG_LCOLOR(f)		textcolor(f)
#define ANSI_ERASE_EOL()		clreol()
#define ANSI_MODE(m)			textmode(m)
#define ANSI_PRINTF			cprintf

#endif

#define ANSI_DEFAULT()			ANSI_PRINTF("\033[0m")
#define ANSI_CURSOR_UP(n)		ANSI_PRINTF("\033[%dA", (n))
#define ANSI_CURSOR_DOWN(n)		ANSI_PRINTF("\033[%dB", (n))
#define ANSI_CURSOR_RIGHT(n)		ANSI_PRINTF("\033[%dC", (n))
#define ANSI_CURSOR_LEFT(n)		ANSI_PRINTF("\033[%dD", (n))
#define ANSI_SAVE_CURSOR()		ANSI_PRINTF("\033[s")
#define ANSI_LOAD_CURSOR()		ANSI_PRINTF("\033[u")
#define ANSI_ATTR(m)			ANSI_PRINTF("\033[%dm", (m))

enum conio_functions {
 f_ansicls=f_lastfunc+1,
 f_ansieraseeol,
 f_ansidelline,
 f_ansiinsline,
 f_getch,
 f_kbhit,
 f_wherex,
 f_wherey,
 f_ansimode,
 f_ansiattr,
 f_setcursor,
 f_ansicolor,
 f_ansigoto,
 f_ansicursorup,
 f_ansicursordown,
 f_ansicursorright,
 f_ansicursorleft,
 f_ansisavecursor,
 f_ansiloadcursor
};
