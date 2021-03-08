#!/bin/bash

# chao.exe 

param="$1"
CC="i686-w64-mingw32-g++"
INCLUDE_PATH="-I SDL/include/SDL2"
LIB_PATH="-L SDL/lib"
COMPILER_FLAGS="-w -Wl,-subsystem,windows"
LINKER_FLAGS="-lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -static-libgcc -static-libstdc++"
SOURCES="src/main.cpp src/duktape.c"
# HEADERS="src/duktape.h src/chao.h"
OUTPUT_PATH="bin/chao.exe"


if [ "$param" == "clean" ]
then
	rm -rf bin
fi

mkdir -p bin

$CC $SOURCES $INCLUDE_PATH $LIB_PATH $COMPILER_FLAGS $LINKER_FLAGS -o $OUTPUT_PATH

cp -r assets bin/
cp SDL/bin/zlib1.dll bin/
cp SDL/bin/SDL2.dll bin/
cp SDL/bin/SDL2_image.dll bin/
cp SDL/bin/SDL2_mixer.dll bin/
cp SDL/bin/libpng16-16.dll bin/