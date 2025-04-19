#! bin/bash
# this script should be run on llama.cpp/ dir.

# silver core control
su -c "echo 0 > /sys/devices/system/cpu/cpu1/online"
su -c "echo 0 > /sys/devices/system/cpu/cpu2/online"
su -c "echo 0 > /sys/devices/system/cpu/cpu3/online"

# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 0 RAM 0
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_0_0.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 0 \
    --ram-freq 0

sleep 60



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 0 RAM 4
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_0_4.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 0 \
    --ram-freq 4
    
sleep 60



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 0 RAM 6
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_0_6.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 0 \
    --ram-freq 6
    
sleep 60



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 0 RAM 9
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_0_9.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 0 \
    --ram-freq 9
    
sleep 60



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 0 RAM 10
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_0_10.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 0 \
    --ram-freq 10
    
sleep 60



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 0 RAM 11
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_0_11.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 0 \
    --ram-freq 11
    
sleep 60



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 0 RAM 12
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_0_12.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 0 \
    --ram-freq 12
    
sleep 60



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 2 RAM 0
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_2_0.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 2 \
    --ram-freq 0

sleep 60



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 2 RAM 4
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_2_4.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 2 \
    --ram-freq 4
    
sleep 60



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 2 RAM 6
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_2_6.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 2 \
    --ram-freq 6
    
sleep 60



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 2 RAM 9
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_2_9.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 2 \
    --ram-freq 9
    
sleep 60



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 2 RAM 10
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_2_10.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 2 \
    --ram-freq 10
    
sleep 60



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 2 RAM 11
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_2_11.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 2 \
    --ram-freq 11
    
sleep 60



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 2 RAM 12
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_2_12.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 2 \
    --ram-freq 12
    
sleep 60


# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 6 RAM 0
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_6_0.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 6 \
    --ram-freq 0

sleep 180



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 6 RAM 4
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_6_4.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 6 \
    --ram-freq 4
    
sleep 180



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 6 RAM 6
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_6_6.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 6 \
    --ram-freq 6
    
sleep 180



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 6 RAM 9
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_6_9.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 6 \
    --ram-freq 9
    
sleep 180



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 6 RAM 10
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_6_10.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 6 \
    --ram-freq 10
    
sleep 180



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 6 RAM 11
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_6_11.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 6 \
    --ram-freq 11
    
sleep 180



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 6 RAM 12
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_6_12.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 6 \
    --ram-freq 12
    
sleep 180



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 8 RAM 0
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_8_0.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 8 \
    --ram-freq 0

sleep 300



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 8 RAM 4
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_8_4.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 8 \
    --ram-freq 4
    
sleep 300



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 8 RAM 6
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_8_6.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 8 \
    --ram-freq 6
    
sleep 300



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 8 RAM 9
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_8_9.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 8 \
    --ram-freq 9
    
sleep 300



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 8 RAM 10
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_8_10.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 8 \
    --ram-freq 10
    
sleep 300



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 8 RAM 11
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_8_11.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 8 \
    --ram-freq 11
    
sleep 300



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 8 RAM 12
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_8_12.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 8 \
    --ram-freq 12
    
sleep 300


# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 10 RAM 0
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_10_0.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 10 \
    --ram-freq 0

sleep 360



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 10 RAM 4
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_10_4.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 10 \
    --ram-freq 4
    
sleep 360



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 10 RAM 6
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_10_6.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 10 \
    --ram-freq 6
    
sleep 360



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 10 RAM 9
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_10_9.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 10 \
    --ram-freq 9
    
sleep 360



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 10 RAM 10
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_10_10.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 10 \
    --ram-freq 10
    
sleep 360



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 10 RAM 11
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_10_11.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 10 \
    --ram-freq 11
    
sleep 360



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 10 RAM 12
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_10_12.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 10 \
    --ram-freq 12
    
sleep 360



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 12 RAM 0
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_12_0.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 12 \
    --ram-freq 0

sleep 420



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 12 RAM 4
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_12_4.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 12 \
    --ram-freq 4
    
sleep 420



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 12 RAM 6
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_12_6.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 12 \
    --ram-freq 6
    
sleep 420



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 12 RAM 9
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_12_9.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 12 \
    --ram-freq 9
    
sleep 420



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 12 RAM 10
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_12_10.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 12 \
    --ram-freq 10
    
sleep 420



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 12 RAM 11
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_12_11.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 12 \
    --ram-freq 11
    
sleep 420



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 12 RAM 12
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_12_12.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 12 \
    --ram-freq 12
    
sleep 420



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 16 RAM 0
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_16_0.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 16 \
    --ram-freq 0

sleep 480



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 16 RAM 4
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_16_4.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 16 \
    --ram-freq 4
    
sleep 480



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 16 RAM 6
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_16_6.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 16 \
    --ram-freq 6
    
sleep 480



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 16 RAM 9
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_16_9.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 16 \
    --ram-freq 9
    
sleep 480



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 16 RAM 10
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_16_10.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 16 \
    --ram-freq 10
    
sleep 480



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 16 RAM 11
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_16_11.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 16 \
    --ram-freq 11
    
sleep 480



# screen brightness control
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
# CPU 16 RAM 12
./bin/llama-cli \
    -m ~/.cache/llama.cpp/tensorblock_Qwen1.5-0.5B-GGUF_Qwen1.5-0.5B-Q4_K.gguf \ 
    -cnv \
    --temp 0 \
    -p "You're a helpful assistant." \
    -i \
    --top-k 5 \
    --threads 1 \
    --device-name Pixel9 \
    --output-path outputs/hotpot_16_12.csv \
    --json-path dataset/hotpot_qa_30.json \
    --cpu-freq 16 \
    --ram-freq 12
    
sleep 480

# experiment done -> let screen brightness bright again
su -c "echo 512 > /sys/class/backlight/panel0-backlight/brightness"

