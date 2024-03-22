#!/bin/bash

BASEDIR=$PCLSRCDIR/3rdparty
NPROC=$(nproc --ignore=4)

cd $BASEDIR/cminpack/linux/g++
make clean
make -j $NPROC

cd $BASEDIR/lcms/linux/g++
make clean
make -j $NPROC

cd $BASEDIR/lz4/linux/g++
make clean
make -j $NPROC

cd $BASEDIR/RFC6234/linux/g++
make clean
make -j $NPROC

cd $BASEDIR/zlib/linux/g++
make clean
make -j $NPROC

cd $BASEDIR/zstd/linux/g++
make clean
make -j $NPROC

cd $BASEDIR
