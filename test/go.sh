#!/bin/sh

#gcc -fPIC -c libprog2.c
#gcc -shared -o libprog2.so libprog2.o -lelf -lbsd
#gcc -fPIC -c libtst.c
#gcc -shared -o libtst.so libtst.o -lelf -lbsd
mips-linux-gnu-gcc -fPIC -c hello.c
mips-linux-gnu-gcc -shared -o hello.so hello.o -lelf -lbsd

mips-linux-gnu-objdump -hd hello.so > hello.so.obj
(cd ..; luajit loadelf.lua test/hello.so > test/hello.so.scn)

#luajit test.lua
