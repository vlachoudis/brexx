/*
 * EditLine Library
 * Vasilis.Vlachoudis@cern.ch	(c) 2002
 */
/* --- Edit a single line --- */
EditLine: procedure expose history. timeout
	line = arg(1)
	parse source os calltype .
	if os=="UNIX" | calltype=="COMMAND" then do
		call write ,line
		parse pull line
		return line
	end

	cur = length(line)
	h = history.0+1
	x = wherex()
	y = wherey()

	do forever
		call gotoxy x,y
		call write ,line
		call clreol
		cur = max(0,min(cur,length(line)))
		call gotoxy x+cur,y
		if arg(2,'E') then do
			timeout = arg(2)
			call time 'R'
			do while time('E')<timeout
				if kbhit() then do
					timeout = 0
					leave
				end
			end
			if timeout<>0 then do
				timeout = 1
				return line
			end
		end
		ch = getch()
		select
			when ch=="00"x then do
				ch=getch()
				select
					when ch=='23'x then cur = length(line)
					when ch=='24'x then cur = 0
					when ch=='25'x then cur = cur-1
					when ch=='26'x then do
						h = max(h-1,1)
						line = history.h
						cur = length(line)
					end
					when ch=='27'x then cur = cur+1
					when ch=='28'x then do
						h = min(h+1,history.0+1)
						line = history.h
						cur = length(line)
					end
					when ch=='2E'x then
						if cur=length(line) then do
								if cur>0 then do
									line = delstr(line,cur,1)
									cur = cur-1
								end
							end; else line = delstr(line,cur+1,1)
				end
			end
			when ch=="08"x then
				if cur>0 then do
					line = delstr(line,cur,1)
					cur = cur-1
				end
			when ch=="0D"x then return line
			when ch=="1B"x then do
				line = ""; cur=0
			end
			otherwise
				line = Insert(ch,line,cur)
				cur = cur+1
		end
	end

/* --- Initialize Edit Line --- */
EditLineInit:	procedure expose history.
	parse arg max
	history. = ""
	history.0 = 0
	if ^datatype(max,"N") then max = 20
	history._MAX = max
return

/* --- SetHistoryMax --- */
SetHistoryMax: procedure expose history.
	parse arg max
	if ^datatype(max,"N") then max = 20
	history._MAX = max
return

/* --- HistoryList --- */
HistoryList: procedure expose history.
	do i=1 to history.0
		say history.i
	end
return

/* --- Add2History --- */
Add2History: procedure expose history.
	parse arg cmdline
	h = history.0
	if cmdline==history.h then return
	h = h + 1
	if h>=history._MAX then do
		do i=1 to history.0-1
			j=i+1
			history.i = history.j
		end
		h=h-1
	end
	history.0 = h
	history.h = cmdline
return

