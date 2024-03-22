cd %PCLSRCDIR%\3rdparty\cminpack\windows\vc17
msbuild cminpack.vcxproj /t:Build -m /p:Configuration=Release /p:Platform=x64

cd %PCLSRCDIR%\3rdparty\lcms\windows\vc17
msbuild lcms.vcxproj /t:Build -m /p:Configuration=Release /p:Platform=x64

cd %PCLSRCDIR%\3rdparty\lz4\windows\vc17
msbuild lz4.vcxproj /t:Build -m /p:Configuration=Release /p:Platform=x64

cd %PCLSRCDIR%\3rdparty\RFC6234\windows\vc17
msbuild RFC6234.vcxproj /t:Build -m /p:Configuration=Release /p:Platform=x64

cd %PCLSRCDIR%\3rdparty\zlib\windows\vc17
msbuild zlib.vcxproj /t:Build -m /p:Configuration=Release /p:Platform=x64

cd %PCLSRCDIR%\3rdparty\zstd\windows\vc17
msbuild zstd.vcxproj /t:Build -m /p:Configuration=Release /p:Platform=x64

cd %PCLSRCDIR%
