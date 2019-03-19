#include "MainWindow.hpp"
#include <LoginFunctionBasic.hpp>

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

    class LoginFunction final : public LoginFunctionBasic {
    private:
        class ThisData{
        public:
            std::shared_ptr< LoginFunctionAns > ans;
            QString passWord;
            bool isErrorYield{false};
        }thisData;
    public:
        template<typename T,typename U>
        inline LoginFunction(T && userName,U && passWord);
    protected:
        inline void doRun() override ;
        inline void errorYield();
    private:
        sstd_class(LoginFunction);
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

    template<typename T,typename U>
    inline LoginFunction::LoginFunction(T && userName, U && passWord){
        thisData.ans = sstd_make_shared< LoginFunctionAns >();
        thisData.ans->userName = userName;
        thisData.passWord = std::forward<U>(passWord);
    }

    inline void LoginFunction::doRun(){
#define error_goto(...) varThisData->isErrorYield = true ; goto __VA_ARGS__

        auto varThis = this->shared_from_this()/*当前堆栈获得数据所有权*/;
        auto varLoginAns = thisData.ans/*当前堆栈获得数据所有权*/;
        auto varThisData = &thisData;
        auto varNetworkAccessManager = varLoginAns->networkAccessManager.get();

just_start_label:errorYield();

        /*访问百度贴吧,检查网络，并获得一些cookies*/
        {
            varLoginAns->hasError=false;
            QNetworkRequest varRequest{ QStringLiteral(R"(https://tieba.baidu.com/index.html)") };
            auto varReply = varNetworkAccessManager->get( varRequest );
            varReply->connect( varReply , &QNetworkReply::finished,
                               [varReply,this,varThis,varThisData](){
                varReply->deleteLater();
                if( varReply->error() != QNetworkReply::NoError ){
                    varThisData->ans->hasError = true;
                    varThisData->ans->ErrorString = varReply->errorString();
                }
                this->resume();
            } );
            this->yield();
        }
        if( varLoginAns->hasError ){
            error_goto(just_start_label);
        }



        /*登录完成*/
        this->finished( thisData.ans );

#undef error_goto
    }

    inline void LoginFunction::errorYield(){
        if(thisData.isErrorYield){
            thisData.isErrorYield = false;
            this->finished( thisData.ans );
            this->yield();
        }
    }

}/*_theMainWindowFile*/

