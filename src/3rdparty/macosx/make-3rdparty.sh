#!/bin/bash

BASEDIR=$PCLSRCDIR/3rdparty
NPROC=$(sysctl -n hw.ncpu)

cd $BASEDIR/cfitsio/macosx/g++
make -j $NPROC

cd $BASEDIR/cminpack/macosx/g++
make -j $NPROC

cd $BASEDIR/jasper/macosx/g++
make -j $NPROC

cd $BASEDIR/jpeg/macosx/g++
make -j $NPROC

cd $BASEDIR/lcms/macosx/g++
make -j $NPROC

# cd $BASEDIR/libraw/macosx/g++
# make -j $NPROC

cd $BASEDIR/libtiff/macosx/g++
make -j $NPROC

cd $BASEDIR/lz4/macosx/g++
make -j $NPROC

cd $BASEDIR/RFC6234/macosx/g++
make -j $NPROC

cd $BASEDIR/zlib/macosx/g++
make -j $NPROC

cd $BASEDIR/zstd/macosx/g++
make -j $NPROC

cd $BASEDIR
