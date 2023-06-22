cd %PCLSRCDIR%\3rdparty\cfitsio\windows\vc17
msbuild cfitsio.vcxproj /t:Clean,Build -m /p:Configuration=Release /p:Platform=x64

cd %PCLSRCDIR%\3rdparty\cminpack\windows\vc17
msbuild cminpack.vcxproj /t:Clean,Build -m /p:Configuration=Release /p:Platform=x64

cd %PCLSRCDIR%\3rdparty\jasper\windows\vc17
msbuild jasper.vcxproj /t:Clean,Build -m /p:Configuration=Release /p:Platform=x64

cd %PCLSRCDIR%\3rdparty\jpeg\windows\vc17
msbuild jpeg.vcxproj /t:Clean,Build -m /p:Configuration=Release /p:Platform=x64

cd %PCLSRCDIR%\3rdparty\lcms\windows\vc17
msbuild lcms.vcxproj /t:Clean,Build -m /p:Configuration=Release /p:Platform=x64

cd %PCLSRCDIR%\3rdparty\libtiff\windows\vc17
msbuild libtiff.vcxproj /t:Clean,Build -m /p:Configuration=Release /p:Platform=x64

cd %PCLSRCDIR%\3rdparty\lz4\windows\vc17
msbuild lz4.vcxproj /t:Clean,Build -m /p:Configuration=Release /p:Platform=x64

cd %PCLSRCDIR%\3rdparty\RFC6234\windows\vc17
msbuild RFC6234.vcxproj /t:Clean,Build -m /p:Configuration=Release /p:Platform=x64

cd %PCLSRCDIR%\3rdparty\zlib\windows\vc17
msbuild zlib.vcxproj /t:Clean,Build -m /p:Configuration=Release /p:Platform=x64

cd %PCLSRCDIR%
