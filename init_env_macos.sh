#!/bin/bash
export PCLDIR=$HOME/work/PCL
export PCLSRCDIR=$PCLDIR/src
export PCLINCDIR=$PCLDIR/include
export PCLLIBDIR64=$PCLDIR/lib/macosx/x64
export PCLLIBDIR=$PCLLIBDIR64
export PCLBINDIR64=$PCLDIR/bin
export PCLBINDIR=$PCLBINDIR64

mkdir -p "$PCLDIR"
mkdir -p "$PCLSRCDIR"
mkdir -p "$PCLINCDIR"
mkdir -p "$PCLLIBDIR64"
mkdir -p "$PCLBINDIR64"
