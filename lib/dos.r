/*
 *      Library with DOS routines
 *      Vasilis.Vlachoudis@cern.ch 1998
 */
parse source . calltype .
if calltype ^== "COMMAND" then return

exit 1

/*
 * Seg(name)
 * returns the segment in hex
 */
Seg:
        return d2x(arg(1)%16)

/*
 * Ofs(name)
 * returns the offset in hex
 */
Ofs:
        return d2x(arg(1)//16)

/*
 * IntAddr(int)
 * returns the address of an interrupt
 */
IntAddr: procedure
	addr = c2x(reverse(storage(arg(1)*4,4)))
	seg = x2d(left(addr,4))
	ofs = x2d(right(addr,4))
return seg*16 + ofs

/* --- AsciiZ(asciizstring) --- */
AsciiZ: procedure
	p = pos('00'x,arg(1))
	if p>1
		then return left(arg(1),p-1)
		else return arg(1)

/*
 * Attr2Str(attr)
 * returns a string of file attributes 
 * attributes are:
 *      R - Read Only
 *      H - Hidden
 *      S - System
 *      V - Volume
 *      D - Directory
 *      A - Archive
 */
Attr2Str: procedure
	parse arg attr
        p = 1
        atr = ""
        do i=1 to 6
                if bitand(attr,d2c(p)) ^== '00'x
                        then atr = atr || substr('RHSVDA',i,1)
                        else atr = atr || "-"
                p = p*2
        end
return atr

/*
 * Str2Attr(attr)
 * returns a attribute from string
 */
Str2Attr: procedure
	arg attr
	hexattr = "00"x
	do i=1 to length(attr)
		a = substr(attr,i,1)
		p = pos(a,'RHSVDA')
		if p > 0 then do
			bit = d2x(2**(p-1))
			hexattr = bitor(hexattr,bit)
		end; else
			return "ERROR"
	end
return c2x(hexattr)

/*
 * SetDta()
 * setups the dta space as a variable of size 128
 * in the main pool as a variable '#dta'
 * Must be called once only before calling the FindFirst
 * FindNext functions
 */
SetDta: procedure
        /* init value in the main pool 0 */
        call value '#dta', copies('-',128), 0
        seg = Seg(addr('#dta',,0))
        ofs = Ofs(addr('#dta',,0))
        /* call dos to set dta */
        call intr '21'h, 'AX=1A00 DX='ofs 'DS='seg
return

/*
 * Dta2Str
 * translates the #dta to a readable string
 */
Dta2Str: procedure
        attr = substr(value('#dta',,0),'16'h,1)
        time = substr(value('#dta',,0),'17'h,2)
        date = substr(value('#dta',,0),'19'h,2)
        size = substr(value('#dta',,0),'1B'h,4)
        file = substr(value('#dta',,0),'1F'h,13)

        file = left(asciiz(file),13)

        if bitand(attr,'10'x) ^== '00'x
                then size = "<DIR>"
                else size = c2d(reverse(size))
        size = right(size,6)

        time = c2d(reverse(time))
        hour = left(time%2048,2)
        time = time//2048
        min  = left(time%32,2,0)
        sec  = left((time//32)%2,2,0)

        date = c2d(reverse(date))
        year = date%512 + 1980
        date = date//512
        month = right(date%32,2,0)
        day  = right(date//32,2)
return file size day"/"month"/"year hour":"min":"sec Attr2Str(attr)

/*
 * FindFirst(pattern[,attributes])
 * desc: returns the first file found
 *       in format given by formatdta()
 * pattern can be any normal dos search pattern ie.  '*.*' '??XX.R' etc.
 * attributes is a string ReadOnly,Hidden,System,Volume,Directory or Archive
 * ie.   call FindFirst('*.*','HDA')
 *      find all Hidden, Directories and Archives
 * returns the File Data
 *      or in error "NOTFOUND"
 *      or "ERROR" when an attribute is wrong
 */
FindFirst: procedure
        arg target,attr
        /* make it an ASCIIZ string */
        target = target || "00"x
        seg = Seg(addr('target'))
        ofs = Ofs(addr('target'))

        if length(attr) = 0 then
                hexattr = "0020"
        else do
        	hexattr = Str2Attr(attr)
        	if hexattr = "ERROR" then return "ERROR"
        end

        regs = intr('21'h,'AX=4E00 CX='hexattr 'DX='ofs 'DS='seg)
        parse var regs 'AX=' ax .
        select
                when ax="0002" then return "File Not Found"
                when ax="0003" then return "Path Not Found"
                when ax="0012" then return "END"
        end
return Dta2Str()

/*
 * FindNext()
 * return the next file data otherwise returns 'END'
 */
FindNext: procedure
        regs = intr('21'h,'AX=4F00')
        parse var regs 'AX=' ax .
        if ax="0012" then return "END"
return Dta2Str()

/*
 * DiskReset()
 * This function flushes all file buffers but does not
 * close files.
 */
DiskReset: procedure
        call intr '21'h,'AX=0D00'
return

/*
 * GetCurDisk()
 * returns current disk name
 */
GetCurDisk: procedure
        parse value intr('21'h,"AX=1900") with "AX=" ax .
return d2c(x2d(right(ax,2))+c2d('A'))

/*
 * SelectDrive(drive)
 * Select default drive 'A'..'Z'
 * returns the number of logical drives in the system
 */
SelectDrive: procedure
        arg drive
        drive = d2x(c2d(left(drive,1))-c2d('A'))
        parse value intr('21'h,"AX=0E00 DX=00"drive) with "AX=" ax .
return x2d(right(ax,2))

/*
 * DosVersion()
 * returns dos version
 */
DosVersion: procedure
        parse value intr('21'h,"AX=3000") with "AX=" ax .
return x2d(right(ax,2))'.'x2d(left(ax,2))

/*
 * GetCurDir([drive])
 * returns current directory for drive 'drive' if exists, otherwise
 * for current drive
 */
GetCurDir: procedure
	if arg(1,'E') then do
	        arg drive
		drive = d2x(c2d(left(drive,1))-c2d('A')+1)
	end; else
		drive = "00"
	path = copies(' ',64)	/* 64 bytes buffer */
	seg  = Seg(addr('PATH'))
	ofs  = Ofs(addr('PATH'))
	regs = intr('21'h,"AX=4700 DX=00"drive "DS="seg "SI="ofs)
	parse var regs "FLAGS=" flags .
	if pos('C',flags) > 0 then return "ERROR"
return Asciiz(path)

/*
 * ChDir(newdir)
 * Change directory
 * returns 0 if everything is ok, otherwise 1
 */
ChDir: procedure
	arg dir
	dir = dir || "00"x
	seg  = Seg(addr('DIR'))
	ofs  = Ofs(addr('DIR'))
	regs = intr('21'h,"AX=3B00 DS="seg "DX="ofs)
	parse var regs "FLAGS=" flags .
return pos('C',flags)^=0

/*
 * DriveSpace([drive])
 * returns in bytes the free space for drive 'drive'
 * and the total space
 */
DriveSpace: procedure
	if arg(1,'E') then do
	        arg drive
		drive = d2x(c2d(left(drive,1))-c2d('A')+1)
	end; else
		drive = "00"
	regs = intr('21'h,"AX=3600 DX=00"drive)
	parse var regs "AX=" sc . /* sectors per cluster*/
	if sc = "FFFF" then return "ERROR"	/* invalid drive */
	parse var regs "BX=" ac . /* available clusters	*/
	parse var regs "CX=" bs . /* bytes per sector	*/
	parse var regs "DX=" tc . /* total number of clusters on drive */
	sc = x2d(sc,8)
	ac = x2d(ac,8)
	bs = x2d(bs,8)
	tc = x2d(tc,8)
return ac*sc*bs tc*sc*bs

/*
 * FileAttr(file[,attr])
 * get/set file attributes
 */
FileAttr: procedure
	arg file
	file = file || "00"x
	seg  = Seg(addr('FILE'))
	ofs  = Ofs(addr('FILE'))
	if arg(2,'E') then do	/* set attributes */
		hexattr = str2attr(arg(2))
		if hexattr=="ERROR" then return "ERROR"
		regs = intr('21'h,"AX=4301 CX="hexattr "DS="seg "DX="ofs)
	end; else
		regs = intr('21'h,"AX=4300 DS="seg "DX="ofs)
	parse var regs "FLAGS=" flags .
	if pos('C',flags)>0 then do
		parse var regs "AX=" ax .
		return "ERROR" ax
	end
	parse var regs "CX=" attr .
return Attr2Str(x2c(right(attr,2)))

/*
 * MachineName()
 * Returns the machine name
 */
MachineName: procedure
	buffer = copies(' ',16)
	seg  = Seg(addr('BUFFER'))
	ofs  = Ofs(addr('BUFFER'))
	regs = intr('21'h,"AX=5E00 DS="seg "DX="ofs)
	parse var regs "CX=" +3 ch +2
	if ch=="00" then return "Not defined"
return asciiz(buffer)
