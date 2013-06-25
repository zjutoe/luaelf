#!/bin/sh

gcc -fPIC -c libelfconn.c
gcc -shared -o libelfconn.so libelfconn.o -lelf -lbsd

