# # !/bin/bash

# # 현재 스크립트가 있는 디렉토리
# SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# # 라이브러리(.so) 파일 찾기
# LIB_DIRS=$(find "$SCRIPT_DIR" -type f -name "*.so" -exec dirname {} \; | sort -u)

# # 실행 파일 찾기 (.so가 아닌 실행 가능한 ELF 바이너리)
# EXECUTABLE=$(find "$SCRIPT_DIR" -type f -executable ! -name "*.so" | grep -i "main\|llama" | head -n 1)

# # 라이브러리 경로 설정
# if [[ -n "$LIB_DIRS" ]]; then
#     export LD_LIBRARY_PATH=$(echo "$LIB_DIRS" | tr '\n' ':'):$LD_LIBRARY_PATH
#     echo "✅ LD_LIBRARY_PATH 설정 완료: $LD_LIBRARY_PATH"
# else
#     echo "⚠️ 라이브러리 디렉토리를 찾을 수 없습니다. 실행이 실패할 수 있습니다."
# fi

# # # 실행 파일이 존재하는지 확인
# # if [[ -z "$EXECUTABLE" ]]; then
# #     echo "❌ 실행 가능한 바이너리를 찾을 수 없습니다!"
# #     exit 1
# # fi

# # 실행 파일 실행
# echo "🚀 실행 중: $EXECUTABLE"

# !/data/data/com.termux/usr/bin/bash

./build/bin/llama-cli \
--hf-repo Qwen/Qwen1.5-1.8B-Chat-GGUF \
--hf-file qwen1_5-1_8b-chat-q4_0.gguf \
-cnv \
--device-name Pixel9 \
--json-path hotpot_qa_3.json \
--cpu-freq 12 \
--ram-freq 12 
