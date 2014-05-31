@echo off
if exist UserLib\ goto :test2
if not exist ..\..\UserLib\ goto :usage
cd ..\..\
:test2
if not "%1" == "" goto :test3
if "%VS120COMNTOOLS%" == "" goto :test9
set P=v120
goto :test4
:test9
if "%VS110COMNTOOLS%" == "" goto :test8
set P=v110
goto :test4
:test8
if "%VS100COMNTOOLS%" == "" goto :usage
set P=v100
goto :test4
:test3
set P=%1
:test4
md "UserLib\vs\%P%\Debug"
md "UserLib\vs\%P%\Release"
if "%P%" == "v100" goto :v100
if "%P%" == "v110" goto :v110
if "%P%" == "v120" goto :v120
goto :usage
:v100
if "%VS100COMNTOOLS%" == "" goto :usage
if not exist "%VS100COMNTOOLS%" goto :usage
call "%VS100COMNTOOLS%..\..\VC\bin\vcvars32.bat"
if "%VSINSTALLDIR%"=="" goto :usage
goto :bzip2
:v110
if "%VS110COMNTOOLS%" == "" goto :usage
if not exist "%VS110COMNTOOLS%" goto :usage
call "%VS110COMNTOOLS%..\..\VC\bin\vcvars32.bat"
if "%VSINSTALLDIR%"=="" goto :usage
goto :bzip2
:v120
if "%VS120COMNTOOLS%" == "" goto :usage
if not exist "%VS120COMNTOOLS%" goto :usage
call "%VS120COMNTOOLS%..\..\VC\bin\vcvars32.bat"
if "%VSINSTALLDIR%"=="" goto :usage
:bzip2
set D=bzip2-1.0.6\
if not exist %D% goto :expat
pushd %D%
call :makedir bzip2.lib %1 %2
popd
:expat
set D=expat-2.1.0\lib\
if not exist %D% goto :sqlite
pushd %D%
call :makedir2 expat.lib %1 %2
popd
:sqlite
set D=sqlite-autoconf-3080403\
if not exist %D% goto :xz
pushd %D%
call :makedir sqlite3.lib %1 %2
popd
:xz
set D=xz-5.0.5\windows\
if not exist %D% goto :zlib
pushd %D%
call :makedir2 xz.lib %1 %2
popd
:zlib
set D=zlib-1.2.8\
if not exist %D% goto :EOF
pushd %D%
call :makedir3 zlib.lib %1 %2
popd
goto :EOF
:makedir
copy ..\UserLib\Scripts\%D%makefile-ok.msc .
if "%3" == "" goto :makedir0
nmake -f makefile-ok.msc clean
:makedir0
nmake -f makefile-ok.msc
if errorlevel 1 goto :EOF
copy "%1" "..\UserLib\vs\%2\Debug"
copy "%1" "..\UserLib\vs\%2\Release"
goto :EOF
:makedir2
copy ..\..\UserLib\Scripts\%D%makefile-ok.msc .
if "%3" == "" goto :makedir9
nmake -f makefile-ok.msc clean
:makedir9
nmake -f makefile-ok.msc
if errorlevel 1 goto :EOF
copy "%1" "..\..\UserLib\vs\%2\Debug"
copy "%1" "..\..\UserLib\vs\%2\Release"
goto :EOF
:makedir3
copy ..\UserLib\Scripts\%D%makefile-ok.msc .
if "%3" == "" goto :makedir8
nmake -f makefile-ok.msc clean
:makedir8
nmake -f makefile-ok.msc LOC="-DASMV -DASMINF" OBJA="inffas32.obj match686.obj"
if errorlevel 1 goto :EOF
copy "%1" "..\UserLib\vs\%2\Debug"
copy "%1" "..\UserLib\vs\%2\Release"
goto :EOF
:usage
echo dowin32 'version' [clean]
echo version is eg v110 for Visual Studio 2012.
echo run this script from the parent of UserLib
echo with all the packages parallel to UserLib.
