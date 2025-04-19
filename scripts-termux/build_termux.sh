#! bin/bash

cmake -S . \
	-DCMAKE_C_FLAGS="-march=armv8-a+dotprod+sve -pthread" \
	-DCMAKE_CXX_FLAGS="-march=armv8-a+dotprod+sve -pthread" \
	-B build-termux

cmake --build build-termux --config Release -j4
