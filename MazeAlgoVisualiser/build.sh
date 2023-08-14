#!/bin/bash

mkdir -p bin
cc ./main.c ./algo.c ./queue.c $(pkg-config --libs --cflags raylib) -o ./bin/visualise -ggdb
