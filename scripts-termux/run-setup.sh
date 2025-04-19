#!bin/bash

# directory check
if [ -d "~/.cache/llama.cpp" ]
then
    echo "model directory is ready"
else
    echo "model directory does not exist"
    mkdir -p ~/.cache/llama.cpp/
    echo "model directory is ready now"
fi


# model check
if [ -e "~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf" ]
then
    curl -L https://huggingface.co/tensorblock/Qwen1.5-0.5B-GGUF/resolve/main/Qwen1.5-0.5B-Q5_K_M.gguf --output ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf
fi


# directory check
if [ -d "outputs" ]
then
    echo "ouputs directory is ready"
else
    echo "outputs directory does not exist"
    mkdir outputs
    echo "outputs directory is ready now"
fi