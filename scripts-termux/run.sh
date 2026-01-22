#! bin/bash
# this script should be run on llama.cpp/ dir.

# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"

# silver core control
su -c "echo 1 > /sys/devices/system/cpu/cpu1/online"
su -c "echo 1 > /sys/devices/system/cpu/cpu2/online"
su -c "echo 1 > /sys/devices/system/cpu/cpu3/online"


./build/bin/ignite \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --ignite-verbose off \
    --strict on \
    --strict-length 64 \
    --output-dir outputs/ \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-p 12 \
    --ram-d 11 \
    --cpu-p 12 \
    --ram-d 11 \
    -ctx 1024


su -c "echo 1 > /sys/devices/system/cpu/cpu1/online"
su -c "echo 1 > /sys/devices/system/cpu/cpu2/online"
su -c "echo 1 > /sys/devices/system/cpu/cpu3/online"

# experiment done -> let screen brightness bright again
su -c "echo 1023 > /sys/class/backlight/panel0-backlight/brightness"

