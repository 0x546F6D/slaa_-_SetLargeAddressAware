#!bin/sh
clear
gcc -o slaa slaa.c -s -O3 -Wl,--gc-sections -fno-asynchronous-unwind-tables -nostartfiles --entry=slaa
test -f slaa.exe || exit
wc -c slaa.exe
strip --strip-all slaa.exe
wc -c slaa.exe
