set(TARGET llama-phase-pause)

# `hard/` 디렉터리의 모든 .cpp 파일 포함
# file(GLOB_RECURSE HARD_SOURCES "hard/*.cpp")

# 실행 파일 추가 (main.cpp + hard 디렉터리 소스 포함)
add_executable(${TARGET} phase-pause.cpp ${HARD_SOURCES})

# `hard/` 디렉터리를 포함 경로로 추가 (헤더 파일 사용 가능)
# target_include_directories(${TARGET} PRIVATE "hard")

# 필요한 라이브러리 연결 (기존 코드 유지)
# hard library is integrated into ignite
find_package(Threads REQUIRED)
target_link_libraries(${TARGET} PRIVATE common llama dvfs ${CMAKE_THREAD_LIBS_INIT} Threads::Threads)

# C++17 사용 (기존 코드 유지)
target_compile_features(${TARGET} PRIVATE cxx_std_17)

# 실행 파일 설치 (기존 코드 유지)
install(TARGETS ${TARGET} RUNTIME)
