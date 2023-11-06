#!/bin/sh
g++ color.cpp -c -o color.o -iquote "../../include/" -std=c++20
g++ name.cpp -c -o name.o -iquote "../../include/" -std=c++20
g++ main.cpp -c -o main.o -iquote "../../include/" -std=c++20
g++ color.o name.o main.o -o format.out
./format.out

