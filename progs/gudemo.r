/*
||
|| A simple "Phone Book" BRexx program
|| Just an example of "guiutils.r" library usage.
||
|| 02/05/1999 by Generoso Martello
||               generoso@martello.com
||               Catanzaro Lido (ITALY)
||
*/

call load 'guiutils.r'

rxEnter=d2c(13)
rxEsc=d2c(27)

call loadPhoneBook()

/*
|| Main program... isn't it very short ? :)
*/
fr=0 ; op=0 ; ch='' ; call ansiSetCursor('OFF')
do while ch~=rxEsc

	call ansiColor('LIGHTCYAN','BLACK')
	call ansiCls

	call ansiMenu 42,4,35,6,'Command List'
	call ansiColor('LIGHTCYAN','BLUE')
	call ansiGoto 54,6  ; say '    A = Add'
	call ansiGoto 54,7  ; say '    E = Edit'
	call ansiGoto 54,8  ; say '    D = Delete'
	call ansiGoto 54,9  ; say 'Enter = View'
	call ansiGoto 54,10 ; say '  Esc = Exit'

	call ansiColor('WHITE','BLACK')

	call ansiGoto 13,1  ; say 'PHONE BOOK v 1.0'
	call ansiGoto 11,2  ; say 'by Generoso Martello'
	call ansiGoto 42,14 ; say 'This is an example application which'
	call ansiGoto 42,15 ; say 'uses my "guiutils.r" library.'
	call ansiGoto 42,17 ; say 'The library is supposed to work with'
	call ansiGoto 42,18 ; say 'the 32-bit DOS version of Brexx.'
	call ansiGoto 42,20 ; say 'Through this library you can "dress"'
	call ansiGoto 42,21 ; say 'your programs with colourful menus,'
	call ansiGoto 42,22 ; say 'lists, input boxes, requesters...'
	call ansiGoto 42,23 ; say '... Enjoy it! :)'

	ch=ansiList(2,4,35,19,fr,op,'Available Phone Books','A=Add|E=Edit|D=Delete|Enter=Open|Esc=Exit','GUlist','I','')

	op=subword(ch,1,1) ; fr=subword(ch,2,1) ; ch=subword(ch,3,1)

	if ch=rxEnter & GUlist.count>0 then do
		fr1=0 ; op1=0 ; ch=''
	do while ch~=rxEsc
		ch=ansiList(20,12,55,10,fr1,op1,'"'GUlist.op'" Phone Book','A=Add|E=Edit|D=Delete|Enter=View|Esc=Exit','GUlist.'op,'','')
		op1=subword(ch,1,1) ; fr1=subword(ch,2,1) ; ch=subword(ch,3,1)
		if ch='A' | (ch='E' & GUlist.op.count>0) then do
			if ch='A' then do
				name=''
				phon='+39-'
				note=''
			end
			else do
				parse var GUlist.op.op1 name' ³ 'phon' ³ 'note
				name=strip(name)
				phon=strip(phon)
				note=strip(note)
			end
			call ansiMenu 20,12,55,10,'Add/Modify Record - "'GUlist.op'"'
			call ansiColor 'WHITE','BLUE'
			call ansiGoto 22,15 ; say 'First Name and Last Name'
			call ansiGoto 22,17 ; say 'Telephone number'
			call ansiGoto 22,19 ; say 'Note'
			call ansiColor 'BLACK','WHITE'
			call ansiOLine 22,16,53,' '
			call ansiOLine 22,18,53,' '
			call ansiOLine 22,20,53,' '
			call ansiOLine 22,21,53,' '
			call ansiGoto 22,16 ; say name'ee'
			call ansiGoto 22,18 ; say phon
			call ansiGoto 22,20 ; say substr(note,1,52)
			call ansiGoto 22,21 ; say substr(note,53)

			name=ansiInputLine(22,16,1,0,25,name)
			phon=ansiInputLine(22,18,1,0,25,phon)
			note=ansiInputLine(22,20,2,0,52,note)

			if strip(name)~='' then do
				if ch='A' then do
					cn=GUlist.op.count
					GUlist.op.cn=left(name,25)' ³ 'left(phon,25)' ³ 'note
					GUlist.op.count=cn+1
				end
				else	GUlist.op.op1=left(name,25)' ³ 'left(phon,25)' ³ 'note

				/* BUBBLE SORT ... un po' di ordine... :) */
				ordinato = 0 ; do while ~ordinato
					ordinato=1
					do curr=0 for GUlist.op.count-1
						next=curr+1
						if GUlist.op.curr > GUlist.op.next then do
							p=GUlist.op.curr
							GUlist.op.curr=GUlist.op.next
							GUlist.op.next=p
							ordinato=0
						end
					end
				end
			end

		end
		else if ch='D' & GUlist.op.count>0 then do
			if ansiRequest(32,14,30,"ARE YOU SURE ?","Delete this entry ?","Yes","No") then do
				do x=op1+fr1-1 for GUlist.op.count
					next=x+1
					GUlist.op.x=GUlist.op.next
				end
				GUlist.op.count=GUlist.op.count-1
			end
		end
		else if ch=rxEnter & GUlist.op.count>0 then do
			parse var GUlist.op.op1 name' ³ 'phon' ³ 'note
			call ansiMenu 20,12,55,10,'Record Details - Hit any key to exit'
			call ansiColor 'WHITE','BLUE'
			call ansiGoto 22,15 ; say 'First Name and Last Name'
			call ansiGoto 22,17 ; say 'Telephone number'
			call ansiGoto 22,19 ; say 'Note'
			call ansiColor 'BLACK','WHITE'
			call ansiOLine 22,16,53,' '
			call ansiOLine 22,18,53,' '
			call ansiOLine 22,20,53,' '
			call ansiOLine 22,21,53,' '
			call ansiGoto 22,16 ; say name
			call ansiGoto 22,18 ; say phon
			call ansiGoto 22,20 ; say substr(note,1,52)
			call ansiGoto 22,21 ; say substr(note,53)
			dummy=getch()
		end
	end
		ch=''
	end
	else if ch='A' | (ch='E' & GUlist.count>0) then do
		if ch='A' then name=''
		else name=GUlist.op
		call ansiMenu 20,12,32,5,'Add/Modify Phone Book'
		call ansiColor 'WHITE','BLUE'
		call ansiGoto 22,15 ; say 'Phone Book Name'
		call ansiColor 'BLACK','WHITE'
		name=ansiInputLine(22,16,1,0,30,name)
		if strip(name)~='' then do
			if ch='A' then do
				cn=GUlist.count
				GUlist.cn=name ; GUlist.cn.count=0
				GUlist.count=cn+1
			end
			else GUlist.op=name
		end
	end
	else if ch='D' & GUlist.count>0 then do
		if ansiRequest(26,12,30,"ARE YOU SURE ?","Delete this section ?","Yes","No") then do
			GUlist.op.flag='D'
			call savePhoneBook()
			call loadPhoneBook()
			fr=0 ; op=0
		end
	end
