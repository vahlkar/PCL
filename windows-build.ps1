# doenst work, cannot access buildtools since VCINSTALLDIR is not defined
Write-Host "Check build tools installation"
Write-Host "$Env:ProgramFiles (x86)"
$buildToolsPath = cmd.exe /c "$Env:ProgramFiles (x86)\Microsoft Visual Studio\Installer\vswhere.exe" -products Microsoft.VisualStudio.Product.BuildTools -latest -property installationPath
Write-Host $buildToolsPath

Write-Host "Set environment"
$Env:PCLSRCDIR = "$Env:CI_PROJECT_DIR\src"
$Env:PCLINCDIR = "$Env:CI_PROJECT_DIR\include"
$Env:PCLLIBDIR64 = "$Env:CI_BUILDS_DIR\lib"
$Env:PCLBINDIR64 = "$Env:CI_BUILDS_DIR\bin"
$Env:INDIGODIR = "$Env:CI_BUILDS_DIR\indigo"
$Env:AdditionalIncludeDirectories="$Env:INDIGODIR\indigo_libs"

Write-Host "Create build directories"
cmd.exe /c "mkdir $Env:PCLLIBDIR64"
cmd.exe /c "mkdir $Env:PCLBINDIR64"

Write-HOST "Building pcl library"
cmd.exe /c "$buildToolsPath\MSBuild\Current\Bin\msbuild.exe" .\src\pcl\windows\vc15\pcl.vcxproj /p:PlatformToolset=v142 /p:configuration=release /p:platform=x64 
Write-HOST "Building RFC6234 library"
cmd.exe /c "$buildToolsPath\MSBuild\Current\Bin\msbuild.exe" .\src\3rdparty\RFC6234\windows\vc15\RFC6234.vcxproj /p:PlatformToolset=v142 /p:configuration=release /p:platform=x64 
Write-HOST "Building lz4 library"
cmd.exe /c "$buildToolsPath\MSBuild\Current\Bin\msbuild.exe" .\src\3rdparty\lz4\windows\vc15\lz4.vcxproj /p:PlatformToolset=v142 /p:configuration=release /p:platform=x64 
Write-HOST "Building cminpack library"
cmd.exe /c "$buildToolsPath\MSBuild\Current\Bin\msbuild.exe" .\src\3rdparty\cminpack\windows\vc15\cminpack.vcxproj /p:PlatformToolset=v142 /p:configuration=release /p:platform=x64 
Write-HOST "Building zlib library"
cmd.exe /c "$buildToolsPath\MSBuild\Current\Bin\msbuild.exe" .\src\3rdparty\zlib\windows\vc15\zlib.vcxproj /p:PlatformToolset=v142 /p:configuration=release /p:platform=x64 
Write-HOST "Building lcms library"
cmd.exe /c "$buildToolsPath\MSBuild\Current\Bin\msbuild.exe" .\src\3rdparty\lcms\windows\vc15\lcms.vcxproj /p:PlatformToolset=v142 /p:configuration=release /p:platform=x64 
Write-HOST "Building indigo library"
cmd.exe /c "$buildToolsPath\MSBuild\Current\Bin\msbuild.exe" $Env:INDIGODIR\indigo_windows\indigo_client\indigo_client.vcxproj   /p:PlatformToolset=v142 /p:WindowsTargetPlatformVersion=10.0  /p:configuration=release /p:platform=x64 

Write-HOST "Building PixInsight Indigo Client"
cmd.exe /c "$buildToolsPath\MSBuild\Current\Bin\msbuild.exe" .\src\modules\processes\contrib\kkretzschmar\INDIClient\windows\vc15\INDIClient.vcxproj /p:PlatformToolset=v142 /p:configuration=release /p:platform=x64 
