#!/usr/bin/env bash
clang -Xclang -load -Xclang build/lib/libCustomPass.so "resources/$1"