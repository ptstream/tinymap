export PYTHON=/bin/python
export EMSC=~/emsdk/upstream/emscripten/emrun.py 
export HTML=~/Documents/Development/build-tinymap-Qt_5_15_2_WebAssembly-Release/sample/sample.html

$PYTHON $EMSC --browser firefox --port 30002 --no_emrun_detect --serve_after_close $HTML
