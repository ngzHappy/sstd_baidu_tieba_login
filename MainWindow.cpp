#include "MainWindow.hpp"

namespace _theMainWindowFile{
    class LineItem{
    public:
        QLabel * label;
        QLineEdit * text;
        inline LineItem();
        inline LineItem & construct(QBoxLayout *);
    private:
        sstd_class(LineItem);
    };
}/*_theMainWindowFile*/

class MainWindowPrivate{
public:
    using LineItem = _theMainWindowFile::LineItem;
    LineItem userName;
    LineItem passWord;
private:
    sstd_class(MainWindowPrivate);
};

MainWindow::MainWindow() :
    thisPrivate{ sstd_new<MainWindowPrivate>() } {
    this->setMinimumWidth( 512 );
    this->setMinimumHeight( 128 );

    auto varLayout =
        sstd_make_virtual_unique<QVBoxLayout>();

    {
        auto & var =
        thisPrivate->userName.construct(varLayout.get() );
        var.label->setText( trUtf8(u8R"(用户名)") );
    }

    {
        auto & var =
        thisPrivate->passWord.construct(varLayout.get() );
        var.label->setText( trUtf8(u8R"(密码)") );
    }

    this->setLayout(varLayout.release());

}

MainWindow::~MainWindow(){
    delete thisPrivate;
}


namespace _theMainWindowFile{

    inline LineItem::LineItem(){
    }

    inline LineItem & LineItem::construct(QBoxLayout * arg){
        auto varLineLayout =
            sstd_virtual_new< QHBoxLayout >();
        arg->addLayout(varLineLayout);
        auto varLabel =
                sstd_virtual_new<QLabel>();
        varLineLayout->addWidget(varLabel);
        auto varText =
                sstd_virtual_new<QLineEdit>();
        varLineLayout->addWidget(varText);

        label = varLabel;
        text = varText;

        return *this;
    }

}/*_theMainWindowFile*/

