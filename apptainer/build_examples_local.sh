#!/bin/bash -v

# build the examples dir from local install (not rpm)
# We assume you've done this
#apptainer pull docker://fedora:37
#sudo apptainer build Fedora37Test.sif Fedora37.def
# apptainer exec --bind `pwd`/../:/source_tree --bind `pwd`/world1:/mnt Fedora37Test.sif ./build_kit_n_rpms.sh

cd /mnt
# then build the  examples
mkdir example
cd example
cp /mnt/install_root/share/SoDaUtils/examples/* .
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=/mnt/install_root ../
make
