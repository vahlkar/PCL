#!/bin/bash

BASEDIR=$PCLSRCDIR/modules/processes
NPROC=$(sysctl -n hw.ncpu)

cd $BASEDIR/APASS/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/CloneStamp/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/ColorCalibration/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/ColorManagement/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/ColorSpaces/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/Convolution/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/EphemerisGeneration/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/FilterManager/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/FindingChart/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/Flux/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/Fourier/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/Gaia/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/Geometry/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/Global/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/GREYCstoration/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/Image/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/ImageCalibration/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/ImageIntegration/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/IntensityTransformations/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/Morphology/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/NetworkService/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/NoiseGeneration/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/NoOperation/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/PixelMath/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/RestorationFilters/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/Sandbox/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/contrib/cleger/SubframeSelector/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/contrib/gviehoever/GradientDomain/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/contrib/kkretzschmar/INDIClient/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/contrib/nvolkov/Blink/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/contrib/nvolkov/CometAlignment/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/contrib/nvolkov/CosmeticCorrection/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/contrib/nvolkov/SplitCFA/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/contrib/sharkmelley/ArcsinhStretch/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/contrib/spool/Debayer/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/contrib/zvrastil/Annotation/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/contrib/zvrastil/AssistedColorCalibration/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR/contrib/zvrastil/LocalHistogramEqualization/macosx/g++
make clean
make -j $NPROC

cd $BASEDIR
