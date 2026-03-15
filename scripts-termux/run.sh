#! bin/bash
# this script should be run on llama.cpp/ dir.

# screen brightness control
echo 0 > /sys/class/backlight/panel0-backlight/brightness

# silver core control
# su -c "echo 1 > /sys/devices/system/cpu/cpu1/online"
# su -c "echo 1 > /sys/devices/system/cpu/cpu2/online"
# su -c "echo 1 > /sys/devices/system/cpu/cpu3/online"


./build/bin/ignite \
    -m models/qwen-3-0.6b-q4k.gguf \
    -i -cnv -tb 4 -t 1 -ub 512 -b 512 \
    -c 1024 \
    --temp 0 \
    --top-k 1 \
    --top-p 0 \
    --device-name Pixel9 \
    --output-dir outputs/ \
    --input-path dataset/hotpot_qa_30.json \
    --strict on \
    --strict-limit 64 \
    --max-query-number 20 \
    --cpu-p 12 \
    --ram-p 11 \
    --cpu-d 12 \
    --ram-d 11

# --layer-pause LP[ms]

# su -c "echo 1 > /sys/devices/system/cpu/cpu1/online"
# su -c "echo 1 > /sys/devices/system/cpu/cpu2/online"
# su -c "echo 1 > /sys/devices/system/cpu/cpu3/online"

# experiment done -> let screen brightness bright again
echo 1023 > /sys/class/backlight/panel0-backlight/brightness

