/*                                                                      *
*      $Source:  panel15 exec $                                         *
*      $HEADER:  Panel15.exec,v 1.5 90/2/5 12:58:48 A.Yamanaka locked $ *
*      $Author:  A.Yamanaka , tab3kac@jpntohok                          *
*      $Locker:  A.Yamanaka                                             *
*      Copyright  (c) 1990 Education Center for Information Processing */
 
/*
 Permission to use, copy, modify, and distributed this software
 and its documentation for any purpose and without free is hereby granted
 , provided thst the above copyright notice appear in all copies and that
 both that copyright notice and this permission notice appear in support-
 ing documentation, and that the name of ECIP not be used in advertising
 or publicity pertaining to distribution of the software without specific
 , written prior permission.
 ECIP makes no representations about the suitability of this software for
 any purpose. It is provided "as is" without express or implied warrantry
.  */
 
/**  Author:  Atsuhiko Yamanaka - Project ADVAN ,ECIP **/
/**  Version  1.5      **/
 
/*'CP SET LINEDIT OFF'
'NUCXLOAD TYPLIN SEMITYPE (SERVICE PUSH'
'TYPLIN START'
'TYPLIN ESCAPE +' */
 
call load 'ansi.r'
call ansicls
call ansigoto 40,15

SAY 'Now setting up , wait'
 
POINT. = 0
POINTER = 2
X. = 0
Y. = 0
 
CALL SETUP POINT. POINTER X. Y.
CALL GRID POINT.

call ansigoto 40,15
SAY '                      '
 
CALL HELP
 
DO FOREVER
    call ansigoto 1,21
    call write ,copies(' ',79)
    call ansigoto 1,21
    call write ,'Enter cmd: '
    PARSE PULL  PANEL
    UPPER PANEL
    IF PANEL = 'BYE' THEN CALL BYE
    IF PANEL = 'GIVE UP' THEN CALL GIVE_UP POINT. POINTER X. Y. STACK.
    IF PANEL = 'SCREEN'  THEN CALL SCREEN  POINT.
    CALL GETPLACE POINT. PANEL XP YP MODE
    CALL ISBLANK? POINT. DIRECTION XP YP MODE STACK. XP_O YP_O
    CALL SETSTACK POINT. XP_O YP_O XP YP MODE STACK. POINTER X. Y.
    CALL CHANGE POINT. MODE STACK.
END
EXIT
 
SCREEN : PROCEDURE EXPOSE POINT.
CALL HELP
CALL GRID POINT.
RETURN
 
SETUP : PROCEDURE EXPOSE POINT. POINTER X. Y.
POINT.2.2 = ' 1 '
POINT.2.3 = ' 2 '
POINT.2.4 = ' 3 '
POINT.2.5 = ' 4 '
POINT.3.2 = ' 5 '
POINT.3.3 = ' 6 '
POINT.3.4 = ' 7 '
POINT.3.5 = ' 8 '
POINT.4.2 = ' 9 '
POINT.4.3 = ' 10'
POINT.4.4 = ' 11'
POINT.4.5 = ' 12'
POINT.5.2 = ' 13'
POINT.5.3 = ' 14'
POINT.5.4 = ' 15'
POINT.5.5 = '   '
 
XP = 5
YP = 5
X.1 = XP
Y.1 = YP
POINTER = 2
 
DO I = 1 TO 200
   DIRECTION = RANDOM(1,4)
   IF DIRECTION = 1 THEN DO    /*UP*/
      XP_O = XP
      YP_O = YP - 1
   END
   IF DIRECTION = 2 THEN DO    /*RIGHT*/
      XP_O = XP + 1
      YP_O = YP
   END
   IF DIRECTION = 3 THEN DO    /*DOWN*/
      XP_O = XP
      YP_O = YP + 1
   END
   IF DIRECTION = 4 THEN DO    /*LEFT*/
      XP_O = XP - 1
      YP_O = YP
   END
   IF POINT.YP_O.XP_O ^= 0 THEN DO
      POINT.YP.XP = POINT.YP_O.XP_O
      POINT.YP_O.XP_O = '   '
      XP = XP_O
      YP = YP_O
      Y.POINTER = YP
      X.POINTER = XP
      POINTER = POINTER + 1
   END
END
RETURN
 
 
GIVE_UP : PROCEDURE EXPOSE POINT. POINTER X. Y.
POINTER = POINTER - 1
call ansigoto 20,4
SAY ' Hey boy..  I will play instead of you.  '
 
DO I = POINTER TO 2 BY -1
/* DO K = 1 TO 2500
   END  */
   MODE = 2
   J = I - 1
   NX = X.I
   NY = Y.I
   OX = X.J
   OY = Y.J
   IF OY = NY THEN MODE = 1
   POINT.NY.NX = POINT.OY.OX
   POINT.OY.OX = '   '
   IF MODE = 1 THEN STACK.1 = OY
   IF MODE = 2 THEN DO
      STACK.1 = OY
      STACK.2 = NY
   END
   CALL CHANGE POINT. MODE STACK.
