/*
        Library with for HTML CGI-SCRIPTING routines		2002
        Vasilis N. Vlachoudis <Vasilis.Vlachoudis@cern.ch>
*/
return

/* --- HTMLInit --- */
HTMLInit: procedure
	parse arg type
	if type = "" then type = "text/html"
	call value "@html.0",0,0
	call value "@htmlheader.0",0,0
	if type<>"no" then call HTMLHeader "Content-type:" type
return

/* --- HTMLVars --- */
HTMLVars:
	if translate(getenv("REQUEST_METHOD")) == "POST" then
		parse value getenv("CONTENT_TYPE") with __mime ";"
	else
		__mime=""

	__htmlvars = ""

	if __mime == "multipart/form-data" then do
		__data = read(0,getenv("CONTENT_LENGTH"))
		/*call write "/tmp/out",__data*/

		parse var __data __boundary "0D0A"x __data
		__boundary="0D0A"x || __boundary

		do forever
			parse var __data 'name="' __name '"' __data
			if __name == "" then leave
			__name=HTMLVarDecode(__name)

			if left(__data,1)==";" then do
				parse var __data 'filename="' __value '"'__data
				if __filename<>"" then do
					call value __name"@name",__value
					__htmlvars = __htmlvars __name"@name"
				end
			end

			parse var __data "0D0A"x __line "0D0A"x __data
			if __line ^== "" then do
				parse var __line ": " __value
				call value __name"@type",__value
				__htmlvars = __htmlvars __name"@type"
				parse var __data "0D0A"x __data
			end

			parse var __data __value (__boundary) __data
			call value __name,__value
			__htmlvars = __htmlvars __name
		end
		drop __data __line __name __value __boundary
	end; else
	if __mime=="application/x-www-form-urlencoded" | __mime=="" then do
		if __mime=="" then
			__vars = getenv("QUERY_STRING")
		else
			__vars = read(0,'F')
		do while __vars <> ""
			parse var __vars __name "=" __value "&" __vars
			if __name <> "" then do
				__name=HTMLVarDecode(__name)
				call value __name,HTMLDecode(__value)
				__htmlvars = __htmlvars __name
			end
		end
		drop __vars __value __name
	end; else do
		say "Content-type: text/html"||"0D0A"x
		say "<P><FONT COLOR=RED>Unknown Content Type="__mime"</FONT>"
		exit
	end
	drop __mime
return __htmlvars

/* --- HTMLCookieVars --- */
HTMLCookieVars:
	__cookies = getenv("HTTP_COOKIE")
	if __cookies="" then do
		drop __cookies
		return
	end
	do while __cookies <> ""
		parse var __cookies __name "=" __value ";" __cookies
		/* Konqueror sends also the version appended with "," */
		parse var __value __value "," .
		__quote = left(__value,1)
		if __quote='"' | __quote="'" then
			__value = substr(__value,2,length(__value)-2)
		call value strip(__name),HTMLDecode(__value)
	end

	drop __cookies __name __value __quote
return

/* --- HTMLSaveCookieVars --- */
HTMLSaveCookieVars:
	__cookies = arg(1)
	do while __cookies <> ""
		parse var __cookies __name __cookies
		if symbol(__name)=="VAR"
			then call HTMLCookie __name,value(__name,,0)
	end
	drop __cookies __name
return

/* --- HTMLDelCookies --- */
HTMLDelCookies: procedure
	cookies = arg(1)
	do while cookies <> ""
		parse var cookies name cookies
		call HTMLCookie name,"","Monday, 01-Jan-91 00:00:00 GMT"
	end
return

