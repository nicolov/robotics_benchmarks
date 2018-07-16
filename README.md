# Robotics benchmarks

[![CircleCI](https://circleci.com/gh/nicolov/robotics_benchmarks.svg?style=shield)](https://circleci.com/gh/nicolov/robotics_benchmarks)

This repo contains benchmarks for a few standalone algorithms in Robotics. The code
has no system dependencies, and compiles down to a static binary using a cross
compiler built using [crosstool-ng](https://crosstool-ng.github.io/). This allows
us to use a recent `gcc` version (currently 8.1.0) to get the latest
optimizations.

The [CircleCI](https://circleci.com/gh/nicolov/robotics_benchmarks) configuration
builds (and runs) the benchmark binaries for both Intel x86 and a Raspberry Pi 3,
using both `gcc` and `clang`.

## Included benchmarks

- **Kalman filter**: the covariance update step of a fictional Kalman filter.
This is a good test of linear algebra performance using Eigen, as it requires
matrix multiplication and inversion.

## Running

```
docker-compose build
docker-compose run runner bash

mkdir build
cd build
TRIPLE=x86_64-unknown-linux-gnu cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake ..
make -j$(nproc)
./bm
```
