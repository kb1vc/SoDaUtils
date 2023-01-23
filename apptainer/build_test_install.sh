#!/bin/bash -v

# build the whole shebang.

cd /mnt
mkdir tool_tree
cd tool_tree
mkdir build
cd build
cmake  -DCMAKE_INSTALL_PREFIX=/usr /source_tree
make
make install
# now build the examples
cd /mnt
mkdir example_test
cd example_test
mkdir build
cp /mnt/install_root/share/SoDaUtils/examples/* .
cd build
cmake ../
make
./PropertyTreeYamlExample ../test1.yaml
./FormatExample

