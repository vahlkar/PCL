$env:PCLDIR = "$env:USERPROFILE\PCL"
$env:PCLBINDIR64 = "$env:PCLDIR\bin"
$env:PCLBINDIR = "$env:PCLBINDIR64"
$env:PCLLIBDIR64 = "$env:PCLDIR\lib\windows\x64"
$env:PCLLIBDIR = "$env:PCLLIBDIR64"
$env:PCLINCDIR = "$env:PCLDIR\include"
$env:PCLSRCDIR = "$env:PCLDIR\src"
 & "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe" $env:PCLSRCDIR\modules\processes\contrib\nvolkov\Blink\windows\vc17\Blink.vcxproj
