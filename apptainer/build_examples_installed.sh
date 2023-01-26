#!/bin/bash -v

# build the examples dir from install from rpm
# We assume you've done this
#apptainer pull docker://fedora:37
#sudo apptainer build Fedora37Test_rpm_test.sif Fedora37_rpm_test.def
# now do
#apptainer exec --bind `pwd`/test_rpm:/mnt Fedora37_rpm_test.sif build_examples_installed.sh
cd /mnt
# then build the  examples
mkdir example
cd example
cp /usr/share/SoDaUtils/examples/* .
mkdir build
cd build
cmake ../
make
