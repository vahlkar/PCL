#!/bin/bash

BASEDIR=$PCLSRCDIR/modules/file-formats
NPROC=$(sysctl -n hw.ncpu)

cd $BASEDIR/BMP/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/RAW/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/FITS/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/JPEG/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/JPEG2000/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/TIFF/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/XISF/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR
