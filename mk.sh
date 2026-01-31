O2CARCH=Unix32
CC="o2gcc -g"
CFLAGS="-g -O0 -iquote obj -iquote system"
# CFLAGS="-O2 -DDISABLE_RTC -iquote obj -iquote system"
# LDFLAGS = -lm -Wl,-s
LDFLAGS=-lm

MORsv="-Mv"
MORv="-Mv"
MRv="-Mv"

if [ ! -d obj.sav ];
then
    echo "copy obj.distrib to obj.sav"
    exit 1
fi

cp lib/$O2CARCH/*.Mod lib/
cp compiler/$O2CARCH/*.Mod compiler/

rm -f o2b o2ef o2whereis o2c o2c_stage*
rm -rf obj obj.distrib UpdateLib
rm -f sym/*
cp -rp obj.sav obj

#
# STAGE0
#
echo "Building o2c_stage0..."
rm -f obj/*.o make.log
make -f makefile_o2c CC="$CC" CFLAGS="$CFLAGS" LDFLAGS="$LDFLAGS" O2CARCH="$O2CARCH" build 2>&1 | tee make.log

mv obj obj.distrib
mkdir obj

./o2c $MORv --redir system/o2c.red.template o2c X
cp obj.distrib/* obj
touch obj/*.o

./o2c $MORsv --redir system/o2c.red.template o2b
./o2c $MORsv --redir system/o2c.red.template o2ef
./o2c $MORsv --redir system/o2c.red.template o2whereis
mv o2c o2c_stage0

#
# STAGE1
#
echo "Building o2c_stage1..."
rm -f o2c_stage1 o2c_stage2 o2c all sym/* obj/*
./o2c_stage0 $MRv --redir system/o2c.red.template o2c
mv o2c o2c_stage1

#
# STAGE2
#
rm -f o2c_stage2 o2c all sym/* obj/*
./o2c_stage1 $MORsv --redir system/o2c.red.template o2c
./o2c $MORv --redir system/o2c.red.template UpdateLib X
touch obj/*.o
./o2c $MORv --redir system/o2c.red.template UpdateLib
mv o2c o2c_stage2


# cleanup
cp o2c_stage2 o2c
rm -f o2c_stage*
rm -rf obj.distrib UpdateLib
exit 0

# for f in $ARCHFILES
# do
#     rm $f.Mod
# done

