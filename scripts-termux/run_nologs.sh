#!/bin/sh
# log_llama.sh - S25 NPU inference with HW logging (thermal/clock/power/CPU util)
# Run from: /data/local/tmp/llama.cpp
# Usage: su -c "sh log_llama.sh"

# ---- Cleanup on exit (normal/SIGINT/SIGTERM) ----
cleanup() {
    kill "$LOG_PID" 2>/dev/null
    wait "$LOG_PID" 2>/dev/null
    # su 자식 프로세스(sh hw_log_cmd, awk 등)는 root 소유 -> su -c pkill로 정리
    su -c "pkill -f hw_log_cmd" 2>/dev/null
    su -c "pkill -f 'awk.*thermal'" 2>/dev/null
    rm -f "$LOG_SCRIPT" 2>/dev/null
}
trap cleanup EXIT INT TERM
# ---- S25 CPU freq (kHz) ----
CLK0=2918400
CLK6=3840000

echo "[setup] policy0=${CLK0} kHz, policy6=${CLK6} kHz"

# ---- Screen off ----
su -c "echo 0 > /sys/class/backlight/panel0-backlight/brightness"

# ---- CPU governor: performance + fix freq ----
su -c "echo performance > /sys/devices/system/cpu/cpufreq/policy0/scaling_governor"
su -c "echo performance > /sys/devices/system/cpu/cpufreq/policy6/scaling_governor"

su -c "chmod 644 /sys/devices/system/cpu/cpufreq/policy0/scaling_max_freq"
su -c "echo $CLK0 > /sys/devices/system/cpu/cpufreq/policy0/scaling_max_freq"
su -c "chmod 444 /sys/devices/system/cpu/cpufreq/policy0/scaling_max_freq"
su -c "chmod 644 /sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq"
su -c "echo $CLK0 > /sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq"
su -c "chmod 444 /sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq"

su -c "chmod 644 /sys/devices/system/cpu/cpufreq/policy6/scaling_max_freq"
su -c "echo $CLK6 > /sys/devices/system/cpu/cpufreq/policy6/scaling_max_freq"
su -c "chmod 444 /sys/devices/system/cpu/cpufreq/policy6/scaling_max_freq"
su -c "chmod 644 /sys/devices/system/cpu/cpufreq/policy6/scaling_min_freq"
su -c "echo $CLK6 > /sys/devices/system/cpu/cpufreq/policy6/scaling_min_freq"
su -c "chmod 444 /sys/devices/system/cpu/cpufreq/policy6/scaling_min_freq"
su -c "chmod 644 /sys/devices/system/cpu/bus_dcvs/DDR/boost_freq"
su -c "echo 4761000 > /sys/devices/system/cpu/bus_dcvs/DDR/boost_freq"
su -c "chmod 444 /sys/devices/system/cpu/bus_dcvs/DDR/boost_freq"
echo "[setup] DDR boost_freq: $(su -c 'cat /sys/devices/system/cpu/bus_dcvs/DDR/boost_freq')"

echo "[setup] CPU gov: $(su -c 'cat /sys/devices/system/cpu/cpufreq/policy0/scaling_governor') / $(su -c 'cat /sys/devices/system/cpu/cpufreq/policy6/scaling_governor')"
sleep 2


# ---- Run NPU inference (CPU 4-7 via taskset f0) ----
echo "[inference] starting..."
su -p -c "setenforce 0 && \
    export LD_LIBRARY_PATH=/data/local/tmp/llama.cpp/lib && \
    export ADSP_LIBRARY_PATH=/data/local/tmp/llama.cpp/lib && \
    export GGML_HEXAGON_HOSTBUF=1 && \
    cd /data/local/tmp/llama.cpp && \
    taskset f0 ./bin/llama-ignite-npu \
        -m /data/local/tmp/gguf/qwen1_5-0_5b-chat-q4_k_m.gguf \
        -t 1 -tb 4 -np 1 -ub 512 -b 512 -fa off \
        --json-path data/hotpot_qa_30.json \
        --output-dir output \
	--temp 0 \
	--top-k 1 \
	--top-p 0 \
        -c 1024 \
        --device HTP0"

echo "[inference] done."


# ---- Restore CPU freq ----
su -c "chmod 644 /sys/devices/system/cpu/cpufreq/policy0/scaling_max_freq"
su -c "echo 3532800 > /sys/devices/system/cpu/cpufreq/policy0/scaling_max_freq"
su -c "chmod 644 /sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq"
su -c "echo 384000 > /sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq"

su -c "chmod 644 /sys/devices/system/cpu/cpufreq/policy6/scaling_max_freq"
su -c "echo 4473600 > /sys/devices/system/cpu/cpufreq/policy6/scaling_max_freq"
su -c "chmod 644 /sys/devices/system/cpu/cpufreq/policy6/scaling_min_freq"
su -c "echo 1017600 > /sys/devices/system/cpu/cpufreq/policy6/scaling_min_freq"

su -c "echo walt > /sys/devices/system/cpu/cpufreq/policy0/scaling_governor"
su -c "echo walt > /sys/devices/system/cpu/cpufreq/policy6/scaling_governor"
su -c "chmod 644 /sys/devices/system/cpu/bus_dcvs/DDR/boost_freq"
su -c "echo 5470000 > /sys/devices/system/cpu/bus_dcvs/DDR/boost_freq"
echo "[restore] CPU freq/governor restored"

# ---- Screen on ----
su -c "echo 1023 > /sys/class/backlight/panel0-backlight/brightness"

echo "[done]"
