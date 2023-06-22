#!/bin/bash

BASEDIR=$PCLSRCDIR/modules/file-formats
NPROC=$(nproc --ignore=4)

cd $BASEDIR/BMP/linux/g++
make clean
make -j $NPROC

cd $BASEDIR/RAW/linux/g++
make clean
make -j $NPROC

cd $BASEDIR/FITS/linux/g++
make clean
make -j $NPROC

cd $BASEDIR/JPEG/linux/g++
make clean
make -j $NPROC

cd $BASEDIR/JPEG2000/linux/g++
make clean
make -j $NPROC

cd $BASEDIR/TIFF/linux/g++
make clean
make -j $NPROC

cd $BASEDIR/XISF/linux/g++
make clean
make -j $NPROC

cd $BASEDIR
