#!/usr/bin/env bash
#
# Builds Switch-specific dependencies that are not yet packaged in standard dkp-pacman
# (e.g. SDL3, plutovg, plutosvg, webp) using the devkitA64 toolchain.
#
# Usage: ./build-switch-deps.sh <install-prefix>
# Example: ./build-switch-deps.sh /opt/devkitpro/portlibs/switch
#

set -euo pipefail

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <install-prefix>"
    exit 1
fi

PREFIX=$(realpath "$1")
BUILD_DIR=$(mktemp -d /tmp/switch-deps-build.XXXXXX)
NPROCS="$(nproc || echo 4)"

echo "=== Building Nintendo Switch Dependencies ==="
echo "Target Prefix: $PREFIX"
echo "Working Directory: $BUILD_DIR"

export DEVKITPRO="${DEVKITPRO:-/opt/devkitpro}"
TOOLCHAIN_FILE="${DEVKITPRO}/cmake/Switch.cmake"

if [ ! -f "$TOOLCHAIN_FILE" ]; then
    echo "Error: Switch.cmake not found at $TOOLCHAIN_FILE"
    exit 1
fi

CMAKE_COMMON_FLAGS=(
    "-DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE"
    "-DCMAKE_INSTALL_PREFIX=$PREFIX"
    "-DCMAKE_PREFIX_PATH=$PREFIX"
    "-DCMAKE_BUILD_TYPE=Release"
    "-DBUILD_SHARED_LIBS=OFF"
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
)

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../../.." && pwd)"

# 1. Plutovg (using patched in-tree source)
echo "--- Building Plutovg ---"
cmake -B "$BUILD_DIR/build-plutovg" -S "$REPO_ROOT/3rdparty/plutovg" \
    "${CMAKE_COMMON_FLAGS[@]}" \
    -DPLUTOVG_BUILD_STATIC=ON \
    -DPLUTOVG_BUILD_EXAMPLES=OFF
cmake --build "$BUILD_DIR/build-plutovg" -j"$NPROCS" --target install

# 2. Plutosvg (using in-tree source)
echo "--- Building Plutosvg ---"
cmake -B "$BUILD_DIR/build-plutosvg" -S "$REPO_ROOT/3rdparty/plutosvg" \
    "${CMAKE_COMMON_FLAGS[@]}" \
    -DPLUTOSVG_BUILD_STATIC=ON \
    -DPLUTOSVG_BUILD_EXAMPLES=OFF
cmake --build "$BUILD_DIR/build-plutosvg" -j"$NPROCS" --target install

# 3. SDL3 for Nintendo Switch
echo "--- Building SDL3 ---"
cd "$BUILD_DIR"
git clone --depth 1 --branch release-3.2.6 https://github.com/libsdl-org/SDL.git
cmake -B build-sdl3 -S SDL \
    "${CMAKE_COMMON_FLAGS[@]}" \
    -DSDL_STATIC=ON \
    -DSDL_SHARED=OFF \
    -DSDL_TEST_LIBRARY=OFF
cmake --build build-sdl3 -j"$NPROCS" --target install

# 4. Shaderc (glslang + SPIRV-Tools) for Nintendo Switch
echo "--- Building Shaderc ---"
cd "$BUILD_DIR"
git clone --depth 1 --branch v2025.4 https://github.com/google/shaderc.git
cd shaderc
python3 utils/git-sync-deps
cd ..
cmake -B build-shaderc -S shaderc \
    "${CMAKE_COMMON_FLAGS[@]}" \
    -DSHADERC_SKIP_TESTS=ON \
    -DSHADERC_SKIP_EXAMPLES=ON \
    -DSHADERC_SKIP_COPYRIGHT_CHECK=ON \
    -DENABLE_GLSLANG_BINARIES=OFF \
    -DSPIRV_SKIP_EXECUTABLES=ON
cmake --build build-shaderc -j"$NPROCS" --target shaderc_combined
mkdir -p "$PREFIX/lib" "$PREFIX/include"
cp build-shaderc/libshaderc/libshaderc_combined.a "$PREFIX/lib/"
cp -r shaderc/libshaderc/include/shaderc "$PREFIX/include/"

# Cleanup temporary build directory
rm -rf "$BUILD_DIR"
echo "=== All Switch dependencies successfully built and installed to $PREFIX ==="
