
TEMPLATE = app

CONFIG(debug,debug|release){
    TARGET = sstd_baidu_tieba_login_debug
}else{
    TARGET = sstd_baidu_tieba_login
}

include($$PWD/../sstd_library/sstd_library.pri)
include($$PWD/../sstd_qt_qml_quick_library/sstd_qt_qml_quick_library.pri)

SOURCES += $$PWD/main.cpp \
    $$PWD/MainWindow.cpp \
    LoginFunctionBasic.cpp

HEADERS += \
    $$PWD/MainWindow.hpp \
    LoginFunctionBasic.hpp

DESTDIR = $${SSTD_LIBRARY_OUTPUT_PATH}


