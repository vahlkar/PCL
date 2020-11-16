#!/bin/bash

BASEDIR=$PCLSRCDIR/3rdparty

cd $BASEDIR/cfitsio/linux/g++
make clean
make -j$(nproc)

cd $BASEDIR/cminpack/linux/g++
make clean
make -j$(nproc)

cd $BASEDIR/jasper/linux/g++
make clean
make -j$(nproc)

cd $BASEDIR/jpeg/linux/g++
make clean
make -j$(nproc)

cd $BASEDIR/lcms/linux/g++
make clean
make -j$(nproc)

cd $BASEDIR/libraw/linux/g++
make clean
make -j$(nproc)

cd $BASEDIR/libraw-compat/linux/g++
make clean
make -j$(nproc)

cd $BASEDIR/libtiff/linux/g++
make clean
make -j$(nproc)

cd $BASEDIR/lz4/linux/g++
make clean
make -j$(nproc)

cd $BASEDIR/RFC6234/linux/g++
make clean
make -j$(nproc)

cd $BASEDIR/zlib/linux/g++
make clean
make -j$(nproc)

cd $BASEDIR