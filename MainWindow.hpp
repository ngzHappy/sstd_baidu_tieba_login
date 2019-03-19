#pragma once

#include <sstd_qt_qml_quick_library.hpp>

class MainWindowPrivate;
class MainWindow : public QWidget {
    Q_OBJECT
private:
    MainWindowPrivate * const thisPrivate;
public:
    MainWindow();
    ~MainWindow();
private:
    sstd_class(MainWindow);
};







