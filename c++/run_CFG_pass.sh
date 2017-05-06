#!/usr/bin/env bash
clang-3.9 -w -Xclang -load -Xclang build/lib/libCFGPass.so "resources/$1"