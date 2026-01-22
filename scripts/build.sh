#!/bin/bash
mkdir ../build
cd ../build || exit

cmake -S .. \
      -DLLAMA_CURL=ON \
      -DLLAMA_BUILD_IGNITE=ON \
      -DLLAMA_IGNITE_INSTALL=ON \
      -DIGNITE_USE_SYSTEM_DVFS=OFF
cmake --build . --config Debug -j
