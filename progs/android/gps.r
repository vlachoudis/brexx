call import "android.r"
call AndroidInit

say "Finding ZIP code."
location = getLastKnownLocation()
say location
loc = json(location,"gps")
say "GPS=" loc
if loc="null" then do
	loc = json(location,"network")
	say "Network=" loc
end
if loc="null" then do
	say "Unable to find location"
	exit
end
lon = 
Lon = json(loc,"longitude")
lat = json(loc,"latitude")
say "Lon=" lon
say "Lon=" lat
addr = geocode(lat, lon)
say "Addr=" addr
zip = json(addr,"postal_code")
say "Zip=" zip
