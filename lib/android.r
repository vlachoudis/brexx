/* Android send SL4A command */
Android: procedure
	/* prepare args */
	args = ""
	do i=2 to arg()
		a = arg(i)
		if ^datatype(a,"N") then a = '"'jsonesc(a)'"'
		if args^=="" then
			args = args","a
		else
			args = a
	end

	sock = value('@ANDROIDSOCKET',,0)
	id   = value('@ANDROIDID',,0)
	json = '{"method":"'arg(1)'", "id":'id', "params": ['args']}'||"0a"x
	/*say "<<<" json*/
	call Send sock, json

	str = Recv(sock)
	/*say ">>>" str*/
	call Value '@ANDROIDID', id+1, 0

	error = json(str, "error")
	return json(str, "result")

/* --- Initialize Android Communications --- */
AndroidInit: procedure
	if import("librxunix.so")>0 then return -1
	if import("librxjson.so")>0 then return -1
	if import("androidapi.r") > 0 then return -1
	sock = Socket(getenv("AP_HOST"), getenv("AP_PORT"))
	call value "@ANDROIDSOCKET", sock, 0
	call value "@ANDROIDID", 0, 0
	if getenv("AP_HANDSHAKE") <> "" then
		call Android "_authenticate", getenv("AP_HANDSHAKE")
	return sock

/* --- Close the communication --- */
AndroidEnd:
	return SocketClose(value("@ANDROIDSOCKET",,0))
