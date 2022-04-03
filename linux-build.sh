#!/bin/bash
echo "Set environment:"
export PCLSRCDIR=$CI_PROJECT_DIR/src
export PCLINCDIR=$CI_PROJECT_DIR/include
export PCLLIBDIR64C=$CI_BUILDS_DIR/lib
export PCLBINDIR64C=$CI_BUILDS_DIR/bin
export INDIGODIR=$CI_BUILDS_DIR/indigo

echo "Create build directories:"
mkdir $PCLLIBDIR64 && mkdir $PCLBINDIR64

echo "Install build dependencies:"
#sudo apt-get update
#sudo apt-get --assume-yes install  gcc-9 g++-9 
#sudo apt-get --assume-yes install  build-essential autoconf autotools-dev libtool cmake libudev-dev libavahi-compat-libdnssd-dev libusb-1.0-0-dev libcurl4-gnutls-dev libgphoto2-dev libz-dev git curl
#echo "Install Cuda libraries"
#wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu1804/x86_64/cuda-ubuntu1804.pin
#sudo mv cuda-ubuntu1804.pin /etc/apt/preferences.d/cuda-repository-pin-600
#wget https://developer.download.nvidia.com/compute/cuda/11.2.0/local_installers/cuda-repo-ubuntu1804-11-2-local_11.2.0-460.27.04-1_amd64.deb
#sudo dpkg -i cuda-repo-ubuntu1804-11-2-local_11.2.0-460.27.04-1_amd64.deb
#sudo apt-key add /var/cuda-repo-ubuntu1804-11-2-local/7fa2af80.pub
#sudo apt-get update
#sudo apt-get -y install cuda


echo "Download latest Indigo source tree:"
#cd $CI_BUILDS_DIR && git clone https://github.com/indigo-astronomy/indigo.git


echo "Building pcl library"
cd $PCLSRCDIR/pcl/linux/g++c && make -j 8  all 
echo "Building RFC6234 library"
cd $PCLSRCDIR/3rdparty/RFC6234/linux/g++c && make -j 8 all 
echo "Building lz4 library"
cd $PCLSRCDIR/3rdparty/lz4/linux/g++c && make -j 8 all 
echo "Building cminpack library"
cd $PCLSRCDIR/3rdparty/cminpack/linux/g++c && make -j 8 all 
echo "Building zlib library"
cd $PCLSRCDIR/3rdparty/zlib/linux/g++c && make -j 8 all 
echo "Building lcms library"
cd $PCLSRCDIR/3rdparty/lcms/linux/g++c && make -j 8 all 
#echo "Building indigo" 
#cd $INDIGODIR && make -C indigo_libs all
echo "Building PixInsight Indigo Client"
cd $PCLSRCDIR/modules/processes/contrib/kkretzschmar/INDIClient/linux/g++c && make -j 8   all
