#!/bin/bash

set -xe
gcc -Wall -Wextra -std=c17 main.c pfs.c simlib.c -I./ -lm -lraylib -o main
./main
ffplay -fs videos/*

