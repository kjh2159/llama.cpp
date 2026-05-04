# this script should be run on llama.cpp/ dir.

# screen brightness control
echo 0 > /sys/class/backlight/panel0-backlight/brightness

# silver core control
# su -c "echo 1 > /sys/devices/system/cpu/cpu1/online"
# su -c "echo 1 > /sys/devices/system/cpu/cpu2/online"
# su -c "echo 1 > /sys/devices/system/cpu/cpu3/online"

# CPU Governor: performance
echo performance > /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
echo performance > /sys/devices/system/cpu/cpufreq/policy6/scaling_governor
echo "CPU Governor (policy0): $(cat /sys/devices/system/cpu/cpufreq/policy0/scaling_governor)"
echo "CPU Governor (policy6): $(cat /sys/devices/system/cpu/cpufreq/policy6/scaling_governor)"
sleep 3

./build/bin/ignite \
    -m models/qwen-3-0.6b-q4_k_m.gguf \
    -i -cnv -tb 5 -t 5 -ub 512 -b 512 \
    -c 1024 \
    --temp 0 \
    --top-k 1 \
    --device-name S25 \
    --output-dir output/ \
    --input-path data/qwen3_prefill_64.json \
    -fa off \
    --strict on \
    --strict-limit 128 \
    --max-query-number 30 \
    --cpu-p 15 \
    --ram-p 9 \
    --cpu-d 15 \
    --ram-d 9

# --layer-pause LP[ms]

# su -c "echo 1 > /sys/devices/system/cpu/cpu1/online"
# su -c "echo 1 > /sys/devices/system/cpu/cpu2/online"
# su -c "echo 1 > /sys/devices/system/cpu/cpu3/online"

# CPU Governor reset: walt
echo walt > /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
echo walt > /sys/devices/system/cpu/cpufreq/policy6/scaling_governor
echo "CPU Governor reset (policy0): $(cat /sys/devices/system/cpu/cpufreq/policy0/scaling_governor)"
echo "CPU Governor reset (policy6): $(cat /sys/devices/system/cpu/cpufreq/policy6/scaling_governor)"

# experiment done -> let screen brightness bright again
echo 1023 > /sys/class/backlight/panel0-backlight/brightness
