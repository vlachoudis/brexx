make clean
make android
mkdir -p brexx/bin
mkdir -p brexx/lib
cp src/rexx brexx/bin
cp lstring/liblstring.so brexx/lib
cp modules/*.so brexx/lib

rm brexx.zip
zip -r brexx.zip brexx/
rm -Rf brexx/*

mkdir brexx
cp AUTHORS COPYING INSTALL README NEWS ChangeLog BUGS brexx/
mkdir -p brexx/doc
mkdir -p brexx/lib
cp -Rdpvu doc/*.html brexx/doc
cp -Rdpvu lib/*.r brexx/lib

rm brexx_extras.zip
zip -r brexx_extras.zip brexx
rm -Rf brexx/*

mkdir -p brexx/ce
mkdir -p brexx/android
cp progs/*.r brexx/
cp progs/ce/*.r brexx/ce
cp progs/android/*.r brexx/android
rm brexx_scripts.zip
zip -r brexx_scripts.zip brexx
rm -Rf brexx
