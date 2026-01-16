O2CARCH=64
CC="o2gcc -g"
CFLAGS="-g -O0 -DDISABLE_RTC -iquote obj -iquote system"
# CFLAGS="-O2 -DDISABLE_RTC -iquote obj -iquote system"
# LDFLAGS = -lm -Wl,-s
LDFLAGS=-lm
ARCHFILES="lib/CType lib/IntStr compiler/OGenGCC compiler/OMachine"

if [ ! -d obj.sav ];
then
    echo "obj.sav not found"
    exit 1
fi

for f in $ARCHFILES
do
    echo "$f$O2CARCH.Mod => $f.Mod"
    cp $f$O2CARCH.Mod $f.Mod
done

rm -rf obj obj.distrib
cp -rp obj.sav obj

rm -f obj/*.o make.log
make -f makefile_o2c CC="$CC" CFLAGS="$CFLAGS" LDFLAGS="$LDFLAGS" O2CARCH="$O2CARCH" build 2>&1 | tee make.log

mv obj obj.distrib
mkdir obj

./o2c -vMOR --redir system/o2c.red.template o2c X
cp obj.distrib/* obj
touch obj/*.o

./o2c -MORsv --redir system/o2c.red.template o2b
./o2c -MORsv --redir system/o2c.red.template o2ef
./o2c -MORsv --redir system/o2c.red.template o2whereis
mv o2c o2c_stage0

for f in $ARCHFILES
do
    rm $f.Mod
done

rm -rf obj obj.distrib
rm -f sym/*
