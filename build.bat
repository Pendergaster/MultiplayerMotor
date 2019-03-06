@echo off

SETLOCAL
REM ox- nopee optimointi, 0t nopeutta , oB2 inlineany, Oi instribdsdsds
REM Od ei optimisaatioita
REM -OB2
REM -Ox -Ot  -Oi -W4 -wd4201
REM nologo ei turhaa printtiä / /MD common runtime multithreaded   /   /link alottaa linkkaamisen / 
REM -LD -> buildaa .dll -MD jälkee
REM -LD -> buildaa .dll -MD jälkee
if not defined DEV_ENV (
		CALL "%VS140COMNTOOLS%/../../VC/vcvarsall.bat" x64
		)
set DEV_ENV=???

set includes=-I"../external"
set libs=glfw3.lib opengl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  Shell32.lib
SET CLAGS= -Od -W4 -wd4201 -wd4505
SET BUILD_DIR=bin

IF /I "%1"=="build_debug" (
		cls
		pushd %BUILD_DIR%
		REM 
		cl %CLAGS% -Z7 -nologo /EHsc /DEBUG /fp:fast ..\src\main.cpp   %includes% /MD /link %libs% -LIBPATH:../libs/debuglibs 
		popd
		)

IF /I "%1"=="build_release" (
		cls
		pushd %BUILD_DIR%
		REM 
		cl %CLAGS% -Z7 -nologo /EHsc /DEBUG /fp:fast ..\src\main.cpp   %includes% /MD /link %libs% -LIBPATH:../libs/releaselibs 
		popd
		)

IF /I "%1"=="run" (
		cls
		chdir %~dp0
		bin\main.exe
		popd
		)

REM TestBin\test.exe
IF /I "%1"=="run_test" (
		REM chdir %~dp0
		REM pushd TestBin
		echo "Running test"
		TestBin\test.exe
		REM popd
		)

ENDLOCAL
