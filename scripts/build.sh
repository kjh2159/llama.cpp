#!/bin/bash
mkdir ../build
cd ../build || exit

cmake -S .. -DLLAMA_CURL=ON
cmake --build . --config Release -j