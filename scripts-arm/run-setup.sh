#!/bin/bash
# RUN
# mllm/~$ sh scripts-arm/run-setup.sh
#

# configurations
TARGET_PATH="/data/local/tmp/llama.cpp/"
MODEL="models/Qwen1.5-0.5B-Q4_K_M.gguf"
DATASET="dataset/hotpot_qa_20.json"

# check directory (essential)
adb shell "su -c 'chmod -R 777 /data/local/tmp/llama.cpp'"
adb shell '[ -d /data/local/tmp/llama.cpp/bin ] || mkdir -p /data/local/tmp/llama.cpp/bin'
adb shell '[ -d /data/local/tmp/llama.cpp/models ] || mkdir -p /data/local/tmp/llama.cpp/models'
adb shell '[ -d /data/local/tmp/llama.cpp/dataset ] || mkdir -p /data/local/tmp/llama.cpp/dataset'
adb shell '[ -d /data/local/tmp/llama.cpp/output ] || mkdir -p /data/local/tmp/llama.cpp/output'

# check executable directory
adb shell '[ -x /data/local/tmp/llama.cpp ] || chmod -R 777 /data/local/tmp/llama.cpp'
if [ $? -ne 0 ]; then
    echo "Operation Denied"
fi

adb shell "su -c '[ -x /data/local/tmp/llama.cpp ] || chmod -R 777 /data/local/llama.cpp/mllm'"
if [ $? -ne 0 ]; then
    echo "Root Authority Denied"
    exit 1
fi
echo directory is executable

# push files
# first, please place the model files in the direcotry mllm/models/
# model
adb shell "[ -f $TARGET_PATH$MODEL ]" || adb push "$MODEL" "$TARGET_PATH/models"
if [ $? -eq 0 ]; then
    echo "$MODEL is pushed"
else
    echo "[ERROR] $MODEL is not pushed"
fi
# dataset
adb shell "[ -f $TARGET_PATH$DATASET ]" || adb push "$DATASET" "$TARGET_PATH/dataset"
if [ $? -eq 0 ]; then
    echo "$DATASET is pushed"
else
    echo "[ERROR] $DATASET is not pushed"
fi
# files
adb push build-android/install /data/local/tmp/llama.cpp

# last executable
adb shell "su -c '[ -x /data/local/tmp/mllm ] || chmod -R 777 /data/local/tmp/llama.cpp'"
if [ $? -ne 0 ]; then
    echo "Root Authority Denied"
    exit 1
fi
echo directory is executable