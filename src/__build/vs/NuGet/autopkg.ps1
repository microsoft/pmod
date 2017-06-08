param (
   [string]$NugetVersion
)
Write-NuGetPackage pmodsdk.autopkg  -defines:NUGET_VERSION=$NugetVersion
Write-NuGetPackage pmod_libsdk.autopkg -defines:NUGET_VERSION=$NugetVersion
New-Item -Force -ItemType directory -Path ..\..\..\..\bin\Product\nuget\coapp
move-item *.nupkg ..\..\..\..\bin\Product\nuget\coapp -force
