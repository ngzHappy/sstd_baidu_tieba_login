
#include <sstd_botan.hpp>
#include <sstd_qt_qml_quick_library.hpp>

#include "MainWindow.hpp"

int main(int argc, char ** argv) {

    QApplication varApplication{ argc , argv };

    MainWindow varWindow;
    varWindow.show();

    return varApplication.exec();

}



