arg file options .
parse source system .
if file = '' then do 
	say 'syntax: soundex.r [directory\]name options'
	say 'searches directory for a file that the FILENAME sounds like "NAME"'
	say 'options can be "S" for subdirectory'
	if system == "MSDOS" then say '4DOS is required!'
	exit 
end
if system == "UNIX"
	then slash = "/"
	else slash = "\"
backslash = lastpos(slash,file)
dir = ''
if backslash<>0 then dir = left(file,backslash)
file = substr(file,backslash+1)
soundfile = soundex(file)

if system == "UNIX" then do
	if left(options,2) = '-S'
			then options = '-R'
			else options = ''
	"ls" options dir "(stack"
end; else do
	if left(options,2) = '/S'
			then options = '/S'
			else options = ''
	'*dir /B' dir options '(stack'
end

do queued()
	parse pull realfile
	file = translate(realfile)
	bs = lastpos(slash,file)
	dir = left(file,bs)
	file = substr(file,bs+1)
	parse var file fn '.' ft
	if soundex(fn) = soundfile then say dir || realfile
end
