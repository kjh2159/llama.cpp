D_LIBRARY_PATH=lib ./bin/llama-cli \
-m ../models/Qwen1.5-0.5B-Q4_K_M.gguf \
-cnv \
--temp 0
-p "You're a helpful assistant" \
-i \
--top-k 5 \
--threads 1 \
--threads-batch 1 \
--device-name Pixel9 \
--output-path ../output/hotpot_16_12.csv \
--json-path ../dataset/hotpot_qa_20.json \
--cpu-freq 16 \
--ram-freq 12

