# Copyright 2023 Zzzode. All rights reserved.

set -e

# Remove the build directory if it exists
if [ -d 'build' ]; then
  rm -rf build
fi

# Configure the build with CMake
cmake -DCMAKE_BUILD_TYPE=Release -Bbuild -GNinja .

# Build the base64 target with Ninja
ninja -C build main
