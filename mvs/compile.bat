del *.o
del *.obj
del *.exe

copy mvs_config.h config.h
pushd ..\inc
ren config.h config.i
popd

call stdcomp ../src/address.c
call stdcomp ../src/bintree.c
call stdcomp ../src/bmem.c
call stdcomp ../src/builtin.c
call stdcomp ../src/compile.c
call stdcomp ../src/dqueue.c
call stdcomp ../src/error.c
call stdcomp ../src/expr.c
call stdcomp ../src/interpre.c
call stdcomp ../src/main.c
call stdcomp ../src/nextsymb.c
call stdcomp ../src/rexx.c
call stdcomp ../src/rexxfunc.c
call stdcomp ../src/rxconv.c
call stdcomp ../src/rxfiles.c
call stdcomp ../src/rxmath.c
call stdcomp ../src/rxstr.c
call stdcomp ../src/rxword.c
call stdcomp ../src/stack.c
call stdcomp ../src/template.c
call stdcomp ../src/trace.c
call stdcomp ../src/variable.c

call stdcomp ../lstring/abbrev.c
call stdcomp ../lstring/abs.c
call stdcomp ../lstring/add.c
call stdcomp ../lstring/atan2.c
call stdcomp ../lstring/bitand.c
call stdcomp ../lstring/bitor.c
call stdcomp ../lstring/bitxor.c
call stdcomp ../lstring/bool.c
call stdcomp ../lstring/b2x.c
call stdcomp ../lstring/center.c
call stdcomp ../lstring/changest.c
call stdcomp ../lstring/charin.c
call stdcomp ../lstring/charout.c
call stdcomp ../lstring/chars.c
call stdcomp ../lstring/compare.c
call stdcomp ../lstring/copies.c
call stdcomp ../lstring/countstr.c
call stdcomp ../lstring/c2d.c
call stdcomp ../lstring/c2x.c
call stdcomp ../lstring/datatype.c
call stdcomp ../lstring/date.c
call stdcomp ../lstring/dec.c
call stdcomp ../lstring/delstr.c
call stdcomp ../lstring/delword.c
call stdcomp ../lstring/div.c
call stdcomp ../lstring/d2c.c
call stdcomp ../lstring/d2x.c
call stdcomp ../lstring/equal.c
call stdcomp ../lstring/errortxt.c
call stdcomp ../lstring/expose.c
call stdcomp ../lstring/format.c
call stdcomp ../lstring/hashvalu.c
call stdcomp ../lstring/inc.c
call stdcomp ../lstring/index.c
call stdcomp ../lstring/insert.c
call stdcomp ../lstring/intdiv.c
call stdcomp ../lstring/justify.c
call stdcomp ../lstring/lastpos.c
call stdcomp ../lstring/linein.c
call stdcomp ../lstring/lineout.c
call stdcomp ../lstring/lines.c
call stdcomp ../lstring/lmath.c
call stdcomp ../lstring/lower.c
call stdcomp ../lstring/lstring.c
call stdcomp ../lstring/mod.c
call stdcomp ../lstring/mult.c
call stdcomp ../lstring/neg.c
call stdcomp ../lstring/overlay.c
call stdcomp ../lstring/pow.c
call stdcomp ../lstring/print.c
call stdcomp ../lstring/read.c
call stdcomp ../lstring/reverse.c
call stdcomp ../lstring/right.c
call stdcomp ../lstring/sign.c
call stdcomp ../lstring/soundex.c
call stdcomp ../lstring/space.c
call stdcomp ../lstring/stderr.c
call stdcomp ../lstring/strip.c
call stdcomp ../lstring/sub.c
call stdcomp ../lstring/substr.c
call stdcomp ../lstring/subword.c
call stdcomp ../lstring/time.c
call stdcomp ../lstring/translat.c
call stdcomp ../lstring/trunc.c
call stdcomp ../lstring/upper.c
call stdcomp ../lstring/verify.c
call stdcomp ../lstring/word.c
call stdcomp ../lstring/wordidx.c
call stdcomp ../lstring/wordlen.c
call stdcomp ../lstring/wordpos.c
call stdcomp ../lstring/words.c
call stdcomp ../lstring/write.c
call stdcomp ../lstring/xrange.c
call stdcomp ../lstring/x2b.c
call stdcomp ../lstring/x2c.c
call stdcomp ../lstring/x2d.c

call stdcomp extra.c

rem gcc -o brexx.exe -nostdlib *.o ../pdos/pdpclib/pdpwin32.a -lkernel32 -lgcc
rem gcc -o brexx.exe *.o
rem bcc32 -ebrexx.exe *.obj
rem wcl386 -zq -e=1 -za -w- -fe=brexx.exe *.obj

pushd ..\inc
ren config.i config.h
popd
