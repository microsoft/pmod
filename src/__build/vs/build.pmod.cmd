if /I "%1"=="clean" (
echo "Clean Build"
    set target=Clean;Build
    rmdir /S /Q ..\..\..\bin
) ELSE (
    set target=Build
)

msbuild version.proj

set targets=x86;x64;ARM;Xamarin.Droid

for %%t in (%targets%) do (
    echo "Building target:"%%i

    msbuild pmod.sln /t:%target% /p:Configuration=Debug /p:Platform=%%t /v:m
    msbuild pmod.sln /t:%target% /p:Configuration=Release /p:Platform=%%t /v:m
)
