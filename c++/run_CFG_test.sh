#!/usr/bin/env bash
FILES="resources/c/CFGTests/*.c"
for f in $FILES
do
  echo
  echo
  echo $f
  echo

  clang-3.9 -w -Xclang -load -Xclang build/lib/libCFGPass.so $f
done