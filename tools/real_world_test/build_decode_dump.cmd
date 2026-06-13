@echo off
setlocal
set "ROOT=%~dp0..\..\.."
call "C:\Program Files\Microsoft Visual Studio\18\Enterprise\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul
cl /nologo /std:c11 /W4 /WX /MDd /DASMKIT_ENABLE_TEXT=1 /I"%ROOT%\runtime\include" "%ROOT%\runtime\tools\real_world_test\asmkit_decode_dump.c" "%ROOT%\build-asmkit-runtime-text\Debug\asmkit.lib" /Fe:"%ROOT%\build-asmkit-runtime-text\Debug\asmkit_decode_dump.exe" /link /WX
