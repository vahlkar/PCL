#!/bin/bash

NPROC=$(sysctl -n hw.ncpu)

cd $PCLSRCDIR/pcl/macosx/g++
make clean
make -j $NPROC

cd $PCLSRCDIR/3rdparty/macosx
./rebuild-3rdparty.sh

cd $PCLSRCDIR/modules/file-formats/macosx
./rebuild-file-formats.sh

cd $PCLSRCDIR/modules/processes/macosx
./rebuild-processes.sh

cd $PCLSRCDIR
