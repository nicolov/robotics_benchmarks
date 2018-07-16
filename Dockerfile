FROM ubuntu:18.04

ENV DEBIAN_FRONTEND noninteractive

# Development
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        autoconf \
        automake \
        bison \
        build-essential \
        ca-certificates \
        cmake \
        curl \
        flex \
        gawk \
        git \
        help2man \
        libncurses-dev \
        libtool \
        libtool-bin \
        python-dev \
        texinfo \
        unzip \
        vim \
        wget

# Install crosstool-ng
COPY scripts/install_crosstool_ng /tmp
RUN /tmp/install_crosstool_ng
# crosstool-ng refuses to run as root
RUN useradd -m ctuser
# prepare destination directory for the toolchains
RUN mkdir /opt/x-tools && chown -R ctuser /opt/x-tools

# build the toolchains
COPY scripts/build_toolchain /tmp
COPY toolchain_configs /tmp/toolchain_configs

RUN su ctuser -c 'CT_PREFIX=/opt/x-tools TRIPLE=x86_64-unknown-linux-gnu /tmp/build_toolchain'
RUN su ctuser -c 'CT_PREFIX=/opt/x-tools TRIPLE=armv8-rpi3-linux-gnueabihf /tmp/build_toolchain'

# download and install the clang release
RUN wget -O /tmp/clang.tar.gz \
        http://releases.llvm.org/6.0.0/clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-16.04.tar.xz \
    && mkdir /opt/clang \
    && tar -C /opt/clang --strip-components=1 -xf /tmp/clang.tar.gz \
    && rm /tmp/clang.tar.gz

# extra tooling for circle
RUN apt-get install -y --no-install-recommends \
    git \
    ssh
