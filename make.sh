#!/bin/sh

gcc -g -O0 -o ixhash ixhash_ascii.c ixhash.c main.c -lcrypto
