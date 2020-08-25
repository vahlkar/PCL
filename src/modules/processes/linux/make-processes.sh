#!/bin/bash

BASEDIR=$PCLSRCDIR/modules/processes

# cd $BASEDIR/BackgroundModelization/host/g++
# make -j

cd $BASEDIR/CloneStamp/host/g++
make -j

cd $BASEDIR/ColorCalibration/host/g++
make -j

cd $BASEDIR/ColorManagement/host/g++
make -j

cd $BASEDIR/ColorSpaces/host/g++
make -j

# cd $BASEDIR/Compatibility/host/g++
# make -j

cd $BASEDIR/Convolution/host/g++
make -j

# cd $BASEDIR/Deconvolution/host/g++
# make -j

cd $BASEDIR/Flux/host/g++
make -j

cd $BASEDIR/Fourier/host/g++
make -j

cd $BASEDIR/Geometry/host/g++
make -j

cd $BASEDIR/Global/host/g++
make -j

cd $BASEDIR/GREYCstoration/host/g++
make -j

cd $BASEDIR/Image/host/g++
make -j

cd $BASEDIR/ImageCalibration/host/g++
make -j

cd $BASEDIR/ImageIntegration/host/g++
make -j

# cd $BASEDIR/ImageRegistration/host/g++
# make -j

cd $BASEDIR/IntensityTransformations/host/g++
make -j

# cd $BASEDIR/MaskGeneration/host/g++
# make -j

cd $BASEDIR/Morphology/host/g++
make -j

# cd $BASEDIR/MultiscaleProcessing/host/g++
# make -j

cd $BASEDIR/NoiseGeneration/host/g++
make -j

# cd $BASEDIR/NoiseReduction/host/g++
# make -j

cd $BASEDIR/NoOperation/host/g++
make -j

# cd $BASEDIR/Obsolete/host/g++
# make -j

cd $BASEDIR/PixelMath/host/g++
make -j

cd $BASEDIR/RestorationFilters/host/g++
make -j

cd $BASEDIR/Sandbox/host/g++
make -j

cd $BASEDIR/StarGenerator/host/g++
make -j

# cd $BASEDIR/TGV/host/g++
# make -j

cd $BASEDIR/contrib/cleger/SubframeSelector/host/g++
make -j

cd $BASEDIR/contrib/gviehoever/GradientDomain/host/g++
make -j

cd $BASEDIR/contrib/kkretzschmar/INDIClient/host/g++
make -j

cd $BASEDIR/contrib/nmisiura/StarNet/host/g++
make -j

cd $BASEDIR/contrib/nvolkov/Blink/host/g++
make -j

cd $BASEDIR/contrib/nvolkov/CometAlignment/host/g++
make -j

cd $BASEDIR/contrib/nvolkov/CosmeticCorrection/host/g++
make -j

cd $BASEDIR/contrib/nvolkov/SplitCFA/host/g++
make -j

cd $BASEDIR/contrib/sharkmelley/ArcsinhStretch/host/g++
make -j

cd $BASEDIR/contrib/spool/Debayer/host/g++
make -j

cd $BASEDIR/contrib/zvrastil/Annotation/host/g++
make -j

cd $BASEDIR/contrib/zvrastil/AssistedColorCalibration/host/g++
make -j

cd $BASEDIR/contrib/zvrastil/LocalHistogramEqualization/host/g++
make -j

cd $BASEDIR
