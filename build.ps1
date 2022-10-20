cd $PSScriptRoot

# Build Wizard-2020
cd src
cmd /c "C:\BuildTools\Common7\Tools\VsDevCmd.bat && msbuild Wizard-2020.sln /m /t:Rebuild /p:Configuration=Release"
