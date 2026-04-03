#!/bin/sh
# hw_setup_log.sh - S25 HW setup + HW logging (thermal/clock/power/CPU util)
# Usage:
#   sh hw_setup_log.sh start
#   sh hw_setup_log.sh stop
#
# Notes:
# - Logging process is pinned to LITTLE cores (CPU0 by default) and runs at low priority.
# - NPU inference should be pinned to CPU4-7 (see run_npu_infer.sh).

# ---- S25 freq tables (kHz), index is 0-based ----
S25_CPU0_FREQS="384000 556800 748800 960000 1152000 1363200 1555200 1785600 1996800 2227200 2400000 2745600 2918400 3072000 3321600 3532800"
S25_CPU6_FREQS="1017600 1209600 1401600 1689600 1958400 2246400 2438400 2649600 2841600 3072000 3283200 3513600 3840000 4089600 4281600 4473600"
S25_DDR_FREQS="547000 1353000 1555000 1708000 2092000 2736000 3187000 3686000 4224000 4761000"

# ---- User-tunable indices (0-based) ----
# 기존 스크립트에서 사용하던 값:
#   policy0: 2918400 kHz -> idx 12
#   policy6: 3840000 kHz -> idx 12
#   DDR boost: 4761000 kHz -> idx 9
CLK0_IDX=12
CLK6_IDX=12
DDR_IDX=9

# 로깅 주기(초). 기존 0.1 유지
LOG_INTERVAL="0.1"

# 로깅 CPU affinity (taskset mask)
# - 기본: cpu0 (01)로 제한해서 cpu4-7(NPU 추론)와 분리
LOG_TASKSET_MASK="01"

LLAMA_DIR="/data/local/tmp/llama.cpp"
OUT_DIR="${LLAMA_DIR}/output"
STATE_FILE="${OUT_DIR}/hw_log.state"
LOG_ROOT_SCRIPT="/data/local/tmp/hw_log_root.sh"
LOG_ROOT_PID_FILE="${OUT_DIR}/hw_log_root.pid"

freq_from_idx() {
    idx="$1"; shift
    i=0
    for v in "$@"; do
        if [ "$i" -eq "$idx" ]; then
            echo "$v"
            return 0
        fi
        i=$((i+1))
    done
    return 1
}

