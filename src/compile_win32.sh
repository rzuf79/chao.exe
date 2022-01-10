#!/bin/bash
SOURCES="main.cpp upng.c"
i686-w64-mingw32-g++ $SOURCES -o rogue -s -static -mwindows -lgdi32 -lopengl32 -lglu32 -lwinmm

