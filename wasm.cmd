@echo off

rem Your Python installation.
set PYTHON=%LOCALAPPDATA%/Microsoft/WindowsApps/python.exe

rem Your emsdk installation.
set EMSC=D:/Qt/emsdk/upstream/emscripten/emrun.py

rem Your release build.
set HTML=%USERPROFILE%/Documents/Development/build-tinymap-Qt_5_15_2_WebAssembly-Release/sample/sample.html

rem Start sample under firefox.
%PYTHON% %EMSC% --browser firefox --port 30001 --no_emrun_detect --serve_after_close %HTML%