del all.zip
zip -0 -X -ll -j all.zip *.c ..\src\*.c ..\lstring\*.c
zip -0 -X -ll -j all.zip ..\inc\*.h
zip -0 -X -ll -j all.zip ..\mvs\*.c ..\mvs\*.h *.txt *.exec *.parm ..\COPYING
copy ..\mvs\mvs_config.h config.h
zip -0 -X -ll -j all.zip config.h *.c *.asm *.rexx
