# product name
# DEV="$(getprop ro.product.product.model)"
# DEV="$(printf '%s' "$DEV" | tr -d '[:space:]')"
DEV="S25"
echo "Device: $DEV"

# turn-off screen
if [ "$DEV" = "Pixel9" ]; then
  # Pixel9
  su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
elif [ "$DEV" = "S24" ] || [ "$DEV" = "S25" ]; then
  # S24, S25
  su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"
else
  # Default
  su -c "echo 0 > /sys/class/backlight/panel/brightness"
  DEV="S25"
fi


# CPU Governor: performance
su -c "echo performance > /sys/devices/system/cpu/cpufreq/policy0/scaling_governor"
su -c "echo performance > /sys/devices/system/cpu/cpufreq/policy6/scaling_governor"
echo "CPU Governor (policy0): $(cat /sys/devices/system/cpu/cpufreq/policy0/scaling_governor)"
echo "CPU Governor (policy6): $(cat /sys/devices/system/cpu/cpufreq/policy6/scaling_governor)"
sleep 3

# silver core control (Except S25)
if [ "$DEV" != "S25" ]; then
  su -c "echo 0 > /sys/devices/system/cpu/cpu1/online"
  su -c "echo 0 > /sys/devices/system/cpu/cpu2/online"
  su -c "echo 0 > /sys/devices/system/cpu/cpu3/online"
fi

  # -m: model path
  # -v: vocabulary path
  # -e: merges path
  # -f: model family
  # -b: model size
  # -t: num of threads
  # -l: max KV cache size
  # -i: print inference interface
  # -s: starting num of queries
  # -L: num of queries
  # -I: input dataset path of csv
  # -O: output directory path
  # -S: save query-answer pairs with json
  # -D: device name
  # --strict: apply tokwn limits to only output tokens
  # --cpu-p: specify CPU frequency for CPU DVFS
  # --ram-p: specify RAM frequency for RAM DVFS
  # --cpu-d: specify CPU frequency for CPU DVFS
  # --ram-d: specify RAM frequency for RAM DVFS
  # --phase-pause: specify a pause time between phases (ms)
  # --token-pause: specify a pause time between generation tokens (ms)
  # --layer-pause: specify a pause time between self-attention layers during prefill (ms)
  # --query-interval: specify an interval time between queries (s)

./bin-arm/stream_qwen \
  -m models/qwen-1-5-0.5b-q4_k.mllm \
  -v vocab/qwen_vocab.mllm \
  -e vocab/qwen_merges.txt \
  -f Qwen1.5 \
  -b 0.5B \
  -t 4 \
  -l 1024 \
  -i 1 \
  -s 1 \
  -L 30 \
  -I dataset/hotpot_qa.csv \
  -O output/ \
  -S 0 \
  -D "$DEV" \
  --strict 0 \
  --cpu-p $1 \
  --ram-p $2 \
  --cpu-d $3 \
  --ram-d $4 \
  --phase-pause 0 \
  --token-pause 0 \
  --layer-pause 0 \
  --query-interval 0

# [pause-unit] = ms
# [interval-unit] = s

# silver core reset (except S25)
if [ "$DEV" != "S25" ]; then
  su -c "echo 1 > /sys/devices/system/cpu/cpu1/online"
  su -c "echo 1 > /sys/devices/system/cpu/cpu2/online"
  su -c "echo 1 > /sys/devices/system/cpu/cpu3/online"
fi

# CPU Governor reset: walt
su -c "echo walt > /sys/devices/system/cpu/cpufreq/policy0/scaling_governor"
su -c "echo walt > /sys/devices/system/cpu/cpufreq/policy6/scaling_governor"
echo "CPU Governor reset (policy0): $(cat /sys/devices/system/cpu/cpufreq/policy0/scaling_governor)"
echo "CPU Governor reset (policy6): $(cat /sys/devices/system/cpu/cpufreq/policy6/scaling_governor)"

# turn-on screen
if [ "$DEV" = "S25" ]; then
  # S25
  su -c "echo 1023 > /sys/class/backlight/panel0-backlight/brightness"
else
  # S24
  su -c "echo 1023 > /sys/class/backlight/panel/brightness"
fi
