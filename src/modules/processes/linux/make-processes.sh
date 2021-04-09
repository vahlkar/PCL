#!/bin/bash

BASEDIR=$PCLSRCDIR/modules/processes

cd $BASEDIR/APASS/linux/g++
make -j$(nproc)

# cd $BASEDIR/BackgroundModelization/linux/g++
# make -j$(nproc)

cd $BASEDIR/CloneStamp/linux/g++
make -j$(nproc)

cd $BASEDIR/ColorCalibration/linux/g++
make -j$(nproc)

cd $BASEDIR/ColorManagement/linux/g++
make -j$(nproc)

cd $BASEDIR/ColorSpaces/linux/g++
make -j$(nproc)

# cd $BASEDIR/Compatibility/linux/g++
# make -j$(nproc)

cd $BASEDIR/Convolution/linux/g++
make -j$(nproc)

# cd $BASEDIR/Deconvolution/linux/g++
# make -j$(nproc)

cd $BASEDIR/EphemerisGeneration/linux/g++
make -j$(nproc)

cd $BASEDIR/Flux/linux/g++
make -j$(nproc)

cd $BASEDIR/Fourier/linux/g++
make -j$(nproc)

cd $BASEDIR/Gaia/linux/g++
make -j$(nproc)

cd $BASEDIR/Geometry/linux/g++
make -j$(nproc)

cd $BASEDIR/Global/linux/g++
make -j$(nproc)

cd $BASEDIR/GREYCstoration/linux/g++
make -j$(nproc)

cd $BASEDIR/Image/linux/g++
make -j$(nproc)

cd $BASEDIR/ImageCalibration/linux/g++
make -j$(nproc)

cd $BASEDIR/ImageIntegration/linux/g++
make -j$(nproc)

# cd $BASEDIR/ImageRegistration/linux/g++
# make -j$(nproc)

cd $BASEDIR/IntensityTransformations/linux/g++
make -j$(nproc)

# cd $BASEDIR/MaskGeneration/linux/g++
# make -j$(nproc)

cd $BASEDIR/Morphology/linux/g++
make -j$(nproc)

# cd $BASEDIR/MultiscaleProcessing/linux/g++
# make -j$(nproc)

cd $BASEDIR/NoiseGeneration/linux/g++
make -j$(nproc)

# cd $BASEDIR/NoiseReduction/linux/g++
# make -j$(nproc)

cd $BASEDIR/NoOperation/linux/g++
make -j$(nproc)

# cd $BASEDIR/Obsolete/linux/g++
# make -j$(nproc)

cd $BASEDIR/PixelMath/linux/g++
make -j$(nproc)

cd $BASEDIR/RestorationFilters/linux/g++
make -j$(nproc)

cd $BASEDIR/Sandbox/linux/g++
make -j$(nproc)

cd $BASEDIR/StarGenerator/linux/g++
make -j$(nproc)

# cd $BASEDIR/TGV/linux/g++
# make -j$(nproc)

cd $BASEDIR/contrib/cleger/SubframeSelector/linux/g++
make -j$(nproc)

cd $BASEDIR/contrib/gviehoever/GradientDomain/linux/g++
make -j$(nproc)

cd $BASEDIR/contrib/kkretzschmar/INDIClient/linux/g++
make -j$(nproc)

cd $BASEDIR/contrib/nmisiura/StarNet/linux/g++
make -j$(nproc)

cd $BASEDIR/contrib/nvolkov/Blink/linux/g++
make -j$(nproc)

cd $BASEDIR/contrib/nvolkov/CometAlignment/linux/g++
make -j$(nproc)

cd $BASEDIR/contrib/nvolkov/CosmeticCorrection/linux/g++
make -j$(nproc)

cd $BASEDIR/contrib/nvolkov/SplitCFA/linux/g++
make -j$(nproc)

cd $BASEDIR/contrib/sharkmelley/ArcsinhStretch/linux/g++
make -j$(nproc)

cd $BASEDIR/contrib/spool/Debayer/linux/g++
make -j$(nproc)

cd $BASEDIR/contrib/zvrastil/Annotation/linux/g++
make -j$(nproc)

cd $BASEDIR/contrib/zvrastil/AssistedColorCalibration/linux/g++
make -j$(nproc)

cd $BASEDIR/contrib/zvrastil/LocalHistogramEqualization/linux/g++
make -j$(nproc)

cd $BASEDIR
