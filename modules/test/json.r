call import "librxjson.so"
str = '  {   "a" :    null, "bb" : -123e-5, "ccc":{"a":1,"b":true} , "d":[1,null,"three" ,4, {"aa":true},6  ]}'
array = json(str,"d")
len = jsonarray(array)
do i=1 to len
	say "array."i"=" jsonarray(array,i)
end
start = jsonfind(str,"d")
do i=1 to len
	say "array."i"=" jsonarray(str,i,start)
end

say json(str,"d")
say json(str,"ccc")
say json(str,"bb")
say json(str,"a")
location = '{"passive":null,"gps":null,"network":null}'
say json(location,"gps")
say json(location,"network")
