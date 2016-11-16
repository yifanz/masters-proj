#!/bin/bash

# Usage: ./print_dylibs <path to executable>
# Recurisvely prints all transitive dynamic libraries required by an executable.

deps=""

find_deps()
{
    bin=$1
    while read lib; do
        lib=${lib%%\(*\)}
        lib=${lib%%[[:space:]]}
        lib=${lib##[[:space:]]}
        if [[ ! "$deps" =~ "$lib" ]]; then
            deps+="$lib"$'\n'
            find_deps "$lib"
        fi
    done < <(otool -L "$bin" | tail -n +2)
}

find_deps "$1"
deps=${deps%%[[:space:]]}
echo "$deps"
