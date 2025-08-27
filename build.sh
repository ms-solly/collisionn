#!/bin/bash
set -e

# Set project directories
# Compile all .cpp files in the current directory
echo "Compiling source files..."
gcc -Wall -O2 -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 main.c -o collision

echo "Build complete. Executable is at ./collision"

./collision