c:\jcc\jcc -r -z20000 -Ic:\jcc\include -D__MVS__ -I . -I..\inc %1

@if ERRORLEVEL 1 goto wait
@goto exit
:wait
pause
:exit
