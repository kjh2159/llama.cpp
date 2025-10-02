find_package(Threads REQUIRED)
llama_add_compile_flags()

set(TARGET ignite)
add_executable(${TARGET} ignite.cpp)
target_link_libraries(${TARGET} PRIVATE common llama dvfs ${CMAKE_THREAD_LIBS_INIT})
target_compile_features(${TARGET} PRIVATE cxx_std_17)
install(TARGETS ${TARGET} RUNTIME)
