#!/usr/bin/env bash
clang -w -Xclang -load -Xclang build/lib/libCFGPass.so "resources/$1"