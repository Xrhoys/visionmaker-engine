@echo off
@set OUT_DIR=Build
@set SHADER_OUTPUT_DIR=Fxc
@set OUT_EXE=main
@set INCLUDES=/I imgui\
@set SOURCES=main.cpp imgui\imgui*.cpp
@set LIBS=User32.lib gdi32.lib d3d11.lib d3dcompiler.lib
@set FLAGS=-DDEBUG=1
IF NOT EXIST %OUT_DIR%\ MKDIR %OUT_DIR%
IF NOT EXIST %OUT_DIR%\%SHADER_OUTPUT_DIR%\ MKDIR %OUT_DIR%\%SHADER_OUTPUT_DIR%

cl /nologo /Zi %FLAGS% %INCLUDES% %SOURCES% /Fe%OUT_DIR%/%OUT_EXE%.exe /Fo%OUT_DIR%/ /link %LIBS%

@REM Shader buildig process
FOR /f %%f IN ('dir /b *.hlsl') DO (
	fxc /nologo /Zi /T vs_5_0 /E VS /Fo %OUT_DIR%/%SHADER_OUTPUT_DIR%/%%~nf_vs.fxc %%f
	fxc /nologo /Zi /T ps_5_0 /E PS /Fo %OUT_DIR%/%SHADER_OUTPUT_DIR%/%%~nf_ps.fxc %%f
)

@REM copy assets and fxc folders to
xcopy assets\ %OUT_DIR%\assets\ /E /Y