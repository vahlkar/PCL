#!/bin/bash

BASEDIR=$PCLSRCDIR/modules/file-formats

cd $BASEDIR/BMP/host/g++
make -j

cd $BASEDIR/RAW/host/g++
make -j

cd $BASEDIR/FITS/host/g++
make -j

cd $BASEDIR/JPEG/host/g++
make -j

cd $BASEDIR/JPEG2000/host/g++
make -j

cd $BASEDIR/TIFF/host/g++
make -j

cd $BASEDIR/XISF/host/g++
make -j

cd $BASEDIR
