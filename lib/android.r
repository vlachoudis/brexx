/* Android send SL4A command */
Android: procedure
	/* prepare args */
	args = ""
	do i=2 to arg()
		a = arg(i)
		if ^datatype(a,"N") then do
			l = left(a,1)
			r = right(a,1)
			select	/* ignore arguments starting/ending with [], {} */
				/* treat them as json objects */
				when l=='[' & r==']' then nop
				when l=='{' & r=='}' then nop
				otherwise a = '"'jsonesc(a)'"'
			end
		end
		if args^=="" then
			args = args","a
		else
			args = a
	end

	sock = value('@ANDROIDSOCKET',,0)
	id   = value('@ANDROIDID',,0)
	json = '{"method":"'arg(1)'", "id":'id', "params": ['args']}'||"0a"x
	call Send sock, json

	str = Recv(sock)
	call Value '@ANDROIDID', id+1, 0
	call Value '@ANDROIDERROR', json(str, "error"), 0
	return json(str, "result")

/* --- Return last error --- */
AndroidError: return Value('@ANDROIDERROR',,0)

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
