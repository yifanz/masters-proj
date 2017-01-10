#!/bin/bash

CUR_DIR="${0%/*}"

cd $CUR_DIR
cd ../

if [ "$1" == "DEBUG" ]; then
    out/hvexec -i -l DEBUG -o out/log -k out/boot "${@:2}"
else
    out/hvexec -l INFO -o out/log -k out/boot "$@"
fi
