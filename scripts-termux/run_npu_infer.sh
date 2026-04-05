#!/bin/sh
# run_npu_infer.sh - NPU inference only
# Run from: /data/local/tmp/llama.cpp

echo "[inference] starting..."

su -p -c "setenforce 0 && \
    export LD_LIBRARY_PATH=/data/local/tmp/llama.cpp/lib && \
    export ADSP_LIBRARY_PATH=/data/local/tmp/llama.cpp/lib && \
    export GGML_HEXAGON_HOSTBUF=1 && \
    cd /data/local/tmp/llama.cpp && \
    taskset f0 ./bin/llama-ignite-npu \
        -m /data/local/tmp/gguf/qwen1_5-0_5b-chat-q4_k_m.gguf \
        -t 1 -tb 4 -i -cnv -ub 512 -b 512 -fa off \
        --json-path data/hotpot_qa_30.json \
        --output-dir output \
        --temp 0 \
        --top-k 1 \
        --top-p 0 \
        -c 1024 \
        --device HTP0"

echo "[inference] done."
