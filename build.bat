@ECHO OFF

set "IMAGE_NAME=ms_build"
set "BUILD_CMD=em++ --bind -o library.js Minesweeper.cpp -lembind --emit-tsd ms_interface.ts -o /tmp/build/ms_module.js"

for /f %%i in ('docker images -q %IMAGE_NAME%') do set RESULT=%%i
IF [%RESULT%] == [] (
  REM Image doesn't exist, build it first
  docker build -t %IMAGE_NAME% . 
) 

mkdir .\build > nul 2>&1
docker run --rm -v .:/tmp -w /tmp/src %IMAGE_NAME% %BUILD_CMD%