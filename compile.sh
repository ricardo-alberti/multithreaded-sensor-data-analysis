#!/bin/bash

# optmization flags
#FLAGS="-O3 -Wall -Werror -Wextra"

# compilation for debugging
FLAGS="-g -Wall -Werror -Wextra"
OUT_DIR="build"

mkdir -p $OUT_DIR

# check if yyjson.o exists
if [ ! -f $OUT_DIR/yyjson.o ]; then
    echo "yyjson.o not found. Compiling library..."
    gcc -O3 -c src/yyjson.c -o $OUT_DIR/yyjson.o
else
    echo "yyjson.o exists. Skipping library compilation."
fi

echo "Compiling main.c"
gcc $FLAGS -c src/main.c -o $OUT_DIR/main.o
echo "Linking object files"
gcc $FLAGS $OUT_DIR/main.o $OUT_DIR/yyjson.o -o sensor-data-analysis

echo "Compilation completed. Run with: ./sensor-data-analysis"
