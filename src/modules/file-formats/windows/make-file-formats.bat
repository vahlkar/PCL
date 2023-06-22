cd %PCLSRCDIR%\modules\file-formats\BMP\windows\vc17
msbuild BMP.vcxproj /t:Build -m /p:Configuration=Release /p:Platform=x64

cd %PCLSRCDIR%\modules\file-formats\RAW\windows\vc17
msbuild RAW.vcxproj /t:Build -m /p:Configuration=Release /p:Platform=x64

cd %PCLSRCDIR%\modules\file-formats\FITS\windows\vc17
msbuild FITS.vcxproj /t:Build -m /p:Configuration=Release /p:Platform=x64

cd %PCLSRCDIR%\modules\file-formats\JPEG\windows\vc17
msbuild JPEG.vcxproj /t:Build -m /p:Configuration=Release /p:Platform=x64

cd %PCLSRCDIR%\modules\file-formats\JPEG2000\windows\vc17
msbuild JPEG2000.vcxproj /t:Build -m /p:Configuration=Release /p:Platform=x64

cd %PCLSRCDIR%\modules\file-formats\TIFF\windows\vc17
msbuild TIFF.vcxproj /t:Build -m /p:Configuration=Release /p:Platform=x64

cd %PCLSRCDIR%\modules\file-formats\XISF\windows\vc17
msbuild XISF.vcxproj /t:Build -m /p:Configuration=Release /p:Platform=x64

cd %PCLSRCDIR%
