# dynpax
C++ utility to turn your dynamic ELF binary into standalone packaged bundle


## Why it is useful?

Sometimes it is painful to create fully static build of C/C++ application, so one must collect all libraries by 
using output from `ldd` and manually or using scripts to copy all needed libraries into fake root directory.
This tool simplifies this process as well as add some additional features.

## Build/Compilation

### Dependencies

1. cmake >= 3.28
2. ninja >= 1.11
3. gcc   >= 14.0
4. vcpkg

### Compile and install

1. Set `VCPKG_ROOT` environment variable e.g `export VCPKG_ROOT=<path to vcpkg repository>`
2. Run `cmake --preset linux-gcc14-release` to configure and install dependencies
3. Run `cmake --build --preset linux-gcc14-release --target dynpax` to build the target
4. Run `cmake --install ./build/linux-gcc14-release --prefix /tmp/dynpax` to install
5. Run `/opt/dynpax/bin/dynpax -t /opt/dynpax/bin/dynpax -f /usr/local/dynpax` to install clean standalone bundle.

### Create docker image

1. Run `docker build -t smartcoder/dynpax .`

### Use docker image

See example of possible usage in `Dockerfile.example`

1. Run `docker build -t example . -f Dockerfile.example`
2. Run `docker --rm -it example`
