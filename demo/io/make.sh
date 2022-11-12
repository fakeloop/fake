#!/bin/sh
g++ io.cpp -o io.out -iquote "../../include/" -Wno-non-template-friend -std=c++20
./io.out

