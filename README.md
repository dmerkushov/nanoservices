nanoservices
============

[![Documentation](https://codedocs.xyz/dmerkushov/nanoservices.svg)](https://codedocs.xyz/dmerkushov/nanoservices/)

Nanoservices framework

It is in development by now, a more meaningful readme will be added later

How to build
------------

You'll need: `cmake` (minimum version is 3.0), `make`, `libzookeeper-mt2`, local instance of `zookeeper` and a C++11 compiler (the framework is tested with gcc 4.7)

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

Also successful build generate misc tool `ns-configurator` at `nanoservices/build/misc/ns-configurator`

Build debian packages
---------------------

For build debian packages from builded sources

```bash
cd build/
cpack
```


