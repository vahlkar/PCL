#!/bin/bash

BASEDIR=$PCLSRCDIR/modules/processes
NPROC=$(nproc --ignore=4)

cd $BASEDIR/APASS/linux/g++
make -j $NPROC

cd $BASEDIR/CloneStamp/linux/g++
make -j $NPROC

cd $BASEDIR/ColorCalibration/linux/g++
make -j $NPROC

cd $BASEDIR/ColorManagement/linux/g++
make -j $NPROC

cd $BASEDIR/ColorSpaces/linux/g++
make -j $NPROC

cd $BASEDIR/Convolution/linux/g++
make -j $NPROC

cd $BASEDIR/EphemerisGeneration/linux/g++
make -j $NPROC

cd $BASEDIR/FindingChart/linux/g++
make -j $NPROC

cd $BASEDIR/Flux/linux/g++
make -j $NPROC

cd $BASEDIR/Fourier/linux/g++
make -j $NPROC

cd $BASEDIR/Gaia/linux/g++
make -j $NPROC

cd $BASEDIR/Geometry/linux/g++
make -j $NPROC

cd $BASEDIR/Global/linux/g++
make -j $NPROC

cd $BASEDIR/GREYCstoration/linux/g++
make -j $NPROC

cd $BASEDIR/Image/linux/g++
make -j $NPROC

cd $BASEDIR/ImageCalibration/linux/g++
make -j $NPROC

cd $BASEDIR/ImageIntegration/linux/g++
make -j $NPROC

cd $BASEDIR/IntensityTransformations/linux/g++
make -j $NPROC

cd $BASEDIR/Morphology/linux/g++
make -j $NPROC

cd $BASEDIR/NetworkService/linux/g++
make -j $NPROC

cd $BASEDIR/NoiseGeneration/linux/g++
make -j $NPROC

cd $BASEDIR/NoOperation/linux/g++
make -j $NPROC

cd $BASEDIR/PixelMath/linux/g++
make -j $NPROC

cd $BASEDIR/RestorationFilters/linux/g++
make -j $NPROC

cd $BASEDIR/Sandbox/linux/g++
make -j $NPROC

cd $BASEDIR/contrib/cleger/SubframeSelector/linux/g++
make -j $NPROC

cd $BASEDIR/contrib/gviehoever/GradientDomain/linux/g++
make -j $NPROC

cd $BASEDIR/contrib/kkretzschmar/INDIClient/linux/g++
make -j $NPROC

cd $BASEDIR/contrib/nvolkov/Blink/linux/g++
make -j $NPROC

cd $BASEDIR/contrib/nvolkov/CometAlignment/linux/g++
make -j $NPROC

cd $BASEDIR/contrib/nvolkov/CosmeticCorrection/linux/g++
make -j $NPROC

cd $BASEDIR/contrib/nvolkov/SplitCFA/linux/g++
make -j $NPROC

cd $BASEDIR/contrib/sharkmelley/ArcsinhStretch/linux/g++
make -j $NPROC

cd $BASEDIR/contrib/spool/Debayer/linux/g++
make -j $NPROC

cd $BASEDIR/contrib/zvrastil/Annotation/linux/g++
make -j $NPROC

cd $BASEDIR/contrib/zvrastil/AssistedColorCalibration/linux/g++
make -j $NPROC

cd $BASEDIR/contrib/zvrastil/LocalHistogramEqualization/linux/g++
make -j $NPROC

cd $BASEDIR
