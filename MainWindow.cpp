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
        }thisData;
    public:
        template<typename T,typename U>
        inline LoginFunction(T && userName,U && passWord);
    protected:
        inline void doRun() override ;
        inline void doQuit() noexcept override ;
        inline void doException() noexcept override ;
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

    /*如果发生异常，这是逻辑设计错误或严重错误，根本就不应当发生异常*/
    inline void LoginFunction::doException() noexcept{
        auto varLoginAns = thisData.ans/*当前堆栈获得数据所有权*/;
        varLoginAns->hasError = true;
        sstd_try {
            std::rethrow_exception( std::current_exception() );
        } sstd_catch(const QString & arg){
            varLoginAns->ErrorString = QStringLiteral("exception:")+arg;
        } sstd_catch (...) {
            varLoginAns->ErrorString = QStringLiteral("exception:unknow");
        }
        this->finished( varLoginAns );
    }

    /*如果发生运行时已知错误将控制权转交给调用者*/
    inline void LoginFunction::errorYield(){
            this->finished( thisData.ans );
            this->outerYiled();
    }

    /*如果用户主动调用quit()*/
    void LoginFunction::doQuit() noexcept{
        auto varLoginAns = thisData.ans/*当前堆栈获得数据所有权*/;
        varLoginAns->hasError = true;
        varLoginAns->ErrorString = QStringLiteral("quit:but not finished");
        this->finished( thisData.ans );
    }

    inline void LoginFunction::doRun(){

#define error_goto(...) if( varLoginAns->hasError ) { \
    goto __VA_ARGS__; } static_assert (true)

        auto varLoginAns = thisData.ans/*当前堆栈获得数据所有权*/;
        auto varThisData = &thisData;
        auto varNetworkAccessManager = varLoginAns->networkAccessManager.get();

just_start_label:errorYield();
        /*访问百度贴吧,检查网络，并获得一些cookies*/
        {
            varThisData->ans->hasError = false;
            QNetworkRequest varRequest{ QStringLiteral(R"(https://tieba.baidu.com/index.html)") };
            auto varReply = varNetworkAccessManager->get( varRequest );
            varReply->connect( varReply , &QNetworkReply::finished,
                             bind( [varReply,varThisData](){
                varReply->deleteLater();
                if( varReply->error() != QNetworkReply::NoError ){
                    varThisData->ans->hasError=true;
                    varThisData->ans->ErrorString = QVariant( varReply->error() ).toString() ;
                } } ));
            this->innerYield();
        }
        error_goto(just_start_label);




        /*登录完成*/
        varLoginAns->hasError = false;
        varLoginAns->ErrorString.clear();
        this->finished( varLoginAns );

#undef error_goto
    }

}/*_theMainWindowFile*/

