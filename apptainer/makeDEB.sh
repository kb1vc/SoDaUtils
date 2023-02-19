#!/bin/bash

apptainer pull docker://ubuntu:22.04

# first build the "common container"
sudo rm -rf Ubuntu_CommonTools.sif
sudo apptainer build Ubuntu_CommonTools.sif Ubuntu_CommonTools.def

# make the  two directories that we'll need
mkdir build_deb_kit
mkdir test_deb_kit
echo "***************************"
echo "BUILDING DEB KIT"
echo "***************************"
cat Ubuntu_generic.def | sed -e 's|#STEP1|apt-get -y install libyaml-cpp-dev|' | sed -e 's|#STEP2|/mnt/script/build_deb_kit.sh INSTALL BUILD_DEB_KIT |' > build_deb_kit/Ubuntu_build.def
# First items 1, 3
sudo apptainer build --bind `pwd`/build_deb_kit:/mnt --bind `pwd`/../:/mnt/source_tree --bind `pwd`:/mnt/script build_deb_kit/Ubuntu_build.sif build_deb_kit/Ubuntu_build.def
echo "\n\n\n"
echo "\n\n\n"
apptainer exec --bind `pwd`/build_deb_kit:/mnt build_deb_kit/Ubuntu_build.sif `pwd`/test_deb_kit.sh INSTALL


echo "***************************"
echo "TESTING DEB KIT"
echo "***************************"
mkdir test_deb_kit
cp build_deb_kit/tool_tree/build/SoDaUtils*.deb test_deb_kit/SoDaUtils_kit.deb
cp build_deb_kit/tool_tree/build/SoDaUtils*.deb ./
cat Ubuntu_generic.def | sed -e 's|#STEP2|apt-get -y install /mnt/SoDaUtils_kit.deb|' > test_deb_kit/Ubuntu_deb.def
sudo apptainer build --bind `pwd`/test_deb_kit:/mnt test_deb_kit/Ubuntu_deb.sif test_deb_kit/Ubuntu_deb.def
echo "\n\n\n"
echo "\n\n\n"
apptainer exec --bind `pwd`/test_deb_kit:/mnt --bind `pwd`/../:/source_tree  test_deb_kit/Ubuntu_deb.sif `pwd`/test_deb_kit.sh INSTALL TESTYAML
