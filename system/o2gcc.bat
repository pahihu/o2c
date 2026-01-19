@echo off
set OPTS=-fno-builtin -Wno-implicit-function-declaration -Wno-int-conversion -Wno-builtin-declaration-mismatch
set LDFLAGS=-L/opt/homebrew/lib
gcc -std=gnu89 %OPTS% %LDFLAGS% %*
