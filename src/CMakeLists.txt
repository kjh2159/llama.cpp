llama_add_compile_flags()

#
# libraries
#

# llama

add_library(llama
            ../include/llama.h
            llama.cpp
            llama-adapter.cpp
            llama-arch.cpp
            llama-batch.cpp
            llama-chat.cpp
            llama-context.cpp
            llama-grammar.cpp
            llama-hparams.cpp
            llama-impl.cpp
            llama-kv-cache.cpp
            llama-mmap.cpp
            llama-model-loader.cpp
            llama-model.cpp
            llama-quant.cpp
            llama-sampling.cpp
            llama-vocab.cpp
            unicode.h
            unicode.cpp
            unicode-data.cpp
            )

add_library(ignite
            hard/device.h
            hard/device.cpp
            hard/dvfs.h
            hard/dvfs.cpp
            hard/record.h
            hard/record.cpp
            hard/utils.h
            hard/utils.cpp
            )

target_include_directories(llama PUBLIC . ../include)
target_compile_features   (llama PUBLIC cxx_std_17) # don't bump
target_link_libraries(llama PUBLIC ggml)

target_include_directories(ignite PUBLIC . .)
target_compile_features   (ignite PUBLIC cxx_std_17) # don't bump
target_link_libraries(ignite PUBLIC ggml)

if (BUILD_SHARED_LIBS)
    set_target_properties(llama PROPERTIES POSITION_INDEPENDENT_CODE ON)
    target_compile_definitions(llama PRIVATE LLAMA_BUILD)
    target_compile_definitions(llama PUBLIC  LLAMA_SHARED)
endif()
