#!/bin/bash

set -eu

mkdir -p build 

if [ $# -ne 0 ] && [ "$1" == "ejudge" ]; then
	cpp-merge -i include -o build/__merged__.cpp src/main.cpp
	exit 0
fi

cmake -S . \
			-B build \
			-G Ninja \
			-DCMAKE_CXX_COMPILER=afl-clang-lto++ \
			-DCMAKE_BUILD_TYPE=Debug \
			-DCMAKE_EXPORT_COMPILE_COMMANDS=1 
cmake --build build --parallel $(nproc)
cp -f build/compile_commands.json .

if [ $# -ne 0 ] && [ "$1" == "run" ]; then
	echo -e "\n"
	./build/pn_compiler -jobs=8
fi
