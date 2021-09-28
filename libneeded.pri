win32-msvc: {
LIBFILE=$$join(LIBNAME,,,'.lib') # Assume VC++
} else {
LIBFILE=$$join(LIBNAME,,'lib','.a') # Assume Mingw-linux-macx
}

LIBDIR=$$OUT_PWD/../$$LIBNAME

win32:CONFIG(release, debug|release): LIBPATH = $$LIBDIR/release/$$LIBFILE
else:win32:CONFIG(debug, debug|release): LIBPATH = $$LIBDIR/debug/$$LIBFILE
else: LIBPATH = $$LIBDIR/$$LIBFILE

LIBS += $$LIBPATH
