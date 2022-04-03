#!/bin/bash
echo "Set environment:"
export PCLSRCDIR=$CI_PROJECT_DIR/src
export PCLINCDIR=$CI_PROJECT_DIR/include
export PCLLIBDIR64=$CI_BUILDS_DIR/lib
export PCLBINDIR64=$CI_BUILDS_DIR/bin

echo "Create build directories:"
mkdir $PCLLIBDIR64 && mkdir $PCLBINDIR64

echo "Install build dependencies:"


echo "Download latest Indigo source tree:"
#cd $CI_BUILDS_DIR && git clone https://github.com/indigo-astronomy/indigo.git


echo "Building pcl library"
cd $PCLSRCDIR/pcl/macosx/g++ && make -j 8  all 
echo "Building RFC6234 library"
cd $PCLSRCDIR/3rdparty/RFC6234/macosx/g++ && make -j 8 all 
echo "Building lz4 library"
cd $PCLSRCDIR/3rdparty/lz4/macosx/g++ && make -j 8 all 
echo "Building cminpack library"
cd $PCLSRCDIR/3rdparty/cminpack/macosx/g++ && make -j 8 all 
echo "Building zlib library"
cd $PCLSRCDIR/3rdparty/zlib/macosx/g++ && make -j 8 all 
echo "Building lcms library"
cd $PCLSRCDIR/3rdparty/lcms/macosx/g++ && make -j 8 all 
echo "Building indigo" 
# tools/cltools.sh # install autocon tools
cd $INDIGODIR && make  -C indigo_libs all
echo "Building PixInsight Indigo Client"
cd $PCLSRCDIR/modules/processes/contrib/kkretzschmar/INDIClient/macosx/g++ && make -j 8   all
