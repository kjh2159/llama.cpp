#!/bin/sh
# run_all.sh - wrapper: HW setup + logging -> NPU inference -> stop logging + restore
# Run:
#   sh run_all.sh
# (필요시) su 권한은 각 스크립트 내부에서 su -c 로 요청함

cd /data/local/tmp/llama.cpp 2>/dev/null || {
    echo "[error] cannot cd to /data/local/tmp/llama.cpp";
    exit 1;
}

sh ./hw_setup_log.sh start

cleanup() {
    sh ./hw_setup_log.sh stop
}
trap cleanup EXIT INT TERM

sh ./run_npu_infer.sh

# trap에 의해 stop/restore 수행
