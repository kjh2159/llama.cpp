# llama.cpp

![llama](https://user-images.githubusercontent.com/1991296/230134379-7181e485-c521-4d23-a0d6-f7b3b61ba524.png)

[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Release](https://img.shields.io/github/v/release/ggml-org/llama.cpp)](https://github.com/ggml-org/llama.cpp/releases)
[![Server](https://github.com/ggml-org/llama.cpp/actions/workflows/server.yml/badge.svg)](https://github.com/ggml-org/llama.cpp/actions/workflows/server.yml)

[Manifesto](https://github.com/ggml-org/llama.cpp/discussions/205) / [ggml](https://github.com/ggml-org/ggml) / [ops](https://github.com/ggml-org/llama.cpp/blob/master/docs/ops.md)

LLM inference in C/C++

## Recent API changes

- [Changelog for `libllama` API](https://github.com/ggml-org/llama.cpp/issues/9289)
- [Changelog for `llama-server` REST API](https://github.com/ggml-org/llama.cpp/issues/9291)

## Hot topics

- **[guide : running gpt-oss with llama.cpp](https://github.com/ggml-org/llama.cpp/discussions/15396)**
- **[[FEEDBACK] Better packaging for llama.cpp to support downstream consumers 🤗](https://github.com/ggml-org/llama.cpp/discussions/15313)**
- Support for the `gpt-oss` model with native MXFP4 format has been added | [PR](https://github.com/ggml-org/llama.cpp/pull/15091) | [Collaboration with NVIDIA](https://blogs.nvidia.com/blog/rtx-ai-garage-openai-oss) | [Comment](https://github.com/ggml-org/llama.cpp/discussions/15095)
- Hot PRs: [All](https://github.com/ggml-org/llama.cpp/pulls?q=is%3Apr+label%3Ahot+) | [Open](https://github.com/ggml-org/llama.cpp/pulls?q=is%3Apr+label%3Ahot+is%3Aopen)
- Multimodal support arrived in `llama-server`: [#12898](https://github.com/ggml-org/llama.cpp/pull/12898) | [documentation](./docs/multimodal.md)
- VS Code extension for FIM completions: https://github.com/ggml-org/llama.vscode
- Vim/Neovim plugin for FIM completions: https://github.com/ggml-org/llama.vim
- Introducing GGUF-my-LoRA https://github.com/ggml-org/llama.cpp/discussions/10123
- Hugging Face Inference Endpoints now support GGUF out of the box! https://github.com/ggml-org/llama.cpp/discussions/9669
- Hugging Face GGUF editor: [discussion](https://github.com/ggml-org/llama.cpp/discussions/9268) | [tool](https://huggingface.co/spaces/CISCai/gguf-editor)

----

## Quick start (llama.cpp)

The quick start guide of the basic llama.cpp follows the [original repository](https://github.com/ggml-org/llama.cpp).
The main focus of *IGNITE* for on-device inference is based on `llama-cli`, guided by the [`llama-completion`](https://github.com/ggml-org/llama.cpp/tree/master/tools/completion).

## Quick start (*IGNITE*)

### Model download

```sh
python downloader.py
```
Through this file, you can download models, which are pre-selected to evaluate themselves on *IGNITE*.
If there is no preferred model, you can download and run your own gguf models also.


### Build (on-device)

```sh
cd scripts && sh build-android.sh && cd ..
```

### Run (on-device)

```sh
chmod +x scripts-termux/run.sh
su -c "taskset f0 sh scripts-termux/run.sh"
```

### Build (Linux)

```sh
cd scripts && sh build.sh && cd ..
```

### Run (Linux)
```sh
./build/bin/ignite \
    -m models/qwen-1.5-0.5b-chat-q4k.gguf \
    -cnv \
    --temp 0 \
    --top-k 1 \
    --threads 1 \
    --output-path outputs/hotpot_0_0.csv \
    --json-path dataset/hotpot_qa_30.json
```

*This will be filled up. Please wait.*