end

byebebe:
call ansiColor 'BLACK','BLACK'
call ansiBox 42,12,35,11
call savePhoneBook()
call ansiColor 'LIGHTRED','BLACK'
call ansiGoto 42,20 ; say 'Done! Press any key to exit.'
dummy=getch()
call ansiColor('WHITE','BLACK') ; call ansiCls
call ansiSetCursor("NORMAL")
exit

/*
|| Loads my phonebook data
*/
loadPhoneBook: procedure expose GUlist.
lc=0
fd=open('gudemo.dat','r')
if fd>0 then do
	l=read(fd)
	do while ~eof(fd)
		if l~='' then GUlist.lc=l
		ic=0 ; if ~eof(fd) then l=read(fd)
		do while subword(l,1,1)~='#' & ~eof(fd)
			GUlist.lc.ic=l ; ic=ic+1
			l=read(fd)
		end
		GUlist.lc.count=ic ; lc=lc+1
		if ~eof(fd) then l=read(fd)
	end
	call close fd
end
GUlist.count=lc
return 0

/*
|| Saves my phonebook data
*/
savePhoneBook: procedure expose GUlist.
fd=open('gudemo.dat','w')
if fd>0 then do
	call ansiMenu 14,11,56,6,'Updating Phone Book data...'
	call ansiColor 'YELLOW','BLUE'
	call ansiGoto 16,14 ; say 'Sections'
	call ansiGoto 16,16 ; say ' Records'
	do x=0 for GUlist.count
		call ansiProgressBar 25,14,((x+1)*100)/GUlist.count,38
		if GUlist.x.flag~='D' then do
			call write fd,GUlist.x,'nl'
			do y=0 for GUlist.x.count
				call ansiProgressBar 25,16,((y+1)*100)/GUlist.x.count,38
				call write fd,GUlist.x.y,'nl'
			end
			call write fd,'#','nl'
		end
	end
	call close fd
end
return 0
