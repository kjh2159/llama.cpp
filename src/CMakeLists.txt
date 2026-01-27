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
            llama-cparams.cpp
            llama-grammar.cpp
            llama-graph.cpp
            llama-hparams.cpp
            llama-impl.cpp
            llama-ignite.cpp
            llama-io.cpp
            llama-kv-cache.cpp
            llama-kv-cache-iswa.cpp
            llama-memory.cpp
            llama-memory-hybrid.cpp
            llama-memory-recurrent.cpp
            llama-mmap.cpp
            llama-model-loader.cpp
            llama-model-saver.cpp
            llama-model.cpp
            llama-quant.cpp
            llama-sampling.cpp
            llama-vocab.cpp
            unicode-data.cpp
            unicode.cpp
            unicode.h
            hard/utils.cpp
            )

add_library(dvfs
            hard/device.cpp
            hard/device.h
            hard/dvfs.cpp
            hard/dvfs.h
            hard/record.cpp
            hard/record.h
            hard/utils.cpp
            hard/utils.h
            hard/affinity.cpp
            hard/affinity.h
            )

target_include_directories(llama PRIVATE .)
target_include_directories(llama PUBLIC ../include)
target_compile_features   (llama PRIVATE cxx_std_17) # don't bump
target_link_libraries(llama PUBLIC ggml)

target_include_directories(dvfs PRIVATE .)
target_include_directories(dvfs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features   (dvfs PRIVATE cxx_std_17) # don't bump
target_link_libraries(dvfs PUBLIC ggml)
if (IGNITE_USE_SYSTEM_DVFS)
    target_compile_definitions(dvfs PUBLIC IGNITE_USE_SYSTEM_DVFS=1)
else()
    target_compile_definitions(dvfs PUBLIC IGNITE_USE_SYSTEM_DVFS=0)
endif()

if (BUILD_SHARED_LIBS)
    set_target_properties(llama PROPERTIES POSITION_INDEPENDENT_CODE ON)
    target_compile_definitions(llama PRIVATE LLAMA_BUILD)
    target_compile_definitions(llama PUBLIC  LLAMA_SHARED)
endif()
