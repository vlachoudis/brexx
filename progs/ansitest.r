/*
        Library with ANSI routines		1998
        Vassilis N. Vlachoudis   V.Vlachoudis@cern.ch
*/
call load "ansi.r"
call AnsiCls

say center("ANSI demonstration",79)
say

say "Ansi Colors:"
colors = "BLACK RED GREEN YELLOW BLUE MAGENTA CYAN WHITE"
do i=1 to words(colors)
	call Ansicolor word(colors,i)
	say word(colors,i)
end
do i=1 to words(colors)
	call Ansicolor "BOLD"word(colors,i)
	say word(colors,i)
end
say
say "Press ENTER to continue..."
pull 
call AnsiCls
say "Foreground and Background color set"
say
do i=1 to words(colors)
	do j=1 to words(colors)
		call AnsiColor word(colors,j),word(colors,i)
		call write ,"X"
	end
	do j=1 to words(colors)
		call AnsiColor "BOLD"word(colors,j),word(colors,i)
		call write ,"X"
	end
	say
end
call AnsiAttr "normal"
say
attr = "NORMAL BOLD UNDERLINE BLINK REVERSE UNVISIBLE"
say "Attributes:"
do i=1 to words(attr)
	call AnsiAttr word(attr,i)
	say word(attr,i)
end
say
call AnsiAttr "normal"
say "Press ENTER to continue..."
pull
call AnsiMode(0)
call AnsiSaveCursor
do x=1 to 40
	call AnsiGoto x,x%2
	call write ,'#'
end
call AnsiColor "boldwhite","yellow"
do f=0 to 6.28 by 0.05
	x = trunc(21 + 20*cos(f))
	y = trunc(14 + 12*sin(f))
	call AnsiGoto x,y
	call write ,'*'
end
call AnsiLoadCursor
call AnsiAttr "normal"
say "AnsiGoto, and AnsiMode test"
say "Press ENTER to continue..."
pull 
call AnsiMode(3)
exit 1
