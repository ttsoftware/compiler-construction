#!/usr/bin/env bash
clang -w -Xclang -load -Xclang build/lib/libCustomPass.so "resources/$1"