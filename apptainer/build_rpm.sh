#!/bin/bash -v

# build the whole shebang.

cd /mnt
mkdir buildrpm
cd buildrpm
mkdir build
cd build
cmake  -DBUILD_RPM=1 -DCMAKE_INSTALL_PREFIX=/usr /source_tree
make
make package
mv *.rpm /kitloc/SoDaUtilsKit.rpm
mv *.rpm /kitloc/

