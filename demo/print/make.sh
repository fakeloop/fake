#!/bin/sh
g++ demo.cpp -o print.out -iquote "../../include/" -std=c++20
./print.out

