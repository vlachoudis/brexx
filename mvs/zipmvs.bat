del brxinc.zip
del brxsrc.zip
del brxjcl.zip
del all.zip
zip -9 -X -ll -j brxsrc.zip *.c ..\src\*.c ..\lstring\*.c
zip -9 -X -ll -j brxinc.zip ..\inc\*.h -x ..\inc\config.h
copy mvs_config.h config.h
zip -9 -X -ll -j brxinc.zip config.h
zip -9 -X -ll -j brxjcl.zip *.jcl *.m4 *.txt ..\COPYING
zip -9 -X all *.zip
