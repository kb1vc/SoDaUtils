#!/bin/bash

# For build integrity tests.
# Here's what we test:
# 
# 1. Local build with no yaml installed
# 2. Local build with yaml
# 3. Install build with no yaml installed
# 4. Install build with yaml
# 5. Build with install from RPM
# 
# First
# apptainer pull docker://fedora:37

# Then build the containers where we'll do the kit build
# First items 1, 3
echo "\n\nBUILD 1"
sudo apptainer build  Fedora_noyaml.sif Fedora_noyaml.def
# items 2 and 4
echo "\n\nBUILD 2"
sudo apptainer build  Fedora_yaml.sif `pwd`/Fedora_with_yaml.def

# test 1 local build with no yaml
echo "\n\nTEST 1"
mkdir test_1
rm -rf test_1/*
apptainer exec --bind `pwd`/test_1:/mnt --bind `pwd`/../:/source_tree  Fedora_noyaml.sif ./build_test_local.sh

# now do test 2 local build with yaml  uses
echo "\n\nTEST 2"
mkdir test_2
rm -rf test_2/*
apptainer exec --bind `pwd`/test_2:/mnt --bind `pwd`/../:/source_tree  Fedora_yaml.sif ./build_test_local.sh

# test 3 installed build with no yaml
echo "\n\nTEST 3"
mkdir test_3
rm -rf test_3/*
apptainer exec --bind `pwd`/test_3:/mnt --bind `pwd`/../:/source_tree  Fedora_noyaml.sif ./build_test_install.sh

# test 4 installed build with yaml
echo "\n\nTEST 4"
mkdir test_4
rm -rf test_4/*
apptainer exec --bind `pwd`/test_4:/mnt --bind `pwd`/../:/source_tree  Fedora_yaml.sif ./build_test_install.sh


# now do test 5 -- installed build from RPM
echo "\n\nTEST 5"
mkdir test_5
rm -rf test_5/*
mkdir test_5_build
rm -rf test_5_build/*
mkdir kit_location
rm -rf kit_location/*
# make the RPM
echo "\n\nTEST 5 RPM CREATE"
apptainer exec --bind `pwd`/test5_build:/mnt --bind `pwd`/../:/source_tree --bind `pwd`/kit_location:/kitloc Fedora_noyaml.sif ./build_rpm.sh
# Now build a new image with the RPM installed
echo "\n\nTEST 5 RPM INSTALL/BUILD"
sudo apptainer build --bind `pwd`/kit_location:/mnt  Fedora_rpm_test.sif Fedora_rpm_test.def

# Finally, run test 5
echo "\n\nTEST 5 example build"
apptainer exec --bind `pwd`/test_5:/mnt --bind `pwd`/../:/source_tree  Fedora_rpm_test.sif ./build_test_install.sh
