#! bin/bash
# this script should be run on llama.cpp/ dir.

# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"

# silver core control
# su -c "echo 1 > /sys/devices/system/cpu/cpu1/online"
# su -c "echo 1 > /sys/devices/system/cpu/cpu2/online"
# su -c "echo 1 > /sys/devices/system/cpu/cpu3/online"


./build/bin/ignite \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -c 1024 \
    --temp 0 \
    --top-k 1 \
    --device-name Pixel9 \
    --output-dir outputs/ \
    --json-path dataset/hotpot_qa_30.json \
    --strict on \
    --strict-length 64 \
    --max-query-number 20 \
    --cpu-p 12 \
    --ram-d 11 \
    --cpu-p 12 \
    --ram-d 11 \
    --ignite-verbose


# su -c "echo 1 > /sys/devices/system/cpu/cpu1/online"
# su -c "echo 1 > /sys/devices/system/cpu/cpu2/online"
# su -c "echo 1 > /sys/devices/system/cpu/cpu3/online"

# experiment done -> let screen brightness bright again
su -c "echo 1023 > /sys/class/backlight/panel0-backlight/brightness"

