call import "android.r"
call AndroidInit
message = dialogGetInput("TTS","What would you like to say?")
say "Message=" message
call ttsSpeak message
