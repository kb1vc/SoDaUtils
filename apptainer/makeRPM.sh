#!/bin/bash

apptainer pull docker://fedora:37

# first build the "common container"
sudo rm -rf Fedora_CommonTools.sif
sudo apptainer build Fedora_CommonTools.sif Fedora_CommonTools.def

# make the  two directories that we'll need
mkdir build_rpm_kit
mkdir test_rpm_kit
# 4. Install build with yaml
#    Fedora_yaml_install.sif with post /mnt/build_rpm_kit.sh INSTALL
#    run build_rpm.sh
#    run test_kit.sh INSTALL
echo "***************************"
echo "BUILDING RPM"
echo "***************************"
mkdir build_rpm_kit
cat Fedora_generic.def | sed -e 's/#STEP1/dnf install --assumeyes yaml-cpp-devel/' | sed -e 's|#STEP2|/mnt/script/build_rpm_kit.sh INSTALL BUILD_KIT|' > build_rpm_kit/Fedora_noyaml.def
# First items 1, 3
sudo apptainer build --bind `pwd`/build_rpm_kit:/mnt --bind `pwd`/../:/mnt/source_tree --bind `pwd`:/mnt/script build_rpm_kit/Fedora_noyaml.sif build_rpm_kit/Fedora_noyaml.def
echo "\n\n\n"
echo "\n\n\n"
apptainer exec --bind `pwd`/build_rpm_kit:/mnt build_rpm_kit/Fedora_noyaml.sif `pwd`/test_kit.sh INSTALL


# 5. Build with install from RPM
#    Fedora_rpm_install.sif  
#    run test_kit.sh INSTALL
echo "***************************"
echo "TESTING"
echo "***************************"
mkdir test_rpm_kit
cp build_rpm_kit/tool_tree/build/SoDaUtils*.rpm test_rpm_kit/SoDaUtils_kit.rpm
cp build_rpm_kit/tool_tree/build/SoDaUtils*.rpm ./
cat Fedora_generic.def | sed -e 's|#STEP2|dnf install --assumeyes /mnt/SoDaUtils_kit.rpm|' > test_rpm_kit/Fedora_rpm.def
sudo apptainer build --bind `pwd`/test_rpm_kit:/mnt test_rpm_kit/Fedora_rpm.sif test_rpm_kit/Fedora_rpm.def
echo "\n\n\n"
echo "\n\n\n"
apptainer exec --bind `pwd`/test_rpm_kit:/mnt --bind `pwd`/../:/source_tree  test_rpm_kit/Fedora_rpm.sif `pwd`/test_kit.sh INSTALL TESTYAML