/* --- HTMLCookie --- */
HTMLCookie: procedure
	/* Get/Set
	 * send a set-cookie header with the word secure and the cookie will only
	 * be sent through secure connections
	 * Arguments: name, value [,expires] [,path] [,domain] [,secure]
	 */
	if arg()>1 then do
		name = translate(arg(1))
		value = arg(2)

		setCookie = "Set-Cookie: "
		setCookie = setCookie||name'='HTMLEncode(value)
		if arg(3,"E") then setCookie = setCookie||'; expires="'arg(3)'"'
		if arg(4,"E") then setCookie = setCookie||'; path="'arg(4)'"'
		if arg(5,"E") then setCookie = setCookie||'; domain="'arg(5)'"'
		if arg(6,"E") then setCookie = setCookie||'; secure'
		setCookie = setCookie||'; version="1"'

		/* search to find if it is alreay set! */
		n = HTMLFindCookieHeader(name)
		if n>0	then call value "@htmlheader."n,setCookie,0
			else call HTMLHeader setCookie
	 end; else do
		parse arg cookiename
		cookies = getenv("HTTP_COOKIE")
		do while cookies <> ""
			parse var cookies name "=" value ";" cookies
			if cookiename = name then return HTMLDecode(value)
		end
	 end
return

/* --- HTMLFindCookieHeader --- */
HTMLFindCookieHeader: procedure
	n = value("@htmlheader.0",,0)
	target = "Set-Cookie: "arg(1)
	do i=2 to n
		if abbrev(value("@htmlheader."i,,0),target) then return i
	end
return 0

/* --- HTMLEnv --- */
HTMLEnv: procedure
	"env (stack"
	call HTML "<table border>"
	call HTML "<tr><th align='left'>Name</th><th align='left'>Value</th></tr>"
	do queued()
		parse pull var "=" value
		call HTML "<tr><td>"var"</td><td>"value"</td></tr>"
	end
	call HTML "</table>"
return

/* --- HTMLSetNoCache --- */
HTMLSetNoCache: procedure
	call HTMLHeader "Pragma: nocache"
	call HTMLHeader "Cache-Control: no-cache, must-revalidate, no-store"
return

/* --- HTMLHeader --- */
HTMLHeader: procedure
	n = value("@htmlheader.0",,0)+1
	call value "@htmlheader.0",n,0
	call value "@htmlheader."n,arg(1),0
return

/* --- HTML --- */
HTML: procedure
	n = value("@html.0",,0)+1
	call value "@html.0",n,0
	call value "@html."n,arg(1),0
return

/* --- HTMLBR --- */
HTMLBR:
	call HTML arg(1)"<br>"
return

/* --- HTML Append --- */
HTMLAppend: procedure
	n = value("@html.0",,0)
	call value "@html."n,value("@html."n,,0)||arg(1),0
return

/* --- HTMLFlush --- */
HTMLFlush: procedure
	parse arg out
	n = value("@htmlheader.0",,0)
	if n>0 then do
		do i=1 to n-1
			call lineout out,value("@htmlheader."i,,0)
		end
		call lineout out,value("@htmlheader."n,,0)||"0D0A"x
		call write out,"0D0A"x
	end

	n = value("@html.0",,0)
	do i=1 to n
		call lineout out,value("@html."i,,0)
	end
return

/* --- HTMLDecode --- */
HTMLDecode: procedure
	val = translate(arg(1)," ","+")
	new = ""
	do while val <> ""
		parse var val prefix "%" hex +3 val
		new = new||prefix
		if hex<>"" then new=new||x2c(substr(hex,2))
	end
return new

/* --- HTMLVarDecode --- */
HTMLVarDecode: procedure
	var = HTMLDecode(arg(1))
	if datatype(var,"S") then return var
	ref = xrange("a","z")||xrange("A","Z")||"0123456789@%_.!#"
	do forever
		p = verify(var,ref)
		if p>0 then do
			ch = substr(var,p,1)
			var = changestr(ch,var,"_")
		end; else
			return var
	end

/* --- HTMLEncode --- */
HTMLEncode: procedure
	str = arg(1)
	out = ""
	do i=1 to length(str)
		ch = substr(str,i,1)
		if ch==" " then ch="+"
		else
		if ^datatype(ch,"A") then ch="%"c2x(ch)
		out = out || ch
	end
	return out

