rem gcc -g -w -c -nostdinc -D__WIN32__ -DHAVE_CONFIG_H -I ../../pdos/pdpclib -I . -I ../inc %1 %2 %3 %4 %5 %6 %7 %8 %9
rem gcc -w -c -ansi -DHAVE_CONFIG_H -I . %1 %2 %3 %4 %5 %6 %7 %8 %9
rem bcc32 -w- -A -c -DHAVE_CONFIG_H=1 -I. %1 %2 %3 %4 %5 %6 %7 %8 %9
rem wcl386 -3s -zq -e=1 -za -w- -c -Dopen=xxopen -DHAVE_CONFIG_H -I. %1 %2 %3 %4 %5 %6 %7 %8 %9
gccmvs -Os -S -DHAVE_CONFIG_H -I ../../pdos/pdpclib -I . -I ../inc %1 %2 %3 %4 %5 %6 %7 %8 %9
