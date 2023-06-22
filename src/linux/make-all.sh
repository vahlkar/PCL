#!/bin/bash

# PCL environment variables must be defined.
if [ -z ${PCLDIR+x} ] ||
   [ -z ${PCLBINDIR64+x} ] ||
   [ -z ${PCLBINDIR+x} ] ||
   [ -z ${PCLLIBDIR64+x} ] ||
   [ -z ${PCLLIBDIR+x} ] ||
   [ -z ${PCLINCDIR+x} ] ||
   [ -z ${PCLSRCDIR+x} ]; then
  echo "PCL environment variables must be defined and valid.";
  echo "Reference: https://gitlab.com/pixinsight/PCL";
  exit
fi

# Create the bin and lib directories if not existing.
[ ! -d $PCLBINDIR64 ] && mkdir -p $PCLBINDIR64
[ ! -d $PCLLIBDIR64 ] && mkdir -p $PCLLIBDIR64

NPROC=$(nproc --ignore=4)

cd $PCLSRCDIR/pcl/linux/g++
make -j $NPROC

cd $PCLSRCDIR/3rdparty/linux
./make-3rdparty.sh

cd $PCLSRCDIR/modules/file-formats/linux
./make-file-formats.sh

cd $PCLSRCDIR/modules/processes/linux
./make-processes.sh

cd $PCLSRCDIR
