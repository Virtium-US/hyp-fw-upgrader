# hyp-fw-upgrader

A cross-platform tool for upgrading firmware for Hyperstone USB products.

## Getting Started

### Linux:
```
git clone https://github.com/Virtium-US/hyp-fw-upgrader.git

cd hyp-fw-upgrader

make
sudo ./build/hyp-fw-upgrader [dev_path] [~filepath/dd.txt]
```

### Windows:
*Note: the instructions below were run using [Cygwin](https://www.cygwin.com/)*
```
git clone https://github.com/Virtium-US/hyp-fw-upgrader.git

cd hyp-fw-upgrader

make -f Makefile_WIN32
./build/hyp-fw-upgrader.exe [dev_path] [~filepath\dd.txt]
```