
TEMPLATE = app

include($$PWD/../sstd_library/sstd_library.pri)
include($$PWD/../sstd_qt_qml_quick_library/sstd_qt_qml_quick_library.pri)

SOURCES += $$PWD/main.cpp \
    $$PWD/MainWindow.cpp

HEADERS += \
    $$PWD/MainWindow.hpp