/* --- HTMLHead --- */
HTMLHead:
	if translate(arg(1))="XML" then do
		call HTML '<?xml version="1.0" encoding="iso-8859-1"?>'
		call HTML '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"'
		call HTML '"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">'
	end
	call HTML '<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en" dir="ltr">'

	call HTML '<head>'
	return

/* --- HTMLBody --- */
HTMLBody: procedure
	arg=arg(1)
	if arg<>"" then arg=" "arg
	call HTML "</head>"
	call HTML "<body"arg">"
	return

/* --- HTMLEnd --- */
HTMLEnd:
	call HTML "</body>"
	call HTML "</html>"
	call HTMLFlush arg(1)
	return

/* --- Tag --- */
Tag: procedure
	line = ""
	do a=1 to arg()-1
		if arg(a,"E") then line=line||"<"arg(a)">"
	end
	line=line||arg(arg())
	do a=arg()-1 to 1 by -1
		if arg(a,"E") then line=line||"</"word(arg(a),1)">"
	end
return line

/* --- HTMLTag --- */
HTMLTag: procedure
	line = ""
	if arg()=1 then
		line = "<"arg(1)">"
	else do
		do a=1 to arg()-1
			if arg(a,"E") then line=line||"<"arg(a)">"
		end
		line=line||arg(arg())
		do a=arg()-1 to 1 by -1
			if arg(a,"E") then line=line||"</"word(arg(a),1)">"
		end
	end
	call HTML line
return

/* --- HTMLTagAppend --- */
HTMLTagAppend: procedure
	line = ""
	do a=1 to arg()-1
		if arg(a,"E") then line=line||"<"arg(a)">"
	end
	line=line||arg(arg())
	do a=arg()-1 to 1 by -1
		if arg(a,"E") then line=line||"</"word(arg(a),1)">"
	end
	call HTMLAppend line
return

/* --- HTMLStr --- */
HTMLStr: procedure
	str = changestr('<',arg(1),"&lt;")
	str = changestr('>',str,"&gt;")
	str = changestr('"',str,"&quot;")
return str

/* --- HTMLInput --- *
 * 1. type (Default = "text")
 * 2. name
 * 3. value (Default = value of name)
 * 4. size
 * 5. extra
 */
HTMLInput:
	__line = "<input type='"
	if arg(1,"E")	then __line = __line||arg(1)"'"
			else __line = __line||"text'"
	if arg(2,"E")	then __line = __line "name='"arg(2)"'"
	__line = __line 'value="'
	if arg(3,"E")	then do
		__line = __line||HTMLStr(arg(3))'"'
		if arg(3)=1 then __line = __line "checked"
	end; else do
		if translate(arg(1))="RADIO" | translate(arg(1))="CHECKBOX" then
			__line = __line||'1" checked'
		else do
			__line = __line||HTMLStr(value(arg(2)))'"'
			if value(arg(2))=1 then __line = __line "checked"
		end
	end

	if arg(4,"E")	then __line = __line "size='"arg(4)"'"
/*
	if translate(arg(1))="RADIO" | translate(arg(1))="CHECKBOX" then do
		if value(arg(2))=1 then
			if arg(3) then __line = __line "checked "
	end
*/
	if arg(5,"E")   then __line = __line arg(5)
	__line = __line">"
return __line

/* --- HTMLGetBool --- */
HTMLGetBool: return symbol(arg(1))^="LIT"

/* --- HTMLMime --- *
 * return the mime type
 */
HTMLMime: procedure expose mime.
	if mime. <> "" then call HTMLMimeInit
	parse upper arg filename
	ext = substr(filename,lastpos(".",filename)+1)
	if ext = "" then return "application/octet-stream"
return mime.ext

/* --- HTMLOption --- *
 * HTMLOption(Name[,Value[,1|0 selected]]);
 */
HTMLOption: procedure
	line = '<option'
	if arg(2,"E") then
		line = line 'value="'arg(2)'"'
	if arg(3,"E") then
		if arg(3)=1 then
			line = line 'selected'
