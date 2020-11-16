#!/bin/bash

BASEDIR=$PCLSRCDIR/modules/file-formats

cd $BASEDIR/BMP/linux/g++
make -j$(nproc)

cd $BASEDIR/RAW/linux/g++
make -j$(nproc)

cd $BASEDIR/FITS/linux/g++
make -j$(nproc)

cd $BASEDIR/JPEG/linux/g++
make -j$(nproc)

cd $BASEDIR/JPEG2000/linux/g++
make -j$(nproc)

cd $BASEDIR/TIFF/linux/g++
make -j$(nproc)

cd $BASEDIR/XISF/linux/g++
make -j$(nproc)

cd $BASEDIR
