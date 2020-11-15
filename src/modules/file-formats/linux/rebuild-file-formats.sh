#!/bin/bash

BASEDIR=$PCLSRCDIR/modules/file-formats

cd $BASEDIR/BMP/$PCLHOST/g++
make clean
make -j$(nproc)

cd $BASEDIR/RAW/$PCLHOST/g++
make clean
make -j$(nproc)

cd $BASEDIR/FITS/$PCLHOST/g++
make clean
make -j$(nproc)

cd $BASEDIR/JPEG/$PCLHOST/g++
make clean
make -j$(nproc)

cd $BASEDIR/JPEG2000/$PCLHOST/g++
make clean
make -j$(nproc)

cd $BASEDIR/TIFF/$PCLHOST/g++
make clean
make -j$(nproc)

cd $BASEDIR/XISF/$PCLHOST/g++
make clean
make -j$(nproc)

cd $BASEDIR
