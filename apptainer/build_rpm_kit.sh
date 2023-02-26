#!/bin/bash

# build the whole shebang.

if [ "$1" == "LOCAL" ]
then
    INSTALL_ROOT=/mnt/install_root
    SOURCE_ROOT=/source_tree
else
    INSTALL_ROOT=/usr
    SOURCE_ROOT=/mnt/source_tree
fi

if [ "$2" == "BUILD_KIT" ]
then
    KBUILD_OPT="-DBUILD_RPM=ON"
else
    KBUILD_OPT=""
fi



echo "****************"
echo ${INSTALL_ROOT}
echo $1
echo "***************"


OS_ID=`grep "^ID=" /etc/os-release | sed -e 's/ID=//'`
PVERSION_ID=`grep "VERSION_ID=" /etc/os-release`
VERSION_ID=`echo ${PVERSION_ID} | sed -e 's/VERSION_ID=//' | sed -e 's/\"//g'`

echo "*****************"
echo "OS_ID = [${OS_ID}]"
echo "VERSION_ID = [${VERSION_ID}]"
echo "*****************"
RPM_KIT_VERSION="${OS_ID}_${VERSION_ID}"

cd /mnt
mkdir tool_tree
cd tool_tree
mkdir build
cd build
cmake  -DCMAKE_INSTALL_PREFIX=${INSTALL_ROOT} -DPACKAGE_SYSTEM_NAME=${RPM_KIT_VERSION} ${KBUILD_OPT} ${SOURCE_ROOT}
make
make install


if [ "$2" == "BUILD_KIT" ]
then
    make package
fi


