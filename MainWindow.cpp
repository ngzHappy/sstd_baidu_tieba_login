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

#define error_goto(...)   varThisData->isErrorYield = true ; goto __VA_ARGS__
#define error_yield(...)  __VA_ARGS__ : if(varThisData->isErrorYield){ \
    varThisData->isErrorYield=false; \
    this->finished( thisData.ans ); \
    sstd_function_outer_yield() ; } \
    static_assert (true )

        auto varLoginAns = thisData.ans/*当前堆栈获得数据所有权*/;
        auto varThisData = &thisData;
        auto varNetworkAccessManager = varLoginAns->networkAccessManager.get();

        error_yield(just_start_label);

        /*访问百度贴吧,检查网络，并获得一些cookies*/
        {
            varLoginAns->hasError=false;
            QNetworkRequest varRequest{ QStringLiteral(R"(https://tieba.baidu.com/index.html)") };
            auto varReply = varNetworkAccessManager->get( varRequest );
            varReply->connect( varReply , &QNetworkReply::finished,
                             bindFunctionWithThis(  [varReply, varThisData](){
                varReply->deleteLater();
                if( varReply->error() != QNetworkReply::NoError ){
                    varThisData->ans->hasError = true;
                    varThisData->ans->ErrorString = varReply->errorString();
                }
            } ));
            sstd_function_inner_yield();
        }
        if( varLoginAns->hasError ){
            error_goto(just_start_label);
        }



        /*登录完成*/
        this->finished( thisData.ans );

#undef error_yield
#undef error_goto
    }



}/*_theMainWindowFile*/

