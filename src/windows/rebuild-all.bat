cd %PCLSRCDIR%\pcl\windows\vc16
msbuild PCL.vcxproj /t:Clean,Build -m /p:Configuration=Release /p:Platform=x64

cd %PCLSRCDIR%\3rdparty\windows
call rebuild-3rdparty.bat

cd %PCLSRCDIR%\modules\file-formats\windows
call rebuild-file-formats.bat

cd %PCLSRCDIR%\modules\processes\windows
call rebuild-processes.bat

cd %PCLSRCDIR%
