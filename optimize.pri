win32-g++: {
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3
}

win32-msvc: {
DEFINES += _USE_MATH_DEFINES
}

wasm: {
QMAKE_LFLAGS_RELEASE   -= -O2
QMAKE_LFLAGS_RELEASE   += -s TOTAL_MEMORY=400MB -O3
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3
}


