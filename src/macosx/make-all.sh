#!/bin/bash

NPROC=$(sysctl -n hw.ncpu)

cd $PCLSRCDIR/pcl/macosx/g++
make -j $NPROC

cd $PCLSRCDIR/3rdparty/macosx
./make-3rdparty.sh

cd $PCLSRCDIR/modules/file-formats/macosx
./make-file-formats.sh

cd $PCLSRCDIR/modules/processes/macosx
./make-processes.sh

cd $PCLSRCDIR
