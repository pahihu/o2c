CC=o2gcc
CFLAGS="-O2 -DDISABLE_RTC -iquote obj -iquote system"
# LDFLAGS = -lm -Wl,-s
LDFLAGS=-lm

rm -f obj/*.o make.log
make -f makefile_o2c CFLAGS="$CFLAGS" LDFLAGS="$LDFLAGS" build 2>&1 | tee make.log
