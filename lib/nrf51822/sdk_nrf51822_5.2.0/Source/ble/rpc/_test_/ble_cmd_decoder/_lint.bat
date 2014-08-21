@echo off
rem "chdir %~dp0" changes directory to the directory the script is placed in.
rem Even when running as administrator.
chdir %~dp0
msbuild foo.msbuild /t:Lint
pause
