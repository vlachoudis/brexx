#!/bin/env rexx
fin  = open("api-calls.txt","r")
fout = open("../lib/androidapi.r","w")

call lineout fout,"/* --- Android API CALLS --- */"
call lineout fout,"/* Automatically generated */"
do forever
	line = read(fin)
	if eof(fin) then leave
	parse var line method ret "=" args
	call lineout fout,method": procedure"
	call lineout fout,'	method="'method'"'
	call lineout fout,'	signal _apiCall'
	call lineout fout
end

call lineout fout,"_apiCall:"
call lineout fout,"	n = arg()"
call lineout fout,"	select"
call lineout fout,"		when n=0 then return Android(method)"
call lineout fout,"		when n=1 then return Android(method,arg(1))"
call lineout fout,"		when n=2 then return Android(method,arg(1),arg(2))"
call lineout fout,"		when n=3 then return Android(method,arg(1),arg(2),arg(3))"
call lineout fout,"		when n=4 then return Android(method,arg(1),arg(2),arg(3),arg(4))"
call lineout fout,"		when n=5 then return Android(method,arg(1),arg(2),arg(3),arg(4),arg(5))"
call lineout fout,"		when n=6 then return Android(method,arg(1),arg(2),arg(3),arg(4),arg(5),arg(6))"
call lineout fout,"		when n=7 then return Android(method,arg(1),arg(2),arg(3),arg(4),arg(5),arg(6),arg(7))"
call lineout fout,"		when n=8 then return Android(method,arg(1),arg(2),arg(3),arg(4),arg(5),arg(6),arg(7),arg(8))"
call lineout fout,"		when n=9 then return Android(method,arg(1),arg(2),arg(3),arg(4),arg(5),arg(6),arg(7),arg(8),arg(9))"
call lineout fout,"	end"
call close fin
call close fout
