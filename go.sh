#!/bin/sh

gcc -fPIC -c libprog2.c
gcc -shared -o libprog2.so libprog2.o -lelf -lbsd
luajit test.lua
