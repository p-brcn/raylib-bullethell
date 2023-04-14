#!/bin/sh

set -xe

#dotnet run Program.cs

clang -o main main.c enemy.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -Ofast
