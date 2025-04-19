cmake \
	-DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
	-DCMAKE_BUILD_TYPE=Release \
	-DANDROID_ABI="arm64-v8a" \
	-DANDROID_NATIVE_API_LEVEL=android-28 \
	-DANDROID_PLATFORM=android-28 \
	-DNATIVE_LIBRARY_OUTPUT=. \
	-DNATIVE_INCLUDE_OUTPUT=. \
	-DCMAKE_C_FLAGS="-march=armv8-a+dotprod+sve -pthread" \
	-DCMAKE_CXX_FLAGS="-march=armv8-a+dotprod+sve -pthread" \
	-DDEBUG=OFF \
	-DTEST=OFF \
	-DARM=ON \
	-DAPK=OFF \
	-DGGML_OPENMP=OFF \
	-DGGML_LLAMAFILE=OFF \
	-DBUILD_SHARED_LIBS=ON \
	-B build-android

cmake --build build-android --target all -j$(nproc)
cmake --install build-android --prefix build-android/install --config Release
