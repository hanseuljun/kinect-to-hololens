Install-Module VSSetup -Scope CurrentUser

$vsPath = (Get-VSSetupInstance).InstallationPath + "\MSBuild"
$msBuilds = Get-ChildItem $vsPath -recurse -filter "MSBuild.exe"
$msBuild = $msBuilds[0].FullName

$configuration = "RelWithDebInfo"
$buildPath = (Get-Location).path + "\cpp\build"

&$msBuild ("/t:KinectToHololensPlugin", "/p:Configuration=$configuration", "/p:Platform=Win32", "$buildPath\x86\KinectToHololens.sln")
&$msBuild ("/t:KinectToHololensPlugin", "/p:Configuration=$configuration", "/p:Platform=x64", "$buildPath\x64\KinectToHololens.sln")

$assetsPath = (Get-Location).path + "\unity\KinecttoHololens\Assets"

Copy-Item "$buildPath\x86\plugin\$configuration\KinectToHololensPlugin.dll" -Destination "$assetsPath\Plugins\WSA"
Copy-Item "$buildPath\x64\plugin\$configuration\KinectToHololensPlugin.dll" -Destination "$assetsPath\Editor"

$binPath = (Get-Location).path + "\bin"
Copy-Item "$binPath\msvcp140.dll" -Destination "$assetsPath\Plugins\WSA"
Copy-Item "$binPath\msvcp140d.dll" -Destination "$assetsPath\Plugins\WSA"
Copy-Item "$binPath\vcruntime140.dll" -Destination "$assetsPath\Plugins\WSA"
Copy-Item "$binPath\vcruntime140d.dll" -Destination "$assetsPath\Plugins\WSA"

$vcpkgPath = (Get-Location).path + "\vcpkg"
$ffmpegX86Path = "$vcpkgPath\packages\ffmpeg_x86-windows\bin"
$ffmpegX64Path = "$vcpkgPath\packages\ffmpeg_x64-windows\bin"

Copy-Item "$ffmpegX86Path\avcodec-58.dll" -Destination "$assetsPath\Plugins\WSA"
Copy-Item "$ffmpegX86Path\avutil-56.dll" -Destination "$assetsPath\Plugins\WSA"
Copy-Item "$ffmpegX86Path\swresample-3.dll" -Destination "$assetsPath\Plugins\WSA"

Copy-Item "$ffmpegX64Path\avcodec-58.dll" -Destination "$assetsPath\Editor"
Copy-Item "$ffmpegX64Path\avutil-56.dll" -Destination "$assetsPath\Editor"
Copy-Item "$ffmpegX64Path\swresample-3.dll" -Destination "$assetsPath\Editor"

Pause