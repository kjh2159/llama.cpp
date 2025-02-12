./build/bin/llama-cli --json-path /workspace/test_files/s_data.json -m /workspace/models/Llama-3.2-1B-Instruct-Q4_0.gguf --output-csv-path ./outputs/result_is_os.csv --device-name 'S24' -c 4096 -p 'You are Helpful Assistant. Provide only answer to the user without solution process.' -n 16

./build/bin/llama-cli --json-path /workspace/test_files/l_data.json -m /workspace/models/Llama-3.2-1B-Instruct-Q4_0.gguf --output-csv-path ./outputs/result_il_os.csv --device-name 'S24' -c 4096 -p 'You are Helpful Assistant. Provide only answer to the user without solution process.' -n 16


./build/bin/llama-cli --json-path /workspace//test_files/s_data.json -m /workspace/models/Llama-3.2-1B-Instruct-Q4_0.gguf --output-csv-path ./outputs/result_is_ol.csv --device-name 'S24' -c 4096 -p 'You are Helpful Assistant. Provide the user with the correct answer along with as detailed a solution as possible.' -n 4096 

./build/bin/llama-cli --json-path /workspace/test_files/l_data.json -m /workspace/models/Llama-3.2-1B-Instruct-Q4_0.gguf --output-csv-path ./outputs/result_il_ol.csv --device-name 'S24' -c 4096 -p 'You are Helpful Assistant. Provide the user with the correct answer along with as detailed a solution as possible.' -n 4096
