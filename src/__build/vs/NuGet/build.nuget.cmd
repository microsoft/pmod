set msbuildexe="C:\Program Files (x86)\MSBuild\14.0\Bin\msbuild.exe"

%msbuildexe% pmod.nuget.sln /p:Configuration=Debug
%msbuildexe% pmod.nuget.sln /p:Configuration=Release
%msbuildexe% coapp.proj
