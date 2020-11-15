#!/bin/bash

BASEDIR=$PCLSRCDIR/3rdparty

cd $BASEDIR/cfitsio/$PCLHOST/g++
make clean
make -j$(nproc)

cd $BASEDIR/cminpack/$PCLHOST/g++
make clean
make -j$(nproc)

cd $BASEDIR/jasper/$PCLHOST/g++
make clean
make -j$(nproc)

cd $BASEDIR/jpeg/$PCLHOST/g++
make clean
make -j$(nproc)

cd $BASEDIR/lcms/$PCLHOST/g++
make clean
make -j$(nproc)

cd $BASEDIR/libraw/$PCLHOST/g++
make clean
make -j$(nproc)

cd $BASEDIR/libraw-compat/$PCLHOST/g++
make clean
make -j$(nproc)

cd $BASEDIR/libtiff/$PCLHOST/g++
make clean
make -j$(nproc)

cd $BASEDIR/lz4/$PCLHOST/g++
make clean
make -j$(nproc)

cd $BASEDIR/RFC6234/$PCLHOST/g++
make clean
make -j$(nproc)

cd $BASEDIR/zlib/$PCLHOST/g++
make clean
make -j$(nproc)

cd $BASEDIR