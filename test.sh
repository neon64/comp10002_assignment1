#!/bin/sh

gcc -Wall -ansi -o ass1 ass1.c
 
if [ "$2" == "debug" ]; then
    ./ass1 < test${1}.txt 2>&1 >/dev/null | less
else
    ./ass1 < test${1}.txt > ass1-test${1}.txt
    nvim -d ass1-test${1}.txt test${1}-out-mac.txt
fi
