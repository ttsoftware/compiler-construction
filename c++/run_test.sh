#!/usr/bin/env bash
FILES="resources/c/*.c"
for f in $FILES
do
  echo
  echo
  echo $f
  echo

  clang -w -Xclang -load -Xclang build/lib/libCustomPass.so $f
done