END
CALL BYE
RETURN
 
 
GETPLACE : PROCEDURE EXPOSE POINT. PANEL XP YP MODE
MODE = 0
XP = 0
DO I = 2 TO 5
   DO J = 2 TO 5
      IF PANEL = SUBWORD(POINT.I.J,1) THEN DO
           XP = J
           YP = I
      END
   END
end
IF XP = 0 THEN  MODE = 1
 
RETURN
 
 
SETSTACK : PROCEDURE EXPOSE POINT. XP_O YP_O XP YP MODE  STACK. ,
                            POINTER X. Y.
IF MODE ^= 0 THEN DO
   POINT.YP_O.XP_O = POINT.YP.XP
   POINT.YP.XP = '   '
   Y.POINTER = YP
   X.POINTER = XP
   POINTER = POINTER + 1
   IF MODE = 1 THEN STACK.1 = YP
   IF MODE = 2 THEN DO
      STACK.1 = YP
      STACK.2 = YP_O
   END
END
RETURN
 
 
ISBLANK? : PROCEDURE EXPOSE POINT. DIRECTION XP YP MODE  STACK. ,
                            XP_O YP_O
IF MODE = 1 THEN DO
    MODE = 0
    RETURN
END
 
    XP_O = XP
    YP_O = YP-1
    MODE = 2
IF POINT.YP_O.XP_O  = '   ' THEN RETURN
    XP_O = XP+1
    YP_O = YP
    MODE = 1
IF POINT.YP_O.XP_O  = '   ' THEN RETURN
    XP_O = XP
    YP_O = YP+1
    MODE = 2
IF POINT.YP_O.XP_O  = '   ' THEN RETURN
    XP_O = XP-1
    YP_O = YP
    MODE = 1
IF POINT.YP_O.XP_O  = '   ' THEN RETURN
MODE = 0
RETURN
 
 
 
HELP:
call ansigoto 40, 5; SAY '   ________________________________ '
call ansigoto 40, 6; SAY ' /|_Help_Window____________________|'
call ansigoto 40, 7; SAY ' || 1. To move the number,         |'
call ansigoto 40, 8; SAY ' ||     type the number:           |'
call ansigoto 40, 9; SAY ' ||          eg. 15                |'
call ansigoto 40,10; SAY ' || 2. If you give up ,            |'
call ansigoto 40,11; SAY ' ||     type:                      |'
call ansigoto 40,12; SAY ' ||         give up                |'
call ansigoto 40,13; SAY ' || 3. If screen has disappeared , |'
call ansigoto 40,14; SAY ' ||     type:                      |'
call ansigoto 40,15; SAY ' ||         screen                 |'
call ansigoto 40,16; SAY ' || 4. To exit this puzzle         |'
call ansigoto 40,17; SAY ' ||     type:                      |'
call ansigoto 40,18; SAY ' ||         bye                    |'
call ansigoto 40,19; SAY ' ||________________________________|'
call ansigoto 40,20; SAY ' |/_______________________________/ '
 
return
 
CHANGE: PROCEDURE EXPOSE POINT. MODE STACK.
 
IF MODE = 0 THEN RETURN
 
LINE.  = ' ||'
LINE.1 = ' /|'
IF MODE = 1 THEN DO
    I = STACK.1
    DO J = 2 TO 5
       LINE.I = LINE.I||POINT.I.J||'|'
    END
    call ansigoto 15,(I-2)*2+8
    say  LINE.I
END
ELSE DO
    I = STACK.1
    DO J = 2 TO 5
        LINE.I = LINE.I||POINT.I.J||'|'
    END
    call ansigoto 15,(I-2)*2+8
    say  LINE.I
    I = STACK.2
    DO J = 2 TO 5
        LINE.I = LINE.I||POINT.I.J||'|'
    END
    call ansigoto 15,(I-2)*2+8
    say LINE.I
END
RETURN
 
 
GRID: PROCEDURE EXPOSE POINT.
LINE.  = '||'
LINE.1 = '/|'
DO I = 2 TO 5
   DO J = 2 TO 5
      LINE.I = LINE.I||POINT.I.J||'|'
   END
END
call ansigoto 15,7;  say '  _________________'
call ansigoto 15,8;  say ' 'LINE.2
call ansigoto 15,9;  say ' ||---+---+---+---| '
call ansigoto 15,10; say ' 'LINE.3
call ansigoto 15,11; say ' ||---+---+---+---| '
call ansigoto 15,12; say ' 'LINE.4
call ansigoto 15,13; say ' ||---+---+---+---| '
call ansigoto 15,14; say ' 'LINE.5
call ansigoto 15,15; say ' ||_______________| '
call ansigoto 15,16; say ' |/______________/  '
RETURN
 
 
BYE:
call ansigoto 1,22
EXIT

