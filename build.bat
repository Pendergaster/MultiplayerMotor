@echo off
cls
SETLOCAL

if not defined DEV_ENV (
		CALL "%VS140COMNTOOLS%/../../VC/vcvarsall.bat" x64
		)
set DEV_ENV=???

set includes=-I"../../external"
set libs=glfw3.lib opengl32.lib BulletCollision.lib BulletDynamics.lib BulletSoftBody.lib LinearMath.lib Raknet.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  Shell32.lib

SET CLAGS_DEBUG= -Od -W4 -wd4201 -wd4505
SET CLAGS_RELEASE= -O2 
SET BUILD_DIR=bin

cls
pushd %BUILD_DIR%\client\
REM 
cl %CLAGS_DEBUG% -Z7 -nologo /EHsc /fp:fast ..\..\src\client\main.cpp %includes% -D_ITERATOR_DEBUG_LEVEL#2 /MDd /link /ignore:4099 %libs% -LIBPATH:../../libs/debuglibs /out:"MPclient.exe"
popd
exit
cls
pushd %BUILD_DIR%\server\
REM 
cl %CLAGS_DEBUG% -Z7 -nologo /EHsc /fp:fast ..\..\src\server\main.cpp %includes% -D_ITERATOR_DEBUG_LEVEL#2 /MDd /link %libs% -LIBPATH:../../libs/debuglibs /out:"MPserver.exe"
popd
		
		
IF /I "%1"=="run_client" (
		cls
		bin\client\MPclient.exe
		popd
		)
		
IF /I "%1"=="run_server" (
		cls
		bin\server\MPserver.exe
		popd
		)

ENDLOCAL
