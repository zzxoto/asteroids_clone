@echo off

IF "%VCINSTALLDIR%"=="" (
  pushd "C:\Program Files (x86)\Microsoft Visual Studio 17.0\VC"
  call vcvarsall.bat x64
  popd
) ELSE (
  REM skip me
)

if not exist build mkdir build

set commonCompilerOpts=-Zi -nologo -FC -EHa

set includeRoot=%cd%\shared\include
set includes=-I%includeRoot%\

set code=%cd%\platform_win32.cpp %cd%\game.cpp %cd%\gamestate_welcome_screen.cpp %cd%\gamestate_playing.cpp %cd%\gamestate_end.cpp %cd%\renderer.cpp %cd%\player.cpp %cd%\asteroid.cpp %cd%\bitmap_font.cpp

pushd build
cl %code% %commonCompilerOpts% -Femain user32.lib gdi32.lib Winmm.lib
popd