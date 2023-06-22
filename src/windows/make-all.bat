cd %PCLSRCDIR%\pcl\windows\vc17
msbuild PCL.vcxproj /t:Build -m /p:Configuration=Release /p:Platform=x64

cd %PCLSRCDIR%\3rdparty\windows
call make-3rdparty.bat

cd %PCLSRCDIR%\modules\file-formats\windows
call make-file-formats.bat

cd %PCLSRCDIR%\modules\processes\windows
call make-processes.bat

cd %PCLSRCDIR%
