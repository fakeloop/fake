#!/bin/sh
g++ demo.cpp -o print.out -iquote "../../include/" -Wno-non-template-friend -std=c++20
./print.out

