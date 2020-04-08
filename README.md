nanoservices
============

[![Documentation](https://codedocs.xyz/dmerkushov/nanoservices.svg)](https://codedocs.xyz/dmerkushov/nanoservices/)

Nanoservices framework

It is in development by now, a more meaningful readme will be added later

How to build
------------

You'll need: `cmake` (minimum version is 3.0), `make`, `libzookeeper-mt-dev`, running local instance of `zookeeper` and a C++11 compiler (the framework is tested with gcc 6+)

```bash
git clone git@github.com:dmerkushov/nanoservices.git`
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
More info about [here](/misc/README.md)

Build debian packages
---------------------

To build debian packages from built sources

```bash
cd build/
cpack
```