return line'>'arg(1)'</option>'

/* --- HTMLMimeInit --- */
HTMLMimeInit: procedure expose mime.
	MIME.          = ""
	MIME.PPZ       = 'application/mspowerpoint'
	MIME.PPT       = 'application/mspowerpoint'
	MIME.AI        = 'application/illustrator'
	MIME.XLS       = 'application/msexcel'
	MIME.XLC       = 'application/msexcel'
	MIME.XLL       = 'application/msexcel'
	MIME.XLM       = 'application/msexcel'
	MIME.XLW       = 'application/msexcel'
	MIME.DOC       = 'application/msword'
	MIME.PWD       = 'application/msword'
	MIME.PS        = 'application/postscript'
	MIME.PDF       = 'application/pdf'
	MIME.AG        = 'application/x-applixgraphics'
	MIME.SMIL      = 'application/smil'
	MIME.SMI       = 'application/smil'
	MIME.WPD       = 'application/wordperfect'
	MIME.ABW       = 'application/x-abiword'
	MIME.ABW.GZ    = 'application/x-abiword'
	MIME.ZABW      = 'application/x-abiword'
	MIME.SAM       = 'application/x-amipro'
	MIME.P7S       = 'application/x-pkcs7-signature'
	MIME.AS        = 'application/x-applixspread'
	MIME.AW        = 'application/x-applixword'
	MIME.A         = 'application/x-archive'
	MIME.ARJ       = 'application/x-arj'
	MIME.BZ        = 'application/x-bzip'
	MIME.BZ2       = 'application/x-bzip2'
	MIME.Z         = 'application/x-compress'
	MIME.CPIO      = 'application/x-cpio'
	MIME.UI        = 'application/x-designer'
	MIME.DESKTOP   = 'application/x-desktop'
	MIME.KDELNK    = 'application/x-desktop'
	MIME.DVI       = 'application/x-dvi'
	MIME.EXE       = 'application/x-executable'
	MIME.PFA       = 'application/x-font'
	MIME.PFB       = 'application/x-font'
	MIME.GSF       = 'application/x-font'
	MIME.PCF.Z     = 'application/x-font'
	MIME.GNUMERIC  = 'application/x-gnumeric'
	MIME.GRA       = 'application/x-graphite'
	MIME.GZ        = 'application/x-gzip'
	MIME.TGZ       = 'application/x-gzip'
	MIME.JAR       = 'application/x-jar'
	MIME.CLASS     = 'application/x-java'
	MIME.KARBON    = 'application/x-karbon'
	MIME.CHRT      = 'application/x-kchart'
	MIME.KFO       = 'application/x-kformula'
	MIME.FLW       = 'application/x-kivio'
	MIME.KON       = 'application/x-kontour'
	MIME.KPR       = 'application/x-kpresenter'
	MIME.KPT       = 'application/x-kpresenter'
	MIME.KRA       = 'application/x-krita'
	MIME.KSP       = 'application/x-kspread'
	MIME.KUD       = 'application/x-kugar'
	MIME.KWD       = 'application/x-kword'
	MIME.KWT       = 'application/x-kword'
	MIME.LHA       = 'application/x-lha'
	MIME.LZH       = 'application/x-lha'
	MIME.TS        = 'application/x-linguist'
	MIME.LZO       = 'application/x-lzop'
	MIME.WRI       = 'application/x-mswrite'
	MIME.O         = 'application/x-object'
	MIME.OGG       = 'application/x-ogg'
	MIME.PL        = 'application/x-perl'
	MIME.PERL      = 'application/x-perl'
	MIME.P12       = 'application/x-pkcs12'
	MIME.PFX       = 'application/x-pkcs12'
	MIME.PYC       = 'application/x-python-bytecode'
	MIME.PY        = 'application/x-python'
	MIME.WB1       = 'application/x-quattropro'
	MIME.WB2       = 'application/x-quattropro'
	MIME.WB3       = 'application/x-quattropro'
	MIME.RAR       = 'application/x-rar'
	MIME.RPM       = 'application/x-rpm'
	MIME.SH        = 'application/x-shellscript'
	MIME.CSH       = 'application/x-shellscript'
	MIME.SWF       = 'application/x-shockwave-flash'
	MIME.TAR       = 'application/x-tar'
	MIME.TAR.Z     = 'application/x-tarz'
	MIME.TAR.BZ    = 'application/x-tbz'
	MIME.TAR.BZ2   = 'application/x-tbz'
	MIME.TAR.GZ    = 'application/x-tgz'
	MIME.BAK       = 'application/x-trash'
	MIME.OLD       = 'application/x-trash'
	MIME.SIK       = 'application/x-trash'
	MIME.MAN       = 'application/x-troff-man'
	MIME.TR        = 'application/x-troff'
	MIME.ROFF      = 'application/x-troff'
	MIME.TTF       = 'application/x-truetype-font'
	MIME.TAR.LZO   = 'application/x-tzo'
	MIME.TZO       = 'application/x-tzo'
	MIME.DER       = 'application/x-x509-ca-cert'
	MIME.CER       = 'application/x-x509-ca-cert'
	MIME.CRT       = 'application/x-x509-ca-cert'
	MIME.CERT      = 'application/x-x509-ca-cert'
	MIME.PEM       = 'application/x-x509-ca-cert'
	MIME.ZIP       = 'application/x-zip'
	MIME.ZOO       = 'application/x-zoo'
	MIME.DEB       = 'application/x-debian-package'
	MIME.KSYSV     = 'application/x-ksysv-package'
	MIME.SHELL     = 'application/x-konsole'
	MIME.SGRD      = 'application/x-ksysguard'
	MIME.KTHEME    = 'application/x-ktheme'
	MIME.WAR       = 'application/x-webarchive'
	MIME.ARTS      = 'application/x-artsbuilder'
	MIME.PO        = 'application/x-gettext'
	MIME.POT       = 'application/x-gettext'
	MIME.KDEVPRJ   = 'application/x-kdevelop-project'
	MIME.AU        = 'audio/basic'
	MIME.SND       = 'audio/basic'
	MIME.M3U       = 'audio/x-mpegurl'
	MIME.AIFF      = 'audio/x-aiff'
	MIME.MID       = 'audio/x-midi'
	MIME.MOD       = 'audio/x-mod'
	MIME.S3M       = 'audio/x-mod'
	MIME.STM       = 'audio/x-mod'
	MIME.ULT       = 'audio/x-mod'
	MIME.UNI       = 'audio/x-mod'
	MIME.XM        = 'audio/x-mod'
	MIME.M15       = 'audio/x-mod'
	MIME.MTM       = 'audio/x-mod'
	MIME.669       = 'audio/x-mod'
	MIME.IT        = 'audio/x-mod'
	MIME.MP3       = 'audio/x-mp3'
	MIME.RA        = 'audio/x-pn-realaudio'
	MIME.RAM       = 'audio/x-pn-realaudio'
	MIME.RM        = 'audio/x-pn-realaudio'
	MIME.PLS       = 'audio/x-scpls'
	MIME.WAV       = 'audio/x-wav'
	MIME.KAR       = 'audio/x-karaoke'
	MIME.G3        = 'image/fax-g3'
	MIME.CGM       = 'image/cgm'
	MIME.JPEG      = 'image/jpeg'
	MIME.JPG       = 'image/jpeg'
	MIME.GIF       = 'image/gif'
	MIME.SVG       = 'image/svg+xml'
	MIME.PNG       = 'image/png'
	MIME.PCD       = 'image/x-photo-cd'
	MIME.TIF       = 'image/tiff'
	MIME.TIFF      = 'image/tiff'
	MIME.BMP       = 'image/x-bmp'
	MIME.EPS       = 'image/x-eps'
	MIME.EPSI      = 'image/x-eps'
	MIME.EPSF      = 'image/x-eps'
	MIME.ICO       = 'image/x-ico'
	MIME.JNG       = 'image/x-jng'
	MIME.MSOD      = 'image/x-msod'
	MIME.PBM       = 'image/x-portable-bitmap'
	MIME.PGM       = 'image/x-portable-bitmap'
	MIME.PPM       = 'image/x-portable-bitmap'
	MIME.WMF       = 'image/x-wmf'
	MIME.XBM       = 'image/x-xbm'
	MIME.XCF       = 'image/x-xcf-gimp'
	MIME.FIG       = 'image/x-xfig'
	MIME.XPM       = 'image/x-xpm'
	MIME.ICS       = 'text/calendar'
	MIME.CSS       = 'text/css'
	MIME.CSSL      = 'text/css'
	MIME.HTML      = 'text/html'
	MIME.HTM       = 'text/html'
	MIME.TXT       = 'text/plain'
	MIME.RDF       = 'text/rdf'
	MIME.RSS       = 'text/rss'
	MIME.RTF       = 'text/rtf'
	MIME.SGML      = 'text/sgml'
	MIME.ADB       = 'text/x-adasrc'
	MIME.ADS       = 'text/x-adasrc'
	MIME.BIB       = 'text/x-bibtex'
	MIME.HH        = 'text/x-c++hdr'
	MIME.CPP       = 'text/x-c++src'
	MIME.CXX       = 'text/x-c++src'
	MIME.CC        = 'text/x-c++src'
	MIME.C         = 'text/x-c++src'
	MIME.H         = 'text/x-chdr'
	MIME.CSV       = 'text/x-csv'
	MIME.DIFF      = 'text/x-diff'
	MIME.PATCH     = 'text/x-diff'
	MIME.JAVA      = 'text/x-java'
	MIME.LATEX     = 'text/x-latex'
	MIME.TEX       = 'text/x-latex'
	MIME.LOG       = 'text/x-log'
	MIME.LYX       = 'text/x-lyx'
	MIME.MOC       = 'text/x-moc'
	MIME.P         = 'text/x-pascal'
	MIME.PAS       = 'text/x-pascal'
	MIME.PM        = 'text/x-perl'
	MIME.TCL       = 'text/x-tcl'
	MIME.TK        = 'text/x-tcl'
	MIME.LTX       = 'text/x-tex'
	MIME.STY       = 'text/x-tex'
	MIME.CLS       = 'text/x-tex'
	MIME.VCS       = 'text/x-vcalendar'
	MIME.VCT       = 'text/x-vcard'
	MIME.FO        = 'text/x-xslfo'
	MIME.XSLFO     = 'text/x-xslfo'
	MIME.XSLT      = 'text/x-xslt'
	MIME.XML       = 'text/xml'
	MIME.KSYSV_LOG = 'text/x-ksysv-log'
	MIME.QT        = 'video/quicktime'
	MIME.MOV       = 'video/quicktime'
	MIME.MOOV      = 'video/quicktime'
	MIME.QTVR      = 'video/quicktime'
	MIME.MPEG      = 'video/mpeg'
	MIME.MPG       = 'video/mpeg'
	MIME.AVI       = 'video/x-msvideo'
	MIME.FLI       = 'video/x-flic'
	MIME.FLC       = 'video/x-flic'
	MIME.MNG       = 'video/x-mng'
return

/* --- CreateMime ----
CreateMime:
	dir = "/usr/share/mimelnk/"
	"find" dir "-name '*.desktop' (stack"
	mime. = ""
	do queued()
		parse pull file
		parse value "grep"("^MimeType",file) with . "=" mimetype
		if mimetype="" then iterate
		parse value "grep"("^Patterns",file) with . "=" patterns
		if patterns = "" then iterate
		upper patterns
		do while patterns <> ""
			parse var patterns e ";" patterns
			parse var e "*." e
			if mime.e = "" then do
				mime.e = mimetype
				say "	MIME."e "= '"mimetype"'"
			end
		end
	end
return
*/
