FROM ubuntu:noble AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt full-upgrade -y && \
    apt install -y gcc-14 g++-14 git curl zip unzip tar wget \
    cmake ninja-build pkgconf make automake autoconf autoconf-archive

ENV CC=/usr/bin/gcc-14
ENV CXX=/usr/bin/g++-14
RUN mkdir -p /root/vcpkg && git clone https://github.com/microsoft/vcpkg.git /root/vcpkg && \
    cd /root/vcpkg && ./bootstrap-vcpkg.sh -disableMetrics

ENV VCPKG_ROOT="/root/vcpkg" 
ENV PATH="/root/vcpkg:$PATH"

COPY . /tmp/dynpax

ENV VCPKG_FORCE_DOWNLOADED_BINARIES="1"

RUN cd /tmp/dynpax && cmake --preset linux-gcc14-release --fresh && \
    cmake --build --preset linux-gcc14-release --target dynpax && \
    cmake --install ./build/linux-gcc14-release --prefix /opt/dynpax

FROM scratch AS runtime

COPY --from=builder /opt/dynpax /

CMD ["/bin/dynpax"]
