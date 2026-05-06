#!/bin/sh
# run.sh - NPU inference with runtime DVFS controlled by llama-ignite-npu
# Run from: /data/local/tmp/llama.cpp
#
# Optional positional args:
#   $1: prefill CPU DVFS index
#   $2: prefill RAM DVFS index
#   $3: decode CPU DVFS index
#   $4: decode RAM DVFS index
#   $5: phase pause in ms
#   $6: token pause in ms
#   $7: layer pause in ms
#   $8: ignite verbose [on|off]

DEV="${DEV:-S25}"
CPU_P="${1:-15}"
RAM_P="${2:-9}"
CPU_D="${3:-15}"
RAM_D="${4:-9}"
PHASE_PAUSE_MS="${5:-0}"
TOKEN_PAUSE_MS="${6:-0}"
LAYER_PAUSE_MS="${7:-0}"
IGNITE_VERBOSE="${8:-off}"

case "$IGNITE_VERBOSE" in
    1|on|ON|true|TRUE|yes|YES)
        IGNITE_VERBOSE_ARG="--ignite-verbose"
        ;;
    *)
        IGNITE_VERBOSE_ARG=""
        ;;
esac

restore_system_state() {
    status=$?

    echo 1023 > /sys/class/backlight/panel0-backlight/brightness 2>/dev/null || true
    echo "[restore] screen on"

    echo walt > /sys/devices/system/cpu/cpufreq/policy0/scaling_governor 2>/dev/null || true
    echo walt > /sys/devices/system/cpu/cpufreq/policy6/scaling_governor 2>/dev/null || true
    echo "CPU Governor reset (policy0): $(cat /sys/devices/system/cpu/cpufreq/policy0/scaling_governor 2>/dev/null)"
    echo "CPU Governor reset (policy6): $(cat /sys/devices/system/cpu/cpufreq/policy6/scaling_governor 2>/dev/null)"

    echo "[inference] done."

    trap - EXIT INT TERM
    exit "$status"
}

trap restore_system_state EXIT INT TERM

# screen brightness control
echo 0 > /sys/class/backlight/panel0-backlight/brightness

# Keep governor setup in the script, but let ignite-npu control per-phase DVFS.
echo performance > /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
echo performance > /sys/devices/system/cpu/cpufreq/policy6/scaling_governor
echo "CPU Governor (policy0): $(cat /sys/devices/system/cpu/cpufreq/policy0/scaling_governor)"
echo "CPU Governor (policy6): $(cat /sys/devices/system/cpu/cpufreq/policy6/scaling_governor)"
sleep 2

echo "[setup] DVFS device: $DEV"
echo "[setup] DVFS indices: prefill(cpu=$CPU_P, ram=$RAM_P), decode(cpu=$CPU_D, ram=$RAM_D)"
echo "[setup] Phase pause: ${PHASE_PAUSE_MS}ms"
echo "[setup] Token pause: ${TOKEN_PAUSE_MS}ms"
echo "[setup] Layer pause: ${LAYER_PAUSE_MS}ms"
echo "[setup] Ignite verbose: ${IGNITE_VERBOSE}"

setenforce 0 || true

export LD_LIBRARY_PATH=/data/local/tmp/llama.cpp/lib
export ADSP_LIBRARY_PATH=/data/local/tmp/llama.cpp/lib
export GGML_HEXAGON_HOSTBUF=1

cd /data/local/tmp/llama.cpp || exit 1

taskset fe ./bin/llama-ignite-npu \
    -m /data/local/tmp/gguf/qwen-3-1.7b-q4_k_m.gguf \
    -t 6 -tb 6 -i -cnv -ub 512 -b 512 -fa off \
    --json-path data/qwen3_prefill_64_20.json \
    --max-query-number 20 \
    --strict on \
    --strict-limit 128 \
    --output-dir output \
    --backend-compute-profile \
    --backend-op-breakdown \
    --temp 0 \
    --top-k 1 \
    -c 1024 \
    --device HTP0 \
    --dvfs-device "$DEV" \
    --cpu-p "$CPU_P" \
    --ram-p "$RAM_P" \
    --cpu-d "$CPU_D" \
    --ram-d "$RAM_D" \
    --phase-pause "$PHASE_PAUSE_MS" \
    --token-pause "$TOKEN_PAUSE_MS" \
    --layer-pause "$LAYER_PAUSE_MS" \
    ${IGNITE_VERBOSE_ARG}
