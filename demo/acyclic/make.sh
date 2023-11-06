#!/bin/sh
g++ aspect.cpp -c -o aspect.o -iquote "../../include/" -std=c++20
g++ awaited.cpp -c -o awaited.o -iquote "../../include/" -std=c++20
g++ awaiter.cpp -c -o awaiter.o -iquote "../../include/" -std=c++20
g++ branch.cpp -c -o branch.o -iquote "../../include/" -std=c++20
g++ coroutine.cpp -c -o coroutine.o -iquote "../../include/" -std=c++20
g++ demo.cpp -c -o demo.o -iquote "../../include/" -std=c++20
g++ exception.cpp -c -o exception.o -iquote "../../include/" -std=c++20
g++ nested.cpp -c -o nested.o -iquote "../../include/" -std=c++20
g++ sequence.cpp -c -o sequence.o -iquote "../../include/" -std=c++20
g++ simple.cpp -c -o simple.o -iquote "../../include/" -std=c++20
g++ aspect.o awaited.o awaiter.o branch.o coroutine.o demo.o exception.o nested.o sequence.o simple.o -o demo.out -pthread
./demo.out

