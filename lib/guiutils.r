/*
|| RXCONIO.C based utility library
||
|| Version  : 0.9 - 12/02/1999
|| Author   : Generoso Martello
|| E-Mail   : generoso@martello.com
|| Location : Catanzaro Lido (ITALY)
||
*/
say 'This file is meant to be used as an external library.'
exit

/*
|| This is a very useful function from Amiga ARexx
*/
compress: procedure
parse arg var,str
do yy=0 for length(str)
	chr=substr(str,yy+1,1)
	xx=index(var,chr)
	do while xx>0
		var=delstr(var,xx,1)
		xx=index(var,chr)
	end
end
return var

/*
|| Others functions based on RXCONIO.C
*/

ansiClear: procedure expose prg_titleid prg_version
call ansiColor 'WHITE','BLACK' ; call ansiCls
call ansiColor 'YELLOW','BLUE'
call write 1,center(prg_titleid' 'prg_version,80)
call ansiColor 'WHITE','BLACK'
return 0

ansiBox: procedure
parse arg x,y,dx,dy
call ansiGoto x,y
call write 1,d2c(201)||copies(d2c(205),dx)||d2c(187)
do n=0 for dy
	call ansiGoto x,y+n+1
	call write 1,d2c(186)||copies(' ',dx)||d2c(186)
end
call ansiGoto x,y+n+1
call write 1,d2c(200)||copies(d2c(205),dx)||d2c(188)
return 0

ansiEBox: procedure
parse arg x,y,dx,dy
do n=0 for dy+2
	call ansiGoto x,y+n
	call write 1,copies(' ',dx+2)
end
return 0

ansiOLine: procedure
parse arg x,y,dx,c
call ansiGoto x,y
call write 1,copies(c,dx)
return 0

ansiProgressBar: procedure
parse arg x,y,px,mx
dx=trunc(mx*(px/100))
call ansiGoto x,y
call write 1,copies(d2c(176),mx)
call ansiGoto x,y
call write 1,copies(d2c(178),dx)
call ansiGoto x+mx+1,y
call write 1,right(' ('trunc(px)'%)',6)
call ansiGoto 1,24
return 0

ansiVLine: procedure
parse arg x,y,dy,c
do n=0 for dy-1
	call ansiGoto x,y+n
	call write 1,c
end
return 0

ansiMenu: procedure
parse arg x,y,dx,dy,title
call ansiColor 'WHITE','BLUE'
call ansiBox x,y,dx,dy
call ansiColor 'YELLOW','RED'
call ansiGoto x+1,y+1
call write 1,center(title,dx)
call ansiColor 'BLUE','BLACK'
call ansiOLine x-1,y+dy+2,dx+4,' '
call ansiOLine x-1,y-1,dx+4,' '
call ansiVLine x-1,y,dy+3,' '
call ansiVLine x+dx+2,y,dy+3,' '
return 0

ansiList: procedure expose GUlist.
parse arg x,y,dx,dy,cr,ci,title,opt,ls,style,sym
call ansiMenu x,y,dx,dy,title
call ansiColor 'YELLOW','YELLOW'
call ansiGoto x+1,y+dy
call write 1,center(opt,dx)
valid_opt=''
do while index(opt,'=')>0
	parse var opt o'='dsc'|'opt
	if translate(o)='INVIO' | translate(o)='ENTER' then o=d2c(13)
	else if translate(o)='ESC' then o=d2c(27)
	valid_opt=valid_opt||o
end
valid_opt=compress(valid_opt,' ')
done=0
do while ~done
	call ansiColor 'YELLOW','YELLOW'
	call ansiGoto x+dx-8,y+dy ; call write 1,right((ci+1)||'/'value(ls'.count'),8)
	n=0
	do while n<dy-2 & n<value(ls'.count')
		m=n+ci-cr
		call ansiGoto x+2,y+2+n
		if cr=n then call ansiColor 'BLACK','WHITE'
		else do
			call ansiColor 'WHITE','BLUE'
		end
		if sym='' then do
			if index(style,'I')>0 then call write 1,left((m+1)||') '||value(ls'.'m),dx-2)
			else call write 1,left(value(ls'.'m),dx-2)
		end
		else do
			if index(style,'I')>0 then call write 1,left(right((m+1),3)||') '||value(ls'.'m'.'sym),dx-2)
			else call write 1,left(value(ls'.'m'.'sym),dx-2)
		end
		n=n+1
	end
	call ansiGoto 1,24
	cc=getch(); if c2d(cc)=0 then cc='0'getch()
	if cc='0H' & ci>0 then do
		ci=ci-1
		if cr>0 then cr=cr-1
	end
	else if cc='0P' & ci+1<value(ls'.count') then do
		ci=ci+1
		if cr<dy-3 then cr=cr+1
	end
	else if cc='0I' then do
		ci=ci-(dy-3) ; if ci<0 then ci=0
		cr=0
	end
	else if cc='0Q' then do
		ci=ci+(dy-3) ; if ci>(value(ls'.count')-1) then ci=(value(ls'.count')-1)
		cr=dy-3 ; if cr>(value(ls'.count')-1) then cr=(value(ls'.count')-1)
	end
	else if index(translate(valid_opt),translate(cc))>0 then done=1
end
return ci' 'cr' 'translate(cc)

ansiRequest: procedure
parse arg x,y,dx,title,request,t,f
if c2d(left(t,1))=13 then call ansiMenu x,y,dx,4,title
else call ansiMenu x,y,dx,6,title
call ansiColor 'WHITE','BLUE'
call ansiGoto x+1,y+3
call write 1,center(request,dx)
if c2d(left(t,1))<>13 then do
	call ansiColor 'YELLOW','YELLOW'
	call ansiGoto x+3,y+5
	call write 1,' 't' '
	call ansiGoto x+dx-length(f)-3,y+5
	call write 1,' 'f' '
end
ans=-1
do while ans=-1
	cc=getch()
	if      translate(cc)=left(t,1) then ans=1
	else if translate(cc)=left(f,1) then ans=0
end
return ans

ansiInputLine: procedure
parse arg x,y,z,lmin,lmax,ans
if length(ans)>(lmax*z) then ans=left(ans,(lmax*z))
do k=0 for z
	call ansiGoto x,y+k ; call write 1,copies(' ',lmax+1)
	call ansiGoto x,y+k ; call write 1,substr(ans,(lmax*k)+1,lmax)
end
call ansiGoto x+(length(ans) // (lmax+1))+(trunc(length(ans) / (lmax+1))), y+trunc(length(ans) / (lmax+1))
call ansiSetCursor("SOLID")
cc=''
do while (c2d(cc)~=13 & c2d(cc)~=10) | (length(ans)<lmin)
	cc=getch()
	if (length(ans)<(lmax*z)) & (c2d(cc)~=13) & ((c2d(cc)>=32 & c2d(cc)<=126)) then do
		call ansiGoto x+(length(ans) // lmax), y+trunc(length(ans) / lmax)
		ans=ans||cc
		call write 1,cc
	end
	else if (cc=d2c(8) & length(ans)>0) then do
		ans=left(ans,length(ans)-1)
		call ansiGoto x+(length(ans) // lmax), y+trunc(length(ans) / lmax)
		call write 1,' '
		call ansiGoto x+(length(ans) // lmax), y+trunc(length(ans) / lmax)
	end
	else if c2d(cc)=0 then do
		cc=getch()
		cc=''
	end
end
call ansiSetCursor("OFF")
return ans
