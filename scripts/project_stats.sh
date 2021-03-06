#!/bin/bash

# Prints source code line counts

CUR_DIR="${0%/*}"

cd $CUR_DIR
cd ../

find . -type f -iname "*.[ch]" \
    -o -type f -iname "*.asm" \
    -o -type f -iname "*.sh" \
    -o -type f -iname Makefile \
    | xargs -I{} wc -l {}

find . -type f -iname "*.[ch]" \
    -o -type f -iname "*.asm" \
    -o -type f -iname "*.sh" \
    -o -type f -iname Makefile \
    | xargs -I{} wc -l {} | cut -c 1-8 | awk '{sum += $1} END {print sum, "total"}'
