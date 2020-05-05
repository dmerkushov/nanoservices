nanoservices
============

[![Documentation](https://codedocs.xyz/dmerkushov/nanoservices.svg)](https://codedocs.xyz/dmerkushov/nanoservices/)

Nanoservices framework

It is in development by now, a more meaningful readme will be added later

## Requirements

You'll need: `cmake` (minimum version is 3.0), `make`, `libzookeeper-mt-dev`, running local instance of `zookeeper` and a C++11 compiler (the framework is tested with gcc 6+)

### Ubuntu 18.04

`build-essential`

`software-properties-common`

`gcc-6 g++-6`

`cmake`

`make`

`libzookeeper-mt-dev`

`zookeeper`


If you want to build with gcc 6+ do the following

```bash
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-6 60 --slave /usr/bin/g++ g++ /usr/bin/g++-6

# to pass test with zookeeper run
 sudo /usr/share/zookeeper/bin/zkServer.sh start

How to build
------------



```bash
git clone https://github.com/dmerkushov/nanoservices.git
git checkout ag-develop
cd nanoservices/
mkdir build
cd build/
cmake ..
cmake --build .
cd ../..
```

For build without dependence from `zookeeper`

```bash
cmake -DZOO=OFF ..
```

After a successful build and all the tests passing the resulting shared object will be `nanoservices/build/ns-skeleton/libns-skeleton.so`

Header files to include to use the library will be in `nanoservices/ns-skeleton/include/`

A successful build will also generate some tools in `nanoservices/build/misc/` folder. 
By now, these are tools for configuring nanoservices (ns-configurator) and shutdowning a started nanoservice remotely (ns-shutdown). [More info](/misc/README.md)

It is highly recommended to use the supplied tools to configure nanoservices, as different configuration source strategies may be applied by the nanoservice hosting teams.

Build debian packages
---------------------

To build debian packages from built sources

```bash
cd build/
cpack
```
