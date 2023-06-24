#!/bin/bash

# environment vars must be defined
if [ -z ${PCLDIR+x} ] ||
   [ -z ${PCLBINDIR64+x} ] ||
   [ -z ${PCLBINDIR+x} ] ||
   [ -z ${PCLLIBDIR64+x} ] ||
   [ -z ${PCLLIBDIR+x} ] ||
   [ -z ${PCLINCDIR+x} ] ||
   [ -z ${PCLSRCDIR+x} ]; then
  echo "PCL environment variables must be defined.";
  echo "Reference: https://gitlab.com/pixinsight/PCL";
  exit
fi

# create the lib dir if not existing
[ ! -d $PCLLIBDIR64 ] && mkdir -p $PCLLIBDIR64
[ ! -d $PCLBINDIR64 ] && mkdir -p $PCLBINDIR64

PCL_MAKE_PATH="${PCLSRCDIR}/pcl/linux/g++"
LIB_3RDPARTY_SCRIPT="${PCLSRCDIR}/3rdparty/linux/make-3rdparty-libs.sh"
FILE_FORMAT_SCRIPT="${PCLSRCDIR}/modules/file-formats/linux/make-file-formats.sh"
PROCESSES_SCRIPT="${PCLSRCDIR}/modules/processes/linux/make-processes.sh"

set -e

# build PCL
cd $PCL_MAKE_PATH
make -j$(nproc)

# build 3rd party libs
source "$LIB_3RDPARTY_SCRIPT"

# build file formats
source "$FILE_FORMAT_SCRIPT"

# build processes
source "$PROCESSES_SCRIPT"
