copy ..\src\*.c
copy ..\lstring\*.c
del rxmysql.c
copy mvs_config.h config.h

for %%i in (*.c) do call runjcc %%i
for %%i in (lstring\*.c) do call runjcc %%i

@del plink.txt
for %%i in (*.c) do @echo %%~ni.obj>> plink.txt
for %%i in (lstring\*.c) do @echo %%~ni.obj>> plink.txt

c:\jcc\prelink -s c:\jcc\objs\rent mvsrexx.obj -plink.txt