start_hw_and_log() {
    mkdir -p "$OUT_DIR" 2>/dev/null

    # ---- Resolve kHz from indices ----
    CLK0=$(freq_from_idx "$CLK0_IDX" $S25_CPU0_FREQS) || { echo "[error] invalid CLK0_IDX=$CLK0_IDX"; exit 1; }
    CLK6=$(freq_from_idx "$CLK6_IDX" $S25_CPU6_FREQS) || { echo "[error] invalid CLK6_IDX=$CLK6_IDX"; exit 1; }
    DDR_BOOST=$(freq_from_idx "$DDR_IDX" $S25_DDR_FREQS) || { echo "[error] invalid DDR_IDX=$DDR_IDX"; exit 1; }

    echo "[setup] policy0 idx=${CLK0_IDX} -> ${CLK0} kHz, policy6 idx=${CLK6_IDX} -> ${CLK6} kHz, DDR idx=${DDR_IDX} -> ${DDR_BOOST} kHz"

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
    su -c "echo $DDR_BOOST > /sys/devices/system/cpu/bus_dcvs/DDR/boost_freq"
    su -c "chmod 444 /sys/devices/system/cpu/bus_dcvs/DDR/boost_freq"
    echo "[setup] DDR boost_freq: $(su -c 'cat /sys/devices/system/cpu/bus_dcvs/DDR/boost_freq' 2>/dev/null)"

    echo "[setup] CPU gov: $(su -c 'cat /sys/devices/system/cpu/cpufreq/policy0/scaling_governor' 2>/dev/null) / $(su -c 'cat /sys/devices/system/cpu/cpufreq/policy6/scaling_governor' 2>/dev/null)"
    sleep 2

    # ---- Output paths ----
    TIMESTAMP=$(date +%Y%m%d_%H%M%S)
    LOG_FILE="${OUT_DIR}/npu_llama_${CLK6_IDX}_${DDR_IDX}.csv"

    # ---- CSV header ----
    THERMAL_NAMES=$(su -c "cat /sys/devices/virtual/thermal/thermal_zone*/type" 2>/dev/null | tr '\n' ',')
    MEMINFO_NAMES=$(su -c "awk '{print \$1}' /proc/meminfo" 2>/dev/null | tr -d ':' | tr '\n' ',')

    # create file as shell user (so ownership stays user), then root appends
    : > "$LOG_FILE"

    printf "Time,%s" "$THERMAL_NAMES" > "$LOG_FILE"
    printf "gpu_min_clock,gpu_max_clock," >> "$LOG_FILE"
    printf "cpu0_max_freq,cpu0_cur_freq,cpu6_max_freq,cpu6_cur_freq," >> "$LOG_FILE"
    printf "%s" "$MEMINFO_NAMES" >> "$LOG_FILE"
    printf "power_now,current_now,voltage_now," >> "$LOG_FILE"
    printf "ddr_cur_freq," >> "$LOG_FILE"
    printf "cpu0_util,cpu1_util,cpu2_util,cpu3_util,cpu4_util,cpu5_util,cpu6_util,cpu7_util\n" >> "$LOG_FILE"

    echo "[log] CSV: $LOG_FILE"

    # ---- Root logging loop script (single su process; no per-iteration su) ----
    cat > "$LOG_ROOT_SCRIPT" <<EOF
#!/bin/sh
LOG_FILE="$LOG_FILE"
INTERVAL="$LOG_INTERVAL"
START_MS=$(date +%s%3N)
PID_FILE="$LOG_ROOT_PID_FILE"
EOF

    cat >> "$LOG_ROOT_SCRIPT" <<'EOF'
PREV_STAT=$(grep '^cpu[0-9]' /proc/stat)
echo $$ > "$PID_FILE"

while true; do
    NOW_MS=$(date +%s%3N)
    ELAPSED=$(awk "BEGIN {printf \"%.3f\", ($NOW_MS - $START_MS) / 1000.0}")

    ROW=""

    # thermal temps (C)
    for f in /sys/devices/virtual/thermal/thermal_zone*/temp; do
        if [ -r "$f" ]; then
            read v < "$f"
            ROW="${ROW}$(awk "BEGIN {printf \"%.2f\", $v/1000.0}"),"
        else
            ROW="${ROW},"
        fi
    done

    # gpu min/max
    if read v < /sys/class/kgsl/kgsl-3d0/devfreq/min_freq 2>/dev/null; then ROW="${ROW}${v},"; else ROW="${ROW},"; fi
    if read v < /sys/class/kgsl/kgsl-3d0/devfreq/max_freq 2>/dev/null; then ROW="${ROW}${v},"; else ROW="${ROW},"; fi

    # cpu freqs (MHz)
    if read v < /sys/devices/system/cpu/cpufreq/policy0/scaling_max_freq 2>/dev/null; then ROW="${ROW}$((v/1000)),"; else ROW="${ROW},"; fi
    if read v < /sys/devices/system/cpu/cpufreq/policy0/scaling_cur_freq 2>/dev/null; then ROW="${ROW}$((v/1000)),"; else ROW="${ROW},"; fi
    if read v < /sys/devices/system/cpu/cpufreq/policy6/scaling_max_freq 2>/dev/null; then ROW="${ROW}$((v/1000)),"; else ROW="${ROW},"; fi
    if read v < /sys/devices/system/cpu/cpufreq/policy6/scaling_cur_freq 2>/dev/null; then ROW="${ROW}$((v/1000)),"; else ROW="${ROW},"; fi

    # meminfo (MB)
    ROW="${ROW}$(awk '{printf "%s,", $2/1024}' /proc/meminfo 2>/dev/null)"

    # battery
    if read v < /sys/class/power_supply/battery/power_now 2>/dev/null; then ROW="${ROW}${v},"; else ROW="${ROW},"; fi
    if read v < /sys/class/power_supply/battery/current_now 2>/dev/null; then ROW="${ROW}${v},"; else ROW="${ROW},"; fi
    if read v < /sys/class/power_supply/battery/voltage_now 2>/dev/null; then ROW="${ROW}${v},"; else ROW="${ROW},"; fi

    # ddr cur (MHz)
    if read v < /sys/devices/system/cpu/bus_dcvs/DDR/cur_freq 2>/dev/null; then ROW="${ROW}$((v/1000)),"; else ROW="${ROW},"; fi

    # cpu util
    CURR_STAT=$(grep '^cpu[0-9]' /proc/stat)
    CPU_UTIL=$(printf "%s\n%s" "$PREV_STAT" "$CURR_STAT" | awk '
    {
        core=$1; idle=$5; total=0
        for(i=2;i<=NF;i++) total+=$i
        if(seen[core]) {
            d_idle  = idle - prev_idle[core]
            d_total = total - prev_total[core]
            util = (d_total > 0) ? (d_total - d_idle) / d_total * 100 : 0
            printf "%.1f,", util
        }
        prev_idle[core]=idle; prev_total[core]=total; seen[core]=1
    }')
    PREV_STAT="$CURR_STAT"

    printf "%s,%s%s\n" "$ELAPSED" "$ROW" "$CPU_UTIL" >> "$LOG_FILE"
    sleep "$INTERVAL"
done
EOF

    su -c "chmod 755 $LOG_ROOT_SCRIPT"

    # ---- Start logging: pin to little cores + low priority ----
    # (taskset affinity가 su 내부에서도 적용되도록, root에서 taskset 적용)
    su -c "taskset $LOG_TASKSET_MASK nice -n 19 sh $LOG_ROOT_SCRIPT" &
    LOG_PID=$!

    echo "[log] HW logging started (PID=${LOG_PID}, taskset=${LOG_TASKSET_MASK}, nice=19)"

    # ---- Save state for stop ----
    cat > "$STATE_FILE" <<EOF
LOG_FILE="$LOG_FILE"
LOG_PID="$LOG_PID"
LOG_ROOT_SCRIPT="$LOG_ROOT_SCRIPT"
LOG_ROOT_PID_FILE="$LOG_ROOT_PID_FILE"
CLK0_IDX="$CLK0_IDX"
CLK6_IDX="$CLK6_IDX"
DDR_IDX="$DDR_IDX"
EOF
}

stop_hw_and_log() {
    if [ ! -f "$STATE_FILE" ]; then
        echo "[stop] no state file: $STATE_FILE (nothing to stop?)"
    else
        # shellcheck disable=SC1090
        . "$STATE_FILE" 2>/dev/null

        if [ -n "$LOG_ROOT_PID_FILE" ] && [ -f "$LOG_ROOT_PID_FILE" ]; then
            ROOT_PID=$(cat "$LOG_ROOT_PID_FILE" 2>/dev/null)
            if [ -n "$ROOT_PID" ]; then
                su -c "kill $ROOT_PID" 2>/dev/null
                sleep 0.2
                su -c "kill -9 $ROOT_PID" 2>/dev/null
                echo "[stop] logging stopped (root logger PID=${ROOT_PID})"
            fi
            rm -f "$LOG_ROOT_PID_FILE" 2>/dev/null
        fi

        if [ -n "$LOG_PID" ]; then
            su -c "kill $LOG_PID" 2>/dev/null
            sleep 0.2
            su -c "kill -9 $LOG_PID" 2>/dev/null
        fi

        if [ -n "$LOG_ROOT_SCRIPT" ]; then
            su -c "pkill -f \"$LOG_ROOT_SCRIPT\"" 2>/dev/null
            su -c "rm -f $LOG_ROOT_SCRIPT" 2>/dev/null
        fi

        rm -f "$STATE_FILE" 2>/dev/null
    fi

    # ---- Restore CPU freq (index 기반) ----
    RST_CLK0_MAX=$(freq_from_idx 15 $S25_CPU0_FREQS)
    RST_CLK0_MIN=$(freq_from_idx 0  $S25_CPU0_FREQS)
    RST_CLK6_MAX=$(freq_from_idx 15 $S25_CPU6_FREQS)
    RST_CLK6_MIN=$(freq_from_idx 0  $S25_CPU6_FREQS)
    RST_DDR=$(freq_from_idx 0 $S25_DDR_FREQS)

    su -c "chmod 644 /sys/devices/system/cpu/cpufreq/policy0/scaling_max_freq"
    su -c "echo $RST_CLK0_MAX > /sys/devices/system/cpu/cpufreq/policy0/scaling_max_freq"
    su -c "chmod 644 /sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq"
    su -c "echo $RST_CLK0_MIN > /sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq"

    su -c "chmod 644 /sys/devices/system/cpu/cpufreq/policy6/scaling_max_freq"
    su -c "echo $RST_CLK6_MAX > /sys/devices/system/cpu/cpufreq/policy6/scaling_max_freq"
    su -c "chmod 644 /sys/devices/system/cpu/cpufreq/policy6/scaling_min_freq"
    su -c "echo $RST_CLK6_MIN > /sys/devices/system/cpu/cpufreq/policy6/scaling_min_freq"

    su -c "echo walt > /sys/devices/system/cpu/cpufreq/policy0/scaling_governor"
    su -c "echo walt > /sys/devices/system/cpu/cpufreq/policy6/scaling_governor"

    su -c "chmod 644 /sys/devices/system/cpu/bus_dcvs/DDR/boost_freq"
    su -c "echo $RST_DDR > /sys/devices/system/cpu/bus_dcvs/DDR/boost_freq"

    echo "[restore] CPU freq/governor restored"

    # ---- Screen on ----
    su -c "echo 1023 > /sys/class/backlight/panel0-backlight/brightness"
    echo "[restore] screen on"
}

case "$1" in
    start)
        start_hw_and_log
        ;;
    stop)
        stop_hw_and_log
        ;;
    *)
        echo "Usage: sh $0 {start|stop}"
        exit 1
        ;;
esac
