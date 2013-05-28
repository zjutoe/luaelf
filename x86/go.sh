#!/bin/sh

#gcc -fPIC -c libprog2.c
#gcc -shared -o libprog2.so libprog2.o -lelf -lbsd
#gcc -fPIC -c libtst.c
#gcc -shared -o libtst.so libtst.o -lelf -lbsd
gcc -fPIC -c hello.c
gcc -shared -o hello.so hello.o -lelf -lbsd

objdump -hd hello.so > hello.so.obj
(cd ..; luajit loadelf.lua x86/hello.so > x86/hello.so.scn)

#luajit test.lua
