#!/bin/bash

BASEDIR=$PCLSRCDIR/3rdparty
NPROC=$(nproc --ignore=4)

cd $BASEDIR/cfitsio/linux/g++
make -j $NPROC

cd $BASEDIR/cminpack/linux/g++
make -j $NPROC

cd $BASEDIR/jasper/linux/g++
make -j $NPROC

cd $BASEDIR/jpeg/linux/g++
make -j $NPROC

cd $BASEDIR/lcms/linux/g++
make -j $NPROC

# cd $BASEDIR/libraw/linux/g++
# make -j $NPROC

cd $BASEDIR/libtiff/linux/g++
make -j $NPROC

cd $BASEDIR/lz4/linux/g++
make -j $NPROC

cd $BASEDIR/RFC6234/linux/g++
make -j $NPROC

cd $BASEDIR/zlib/linux/g++
make -j $NPROC

cd $BASEDIR
