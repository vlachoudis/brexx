/* ------  Console input/output routine ------ */
/* various video functions via bios int 10h    */
/* by bill V.Vlachoudis@cern.ch                */
/* ------------------------------------------- */
parse source . calltype .
if calltype ^== "COMMAND" then return

say "This is a library file, and cannot run alone..."
exit 1

/* ------------------ clear screen ---------------- */
cls: procedure
call intr '10'h, "ax=0600 bx=0700 cx=0000 dx=1950"
call intr '10'h, "ax=0200 bx=0000 dx=0000"
return

/* ------------------ eoline -------------------- */
eoline: procedure
call write ,'1b'x || "[K"
return

/* ------------------ setmode ------------------- */
setmode: procedure
call intr '10'h,"ax=" || right(d2x(arg(1)),2,0)
return

/* ------------------ getmode ------------------ */
getmode: procedure
parse value intr('10'h, "ax=0f00") with "AX=" col +5 mode +2 .
return x2d(mode) x2d(col)

/* -------------- position cursor --------------- */
gotoxy: procedure
dx = right(d2x(arg(2)),2,0) || right(d2x(arg(1)),2,0)
call intr '10'h,"ax=0200 bx=0000" "dx="dx
return

/* ------------- get cursor position ------------ */
wherexy: procedure
parse value intr('10'h,"ax=0300 bx=0000") with "DX=" +3 Y +2 X +2 .
return x2d(x) x2d(y)

/* ------------------- wherex ------------------- */
wherex: procedure
return word(wherexy(),1)

/* ------------------- wherey ------------------- */
wherey: procedure
return word(wherexy(),2)

/* --------------- put pixel -------------------- */
putpixel: procedure
call intr '10'h, "AX=0C"d2x(arg(3),2) "BX=0000 CX="d2x(arg(1)) "DX="d2x(arg(2))
return

/* --------------- get pixel -------------------- */
getpixel: procedure
return x2d(substr(intr('10'h,"AX=0D00" "BX=0000 CX="d2x(arg(1)) "DX="d2x(arg(2))),6,2))

/* --------------------- getch ------------------ */
/* waits for a key to be pressed an returns the   */
/* character. If it is a special character then   */
/* the scan code will be returned in HEX 2 bytes  */
/* ---------------------------------------------- */
getch: procedure
parse value intr('16'h,"AX=0000") with "AX=" +3 scan +2 char +2
if char == "00" then return scan
else return x2c(char)

/* -------------------- getche ------------------ */
/* like the above and it echos the char to screen */
/* ---------------------------------------------- */
getche: procedure
parse value intr('16'h,"AX=0000") with "AX=" +3 scan +2 char +2
if char == "00" then return scan
else do
	char = x2c(char)
	call write ,char
	return char
end

/* -------------------- kbhit ------------------- */
/* returns '1' if a character is waiting in the   */
/* keyboard buffer, else returns '0'              */
/* ---------------------------------------------- */
kbhit: procedure
parse value intr('16'h,"AX=0100") with "FLAGS=" flags .
keypressed = (pos('Z',flags)==0)
return keypressed
