/*
        Library with ANSI routines		1998
        Vassilis N. Vlachoudis   V.Vlachoudis@cern.ch
*/
parse source . calltype .
if calltype ^== "COMMAND" then return

exit 1

/* --- AnsiMode(mode) --- */
/* 0 - 40x25 C, 1 - 40x25 BW, 3 - 80x25 C .... */
AnsiMode:
	call write ,'1B'x||'[='arg(1)'h'
return

/* --- Clear Screen --- */
AnsiCls:
	call write ,'1B'x||'[2J'
return

/* --- Erase until end of line --- */
AnsiEraseEOL:
	call write ,'1B'x||'[K'
return

/* --- AnsiCursorUp, n-counts --- */
AnsiCursorUp: procedure
	parse arg n
	if n=="" then n=1
	call write ,'1B'x||'['n'A'
return

/* --- AnsiCursorDown --- */
AnsiCursorDown: procedure
	parse arg n
	if n=="" then n=1
	call write ,'1B'x||'['n'B'
return

/* --- AnsiCursorRight --- */
AnsiCursorRight: procedure
	parse arg n
	if n=="" then n=1
	call write ,'1B'x||'['n'C'
return

/* --- AnsiCursorLeft --- */
AnsiCursorLeft: procedure
	parse arg n
	if n=="" then n=1
	call write ,'1B'x||'['n'D'
return

/* --- AnsiGoto(X,Y) --- */
AnsiGoto:
	call write ,'1B'x||'['arg(2)';'arg(1)'H'
return

/* --- AnsiSaveCursor --- */
AnsiSaveCursor:
	call write ,'1B'x||'[s'
return

/* --- AnsiLoadCursor --- */
AnsiLoadCursor:
	call write ,'1B'x||'[u'
return

/* --- AnsiColor(forecolor[,backcolor]) --- */
/* --- color can be   [BOLD]color       --- */
AnsiColor: procedure
	arg fg,bg,attr
	colors = "BLACK RED GREEN YELLOW BLUE MAGENTA CYAN WHITE"
	bold = 0
	if left(fg,4) == "BOLD" then do
		bold = 1
		fg = substr(fg,5)
	end
	if bg ^== "" then do
		b = wordpos(bg,colors)
		if b = 0 then return "ERROR"
		back = ";4"||b-1
	end; else
		back = ""
	f = wordpos(fg,colors)
	if f = 0 then return "ERROR"
	if bold then
		call write ,"1B"x||"[1;3"||f-1||back||"m"
	else
		call write ,"1B"x||"[3"||f-1||back||"m"
return

/* ----- Atributes ------ */
AnsiAttr: procedure
	arg attr
	p = wordpos(attr,"NORMAL BOLD UNDERLINE BLINK REVERSE UNVISIBLE")
	if p = 0 then return "ERROR"
	call write ,"1B"x||"["p-1"m"
return
