##1
./build/bin/llama-cli \
-m /Users/hongseoyong/Documents/code/llama.cpp-dgist/qwen1_5-0_5b-chat-q4_0.gguf \
-cnv \
--device-name Pixel9 \
--output-path ./outputs/hotpot_0_0 \
--json-path hotpot_qa_30.json \
--cpu-freq 0 \
--ram-freq 0 

sleep 1800

##2
./build/bin/llama-cli \
-m /data/data/com.termux/files/home/llama.cpp-dgist/models/qwen1_5-0_5b-chat-q4_0.gguf \
-cnv \
--device-name Pixel9 \
--output-path ./outputs/hotpot_0_6 \
--json-path hotpot_qa_3.json \
--cpu-freq 0 \
--ram-freq 6 

sleep 1800

##3
./build/bin/llama-cli \
-m /data/data/com.termux/files/home/llama.cpp-dgist/models/qwen1_5-0_5b-chat-q4_0.gguf \
-cnv \
--device-name Pixel9 \
--output-path ./outputs/hotpot_0_12 \
--json-path hotpot_qa_3.json \
--cpu-freq 0 \
--ram-freq 12 

sleep 1800

##4
./build/bin/llama-cli \
-m /data/data/com.termux/files/home/llama.cpp-dgist/models/qwen1_5-0_5b-chat-q4_0.gguf \
-cnv \
--device-name Pixel9 \
--json-path hotpot_qa_3.json \
--output-path ./outputs/hotpot_8_0 \
--cpu-freq 8 \
--ram-freq 0

sleep 1800

##5
./build/bin/llama-cli \
-m /data/data/com.termux/files/home/llama.cpp-dgist/models/qwen1_5-0_5b-chat-q4_0.gguf \
-cnv \
--device-name Pixel9 \
--json-path hotpot_qa_3.json \
--output-path ./outputs/hotpot_8_6 \
--cpu-freq 8 \
--ram-freq 6

sleep 1800

##6
./build/bin/llama-cli \
-m /data/data/com.termux/files/home/llama.cpp-dgist/models/qwen1_5-0_5b-chat-q4_0.gguf \
-cnv \
--device-name Pixel9 \
--json-path hotpot_qa_3.json \
--output-path ./outputs/hotpot_8_12 \
--cpu-freq 8 \
--ram-freq 12 

sleep 1800

##7
./build/bin/llama-cli \
-m /data/data/com.termux/files/home/llama.cpp-dgist/models/qwen1_5-0_5b-chat-q4_0.gguf \
-cnv \
--device-name Pixel9 \
--json-path hotpot_qa_3.json \
--output-path ./outputs/hotpot_16_0 \
--cpu-freq 16 \
--ram-freq 0 

sleep 1800

##8
./build/bin/llama-cli \
-m /data/data/com.termux/files/home/llama.cpp-dgist/models/qwen1_5-0_5b-chat-q4_0.gguf \
-cnv \
--device-name Pixel9 \
--json-path hotpot_qa_3.json \
--output-path ./outputs/hotpot_16_6 \
--cpu-freq 16 \
--ram-freq 6

sleep 1800

##9
./build/bin/llama-cli \
-m /data/data/com.termux/files/home/llama.cpp-dgist/models/qwen1_5-0_5b-chat-q4_0.gguf \
-cnv \
--device-name Pixel9 \
--json-path hotpot_qa_3.json \
--output-path ./outputs/hotpot_16_12 \
--cpu-freq 16 \
--ram-freq 12

